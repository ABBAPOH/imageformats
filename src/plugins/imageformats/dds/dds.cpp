#include "dds.h"
#include "ddsheader.h"
#include "dxt.h"

#include <QtGui/QImage>

#include <QDebug>
#include <qendian.h>

// All magic numbers are little-endian as long as dds format has little
// endian byte order
static const quint32 ddsMagic = 0x20534444; // "DDS "

static const quint32 dxt1Magic = 0x31545844; // "DXT1"
static const quint32 dxt2Magic = 0x32545844; // "DXT2"
static const quint32 dxt3Magic = 0x33545844; // "DXT3"
static const quint32 dxt4Magic = 0x34545844; // "DXT4"
static const quint32 dxt5Magic = 0x35545844; // "DXT5"

static const qint64 headerSize = 128;

static int faceOffset[6][2] = { {2, 1}, {0, 1}, {1, 0}, {1, 2}, {1, 1}, {3, 1} };
static int faceFlags[6] = {
    DDSHeader::DDSCAPS2_CUBEMAP_POSITIVEX,
    DDSHeader::DDSCAPS2_CUBEMAP_NEGATIVEX,
    DDSHeader::DDSCAPS2_CUBEMAP_POSITIVEY,
    DDSHeader::DDSCAPS2_CUBEMAP_NEGATIVEY,
    DDSHeader::DDSCAPS2_CUBEMAP_POSITIVEZ,
    DDSHeader::DDSCAPS2_CUBEMAP_NEGATIVEZ
};

enum Colors {
    Red = 0,
    Green,
    Blue,
    Alpha,
    ColorCount
};

enum Type {
    TypeUnknown = 0,
    TypeDXT1,
    TypeDXT2,
    TypeDXT3,
    TypeDXT4,
    TypeDXT5,
    TypeRGB,
    TypeRGBA,
    TypeAlpha,
    TypeYUV,
    TypeLuminance,
    TypeLuminanceAlpha,
    TypeIndexed8,
    TypeCount
};

static int shift(quint32 mask)
{
    if (mask == 0)
        return 0;

    int result = 0;
    while (!((mask >> result) & 1))
        result++;
    return result;
}

static int bits(quint32 mask)
{
    int result = 0;
    while (mask) {
       if (mask & 1)
           result++;
       mask >>= 1;
    }
    return result;
}

static quint32 readValue(QDataStream &s, quint32 size)
{
    Q_ASSERT(size == 8 || size == 16 || size == 24 || size == 32);

    quint32 value = 0;
    for (unsigned bit = 0; bit < size/8; ++bit) {
        quint8 tmp;
        s >> tmp;
        value = value + (quint32(tmp) << 8*bit);
    }
    return value;
}

static inline bool hasAlpha(const DDSHeader &dds)
{
    quint32 flags = dds.pixelFormat.flags;
    return flags & DDSPixelFormat::DDPF_ALPHAPIXELS ||
            flags & DDSPixelFormat::DDPF_ALPHA;
}

static inline bool isCubeMap(const DDSHeader &dds)
{
    return dds.caps2 & DDSHeader::DDSCAPS2_CUBEMAP;
}

static Type getType(const DDSHeader &dds)
{
    quint32 flags = dds.pixelFormat.flags;
    if (flags & DDSPixelFormat::DDPF_FOURCC) {
        switch (dds.pixelFormat.fourCC) {
        case dxt1Magic:
            return TypeDXT1;
        case dxt2Magic:
            return TypeDXT2;
        case dxt3Magic:
            return TypeDXT3;
        case dxt4Magic:
            return TypeDXT4;
        case dxt5Magic:
            return TypeDXT5;
        default:
            break;
        }
    }

    bool hasAlpha = ::hasAlpha(dds);

    if (flags & DDSPixelFormat::DDPF_RGB && hasAlpha)
        return TypeRGBA;
    else if (flags & DDSPixelFormat::DDPF_RGB && !hasAlpha)
        return TypeRGB;
    else if (flags & DDSPixelFormat::DDPF_YUV)
        return TypeYUV;
    else if (flags & DDSPixelFormat::DDPF_LUMINANCE && hasAlpha)
        return TypeLuminanceAlpha;
    else if (flags & DDSPixelFormat::DDPF_LUMINANCE && !hasAlpha)
        return TypeLuminance;
    else if (hasAlpha)
        return TypeAlpha;
    else if (flags & DDSPixelFormat::DDPF_PALETTEINDEXED8)
        return TypeIndexed8;

    return TypeUnknown;
}

DDSHandler::DDSHandler() :
    m_currentImage(0),
    m_headerCached(false)
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

static QImage readValueBased(QDataStream & s, const DDSHeader & dds, quint32 width, quint32 height, bool hasAlpha)
{
    quint32 flags = dds.pixelFormat.flags;

    quint32 masks[ColorCount];
    quint8 shifts[ColorCount];
    quint8 bits[ColorCount];
    masks[Red] = dds.pixelFormat.rBitMask;
    masks[Green] = dds.pixelFormat.gBitMask;
    masks[Blue] = dds.pixelFormat.bBitMask;
    masks[Alpha] = hasAlpha ? dds.pixelFormat.aBitMask : 0;
    for (int i = 0; i < ColorCount; ++i) {
        shifts[i] = ::shift(masks[i]);
        bits[i] = ::bits(masks[i]);

        // move mask to the left
        if (bits[i] <= 8)
            masks[i] = (masks[i] >> shifts[i]) << (8 - bits[i]);
    }

    const QImage::Format format = hasAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32;

    QImage img(width, height, format);

    for (quint32 y = 0; y < height; y++) {
        for (quint32 x = 0; x < width; x++) {
            quint32 value = ::readValue(s, dds.pixelFormat.rgbBitCount);
            quint8 colors[ColorCount];

            for (int c = 0; c < ColorCount; ++c) {
                if (bits[c] > 8) {
                    // truncate unneseccary bits
                    colors[c] = (value & masks[c]) >> shifts[c] >> (bits[c] - 8);
                } else {
                    // move color to the left
                    quint8 color = value >> shifts[c] << (8 - bits[c]) & masks[c];
                    if (color)
                        colors[c] = color * 0xff / masks[c];
                    else
                        colors[c] = 0;
                }
            }

            if ( (flags & DDSPixelFormat::DDPF_LUMINANCE) ) {
                colors[Green] = colors[Red];
                colors[Blue] = colors[Red];
            } else if ( (flags & DDSPixelFormat::DDPF_YUV) ) {
                quint8 Y = colors[Red];
                quint8 U = colors[Green];
                quint8 V = colors[Blue];
                colors[Red] = Y + 1.13983 * (V - 128);
                colors[Green] = Y - 0.39465 * (U - 128) - 0.58060 * (V - 128);
                colors[Blue] = Y + 2.03211 * (U - 128);
            }

            img.setPixel(x, y, qRgba(colors[Red], colors[Green], colors[Blue], colors[Alpha]));
        }
    }

    return img;
}

static QImage readPaletteBased(QDataStream & s, const DDSHeader &/*dds*/, quint32 width, quint32 height)
{
    QImage img(width, height, QImage::Format_Indexed8);
    for (int i = 0; i < 256; ++i) {
        quint8 r, g, b, a;
        s >> r >> g >> b >> a;
        img.setColor(i, qRgba(r, g, b, a));
    }

    for (quint32 y = 0; y < height; y++) {
        for (quint32 x = 0; x < width; x++) {
            quint8 index;
            s >> index;
            img.setPixel(x, y, index);
        }
    }

    return img;
}

QImage readLayer(QDataStream & s, const DDSHeader & dds, quint32 width, quint32 height)
{
    switch (getType(dds)) {
    case TypeDXT1:
        return QDXT::loadDXT1(s, width, height);
    case TypeDXT2:
        return QDXT::loadDXT2(s, width, height);
    case TypeDXT3:
        return QDXT::loadDXT3(s, width, height);
    case TypeDXT4:
        return QDXT::loadDXT4(s, width, height);
    case TypeDXT5:
        return QDXT::loadDXT5(s, width, height);
    case TypeRGB:
    case TypeYUV:
    case TypeLuminance:
        return readValueBased(s, dds, width, height, false);
    case TypeRGBA:
    case TypeAlpha:
    case TypeLuminanceAlpha:
        return readValueBased(s, dds, width, height, true);
    case TypeIndexed8:
        return readPaletteBased(s, dds, width, height);
    default:
        break;
    }

    return QImage();
}

QImage readTexture(QDataStream & s, const DDSHeader & dds, int mipmapLevel)
{
    quint32 width = dds.width / (1 << mipmapLevel);
    quint32 height = dds.height / (1 << mipmapLevel);
    return readLayer(s, dds, width, height);
}

static qint64 mipmapSize(const DDSHeader &dds, int level)
{
    quint32 w = dds.width/(1 << level);
    quint32 h = dds.height/(1 << level);

    Type type = getType(dds);
    switch (type) {
    case TypeDXT1:
        return ((w+3)/4)*((h+3)/4)*8;
    case TypeDXT2:
    case TypeDXT3:
    case TypeDXT4:
    case TypeDXT5:
        return ((w+3)/4)*((h+3)/4)*16;
    case TypeRGB:
    case TypeRGBA:
    case TypeAlpha:
    case TypeYUV:
    case TypeLuminance:
    case TypeLuminanceAlpha:
        return w*h*dds.pixelFormat.rgbBitCount/8;
    case TypeIndexed8:
        return 256 + w*h*8;
    default:
        break;
    }
    return 0;
}

static qint64 mipmapOffset(const DDSHeader &dds, int level)
{
    qint64 result = 0;
    for (int i = 0; i < level; ++i) {
        result += mipmapSize(dds, i);
    }
    return result;
}

QImage readCubeMap(QDataStream & s, const DDSHeader & dds)
{
    bool hasAlpha = ::hasAlpha(dds);
    QImage::Format format = hasAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32;
    QImage img(4 * dds.width, 3 * dds.height, format);

    img.fill(0);

    int offset = s.device()->pos();
    const int size = mipmapSize(dds, 0);

    for (int i = 0; i < 6; i++) {
        if (!(dds.caps2 & faceFlags[i]))
            continue; // Skip face.

        s.device()->seek(offset);
        offset += size;

        const QImage face = ::readLayer(s, dds, dds.width, dds.height);

        // Compute face offsets.
        int offset_x = faceOffset[i][0] * dds.width;
        int offset_y = faceOffset[i][1] * dds.height;

        // Copy face on the image.
        for (quint32 y = 0; y < dds.height; y++) {
            const QRgb *src = reinterpret_cast<const QRgb *>(face.scanLine(y));
            QRgb *dst = reinterpret_cast<QRgb *>(img.scanLine( y + offset_y )) + offset_x;
            memcpy(dst, src, sizeof(QRgb) * dds.width);
        }
    }

    return img;
}

bool DDSHandler::read(QImage *outImage)
{
    ensureHeaderCached();

    if (!device()->isSequential()) {
        qint64 pos = headerSize + mipmapOffset(header, m_currentImage);
        if (!device()->seek(pos))
            return false;
        QDataStream s(device());
        s.setByteOrder(QDataStream::LittleEndian);

        QImage img;
        if (isCubeMap(header))
            img = readCubeMap(s, header);
        else
            img = readTexture(s, header, m_currentImage);

        bool ok = s.status() == QDataStream::Ok && !img.isNull();
        if (ok)
            *outImage = img;
        return ok;
    }

    return true;
}

bool DDSHandler::write(const QImage &outImage)
{
    QDataStream s( device() );
    s.setByteOrder(QDataStream::LittleEndian);

    // Filling header
    DDSHeader dds;
    // Filling header
    dds.magic = ddsMagic;
    dds.size = 124;
    dds.flags = DDSHeader::DDSD_CAPS | DDSHeader::DDSD_HEIGHT |
                DDSHeader::DDSD_WIDTH | DDSHeader::DDSD_PIXELFORMAT;
    dds.height = outImage.height();
    dds.width = outImage.width();
    dds.pitchOrLinearSize = 128;
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

int DDSHandler::imageCount() const
{
    ensureHeaderCached();
    return qMax<quint32>(1, header.mipMapCount);
}

bool DDSHandler::jumpToImage(int imageNumber)
{
    if (imageNumber >= imageCount())
        return false;

    m_currentImage = imageNumber;
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

    if (device->isSequential())
        return false;

    return device->peek(4) == "DDS ";
}

void DDSHandler::ensureHeaderCached() const
{
    if (m_headerCached)
        return;

    if (device()->isSequential())
        return;

    qint64 oldPos = device()->pos();
    device()->seek(0);

    DDSHandler *that = const_cast<DDSHandler *>(this);
    QDataStream s(device());
    s.setByteOrder(QDataStream::LittleEndian);
    s >> that->header;

    device()->seek(oldPos);
}

// ===================== DDSPlugin =====================

#if QT_VERSION < 0x050000
QStringList DDSPlugin::keys() const
{
    return QStringList() << "dds";
}
#endif

QImageIOPlugin::Capabilities DDSPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (!device || !device->isOpen())
        return 0;
    if (format.toLower() != "dds")
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
