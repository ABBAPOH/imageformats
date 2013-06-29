#include "ddsheader.h"

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

QDataStream & operator>>(QDataStream &s, DDSHeader &header)
{
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

