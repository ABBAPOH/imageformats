
#ifndef QICNSHANDLER_H
#define QICNSHANDLER_H

#include <QtGui/QImageIOHandler>
#include <QtGui/QImage>
#include <QtCore/QVector>
#include <QtCore/QBuffer>
#include <QtCore/QtMath>
#include <QtCore/QRegularExpression>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

struct IcnsBlockHeader {
    enum SpecificOSTypes {
        OSType_icns     = 0x69636E73,
        OSType_TOC_     = 0x544F4320,
        OSType_icnV     = 0x69636E56
    };

    quint32 OSType;
    quint32 length;
};

struct IcnsEntry
{
    enum IconGroup {
        IconGroupUnk        = 0,
        IconGroupMini       = 0x6D, // "m" for "mini" (16x12)
        IconGroupSmall      = 0x73, // "s" for "small" (16x16)
        IconGroupLarge      = 0x6C, // "l" for "large" (32x32)
        IconGroupHuge       = 0x68, // "h" for "huge" (48x48)
        IconGroupThumbnail  = 0x74, // "t" for "thumbnail" (128x128)
        IconGroupCompressed = 0x63, // "c" for "compressed"? (various sizes)
        // Legacy icons spotted:
        IconGroupICON       = 0x4E, // [FULL SUPPORT] "N" from OSType "ICON" (32x32)
        IconGroupOpen       = 0x6E, // [NYI, UNKNOWN] "n" from OSType "open" (ostype: 0x6f70656e)
        IconGroupTile       = 0x65, // [NYI, UNKNOWN] "e" from OSType "tile" (ostype: 0x74696c65)
        IconGroupDrop       = 0x70, // [NYI, UNKNOWN] "p" from OSTypes "drop" and "odrp" (0x64726f70, 0x6f647270)
        IconGroupOver       = 0x72  // [NYI, UNKNOWN] "r" from OSType "over" (ostype: 0x6f766572)
    };
    enum Depth {
        IconDepthUnk    = 0,
        IconMono        = 1,
        Icon4bit        = 4,
        Icon8bit        = 8,
        Icon32bit       = 32
    };
    enum IconMaskType {
        IconMaskUnk,
        IconNoMask,     // Plain icon without alpha
        IconPlusMask,   // Plain icon and alpha mask (double size)
        IconIsMask      // The whole icon entry is alpha mask
    };

    IcnsBlockHeader     header;       // Original block header
    IconGroup           group;        // ASCII character number pointing to a format
    Depth               depth;        // Color depth for uncompr. icons or icon format num for compressed
    IconMaskType        mask;         // For Uncompressed icons only
    quint32             width;        // For Uncompressed icons only
    quint32             height;       // For Uncompressed icons only
    bool                isValid;      // True if correctly parsed
    quint32             dataLength;   // header.length - sizeof(header)
    quint32             dataOffset;   // Offset from the initial position of the file/device
    bool                dataIsRLE;    // 32bit raw icons may be in rle24 compressed state
};

class QIcnsHandler : public QImageIOHandler
{
    enum IcnsHandlerState {
        IcnsFileIsParsed,
        IcnsFileIsNotParsed,
        IcnsFileParsingError
    };

public:
    QIcnsHandler();
    QIcnsHandler(QIODevice *d, const QByteArray &format);

    bool canRead() const;
    bool read(QImage *image);
    bool write(const QImage &image);

    QByteArray name() const;

    bool supportsOption(ImageOption option) const;
    QVariant option(ImageOption option) const;

    int imageCount() const;
    bool jumpToImage(int imageNumber);
    bool jumpToNextImage();

    static bool canRead(QIODevice *device);
    static bool canWrite(QIODevice *device);

private:
    bool isScanned() const;
    bool isParsed() const;
    void scanDevice();
    bool addEntry(const IcnsBlockHeader &header, quint32 imgDataOffset);
    IcnsEntry getIconMask(const IcnsEntry &icon) const;

private:
    int m_currentIconIndex;
    QDataStream m_stream;
    QVector<IcnsEntry> m_icons;
    QVector<IcnsEntry> m_masks;
    IcnsHandlerState m_scanstate;
};

QT_END_NAMESPACE

#endif /* QICNSHANDLER_H */
