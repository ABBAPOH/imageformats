#ifndef ICNSHEADER_H
#define ICNSHEADER_H

#include <QtCore/QtGlobal>

struct IcnsBlockHeader
{
    quint32 magic;
    quint32 length;
};

#define IcnsBlockHeaderSize 8

struct IcnsIconEntry
{
    IcnsBlockHeader header;         // Original block header
    quint8          iconGroup;      // ASCII character number pointing to a format or size
    quint8          iconBitDepth;   // For Uncompressed icons only
    bool            iconIsMask;     // For Uncompressed icons only
    quint32         imageDataOffset;// Offset from the initial position of the file/device
    quint32         imageDataSize;  // header.length - sizeof(header)
};

enum IcnsMagic {
    icnsfile = 0x69636E73,
    TOC_ = 0x544F4320,
    icnV = 0x69636E56,
    // Note to everyone: These are deprecated types of icons.
    // What should we to do to them?
    tile = 0x74696c65,
    over = 0x6f766572,
    drop = 0x64726f70,
    open = 0x6f70656e,
    odrp = 0x6f647270
};

enum IcnsIconGroup {
    IconGroup16x12          = 0x6D, // "m" for "mini"
    IconGroup16x16          = 0x73, // "s" for "small"
    IconGroup32x32          = 0x6C, // "l" for "large"
    IconGroup32x32Old       = 0x4E, // "N" (deprecated type)
    IconGroup48x48          = 0x68, // "h" for "huge"
    IconGroup128x128        = 0x74, // "t" for "thumbnail"
    IconGroupCompressed     = 0x63  // "c" for "compressed"?
};

enum IcnsIconBitDepth {
    IconBitDepthInvalid = 0,
    IconMono = 1,
    Icon4bit = 4,
    Icon8bit = 8,
    IconRLE24 = 32
};

#endif // ICNSHEADER
