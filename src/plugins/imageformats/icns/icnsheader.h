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
    icns = 0x69636E73,
    TOC_ = 0x544F4320,
    icnV = 0x69636E56
};

struct IcnsIconEntry
{
    IcnsBlockHeader header;
    quint32 imageDataOffset; // Calculate it from the beginning of the file/device
    // quint32 imageDataSize = header.blockLength - sizeof(header)
};

#endif // ICNSHEADER
