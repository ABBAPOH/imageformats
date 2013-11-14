#ifndef ICNSHANDLER_H
#define ICNSHANDLER_H

#include <cmath>
#include <QtGui/QImageIOHandler>
#include <QtGui/QImageReader>
#include <QtGui/QImage>
#include <QtCore/QVector>
#include <QtCore/QBuffer>
#if QT_VERSION >= 0x050000
#include <QtCore/QRegularExpression>
#endif
#include <QtCore/QDebug>

struct IcnsBlockHeader {
    quint32 OSType;
    quint32 length;
};
static const quint8 IcnsBlockHeaderSize = 8;

class QIcnsHandler : public QImageIOHandler
{
    enum OSType {
        OSType_icns     = 0x69636E73,
        OSType_TOC_     = 0x544F4320,
        OSType_icnV     = 0x69636E56
    };
    enum IconGroup {
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
    enum IconBitDepth {
        IconDepthUnk    = 0, // placeholder
        IconMono        = 1,
        Icon4bit        = 4,
        Icon8bit        = 8,
        Icon32bit       = 32
    };
    enum IconMaskType {
        IconMaskUnk,    // Not identified yet
        IconNoMask,     // Plain icon without alpha
        IconPlusMask,   // Plain icon and alpha mask (double size)
        IconIsMask      // The whole icon entry is alpha mask
    };

    class QIcnsIconEntry
    {
    public:
        QIcnsIconEntry();
        QIcnsIconEntry(IcnsBlockHeader &header, quint32 imgDataOffset);
        quint32             getOSType()     const { return m_header.OSType; }
        IconGroup           group()         const { return m_iconGroup; }
        IconBitDepth        depth()         const { return m_iconDepth; }
        IconMaskType        mask()          const { return m_iconMaskType; }
        quint32             width()         const { return m_iconWidth; }
        quint32             height()        const { return m_iconHeight; }
        quint32             dataLength()    const { return m_imageDataLength; }
        quint32             dataOffset()    const { return m_imageDataOffset; }
        bool                isValid()       const { return m_iconIsParsed; }
        bool                isRLE24()       const { return m_imageDataIsRLE; }

    private:
        IcnsBlockHeader     m_header;           // Original block header
        IconGroup           m_iconGroup;        // ASCII character number pointing to a format
        IconBitDepth        m_iconDepth;        // Color depth for uncompr. icons or icon format num for compressed
        IconMaskType        m_iconMaskType;     // For Uncompressed icons only
        quint32             m_iconWidth;        //
        quint32             m_iconHeight;       //
        bool                m_iconIsParsed;     //
        quint32             m_imageDataLength;  // header.length - sizeof(header)
        quint32             m_imageDataOffset;  // Offset from the initial position of the file/device
        bool                m_imageDataIsRLE;   // 32bit raw icons may be in rle24 compressed state
    };

public:
    QIcnsHandler(QIODevice *d, const QByteArray &format);

    bool canRead() const;
    bool read(QImage *image);
    bool write(const QImage &image);

    QByteArray name() const;

    int imageCount() const;
    bool jumpToImage(int imageNumber);
    bool jumpToNextImage();

    static bool canRead(QIODevice *device);
    static bool canWrite(QIODevice *device);

    static QVector<QRgb> getColorTable(const IconBitDepth & depth);
    static QByteArray getRGB32fromRLE24(const QByteArray & encodedBytes, quint32 expectedPixelCount);

private:
    int m_currentIconIndex;
    QDataStream m_stream;
    QVector<QIcnsIconEntry> m_icons;
    QVector<QIcnsIconEntry> m_masks;
    bool m_scanned;

    bool scanDevice();
    QImage iconAlphaAt(int index);

    bool addIcon(QIcnsIconEntry &icon);
};

#endif //ICNSHANDLER_H
