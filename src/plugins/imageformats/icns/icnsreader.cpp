#include "icnsreader.h"

QDataStream &operator>>(QDataStream &in, IcnsBlockHeader &p)
{
    in >> p.OSType;
    in >> p.length;
    return in;
}

IcnsReader::IcnsReader(QIODevice *iodevice)
{
    Q_ASSERT(iodevice);
    m_stream.setDevice(iodevice);
    m_stream.setByteOrder(QDataStream::BigEndian);
    m_scanned = scanFile();
}

bool IcnsReader::decompressRLE24toR8G8B8(QByteArray &encodedBytes, quint32 expectedPixelCount)
{
    if(encodedBytes.isEmpty()) {
        qWarning("IcnsReader::decompressRLE24(): encoded bytes are empty!");
        return false;
    }
    quint32 rawDataSize = encodedBytes.size();
    quint32 destIconDataSize = expectedPixelCount * 3;
    QByteArray destIconBytes(destIconDataSize,0);   // Decompressed Raw Icon Data
    qDebug("IcnsReader::decompressRLE24(): Compressed RLE data size is %d",encodedBytes.size());
    qDebug("IcnsReader::decompressRLE24(): Decompressed will be %d bytes (%d pixels)",(int)destIconDataSize,(int)expectedPixelCount);
    qDebug("IcnsReader::decompressRLE24(): Decoding RLE data into RGB pixels...");
    quint32	dataOffset = 0;
    if(*((quint32*)encodedBytes.constData()) == 0x00000000) {
        qDebug("IcnsReader::decompressRLE24: 4 byte null padding found in rle data!");
        dataOffset = 4;
    }
    // Data is stored in red run, green run,blue run
    // So we decompress to pixel format RGB
    // RED:   byte[0], byte[4], byte[8]  ...
    // GREEN: byte[1], byte[5], byte[9]  ...
    // BLUE:  byte[2], byte[6], byte[10] ...
    quint8	colorOffset = 0;
    uchar   colorValue = 0;
    quint8	runLength = 0;
    quint32	pixelOffset = 0;
    for(colorOffset = 0; colorOffset < 3; colorOffset++) {
        pixelOffset = 0;
        while((pixelOffset < expectedPixelCount) && (dataOffset < rawDataSize)) {
            if((encodedBytes[dataOffset] & 0x80) == 0) {
                // Top bit is clear - run of various values to follow
                runLength = (0xFF & encodedBytes[dataOffset++]) + 1; // 1 <= len <= 128
                for(quint32	i = 0; (i < runLength) && (pixelOffset < expectedPixelCount) && (dataOffset < rawDataSize); i++) {
                    destIconBytes[(pixelOffset * 3) + colorOffset] = encodedBytes[dataOffset++];
                    pixelOffset++;
                }
            }
            else {
                // Top bit is set - run of one value to follow
                runLength = (0xFF & encodedBytes[dataOffset++]) - 125; // 3 <= len <= 130
                // Set the value to the color shifted to the correct bit offset
                colorValue = encodedBytes[dataOffset++];
                for(quint32	i = 0; (i < runLength) && (pixelOffset < expectedPixelCount); i++) {
                    destIconBytes[(pixelOffset * 3) + colorOffset] = colorValue;
                    pixelOffset++;
                }
            }
        }
    }
    encodedBytes = destIconBytes;
    return true;
}

bool IcnsReader::getA8MaskForIcon(const IcnsIconEntry &icon, QByteArray &A8Mask)
{
    if(icon.iconWidth > 0 && icon.iconHeight > 0) {
        IcnsIconEntry mask;
        bool hasMask = false;
        if(icon.iconMaskType == IconPlusMask) {
            mask = icon;
            hasMask = true;
        }
        else {
            IcnsIconBitDepth targetDepth = (icon.iconBitDepth == IconRLE24) ? Icon8bit : IconMono;
            for (int i = 0; i < m_masks.size(); i++) {
                bool suitable = m_masks.at(i).iconGroup == icon.iconGroup ||
                        (m_masks.at(i).iconHeight == icon.iconHeight &&
                         m_masks.at(i).iconWidth == icon.iconWidth);
                if(suitable) {
                    if(m_masks.at(i).iconBitDepth == targetDepth) {
                        mask = m_masks.at(i);
                        hasMask = true;
                    }
                }
            }
        }
        if(hasMask) {
            if(mask.iconBitDepth != IconMono && mask.iconBitDepth != Icon8bit) {
                qWarning() << "IcnsReader::getA8MaskForIcon(): Mask has unusual bit depth, can't read:"
                           << mask.iconBitDepth << "OSType:" << mask.header.OSType;
                return false;
            }
            const quint32 pixelCount = mask.iconWidth * mask.iconHeight;
            const float bytesPerPixel = ((float)mask.iconBitDepth / 8);
            const quint32 imageDataSize = pixelCount*bytesPerPixel;
            const quint32 basePos = mask.imageDataOffset;
            const quint32 pos = (mask.iconMaskType == IconPlusMask) ? (basePos + imageDataSize) : basePos;
            const qint64 oldPos = m_stream.device()->pos();
            if(m_stream.device()->seek(pos)) {
                A8Mask.fill(0, pixelCount);
                quint8 byte = 0;
                for(quint32 pixel = 0; pixel < pixelCount; pixel++) {
                    if(mask.iconBitDepth == IconMono) {
                        if(pixel % 8 == 0)
                            m_stream >> byte;
                        quint8 alpha = (byte & 0x80) ? 0xFF : 0x00; // left 1 bit
                        byte = byte << 1;
                        A8Mask[pixel] = alpha;
                    }
                    else {
                        m_stream >> byte;
                        A8Mask[pixel] = byte;
                    }
                }
                m_stream.device()->seek(oldPos);
                return true;
            }
        }
    }
    return false;
}

bool IcnsReader::parseIconDetails(IcnsIconEntry &icon) {
    const QByteArray OSType = QByteArray::fromHex(QByteArray::number(icon.header.OSType,16));
    // Typical OSType naming: <junk><group><depth><mask>;
#if QT_VERSION >= 0x050000
    const char* pattern = "^(?<junk>[\\D]{0,4})(?<group>[a-z|A-Z]{1})(?<depth>\\d{0,2})(?<mask>[#mk]{0,2})$";
    QRegularExpression regexp(pattern);
    QRegularExpressionMatch match = regexp.match(OSType);
    const bool hasMatch = match.hasMatch();
    const QString junk = match.captured("junk");
    const QString group = match.captured("group");
    QString depth = match.captured("depth");
    const QString mask = match.captured("mask");
#else
    const char* pattern = "^([\\D]{0,4})([a-z|A-Z]{1})(\\d{0,2})([#mk]{0,2})$";
    QRegExp regexp(pattern);
    const bool hasMatch = (regexp.indexIn(OSType) >= 0);
    QStringList match = regexp.capturedTexts();
    const QString junk = (1 <= match.size()) ? match.at(1) : "";
    const QString group = (2 <= match.size()) ? match.at(2) : "";
    QString depth = (3 <= match.size()) ? match.at(3) : "";
    const QString mask = (4 <= match.size()) ? match.at(4) : "";
#endif
    // Icon group:
    icon.iconGroup = IcnsIconGroup(group.at(0).toLatin1());
    // Width/height:
    quint32 width = 0;
    quint32 height = 0;
    switch(icon.iconGroup) {
    case IconGroup16x12: {
        width = 16;
        height = 12;
        break;
    }
    case IconGroup16x16: {
        width = 16;
        height = 16;
        break;
    }
    case IconGroup32x32Old:
        depth = "1";
    case IconGroup32x32: {
        width = 32;
        height = 32;
        break;
    }
    case IconGroup48x48: {
        width = 48;
        height = 48;
        break;
    }
    case IconGroup128x128: {
        width = 128;
        height = 128;
        break;
    }
    default:
        // Deprecated icons like "tile" need to be handled somehow.
        break;
    }
    icon.iconWidth = width;
    icon.iconHeight = height;
    // Depth:
    icon.iconBitDepth = (mask == "#") ? IconMono : IcnsIconBitDepth(depth.toUInt());
    // Mask:
    const float bytesPerPixel = ((float)icon.iconBitDepth / 8);
    const quint32 assumedImageDataSize = (width*height)*bytesPerPixel;
    IcnsIconMaskType maskType = mask.isEmpty() ? IconNoMask : IconIsMask;
    if(maskType != IconNoMask && icon.imageDataSize == assumedImageDataSize*2)
        maskType = IconPlusMask;
    icon.iconMaskType = maskType;
    // Notify and return:
    if(hasMatch)
        qDebug() << "IcnsReader::parseIconDetails() Parsing:" << junk << group << depth << mask;
    else
        qWarning() << "IcnsReader::parseIconDetails() Parsing failed, ignored. Reg exp: no match for:" << OSType;

    return hasMatch;
}

bool IcnsReader::addIcon(IcnsIconEntry &icon)
{
    bool success = parseIconDetails(icon);
    if(success) {
        switch(icon.iconMaskType) {
        case IconNoMask:
            m_icons << icon;
            break;
        case IconPlusMask: {
            m_icons << icon;
            m_masks << icon;
        }
        case IconIsMask:
            m_masks << icon;
        }
    }
    else
        qWarning() << "IcnsReader::addIcon(): Unable to parse icon" << icon.header.OSType;
    return success;
}

bool IcnsReader::scanFile()
{
    m_stream.device()->seek(0);

    IcnsBlockHeader blockHeader;
    while (!m_stream.atEnd()) {

        m_stream >> blockHeader;
        if (m_stream.status() != QDataStream::Ok)
            return false;

        switch (blockHeader.OSType) {
        case OSType_icnsfile:
            if (m_stream.device()->size() != blockHeader.length)
                return false;
            break;
        case OSType_icnV:
            m_stream.skipRawData(4);
            break;
        case OSType_TOC_: {
            QVector<IcnsBlockHeader> toc;
            const quint32 tocEntriesCount = (blockHeader.length - IcnsBlockHeaderSize) / IcnsBlockHeaderSize;
            for(uint i = 0; i < tocEntriesCount; i++) {
                IcnsBlockHeader tocEntry;
                m_stream >> tocEntry;
                toc << tocEntry;
                IcnsIconEntry icon;
                icon.header = tocEntry;
                quint32 imgDataOffset = blockHeader.length + IcnsBlockHeaderSize;
                for(uint n = 0; n < i; n++)
                    imgDataOffset += toc.at(n).length;
                icon.imageDataOffset = imgDataOffset + IcnsBlockHeaderSize;
                icon.imageDataSize = icon.header.length - IcnsBlockHeaderSize;
                addIcon(icon);
            }
            return true; // TOC scan gives enough data to discard scan of other blocks
        }
        default: {
            IcnsIconEntry icon;
            icon.header = blockHeader;
            icon.imageDataOffset = m_stream.device()->pos();
            icon.imageDataSize = icon.header.length - IcnsBlockHeaderSize;
            addIcon(icon);
            m_stream.skipRawData(icon.imageDataSize);
            break;
        }
        }
    }
    return true;
}

int IcnsReader::count()
{
    return m_icons.size();
}

QImage IcnsReader::iconAt(int index)
{
    QImage img;

    if(!m_scanned) {
        qWarning("IcnsReader::iconAt(): IcnsReader was unable to scan device");
        return img;
    }

    IcnsIconEntry iconEntry = m_icons.at(index);

    if(m_stream.device()->seek(iconEntry.imageDataOffset)) {
        if(iconEntry.iconGroup == IconGroupCompressed) {
            if(m_stream.device()->peek(8).toHex() == "89504e470d0a1a0a")
                // if PNG magic
                return QImage::fromData(m_stream.device()->peek(iconEntry.imageDataSize), "png");
            else if(m_stream.device()->peek(12).toHex() == "0000000c6a5020200d0a870a")
                // if JPEG 2000 magic
                return QImage::fromData(m_stream.device()->peek(iconEntry.imageDataSize), "jp2");
            else {
                qWarning() << "IcnsReader::iconAt(): Unsupported compressed icon format, OSType:" << iconEntry.header.OSType;
                return img;
            }
        }
        else if(iconEntry.iconHeight == 0 || iconEntry.iconWidth == 0) {
            qWarning() << "IcnsReader::iconAt(): Sizes are not known for raw icon, OSType:" << iconEntry.header.OSType;
            return img;
        }
        // To do: more subformats!
        const quint32 width = iconEntry.iconWidth;
        const quint32 height = iconEntry.iconHeight;
        QByteArray maskA8;
        const bool iconHasAlphaMask = getA8MaskForIcon(iconEntry, maskA8);
        const QImage::Format format = iconHasAlphaMask ? QImage::Format_ARGB32 : QImage::Format_RGB32;
        img = QImage(width, height, format);
        quint8 byte = 0;
        quint32 pixel = 0;
        quint8 red = 0;
        quint8 green = 0;
        quint8 blue = 0;

        switch(iconEntry.iconBitDepth) {
        case IconMono:
        case Icon4bit:
        case Icon8bit: {
            for(uint y = 0; y < height; y++) {
                for(uint x = 0; x < width; x++) {
                    if(pixel % (8 / iconEntry.iconBitDepth) == 0)
                        m_stream >> byte;
                    switch(iconEntry.iconBitDepth) {
                    case IconMono: {
                        quint8 value = (byte & 0x80) ? 0x00 : 0xFF; // left 1 bit
                        red = value;
                        green = value;
                        blue = value;
                        break;
                    }
                    case Icon4bit: {
                        quint8 value = ((byte & 0xF0) >> 4); // left 4 bits
                        IcnsColorEntry888 color = IcnsColorTable4bit[value];
                        red = color.red;
                        green = color.green;
                        blue = color.blue;
                        break;
                    }
                    default: { //8bit
                        IcnsColorEntry888 color = IcnsColorTable8bit[byte];
                        red = color.red;
                        green = color.green;
                        blue = color.blue;
                    }
                    }
                    byte = byte << iconEntry.iconBitDepth;
                    if(iconHasAlphaMask)
                        img.setPixel(x,y,qRgba(red,green,blue,maskA8.at(pixel)));
                    else
                        img.setPixel(x,y,qRgb(red,green,blue));
                    pixel++;
                }
            }
            break;
        }
        case IconRLE24: {
            QByteArray data = m_stream.device()->peek(iconEntry.imageDataSize);
            if(decompressRLE24toR8G8B8(data, width*height)) {
                QDataStream stream(data);
                for(uint y = 0; y < height; y++) {
                    for(uint x = 0; x < width; x++) {
                        stream >> red >> green >> blue;
                        if(iconHasAlphaMask)
                            img.setPixel(x,y,qRgba(red,green,blue,maskA8.at(pixel)));
                        else
                            img.setPixel(x,y,qRgb(red,green,blue));
                        pixel++;
                    }
                }
            }
            else
                qWarning() << "IcnsReader::iconAt(): RLE24 decompressing failed for OSType:" << iconEntry.header.OSType;
            break;
        }
        default: {
            qWarning() << "IcnsReader::iconAt(): Icon:" << index
                       << "Unsupported icon bit depth:" << iconEntry.iconBitDepth;
        }
        }
    }
    return img;
}
