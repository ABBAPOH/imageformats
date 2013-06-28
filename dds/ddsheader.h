#ifndef DDSHEADER_H
#define DDSHEADER_H

#include <qglobal.h>

struct DDSPixelFormat
{
    quint32 size; // Structure size; set to 32 (bytes).
    quint32 flags; // Values which indicate what type of data is in the surface.
    enum DDSPixelFormatFlags { DDPF_ALPHAPIXELS = 0x01, // Texture contains alpha data; dwRGBAlphaBitMask contains valid data.
                               DDPF_ALPHA = 0x02, // Used in some older DDS files for alpha channel only uncompressed data (dwRGBBitCount contains the alpha channel bitcount; dwABitMask contains valid data)
                               DDPF_FOURCC = 0x4, // Texture contains compressed RGB data; fourCC contains valid data.
                               DDPF_RGB = 0x40, // Texture contains uncompressed RGB data; rgbBitCount and the RGB masks (rBitMask, gBitMask, bBitMask) contain valid data.
                               DDPF_YUV = 0x200, // Used in some older DDS files for YUV uncompressed data (rgbBitCount contains the YUV bit count; rBitMask contains the Y mask, gBitMask contains the U mask, bBitMask contains the V mask)
                               DDPF_LUMINANCE = 0x20000 // Used in some older DDS files for single channel color uncompressed data (dwRGBBitCount contains the luminance channel bit count; dwRBitMask contains the channel mask). Can be combined with DDPF_ALPHAPIXELS for a two channel DDS file.
                                            };

    quint32 fourCC; // Four-character codes for specifying compressed or custom formats. Possible values include: DXT1, DXT2, DXT3, DXT4, or DXT5. A FourCC of DX10 indicates the prescense of the DDS_HEADER_DXT10 extended header, and the dxgiFormat member of that structure indicates the true format. When using a four-character code, flags must include DDPF_FOURCC.
    quint32 rgbBitCount; // Number of bits in an RGB (possibly including alpha) format. Valid when dwFlags includes DDPF_RGB, DDPF_LUMINANCE, or DDPF_YUV.
    quint32 rBitMask; // Red (or lumiannce or Y) mask for reading color data. For instance, given the A8R8G8B8 format, the red mask would be 0x00ff0000.
    quint32 gBitMask; // Green (or U) mask for reading color data. For instance, given the A8R8G8B8 format, the green mask would be 0x0000ff00.
    quint32 bBitMask; // Blue (or V) mask for reading color data. For instance, given the A8R8G8B8 format, the blue mask would be 0x000000ff.
    quint32 aBitMask; // Alpha mask for reading alpha data. flags must include DDPF_ALPHAPIXELS or DDPF_ALPHA. For instance, given the A8R8G8B8 format, the alpha mask would be 0xff000000.
};

struct DDSHeader
{
    quint32           size; // Size of structure. This member must be set to 124.
    quint32           flags; // Flags to indicate which members contain valid data.
    enum DDSFlags { DDSD_CAPS = 0x01, // Required in every .dds file.
                    DDSD_HEIGHT = 0x02, // Required in every .dds file.
                    DDSD_WIDTH = 0x04, // Required in every .dds file.
                    DDSD_PITCH = 0x08, // Required when pitch is provided for an uncompressed texture.
                    DDSD_PIXELFORMAT = 0x1000, // Required in every .dds file.
                    DDSD_MIPMAPCOUNT = 0x20000, // Required in a mipmapped texture.
                    DDSD_LINEARSIZE= 0x80000, // Required when pitch is provided for a compressed texture.
                    DDSD_DEPTH = 0x800000 // Required in a depth texture.
                             };

    quint32           height; // Surface height (in pixels).
    quint32           width; // Surface width (in pixels).
    quint32           linearSize; // The number of bytes per scan line in an uncompressed texture; the total number of bytes in the top level texture for a compressed texture. The pitch must be quint32 aligned.
    quint32           depth; // Depth of a volume texture (in pixels), otherwise unused.
    quint32           mipMapCount; // Number of mipmap levels, otherwise unused.
    quint32           reserved1[11]; // Unused.
    DDSPixelFormat    pixelFormat; // The pixel format (see DDSPixelFormat).
    quint32           caps; // Specifies the complexity of the surfaces stored.
    enum DDSCapsFlags { DDSCAPS_COMPLEX = 0x8, // Optional; must be used on any file that contains more than one surface (a mipmap, a cubic environment map, or volume texture).
                        DDSCAPS_MIPMAP = 0x400000, // Optional; should be used for a mipmap.
                        DDSCAPS_TEXTURE = 0x1000 // Required
                                      };

    quint32           caps2; // Additional detail about the surfaces stored.
    quint32           caps3;
    quint32           caps4;
    quint32           reserved2; // Unused.
};

#endif // DDSHEADER_H
