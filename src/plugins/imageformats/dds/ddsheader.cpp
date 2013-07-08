#include "ddsheader.h"

/*!
    \struct DDSPixelFormat

    Surface pixel format.
*/

/*!
    \enum DDSPixelFormat::DDSPixelFormatFlags

    This enum describes possible pixel format flags.

    \var DDSPixelFormat::DDSPixelFormatFlags::DDPF_ALPHAPIXELS
    Texture contains alpha data; dwRGBAlphaBitMask contains valid data.

    \var DDSPixelFormat::DDSPixelFormatFlags::DDPF_ALPHA

    Used in some older DDS files for alpha channel only uncompressed data
    (dwRGBBitCount contains the alpha channel bitcount; dwABitMask contains
    valid data)

    \var DDSPixelFormat::DDSPixelFormatFlags::DDPF_FOURCC

    Texture contains compressed RGB data; fourCC contains valid data.

    \var DDSPixelFormat::DDSPixelFormatFlags::DDPF_PALETTEINDEXED8

    [NYI](Legacy?)The surface is 8-bit color indexed.

    \var DDSPixelFormat::DDSPixelFormatFlags::DDPF_RGB

    Texture contains uncompressed RGB data; rgbBitCount and the RGB masks
    (rBitMask, gBitMask, bBitMask) contain valid data.

    \var DDSPixelFormat::DDSPixelFormatFlags::DDPF_YUV

    Used in some older DDS files for YUV uncompressed data (rgbBitCount
    contains the YUV bit count; rBitMask contains the Y mask, gBitMask contains
    the U mask, bBitMask contains the V mask)

    \var DDSPixelFormat::DDSPixelFormatFlags::DDPF_LUMINANCE

    Used in some older DDS files for single channel color uncompressed data
    (dwRGBBitCount contains the luminance channel bit count; dwRBitMask
    contains the channel mask). Can be combined with DDPF_ALPHAPIXELS for a two
    channel DDS file.

    \var DDSPixelFormat::DDSPixelFormatFlags::DDPF_NORMAL

    [NYI](Unusable?)Nvidia specific
*/

/*!
    \var DDSPixelFormat::size

    Structure size; set to 32 (bytes).
*/

/*!
    \var DDSPixelFormat::flags

    Values which indicate what type of data is in the surface.
*/

/*!
    \var DDSPixelFormat::fourCC

    Four-character codes for specifying compressed or custom formats. Possible
    values include: DXT1, DXT2, DXT3, DXT4, or DXT5. A FourCC of DX10 indicates
    the prescense of the DDS_HEADER_DXT10 extended header, and the dxgiFormat
    member of that structure indicates the true format. When using a
    four-character code, flags must include DDPF_FOURCC.
*/

/*!
    \var DDSPixelFormat::rgbBitCount

    Number of bits in an RGB (possibly including alpha) format. Valid when
    dwFlags includes DDPF_RGB, DDPF_LUMINANCE, or DDPF_YUV.
*/

/*!
    \var DDSPixelFormat::rBitMask

    Red (or lumiannce or Y) mask for reading color data. For instance, given
    the A8R8G8B8 format, the red mask would be 0x00ff0000.
*/

/*!
    \var DDSPixelFormat::gBitMask

    Green (or U) mask for reading color data. For instance, given the A8R8G8B8
    format, the green mask would be 0x0000ff00.
*/

/*!
    \var DDSPixelFormat::bBitMask

    Blue (or V) mask for reading color data. For instance, given the A8R8G8B8
    format, the blue mask would be 0x000000ff.
*/

/*!
    \var DDSPixelFormat::aBitMask

    Alpha mask for reading alpha data. flags must include DDPF_ALPHAPIXELS or
    DDPF_ALPHA. For instance, given the A8R8G8B8 format, the alpha mask would
    be 0xff000000.
*/

/*!
    Reads a DDSPixelFormat struct from the stream into \a pixelFormat, and
    returns a reference to the stream.
*/
QDataStream & operator>>(QDataStream &s, DDSPixelFormat &pixelFormat)
{
    s >> pixelFormat.size;
    s >> pixelFormat.flags;
    s >> pixelFormat.fourCC;
    s >> pixelFormat.rgbBitCount;
    s >> pixelFormat.rBitMask;
    s >> pixelFormat.gBitMask;
    s >> pixelFormat.bBitMask;
    s >> pixelFormat.aBitMask;
    return s;
}

/*!
    Writes a DDSPixelFormat struct, \a pixelFormat, to the stream and returns
    a reference to the stream.
*/
QDataStream & operator<<(QDataStream &s, const DDSPixelFormat &pixelFormat)
{
    s << pixelFormat.size;
    s << pixelFormat.flags;
    s << pixelFormat.fourCC;
    s << pixelFormat.rgbBitCount;
    s << pixelFormat.rBitMask;
    s << pixelFormat.gBitMask;
    s << pixelFormat.bBitMask;
    s << pixelFormat.aBitMask;
    return s;
}

/*!
    \struct DDSHeader

    Describes a DDS file header.
*/

/*!
    \enum DDSHeader::DDSFlags

    This enum describes dds flags.

    \var DDSHeader::DDSFlags DDSHeader::DDSD_CAPS
    Required in every .dds file.

    \var DDSHeader::DDSFlags DDSHeader::DDSD_HEIGHT
    Required in every .dds file.

    \var DDSHeader::DDSFlags DDSHeader::DDSD_WIDTH
    Required in every .dds file.

    \var DDSHeader::DDSFlags DDSHeader::DDSD_PITCH
    Required when pitch is provided for an uncompressed texture.

    \var DDSHeader::DDSFlags DDSHeader::DDSD_PIXELFORMAT
    Required in every .dds file.

    \var DDSHeader::DDSFlags DDSHeader::DDSD_MIPMAPCOUNT
    Required in a mipmapped texture.

    \var DDSHeader::DDSFlags DDSHeader::DDSD_LINEARSIZE
    Required when pitch is provided for a compressed texture.

    \var DDSHeader::DDSFlags DDSHeader::DDSD_DEPTH
    Required in a depth texture.
*/

/*!
    \enum DDSHeader::DDSCapsFlags

    This enums describes capabilities flags.

    \var DDSHeader::DDSCapsFlags DDSHeader::DDSCAPS_COMPLEX
    Optional; must be used on any file that contains more than one surface (a
    mipmap, a cubic environment map, or volume texture).

    \var DDSHeader::DDSCapsFlags DDSHeader::DDSCAPS_TEXTURE
    Required.

    \var DDSHeader::DDSCapsFlags DDSHeader::DDSCAPS_MIPMAP
    Optional; should be used for a mipmap.
*/

/*!
    \enum DDSHeader::DDSCapsFlags2

    This enums describes additional capabilities flags.

    \var DDSHeader::DDSCapsFlags2 DDSHeader::DDSCAPS2_CUBEMAP
    Required for a cube map.

    \var DDSHeader::DDSCapsFlags2 DDSHeader::DDSCAPS2_CUBEMAP_POSITIVEX
    Required when these surfaces are stored in a cube map.

    \var DDSHeader::DDSCapsFlags2 DDSHeader::DDSCAPS2_CUBEMAP_NEGATIVEX
    Required when these surfaces are stored in a cube map.

    \var DDSHeader::DDSCapsFlags2 DDSHeader::DDSCAPS2_CUBEMAP_POSITIVEY
    Required when these surfaces are stored in a cube map.

    \var DDSHeader::DDSCapsFlags2 DDSHeader::DDSCAPS2_CUBEMAP_NEGATIVEY

    Required when these surfaces are stored in a cube map.

    \var DDSHeader::DDSCapsFlags2 DDSHeader::DDSCAPS2_CUBEMAP_POSITIVEZ
    Required when these surfaces are stored in a cube map.

    \var DDSHeader::DDSCapsFlags2 DDSHeader::DDSCAPS2_CUBEMAP_NEGATIVEZ

    Required when these surfaces are stored in a cube map.

    \var DDSHeader::DDSCapsFlags2 DDSHeader::DDSCAPS2_VOLUME
    Required for a volume texture.
*/

/*!
    \var DDSHeader::size

    Size of structure. This member must be set to 124.
*/

/*!
    \var DDSHeader::flags

    Flags to indicate which members contain valid data.
*/

/*!
    \var DDSHeader::height

    Surface height (in pixels).
*/

/*!
    \var DDSHeader::width

    Surface width (in pixels).
*/

/*!
    \var DDSHeader::linearSize

    The number of bytes per scan line in an uncompressed texture; the total
    number of bytes in the top level texture for a compressed texture. The
    pitch must be quint32 aligned.
*/

/*!
    \var DDSHeader::depth

    Depth of a volume texture (in pixels), otherwise unused.
*/

/*!
    \var DDSHeader::mipMapCount

    Number of mipmap levels, otherwise unused.
*/

/*!
    \var DDSHeader::reserved1

    Unused.
*/

/*!
    \var DDSHeader::pixelFormat

    The pixel format (see DDSPixelFormat).
*/

/*!
    \var DDSHeader::caps

    Specifies the complexity of the surfaces stored.

    \note When you write .dds files, you should set the DDSCAPS_TEXTURE flag,
    and for multiple surfaces you should also set the DDSCAPS_COMPLEX flag.
    However, when you read a .dds file, you should not rely on the
    DDSCAPS_TEXTURE and DDSCAPS_COMPLEX flags being set because some writers of
    such a file might not set these flags.
*/

/*!
    \var DDSHeader::caps2

    Additional detail about the surfaces stored.
*/

/*!
    \var DDSHeader::caps3

    Unused.
*/

/*!
    \var DDSHeader::caps4

    Unused.
*/

/*!
    \var DDSHeader::reserved2

    Unused.
*/

QDataStream & operator>>(QDataStream &s, DDSHeader &header)
{
    s >> header.magic;
    s >> header.size;
    s >> header.flags;
    s >> header.height;
    s >> header.width;
    s >> header.linearSize;
    s >> header.depth;
    s >> header.mipMapCount;
    for (int i = 0; i< 11; i++) {
        s >> header.reserved1[i];
    }
    s >> header.pixelFormat;
    s >> header.caps;
    s >> header.caps2;
    s >> header.caps3;
    s >> header.caps4;
    s >> header.reserved2;
    return s;
}

QDataStream & operator<<(QDataStream &s, const DDSHeader &header)
{
    s << header.magic;
    s << header.size;
    s << header.flags;
    s << header.height;
    s << header.width;
    s << header.linearSize;
    s << header.depth;
    s << header.mipMapCount;
    for (int i = 0; i< 11; i++) {
        s << header.reserved1[i];
    }
    s << header.pixelFormat;
    s << header.caps;
    s << header.caps2;
    s << header.caps3;
    s << header.caps4;
    s << header.reserved2;
    return s;
}

