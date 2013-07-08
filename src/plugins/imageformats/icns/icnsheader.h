#ifndef ICNSHEADER_H
#define ICNSHEADER_H

#include <QtCore/QtGlobal>

struct IcnsBlockHeader
{
    quint32 magic;
    quint32 length;
};

#define IcnsBlockHeaderSize 8

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
    IconGroup16x12          = 0x6D, // "m"
    IconGroup16x16          = 0x73, // "s"
    IconGroup32x32          = 0x6C, // "l"
    IconGroup32x32Old       = 0x4E, // "N"
    IconGroup48x48          = 0x68, // "h"
    IconGroup128x128        = 0x33, // "t"
    IconGroupHDCompressed   = 0x63  // "c"
};

enum IcnsIconBitDepth {
    IconBitDepthInvalid = 0,
    IconMono = 1,
    Icon4bit = 4,
    Icon8bit = 8,
    IconRLE24 = 32
};

struct IcnsIconEntry
{
    IcnsBlockHeader header;         // Original block header
    quint8          iconGroup;      // ASCII character number pointing to a format or size
    quint8          iconBitDepth;   // For Uncompressed icons only
    bool            iconIsMask;     // For Uncompressed icons only
    quint32         imageDataOffset;// Offset from the initial position of the file/device
    quint32         imageDataSize;  // header.length - sizeof(header)
};

#endif // ICNSHEADER
