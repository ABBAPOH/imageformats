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

#include "qicnshandler.h"

static const quint8 ICNSBlockHeaderSize = 8;
static const char ICNSMagicPNGHex[] = "89504e470d0a1a0a";
static const char ICNSMagicJP2Hex[] = "0000000c6a5020200d0a870a";

static const QRgb ICNSColorTableMono[] = {
    qRgb(0xFF, 0xFF, 0xFF),
    qRgb(0x00, 0x00, 0x00)
};
Q_STATIC_ASSERT(sizeof(ICNSColorTableMono) == (sizeof(QRgb) * 2));

static const QRgb ICNSColorTable4bit[] = {
    qRgb(0xFF, 0xFF, 0xFF),
    qRgb(0xFC, 0xF3, 0x05),
    qRgb(0xFF, 0x64, 0x02),
    qRgb(0xDD, 0x08, 0x06),
    qRgb(0xF2, 0x08, 0x84),
    qRgb(0x46, 0x00, 0xA5),
    qRgb(0x00, 0x00, 0xD4),
    qRgb(0x02, 0xAB, 0xEA),
    qRgb(0x1F, 0xB7, 0x14),
    qRgb(0x00, 0x64, 0x11),
    qRgb(0x56, 0x2C, 0x05),
    qRgb(0x90, 0x71, 0x3A),
    qRgb(0xC0, 0xC0, 0xC0),
    qRgb(0x80, 0x80, 0x80),
    qRgb(0x40, 0x40, 0x40),
    qRgb(0x00, 0x00, 0x00)
};
Q_STATIC_ASSERT(sizeof(ICNSColorTable4bit) == (sizeof(QRgb) * 16));

static const QRgb ICNSColorTable8bit[] = {
    qRgb(0xFF, 0xFF, 0xFF),
    qRgb(0xFF, 0xFF, 0xCC),
    qRgb(0xFF, 0xFF, 0x99),
    qRgb(0xFF, 0xFF, 0x66),
    qRgb(0xFF, 0xFF, 0x33),
    qRgb(0xFF, 0xFF, 0x00),
    qRgb(0xFF, 0xCC, 0xFF),
    qRgb(0xFF, 0xCC, 0xCC),
    qRgb(0xFF, 0xCC, 0x99),
    qRgb(0xFF, 0xCC, 0x66),
    qRgb(0xFF, 0xCC, 0x33),
    qRgb(0xFF, 0xCC, 0x00),
    qRgb(0xFF, 0x99, 0xFF),
    qRgb(0xFF, 0x99, 0xCC),
    qRgb(0xFF, 0x99, 0x99),
    qRgb(0xFF, 0x99, 0x66),
    qRgb(0xFF, 0x99, 0x33),
    qRgb(0xFF, 0x99, 0x00),
    qRgb(0xFF, 0x66, 0xFF),
    qRgb(0xFF, 0x66, 0xCC),
    qRgb(0xFF, 0x66, 0x99),
    qRgb(0xFF, 0x66, 0x66),
    qRgb(0xFF, 0x66, 0x33),
    qRgb(0xFF, 0x66, 0x00),
    qRgb(0xFF, 0x33, 0xFF),
    qRgb(0xFF, 0x33, 0xCC),
    qRgb(0xFF, 0x33, 0x99),
    qRgb(0xFF, 0x33, 0x66),
    qRgb(0xFF, 0x33, 0x33),
    qRgb(0xFF, 0x33, 0x00),
    qRgb(0xFF, 0x00, 0xFF),
    qRgb(0xFF, 0x00, 0xCC),
    qRgb(0xFF, 0x00, 0x99),
    qRgb(0xFF, 0x00, 0x66),
    qRgb(0xFF, 0x00, 0x33),
    qRgb(0xFF, 0x00, 0x00),
    qRgb(0xCC, 0xFF, 0xFF),
    qRgb(0xCC, 0xFF, 0xCC),
    qRgb(0xCC, 0xFF, 0x99),
    qRgb(0xCC, 0xFF, 0x66),
    qRgb(0xCC, 0xFF, 0x33),
    qRgb(0xCC, 0xFF, 0x00),
    qRgb(0xCC, 0xCC, 0xFF),
    qRgb(0xCC, 0xCC, 0xCC),
    qRgb(0xCC, 0xCC, 0x99),
    qRgb(0xCC, 0xCC, 0x66),
    qRgb(0xCC, 0xCC, 0x33),
    qRgb(0xCC, 0xCC, 0x00),
    qRgb(0xCC, 0x99, 0xFF),
    qRgb(0xCC, 0x99, 0xCC),
    qRgb(0xCC, 0x99, 0x99),
    qRgb(0xCC, 0x99, 0x66),
    qRgb(0xCC, 0x99, 0x33),
    qRgb(0xCC, 0x99, 0x00),
    qRgb(0xCC, 0x66, 0xFF),
    qRgb(0xCC, 0x66, 0xCC),
    qRgb(0xCC, 0x66, 0x99),
    qRgb(0xCC, 0x66, 0x66),
    qRgb(0xCC, 0x66, 0x33),
    qRgb(0xCC, 0x66, 0x00),
    qRgb(0xCC, 0x33, 0xFF),
    qRgb(0xCC, 0x33, 0xCC),
    qRgb(0xCC, 0x33, 0x99),
    qRgb(0xCC, 0x33, 0x66),
    qRgb(0xCC, 0x33, 0x33),
    qRgb(0xCC, 0x33, 0x00),
    qRgb(0xCC, 0x00, 0xFF),
    qRgb(0xCC, 0x00, 0xCC),
    qRgb(0xCC, 0x00, 0x99),
    qRgb(0xCC, 0x00, 0x66),
    qRgb(0xCC, 0x00, 0x33),
    qRgb(0xCC, 0x00, 0x00),
    qRgb(0x99, 0xFF, 0xFF),
    qRgb(0x99, 0xFF, 0xCC),
    qRgb(0x99, 0xFF, 0x99),
    qRgb(0x99, 0xFF, 0x66),
    qRgb(0x99, 0xFF, 0x33),
    qRgb(0x99, 0xFF, 0x00),
    qRgb(0x99, 0xCC, 0xFF),
    qRgb(0x99, 0xCC, 0xCC),
    qRgb(0x99, 0xCC, 0x99),
    qRgb(0x99, 0xCC, 0x66),
    qRgb(0x99, 0xCC, 0x33),
    qRgb(0x99, 0xCC, 0x00),
    qRgb(0x99, 0x99, 0xFF),
    qRgb(0x99, 0x99, 0xCC),
    qRgb(0x99, 0x99, 0x99),
    qRgb(0x99, 0x99, 0x66),
    qRgb(0x99, 0x99, 0x33),
    qRgb(0x99, 0x99, 0x00),
    qRgb(0x99, 0x66, 0xFF),
    qRgb(0x99, 0x66, 0xCC),
    qRgb(0x99, 0x66, 0x99),
    qRgb(0x99, 0x66, 0x66),
    qRgb(0x99, 0x66, 0x33),
    qRgb(0x99, 0x66, 0x00),
    qRgb(0x99, 0x33, 0xFF),
    qRgb(0x99, 0x33, 0xCC),
    qRgb(0x99, 0x33, 0x99),
    qRgb(0x99, 0x33, 0x66),
    qRgb(0x99, 0x33, 0x33),
    qRgb(0x99, 0x33, 0x00),
    qRgb(0x99, 0x00, 0xFF),
    qRgb(0x99, 0x00, 0xCC),
    qRgb(0x99, 0x00, 0x99),
    qRgb(0x99, 0x00, 0x66),
    qRgb(0x99, 0x00, 0x33),
    qRgb(0x99, 0x00, 0x00),
    qRgb(0x66, 0xFF, 0xFF),
    qRgb(0x66, 0xFF, 0xCC),
    qRgb(0x66, 0xFF, 0x99),
    qRgb(0x66, 0xFF, 0x66),
    qRgb(0x66, 0xFF, 0x33),
    qRgb(0x66, 0xFF, 0x00),
    qRgb(0x66, 0xCC, 0xFF),
    qRgb(0x66, 0xCC, 0xCC),
    qRgb(0x66, 0xCC, 0x99),
    qRgb(0x66, 0xCC, 0x66),
    qRgb(0x66, 0xCC, 0x33),
    qRgb(0x66, 0xCC, 0x00),
    qRgb(0x66, 0x99, 0xFF),
    qRgb(0x66, 0x99, 0xCC),
    qRgb(0x66, 0x99, 0x99),
    qRgb(0x66, 0x99, 0x66),
    qRgb(0x66, 0x99, 0x33),
    qRgb(0x66, 0x99, 0x00),
    qRgb(0x66, 0x66, 0xFF),
    qRgb(0x66, 0x66, 0xCC),
    qRgb(0x66, 0x66, 0x99),
    qRgb(0x66, 0x66, 0x66),
    qRgb(0x66, 0x66, 0x33),
    qRgb(0x66, 0x66, 0x00),
    qRgb(0x66, 0x33, 0xFF),
    qRgb(0x66, 0x33, 0xCC),
    qRgb(0x66, 0x33, 0x99),
    qRgb(0x66, 0x33, 0x66),
    qRgb(0x66, 0x33, 0x33),
    qRgb(0x66, 0x33, 0x00),
    qRgb(0x66, 0x00, 0xFF),
    qRgb(0x66, 0x00, 0xCC),
    qRgb(0x66, 0x00, 0x99),
    qRgb(0x66, 0x00, 0x66),
    qRgb(0x66, 0x00, 0x33),
    qRgb(0x66, 0x00, 0x00),
    qRgb(0x33, 0xFF, 0xFF),
    qRgb(0x33, 0xFF, 0xCC),
    qRgb(0x33, 0xFF, 0x99),
    qRgb(0x33, 0xFF, 0x66),
    qRgb(0x33, 0xFF, 0x33),
    qRgb(0x33, 0xFF, 0x00),
    qRgb(0x33, 0xCC, 0xFF),
    qRgb(0x33, 0xCC, 0xCC),
    qRgb(0x33, 0xCC, 0x99),
    qRgb(0x33, 0xCC, 0x66),
    qRgb(0x33, 0xCC, 0x33),
    qRgb(0x33, 0xCC, 0x00),
    qRgb(0x33, 0x99, 0xFF),
    qRgb(0x33, 0x99, 0xCC),
    qRgb(0x33, 0x99, 0x99),
    qRgb(0x33, 0x99, 0x66),
    qRgb(0x33, 0x99, 0x33),
    qRgb(0x33, 0x99, 0x00),
    qRgb(0x33, 0x66, 0xFF),
    qRgb(0x33, 0x66, 0xCC),
    qRgb(0x33, 0x66, 0x99),
    qRgb(0x33, 0x66, 0x66),
    qRgb(0x33, 0x66, 0x33),
    qRgb(0x33, 0x66, 0x00),
    qRgb(0x33, 0x33, 0xFF),
    qRgb(0x33, 0x33, 0xCC),
    qRgb(0x33, 0x33, 0x99),
    qRgb(0x33, 0x33, 0x66),
    qRgb(0x33, 0x33, 0x33),
    qRgb(0x33, 0x33, 0x00),
    qRgb(0x33, 0x00, 0xFF),
    qRgb(0x33, 0x00, 0xCC),
    qRgb(0x33, 0x00, 0x99),
    qRgb(0x33, 0x00, 0x66),
    qRgb(0x33, 0x00, 0x33),
    qRgb(0x33, 0x00, 0x00),
    qRgb(0x00, 0xFF, 0xFF),
    qRgb(0x00, 0xFF, 0xCC),
    qRgb(0x00, 0xFF, 0x99),
    qRgb(0x00, 0xFF, 0x66),
    qRgb(0x00, 0xFF, 0x33),
    qRgb(0x00, 0xFF, 0x00),
    qRgb(0x00, 0xCC, 0xFF),
    qRgb(0x00, 0xCC, 0xCC),
    qRgb(0x00, 0xCC, 0x99),
    qRgb(0x00, 0xCC, 0x66),
    qRgb(0x00, 0xCC, 0x33),
    qRgb(0x00, 0xCC, 0x00),
    qRgb(0x00, 0x99, 0xFF),
    qRgb(0x00, 0x99, 0xCC),
    qRgb(0x00, 0x99, 0x99),
    qRgb(0x00, 0x99, 0x66),
    qRgb(0x00, 0x99, 0x33),
    qRgb(0x00, 0x99, 0x00),
    qRgb(0x00, 0x66, 0xFF),
    qRgb(0x00, 0x66, 0xCC),
    qRgb(0x00, 0x66, 0x99),
    qRgb(0x00, 0x66, 0x66),
    qRgb(0x00, 0x66, 0x33),
    qRgb(0x00, 0x66, 0x00),
    qRgb(0x00, 0x33, 0xFF),
    qRgb(0x00, 0x33, 0xCC),
    qRgb(0x00, 0x33, 0x99),
    qRgb(0x00, 0x33, 0x66),
    qRgb(0x00, 0x33, 0x33),
    qRgb(0x00, 0x33, 0x00),
    qRgb(0x00, 0x00, 0xFF),
    qRgb(0x00, 0x00, 0xCC),
    qRgb(0x00, 0x00, 0x99),
    qRgb(0x00, 0x00, 0x66),
    qRgb(0x00, 0x00, 0x33),
    qRgb(0xEE, 0x00, 0x00),
    qRgb(0xDD, 0x00, 0x00),
    qRgb(0xBB, 0x00, 0x00),
    qRgb(0xAA, 0x00, 0x00),
    qRgb(0x88, 0x00, 0x00),
    qRgb(0x77, 0x00, 0x00),
    qRgb(0x55, 0x00, 0x00),
    qRgb(0x44, 0x00, 0x00),
    qRgb(0x22, 0x00, 0x00),
    qRgb(0x11, 0x00, 0x00),
    qRgb(0x00, 0xEE, 0x00),
    qRgb(0x00, 0xDD, 0x00),
    qRgb(0x00, 0xBB, 0x00),
    qRgb(0x00, 0xAA, 0x00),
    qRgb(0x00, 0x88, 0x00),
    qRgb(0x00, 0x77, 0x00),
    qRgb(0x00, 0x55, 0x00),
    qRgb(0x00, 0x44, 0x00),
    qRgb(0x00, 0x22, 0x00),
    qRgb(0x00, 0x11, 0x00),
    qRgb(0x00, 0x00, 0xEE),
    qRgb(0x00, 0x00, 0xDD),
    qRgb(0x00, 0x00, 0xBB),
    qRgb(0x00, 0x00, 0xAA),
    qRgb(0x00, 0x00, 0x88),
    qRgb(0x00, 0x00, 0x77),
    qRgb(0x00, 0x00, 0x55),
    qRgb(0x00, 0x00, 0x44),
    qRgb(0x00, 0x00, 0x22),
    qRgb(0x00, 0x00, 0x11),
    qRgb(0xEE, 0xEE, 0xEE),
    qRgb(0xDD, 0xDD, 0xDD),
    qRgb(0xBB, 0xBB, 0xBB),
    qRgb(0xAA, 0xAA, 0xAA),
    qRgb(0x88, 0x88, 0x88),
    qRgb(0x77, 0x77, 0x77),
    qRgb(0x55, 0x55, 0x55),
    qRgb(0x44, 0x44, 0x44),
    qRgb(0x22, 0x22, 0x22),
    qRgb(0x11, 0x11, 0x11),
    qRgb(0x00, 0x00, 0x00)
};
Q_STATIC_ASSERT(sizeof(ICNSColorTable8bit) == (sizeof(QRgb) * 256));

static inline QDataStream &operator>>(QDataStream &in, ICNSBlockHeader &p)
{
    in >> p.ostype;
    in >> p.length;
    return in;
}

static inline QDataStream &operator<<(QDataStream &out, const ICNSBlockHeader &p)
{
    out << p.ostype;
    out << p.length;
    return out;
}

static inline QVector<QRgb> getColorTable(ICNSEntry::Depth depth)
{
    QVector<QRgb> table;
    uint n = (1 << depth);
    const QRgb *data;
    switch (depth) {
    case ICNSEntry::DepthMono:
        data = ICNSColorTableMono;
        break;
    case ICNSEntry::Depth4bit:
        data = ICNSColorTable4bit;
        break;
    case ICNSEntry::Depth8bit:
        data = ICNSColorTable8bit;
        break;
    default:
        qWarning("getColorTable(): No color table for bit depth: %u", depth);
        return table;
    }
    table.resize(n);
    memcpy(table.data(), data, (sizeof(QRgb) * n));
    return table;
}

static bool parseIconEntry(ICNSEntry &icon)
{
    const quint32 ostypebo = qToBigEndian<quint32>(icon.header.ostype);
    const QByteArray ostype = QByteArray::fromRawData((const char*)&ostypebo, 4);
    // Typical OSType naming: <junk><group><depth><mask>;
    const QString ptrn = QStringLiteral("^(?<junk>[\\D]{0,4})(?<group>[a-z|A-Z]{1})(?<depth>\\d{0,2})(?<mask>[#mk]{0,2})$");
    QRegularExpression regexp(ptrn);
    QRegularExpressionMatch match = regexp.match(ostype);
    const bool hasMatch = match.hasMatch();
    const QString group = match.captured("group");
    const QString depth = match.captured("depth");
    const QString mask = match.captured("mask");
    // Icon group:
    icon.group = group.isEmpty() ? ICNSEntry::GroupUnknown : ICNSEntry::Group(group.at(0).toLatin1());
    // Icon depth:
    icon.depth = depth.isEmpty() ? ICNSEntry::DepthUnknown : ICNSEntry::Depth(depth.toUInt());
    // Width/height/mask:
    icon.width = 0;
    icon.height = 0;
    icon.mask = ICNSEntry::MaskUnknown;
    if (icon.group != ICNSEntry::GroupCompressed && icon.group != ICNSEntry::GroupPortable) {
        if (icon.depth == ICNSEntry::DepthUnknown)
            icon.depth = ICNSEntry::DepthMono;
        const qreal bytespp = ((qreal)icon.depth / 8);
        const qreal r1 = qSqrt(icon.dataLength / bytespp);
        const qreal r2 = qSqrt((icon.dataLength / bytespp) / 2);
        const quint32 r1u = qRound(r1);
        const quint32 r2u = qRound(r2);
        const bool r1IsPowOfTwoOrDevidesBy16 = (r1u == r1 && r1u % 16 == 0)
                || (r1u == r1 && r1 >= 16 && ((r1u & (r1u - 1)) == 0));
        const bool r2IsPowOfTwoOrDevidesBy16 = (r2u == r2 && r2u % 16 == 0)
                || (r2u == r2 && r2 >= 16 && ((r2u & (r2u - 1)) == 0));

        if (r1IsPowOfTwoOrDevidesBy16) {
            icon.mask = mask.isEmpty() ? ICNSEntry::IsIcon : ICNSEntry::IsMask;
            icon.width = r1;
            icon.height = r1;
        } else if (r2IsPowOfTwoOrDevidesBy16) {
            icon.mask = ICNSEntry::IconPlusMask;
            icon.width = r2;
            icon.height = r2;
        } else if (icon.group == ICNSEntry::GroupMini) {
            // Legacy 16x12 icons are an exception from the generic square formula
            const bool doubleSize = (icon.dataLength == (192 * bytespp * 2));
            icon.mask = doubleSize ? ICNSEntry::IconPlusMask : ICNSEntry::IsIcon;
            icon.width = 16;
            icon.height = 12;
        } else if (icon.depth == ICNSEntry::Depth32bit) {
            // 32bit icon may be encoded
            icon.dataIsRLE = true;
            icon.mask = mask.isEmpty() ? ICNSEntry::IsIcon : ICNSEntry::IsMask;
            switch (icon.group) {
            case ICNSEntry::GroupSmall:
                icon.width = 16;
                icon.height = 16;
                break;
            case ICNSEntry::GroupLarge:
                icon.width = 32;
                icon.height = 32;
                break;
            case ICNSEntry::GroupHuge:
                icon.width = 48;
                icon.height = 48;
                break;
            case ICNSEntry::GroupThumbnail:
                icon.width = 128;
                icon.height = 128;
                break;
            default:
                qWarning("parseIconEntry(): Failed, 32bit icon from an unknown group. OSType: \"%s\"",
                         ostype.constData());
            }
        }
    } else {
        // TODO: Add parsing of png/jp2 headers to enable feature reporting by plugin?
        icon.mask = ICNSEntry::IsIcon;
    }
    if (!hasMatch)
        qWarning("parseIconEntry(): Failed, OSType doesn't match: \"%s\"",
                 ostype.constData());
    return hasMatch;
}

static QImage readMaskFromStream(const ICNSEntry &mask, QDataStream &stream)
{
    if ((mask.mask & ICNSEntry::IsMask) == 0 || !mask.isValid)
        return QImage();
    if (mask.depth != ICNSEntry::DepthMono && mask.depth != ICNSEntry::Depth8bit) {
        qWarning("readMaskFromStream(): Failed, unusual bit depth: %u OSType: %u",
                 mask.depth, mask.header.ostype);
        return QImage();
    }
    const bool doubleSize = (mask.mask == ICNSEntry::IconPlusMask);
    const quint32 imageDataSize = ((mask.width * mask.height * mask.depth) / 8);
    const qint64 pos = doubleSize ? (mask.dataOffset + imageDataSize) : mask.dataOffset;
    const qint64 oldPos = stream.device()->pos();
    if (!stream.device()->seek(pos))
        return QImage();
    QImage img(mask.width, mask.height, QImage::Format_RGB32);
    quint8 byte = 0;
    quint32 pixel = 0;
    for (quint32 y = 0; y < mask.height; y++) {
        QRgb *line = reinterpret_cast<QRgb *>(img.scanLine(y));
        for (quint32 x = 0; x < mask.width; x++) {
            if (pixel % (8 / mask.depth) == 0)
                stream >> byte;
            quint8 alpha = (mask.depth == ICNSEntry::DepthMono) ? (byte >> 7) * 0xFF : byte;
            byte = (byte << 1);
            line[x] = qRgb(alpha, alpha, alpha);
            pixel++;
        }
    }
    stream.device()->seek(oldPos);
    return img;
}

static QImage readLowDepthIconFromStream(const ICNSEntry &icon, QDataStream &stream)
{
    quint8 byte = 0;
    const bool isMono = (icon.depth == ICNSEntry::DepthMono);
    const QImage::Format format = isMono ? QImage::Format_Mono : QImage::Format_Indexed8;
    const QVector<QRgb> colortable = getColorTable(icon.depth);
    if (colortable.isEmpty())
        return QImage();
    QImage img(icon.width, icon.height, format);
    img.setColorTable(colortable);
    quint32 pixel = 0;
    for (quint32 y = 0; y < icon.height; y++) {
        for (quint32 x = 0; x < icon.width; x++) {
            if (pixel % (8 / icon.depth) == 0)
                stream >> byte;
            quint8 cindex = 0;
            switch (icon.depth) {
            case ICNSEntry::DepthMono: {
                cindex = (byte & 0x80) ? 1 : 0; // left 1 bit
                break;
            }

            case ICNSEntry::Depth4bit: {
                quint8 value = ((byte & 0xF0) >> 4); // left 4 bits
                cindex = (value < (1 << icon.depth)) ? value : 0;
                break;
            }

            default:
                cindex = (byte < (1 << icon.depth)) ? byte : 0;
            }
            byte = byte << icon.depth;
            img.setPixel(x, y, cindex);
            pixel++;
        }
    }
    return img;
}

static QImage read32bitIconFromStream(const ICNSEntry &icon, QDataStream &stream)
{
    QImage img = QImage(icon.width, icon.height, QImage::Format_RGB32);
    if (!icon.dataIsRLE) {
        for (quint32 y = 0; y < icon.height; y++) {
            QRgb *line = reinterpret_cast<QRgb *>(img.scanLine(y));
            for (quint32 x = 0; x < icon.width; x++) {
                quint8 r, g, b, a;
                stream >> r >> g >> b >> a;
                line[x] = qRgb(r, g, b);
            }
        }
    } else {
        const quint32 estPxsNum = (icon.width * icon.height);
        const QByteArray &bytes = stream.device()->peek(4);
        if (bytes.isEmpty())
            return QImage();
        // Zero-padding may be present:
        if (qFromBigEndian<quint32>(*bytes.constData()) == 0)
            stream.skipRawData(4);
        for (quint8 colorNRun = 0; colorNRun < 3; colorNRun++) {
            quint32 pixel = 0;
            QRgb *line;
            while ((pixel < estPxsNum) && !stream.atEnd()) {
                quint8 byte, value;
                stream >> byte;
                const bool bitIsClear = ((byte & 0x80) == 0);
                // If high bit is clear: run of different values; else: same value
                quint8 runLength = bitIsClear ? ((0xFF & byte) + 1) : ((0xFF & byte) - 125);
                // Length of the run for for different values: 1 <= len <= 128
                // Length of the run for same values: 3 <= len <= 130
                if (!bitIsClear)
                    stream >> value;
                for (quint8 i = 0; (i < runLength) && (pixel < estPxsNum); i++) {
                    if (bitIsClear)
                        stream >> value;
                    const quint32 y = (pixel / icon.height);
                    const quint32 x = (pixel - (icon.width * y));
                    if (pixel % icon.height == 0)
                        line = reinterpret_cast<QRgb *>(img.scanLine(y));
                    QRgb rgb = line[x];
                    const int r = (colorNRun == 0) ? value : qRed(rgb);
                    const int g = (colorNRun == 1) ? value : qGreen(rgb);
                    const int b = (colorNRun == 2) ? value : qBlue(rgb);
                    line[x] = qRgb(r, g, b);
                    pixel++;
                }
            }
        }
    }
    return img;
}

QICNSHandler::QICNSHandler() :
    m_currentIconIndex(0), m_scanned(false)
{
}

QByteArray QICNSHandler::name() const
{
    return QByteArrayLiteral("icns");
}

bool QICNSHandler::canRead(QIODevice *device)
{
    if (!device || !device->isReadable()) {
        qWarning("QICNSHandler::canRead() called without a readable device");
        return false;
    }
    if (device->isSequential()) {
        qWarning("QICNSHandler::canRead() called on a sequential device (NYI)");
        return false;
    }
    return (device->peek(4) == QByteArrayLiteral("icns"));
}

bool QICNSHandler::canRead() const
{
    if (canRead(device())) {
        setFormat(QByteArrayLiteral("icns"));
        return true;
    }
    return false;
}

bool QICNSHandler::read(QImage *outImage)
{
    QImage img;
    if (!ensureScanned()) {
        qWarning("QICNSHandler::read(): The device was not parced properly!");
        return false;
    }

    const ICNSEntry &icon = m_icons.at(m_currentIconIndex);
    QDataStream stream(device());
    stream.setByteOrder(QDataStream::BigEndian);
    if (!device()->seek(icon.dataOffset))
        return false;

    const QByteArray magicHex = device()->peek(12).toHex();
    const bool isPNG = magicHex.startsWith(ICNSMagicPNGHex);
    const bool isJP2 = (magicHex == ICNSMagicJP2Hex);
    const bool isPortable = (icon.group == ICNSEntry::GroupPortable);
    const bool isCompressed = (isPortable || (icon.group == ICNSEntry::GroupCompressed));
    if (isPNG || isJP2 || isCompressed) {
        const QByteArray ba = device()->read(icon.dataLength);
        if (ba.isEmpty()) {
            qWarning("QICNSHandler::read(): Failed, compressed image data is empty. OSType: %u",
                     icon.header.ostype);
            return false;
        }
        if (isPNG || isJP2) {
            const char *format = isPNG ? "png" : "jp2";
            img = QImage::fromData(ba, format);
            if (img.isNull()) {
                qWarning("QICNSHandler::read(): Failed, format \"%s\" is not supported by your Qt lib. OSType: %u",
                         format, icon.header.ostype);
            }
        } else {
            // Try anyway
            img = QImage::fromData(ba);
            if (img.isNull()) {
                qWarning("QICNSHandler::read(): Failed, unsupported compressed icon format, OSType: %u",
                         icon.header.ostype);
            }
        }
    } else if (icon.height == 0 || icon.width == 0) {
        qWarning("QICNSHandler::read(): Failed, size of a raw icon is unknown, OSType: %u",
                 icon.header.ostype);
    } else {
        switch (icon.depth) {
        case ICNSEntry::DepthMono:
        case ICNSEntry::Depth4bit:
        case ICNSEntry::Depth8bit:
            img = readLowDepthIconFromStream(icon, stream);
            break;
        case ICNSEntry::Depth32bit:
            img = read32bitIconFromStream(icon, stream);
            break;
        default:
            qWarning("QICNSHandler::read(): Failed, unsupported icon bit depth: %u, OSType: %u",
                     icon.depth, icon.header.ostype);
        }
    }
    if (!img.isNull()) {
        QImage alpha = readMaskFromStream(getIconMask(icon), stream);
        if (!alpha.isNull())
            img.setAlphaChannel(alpha);
    }
    *outImage = img;
    return !img.isNull();
}

bool QICNSHandler::write(const QImage &image)
{
    QIODevice *device = this->device();
    // NOTE: Experimental implementation. Just for simple converting tasks / testing purposes.
    // LIMITATIONS: Writes a complete icns file containing only one square icon in PNG format to a device.
    // Currently uses non-hardcoded OSTypes.
    QImage img = image;
    const int width = img.size().width();
    const int height = img.size().height();
    const bool sizeIsCorrect = (width == height) && (width >= 16) && ((width & (width - 1)) == 0);
    if (!device->isWritable() || !sizeIsCorrect)
        return false;
    // Construct icon OSType
    int i = width;
    uint p = 0;
    while (i >>= 1)
        p++;
    if (p > 10) {
        // Force resizing to 1024x1024. Values over 10 are reserved for retina icons
        p = 10;
        img = img.scaled(1024, 1024, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    // Small / big icons naming policy
    const QByteArray ostypeb = (p < 7) ? QByteArrayLiteral("icp") : QByteArrayLiteral("ic");
    const bool noZero = (ostypeb.size() > 2 || p >= 10);
    const QByteArray ostypen =  noZero ? QByteArray::number(p) : "0" + QByteArray::number(p);
    const quint32 ostype = qFromBigEndian<quint32>(*(quint32*)((ostypeb + ostypen).constData()));
    // Construct ICNS Header
    ICNSBlockHeader fileHeader;
    fileHeader.ostype = ICNSBlockHeader::icns;
    // Construct TOC Header
    ICNSBlockHeader tocHeader;
    tocHeader.ostype = ICNSBlockHeader::TOC_;
    // Construct TOC Entry
    ICNSBlockHeader tocEntry;
    tocEntry.ostype = ostype;
    // Construct Icon block
    ICNSBlockHeader iconEntry;
    iconEntry.ostype = ostype;
    // Construct image data
    QByteArray imageData;
    QBuffer buffer(&imageData);
    if (!buffer.open(QIODevice::WriteOnly) || !img.save(&buffer, "png"))
        return false;
    buffer.close();
    iconEntry.length = (ICNSBlockHeaderSize + imageData.size());
    tocEntry.length = iconEntry.length;
    tocHeader.length = (ICNSBlockHeaderSize * 2);
    fileHeader.length = (ICNSBlockHeaderSize + tocHeader.length + iconEntry.length);
    // Write everything
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << fileHeader << tocHeader << tocEntry << iconEntry;
    stream.writeRawData(imageData.constData(), imageData.size());
    if (stream.status() != QDataStream::Ok)
        return false;
    return true;
}

bool QICNSHandler::supportsOption(QImageIOHandler::ImageOption option) const
{
    return (option == QImageIOHandler::Name || option == QImageIOHandler::SubType);
}

QVariant QICNSHandler::option(QImageIOHandler::ImageOption option) const
{
    if (!supportsOption(option) || !ensureScanned())
        return QVariant();
    if (imageCount() > 0 && m_currentIconIndex <= imageCount()) {
        const ICNSEntry &entry = m_icons.at(m_currentIconIndex);
        return QByteArray::fromRawData((const char*)qToBigEndian<quint32>(entry.header.ostype), 4);
    }
    return QVariant();
}

int QICNSHandler::imageCount() const
{
    if (!ensureScanned())
        return 0;
    return m_icons.size();
}

bool QICNSHandler::jumpToImage(int imageNumber)
{
    if (imageNumber >= imageCount())
        return false;
    m_currentIconIndex = imageNumber;
    return true;
}

bool QICNSHandler::jumpToNextImage()
{
    return jumpToImage(m_currentIconIndex + 1);
}

bool QICNSHandler::ensureScanned() const
{
    if (!m_scanned) {
        QICNSHandler *that = const_cast<QICNSHandler *>(this);
        that->m_scanned = that->scanDevice();
    }
    return m_scanned;
}

bool QICNSHandler::addEntry(const ICNSBlockHeader &header, quint32 imgDataOffset)
{
    if (header.ostype == 0 || header.length == 0) {
        qWarning("QICNSHandler::addEntry(): Failed, invalid header. OSType %u, length %u",
                 header.ostype, header.length);
        return false;
    }
    ICNSEntry entry;
    // Header:
    entry.header.ostype = header.ostype;
    entry.header.length = header.length;
    // Image data:
    entry.dataOffset = imgDataOffset;
    entry.dataLength = (header.length - ICNSBlockHeaderSize);
    entry.dataIsRLE = false;
    entry.isValid = false;
    // Parse everything else:
    entry.isValid = parseIconEntry(entry);
    if (entry.isValid) {
        if ((entry.mask & ICNSEntry::IsMask) != 0)
            m_masks << entry;
        if ((entry.mask & ICNSEntry::IsIcon) != 0)
            m_icons << entry;
    } else {
        qWarning("QICNSHandler::addEntry(): Failed, unable to parse icon. OSType: %u",
                 entry.header.ostype);
    }
    return entry.isValid;
}

bool QICNSHandler::scanDevice()
{
    if (m_scanned)
        return true;

    if (!device()->seek(0))
        return false;

    QDataStream stream(device());
    stream.setByteOrder(QDataStream::BigEndian);
    qint64 filelength = device()->size();
    ICNSBlockHeader blockHeader;
    while (!stream.atEnd() || (device()->pos() < filelength)) {
        stream >> blockHeader;
        if (stream.status() != QDataStream::Ok)
            return false;
        const quint32 blockDataLength = (blockHeader.length - ICNSBlockHeaderSize);

        bool tocScan = false;
        switch (blockHeader.ostype) {
        case ICNSBlockHeader::icns:
            filelength = blockHeader.length;
            if (device()->size() < blockHeader.length)
                return false;
            break;
        case ICNSBlockHeader::icnV:
        case ICNSBlockHeader::clut:
            // We don't have a good use for these blocks... yet.
            stream.skipRawData(blockDataLength);
            break;
        case ICNSBlockHeader::TOC_: {
            QVector<ICNSBlockHeader> toc;
            const quint32 tocEntriesCount = (blockDataLength / ICNSBlockHeaderSize);
            for (uint i = 0; i < tocEntriesCount; i++) {
                ICNSBlockHeader tocEntry;
                stream >> tocEntry;
                toc << tocEntry;
                quint32 imgDataOffset = (blockHeader.length + ICNSBlockHeaderSize);
                for (quint32 n = 0; n < i; n++)
                    imgDataOffset += toc.at(n).length;
                imgDataOffset += ICNSBlockHeaderSize;
                if (!addEntry(tocEntry, imgDataOffset))
                    return false;
            }
            tocScan = true;
            break;
        }
        default:
            if (!addEntry(blockHeader, device()->pos()))
                return false;
            stream.skipRawData(blockDataLength);
            break;
        }
        if (tocScan)
            break;
    }
    return true;
}

const ICNSEntry &QICNSHandler::getIconMask(const ICNSEntry &icon) const
{
    const bool is32bit = (icon.depth == ICNSEntry::Depth32bit);
    ICNSEntry::Depth targetDepth = is32bit ? ICNSEntry::Depth8bit : ICNSEntry::DepthMono;
    for (int i = 0; i < m_masks.size(); i++) {
        const ICNSEntry &entry = m_masks.at(i);
        const bool suitableDepth = (entry.depth == targetDepth);
        if (suitableDepth && ( (entry.group == icon.group)
                              || ((entry.height == icon.height) && (entry.width == icon.width)) ))
            return entry;
    }
    return icon;
}
