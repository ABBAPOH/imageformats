/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2013 Alex Char.
** Contact: http://www.qt-project.org/legal
**
** This file is part of the ICNS plugin in the Qt ImageFormats module.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QICNSHANDLER_P_H
#define QICNSHANDLER_P_H

#include <QtGui/QImageIOHandler>
#include <QtGui/QImage>
#include <QtCore/QVector>

#ifndef MAKEOSTYPE
#define MAKEOSTYPE(c0,c1,c2,c3) (((quint8)c0 << 24) | ((quint8)c1 << 16) | ((quint8)c2 << 8) | (quint8)c3)
#endif

QT_BEGIN_NAMESPACE

struct ICNSBlockHeader
{
    enum OS {
        TypeIcns = MAKEOSTYPE('i', 'c', 'n', 's'), // Icns container magic
        TypeToc  = MAKEOSTYPE('T', 'O', 'C', ' '), // Table of contents
        TypeIcnv = MAKEOSTYPE('i', 'c', 'n', 'V'), // Version of the icns tool
        // Legacy:
        TypeClut = MAKEOSTYPE('c', 'l', 'u', 't')  // Color look-up table (pre-OSX resources)
    };

    quint32 ostype;
    quint32 length;
};

struct ICNSEntry
{
    enum Group {
        GroupUnknown    = 0,
        GroupMini       = 'm', // "mini" (16x12)
        GroupSmall      = 's', // "small" (16x16)
        GroupLarge      = 'l', // "large" (32x32)
        GroupHuge       = 'h', // "huge" (48x48)
        GroupThumbnail  = 't', // "thumbnail" (128x128)
        GroupPortable   = 'p', // "portable"? (various sizes, png/jp2)
        GroupCompressed = 'c', // "compressed"? (various sizes, png/jp2)
        // Legacy icons:
        GroupICON       = 'N', // "ICON" (32x32)
    };
    enum Depth {
        DepthUnknown    = 0,    // Default for invalid ones
        DepthMono       = 1,
        Depth4bit       = 4,
        Depth8bit       = 8,
        Depth32bit      = 32
    };
    enum Mask {
        MaskUnknown     = 0x0,              // Default for invalid ones
        IsIcon          = 0x1,              // Plain icon without alpha
        IsMask          = 0x2,              // The whole icon entry is alpha mask
        IconPlusMask    = IsMask | IsIcon   // Plain icon and alpha mask (double size)
    };

    ICNSBlockHeader header; // Original block header
    Group group;            // ASCII character number pointing to a format
    Depth depth;            // Color depth or icon format number for compressed icons
    Mask mask;              // Flags for uncompressed, should be always IsIcon (0x1) for compressed
    quint32 width;          // For uncompressed icons only, zero for compressed ones for now
    quint32 height;         // For uncompressed icons only, zero for compressed ones fow now
    quint32 dataLength;     // header.length - sizeof(header)
    quint32 dataOffset;     // Offset from the initial position of the file/device
    bool dataIsRLE;         // 32bit raw icons may be in rle24 compressed state
};

class QICNSHandler : public QImageIOHandler
{
public:
    QICNSHandler();

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

private:
    bool ensureScanned() const;
    bool scanDevice();
    void addEntry(const ICNSBlockHeader &header, quint32 imgDataOffset);
    const ICNSEntry &getIconMask(const ICNSEntry &icon) const;

private:
    enum ScanState {
        ScanError       = -1,
        ScanNotScanned  = 0,
        ScanSuccess     = 1,
    };

    int m_currentIconIndex;
    QVector<ICNSEntry> m_icons;
    QVector<ICNSEntry> m_masks;
    ScanState m_state;
};

QT_END_NAMESPACE

#endif /* QICNSHANDLER_P_H */
