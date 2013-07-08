#include "qicnshandler.h"

#include <QtGui/QImage>
#include <QtCore/QDataStream>
#if QT_VERSION >= 0x050000
#include <QtCore/QRegularExpression>
#endif

#include <QDebug>

QDataStream &operator>>(QDataStream &in, IcnsBlockHeader &p)
{
    in >> p.magic;
    in >> p.length;
    return in;
}

IcnsReader::IcnsReader(QIODevice *iodevice)
{
    Q_ASSERT(iodevice);
    m_stream.setDevice(iodevice);
    m_stream.setByteOrder(QDataStream::BigEndian);
    m_scanned = false;
}

QByteArray decompressRLE24(const QByteArray &encodedBytes, quint32 expectedPixelCount)
{
    // From libicns
    quint8	colorOffset = 0;
    uchar   colorValue = 0;
    quint8	runLength = 0;
    quint32	dataOffset = 0;
    quint32	pixelOffset = 0;
    quint32	i = 0;
    quint32 rawDataSize = encodedBytes.size();
    quint32 destIconDataSize = expectedPixelCount * 4;
    QByteArray destIconBytes(destIconDataSize,0);   // Decompressed Raw Icon Data

    if(encodedBytes.isEmpty()) {
        qWarning("decompressRLE24(): encoded bytes are empty!");
        return destIconBytes;
    }
    qDebug("Compressed RLE data size is %d",encodedBytes.size());
    qDebug("Decompressed will be %d bytes (%d pixels)",(int)destIconDataSize,(int)expectedPixelCount);
    qDebug("Decoding RLE data into RGB pixels...");
    // What's this??? In the 128x128 icons, we need to start 4 bytes
    // ahead. There is often a NULL padding here for some reason. If
    // we don't, the red channel will be off by 2 pixels, or worse
    if( *((quint32*)encodedBytes.constData()) == 0x00000000 ) {
        qDebug("4 byte null padding found in rle data!");
        dataOffset = 4;
    }
    // Data is stored in red run, green run,blue run
    // So we decompress to pixel format RGBA
    // RED:   byte[0], byte[4], byte[8]  ...
    // GREEN: byte[1], byte[5], byte[9]  ...
    // BLUE:  byte[2], byte[6], byte[10] ...
    // ALPHA: byte[3], byte[7], byte[11] do nothing with these bytes
    for(colorOffset = 0; colorOffset < 3; colorOffset++) {
        pixelOffset = 0;
        while((pixelOffset < expectedPixelCount) && (dataOffset < rawDataSize)) {
            if( (encodedBytes.data()[dataOffset] & 0x80) == 0) {
                // Top bit is clear - run of various values to follow
                runLength = (0xFF & encodedBytes.data()[dataOffset++]) + 1; // 1 <= len <= 128
                for(i = 0; (i < runLength) && (pixelOffset < expectedPixelCount) && (dataOffset < rawDataSize); i++) {
                    destIconBytes.data()[(pixelOffset * 4) + colorOffset] = encodedBytes.data()[dataOffset++];
                    pixelOffset++;
                }
            }
            else {
                // Top bit is set - run of one value to follow
                runLength = (0xFF & encodedBytes.data()[dataOffset++]) - 125; // 3 <= len <= 130
                // Set the value to the color shifted to the correct bit offset
                colorValue = encodedBytes.data()[dataOffset++];
                for(i = 0; (i < runLength) && (pixelOffset < expectedPixelCount); i++) {
                    destIconBytes.data()[(pixelOffset * 4) + colorOffset] = colorValue;
                    pixelOffset++;
                }
            }
        }
    }
    return destIconBytes;
}

void IcnsReader::parseIconDetails(IcnsIconEntry &icon) {
    qint64 oldPos = m_stream.device()->pos();
    if(m_stream.device()->seek(icon.imageDataOffset)) {
        if(m_stream.device()->peek(8).toHex() == "89504e470d0a1a0a")
            icon.iconFormat = IconPNG;
        else if(m_stream.device()->peek(12).toHex() == "0000000c6a5020200d0a870a")
            icon.iconFormat = IconJP2;
        else {
            icon.iconFormat = IconUncompressed;
            QByteArray magic = QByteArray::fromHex(QByteArray::number(icon.header.magic,16));
            // Typical magic naming: <junk><group><depth><mask>;
#if QT_VERSION >= 0x050000
            const char* pattern = "^(?<junk>[\\D]{0,4})(?<group>[a-z|A-Z]{1})(?<depth>\\d{0,2})(?<mask>[#mk]{0,2})$";
            QRegularExpression regexp(pattern);
            QRegularExpressionMatch match = regexp.match(magic);
            const bool hasMatch = match.hasMatch();
            const QString junk = match.captured("junk");
            const QString group = match.captured("group");
            const QString depth = match.captured("depth");
            const QString mask = match.captured("mask");
#else
            const char* pattern = "^([\\D]{0,4})([a-z|A-Z]{1})(\\d{0,2})([#mk]{0,2})$";
            QRegExp regexp(pattern);
            const bool hasMatch = (regexp.indexIn(magic) >= 0);
            QStringList match = regexp.capturedTexts();
            const QString junk = (1 <= match.size()) ? match.at(1) : "";
            const QString group = (2 <= match.size()) ? match.at(2) : "";
            const QString depth = (3 <= match.size()) ? match.at(3) : "";
            const QString mask = (4 <= match.size()) ? match.at(4) : "";
#endif
            icon.iconGroup = group.at(0).toLatin1();
            icon.iconIsMask = !mask.isEmpty();
            icon.iconBitDepth = (mask == "#") ? 1 : depth.toUInt();
            if(hasMatch) {
                qDebug() << "IcnsReader::parseIconDetails() parse:" << junk << group << depth << mask
                         << icon.iconGroup << icon.iconBitDepth << icon.iconIsMask;
            }
            else
                qDebug() << "IcnsReader::parseIconDetails() reg exp: no match for:" << magic;
        }
        m_stream.device()->seek(oldPos);
    }
}

bool IcnsReader::scanBlocks()
{
    Q_ASSERT(m_stream.device());
    m_stream.device()->seek(0);

    IcnsBlockHeader blockHeader;
    while (!m_stream.atEnd()) {

        m_stream >> blockHeader;
        if (m_stream.status() == QDataStream::ReadPastEnd)
            return false;

        switch (blockHeader.magic) {
        case icnsfile:
            if (m_stream.device()->size() != blockHeader.length)
                return false;
            break;
        case TOC_: {
            const quint32 tocEntriesN = (blockHeader.length - IcnsBlockHeaderSize) / IcnsBlockHeaderSize;
            for(uint i = 0; i < tocEntriesN; i++) {
                IcnsBlockHeader tocEntry;
                m_stream >> tocEntry;

                IcnsIconEntry icon;
                icon.header = tocEntry;

                const quint32 imgDataBaseOffset = (i == 0) ?
                            (blockHeader.length + IcnsBlockHeaderSize*2) : // offset for the first icon
                            (blockHeader.length + IcnsBlockHeaderSize);    // offset of the first block
                quint32 imgDataOffset = imgDataBaseOffset;
                for(uint n = 0; n < i; n++)
                    imgDataOffset += m_icons.at(n).header.length;

                icon.imageDataOffset = (i == 0) ? imgDataOffset : imgDataOffset + IcnsBlockHeaderSize;
                icon.imageDataSize = icon.header.length - IcnsBlockHeaderSize;
                parseIconDetails(icon);
                m_icons << icon;
            }
            return true; // TOC scan gives enough data to discard scan of other blocks
        }
        case icnV:
            m_stream.skipRawData(4);
            break;
        default: {
            IcnsIconEntry icon;
            icon.header = blockHeader;
            icon.imageDataOffset = m_stream.device()->pos();
            icon.imageDataSize = icon.header.length - IcnsBlockHeaderSize;
            parseIconDetails(icon);
            m_icons << icon;
            m_stream.skipRawData(icon.imageDataSize);
            break;
        }
        }
    }
    return true;
}

int IcnsReader::count()
{
    if(!m_scanned)
        m_scanned = scanBlocks();
    return m_icons.size();
}

QImage IcnsReader::iconAt(int index)
{
    if(!m_scanned)
        m_scanned = scanBlocks();

    QImage img;
    IcnsIconEntry iconEntry = m_icons.at(index);

    if(m_stream.device()->seek(iconEntry.imageDataOffset)) {
        switch(iconEntry.iconFormat) {
        case IconPNG:
            return QImage::fromData(m_stream.device()->peek(iconEntry.imageDataSize), "png");
            break;
        case IconJP2:
            //To do: JPEG 2000 (need another plugin for that?)
            break;
        default: {
            //Uncompressed/bitmaps:
            if(iconEntry.iconBitDepth <= 0 || iconEntry.iconGroup == 0) {
                qWarning() << "IcnsReader::iconAt(): Icon:" << index
                           << "Unsupported icon type:" << iconEntry.header.magic;
            }
            else {
                //To do: subformats
                switch(iconEntry.iconBitDepth) {
                case 32: {
                    // 32-bit icons are packed into RLE24, needs hardcoding for icon sizes:
                    // dimensions can't be extracted from the size of the data
                    quint8 dimensions;
                    switch (iconEntry.iconGroup) {
                    case 0x74: { // "t" - 128x128
                        dimensions = 128;
                        break;
                    }
                    case 0x68: { // "h" - 48x48
                        dimensions = 48;
                        break;
                    }
                    case 0x6c: { // "l" - 32x32
                        dimensions = 32;
                        break;
                    }
                    case 0x73: { // "s" - 16x16
                        dimensions = 16;
                        break;
                    }
                    default:
                        qWarning() << "IcnsReader::iconAt(): Icon:" << index
                                   << "Unsupported 32-bit icon group:" << iconEntry.iconGroup;
                    }
                    if(dimensions) {
                        img = QImage(dimensions, dimensions, QImage::Format_RGB32);
                        QByteArray RLE24 = m_stream.device()->peek(iconEntry.imageDataSize);
                        QByteArray decompressed = decompressRLE24(RLE24, dimensions*dimensions);
                        QDataStream stream(decompressed);
                        for(uint y = 0; y < dimensions; y++) {
                            for(uint x = 0; x < dimensions; x++) {
                                quint8 red, green, blue;
                                stream >> red >> green >> blue;
                                stream.skipRawData(1); //alpha
                                img.setPixel(x,y,qRgb(red,green,blue));
                            }
                        }
                    }
                    break;
                }
                default: {
                    qWarning() << "IcnsReader::iconAt(): Icon:" << index
                               << "Unsupported icon bit depth:" << iconEntry.iconBitDepth;
                }
                }
            }
        }
        }
    }
    return img;
}

QIcnsHandler::QIcnsHandler(QIODevice *device)
{
    m_reader = new IcnsReader(device);
    m_currentIconIndex = 0;
}

QIcnsHandler::~QIcnsHandler()
{
    delete m_reader;
}

bool QIcnsHandler::read(QImage *outImage)
{
    qDebug("QIcnsHandler::read() call, m_currentIconIndex=%i", m_currentIconIndex);

    QImage img = m_reader->iconAt(m_currentIconIndex);    
    *outImage = img;

    return !img.isNull();
}

QByteArray QIcnsHandler::name() const
{
    return "icns";
}

bool QIcnsHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("QIcnsHandler::canRead() called with no device");
        return false;
    }

    if(device->isSequential()) {
        qWarning("QIcnsHandler::canRead() called on sequential device (NYI)");
        return false;
    }

    return device->peek(4) == "icns";
}

bool QIcnsHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("icns");
        return true;
    }
    return false;
}

int QIcnsHandler::imageCount() const
{
    return m_reader->count();
}

bool QIcnsHandler::jumpToImage(int imageNumber)
{
    if (imageNumber < imageCount())
        m_currentIconIndex = imageNumber;

    return (imageNumber < imageCount()) ? true : false;
}

bool QIcnsHandler::jumpToNextImage()
{
    return jumpToImage(m_currentIconIndex + 1);
}
