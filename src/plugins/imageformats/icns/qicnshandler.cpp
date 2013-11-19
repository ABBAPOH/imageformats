#include "qicnshandler.h"

static const quint8 IcnsBlockHeaderSize = 8;

static QDataStream &operator>>(QDataStream &in, IcnsBlockHeader &p)
{
    in >> p.OSType;
    in >> p.length;
    return in;
}

static QDataStream &operator<<(QDataStream &out, IcnsBlockHeader &p)
{
    out << p.OSType;
    out << p.length;
    return out;
}

static QByteArray getRGB32fromRLE24(const QByteArray &RLEBytes, quint32 estPxsNum)
{
    if (RLEBytes.isEmpty()) {
        qWarning("getRGB32fromRLE24(): Encoded bytes are empty!");
        return RLEBytes;
    }
    QByteArray RGBBytes((estPxsNum * 3), 0);
    qint32 RLEOffset = (*((quint32*)RLEBytes.constData()) == 0) ? 4 : 0; // Sometimes zero-padding is present
    // Data is stored in red run, green run,blue run
    for (quint8 colorNRun = 0; colorNRun < 3; colorNRun++) {
        quint32	pixel = 0;
        while ((pixel < estPxsNum) && (RLEOffset < RLEBytes.size())) {
            if ((RLEBytes[RLEOffset] & 0x80) == 0) {
                // Top bit is clear - run of various values to follow
                quint8 runLength = (0xFF & RLEBytes[RLEOffset++]) + 1; // 1 <= len <= 128
                for (quint8 i = 0; (i < runLength) && (pixel < estPxsNum) && (RLEOffset < RLEBytes.size()); i++) {
                    RGBBytes[(pixel * 3) + colorNRun] = RLEBytes[RLEOffset++];
                    pixel++;
                }
            }
            else {
                // Top bit is set - run of one value to follow
                quint8 runLength = (0xFF & RLEBytes[RLEOffset++]) - 125; // 3 <= len <= 130
                // Set the value to the color shifted to the correct bit offset
                quint8 value = RLEBytes[RLEOffset++];
                for (quint8 i = 0; (i < runLength) && (pixel < estPxsNum); i++) {
                    RGBBytes[(pixel * 3) + colorNRun] = value;
                    pixel++;
                }
            }
        }
    }
    return RGBBytes;
}

QIcnsHandler::QIcnsHandler()
{
    m_currentIconIndex = 0;
    m_scanstate = IcnsFileIsNotParsed;
}

QIcnsHandler::QIcnsHandler(QIODevice * d, const QByteArray &format)
{
    setDevice(d);
    setFormat(format);
    m_stream.setDevice(d);
    m_currentIconIndex = 0;
    m_scanstate = IcnsFileIsNotParsed;
}

QByteArray QIcnsHandler::name() const
{
    return "icns";
}

bool QIcnsHandler::canRead(QIODevice *device)
{
    if (!device || !device->isReadable()) {
        qWarning("QIcnsHandler::canRead() called without a readable device");
        return false;
    }
    if (device->isSequential()) {
        qWarning("QIcnsHandler::canRead() called on sequential device (NYI)");
        return false;
    }
    return device->peek(4) == "icns";
}

bool QIcnsHandler::canWrite(QIODevice *device)
{
    if (!device || !device->isWritable()) {
        qWarning("QIcnsHandler::canRead() called without a writable device");
        return false;
    }
    return true;
}

bool QIcnsHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("icns");
        return true;
    }
    return false;
}

bool QIcnsHandler::read(QImage *outImage)
{
    QImage img;
    if (!isScanned())
        scanDevice();
    if (!isParsed()) {
        qWarning("QIcnsHandler::read(): The device was not parced properly!");
        *outImage = img;
        return false;
    }

    IcnsIconEntry icon = m_icons.at(m_currentIconIndex);
    if (m_stream.device()->seek(icon.dataOffset())) {
        if (m_stream.device()->peek(8).toHex() == QByteArrayLiteral("89504e470d0a1a0a")) { // if PNG magic
            img = QImage::fromData(m_stream.device()->peek(icon.dataLength()), "png");
            if (img.isNull()) {
                qWarning("QIcnsHandler::read(): Read failed, PNG is not supported by your Qt distribution. OSType: %u", icon.getOSType());
            }
        }
        else if (m_stream.device()->peek(12).toHex() == QByteArrayLiteral("0000000c6a5020200d0a870a")) { // if JPEG 2000 magic
            img = QImage::fromData(m_stream.device()->peek(icon.dataLength()), "jp2");
            if (img.isNull()) {
                qWarning("QIcnsHandler::read(): Read failed, JPEG2000 is not supported by your Qt distribution. OSType: %u", icon.getOSType());
            }
        }
        else if (icon.group() == IconGroupCompressed) {
            qWarning("QIcnsHandler::read(): Unsupported compressed icon format, OSType: %u", icon.getOSType());
        }
        else if (icon.height() == 0 || icon.width() == 0) {
            qWarning("QIcnsHandler::read(): Size of a raw icon is unknown, OSType: %u", icon.getOSType());
        }
        else {
            switch(icon.depth()) {
            case IconMono:
            case Icon4bit:
            case Icon8bit:
                img = readLowDepthIconFromStream(icon, m_stream);
                break;
            case Icon32bit:
                img = read32bitIconFromStream(icon, m_stream);
                break;
            default: {
                qWarning() << "QIcnsHandler::read(): Icon #:" << m_currentIconIndex
                           << "Unsupported icon bit depth:" << icon.depth();
            }
            }
        }
    }
    if (!img.isNull()) {
        QImage alpha = iconAlpha(m_currentIconIndex);
        if (!alpha.isNull())
            img.setAlphaChannel(alpha);
        //TODO: Replace with QPainter compositions in the future?
    }
    *outImage = img;
    return !img.isNull();
}

bool QIcnsHandler::write(const QImage &image)
{
    QIODevice *device = QImageIOHandler::device();
    // NOTE: Experemental implementation. Just for simple brainless converting tasks / testing purposes.
    // LIMITATIONS: Writes complete icns file containing only signle square icon in PNG format to a device.
    // Currently uses non-hardcoded OSTypes.
    QImage img = image;
    const int width = img.size().width();
    const int height = img.size().height();
    const bool sizeIsCorrect = (width == height) && (width >= 16) && ((width & (width - 1)) == 0);
    if (!device->isWritable() && !sizeIsCorrect) {
        return false;
    }
    // Construct icon OSType
    int i = width;
    uint p = 0;
    while (i >>= 1) {p++;}
    if (p > 10) {
        // Force resizing to 1024x1024. Values over 10 are reserved for retina icons
        p = 10;
        img = img.scaled(1024,1024);
    }
    const QByteArray ostypebase = (p < 7) ? "ipc" : "ic"; // small / big icons naming policy
    const QByteArray ostypenum = (ostypebase.size() > 2 || p >= 10) ? QByteArray::number(p) : QByteArray::number(p).prepend("0");
    const quint32 ostype = QByteArray(ostypebase).append(ostypenum).toHex().toUInt(NULL,16);
    // Construct ICNS Header
    IcnsBlockHeader fileHeader;
    fileHeader.OSType = OSType_icns;
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
    if (!buffer.open(QIODevice::WriteOnly) && !img.save(&buffer, "png")) {
        return false;
    }
    buffer.close();
    iconEntry.length = IcnsBlockHeaderSize + imageData.size();
    tocEntry.length = iconEntry.length;
    tocHeader.length = IcnsBlockHeaderSize*2;
    fileHeader.length = IcnsBlockHeaderSize + tocHeader.length + iconEntry.length;
    // Write everything
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << fileHeader << tocHeader << tocEntry << iconEntry;
    stream.writeRawData(imageData.constData(),imageData.size());
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    return true;
}

int QIcnsHandler::imageCount() const
{
    if (!isScanned()) {
        QIcnsHandler* that = const_cast<QIcnsHandler *>(this);
        that->scanDevice();
    }
    return m_icons.size();
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

QImage QIcnsHandler::readLowDepthIconFromStream(const QIcnsHandler::IcnsIconEntry &icon, QDataStream &stream)
{
    QImage img;
    quint8 byte = 0;
    quint32 pixel = 0;
    const QImage::Format format = (icon.depth() == IconMono) ? QImage::Format_Mono : QImage::Format_Indexed8;
    QVector<QRgb> colortable = getColorTable(icon.depth());
    if (colortable.size() < 2) {
        return img;
    }
    img = QImage(icon.width(), icon.height(), format);
    img.setColorTable(colortable);
    for (uint y = 0; y < icon.height(); y++) {
        for (uint x = 0; x < icon.width(); x++) {
            if (stream.status() != QDataStream::Ok) {
                return img;
            }
            if (pixel % (8 / icon.depth()) == 0) {
                stream >> byte;
            }
            quint8 cindex = 0;
            switch(icon.depth()) {
            case IconMono: {
                cindex = (byte & 0x80) ? 1 : 0; // left 1 bit
                break;
            }
            case Icon4bit: {
                quint8 value = ((byte & 0xF0) >> 4); // left 4 bits
                cindex = (value < pow(2,icon.depth())) ? value : 0;
                break;
            }
            default: //8bit
                cindex = (byte < pow(2,icon.depth())) ? byte : 0;
            }
            byte = byte << icon.depth();
            img.setPixel(x,y,cindex);
            pixel++;
        }
    }
    return img;
}

QImage QIcnsHandler::read32bitIconFromStream(const QIcnsHandler::IcnsIconEntry &icon, QDataStream &stream)
{
    QByteArray data = stream.device()->peek(icon.dataLength());
    if (data.isEmpty()) {
        return QImage();
    }
    QImage img = QImage(icon.width(), icon.height(), QImage::Format_RGB32);
    if (icon.isRLE24()) {
        data = getRGB32fromRLE24(data, icon.width()*icon.height());
    }
    if (data.isEmpty()) {
        qWarning("QIcnsHandler::read32bitIconFromStream(): RLE24 decompression failed. Icon OSType: %u", icon.getOSType());
        return QImage();
    }
    quint32 pixel = 0;
    QDataStream bastream(data);
    for (uint y = 0; y < icon.height(); y++) {
        for (uint x = 0; x < icon.width(); x++) {
            quint8 r, g, b;
            bastream >> r >> g >> b;
            img.setPixel(x,y,qRgb(r,g,b));
            pixel++;
        }
    }
    return img;
}

bool QIcnsHandler::isScanned() const
{
    return (m_scanstate != IcnsFileIsNotParsed);
}

bool QIcnsHandler::isParsed() const
{
    return (m_scanstate == IcnsFileIsParsed);
}

QVector<QRgb> QIcnsHandler::getColorTable(const IconBitDepth &depth)
{
    QVector<QRgb> table;
    switch(depth) {
    case IconMono: {
        table << qRgb(0xFF, 0xFF, 0xFF);
        table << qRgb(0x00, 0x00, 0x00);
        break;
    }
    case Icon4bit: {
        table << qRgb(0xFF, 0xFF, 0xFF);
        table << qRgb(0xFC, 0xF3, 0x05);
        table << qRgb(0xFF, 0x64, 0x02);
        table << qRgb(0xDD, 0x08, 0x06);
        table << qRgb(0xF2, 0x08, 0x84);
        table << qRgb(0x46, 0x00, 0xA5);
        table << qRgb(0x00, 0x00, 0xD4);
        table << qRgb(0x02, 0xAB, 0xEA);
        table << qRgb(0x1F, 0xB7, 0x14);
        table << qRgb(0x00, 0x64, 0x11);
        table << qRgb(0x56, 0x2C, 0x05);
        table << qRgb(0x90, 0x71, 0x3A);
        table << qRgb(0xC0, 0xC0, 0xC0);
        table << qRgb(0x80, 0x80, 0x80);
        table << qRgb(0x40, 0x40, 0x40);
        table << qRgb(0x00, 0x00, 0x00);
        break;
    }
    case Icon8bit: {
        table << qRgb(0xFF, 0xFF, 0xFF);
        table << qRgb(0xFF, 0xFF, 0xCC);
        table << qRgb(0xFF, 0xFF, 0x99);
        table << qRgb(0xFF, 0xFF, 0x66);
        table << qRgb(0xFF, 0xFF, 0x33);
        table << qRgb(0xFF, 0xFF, 0x00);
        table << qRgb(0xFF, 0xCC, 0xFF);
        table << qRgb(0xFF, 0xCC, 0xCC);
        table << qRgb(0xFF, 0xCC, 0x99);
        table << qRgb(0xFF, 0xCC, 0x66);
        table << qRgb(0xFF, 0xCC, 0x33);
        table << qRgb(0xFF, 0xCC, 0x00);
        table << qRgb(0xFF, 0x99, 0xFF);
        table << qRgb(0xFF, 0x99, 0xCC);
        table << qRgb(0xFF, 0x99, 0x99);
        table << qRgb(0xFF, 0x99, 0x66);
        table << qRgb(0xFF, 0x99, 0x33);
        table << qRgb(0xFF, 0x99, 0x00);
        table << qRgb(0xFF, 0x66, 0xFF);
        table << qRgb(0xFF, 0x66, 0xCC);
        table << qRgb(0xFF, 0x66, 0x99);
        table << qRgb(0xFF, 0x66, 0x66);
        table << qRgb(0xFF, 0x66, 0x33);
        table << qRgb(0xFF, 0x66, 0x00);
        table << qRgb(0xFF, 0x33, 0xFF);
        table << qRgb(0xFF, 0x33, 0xCC);
        table << qRgb(0xFF, 0x33, 0x99);
        table << qRgb(0xFF, 0x33, 0x66);
        table << qRgb(0xFF, 0x33, 0x33);
        table << qRgb(0xFF, 0x33, 0x00);
        table << qRgb(0xFF, 0x00, 0xFF);
        table << qRgb(0xFF, 0x00, 0xCC);
        table << qRgb(0xFF, 0x00, 0x99);
        table << qRgb(0xFF, 0x00, 0x66);
        table << qRgb(0xFF, 0x00, 0x33);
        table << qRgb(0xFF, 0x00, 0x00);
        table << qRgb(0xCC, 0xFF, 0xFF);
        table << qRgb(0xCC, 0xFF, 0xCC);
        table << qRgb(0xCC, 0xFF, 0x99);
        table << qRgb(0xCC, 0xFF, 0x66);
        table << qRgb(0xCC, 0xFF, 0x33);
        table << qRgb(0xCC, 0xFF, 0x00);
        table << qRgb(0xCC, 0xCC, 0xFF);
        table << qRgb(0xCC, 0xCC, 0xCC);
        table << qRgb(0xCC, 0xCC, 0x99);
        table << qRgb(0xCC, 0xCC, 0x66);
        table << qRgb(0xCC, 0xCC, 0x33);
        table << qRgb(0xCC, 0xCC, 0x00);
        table << qRgb(0xCC, 0x99, 0xFF);
        table << qRgb(0xCC, 0x99, 0xCC);
        table << qRgb(0xCC, 0x99, 0x99);
        table << qRgb(0xCC, 0x99, 0x66);
        table << qRgb(0xCC, 0x99, 0x33);
        table << qRgb(0xCC, 0x99, 0x00);
        table << qRgb(0xCC, 0x66, 0xFF);
        table << qRgb(0xCC, 0x66, 0xCC);
        table << qRgb(0xCC, 0x66, 0x99);
        table << qRgb(0xCC, 0x66, 0x66);
        table << qRgb(0xCC, 0x66, 0x33);
        table << qRgb(0xCC, 0x66, 0x00);
        table << qRgb(0xCC, 0x33, 0xFF);
        table << qRgb(0xCC, 0x33, 0xCC);
        table << qRgb(0xCC, 0x33, 0x99);
        table << qRgb(0xCC, 0x33, 0x66);
        table << qRgb(0xCC, 0x33, 0x33);
        table << qRgb(0xCC, 0x33, 0x00);
        table << qRgb(0xCC, 0x00, 0xFF);
        table << qRgb(0xCC, 0x00, 0xCC);
        table << qRgb(0xCC, 0x00, 0x99);
        table << qRgb(0xCC, 0x00, 0x66);
        table << qRgb(0xCC, 0x00, 0x33);
        table << qRgb(0xCC, 0x00, 0x00);
        table << qRgb(0x99, 0xFF, 0xFF);
        table << qRgb(0x99, 0xFF, 0xCC);
        table << qRgb(0x99, 0xFF, 0x99);
        table << qRgb(0x99, 0xFF, 0x66);
        table << qRgb(0x99, 0xFF, 0x33);
        table << qRgb(0x99, 0xFF, 0x00);
        table << qRgb(0x99, 0xCC, 0xFF);
        table << qRgb(0x99, 0xCC, 0xCC);
        table << qRgb(0x99, 0xCC, 0x99);
        table << qRgb(0x99, 0xCC, 0x66);
        table << qRgb(0x99, 0xCC, 0x33);
        table << qRgb(0x99, 0xCC, 0x00);
        table << qRgb(0x99, 0x99, 0xFF);
        table << qRgb(0x99, 0x99, 0xCC);
        table << qRgb(0x99, 0x99, 0x99);
        table << qRgb(0x99, 0x99, 0x66);
        table << qRgb(0x99, 0x99, 0x33);
        table << qRgb(0x99, 0x99, 0x00);
        table << qRgb(0x99, 0x66, 0xFF);
        table << qRgb(0x99, 0x66, 0xCC);
        table << qRgb(0x99, 0x66, 0x99);
        table << qRgb(0x99, 0x66, 0x66);
        table << qRgb(0x99, 0x66, 0x33);
        table << qRgb(0x99, 0x66, 0x00);
        table << qRgb(0x99, 0x33, 0xFF);
        table << qRgb(0x99, 0x33, 0xCC);
        table << qRgb(0x99, 0x33, 0x99);
        table << qRgb(0x99, 0x33, 0x66);
        table << qRgb(0x99, 0x33, 0x33);
        table << qRgb(0x99, 0x33, 0x00);
        table << qRgb(0x99, 0x00, 0xFF);
        table << qRgb(0x99, 0x00, 0xCC);
        table << qRgb(0x99, 0x00, 0x99);
        table << qRgb(0x99, 0x00, 0x66);
        table << qRgb(0x99, 0x00, 0x33);
        table << qRgb(0x99, 0x00, 0x00);
        table << qRgb(0x66, 0xFF, 0xFF);
        table << qRgb(0x66, 0xFF, 0xCC);
        table << qRgb(0x66, 0xFF, 0x99);
        table << qRgb(0x66, 0xFF, 0x66);
        table << qRgb(0x66, 0xFF, 0x33);
        table << qRgb(0x66, 0xFF, 0x00);
        table << qRgb(0x66, 0xCC, 0xFF);
        table << qRgb(0x66, 0xCC, 0xCC);
        table << qRgb(0x66, 0xCC, 0x99);
        table << qRgb(0x66, 0xCC, 0x66);
        table << qRgb(0x66, 0xCC, 0x33);
        table << qRgb(0x66, 0xCC, 0x00);
        table << qRgb(0x66, 0x99, 0xFF);
        table << qRgb(0x66, 0x99, 0xCC);
        table << qRgb(0x66, 0x99, 0x99);
        table << qRgb(0x66, 0x99, 0x66);
        table << qRgb(0x66, 0x99, 0x33);
        table << qRgb(0x66, 0x99, 0x00);
        table << qRgb(0x66, 0x66, 0xFF);
        table << qRgb(0x66, 0x66, 0xCC);
        table << qRgb(0x66, 0x66, 0x99);
        table << qRgb(0x66, 0x66, 0x66);
        table << qRgb(0x66, 0x66, 0x33);
        table << qRgb(0x66, 0x66, 0x00);
        table << qRgb(0x66, 0x33, 0xFF);
        table << qRgb(0x66, 0x33, 0xCC);
        table << qRgb(0x66, 0x33, 0x99);
        table << qRgb(0x66, 0x33, 0x66);
        table << qRgb(0x66, 0x33, 0x33);
        table << qRgb(0x66, 0x33, 0x00);
        table << qRgb(0x66, 0x00, 0xFF);
        table << qRgb(0x66, 0x00, 0xCC);
        table << qRgb(0x66, 0x00, 0x99);
        table << qRgb(0x66, 0x00, 0x66);
        table << qRgb(0x66, 0x00, 0x33);
        table << qRgb(0x66, 0x00, 0x00);
        table << qRgb(0x33, 0xFF, 0xFF);
        table << qRgb(0x33, 0xFF, 0xCC);
        table << qRgb(0x33, 0xFF, 0x99);
        table << qRgb(0x33, 0xFF, 0x66);
        table << qRgb(0x33, 0xFF, 0x33);
        table << qRgb(0x33, 0xFF, 0x00);
        table << qRgb(0x33, 0xCC, 0xFF);
        table << qRgb(0x33, 0xCC, 0xCC);
        table << qRgb(0x33, 0xCC, 0x99);
        table << qRgb(0x33, 0xCC, 0x66);
        table << qRgb(0x33, 0xCC, 0x33);
        table << qRgb(0x33, 0xCC, 0x00);
        table << qRgb(0x33, 0x99, 0xFF);
        table << qRgb(0x33, 0x99, 0xCC);
        table << qRgb(0x33, 0x99, 0x99);
        table << qRgb(0x33, 0x99, 0x66);
        table << qRgb(0x33, 0x99, 0x33);
        table << qRgb(0x33, 0x99, 0x00);
        table << qRgb(0x33, 0x66, 0xFF);
        table << qRgb(0x33, 0x66, 0xCC);
        table << qRgb(0x33, 0x66, 0x99);
        table << qRgb(0x33, 0x66, 0x66);
        table << qRgb(0x33, 0x66, 0x33);
        table << qRgb(0x33, 0x66, 0x00);
        table << qRgb(0x33, 0x33, 0xFF);
        table << qRgb(0x33, 0x33, 0xCC);
        table << qRgb(0x33, 0x33, 0x99);
        table << qRgb(0x33, 0x33, 0x66);
        table << qRgb(0x33, 0x33, 0x33);
        table << qRgb(0x33, 0x33, 0x00);
        table << qRgb(0x33, 0x00, 0xFF);
        table << qRgb(0x33, 0x00, 0xCC);
        table << qRgb(0x33, 0x00, 0x99);
        table << qRgb(0x33, 0x00, 0x66);
        table << qRgb(0x33, 0x00, 0x33);
        table << qRgb(0x33, 0x00, 0x00);
        table << qRgb(0x00, 0xFF, 0xFF);
        table << qRgb(0x00, 0xFF, 0xCC);
        table << qRgb(0x00, 0xFF, 0x99);
        table << qRgb(0x00, 0xFF, 0x66);
        table << qRgb(0x00, 0xFF, 0x33);
        table << qRgb(0x00, 0xFF, 0x00);
        table << qRgb(0x00, 0xCC, 0xFF);
        table << qRgb(0x00, 0xCC, 0xCC);
        table << qRgb(0x00, 0xCC, 0x99);
        table << qRgb(0x00, 0xCC, 0x66);
        table << qRgb(0x00, 0xCC, 0x33);
        table << qRgb(0x00, 0xCC, 0x00);
        table << qRgb(0x00, 0x99, 0xFF);
        table << qRgb(0x00, 0x99, 0xCC);
        table << qRgb(0x00, 0x99, 0x99);
        table << qRgb(0x00, 0x99, 0x66);
        table << qRgb(0x00, 0x99, 0x33);
        table << qRgb(0x00, 0x99, 0x00);
        table << qRgb(0x00, 0x66, 0xFF);
        table << qRgb(0x00, 0x66, 0xCC);
        table << qRgb(0x00, 0x66, 0x99);
        table << qRgb(0x00, 0x66, 0x66);
        table << qRgb(0x00, 0x66, 0x33);
        table << qRgb(0x00, 0x66, 0x00);
        table << qRgb(0x00, 0x33, 0xFF);
        table << qRgb(0x00, 0x33, 0xCC);
        table << qRgb(0x00, 0x33, 0x99);
        table << qRgb(0x00, 0x33, 0x66);
        table << qRgb(0x00, 0x33, 0x33);
        table << qRgb(0x00, 0x33, 0x00);
        table << qRgb(0x00, 0x00, 0xFF);
        table << qRgb(0x00, 0x00, 0xCC);
        table << qRgb(0x00, 0x00, 0x99);
        table << qRgb(0x00, 0x00, 0x66);
        table << qRgb(0x00, 0x00, 0x33);
        table << qRgb(0xEE, 0x00, 0x00);
        table << qRgb(0xDD, 0x00, 0x00);
        table << qRgb(0xBB, 0x00, 0x00);
        table << qRgb(0xAA, 0x00, 0x00);
        table << qRgb(0x88, 0x00, 0x00);
        table << qRgb(0x77, 0x00, 0x00);
        table << qRgb(0x55, 0x00, 0x00);
        table << qRgb(0x44, 0x00, 0x00);
        table << qRgb(0x22, 0x00, 0x00);
        table << qRgb(0x11, 0x00, 0x00);
        table << qRgb(0x00, 0xEE, 0x00);
        table << qRgb(0x00, 0xDD, 0x00);
        table << qRgb(0x00, 0xBB, 0x00);
        table << qRgb(0x00, 0xAA, 0x00);
        table << qRgb(0x00, 0x88, 0x00);
        table << qRgb(0x00, 0x77, 0x00);
        table << qRgb(0x00, 0x55, 0x00);
        table << qRgb(0x00, 0x44, 0x00);
        table << qRgb(0x00, 0x22, 0x00);
        table << qRgb(0x00, 0x11, 0x00);
        table << qRgb(0x00, 0x00, 0xEE);
        table << qRgb(0x00, 0x00, 0xDD);
        table << qRgb(0x00, 0x00, 0xBB);
        table << qRgb(0x00, 0x00, 0xAA);
        table << qRgb(0x00, 0x00, 0x88);
        table << qRgb(0x00, 0x00, 0x77);
        table << qRgb(0x00, 0x00, 0x55);
        table << qRgb(0x00, 0x00, 0x44);
        table << qRgb(0x00, 0x00, 0x22);
        table << qRgb(0x00, 0x00, 0x11);
        table << qRgb(0xEE, 0xEE, 0xEE);
        table << qRgb(0xDD, 0xDD, 0xDD);
        table << qRgb(0xBB, 0xBB, 0xBB);
        table << qRgb(0xAA, 0xAA, 0xAA);
        table << qRgb(0x88, 0x88, 0x88);
        table << qRgb(0x77, 0x77, 0x77);
        table << qRgb(0x55, 0x55, 0x55);
        table << qRgb(0x44, 0x44, 0x44);
        table << qRgb(0x22, 0x22, 0x22);
        table << qRgb(0x11, 0x11, 0x11);
        table << qRgb(0x00, 0x00, 0x00);
        break;
    }
    default:
        qWarning("QIcnsHandler::getColorTable(): No color table for bit depth: %u", depth);
    }
    return table;
}

bool QIcnsHandler::addIcon(IcnsIconEntry &icon)
{
    if (icon.isValid()) {
        switch(icon.mask()) {
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
    else {
        qWarning("QIcnsHandler::addIcon(): Unable to parse icon, OSType: %u", icon.getOSType());
    }
    return icon.isValid();
}

void QIcnsHandler::scanDevice()
{
    if (!m_stream.device()->seek(0)) {
        m_scanstate = IcnsFileParsingError;
        return;
    }
    IcnsBlockHeader blockHeader;
    while (!m_stream.atEnd()) {
        m_stream >> blockHeader;
        if (m_stream.status() != QDataStream::Ok) {
            m_scanstate = IcnsFileParsingError;
            return;
        }
        switch (blockHeader.OSType) {
        case OSType_icns:
            if (m_stream.device()->size() != blockHeader.length) {
                m_scanstate = IcnsFileParsingError;
                return;
            }
            break;
        case OSType_icnV:
            m_stream.skipRawData(4);
            break;
        case OSType_TOC_: {
            QVector<IcnsBlockHeader> toc;
            const quint32 tocEntriesCount = (blockHeader.length - IcnsBlockHeaderSize) / IcnsBlockHeaderSize;
            for (uint i = 0; i < tocEntriesCount; i++) {
                IcnsBlockHeader tocEntry;
                m_stream >> tocEntry;
                toc << tocEntry;
                quint32 imgDataOffset = blockHeader.length + IcnsBlockHeaderSize;
                for (uint n = 0; n < i; n++)
                    imgDataOffset += toc.at(n).length;
                imgDataOffset += IcnsBlockHeaderSize;
                IcnsIconEntry icon(tocEntry,imgDataOffset);
                addIcon(icon);
            }
            m_scanstate = IcnsFileIsParsed;
            return; // TOC scan gives enough data to discard scan of other blocks
        }
        default: {
            IcnsIconEntry icon(blockHeader,m_stream.device()->pos());
            addIcon(icon);
            m_stream.skipRawData(icon.dataLength());
            break;
        }
        }
    }
    m_scanstate = IcnsFileIsParsed;
}

QImage QIcnsHandler::iconAlpha(int index)
{
    QImage img;
    IcnsIconEntry icon = m_icons.at(index);
    if (icon.width() > 0 && icon.height() > 0) {
        bool hasMask = false;
        bool isMask = false;
        int maskIndex;
        if (icon.mask() == IconPlusMask) {
            isMask = true;
            hasMask = true;
        }
        else {
            IconBitDepth targetDepth = (icon.depth() == Icon32bit) ? Icon8bit : IconMono;
            for (int i = 0; i < m_masks.size(); i++) {
                bool suitable = m_masks.at(i).group() == icon.group() ||
                        (m_masks.at(i).height() == icon.height() &&
                         m_masks.at(i).width() == icon.width());
                if (suitable) {
                    if (m_masks.at(i).depth() == targetDepth) {
                        maskIndex = i;
                        hasMask = true;
                    }
                }
            }
        }
        if (hasMask) {
            IcnsIconEntry mask = isMask ? icon : m_masks.at(maskIndex);
            if (mask.depth() != IconMono && mask.depth() != Icon8bit) {
                qWarning() << "IcnsReader::getA8MaskForIcon(): Mask has unusual bit depth, can't read:"
                           << mask.depth() << "OSType:" << mask.getOSType();
                return img;
            }
            const quint32 width = mask.width();
            const quint32 height = mask.height();
            const quint32 pixelCount = width * height;
            const float bytesPerPixel = ((float)mask.depth() / 8);
            const quint32 imageDataSize = pixelCount * bytesPerPixel;
            const quint32 basePos = mask.dataOffset();
            const quint32 pos = (mask.mask() == IconPlusMask) ? (basePos + imageDataSize) : basePos;
            const qint64 oldPos = m_stream.device()->pos();
            if (m_stream.device()->seek(pos)) {
                img = QImage(width, height, QImage::Format_RGB32);
                quint8 byte = 0;
                quint32 pixel = 0;
                for (quint32 y = 0; y < height; y++) {
                    for (quint32 x = 0; x < width; x++) {
                        if (mask.depth() == IconMono) {
                            if (pixel % 8 == 0)
                                m_stream >> byte;
                            quint8 alpha = (byte >> 7) * 0xFF;
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

QIcnsHandler::IcnsIconEntry::IcnsIconEntry()
{
    IcnsBlockHeader h;
    h.OSType = 0;
    h.length = 0;
    m_header = h;
    m_iconGroup = IconGroupUnk;
    m_iconDepth = IconDepthUnk;
    m_iconMaskType = IconMaskUnk;
    m_iconWidth = 0;
    m_iconHeight = 0;
    m_iconIsParsed = false;
    m_imageDataLength = 0;
    m_imageDataOffset = 0;
    m_imageDataIsRLE = false;
}

QIcnsHandler::IcnsIconEntry::IcnsIconEntry(IcnsBlockHeader &header, quint32 imgDataOffset)
{
    // Header:
    m_header.OSType = header.OSType;
    m_header.length = header.length;
    // Image data:
    m_imageDataOffset = imgDataOffset;
    m_imageDataLength = header.length - IcnsBlockHeaderSize;
    m_imageDataIsRLE = false; // Always false unless spotted
    // Parse everything else:
    m_iconIsParsed = parseOSType();
}

bool QIcnsHandler::IcnsIconEntry::parseOSType()
{
    const QByteArray OSType = QByteArray::fromHex(QByteArray::number(m_header.OSType,16));
    // Typical OSType naming: <junk><group><depth><mask>;
#if QT_VERSION >= 0x050000
    const char* pattern = "^(?<junk>[\\D]{0,4})(?<group>[a-z|A-Z]{1})(?<depth>\\d{0,2})(?<mask>[#mk]{0,2})$";
    QRegularExpression regexp(pattern);
    QRegularExpressionMatch match = regexp.match(OSType);
    const bool hasMatch = match.hasMatch();
    const QString junk = match.captured("junk");
    const QString group = match.captured("group");
    const QString depth = match.captured("depth");
    const QString mask = match.captured("mask");
#else
    const char* pattern = "^([\\D]{0,4})([a-z|A-Z]{1})(\\d{0,2})([#mk]{0,2})$";
    QRegExp regexp(pattern);
    const bool hasMatch = (regexp.indexIn(OSType) >= 0);
    QStringList match = regexp.capturedTexts();
    const QString junk = (1 <= match.size()) ? match.at(1) : "";
    const QString group = (2 <= match.size()) ? match.at(2) : "";
    const QString depth = (3 <= match.size()) ? match.at(3) : "";
    const QString mask = (4 <= match.size()) ? match.at(4) : "";
#endif
    // Icon group:
    m_iconGroup = group.isEmpty() ? IconGroupUnk : IconGroup(group.at(0).toLatin1());
    // Icon depth:
    m_iconDepth = depth.toUInt() > 0 ? IconBitDepth(depth.toUInt()) : IconMono;
    // Width/height/mask:
    m_iconWidth = 0; // default for invalid ones
    m_iconHeight = 0; // default for invalid ones
    m_iconMaskType = IconMaskUnk; // default for invalid ones
    if (m_iconGroup != IconGroupCompressed) {
        const float bytespp = ((float)m_iconDepth / 8);
        const qreal r1 = sqrt(m_imageDataLength/bytespp);
        const qreal r2 = sqrt((m_imageDataLength/bytespp)/2);
        const quint32 r1u = (quint32)r1;
        const quint32 r2u = (quint32)r2;
        const bool r1IsPowerOfTwoOrDevidesBy16 = (r1u == r1 && r1u % 16 == 0) || (r1u == r1 && r1 >= 16 && ((r1u & (r1u - 1)) == 0));
        const bool r2IsPowerOfTwoOrDevidesBy16 = (r2u == r2 && r2u % 16 == 0) || (r2u == r2 && r2 >= 16 && ((r2u & (r2u - 1)) == 0));

        if (r1IsPowerOfTwoOrDevidesBy16) {
            m_iconMaskType = mask.isEmpty() ? IconNoMask : IconIsMask;
            m_iconWidth = r1;
            m_iconHeight = r1;
        }
        else if (r2IsPowerOfTwoOrDevidesBy16) {
            m_iconMaskType = IconPlusMask;
            m_iconWidth = r2;
            m_iconHeight = r2;
        }
        else if (m_iconGroup == IconGroupMini) {
            // Legacy 16x12 icons are an exception from the generic square formula
            m_iconMaskType = (m_imageDataLength == 192*bytespp*2) ? IconPlusMask : IconNoMask;
            m_iconWidth = 16;
            m_iconHeight = 12;
        }
        else {
            if (m_iconDepth == Icon32bit) {
                // 32bit icon may be compressed with RLE24.
                // TODO: Find a way to drop hardcoded values?
                m_imageDataIsRLE = true;
                switch(m_iconGroup) {
                case IconGroupSmall :
                    m_iconWidth = 16;
                    m_iconHeight = 16;
                    break;
                case IconGroupLarge :
                    m_iconWidth = 32;
                    m_iconHeight = 32;
                    break;
                case IconGroupHuge :
                    m_iconWidth = 48;
                    m_iconHeight = 48;
                    break;
                case IconGroupThumbnail :
                    m_iconWidth = 128;
                    m_iconHeight = 128;
                    break;
                default :
                    qWarning() << "IcnsIconEntry::parseOSType(): 32bit icon from an unknown group. OSType:"
                               << OSType.constData();
                }
            }
        }
    }
    else {
        // Just for experimental/research purposes.
        // Effectively does nothing at all, just tests Apple's naming policy for OSTypes.
        if (m_iconDepth <= 10) {
            m_iconWidth = pow(2,m_iconDepth);
            m_iconHeight = pow(2,m_iconDepth);
        }
        else {
            qDebug() << "IcnsIconEntry::parseOSType(): Compressed format id > 10 (retina?). OSType:" << OSType.constData();
        }
    }
    if (!hasMatch) {
        qWarning() << "IcnsIconEntry::parseOSType(): Parsing failed, ignored. Reg exp: no match for OSType:" << OSType.constData();
    }
    return hasMatch;
}
