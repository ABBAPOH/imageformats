#ifndef DDSHEADER_H
#define DDSHEADER_H

#include <QtCore/QDataStream>

enum Format {
    FORMAT_UNKNOWN              = 0,

    FORMAT_R8G8B8               = 20,
    FORMAT_A8R8G8B8             = 21,
    FORMAT_X8R8G8B8             = 22,
    FORMAT_R5G6B5               = 23,
    FORMAT_X1R5G5B5             = 24,
    FORMAT_A1R5G5B5             = 25,
    FORMAT_A4R4G4B4             = 26,
    FORMAT_R3G3B2               = 27,
    FORMAT_A8                   = 28,
    FORMAT_A8R3G3B2             = 29,
    FORMAT_X4R4G4B4             = 30,
    FORMAT_A2B10G10R10          = 31,
    FORMAT_A8B8G8R8             = 32,
    FORMAT_X8B8G8R8             = 33,
    FORMAT_G16R16               = 34,
    FORMAT_A2R10G10B10          = 35,
    FORMAT_A16B16G16R16         = 36,

    FORMAT_A8P8                 = 40,
    FORMAT_P8                   = 41,

    FORMAT_L8                   = 50,
    FORMAT_A8L8                 = 51,
    FORMAT_A4L4                 = 52,

    FORMAT_V8U8                 = 60,
    FORMAT_L6V5U5               = 61,
    FORMAT_X8L8V8U8             = 62,
    FORMAT_Q8W8V8U8             = 63,
    FORMAT_V16U16               = 64,
    FORMAT_A2W10V10U10          = 67,

    FORMAT_UYVY                 = 0x59565955, // "UYVY"
    FORMAT_R8G8_B8G8            = 0x47424752, // "RGBG"
    FORMAT_YUY2                 = 0x32595559, // "YUY2"
    FORMAT_G8R8_G8B8            = 0x42475247, // "GRGB"
    FORMAT_DXT1                 = 0x31545844, // "DXT1"
    FORMAT_DXT2                 = 0x32545844, // "DXT2"
    FORMAT_DXT3                 = 0x33545844, // "DXT3"
    FORMAT_DXT4                 = 0x34545844, // "DXT4"
    FORMAT_DXT5                 = 0x35545844, // "DXT5"

    FORMAT_D16_LOCKABLE         = 70,
    FORMAT_D32                  = 71,
    FORMAT_D15S1                = 73,
    FORMAT_D24S8                = 75,
    FORMAT_D24X8                = 77,
    FORMAT_D24X4S4              = 79,
    FORMAT_D16                  = 80,

    FORMAT_D32F_LOCKABLE        = 82,
    FORMAT_D24FS8               = 83,

    FORMAT_D32_LOCKABLE         = 84,
    FORMAT_S8_LOCKABLE          = 85,

    FORMAT_L16                  = 81,

    FORMAT_VERTEXDATA           =100,
    FORMAT_INDEX16              =101,
    FORMAT_INDEX32              =102,

    FORMAT_Q16W16V16U16         = 110,

    FORMAT_MULTI2_ARGB8         = 0x3154454d, // "MET1"

    FORMAT_R16F                 = 111,
    FORMAT_G16R16F              = 112,
    FORMAT_A16B16G16R16F        = 113,

    FORMAT_R32F                 = 114,
    FORMAT_G32R32F              = 115,
    FORMAT_A32B32G32R32F        = 116,

    FORMAT_CxV8U8               = 117,

    FORMAT_A1                   = 118,
    FORMAT_A2B10G10R10_XR_BIAS  = 119,
    FORMAT_BINARYBUFFER         = 199,

    FORMAT_LAST                 =0x7fffffff
};

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
        DDPF_NORMAL          = 0x00080000,
        DDPF_RGBA = DDPF_ALPHAPIXELS | DDPF_RGB,
        DDPF_LA = DDPF_ALPHAPIXELS | DDPF_LUMINANCE
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

    quint32 magic;
    quint32 size;
    quint32 flags;
    quint32 height;
    quint32 width;
    quint32 pitchOrLinearSize;
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

struct DDSHeaderDX10
{
    enum DXGI_FORMAT {
        DXGI_FORMAT_UNKNOWN                    = 0,
        DXGI_FORMAT_R32G32B32A32_TYPELESS      = 1,
        DXGI_FORMAT_R32G32B32A32_FLOAT         = 2,
        DXGI_FORMAT_R32G32B32A32_UINT          = 3,
        DXGI_FORMAT_R32G32B32A32_SINT          = 4,
        DXGI_FORMAT_R32G32B32_TYPELESS         = 5,
        DXGI_FORMAT_R32G32B32_FLOAT            = 6,
        DXGI_FORMAT_R32G32B32_UINT             = 7,
        DXGI_FORMAT_R32G32B32_SINT             = 8,
        DXGI_FORMAT_R16G16B16A16_TYPELESS      = 9,
        DXGI_FORMAT_R16G16B16A16_FLOAT         = 10,
        DXGI_FORMAT_R16G16B16A16_UNORM         = 11,
        DXGI_FORMAT_R16G16B16A16_UINT          = 12,
        DXGI_FORMAT_R16G16B16A16_SNORM         = 13,
        DXGI_FORMAT_R16G16B16A16_SINT          = 14,
        DXGI_FORMAT_R32G32_TYPELESS            = 15,
        DXGI_FORMAT_R32G32_FLOAT               = 16,
        DXGI_FORMAT_R32G32_UINT                = 17,
        DXGI_FORMAT_R32G32_SINT                = 18,
        DXGI_FORMAT_R32G8X24_TYPELESS          = 19,
        DXGI_FORMAT_D32_FLOAT_S8X24_UINT       = 20,
        DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS   = 21,
        DXGI_FORMAT_X32_TYPELESS_G8X24_UINT    = 22,
        DXGI_FORMAT_R10G10B10A2_TYPELESS       = 23,
        DXGI_FORMAT_R10G10B10A2_UNORM          = 24,
        DXGI_FORMAT_R10G10B10A2_UINT           = 25,
        DXGI_FORMAT_R11G11B10_FLOAT            = 26,
        DXGI_FORMAT_R8G8B8A8_TYPELESS          = 27,
        DXGI_FORMAT_R8G8B8A8_UNORM             = 28,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB        = 29,
        DXGI_FORMAT_R8G8B8A8_UINT              = 30,
        DXGI_FORMAT_R8G8B8A8_SNORM             = 31,
        DXGI_FORMAT_R8G8B8A8_SINT              = 32,
        DXGI_FORMAT_R16G16_TYPELESS            = 33,
        DXGI_FORMAT_R16G16_FLOAT               = 34,
        DXGI_FORMAT_R16G16_UNORM               = 35,
        DXGI_FORMAT_R16G16_UINT                = 36,
        DXGI_FORMAT_R16G16_SNORM               = 37,
        DXGI_FORMAT_R16G16_SINT                = 38,
        DXGI_FORMAT_R32_TYPELESS               = 39,
        DXGI_FORMAT_D32_FLOAT                  = 40,
        DXGI_FORMAT_R32_FLOAT                  = 41,
        DXGI_FORMAT_R32_UINT                   = 42,
        DXGI_FORMAT_R32_SINT                   = 43,
        DXGI_FORMAT_R24G8_TYPELESS             = 44,
        DXGI_FORMAT_D24_UNORM_S8_UINT          = 45,
        DXGI_FORMAT_R24_UNORM_X8_TYPELESS      = 46,
        DXGI_FORMAT_X24_TYPELESS_G8_UINT       = 47,
        DXGI_FORMAT_R8G8_TYPELESS              = 48,
        DXGI_FORMAT_R8G8_UNORM                 = 49,
        DXGI_FORMAT_R8G8_UINT                  = 50,
        DXGI_FORMAT_R8G8_SNORM                 = 51,
        DXGI_FORMAT_R8G8_SINT                  = 52,
        DXGI_FORMAT_R16_TYPELESS               = 53,
        DXGI_FORMAT_R16_FLOAT                  = 54,
        DXGI_FORMAT_D16_UNORM                  = 55,
        DXGI_FORMAT_R16_UNORM                  = 56,
        DXGI_FORMAT_R16_UINT                   = 57,
        DXGI_FORMAT_R16_SNORM                  = 58,
        DXGI_FORMAT_R16_SINT                   = 59,
        DXGI_FORMAT_R8_TYPELESS                = 60,
        DXGI_FORMAT_R8_UNORM                   = 61,
        DXGI_FORMAT_R8_UINT                    = 62,
        DXGI_FORMAT_R8_SNORM                   = 63,
        DXGI_FORMAT_R8_SINT                    = 64,
        DXGI_FORMAT_A8_UNORM                   = 65,
        DXGI_FORMAT_R1_UNORM                   = 66,
        DXGI_FORMAT_R9G9B9E5_SHAREDEXP         = 67,
        DXGI_FORMAT_R8G8_B8G8_UNORM            = 68,
        DXGI_FORMAT_G8R8_G8B8_UNORM            = 69,
        DXGI_FORMAT_BC1_TYPELESS               = 70,
        DXGI_FORMAT_BC1_UNORM                  = 71,
        DXGI_FORMAT_BC1_UNORM_SRGB             = 72,
        DXGI_FORMAT_BC2_TYPELESS               = 73,
        DXGI_FORMAT_BC2_UNORM                  = 74,
        DXGI_FORMAT_BC2_UNORM_SRGB             = 75,
        DXGI_FORMAT_BC3_TYPELESS               = 76,
        DXGI_FORMAT_BC3_UNORM                  = 77,
        DXGI_FORMAT_BC3_UNORM_SRGB             = 78,
        DXGI_FORMAT_BC4_TYPELESS               = 79,
        DXGI_FORMAT_BC4_UNORM                  = 80,
        DXGI_FORMAT_BC4_SNORM                  = 81,
        DXGI_FORMAT_BC5_TYPELESS               = 82,
        DXGI_FORMAT_BC5_UNORM                  = 83,
        DXGI_FORMAT_BC5_SNORM                  = 84,
        DXGI_FORMAT_B5G6R5_UNORM               = 85,
        DXGI_FORMAT_B5G5R5A1_UNORM             = 86,
        DXGI_FORMAT_B8G8R8A8_UNORM             = 87,
        DXGI_FORMAT_B8G8R8X8_UNORM             = 88,
        DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM = 89,
        DXGI_FORMAT_B8G8R8A8_TYPELESS          = 90,
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB        = 91,
        DXGI_FORMAT_B8G8R8X8_TYPELESS          = 92,
        DXGI_FORMAT_B8G8R8X8_UNORM_SRGB        = 93,
        DXGI_FORMAT_BC6H_TYPELESS              = 94,
        DXGI_FORMAT_BC6H_UF16                  = 95,
        DXGI_FORMAT_BC6H_SF16                  = 96,
        DXGI_FORMAT_BC7_TYPELESS               = 97,
        DXGI_FORMAT_BC7_UNORM                  = 98,
        DXGI_FORMAT_BC7_UNORM_SRGB             = 99,
        DXGI_FORMAT_AYUV                       = 100,
        DXGI_FORMAT_Y410                       = 101,
        DXGI_FORMAT_Y416                       = 102,
        DXGI_FORMAT_NV12                       = 103,
        DXGI_FORMAT_P010                       = 104,
        DXGI_FORMAT_P016                       = 105,
        DXGI_FORMAT_420_OPAQUE                 = 106,
        DXGI_FORMAT_YUY2                       = 107,
        DXGI_FORMAT_Y210                       = 108,
        DXGI_FORMAT_Y216                       = 109,
        DXGI_FORMAT_NV11                       = 110,
        DXGI_FORMAT_AI44                       = 111,
        DXGI_FORMAT_IA44                       = 112,
        DXGI_FORMAT_P8                         = 113,
        DXGI_FORMAT_A8P8                       = 114,
        DXGI_FORMAT_B4G4R4A4_UNORM             = 115,
        DXGI_FORMAT_FORCE_UINT                 = 0xffffffffUL
    };

    quint32 dxgiFormat;
    quint32 resourceDimension;
    quint32 miscFlag;
    quint32 arraySize;
    quint32 reserved;
};

QDataStream & operator>>(QDataStream &s, DDSHeaderDX10 &header);
QDataStream & operator<<(QDataStream &s, const DDSHeaderDX10 &header);

#endif // DDSHEADER_H
