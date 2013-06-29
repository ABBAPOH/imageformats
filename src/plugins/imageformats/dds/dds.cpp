#include "dds.h"
#include "ddsheader.h"
#include "dxt.h"

#include <QtGui/QImage>

#include <QDebug>

DDSHandler::DDSHandler()
{
}

bool DDSHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("dds");
        return true;
    }
    return false;
}

int shift(quint32 mask)
{
    int result = 32;
    while (mask != 0) {
        result--;
        mask <<= 1;
    }
    return result;
}

bool readData(QDataStream & s, const DDSHeader & dds, QImage &img)
{
    quint32 flags = dds.pixelFormat.flags;
    if (flags & DDSPixelFormat::DDPF_FOURCC) {
            if (memcmp(&dds.pixelFormat.fourCC, "DXT1", 4) == 0)
            img = QDXT::loadDXT1(s, dds.width, dds.height);
        if (memcmp(&dds.pixelFormat.fourCC, "DXT3", 4) == 0)
            img = QDXT::loadDXT3(s, dds.width, dds.height);
        if (memcmp(&dds.pixelFormat.fourCC, "DXT5", 4) == 0)
            img = QDXT::loadDXT5(s, dds.width, dds.height);
    }

    quint32 rBitMask = dds.pixelFormat.rBitMask;
    quint32 gBitMask = dds.pixelFormat.gBitMask;
    quint32 bBitMask = dds.pixelFormat.bBitMask;
    int redShift = shift(rBitMask);
    int greenShift = shift(gBitMask);
    int blueShift = shift(bBitMask);
    quint32 aBitMask = 0;
    int alphaShift = 0;
    if (dds.pixelFormat.flags & DDSPixelFormat::DDPF_ALPHAPIXELS) {
        aBitMask = dds.pixelFormat.aBitMask;
        alphaShift = shift(aBitMask);
    }

    if (flags & DDSPixelFormat::DDPF_RGB) {
        if (dds.pixelFormat.rgbBitCount != 32) {
            qWarning() << "can't read image with bit count " << dds.pixelFormat.rgbBitCount;
            return false;
        }
        if (dds.pixelFormat.flags & DDSPixelFormat::DDPF_ALPHAPIXELS) {
            img = QImage(dds.width, dds.height, QImage::Format_ARGB32);
        } else {
            img = QImage(dds.width, dds.height, QImage::Format_RGB32);
        }
        for (quint32 i = 0; i < dds.width; i++)
            for (quint32 j = 0; j < dds.height; j++) {
                quint32 color;
                s >> color;
                int red = (color & rBitMask) >> redShift;
                int green = (color & gBitMask) >> greenShift;
                int blue = (color & bBitMask) >> blueShift;
                int alpha = (color & aBitMask) >> alphaShift;
                img.setPixel(j, i, qRgba(red, green, blue, alpha));
            }
    }
    return true;
}

bool DDSHandler::read(QImage *outImage)
{
    QDataStream s(device());
    s.setByteOrder(QDataStream::LittleEndian);

    // Read image header.
    DDSHeader dds;
    quint32 magic;
    s >> magic;
    s >> dds;

    QImage img;
    bool result = readData(s, dds, img);

    if (result == false) {
        qWarning() << "Error loading dds file.";
        return false;
    }

    *outImage = img;
    return true;
}

bool DDSHandler::write(const QImage &outImage)
{
    QDataStream s( device() );
    s.setByteOrder(QDataStream::LittleEndian);

    // Magic
    s << 'D';
    s << 'D';
    s << 'S';
    s << ' ';

    // Filling header
    DDSHeader dds;
    // Filling header
    dds.size = 124;
    dds.flags = DDSHeader::DDSD_CAPS | DDSHeader::DDSD_HEIGHT |
                DDSHeader::DDSD_WIDTH | DDSHeader::DDSD_PIXELFORMAT;
    dds.height = outImage.height();
    dds.width = outImage.width();
    dds.linearSize = 0;
    dds.depth = 0;
    dds.mipMapCount = 0;
    for (int i = 0; i< 11; i++) {
        dds.reserved1[i] = 0;
    }
    dds.caps = DDSHeader::DDSCAPS_TEXTURE;
    dds.caps2 = 0;
    dds.caps3 = 0;
    dds.caps4 = 0;
    dds.reserved2 = 0;

    // Filling pixelformat
    dds.pixelFormat.size = 32;
    dds.pixelFormat.flags = DDSPixelFormat::DDPF_ALPHAPIXELS | DDSPixelFormat::DDPF_RGB;
    dds.pixelFormat.fourCC = 0;
    dds.pixelFormat.rgbBitCount = 32;
    dds.pixelFormat.aBitMask = 0xff000000;
    dds.pixelFormat.rBitMask = 0x00ff0000;
    dds.pixelFormat.gBitMask = 0x0000ff00;
    dds.pixelFormat.bBitMask = 0x000000ff;

    s << dds;
    for (int width = 0; width < outImage.width(); width++)
        for (int height = 0; height < outImage.height(); height++) {
        QRgb pixel = outImage.pixel(height, width);;
        quint32 color;
        quint8 alpha = qAlpha(pixel);
        quint8 red = qRed(pixel);
        quint8 green = qGreen(pixel);
        quint8 blue = qBlue(pixel);
        color = (alpha << 24) + (red << 16) + (green << 8) + blue;
        s << color;

    }

    return true;
}

QByteArray DDSHandler::name() const
{
    return "dds";
}

bool DDSHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("DDSHandler::canRead() called with no device");
        return false;
    }

    return device->peek(4) == "DDS ";
}


// ===================== DDSPlugin =====================

QStringList DDSPlugin::keys() const
{
    return QStringList() << "dds" << "DDS";
}

QImageIOPlugin::Capabilities DDSPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "dds" || format == "DDS")
        return Capabilities(CanRead | CanWrite);
    if (!format.isEmpty())
        return 0;
    if (!device->isOpen())
        return 0;

    Capabilities cap;
    if (device->isReadable() && DDSHandler::canRead(device))
        cap |= CanRead;
    if (device->isWritable())
        cap |= CanWrite;
    return cap;
}

QImageIOHandler *DDSPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new DDSHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(DDSPlugin)
Q_EXPORT_PLUGIN2(dds, DDSPlugin)
#endif
