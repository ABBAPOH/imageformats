#ifndef ICNSHEADER_H
#define ICNSHEADER_H

#include <QtCore/QtGlobal>

enum IcnsOSType {
    OSType_icnsfile = 0x69636E73,
    OSType_TOC_ = 0x544F4320,
    OSType_icnV = 0x69636E56
};

enum IcnsIconGroup {
    IconGroup16x12      = 0x6D, // "m" for "mini"
    IconGroup16x16      = 0x73, // "s" for "small"
    IconGroup32x32      = 0x6C, // "l" for "large"
    IconGroup48x48      = 0x68, // "h" for "huge"
    IconGroup128x128    = 0x74, // "t" for "thumbnail"
    IconGroupCompressed = 0x63, // "c" for "compressed"?
    // Legacy icons spotted:
    IconGroup32x32Old   = 0x4E, // [DEPRECATED] "N" from OSType "ICON"
    IconGroupOpen       = 0x6E, // [DEPRECATED] "n" from OSType "open" (ostype: 0x6f70656e)
    IconGroupTile       = 0x65, // [DEPRECATED] "e" from OSType "tile" (ostype: 0x74696c65)
    IconGroupDrop       = 0x70, // [DEPRECATED] "p" from OSTypes "drop" and "odrp" (0x64726f70, 0x6f647270)
    IconGroupOver       = 0x72  // [DEPRECATED] "r" from OSType "over" (ostype: 0x6f766572)
};

enum IcnsIconBitDepth {
    IconMono = 1,
    Icon4bit = 4,
    Icon8bit = 8,
    IconRLE24 = 32
};

enum IcnsIconMaskType {
    IconNoMask = 0, // Plain icon without alpha
    IconPlusMask,   // Plain icon and alpha mask (double size)
    IconIsMask      // The whole icon entry is alpha mask
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
    IcnsIconGroup       iconGroup;      // ASCII character number pointing to a format or size
    IcnsIconBitDepth    iconBitDepth;   // For Uncompressed icons only
    IcnsIconMaskType    iconMaskType;   // For Uncompressed icons only
    quint32             iconWidth;      // For Uncompressed icons only
    quint32             iconHeight;     // For Uncompressed icons only
};

#endif //ICNSHEADER_H
