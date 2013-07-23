#ifndef ICNSFORMAT_H
#define ICNSFORMAT_H

#include <QtCore/QtGlobal>

struct IcnsIconDimensions {
    quint32 width;
    quint32 height;
};

enum IcnsIconGroup {
    IconGroup16x12      = 0x6D, // "m" for "mini"
    IconGroup16x16      = 0x73, // "s" for "small"
    IconGroup32x32      = 0x6C, // "l" for "large"
    IconGroup48x48      = 0x68, // "h" for "huge"
    IconGroup128x128    = 0x74, // "t" for "thumbnail"
    IconGroupCompressed = 0x63, // "c" for "compressed"?
    // Legacy icons spotted:
    IconGroup32x32Old   = 0x4E, // [SUPPORTED][DEPRECATED] "N" from OSType "ICON"
    IconGroupOpen       = 0x6E, // [NYI][DEPRECATED] "n" from OSType "open" (ostype: 0x6f70656e)
    IconGroupTile       = 0x65, // [NYI][DEPRECATED] "e" from OSType "tile" (ostype: 0x74696c65)
    IconGroupDrop       = 0x70, // [NYI][DEPRECATED] "p" from OSTypes "drop" and "odrp" (0x64726f70, 0x6f647270)
    IconGroupOver       = 0x72  // [NYI][DEPRECATED] "r" from OSType "over" (ostype: 0x6f766572)
};

enum IcnsIconBitDepth {
    IconMono  = 1,
    Icon4bit  = 4,
    Icon8bit  = 8,
    IconRLE24 = 32
};

enum IcnsIconMaskType {
    IconNoMask,     // Plain icon without alpha
    IconPlusMask,   // Plain icon and alpha mask (double size)
    IconIsMask      // The whole icon entry is alpha mask
};

#endif //ICNSFORMAT_H
