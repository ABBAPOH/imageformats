#ifndef DDSHEADER_H
#define DDSHEADER_H

#include <QtCore/QDataStream>

struct DDSPixelFormat
{
    enum DDSPixelFormatFlags {
        DDPF_ALPHAPIXELS     = 0x00000001,
        DDPF_ALPHA           = 0x00000002,
        DDPF_FOURCC          = 0x00000004,
        DDPF_PALETTEINDEXED8 = 0x00000020,
        DDPF_RGB             = 0x00000040,
        DDPF_YUV             = 0x00000200,
        DDPF_LUMINANCE       = 0x00020000,
        DDPF_NORMAL          = 0x80000000
    };

    quint32 size;
    quint32 flags;
    quint32 fourCC;
    quint32 rgbBitCount;
    quint32 rBitMask;
    quint32 gBitMask;
    quint32 bBitMask;
    quint32 aBitMask;
};

QDataStream & operator>>(QDataStream &s, DDSPixelFormat &pixelFormat);
QDataStream & operator<<(QDataStream &s, const DDSPixelFormat &pixelFormat);

struct DDSHeader
{
    enum DDSFlags {
        DDSD_CAPS        = 0x000001,
        DDSD_HEIGHT      = 0x000002,
        DDSD_WIDTH       = 0x000004,
        DDSD_PITCH       = 0x000008,
        DDSD_PIXELFORMAT = 0x001000,
        DDSD_MIPMAPCOUNT = 0x020000,
        DDSD_LINEARSIZE  = 0x080000,
        DDSD_DEPTH       = 0x800000
    };

    enum DDSCapsFlags {
        DDSCAPS_COMPLEX = 0x000008,
        DDSCAPS_TEXTURE = 0x001000,
        DDSCAPS_MIPMAP  = 0x400000
    };

    enum DDSCaps2Flags {
        DDSCAPS2_CUBEMAP           = 0x0200,
        DDSCAPS2_CUBEMAP_POSITIVEX = 0x0400,
        DDSCAPS2_CUBEMAP_NEGATIVEX = 0x0800,
        DDSCAPS2_CUBEMAP_POSITIVEY = 0x1000,
        DDSCAPS2_CUBEMAP_NEGATIVEY = 0x2000,
        DDSCAPS2_CUBEMAP_POSITIVEZ = 0x4000,
        DDSCAPS2_CUBEMAP_NEGATIVEZ = 0x8000,
        DDSCAPS2_VOLUME          = 0x200000
    };

    quint32 size;
    quint32 flags;
    quint32 height;
    quint32 width;
    quint32 linearSize;
    quint32 depth;
    quint32 mipMapCount;
    quint32 reserved1[11];
    DDSPixelFormat pixelFormat;
    quint32 caps;
    quint32 caps2;
    quint32 caps3;
    quint32 caps4;
    quint32 reserved2;
};

QDataStream & operator>>(QDataStream &s, DDSHeader &header);
QDataStream & operator<<(QDataStream &s, const DDSHeader &header);

#endif // DDSHEADER_H
