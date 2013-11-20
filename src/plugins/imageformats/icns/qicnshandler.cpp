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

QVector<QRgb> QIcnsHandler::getColorTable(const IconBitDepth &depth)
{
    QVector<QRgb> table;
    switch(depth) {
    case IconMono: {
        table << qRgb(0xFF, 0xFF, 0xFF);
        table << qRgb(0x00, 0x00, 0x00);
        break;
    }
    case Icon4bit: {
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
    case Icon8bit: {
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

QImage QIcnsHandler::readMaskFromStream(const IcnsIconEntry &mask, QDataStream &stream)
{
    QImage img;
    if (!mask.isAlphaMask()) {
        return img;
    }
    if (mask.depth() != IconMono && mask.depth() != Icon8bit) {
        qWarning() << "QIcnsHandler::readMaskFromStream(): Mask has unusual bit depth, can't read:"
                   << mask.depth() << "OSType:" << mask.getOSType();
        return img;
    }
    const qreal bytespp = ((qreal)mask.depth() / 8);
    const quint32 imageDataSize = (mask.width() * mask.height()) * bytespp;
    const quint32 pos = (mask.mask() == IconPlusMask) ? (mask.dataOffset() + imageDataSize) : mask.dataOffset();
    const qint64 oldPos = stream.device()->pos();
    if (stream.device()->seek(pos)) {
        img = QImage(mask.width(), mask.height(), QImage::Format_RGB32);
        quint8 byte = 0;
        quint32 pixel = 0;
        for (quint32 y = 0; y < mask.height(); y++) {
            for (quint32 x = 0; x < mask.width(); x++) {
                if (pixel % (8 / mask.depth()) == 0) {
                    stream >> byte;
                }
                if (stream.status() != QDataStream::Ok) {
                    return img;
                }
                quint8 alpha = (mask.depth() == IconMono) ? (byte >> 7) * 0xFF : byte;
                byte = byte << 1;
                img.setPixel(x,y,qRgb(alpha,alpha,alpha));
                pixel++;
            }
        }
        stream.device()->seek(oldPos);
    }
    return img;
}

QImage QIcnsHandler::readLowDepthIconFromStream(const IcnsIconEntry &icon, QDataStream &stream)
{
    QImage img;
    quint8 byte = 0;
    quint32 pixel = 0;
    const QImage::Format format = (icon.depth() == IconMono) ? QImage::Format_Mono : QImage::Format_Indexed8;
    QVector<QRgb> colortable = getColorTable(icon.depth());
    if (colortable.size() < 2) {
        return img;
    }
    img = QImage(icon.width(), icon.height(), format);
    img.setColorTable(colortable);
    for (uint y = 0; y < icon.height(); y++) {
        for (uint x = 0; x < icon.width(); x++) {
            if (pixel % (8 / icon.depth()) == 0) {
                stream >> byte;
            }
            if (stream.status() != QDataStream::Ok) {
                return img;
            }
            quint8 cindex = 0;
            switch(icon.depth()) {
            case IconMono: {
                cindex = (byte & 0x80) ? 1 : 0; // left 1 bit
                break;
            }
            case Icon4bit: {
                quint8 value = ((byte & 0xF0) >> 4); // left 4 bits
                cindex = (value < qPow(2,icon.depth())) ? value : 0;
                break;
            }
            default: //8bit
                cindex = (byte < qPow(2,icon.depth())) ? byte : 0;
            }
            byte = byte << icon.depth();
            img.setPixel(x,y,cindex);
            pixel++;
        }
    }
    return img;
}

QImage QIcnsHandler::read32bitIconFromStream(const IcnsIconEntry &icon, QDataStream &stream)
{
    QImage img = QImage(icon.width(), icon.height(), QImage::Format_RGB32);
    if (!icon.isRLE24()) {
        quint32 pixel = 0;
        for (uint y = 0; (y < icon.height()); y++) {
            for (uint x = 0; (x < icon.width()); x++) {
                quint8 r, g, b;
                stream >> r >> g >> b;
                if (stream.status() != QDataStream::Ok) {
                    return img;
                }
                img.setPixel(x,y,qRgb(r,g,b));
                pixel++;
            }
        }
    }
    else {
        const quint32 estPxsNum = icon.width() * icon.height();
        const QByteArray bytes = stream.device()->peek(4);
        if (bytes.isEmpty()) {
            return QImage();
        }
        if (*((quint32*)bytes.constData()) == 0) {
            stream.skipRawData(4); // Zero-padding may be present
        }
        for (quint8 colorNRun = 0; colorNRun < 3; colorNRun++) {
            quint32	pixel = 0;
            while ((pixel < estPxsNum) && !stream.atEnd()) {
                quint8 byte, value;
                stream >> byte;
                const bool bitIsClear = ((byte & 0x80) == 0);
                // If high bit is clear: run of different values; else: same value
                quint8 runLength = bitIsClear ? ((0xFF & byte) + 1) : ((0xFF & byte) - 125);
                // Length of the run for for different values: 1 <= len <= 128
                // Length of the run for same values: 3 <= len <= 130
                if (!bitIsClear) {
                    if (stream.atEnd()) {
                        return img;
                    }
                    stream >> value;
                }
                for (quint8 i = 0; (i < runLength) && (pixel < estPxsNum); i++) {
                    if (bitIsClear) {
                        if (stream.atEnd()) {
                            return img;
                        }
                        stream >> value;
                    }
                    const quint32 y = pixel / icon.height();
                    const quint32 x = pixel - (icon.width() * y);
                    const QRgb rgb = img.pixel(x,y);
                    const int r = (colorNRun == 0) ? value : qRed(rgb);
                    const int g = (colorNRun == 1) ? value : qGreen(rgb);
                    const int b = (colorNRun == 2) ? value : qBlue(rgb);
                    img.setPixel(x,y,qRgb(r,g,b));
                    pixel++;
                }
            }
        }
    }
    return img;
}

QIcnsHandler::QIcnsHandler()
{
    m_currentIconIndex = 0;
    m_scanstate = IcnsFileIsNotParsed;
}

QIcnsHandler::QIcnsHandler(QIODevice * d, const QByteArray &format)
{
    setDevice(d);
    setFormat(format);
    m_stream.setDevice(d);
    m_currentIconIndex = 0;
    m_scanstate = IcnsFileIsNotParsed;
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
    if (!isScanned()) {
        scanDevice();
    }
    if (!isParsed()) {
        qWarning("QIcnsHandler::read(): The device was not parced properly!");
        *outImage = img;
        return false;
    }

    IcnsIconEntry icon = m_icons.at(m_currentIconIndex);
    if (m_stream.device()->seek(icon.dataOffset())) {
        if (m_stream.device()->peek(8).toHex() == QByteArrayLiteral("89504e470d0a1a0a")) { // if PNG magic
            img = QImage::fromData(m_stream.device()->peek(icon.dataLength()), "png");
            if (img.isNull()) {
                qWarning("QIcnsHandler::read(): Failed, PNG is not supported by your Qt distribution. OSType: %u", icon.getOSType());
            }
        }
        else if (m_stream.device()->peek(12).toHex() == QByteArrayLiteral("0000000c6a5020200d0a870a")) { // if JPEG 2000 magic
            img = QImage::fromData(m_stream.device()->peek(icon.dataLength()), "jp2");
            if (img.isNull()) {
                qWarning("QIcnsHandler::read(): Failed, JPEG2000 is not supported by your Qt distribution. OSType: %u", icon.getOSType());
            }
        }
        else if (icon.group() == IconGroupCompressed) {
            qWarning("QIcnsHandler::read(): Unsupported compressed icon format, OSType: %u", icon.getOSType());
        }
        else if (icon.height() == 0 || icon.width() == 0) {
            qWarning("QIcnsHandler::read(): Size of a raw icon is unknown, OSType: %u", icon.getOSType());
        }
        else {
            switch(icon.depth()) {
            case IconMono:
            case Icon4bit:
            case Icon8bit:
                img = readLowDepthIconFromStream(icon, m_stream);
                break;
            case Icon32bit:
                img = read32bitIconFromStream(icon, m_stream);
                break;
            default: {
                qWarning() << "QIcnsHandler::read(): Icon #:" << m_currentIconIndex
                           << "Unsupported icon bit depth:" << icon.depth();
            }
            }
        }
    }
    if (!img.isNull()) {
        QImage alpha = readMaskFromStream(getIconMask(icon), m_stream);
        if (!alpha.isNull()) {
            //FIXME: Replace with QPainter compositions in the future or do some other way?
            img.setAlphaChannel(alpha);
        }
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
    if (!device->isWritable() && !sizeIsCorrect) {
        return false;
    }
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
    fileHeader.OSType = OSType_icns;
    // Construct TOC Header
    IcnsBlockHeader tocHeader;
    tocHeader.OSType = OSType_TOC_;
    // Construct TOC Entry
    IcnsBlockHeader tocEntry;
    tocEntry.OSType = ostype;
    // Construct Icon block
    IcnsBlockHeader iconEntry;
    iconEntry.OSType = ostype;
    // Construct image data
    QByteArray imageData;
    QBuffer buffer(&imageData);
    if (!buffer.open(QIODevice::WriteOnly) && !img.save(&buffer, "png")) {
        return false;
    }
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
    if (stream.status() != QDataStream::Ok) {
        return false;
    }
    return true;
}

bool QIcnsHandler::supportsOption(QImageIOHandler::ImageOption option) const
{
    return (option == QImageIOHandler::Name || option == QImageIOHandler::SubType);
}

QVariant QIcnsHandler::option(QImageIOHandler::ImageOption option) const
{
    if (supportsOption(option)) {
        if (!isScanned()) {
            QIcnsHandler* that = const_cast<QIcnsHandler *>(this);
            that->scanDevice();
        }
        if (m_currentIconIndex >= 0 && m_currentIconIndex <= m_icons.size()) {
            return QByteArray::fromHex(QByteArray::number(m_icons.at(m_currentIconIndex).getOSType(), 16));
        }
    }
    return QVariant();
}

int QIcnsHandler::imageCount() const
{
    if (!isScanned()) {
        QIcnsHandler* that = const_cast<QIcnsHandler *>(this);
        that->scanDevice();
    }
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

bool QIcnsHandler::isScanned() const
{
    return (m_scanstate != IcnsFileIsNotParsed);
}

bool QIcnsHandler::isParsed() const
{
    return (m_scanstate == IcnsFileIsParsed);
}

bool QIcnsHandler::addIcon(IcnsIconEntry &icon)
{
    if (icon.isValid()) {
        switch(icon.mask()) {
        case IconPlusMask:
            m_icons << icon;
            m_masks << icon;
            break;
        case IconIsMask:
            m_masks << icon;
            break;
        default: //IconNoMask
            m_icons << icon;
        }
    }
    else {
        qWarning("QIcnsHandler::addIcon(): Unable to parse icon, OSType: %u", icon.getOSType());
    }
    return icon.isValid();
}

void QIcnsHandler::scanDevice()
{
    if (!m_stream.device()->seek(0)) {
        m_scanstate = IcnsFileParsingError;
        return;
    }
    qint64 filelength = m_stream.device()->size();
    IcnsBlockHeader blockHeader;
    while (!m_stream.atEnd() || (m_stream.device()->pos() < filelength)) {
        m_stream >> blockHeader;
        if (m_stream.status() != QDataStream::Ok) {
            m_scanstate = IcnsFileParsingError;
            return;
        }
        switch (blockHeader.OSType) {
        case OSType_icns:
            filelength = blockHeader.length;
            if (m_stream.device()->size() < blockHeader.length) {
                m_scanstate = IcnsFileParsingError;
                return;
            }
            break;
        case OSType_icnV:
            m_stream.skipRawData(4);
            break;
        case OSType_TOC_: {
            QVector<IcnsBlockHeader> toc;
            const quint32 tocEntriesCount = (blockHeader.length - IcnsBlockHeaderSize) / IcnsBlockHeaderSize;
            for (uint i = 0; i < tocEntriesCount; i++) {
                IcnsBlockHeader tocEntry;
                m_stream >> tocEntry;
                toc << tocEntry;
                quint32 imgDataOffset = blockHeader.length + IcnsBlockHeaderSize;
                for (uint n = 0; n < i; n++)
                    imgDataOffset += toc.at(n).length;
                imgDataOffset += IcnsBlockHeaderSize;
                IcnsIconEntry icon(tocEntry,imgDataOffset);
                addIcon(icon);
            }
            m_scanstate = IcnsFileIsParsed;
            return; // TOC scan gives enough data to discard scan of other blocks
        }
        default: {
            IcnsIconEntry icon(blockHeader,m_stream.device()->pos());
            addIcon(icon);
            m_stream.skipRawData(icon.dataLength());
            break;
        }
        }
    }
    m_scanstate = IcnsFileIsParsed;
}

QIcnsHandler::IcnsIconEntry QIcnsHandler::getIconMask(const IcnsIconEntry &icon) const
{
    IcnsIconEntry mask = icon;
    if (icon.isValid() && !icon.isAlphaMask()) {
        IconBitDepth targetDepth = (icon.depth() == Icon32bit) ? Icon8bit : IconMono;
        for (int i = 0; i < m_masks.size(); i++) {
            bool suitable = m_masks.at(i).group() == icon.group() ||
                    (m_masks.at(i).height() == icon.height() &&
                     m_masks.at(i).width() == icon.width());
            if (suitable) {
                if (m_masks.at(i).depth() == targetDepth) {
                    mask = m_masks.at(i);
                }
            }
        }
    }
    return mask;
}

QIcnsHandler::IcnsIconEntry::IcnsIconEntry()
{
    IcnsBlockHeader h;
    h.OSType = 0;
    h.length = 0;
    m_header = h;
    m_group = IconGroupUnk;
    m_depth = IconDepthUnk;
    m_mask = IconMaskUnk;
    m_width = 0;
    m_height = 0;
    m_isValid = false;
    m_dataLength = 0;
    m_dataOffset = 0;
    m_dataIsRLE = false;
}

QIcnsHandler::IcnsIconEntry::IcnsIconEntry(IcnsBlockHeader &header, quint32 imgDataOffset)
{
    // Header:
    m_header.OSType = header.OSType;
    m_header.length = header.length;
    // Image data:
    m_dataOffset = imgDataOffset;
    m_dataLength = header.length - IcnsBlockHeaderSize;
    m_dataIsRLE = false; // Always false unless spotted
    m_isValid = false;
    // Parse everything else:
    m_isValid = parse();
}

bool QIcnsHandler::IcnsIconEntry::parse()
{
    if (m_isValid) {
        return true; // Skip if already parsed
    }
    const QByteArray OSType = QByteArray::fromHex(QByteArray::number(m_header.OSType,16));
    // Typical OSType naming: <junk><group><depth><mask>;
    const char* pattern = "^(?<junk>[\\D]{0,4})(?<group>[a-z|A-Z]{1})(?<depth>\\d{0,2})(?<mask>[#mk]{0,2})$";
    QRegularExpression regexp(pattern);
    QRegularExpressionMatch match = regexp.match(OSType);
    const bool hasMatch = match.hasMatch();
    const QString junk = match.captured("junk");
    const QString group = match.captured("group");
    const QString depth = match.captured("depth");
    const QString mask = match.captured("mask");
    // Icon group:
    m_group = group.isEmpty() ? IconGroupUnk : IconGroup(group.at(0).toLatin1());
    // Icon depth:
    m_depth = depth.toUInt() > 0 ? IconBitDepth(depth.toUInt()) : IconMono;
    // Width/height/mask:
    m_width = 0; // default for invalid ones
    m_height = 0; // default for invalid ones
    m_mask = IconMaskUnk; // default for invalid ones
    if (m_group != IconGroupCompressed) {
        const qreal bytespp = ((qreal)m_depth / 8);
        const qreal r1 = qSqrt(m_dataLength/bytespp);
        const qreal r2 = qSqrt((m_dataLength/bytespp)/2);
        const quint32 r1u = (quint32)r1;
        const quint32 r2u = (quint32)r2;
        const bool r1IsPowerOfTwoOrDevidesBy16 = (r1u == r1 && r1u % 16 == 0) || (r1u == r1 && r1 >= 16 && ((r1u & (r1u - 1)) == 0));
        const bool r2IsPowerOfTwoOrDevidesBy16 = (r2u == r2 && r2u % 16 == 0) || (r2u == r2 && r2 >= 16 && ((r2u & (r2u - 1)) == 0));

        if (r1IsPowerOfTwoOrDevidesBy16) {
            m_mask = mask.isEmpty() ? IconNoMask : IconIsMask;
            m_width = r1;
            m_height = r1;
        }
        else if (r2IsPowerOfTwoOrDevidesBy16) {
            m_mask = IconPlusMask;
            m_width = r2;
            m_height = r2;
        }
        else if (m_group == IconGroupMini) {
            // Legacy 16x12 icons are an exception from the generic square formula
            m_mask = (m_dataLength == 192*bytespp*2) ? IconPlusMask : IconNoMask;
            m_width = 16;
            m_height = 12;
        }
        else {
            if (m_depth == Icon32bit) {
                m_dataIsRLE = true; // 32bit icon may be encoded
                switch(m_group) {
                case IconGroupSmall :
                    m_width = 16;
                    m_height = 16;
                    break;
                case IconGroupLarge :
                    m_width = 32;
                    m_height = 32;
                    break;
                case IconGroupHuge :
                    m_width = 48;
                    m_height = 48;
                    break;
                case IconGroupThumbnail :
                    m_width = 128;
                    m_height = 128;
                    break;
                default :
                    qWarning() << "IcnsIconEntry::parse(): 32bit icon from an unknown group. OSType:"
                               << OSType.constData();
                }
            }
        }
    }
    else {
        // Just for experimental/research purposes.
        // Effectively does nothing at all, just tests Apple's naming policy for OSTypes.
        if (m_depth <= 10) {
            m_width = qPow(2,m_depth);
            m_height = qPow(2,m_depth);
        }
        else {
            qDebug() << "IcnsIconEntry::parse(): Compressed format id > 10 (retina?). OSType:" << OSType.constData();
        }
    }
    if (!hasMatch) {
        qWarning() << "IcnsIconEntry::parse(): Parsing failed, ignored. Reg exp: no match for OSType:" << OSType.constData();
    }
    return hasMatch;
}
