#ifndef ICNSFORMAT_H
#define ICNSFORMAT_H

#include <QtCore/QtGlobal>

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
    IconDepthUnk    = 0, // placeholder
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
    IcnsIconBitDepth depth;         // For Uncompressed icons only
    IcnsIconSizeData size;          // For Uncompressed icons only
};

static const IcnsIconData IcnsKnownGroups[] =
{
    {IconGroupMini,         IconDepthUnk,   {16, 12}},      // Mini
    {IconGroupSmall,        IconDepthUnk,   {16, 16}},      // Small
    {IconGroupLarge,        IconDepthUnk,   {32, 32}},      // Large
    {IconGroupICON,         IconMono,       {32, 32}},      // [DEPRECATED]ICON
    {IconGroupHuge,         IconDepthUnk,   {48, 48}},      // Huge
    {IconGroupThumbnail,    IconDepthUnk,   {128, 128}},    // Thumbnail
    //{IconGroupCompressed,   IconDepthUnk,   {256, 256}},    // Compressed icons only
    //{IconGroupCompressed,   IconDepthUnk,   {512, 512}},    // Compressed icons only
    //{IconGroupCompressed,   IconDepthUnk,   {1024, 1024}},  // Compressed icons only
    {IconGroupUnk,          IconDepthUnk,   {0, 0}}         // Placeholder - end of the list marker
};

#endif //ICNSFORMAT_H
