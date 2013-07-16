#include "dds.h"
#include "ddsheader.h"
#include "dxt.h"

#include <QtGui/QImage>

#include <QDebug>
#include <qendian.h>
#include <qmath.h>

enum Colors
{
    Red = 0,
    Green,
    Blue,
    Alpha,
    ColorCount
};

// All magic numbers are little-endian as long as dds format has little
// endian byte order
static const quint32 ddsMagic = 0x20534444; // "DDS "
static const quint32 dx10Magic = 0x30315844; // "DX10"

static const quint32 argb16Magic = 36; // D3DFMT_A16B16G16R16

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

struct FormatInfo
{
    Format format;
    quint32 flags;
    quint32 bitCount;
    quint32 rBitMask;
    quint32 gBitMask;
    quint32 bBitMask;
    quint32 aBitMask;
};

static const FormatInfo formatInfos [] = {
    { FORMAT_A8R8G8B8,    DDSPixelFormat::DDPF_RGBA, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 }, // 21
    { FORMAT_X8R8G8B8,    DDSPixelFormat::DDPF_RGB,  32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 }, // 22
    { FORMAT_A2B10G10R10, DDSPixelFormat::DDPF_RGBA, 32, 0x000003ff, 0x0000fc00, 0x3ff00000, 0xc0000000 }, // 31
    { FORMAT_A8B8G8R8,    DDSPixelFormat::DDPF_RGBA, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 }, // 32
    { FORMAT_X8B8G8R8,    DDSPixelFormat::DDPF_RGB,  32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0x00000000 }, // 33
    { FORMAT_G16R16,      DDSPixelFormat::DDPF_RGBA, 32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000 }, // 34
    { FORMAT_G16R16,      DDSPixelFormat::DDPF_RGB,  32, 0x0000ffff, 0xffff0000, 0x00000000, 0x00000000 }, // 34
    { FORMAT_A2R10G10B10, DDSPixelFormat::DDPF_RGBA, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000 }, // 35

    { FORMAT_R8G8B8,      DDSPixelFormat::DDPF_RGB,  24, 0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000 }, // 20

    { FORMAT_R5G6B5,      DDSPixelFormat::DDPF_RGB,  16, 0x0000f800, 0x000007e0, 0x0000001f, 0x00000000 }, // 23
    { FORMAT_X1R5G5B5,    DDSPixelFormat::DDPF_RGB,  16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00000000 }, // 24
    { FORMAT_A1R5G5B5,    DDSPixelFormat::DDPF_RGBA, 16, 0x00007c00, 0x000003e0, 0x0000001f, 0x00008000 }, // 25
    { FORMAT_A4R4G4B4,    DDSPixelFormat::DDPF_RGBA, 16, 0x00000f00, 0x000000f0, 0x0000000f, 0x0000f000 }, // 26
    { FORMAT_A8R3G3B2,    DDSPixelFormat::DDPF_RGBA, 16, 0x000000e0, 0x0000001c, 0x00000003, 0x0000ff00 }, // 29
    { FORMAT_X4R4G4B4,    DDSPixelFormat::DDPF_RGB,  16, 0x00000f00, 0x000000f0, 0x0000000f, 0x00000000 }, // 30
    { FORMAT_A8L8,        DDSPixelFormat::DDPF_LA,   16, 0x000000ff, 0x00000000, 0x00000000, 0x0000ff00 }, // 51
    { FORMAT_L16,   DDSPixelFormat::DDPF_LUMINANCE,  16, 0x0000ffff, 0x00000000, 0x00000000, 0x00000000 }, // 81

    { FORMAT_R3G3B2,      DDSPixelFormat::DDPF_RGB,  8,  0x000000e0, 0x0000001c, 0x00000003, 0x00000000 }, // 27
    { FORMAT_A8,        DDSPixelFormat::DDPF_ALPHA,  8,  0x00000000, 0x00000000, 0x00000000, 0x000000ff }, // 28
    { FORMAT_L8,    DDSPixelFormat::DDPF_LUMINANCE,  8,  0x000000ff, 0x00000000, 0x00000000, 0x00000000 }, // 50
    { FORMAT_A4L4,        DDSPixelFormat::DDPF_LA,   8,  0x0000000f, 0x00000000, 0x00000000, 0x000000f0 }, // 52
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

static Format getFormat(const DDSHeader &dds)
{
    const DDSPixelFormat &format = dds.pixelFormat;
    if (format.flags & DDSPixelFormat::DDPF_FOURCC) {
        switch (dds.pixelFormat.fourCC) {
        case FORMAT_DXT1:
            return FORMAT_DXT1;
        case FORMAT_DXT2:
            return FORMAT_DXT2;
        case FORMAT_DXT3:
            return FORMAT_DXT3;
        case FORMAT_DXT4:
            return FORMAT_DXT4;
        case FORMAT_DXT5:
            return FORMAT_DXT5;
        case FORMAT_R16F:
            return FORMAT_R16F;
        case FORMAT_G16R16F:
            return FORMAT_G16R16F;
        case FORMAT_A16B16G16R16F:
            return FORMAT_A16B16G16R16F;
        default:
            return FORMAT_UNKNOWN;
        }
    } else {
        size_t count = sizeof(formatInfos)/sizeof(FormatInfo);
        for (size_t i = 0; i < count; ++i) {
            const FormatInfo &info = formatInfos[i];
            if ( (format.flags & info.flags) == info.flags &&
                 format.rgbBitCount == info.bitCount &&
                 format.rBitMask == info.rBitMask &&
                 format.bBitMask == info.bBitMask &&
                 format.bBitMask == info.bBitMask &&
                 format.aBitMask == info.aBitMask)
                return info.format;
        }
    }

    return FORMAT_UNKNOWN;
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

static double readFloat16(QDataStream &s)
{
    quint16 value;
    s >> value;

    double sign = (value & 0x8000) == 0x8000 ? -1.0 : 1.0;
    qint8 exp = (value & 0x7C00) >> 10;
    quint16 fraction = value & 0x3FF;

    if (exp == 0)
        return sign*qPow(2.0, -14.0)*fraction/1024.0;
    else
        return sign*qPow(2.0, exp - 15)*(1 + fraction/1024.0);
}

static QImage loadR16F(QDataStream &s, const quint32 width, const quint32 height)
{
    QImage img(width, height, QImage::Format_RGB32);

    for (quint32 y = 0; y < height; y++) {
        for (quint32 x = 0; x < width; x++) {
            quint8 r = readFloat16(s) * 255;
            img.setPixel(x, y, qRgba(r, 0, 0, 0));
        }
    }

    return img;
}

static QImage loadRG16F(QDataStream &s, const quint32 width, const quint32 height)
{
    QImage img(width, height, QImage::Format_RGB32);

    for (quint32 y = 0; y < height; y++) {
        for (quint32 x = 0; x < width; x++) {
            quint8 r = readFloat16(s) * 255;
            quint8 g = readFloat16(s) * 255;
            img.setPixel(x, y, qRgba(r, g, 0, 0));
        }
    }

    return img;
}

static QImage loadARGB16F(QDataStream &s, const quint32 width, const quint32 height)
{
    QImage img(width, height, QImage::Format_ARGB32);

    for (quint32 y = 0; y < height; y++) {
        for (quint32 x = 0; x < width; x++) {
            quint8 colors[ColorCount];
            for (int c = 0; c < ColorCount; ++c)
                colors[c] = readFloat16(s) * 255;

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

QImage readLayer(QDataStream & s, const DDSHeader & dds, const int format, quint32 width, quint32 height)
{
    switch (format) {
    case FORMAT_R8G8B8:
    case FORMAT_X8R8G8B8:
    case FORMAT_R5G6B5:
    case FORMAT_R3G3B2:
    case FORMAT_X1R5G5B5:
    case FORMAT_X4R4G4B4:
    case FORMAT_X8B8G8R8:
    case FORMAT_G16R16:
    case FORMAT_L8:
    case FORMAT_L16:
        return readValueBased(s, dds, width, height, false);
    case FORMAT_A8R8G8B8:
    case FORMAT_A1R5G5B5:
    case FORMAT_A4R4G4B4:
    case FORMAT_A8:
    case FORMAT_A8R3G3B2:
    case FORMAT_A2B10G10R10:
    case FORMAT_A8B8G8R8:
    case FORMAT_A2R10G10B10:
    case FORMAT_A8L8:
    case FORMAT_A4L4:
        return readValueBased(s, dds, width, height, true);
    case FORMAT_P8:
        return readPaletteBased(s, dds, width, height);
    case FORMAT_A8P8:
    case FORMAT_A16B16G16R16:
    case FORMAT_V8U8:
    case FORMAT_L6V5U5:
    case FORMAT_X8L8V8U8:
    case FORMAT_Q8W8V8U8:
    case FORMAT_V16U16:
    case FORMAT_A2W10V10U10:
//    case FORMAT_UYVY:
//    case FORMAT_R8G8_B8G8:
//    case FORMAT_YUY2:
//    case FORMAT_G8R8_G8B8:
//    default:
        break;
    case FORMAT_DXT1:
        return QDXT::loadDXT1(s, width, height);
    case FORMAT_DXT2:
        return QDXT::loadDXT2(s, width, height);
    case FORMAT_DXT3:
        return QDXT::loadDXT3(s, width, height);
    case FORMAT_DXT4:
        return QDXT::loadDXT4(s, width, height);
    case FORMAT_DXT5:
        return QDXT::loadDXT5(s, width, height);
    case FORMAT_R16F:
        return loadR16F(s, width, height);
    case FORMAT_G16R16F:
        return loadRG16F(s, width, height);
    case FORMAT_A16B16G16R16F:
        return loadARGB16F(s, width, height);
//    case FORMAT_D16_LOCKABLE:
//    case FORMAT_D32:
//    case FORMAT_D15S1:
//    case FORMAT_D24S8:
//    case FORMAT_D24X8:
//    case FORMAT_D24X4S4:
//    case FORMAT_D16:
//    case FORMAT_D32F_LOCKABLE:
//    case FORMAT_D24FS8:
//    case FORMAT_D32_LOCKABLE:
//    case FORMAT_S8_LOCKABLE:
//    case FORMAT_VERTEXDATA:
//    case FORMAT_INDEX16:
//    case FORMAT_INDEX32:
    case FORMAT_Q16W16V16U16:
//    case FORMAT_MULTI2_ARGB8:
    case FORMAT_R32F:
    case FORMAT_G32R32F:
    case FORMAT_A32B32G32R32F:
    case FORMAT_CxV8U8:
//    case FORMAT_A1:
//    case FORMAT_A2B10G10R10_XR_BIAS:
//    case FORMAT_BINARYBUFFER:
    case FORMAT_LAST:
        break;
    }

    return QImage();
}

QImage readTexture(QDataStream & s, const DDSHeader & dds, const int format, const int mipmapLevel)
{
    quint32 width = dds.width / (1 << mipmapLevel);
    quint32 height = dds.height / (1 << mipmapLevel);
    return readLayer(s, dds, format, width, height);
}

static qint64 mipmapSize(const DDSHeader &dds, const int format, const int level)
{
    quint32 w = dds.width/(1 << level);
    quint32 h = dds.height/(1 << level);

    switch (format) {
    case FORMAT_R8G8B8:
    case FORMAT_X8R8G8B8:
    case FORMAT_R5G6B5:
    case FORMAT_X1R5G5B5:
    case FORMAT_X4R4G4B4:
    case FORMAT_X8B8G8R8:
    case FORMAT_G16R16:
    case FORMAT_L8:
    case FORMAT_L16:
        return w*h*dds.pixelFormat.rgbBitCount/8;
    case FORMAT_A8R8G8B8:
    case FORMAT_A1R5G5B5:
    case FORMAT_A4R4G4B4:
    case FORMAT_A8:
    case FORMAT_A8R3G3B2:
    case FORMAT_A2B10G10R10:
    case FORMAT_A8B8G8R8:
    case FORMAT_A2R10G10B10:
    case FORMAT_A8L8:
    case FORMAT_A4L4:
        return w*h*dds.pixelFormat.rgbBitCount/8;
    case FORMAT_P8:
        return 256 + w*h*8;
    case FORMAT_A8P8:
    case FORMAT_A16B16G16R16:
    case FORMAT_V8U8:
    case FORMAT_L6V5U5:
    case FORMAT_X8L8V8U8:
    case FORMAT_Q8W8V8U8:
    case FORMAT_V16U16:
    case FORMAT_A2W10V10U10:
//    case FORMAT_UYVY:
//    case FORMAT_R8G8_B8G8:
//    case FORMAT_YUY2:
//    case FORMAT_G8R8_G8B8:
//    default:
        break;
    case FORMAT_DXT1:
        return ((w+3)/4)*((h+3)/4)*8;
    case FORMAT_DXT2:
    case FORMAT_DXT3:
    case FORMAT_DXT4:
    case FORMAT_DXT5:
        return ((w+3)/4)*((h+3)/4)*16;
//    case FORMAT_D16_LOCKABLE:
//    case FORMAT_D32:
//    case FORMAT_D15S1:
//    case FORMAT_D24S8:
//    case FORMAT_D24X8:
//    case FORMAT_D24X4S4:
//    case FORMAT_D16:
//    case FORMAT_D32F_LOCKABLE:
//    case FORMAT_D24FS8:
//    case FORMAT_D32_LOCKABLE:
//    case FORMAT_S8_LOCKABLE:
//    case FORMAT_VERTEXDATA:
//    case FORMAT_INDEX16:
//    case FORMAT_INDEX32:
    case FORMAT_Q16W16V16U16:
//    case FORMAT_MULTI2_ARGB8:
    case FORMAT_R16F:
        return w*h*1*2;
    case FORMAT_G16R16F:
        return w*h*2*2;
    case FORMAT_A16B16G16R16F:
        return w*h*4*2;
    case FORMAT_R32F:
    case FORMAT_G32R32F:
    case FORMAT_A32B32G32R32F:
    case FORMAT_CxV8U8:
//    case FORMAT_A1:
//    case FORMAT_A2B10G10R10_XR_BIAS:
//    case FORMAT_BINARYBUFFER:
    case FORMAT_LAST:
        break;
    }

    return 0;
}

static qint64 mipmapOffset(const DDSHeader &dds, const int format, const int level)
{
    qint64 result = 0;
    for (int i = 0; i < level; ++i) {
        result += mipmapSize(dds, format, i);
    }
    return result;
}

QImage readCubeMap(QDataStream & s, const DDSHeader & dds, const int fmt)
{
    bool hasAlpha = ::hasAlpha(dds);
    QImage::Format format = hasAlpha ? QImage::Format_ARGB32 : QImage::Format_RGB32;
    QImage img(4 * dds.width, 3 * dds.height, format);

    img.fill(0);

    for (int i = 0; i < 6; i++) {
        if (!(dds.caps2 & faceFlags[i]))
            continue; // Skip face.

        const QImage face = ::readLayer(s, dds, fmt, dds.width, dds.height);

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
        qint64 pos = headerSize + mipmapOffset(header, m_format, m_currentImage);
        if (!device()->seek(pos))
            return false;
        QDataStream s(device());
        s.setByteOrder(QDataStream::LittleEndian);

        QImage img;
        if (isCubeMap(header))
            img = readCubeMap(s, header, m_format);
        else
            img = readTexture(s, header, m_format, m_currentImage);

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
    if (header.pixelFormat.fourCC == dx10Magic)
        s >> that->header10;

    that->m_format = getFormat(header);

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
