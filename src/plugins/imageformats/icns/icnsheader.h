#ifndef ICNSHEADER_H
#define ICNSHEADER_H

#include <QtCore/QtGlobal>

struct IcnsHeader
{
    quint8 magic[4];
    quint32 size;
};

struct IcnsDataBlockHeader
{
    quint8 ostype[4];
    quint32 blockLength;
};

struct IcnsTOCEntry
{
    quint8 ostype[4];
    quint32 size;
};

struct IcnsIconEntry
{
    IcnsDataBlockHeader header;
    quint32 imageDataOffset; // Calculate it from the beginning of the file/device
    // quint32 imageDataSize = header.blockLength - sizeof(header)
};

#endif // ICNSHEADER
