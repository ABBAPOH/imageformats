#include "icnsreader.h"

QDataStream &operator>>(QDataStream &in, IcnsBlockHeader &p)
{
    in >> p.OSType;
    in >> p.length;
    return in;
}

QDataStream &operator<<(QDataStream &out, IcnsBlockHeader &p)
{
    out << p.OSType;
    out << p.length;
    return out;
}

QDataStream &operator>>(QDataStream &in, IcnsColorEntryR8G8B8 &p)
{
    in >> p.red >> p.green >> p.blue;
    return in;
}

IcnsReader::IcnsReader(QIODevice *iodevice)
{
    Q_ASSERT(iodevice);
    m_stream.setDevice(iodevice);
    m_stream.setByteOrder(QDataStream::BigEndian);
    m_scanned = scanFile();
}

QByteArray IcnsReader::getRGB32fromRLE24(const QByteArray &encodedBytes, quint32 expectedPixelCount)
{
    if(encodedBytes.isEmpty()) {
        qWarning("IcnsReader::getRGB32fromRLE24(): encoded bytes are empty!");
        return QByteArray();
    }
    quint32 rawDataSize = encodedBytes.size();
    quint32 destIconDataSize = expectedPixelCount * 3;
    QByteArray destIconBytes(destIconDataSize,0);   // Decompressed Raw Icon Data
    qDebug("IcnsReader::getRGB32fromRLE24(): Compressed RLE data size is %d",encodedBytes.size());
    qDebug("IcnsReader::getRGB32fromRLE24(): Decompressed will be %d bytes (%d pixels)",(int)destIconDataSize,(int)expectedPixelCount);
    qDebug("IcnsReader::getRGB32fromRLE24(): Decoding RLE data into RGB pixels...");
    quint32	dataOffset = 0;
    if(*((quint32*)encodedBytes.constData()) == 0x00000000) {
        qDebug("IcnsReader::getRGB32fromRLE24(): 4 byte null padding found in rle data!");
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
    return destIconBytes;
}

QVector<QRgb> IcnsReader::getColorTable(const IcnsIconBitDepth &depth)
{
    QVector<QRgb> table;
    switch(depth) {
    case IconMono: {
        IcnsColorEntryR8G8B8 index = IcnsColorTable4bit[0];
        table << qRgb(index.red, index.green, index.blue);
        index = IcnsColorTable4bit[15];
        table << qRgb(index.red, index.green, index.blue);
        break;
    }
    case Icon4bit:
    case Icon8bit: {
        for(quint32 i = 0; i < pow(2,depth); i++) {
            IcnsColorEntryR8G8B8 index = (depth == Icon4bit) ? IcnsColorTable4bit[i] : IcnsColorTable8bit[i];
            table << qRgb(index.red, index.green, index.blue);
        }
        break;
    }
    default:
        qWarning("IcnsReader::getColorTable(): No color table for bit depth: %u", depth);
    }
    return table;
}

bool IcnsReader::parseIconDetails(IcnsIconEntry &icon)
{
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
    icon.iconData.group = IcnsIconGroup(group.at(0).toLatin1());
    // Width/height:
    if(icon.iconData.group != IconGroupCompressed) {
        for(quint8 i = 0; i < IcnsUncompressedGroupsNum; i++) {
            if(IcnsUncompressedGroups[i].group == icon.iconData.group) {
                icon.iconData.depth = IcnsUncompressedGroups[i].depth;
                icon.iconData.size.width = IcnsUncompressedGroups[i].size.width;
                icon.iconData.size.height = IcnsUncompressedGroups[i].size.height;
                break;
            }
        }
    }
    else {
        // Just for experimental/research purpose.
        // Effectively does nothing, just tests Apple's naming policy for OSTypes
        uint formatid = depth.toUInt();
        if(formatid <= 10) {
            icon.iconData.size.width = pow(2,formatid);
            icon.iconData.size.height = pow(2,formatid);
        }
        else {
            icon.iconData.size.width = 0;
            icon.iconData.size.height = 0;
            qDebug() << "IcnsReader::parseIconDetails(): Info - Compr. format size naming differs (retina?):" << icon.header.OSType;
        }
    }
    // Depth for uncompressed / format id for compressed:
    icon.iconData.depth = (mask == "#") ? IconMono : IcnsIconBitDepth(depth.toUInt());
    // Mask:
    const float bytesPerPixel = ((float)icon.iconData.depth / 8);
    const quint32 assumedImageDataSize = (icon.iconData.size.width * icon.iconData.size.height) * bytesPerPixel;
    IcnsIconMaskType maskType = mask.isEmpty() ? IconNoMask : IconIsMask;
    if(maskType != IconNoMask && icon.imageDataSize == assumedImageDataSize * 2)
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
        case IconPlusMask:
            m_icons << icon;
            m_masks << icon;
            break;
        case IconIsMask:
            m_masks << icon;
            break;
        default: //IconNoMask
            m_icons << icon;
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

QImage IcnsReader::iconAlphaAt(int index)
{
    QImage img;
    IcnsIconEntry icon = m_icons.at(index);
    if(icon.iconData.size.width > 0 && icon.iconData.size.height > 0) {
        IcnsIconEntry mask;
        bool hasMask = false;
        if(icon.iconMaskType == IconPlusMask) {
            mask = icon;
            hasMask = true;
        }
        else {
            IcnsIconBitDepth targetDepth = (icon.iconData.depth == IconRLE24) ? Icon8bit : IconMono;
            for (int i = 0; i < m_masks.size(); i++) {
                bool suitable = m_masks.at(i).iconData.group == icon.iconData.group ||
                        (m_masks.at(i).iconData.size.height == icon.iconData.size.height &&
                         m_masks.at(i).iconData.size.width == icon.iconData.size.width);
                if(suitable) {
                    if(m_masks.at(i).iconData.depth == targetDepth) {
                        mask = m_masks.at(i);
                        hasMask = true;
                    }
                }
            }
        }
        if(hasMask) {
            if(mask.iconData.depth != IconMono && mask.iconData.depth != Icon8bit) {
                qWarning() << "IcnsReader::getA8MaskForIcon(): Mask has unusual bit depth, can't read:"
                           << mask.iconData.depth << "OSType:" << mask.header.OSType;
                return img;
            }
            const quint32 width = mask.iconData.size.width;
            const quint32 height = mask.iconData.size.height;
            const quint32 pixelCount = width * height;
            const float bytesPerPixel = ((float)mask.iconData.depth / 8);
            const quint32 imageDataSize = pixelCount * bytesPerPixel;
            const quint32 basePos = mask.imageDataOffset;
            const quint32 pos = (mask.iconMaskType == IconPlusMask) ? (basePos + imageDataSize) : basePos;
            const qint64 oldPos = m_stream.device()->pos();
            if(m_stream.device()->seek(pos)) {
                img = QImage(width, height, QImage::Format_RGB32);
                quint8 byte = 0;
                quint32 pixel = 0;
                for(quint32 y = 0; y < height; y++) {
                    for(quint32 x = 0; x < width; x++) {
                        if(mask.iconData.depth == IconMono) {
                            if(pixel % 8 == 0)
                                m_stream >> byte;
                            quint8 alpha = (byte & 0x80) ? 0xFF : 0x00; // left 1 bit
                            byte = byte << 1;
                            img.setPixel(x,y,qRgb(alpha,alpha,alpha));
                        }
                        else {
                            m_stream >> byte;
                            img.setPixel(x,y,qRgb(byte,byte,byte));
                        }
                        pixel++;
                    }
                }
                m_stream.device()->seek(oldPos);
            }
        }
    }
    return img;
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
        if(iconEntry.iconData.group == IconGroupCompressed) {
            if(m_stream.device()->peek(8).toHex() == "89504e470d0a1a0a")
                // if PNG magic
                return QImage::fromData(m_stream.device()->peek(iconEntry.imageDataSize), "png");
            else if(m_stream.device()->peek(12).toHex() == "0000000c6a5020200d0a870a")
                // if JPEG 2000 magic
                return QImage::fromData(m_stream.device()->peek(iconEntry.imageDataSize), "jp2");
            else {
                qWarning("IcnsReader::iconAt(): Unsupported compressed icon format, OSType: %u", iconEntry.header.OSType);
                return img;
            }
        }
        else if(iconEntry.iconData.size.height == 0 || iconEntry.iconData.size.width == 0) {
            qWarning("IcnsReader::iconAt(): Sizes are not known for raw icon, OSType: %u", iconEntry.header.OSType);
            return img;
        }
        // To do: more subformats!
        const quint32 width = iconEntry.iconData.size.width;
        const quint32 height = iconEntry.iconData.size.height;
        quint8 byte = 0;
        quint32 pixel = 0;

        switch(iconEntry.iconData.depth) {
        case IconMono:
        case Icon4bit:
        case Icon8bit: {
            const QImage::Format format = (iconEntry.iconData.depth == IconMono) ? QImage::Format_Mono : QImage::Format_Indexed8;
            img = QImage(width, height, format);
            img.setColorTable(getColorTable(iconEntry.iconData.depth));

            if(img.colorCount() < 2)
                return img;

            for(uint y = 0; y < height; y++) {
                for(uint x = 0; x < width; x++) {
                    if(pixel % (8 / iconEntry.iconData.depth) == 0)
                        m_stream >> byte;

                    quint8 cindex = 0;
                    switch(iconEntry.iconData.depth) {
                    case IconMono: {
                        cindex = (byte & 0x80) ? 1 : 0; // left 1 bit
                        break;
                    }
                    case Icon4bit: {
                        quint8 value = ((byte & 0xF0) >> 4); // left 4 bits
                        cindex = (value < pow(2,iconEntry.iconData.depth)) ? value : 0;
                        break;
                    }
                    default: //8bit
                        cindex = (byte < pow(2,iconEntry.iconData.depth)) ? byte : 0;
                    }
                    byte = byte << iconEntry.iconData.depth;
                    img.setPixel(x,y,cindex);
                    pixel++;
                }
            }
            break;
        }
        case IconRLE24: {
            img = QImage(width, height, QImage::Format_RGB32);
            IcnsColorEntryR8G8B8 color;
            QByteArray data = getRGB32fromRLE24(m_stream.device()->peek(iconEntry.imageDataSize), width*height);
            if(!data.isEmpty()) {
                QDataStream stream(data);
                for(uint y = 0; y < height; y++) {
                    for(uint x = 0; x < width; x++) {
                        stream >> color;
                        img.setPixel(x,y,qRgb(color.red,color.green,color.blue));
                        pixel++;
                    }
                }
            }
            else
                qWarning("IcnsReader::iconAt(): RLE24 decompressing failed for OSType: %u", iconEntry.header.OSType);
            break;
        }
        default: {
            qWarning() << "IcnsReader::iconAt(): Icon:" << index
                       << "Unsupported icon bit depth:" << iconEntry.iconData.depth;
        }
        }
        if(!img.isNull()) {
            QImage alpha = iconAlphaAt(index);
            if(!alpha.isNull())
                img.setAlphaChannel(alpha);
            //TODO: Replace with QPainter compositions in the future?
        }
    }
    return img;
}

bool IcnsReader::write(QIODevice *device, const QImage &image)
{
    // NOTE: Experemental implementation. Just for simple brainless converting tasks / testing purposes.
    // LIMITATIONS: Writes complete icns file containing only signle square icon in PNG format to a device.
    // Currently uses non-hardcoded OSTypes.
    bool retValue = false;

    const int width = image.size().width();
    const int height = image.size().height();
    const bool sizeIsCorrect = (width == height) && (width >= 16) && ((width & (width - 1)) == 0);
    if (device->isWritable() && sizeIsCorrect) {
        //qint64 origPos = device->pos();
        // Construct icon OSType
        int i = width;
        uint p = 0;
        while (i >>= 1) {p++;}
        const QByteArray ostypebase = (p < 7) ? "ipc" : "ic"; // small / big icons naming policy
        const QByteArray ostypenum = (ostypebase.size() > 2 || p >= 10) ? QByteArray::number(p) : QByteArray::number(p).prepend("0");
        const quint32 ostype = QByteArray(ostypebase).append(ostypenum).toHex().toUInt(NULL,16);
        // Construct ICNS Header
        IcnsBlockHeader fileHeader;
        fileHeader.OSType = OSType_icnsfile;
        // Construct TOC Header
        IcnsBlockHeader tocHeader;
        tocHeader.OSType = OSType_TOC_;
        // Construct TOC Entry
        IcnsBlockHeader tocEntry;
        tocEntry.OSType = ostype;
        // Construct Icon block
        IcnsBlockHeader iconEntry;
        iconEntry.OSType = ostype;
        // Construct image data
        QByteArray imageData;
        QBuffer buffer(&imageData);
        if(buffer.open(QIODevice::WriteOnly) && image.save(&buffer, "png")) {
            buffer.close();
            iconEntry.length = IcnsBlockHeaderSize + imageData.size();
            tocEntry.length = iconEntry.length;
            tocHeader.length = IcnsBlockHeaderSize*2;
            fileHeader.length = IcnsBlockHeaderSize + tocHeader.length + iconEntry.length;
            // Write everything
            QDataStream stream(device);
            stream.setByteOrder(QDataStream::BigEndian);
            stream << fileHeader << tocHeader << tocEntry << iconEntry;
            device->write(imageData); // QDataStream inserts number of bytes written, so we need to use lower level
            if(stream.status() == QDataStream::Ok)
                retValue = true;
        }
    }
    return retValue;
}
