#ifndef ICNSFORMAT_H
#define ICNSFORMAT_H

#include <QtCore/QtGlobal>
#include <QtCore/QDataStream>

enum IcnsOSType {
    OSType_icnsfile = 0x69636E73,
    OSType_TOC_     = 0x544F4320,
    OSType_icnV     = 0x69636E56
};

enum IcnsIconGroup {
    IconGroupUnk        = 0,    // placeholder
    IconGroupMini       = 0x6D, // "m" for "mini" (16x12)
    IconGroupSmall      = 0x73, // "s" for "small" (16x16)
    IconGroupLarge      = 0x6C, // "l" for "large" (32x32)
    IconGroupHuge       = 0x68, // "h" for "huge" (48x48)
    IconGroupThumbnail  = 0x74, // "t" for "thumbnail" (128x128)
    IconGroupCompressed = 0x63, // "c" for "compressed"? (various sizes)
    // Legacy icons spotted:
    IconGroupICON       = 0x4E, // [SUPPORTED][DEPRECATED] "N" from OSType "ICON" (32x32)
    IconGroupOpen       = 0x6E, // [NYI][DEPRECATED] "n" from OSType "open" (ostype: 0x6f70656e)
    IconGroupTile       = 0x65, // [NYI][DEPRECATED] "e" from OSType "tile" (ostype: 0x74696c65)
    IconGroupDrop       = 0x70, // [NYI][DEPRECATED] "p" from OSTypes "drop" and "odrp" (0x64726f70, 0x6f647270)
    IconGroupOver       = 0x72  // [NYI][DEPRECATED] "r" from OSType "over" (ostype: 0x6f766572)
};

enum IcnsIconBitDepth {
    IconDepthAny    = 0, // placeholder
    IconMono        = 1,
    Icon4bit        = 4,
    Icon8bit        = 8,
    IconRLE24       = 32
};

enum IcnsIconMaskType {
    IconNoMask,     // Plain icon without alpha
    IconPlusMask,   // Plain icon and alpha mask (double size)
    IconIsMask      // The whole icon entry is alpha mask
};

struct IcnsIconSizeData {
    quint32 width;
    quint32 height;
};

struct IcnsIconData {
    IcnsIconGroup group;            // ASCII character number pointing to a format
    IcnsIconBitDepth depth;         // Color depth for uncompr. icons or icon format num for compressed
    IcnsIconSizeData size;          // Used for uncompr. icons, but filled for compr. as experiment
};

struct IcnsBlockHeader {
    quint32 OSType;
    quint32 length;
};
#define IcnsBlockHeaderSize 8
QDataStream &operator>>(QDataStream &in, IcnsBlockHeader &p);
QDataStream &operator<<(QDataStream &out, IcnsBlockHeader &p);

struct IcnsIconEntry {
    IcnsBlockHeader     header;         // Original block header
    quint32             imageDataOffset;// Offset from the initial position of the file/device
    quint32             imageDataSize;  // header.length - sizeof(header)
    IcnsIconData        iconData;       // Contains group, bit depth and dimensions
    IcnsIconMaskType    iconMaskType;   // For Uncompressed icons only
};

// Uncompressed icon groups for reading and (possibly) writing:
// Needs some more thinking perphaps
static const IcnsIconData IcnsUncompressedGroups[] = {
    {IconGroupMini,         IconDepthAny,   {16, 12}},      // Mini
    {IconGroupSmall,        IconDepthAny,   {16, 16}},      // Small
    {IconGroupLarge,        IconDepthAny,   {32, 32}},      // Large
    {IconGroupICON,         IconMono,       {32, 32}},      // [DEPRECATED]ICON
    {IconGroupHuge,         IconDepthAny,   {48, 48}},      // Huge
    {IconGroupThumbnail,    IconDepthAny,   {128, 128}}     // Thumbnail
};
#define IcnsUncompressedGroupsNum 6

#endif //ICNSFORMAT_H
