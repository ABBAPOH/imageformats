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

#ifndef QICNSHANDLER_H
#define QICNSHANDLER_H

#include <QtGui/QImageIOHandler>
#include <QtGui/QImage>
#include <QtCore/QVector>
#include <QtCore/QBuffer>
#include <QtCore/QtMath>
#include <QtCore/QRegularExpression>
#include <QtCore/QtEndian>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

struct IcnsBlockHeader
{
    enum SpecificOSTypes {
        OSType_icns = 0x69636E73, // Icns container magic
        OSType_TOC_ = 0x544F4320, // Table of contents
        OSType_icnV = 0x69636E56, // Version of the creating tool
        // Legacy:
        OSType_clut = 0x636c7574  // [NYI] Color look-up table
    };

    quint32 OSType;
    quint32 length;
};

struct IcnsEntry
{
    enum IconGroup {
        IconGroupUnknown    = 0,
        IconGroupMini       = 0x6D, // "m" for "mini" (16x12)
        IconGroupSmall      = 0x73, // "s" for "small" (16x16)
        IconGroupLarge      = 0x6C, // "l" for "large" (32x32)
        IconGroupHuge       = 0x68, // "h" for "huge" (48x48)
        IconGroupThumbnail  = 0x74, // "t" for "thumbnail" (128x128)
        IconGroupPortable   = 0x70, // "p" fpr "portable"? (various sizes, png/jp2)
        IconGroupCompressed = 0x63, // "c" for "compressed"? (various sizes, png/jp2)
        // Legacy icons:
        IconGroupICON       = 0x4E, // [SUPPORTED] "N" from OSType "ICON" (32x32) and [NYI] "SICN" (0x5349434e)
        IconGroupOpen       = 0x6E, // [NYI, UNKNOWN] "n" from OSType "open" (0x6f70656e), "cicn" (0x6369636e), "Icon" (0x49636f6e)
        IconGroupTile       = 0x65, // [NYI, UNKNOWN] "e" from OSType "tile" (ostype: 0x74696c65)
        IconGroupDrop       = 0x70, // [NYI, UNKNOWN] "p" from OSTypes "drop" and "odrp" (0x64726f70, 0x6f647270)
        IconGroupOver       = 0x72  // [NYI, UNKNOWN] "r" from OSType "over" (ostype: 0x6f766572)
    };
    enum Depth {
        IconDepthUnknown    = 0,
        IconMono            = 1,
        Icon4bit            = 4,
        Icon8bit            = 8,
        Icon32bit           = 32
    };
    enum IconMaskType {
        IconMaskUnknown,
        IconNoMask,     // Plain icon without alpha
        IconPlusMask,   // Plain icon and alpha mask (double size)
        IconIsMask      // The whole icon entry is alpha mask
    };

    IcnsBlockHeader header; // Original block header
    IconGroup group;        // ASCII character number pointing to a format
    Depth depth;            // Color depth for uncompr. icons or icon format num for compressed
    IconMaskType mask;      // For Uncompressed icons only
    quint32 width;          // For Uncompressed icons only
    quint32 height;         // For Uncompressed icons only
    bool isValid;           // True if correctly parsed
    quint32 dataLength;     // header.length - sizeof(header)
    quint32 dataOffset;     // Offset from the initial position of the file/device
    bool dataIsRLE;         // 32bit raw icons may be in rle24 compressed state
};

class QIcnsHandler : public QImageIOHandler
{
public:
    QIcnsHandler();

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
    bool ensureScanned() const;
    bool scanDevice();
    bool addEntry(const IcnsBlockHeader &header, quint32 imgDataOffset);
    IcnsEntry getIconMask(const IcnsEntry &icon) const;

private:
    int m_currentIconIndex;
    QVector<IcnsEntry> m_icons;
    QVector<IcnsEntry> m_masks;
    bool m_parsed;
};

QT_END_NAMESPACE

#endif /* QICNSHANDLER_H */
