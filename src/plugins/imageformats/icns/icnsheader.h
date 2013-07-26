#ifndef ICNSHEADER_H
#define ICNSHEADER_H

#include "icnsformat.h"

enum IcnsOSType {
    OSType_icnsfile = 0x69636E73,
    OSType_TOC_     = 0x544F4320,
    OSType_icnV     = 0x69636E56
};

struct IcnsBlockHeader
{
    quint32 OSType;
    quint32 length;
};

#define IcnsBlockHeaderSize 8

struct IcnsIconEntry
{
    IcnsBlockHeader     header;         // Original block header
    quint32             imageDataOffset;// Offset from the initial position of the file/device
    quint32             imageDataSize;  // header.length - sizeof(header)
    IcnsIconData        iconData;       // Contains group, bit depth and dimensions
    IcnsIconMaskType    iconMaskType;   // For Uncompressed icons only
};

#endif //ICNSHEADER_H
