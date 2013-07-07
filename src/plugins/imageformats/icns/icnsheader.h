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

enum IcnsIconFormat {
    IconUncompressed = 0,
    IconJP2 = 1,
    IconPNG = 2
};

struct IcnsIconEntry
{
    IcnsBlockHeader header;         // Original block header
    IcnsIconFormat  iconFormat;     // PNG, JP2 or uncompressed
    quint8          iconFamily;     // For Uncompressed icons only (ASCII character number)
    quint8          iconBitDepth;   // For Uncompressed icons only
    bool            iconIsMask;     // For Uncompressed icons only
    quint32         imageDataOffset;// Offset from the initial position of the file/device
    quint32         imageDataSize;  // header.length - sizeof(header)
};

#endif // ICNSHEADER
