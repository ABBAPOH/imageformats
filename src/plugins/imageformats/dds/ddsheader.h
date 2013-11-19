/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2013 Ivan Komissarov.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the DDS plugin in the Qt ImageFormats module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef DDSHEADER_H
#define DDSHEADER_H

#include <QtCore/QDataStream>

QT_BEGIN_NAMESPACE

enum Format {
    FormatUnknown              = 0,

    FormatR8G8B8               = 20,
    FormatA8R8G8B8             = 21,
    FormatX8R8G8B8             = 22,
    FormatR5G6B5               = 23,
    FormatX1R5G5B5             = 24,
    FormatA1R5G5B5             = 25,
    FormatA4R4G4B4             = 26,
    FormatR3G3B2               = 27,
    FormatA8                   = 28,
    FormatA8R3G3B2             = 29,
    FormatX4R4G4B4             = 30,
    FormatA2B10G10R10          = 31,
    FormatA8B8G8R8             = 32,
    FormatX8B8G8R8             = 33,
    FormatG16R16               = 34,
    FormatA2R10G10B10          = 35,
    FormatA16B16G16R16         = 36,

    FormatA8P8                 = 40,
    FormatP8                   = 41,

    FormatL8                   = 50,
    FormatA8L8                 = 51,
    FormatA4L4                 = 52,

    FormatV8U8                 = 60,
    FormatL6V5U5               = 61,
    FormatX8L8V8U8             = 62,
    FormatQ8W8V8U8             = 63,
    FormatV16U16               = 64,
    FormatA2W10V10U10          = 67,

    FormatUYVY                 = 0x59565955, // "UYVY"
    FormatR8G8B8G8             = 0x47424752, // "RGBG"
    FormatYUY2                 = 0x32595559, // "YUY2"
    FormatG8R8G8B8             = 0x42475247, // "GRGB"
    FormatDXT1                 = 0x31545844, // "DXT1"
    FormatDXT2                 = 0x32545844, // "DXT2"
    FormatDXT3                 = 0x33545844, // "DXT3"
    FormatDXT4                 = 0x34545844, // "DXT4"
    FormatDXT5                 = 0x35545844, // "DXT5"
    FormatRXGB                 = 0x42475852, // "RXGB"
    FormatATI2                 = 0x32495441, // "ATI2"

    FormatD16Lockable         = 70,
    FormatD32                  = 71,
    FormatD15S1                = 73,
    FormatD24S8                = 75,
    FormatD24X8                = 77,
    FormatD24X4S4              = 79,
    FormatD16                  = 80,

    FormatD32FLockable        = 82,
    FormatD24FS8               = 83,

    FormatD32Lockable         = 84,
    FormatS8Lockable          = 85,

    FormatL16                  = 81,

    FormatVertexData           =100,
    FormatIndex16              =101,
    FormatIndex32              =102,

    FormatQ16W16V16U16         = 110,

    FormatMulti2ARGB8         = 0x3154454d, // "MET1"

    FormatR16F                 = 111,
    FormatG16R16F              = 112,
    FormatA16B16G16R16F        = 113,

    FormatR32F                 = 114,
    FormatG32R32F              = 115,
    FormatA32B32G32R32F        = 116,

    FormatCxV8U8               = 117,

    FormatA1                   = 118,
    FormatA2B10G10R10_XR_BIAS  = 119,
    FormatBinaryBuffer         = 199,

    FormatLast                 = 0x7fffffff
};

struct DDSPixelFormat
{
    enum DDSPixelFormatFlags {
        FlagAlphaPixels     = 0x00000001,
        FlagAlpha           = 0x00000002,
        FlagFourCC          = 0x00000004,
        FlagPaletteIndexed8 = 0x00000020,
        FlagRGB             = 0x00000040,
        FlagYUV             = 0x00000200,
        FlagLuminance       = 0x00020000,
        FlagNormal          = 0x00080000,
        FlagRGBA = FlagAlphaPixels | FlagRGB,
        FlagLA = FlagAlphaPixels | FlagLuminance
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
        FlagCaps        = 0x000001,
        FlagHeight      = 0x000002,
        FlagWidth       = 0x000004,
        FlagPitch       = 0x000008,
        FlagPixelFormat = 0x001000,
        FlagMipmapCount = 0x020000,
        FlagLinearSize  = 0x080000,
        FlagDepth       = 0x800000
    };

    enum DDSCapsFlags {
        CapsComplex = 0x000008,
        CapsTexture = 0x001000,
        CapsMipmap  = 0x400000
    };

    enum DDSCaps2Flags {
        Caps2CubeMap          = 0x0200,
        Caps2CubeMapPositiveX = 0x0400,
        Caps2CubeMapNegativeX = 0x0800,
        Caps2CubeMapPositiveY = 0x1000,
        Caps2CubeMapNegativeY = 0x2000,
        Caps2CubeMapPositiveZ = 0x4000,
        Caps2CubeMapNegativeZ = 0x8000,
        Caps2Volume           = 0x200000
    };

    enum { ReservedCount = 11 };

    quint32 magic;
    quint32 size;
    quint32 flags;
    quint32 height;
    quint32 width;
    quint32 pitchOrLinearSize;
    quint32 depth;
    quint32 mipMapCount;
    quint32 reserved1[ReservedCount];
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

QT_END_NAMESPACE

#endif // DDSHEADER_H
