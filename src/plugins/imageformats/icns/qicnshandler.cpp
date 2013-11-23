#include "qicnshandler.h"

static const quint8 IcnsBlockHeaderSize = 8;

static QDataStream &operator>>(QDataStream &in, IcnsBlockHeader &p)
{
    in >> p.OSType;
    in >> p.length;
    return in;
}

static QDataStream &operator<<(QDataStream &out, IcnsBlockHeader &p)
{
    out << p.OSType;
    out << p.length;
    return out;
}

static QVector<QRgb> getColorTable(const IcnsEntry::Depth &depth)
{
    QVector<QRgb> table;
    switch(depth) {
    case IcnsEntry::IconMono: {
        table << qRgb(0xFF, 0xFF, 0xFF);
        table << qRgb(0x00, 0x00, 0x00);
        break;
    }
    case IcnsEntry::Icon4bit: {
        table << qRgb(0xFF, 0xFF, 0xFF);
        table << qRgb(0xFC, 0xF3, 0x05);
        table << qRgb(0xFF, 0x64, 0x02);
        table << qRgb(0xDD, 0x08, 0x06);
        table << qRgb(0xF2, 0x08, 0x84);
        table << qRgb(0x46, 0x00, 0xA5);
        table << qRgb(0x00, 0x00, 0xD4);
        table << qRgb(0x02, 0xAB, 0xEA);
        table << qRgb(0x1F, 0xB7, 0x14);
        table << qRgb(0x00, 0x64, 0x11);
        table << qRgb(0x56, 0x2C, 0x05);
        table << qRgb(0x90, 0x71, 0x3A);
        table << qRgb(0xC0, 0xC0, 0xC0);
        table << qRgb(0x80, 0x80, 0x80);
        table << qRgb(0x40, 0x40, 0x40);
        table << qRgb(0x00, 0x00, 0x00);
        break;
    }
    case IcnsEntry::Icon8bit: {
        table << qRgb(0xFF, 0xFF, 0xFF);
        table << qRgb(0xFF, 0xFF, 0xCC);
        table << qRgb(0xFF, 0xFF, 0x99);
        table << qRgb(0xFF, 0xFF, 0x66);
        table << qRgb(0xFF, 0xFF, 0x33);
        table << qRgb(0xFF, 0xFF, 0x00);
        table << qRgb(0xFF, 0xCC, 0xFF);
        table << qRgb(0xFF, 0xCC, 0xCC);
        table << qRgb(0xFF, 0xCC, 0x99);
        table << qRgb(0xFF, 0xCC, 0x66);
        table << qRgb(0xFF, 0xCC, 0x33);
        table << qRgb(0xFF, 0xCC, 0x00);
        table << qRgb(0xFF, 0x99, 0xFF);
        table << qRgb(0xFF, 0x99, 0xCC);
        table << qRgb(0xFF, 0x99, 0x99);
        table << qRgb(0xFF, 0x99, 0x66);
        table << qRgb(0xFF, 0x99, 0x33);
        table << qRgb(0xFF, 0x99, 0x00);
        table << qRgb(0xFF, 0x66, 0xFF);
        table << qRgb(0xFF, 0x66, 0xCC);
        table << qRgb(0xFF, 0x66, 0x99);
        table << qRgb(0xFF, 0x66, 0x66);
        table << qRgb(0xFF, 0x66, 0x33);
        table << qRgb(0xFF, 0x66, 0x00);
        table << qRgb(0xFF, 0x33, 0xFF);
        table << qRgb(0xFF, 0x33, 0xCC);
        table << qRgb(0xFF, 0x33, 0x99);
        table << qRgb(0xFF, 0x33, 0x66);
        table << qRgb(0xFF, 0x33, 0x33);
        table << qRgb(0xFF, 0x33, 0x00);
        table << qRgb(0xFF, 0x00, 0xFF);
        table << qRgb(0xFF, 0x00, 0xCC);
        table << qRgb(0xFF, 0x00, 0x99);
        table << qRgb(0xFF, 0x00, 0x66);
        table << qRgb(0xFF, 0x00, 0x33);
        table << qRgb(0xFF, 0x00, 0x00);
        table << qRgb(0xCC, 0xFF, 0xFF);
        table << qRgb(0xCC, 0xFF, 0xCC);
        table << qRgb(0xCC, 0xFF, 0x99);
        table << qRgb(0xCC, 0xFF, 0x66);
        table << qRgb(0xCC, 0xFF, 0x33);
        table << qRgb(0xCC, 0xFF, 0x00);
        table << qRgb(0xCC, 0xCC, 0xFF);
        table << qRgb(0xCC, 0xCC, 0xCC);
        table << qRgb(0xCC, 0xCC, 0x99);
        table << qRgb(0xCC, 0xCC, 0x66);
        table << qRgb(0xCC, 0xCC, 0x33);
        table << qRgb(0xCC, 0xCC, 0x00);
        table << qRgb(0xCC, 0x99, 0xFF);
        table << qRgb(0xCC, 0x99, 0xCC);
        table << qRgb(0xCC, 0x99, 0x99);
        table << qRgb(0xCC, 0x99, 0x66);
        table << qRgb(0xCC, 0x99, 0x33);
        table << qRgb(0xCC, 0x99, 0x00);
        table << qRgb(0xCC, 0x66, 0xFF);
        table << qRgb(0xCC, 0x66, 0xCC);
        table << qRgb(0xCC, 0x66, 0x99);
        table << qRgb(0xCC, 0x66, 0x66);
        table << qRgb(0xCC, 0x66, 0x33);
        table << qRgb(0xCC, 0x66, 0x00);
        table << qRgb(0xCC, 0x33, 0xFF);
        table << qRgb(0xCC, 0x33, 0xCC);
        table << qRgb(0xCC, 0x33, 0x99);
        table << qRgb(0xCC, 0x33, 0x66);
        table << qRgb(0xCC, 0x33, 0x33);
        table << qRgb(0xCC, 0x33, 0x00);
        table << qRgb(0xCC, 0x00, 0xFF);
        table << qRgb(0xCC, 0x00, 0xCC);
        table << qRgb(0xCC, 0x00, 0x99);
        table << qRgb(0xCC, 0x00, 0x66);
        table << qRgb(0xCC, 0x00, 0x33);
        table << qRgb(0xCC, 0x00, 0x00);
        table << qRgb(0x99, 0xFF, 0xFF);
        table << qRgb(0x99, 0xFF, 0xCC);
        table << qRgb(0x99, 0xFF, 0x99);
        table << qRgb(0x99, 0xFF, 0x66);
        table << qRgb(0x99, 0xFF, 0x33);
        table << qRgb(0x99, 0xFF, 0x00);
        table << qRgb(0x99, 0xCC, 0xFF);
        table << qRgb(0x99, 0xCC, 0xCC);
        table << qRgb(0x99, 0xCC, 0x99);
        table << qRgb(0x99, 0xCC, 0x66);
        table << qRgb(0x99, 0xCC, 0x33);
        table << qRgb(0x99, 0xCC, 0x00);
        table << qRgb(0x99, 0x99, 0xFF);
        table << qRgb(0x99, 0x99, 0xCC);
        table << qRgb(0x99, 0x99, 0x99);
        table << qRgb(0x99, 0x99, 0x66);
        table << qRgb(0x99, 0x99, 0x33);
        table << qRgb(0x99, 0x99, 0x00);
        table << qRgb(0x99, 0x66, 0xFF);
        table << qRgb(0x99, 0x66, 0xCC);
        table << qRgb(0x99, 0x66, 0x99);
        table << qRgb(0x99, 0x66, 0x66);
        table << qRgb(0x99, 0x66, 0x33);
        table << qRgb(0x99, 0x66, 0x00);
        table << qRgb(0x99, 0x33, 0xFF);
        table << qRgb(0x99, 0x33, 0xCC);
        table << qRgb(0x99, 0x33, 0x99);
        table << qRgb(0x99, 0x33, 0x66);
        table << qRgb(0x99, 0x33, 0x33);
        table << qRgb(0x99, 0x33, 0x00);
        table << qRgb(0x99, 0x00, 0xFF);
        table << qRgb(0x99, 0x00, 0xCC);
        table << qRgb(0x99, 0x00, 0x99);
        table << qRgb(0x99, 0x00, 0x66);
        table << qRgb(0x99, 0x00, 0x33);
        table << qRgb(0x99, 0x00, 0x00);
        table << qRgb(0x66, 0xFF, 0xFF);
        table << qRgb(0x66, 0xFF, 0xCC);
        table << qRgb(0x66, 0xFF, 0x99);
        table << qRgb(0x66, 0xFF, 0x66);
        table << qRgb(0x66, 0xFF, 0x33);
        table << qRgb(0x66, 0xFF, 0x00);
        table << qRgb(0x66, 0xCC, 0xFF);
        table << qRgb(0x66, 0xCC, 0xCC);
        table << qRgb(0x66, 0xCC, 0x99);
        table << qRgb(0x66, 0xCC, 0x66);
        table << qRgb(0x66, 0xCC, 0x33);
        table << qRgb(0x66, 0xCC, 0x00);
        table << qRgb(0x66, 0x99, 0xFF);
        table << qRgb(0x66, 0x99, 0xCC);
        table << qRgb(0x66, 0x99, 0x99);
        table << qRgb(0x66, 0x99, 0x66);
        table << qRgb(0x66, 0x99, 0x33);
        table << qRgb(0x66, 0x99, 0x00);
        table << qRgb(0x66, 0x66, 0xFF);
        table << qRgb(0x66, 0x66, 0xCC);
        table << qRgb(0x66, 0x66, 0x99);
        table << qRgb(0x66, 0x66, 0x66);
        table << qRgb(0x66, 0x66, 0x33);
        table << qRgb(0x66, 0x66, 0x00);
        table << qRgb(0x66, 0x33, 0xFF);
        table << qRgb(0x66, 0x33, 0xCC);
        table << qRgb(0x66, 0x33, 0x99);
        table << qRgb(0x66, 0x33, 0x66);
        table << qRgb(0x66, 0x33, 0x33);
        table << qRgb(0x66, 0x33, 0x00);
        table << qRgb(0x66, 0x00, 0xFF);
        table << qRgb(0x66, 0x00, 0xCC);
        table << qRgb(0x66, 0x00, 0x99);
        table << qRgb(0x66, 0x00, 0x66);
        table << qRgb(0x66, 0x00, 0x33);
        table << qRgb(0x66, 0x00, 0x00);
        table << qRgb(0x33, 0xFF, 0xFF);
        table << qRgb(0x33, 0xFF, 0xCC);
        table << qRgb(0x33, 0xFF, 0x99);
        table << qRgb(0x33, 0xFF, 0x66);
        table << qRgb(0x33, 0xFF, 0x33);
        table << qRgb(0x33, 0xFF, 0x00);
        table << qRgb(0x33, 0xCC, 0xFF);
        table << qRgb(0x33, 0xCC, 0xCC);
        table << qRgb(0x33, 0xCC, 0x99);
        table << qRgb(0x33, 0xCC, 0x66);
        table << qRgb(0x33, 0xCC, 0x33);
        table << qRgb(0x33, 0xCC, 0x00);
        table << qRgb(0x33, 0x99, 0xFF);
        table << qRgb(0x33, 0x99, 0xCC);
        table << qRgb(0x33, 0x99, 0x99);
        table << qRgb(0x33, 0x99, 0x66);
        table << qRgb(0x33, 0x99, 0x33);
        table << qRgb(0x33, 0x99, 0x00);
        table << qRgb(0x33, 0x66, 0xFF);
        table << qRgb(0x33, 0x66, 0xCC);
        table << qRgb(0x33, 0x66, 0x99);
        table << qRgb(0x33, 0x66, 0x66);
        table << qRgb(0x33, 0x66, 0x33);
        table << qRgb(0x33, 0x66, 0x00);
        table << qRgb(0x33, 0x33, 0xFF);
        table << qRgb(0x33, 0x33, 0xCC);
        table << qRgb(0x33, 0x33, 0x99);
        table << qRgb(0x33, 0x33, 0x66);
        table << qRgb(0x33, 0x33, 0x33);
        table << qRgb(0x33, 0x33, 0x00);
        table << qRgb(0x33, 0x00, 0xFF);
        table << qRgb(0x33, 0x00, 0xCC);
        table << qRgb(0x33, 0x00, 0x99);
        table << qRgb(0x33, 0x00, 0x66);
        table << qRgb(0x33, 0x00, 0x33);
        table << qRgb(0x33, 0x00, 0x00);
        table << qRgb(0x00, 0xFF, 0xFF);
        table << qRgb(0x00, 0xFF, 0xCC);
        table << qRgb(0x00, 0xFF, 0x99);
        table << qRgb(0x00, 0xFF, 0x66);
        table << qRgb(0x00, 0xFF, 0x33);
        table << qRgb(0x00, 0xFF, 0x00);
        table << qRgb(0x00, 0xCC, 0xFF);
        table << qRgb(0x00, 0xCC, 0xCC);
        table << qRgb(0x00, 0xCC, 0x99);
        table << qRgb(0x00, 0xCC, 0x66);
        table << qRgb(0x00, 0xCC, 0x33);
        table << qRgb(0x00, 0xCC, 0x00);
        table << qRgb(0x00, 0x99, 0xFF);
        table << qRgb(0x00, 0x99, 0xCC);
        table << qRgb(0x00, 0x99, 0x99);
        table << qRgb(0x00, 0x99, 0x66);
        table << qRgb(0x00, 0x99, 0x33);
        table << qRgb(0x00, 0x99, 0x00);
        table << qRgb(0x00, 0x66, 0xFF);
        table << qRgb(0x00, 0x66, 0xCC);
        table << qRgb(0x00, 0x66, 0x99);
        table << qRgb(0x00, 0x66, 0x66);
        table << qRgb(0x00, 0x66, 0x33);
        table << qRgb(0x00, 0x66, 0x00);
        table << qRgb(0x00, 0x33, 0xFF);
        table << qRgb(0x00, 0x33, 0xCC);
        table << qRgb(0x00, 0x33, 0x99);
        table << qRgb(0x00, 0x33, 0x66);
        table << qRgb(0x00, 0x33, 0x33);
        table << qRgb(0x00, 0x33, 0x00);
        table << qRgb(0x00, 0x00, 0xFF);
        table << qRgb(0x00, 0x00, 0xCC);
        table << qRgb(0x00, 0x00, 0x99);
        table << qRgb(0x00, 0x00, 0x66);
        table << qRgb(0x00, 0x00, 0x33);
        table << qRgb(0xEE, 0x00, 0x00);
        table << qRgb(0xDD, 0x00, 0x00);
        table << qRgb(0xBB, 0x00, 0x00);
        table << qRgb(0xAA, 0x00, 0x00);
        table << qRgb(0x88, 0x00, 0x00);
        table << qRgb(0x77, 0x00, 0x00);
        table << qRgb(0x55, 0x00, 0x00);
        table << qRgb(0x44, 0x00, 0x00);
        table << qRgb(0x22, 0x00, 0x00);
        table << qRgb(0x11, 0x00, 0x00);
        table << qRgb(0x00, 0xEE, 0x00);
        table << qRgb(0x00, 0xDD, 0x00);
        table << qRgb(0x00, 0xBB, 0x00);
        table << qRgb(0x00, 0xAA, 0x00);
        table << qRgb(0x00, 0x88, 0x00);
        table << qRgb(0x00, 0x77, 0x00);
        table << qRgb(0x00, 0x55, 0x00);
        table << qRgb(0x00, 0x44, 0x00);
        table << qRgb(0x00, 0x22, 0x00);
        table << qRgb(0x00, 0x11, 0x00);
        table << qRgb(0x00, 0x00, 0xEE);
        table << qRgb(0x00, 0x00, 0xDD);
        table << qRgb(0x00, 0x00, 0xBB);
        table << qRgb(0x00, 0x00, 0xAA);
        table << qRgb(0x00, 0x00, 0x88);
        table << qRgb(0x00, 0x00, 0x77);
        table << qRgb(0x00, 0x00, 0x55);
        table << qRgb(0x00, 0x00, 0x44);
        table << qRgb(0x00, 0x00, 0x22);
        table << qRgb(0x00, 0x00, 0x11);
        table << qRgb(0xEE, 0xEE, 0xEE);
        table << qRgb(0xDD, 0xDD, 0xDD);
        table << qRgb(0xBB, 0xBB, 0xBB);
        table << qRgb(0xAA, 0xAA, 0xAA);
        table << qRgb(0x88, 0x88, 0x88);
        table << qRgb(0x77, 0x77, 0x77);
        table << qRgb(0x55, 0x55, 0x55);
        table << qRgb(0x44, 0x44, 0x44);
        table << qRgb(0x22, 0x22, 0x22);
        table << qRgb(0x11, 0x11, 0x11);
        table << qRgb(0x00, 0x00, 0x00);
        break;
    }
    default:
        qWarning("QIcnsHandler::getColorTable(): No color table for bit depth: %u", depth);
    }
    return table;
}

static bool parseIconEntry(IcnsEntry &icon)
{
    if (icon.isValid)
        return true; // Skip if already parsed
    const QByteArray OSType = QByteArray::fromHex(QByteArray::number(icon.header.OSType,16));
    // Typical OSType naming: <junk><group><depth><mask>;
    const QString pattern = QStringLiteral("^(?<junk>[\\D]{0,4})(?<group>[a-z|A-Z]{1})(?<depth>\\d{0,2})(?<mask>[#mk]{0,2})$");
    QRegularExpression regexp(pattern);
    QRegularExpressionMatch match = regexp.match(OSType);
    const bool hasMatch = match.hasMatch();
    const QString junk = match.captured("junk");
    const QString group = match.captured("group");
    const QString depth = match.captured("depth");
    const QString mask = match.captured("mask");
    // Icon group:
    icon.group = group.isEmpty() ? IcnsEntry::IconGroupUnknown : IcnsEntry::IconGroup(group.at(0).toLatin1());
    // Icon depth:
    icon.depth = depth.toUInt() > 0 ? IcnsEntry::Depth(depth.toUInt()) : IcnsEntry::IconMono;
    // Width/height/mask:
    icon.width = 0; // default for invalid ones
    icon.height = 0; // default for invalid ones
    icon.mask = IcnsEntry::IconMaskUnknown; // default for invalid ones
    if (icon.group != IcnsEntry::IconGroupCompressed) {
        const qreal bytespp = ((qreal)icon.depth / 8);
        const qreal r1 = qSqrt(icon.dataLength/bytespp);
        const qreal r2 = qSqrt((icon.dataLength/bytespp)/2);
        const quint32 r1u = (quint32)r1;
        const quint32 r2u = (quint32)r2;
        const bool r1IsPowerOfTwoOrDevidesBy16 = (r1u == r1 && r1u % 16 == 0) || (r1u == r1 && r1 >= 16 && ((r1u & (r1u - 1)) == 0));
        const bool r2IsPowerOfTwoOrDevidesBy16 = (r2u == r2 && r2u % 16 == 0) || (r2u == r2 && r2 >= 16 && ((r2u & (r2u - 1)) == 0));

        if (r1IsPowerOfTwoOrDevidesBy16) {
            icon.mask = mask.isEmpty() ? IcnsEntry::IconNoMask : IcnsEntry::IconIsMask;
            icon.width = r1;
            icon.height = r1;
        } else if (r2IsPowerOfTwoOrDevidesBy16) {
            icon.mask = IcnsEntry::IconPlusMask;
            icon.width = r2;
            icon.height = r2;
        } else if (icon.group == IcnsEntry::IconGroupMini) {
            // Legacy 16x12 icons are an exception from the generic square formula
            icon.mask = (icon.dataLength == 192*bytespp*2) ? IcnsEntry::IconPlusMask : IcnsEntry::IconNoMask;
            icon.width = 16;
            icon.height = 12;
        } else if (icon.depth == IcnsEntry::Icon32bit) {
            icon.dataIsRLE = true; // 32bit icon may be encoded
            switch(icon.group) {
            case IcnsEntry::IconGroupSmall :
                icon.width = 16;
                icon.height = 16;
                break;
            case IcnsEntry::IconGroupLarge :
                icon.width = 32;
                icon.height = 32;
                break;
            case IcnsEntry::IconGroupHuge :
                icon.width = 48;
                icon.height = 48;
                break;
            case IcnsEntry::IconGroupThumbnail :
                icon.width = 128;
                icon.height = 128;
                break;
            default :
                qWarning() << "IcnsIconEntry::parse(): 32bit icon from an unknown group. OSType:"
                           << OSType.constData();
            }
        }
    }
    //TODO: Add parsing of png/jp2 headers to enable feature reporting by IOPlugin?
    if (!hasMatch)
        qWarning() << "IcnsIconEntry::parse(): Parsing failed, ignored. Reg exp: no match for OSType:" << OSType.constData();
    return hasMatch;
}

static QImage readMaskFromStream(const IcnsEntry &mask, QDataStream &stream)
{
    QImage img;
    if (mask.mask != IcnsEntry::IconIsMask && mask.mask != IcnsEntry::IconPlusMask)
        return img;
    if (mask.depth != IcnsEntry::IconMono && mask.depth != IcnsEntry::Icon8bit) {
        qWarning() << "QIcnsHandler::readMaskFromStream(): Mask has unusual bit depth, can't read:"
                   << mask.depth << "OSType:" << mask.header.OSType;
        return img;
    }
    const qreal bytespp = ((qreal)mask.depth / 8);
    const quint32 imageDataSize = (mask.width * mask.height) * bytespp;
    const qint64 pos = (mask.mask == IcnsEntry::IconPlusMask) ? (mask.dataOffset + imageDataSize) : mask.dataOffset;
    const qint64 oldPos = stream.device()->pos();
    if (stream.device()->seek(pos)) {
        img = QImage(mask.width, mask.height, QImage::Format_RGB32);
        quint8 byte = 0;
        for (quint32 pixel = 0; pixel < (mask.width * mask.height); pixel++) {
            const quint32 y = pixel / mask.height;
            const quint32 x = pixel - (mask.width * y);
            if (pixel % (8 / mask.depth) == 0)
                stream >> byte;
            if (stream.status() != QDataStream::Ok)
                return img;
            quint8 alpha = (mask.depth == IcnsEntry::IconMono) ? (byte >> 7) * 0xFF : byte;
            byte = byte << 1;
            QRgb *line = reinterpret_cast<QRgb *>(img.scanLine(y));
            line[x] = qRgb(alpha,alpha,alpha);
        }
        stream.device()->seek(oldPos);
    }
    return img;
}

static QImage readLowDepthIconFromStream(const IcnsEntry &icon, QDataStream &stream)
{
    QImage img;
    quint8 byte = 0;
    const QImage::Format format = (icon.depth == IcnsEntry::IconMono) ? QImage::Format_Mono : QImage::Format_Indexed8;
    QVector<QRgb> colortable = getColorTable(icon.depth);
    if (colortable.size() < 2)
        return img;
    img = QImage(icon.width, icon.height, format);
    img.setColorTable(colortable);
    for (quint32 pixel = 0; pixel < (icon.width * icon.height); pixel++) {
        const quint32 y = pixel / icon.height;
        const quint32 x = pixel - (icon.width * y);
        if (pixel % (8 / icon.depth) == 0) {
            stream >> byte;
        }
        if (stream.status() != QDataStream::Ok) {
            return img;
        }
        quint8 cindex = 0;
        switch(icon.depth) {
        case IcnsEntry::IconMono: {
            cindex = (byte & 0x80) ? 1 : 0; // left 1 bit
            break;
        }
        case IcnsEntry::Icon4bit: {
            quint8 value = ((byte & 0xF0) >> 4); // left 4 bits
            cindex = (value < qPow(2,icon.depth)) ? value : 0;
            break;
        }
        default: //8bit
            cindex = (byte < qPow(2,icon.depth)) ? byte : 0;
        }
        byte = byte << icon.depth;
        img.setPixel(x,y,cindex);
    }
    return img;
}

static QImage read32bitIconFromStream(const IcnsEntry &icon, QDataStream &stream)
{
    QImage img = QImage(icon.width, icon.height, QImage::Format_RGB32);
    if (!icon.dataIsRLE) {
        QRgb *line;
        for (quint32 pixel = 0; pixel < (icon.width * icon.height); pixel++) {
            const quint32 y = pixel / icon.height;
            const quint32 x = pixel - (icon.width * y);
            if (pixel % icon.height == 0)
                line = reinterpret_cast<QRgb *>(img.scanLine(y));
            quint8 r, g, b;
            stream >> r >> g >> b;
            if (stream.status() != QDataStream::Ok)
                return img;
            line[x] = qRgb(r,g,b);
        }
    }
    else {
        const quint32 estPxsNum = icon.width * icon.height;
        const QByteArray &bytes = stream.device()->peek(4);
        if (bytes.isEmpty())
            return QImage();

        if (qFromBigEndian<quint32>(*bytes.constData()) == 0)
            stream.skipRawData(4); // Zero-padding may be present
        for (quint8 colorNRun = 0; colorNRun < 3; colorNRun++) {
            quint32	pixel = 0;
            QRgb *line;
            while ((pixel < estPxsNum) && !stream.atEnd()) {
                quint8 byte, value;
                stream >> byte;
                const bool bitIsClear = ((byte & 0x80) == 0);
                // If high bit is clear: run of different values; else: same value
                quint8 runLength = bitIsClear ? ((0xFF & byte) + 1) : ((0xFF & byte) - 125);
                // Length of the run for for different values: 1 <= len <= 128
                // Length of the run for same values: 3 <= len <= 130
                if (!bitIsClear) {
                    if (stream.atEnd())
                        return img;
                    stream >> value;
                }
                for (quint8 i = 0; (i < runLength) && (pixel < estPxsNum); i++) {
                    if (bitIsClear) {
                        if (stream.atEnd())
                            return img;
                        stream >> value;
                    }
                    const quint32 y = pixel / icon.height;
                    const quint32 x = pixel - (icon.width * y);
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

QIcnsHandler::QIcnsHandler()
    :m_currentIconIndex(0), m_parsed(false)
{

}

QByteArray QIcnsHandler::name() const
{
    return "icns";
}

bool QIcnsHandler::canRead(QIODevice *device)
{
    if (!device || !device->isReadable()) {
        qWarning("QIcnsHandler::canRead() called without a readable device");
        return false;
    }
    if (device->isSequential()) {
        qWarning("QIcnsHandler::canRead() called on sequential device (NYI)");
        return false;
    }
    return device->peek(4) == "icns";
}

bool QIcnsHandler::canWrite(QIODevice *device)
{
    if (!device || !device->isWritable()) {
        qWarning("QIcnsHandler::canRead() called without a writable device");
        return false;
    }
    return true;
}

bool QIcnsHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("icns");
        return true;
    }
    return false;
}

bool QIcnsHandler::read(QImage *outImage)
{
    QImage img;
    if (!ensureScanned()) {
        qWarning("QIcnsHandler::read(): The device was not parced properly!");
        return false;
    }

    const IcnsEntry &icon = m_icons.at(m_currentIconIndex);
    QDataStream stream(device());
    if (!device()->seek(icon.dataOffset))
        return false;

    const QByteArray magicCheck = device()->peek(12).toHex();
    const bool isPNG = magicCheck.startsWith(QByteArrayLiteral("89504e470d0a1a0a"));
    const bool isJP2 = (magicCheck == QByteArrayLiteral("0000000c6a5020200d0a870a"));
    if (isPNG || isJP2 || icon.group == IcnsEntry::IconGroupCompressed) {
        const QByteArray ba = device()->read(icon.dataLength);
        if (ba.isEmpty()) {
            qWarning("QIcnsHandler::read(): Compressed image data is empty or couldn't be read. OSType: %u", icon.header.OSType);
            return false;
        }
        if (isPNG || isJP2) {
            const char *format = isPNG ? "png" : "jp2";
            img = QImage::fromData(ba, format);
            if (img.isNull())
                qWarning("QIcnsHandler::read(): Failed, format \"%s\" is not supported by your Qt distribution. OSType: %u", format, icon.header.OSType);
        } else { // Try anyway
            img = QImage::fromData(ba);
            if (img.isNull())
                qWarning("QIcnsHandler::read(): Unsupported compressed icon format, OSType: %u", icon.header.OSType);
        }
    } else if (icon.height == 0 || icon.width == 0) {
        qWarning("QIcnsHandler::read(): Size of a raw icon is unknown, OSType: %u", icon.header.OSType);
    } else {
        switch(icon.depth) {
        case IcnsEntry::IconMono:
        case IcnsEntry::Icon4bit:
        case IcnsEntry::Icon8bit:
            img = readLowDepthIconFromStream(icon, stream);
            break;
        case IcnsEntry::Icon32bit:
            img = read32bitIconFromStream(icon, stream);
            break;
        default:
            qWarning() << "QIcnsHandler::read(): Icon #:" << m_currentIconIndex
                       << "Unsupported icon bit depth:" << icon.depth;
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

bool QIcnsHandler::write(const QImage &image)
{
    QIODevice *device = QImageIOHandler::device();
    // NOTE: Experimental implementation. Just for simple converting tasks / testing purposes.
    // LIMITATIONS: Writes a complete icns file containing only one square icon in PNG format to a device.
    // Currently uses non-hardcoded OSTypes.
    QImage img = image;
    const int width = img.size().width();
    const int height = img.size().height();
    const bool sizeIsCorrect = (width == height) && (width >= 16) && ((width & (width - 1)) == 0);
    if (!device->isWritable() && !sizeIsCorrect)
        return false;

    // Construct icon OSType
    int i = width;
    uint p = 0;
    while (i >>= 1) { p++; }
    if (p > 10) { // Force resizing to 1024x1024. Values over 10 are reserved for retina icons
        p = 10;
        img = img.scaled(1024, 1024);
    }
    // Small / big icons naming policy
    const QByteArray ostypebase = (p < 7) ? QByteArrayLiteral("ipc") : QByteArrayLiteral("ic");
    const QByteArray ostypenum = (ostypebase.size() > 2 || p >= 10) ? QByteArray::number(p) : QByteArray::number(p).prepend("0");
    const quint32 ostype = QByteArray(ostypebase).append(ostypenum).toHex().toUInt(NULL,16);
    // Construct ICNS Header
    IcnsBlockHeader fileHeader;
    fileHeader.OSType = IcnsBlockHeader::OSType_icns;
    // Construct TOC Header
    IcnsBlockHeader tocHeader;
    tocHeader.OSType = IcnsBlockHeader::OSType_TOC_;
    // Construct TOC Entry
    IcnsBlockHeader tocEntry;
    tocEntry.OSType = ostype;
    // Construct Icon block
    IcnsBlockHeader iconEntry;
    iconEntry.OSType = ostype;
    // Construct image data
    QByteArray imageData;
    QBuffer buffer(&imageData);
    if (!buffer.open(QIODevice::WriteOnly) && !img.save(&buffer, "png"))
        return false;
    buffer.close();
    iconEntry.length = IcnsBlockHeaderSize + imageData.size();
    tocEntry.length = iconEntry.length;
    tocHeader.length = IcnsBlockHeaderSize * 2;
    fileHeader.length = IcnsBlockHeaderSize + tocHeader.length + iconEntry.length;
    // Write everything
    QDataStream stream(device);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << fileHeader << tocHeader << tocEntry << iconEntry;
    stream.writeRawData(imageData.constData(), imageData.size());
    if (stream.status() != QDataStream::Ok)
        return false;
    return true;
}

bool QIcnsHandler::supportsOption(QImageIOHandler::ImageOption option) const
{
    return (option == QImageIOHandler::Name || option == QImageIOHandler::SubType);
}

QVariant QIcnsHandler::option(QImageIOHandler::ImageOption option) const
{
    if (supportsOption(option) && ensureScanned()) {
        if (imageCount() > 0 && m_currentIconIndex <= imageCount())
            return QByteArray::fromHex(QByteArray::number(m_icons.at(m_currentIconIndex).header.OSType, 16));
    }
    return QVariant();
}

int QIcnsHandler::imageCount() const
{
    ensureScanned();
    return m_icons.size();
}

bool QIcnsHandler::jumpToImage(int imageNumber)
{
    if (imageNumber < imageCount())
        m_currentIconIndex = imageNumber;
    return (imageNumber < imageCount()) ? true : false;
}

bool QIcnsHandler::jumpToNextImage()
{
    return jumpToImage(m_currentIconIndex + 1);
}

bool QIcnsHandler::ensureScanned() const
{
    if (!m_parsed) {
        QIcnsHandler* that = const_cast<QIcnsHandler *>(this);
        that->m_parsed = that->scanDevice();
    }
    return m_parsed;
}

bool QIcnsHandler::addEntry(const IcnsBlockHeader &header, quint32 imgDataOffset)
{
    IcnsEntry entry;
    // Header:
    entry.header.OSType = header.OSType;
    entry.header.length = header.length;
    // Image data:
    entry.dataOffset = imgDataOffset;
    entry.dataLength = header.length - IcnsBlockHeaderSize;
    entry.dataIsRLE = false; // Always false unless spotted
    entry.isValid = false;
    // Parse everything else:
    entry.isValid = parseIconEntry(entry);
    if (entry.isValid) {
        switch(entry.mask) {
        case IcnsEntry::IconPlusMask:
            m_icons << entry;
            m_masks << entry;
            break;
        case IcnsEntry::IconIsMask:
            m_masks << entry;
            break;
        default: //IconNoMask
            m_icons << entry;
        }
    } else {
        qWarning("QIcnsHandler::addIcon(): Unable to parse icon, OSType: %u", entry.header.OSType);
    }
    return entry.isValid;
}

bool QIcnsHandler::scanDevice()
{
    if(m_parsed)
        return true;

    if (!device()->seek(0))
        return false;

    QDataStream stream(device());
    qint64 filelength = device()->size();
    IcnsBlockHeader blockHeader;
    while (!stream.atEnd() || (device()->pos() < filelength)) {
        stream >> blockHeader;
        if (stream.status() != QDataStream::Ok)
            return false;

        switch (blockHeader.OSType) {
        case IcnsBlockHeader::OSType_icns: {
            filelength = blockHeader.length;
            if (device()->size() < blockHeader.length)
                return false;
            break;
        }
        case IcnsBlockHeader::OSType_icnV: {
            stream.skipRawData(4);
            break;
        }
        case IcnsBlockHeader::OSType_TOC_: {
            QVector<IcnsBlockHeader> toc;
            const quint32 tocEntriesCount = (blockHeader.length - IcnsBlockHeaderSize) / IcnsBlockHeaderSize;
            for (uint i = 0; i < tocEntriesCount; i++) {
                IcnsBlockHeader tocEntry;
                stream >> tocEntry;
                toc << tocEntry;
                quint32 imgDataOffset = blockHeader.length + IcnsBlockHeaderSize;
                for (uint n = 0; n < i; n++)
                    imgDataOffset += toc.at(n).length;
                imgDataOffset += IcnsBlockHeaderSize;
                addEntry(tocEntry, imgDataOffset);
            }
            return true; // TOC scan gives enough data to discard scan of other blocks
        }
        default:
            addEntry(blockHeader, stream.device()->pos());
            stream.skipRawData((blockHeader.length - IcnsBlockHeaderSize));
        }
    }
    return true;
}

IcnsEntry QIcnsHandler::getIconMask(const IcnsEntry &icon) const
{
    if (icon.isValid && (icon.mask != IcnsEntry::IconIsMask && icon.mask != IcnsEntry::IconPlusMask)) {
        IcnsEntry::Depth targetDepth = (icon.depth == IcnsEntry::Icon32bit) ? IcnsEntry::Icon8bit : IcnsEntry::IconMono;
        for (int i = 0; i < m_masks.size(); i++) {
            const bool suitable = m_masks.at(i).group == icon.group
                    || (m_masks.at(i).height == icon.height
                        && m_masks.at(i).width == icon.width);
            if (suitable) {
                if (m_masks.at(i).depth == targetDepth)
                    return m_masks.at(i);
            }
        }
    }
    return icon;
}
