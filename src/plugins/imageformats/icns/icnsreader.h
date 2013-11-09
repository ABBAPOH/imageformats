#ifndef ICNSREADER_H
#define ICNSREADER_H

#include "icnsformat.h"
#include "icnspalette.h"

#include <cmath>
#include <QtGui/QImage>
#include <QtGui/QPainter>
#include <QtCore/QVector>
#include <QtCore/QBuffer>
#if QT_VERSION >= 0x050000
#include <QtCore/QRegularExpression>
#endif
#include <QDebug>

class IcnsReader
{
public:
    IcnsReader(QIODevice *iodevice);

    int count();
    QImage iconAt(int index);
    QImage iconAlphaAt(int index);

    bool write(QIODevice * device, const QImage &image);
private:
    bool scanFile();

    bool parseIconDetails(IcnsIconEntry & icon);
    bool addIcon(IcnsIconEntry & icon);

    QByteArray getRGB32fromRLE24(const QByteArray & encodedBytes, quint32 expectedPixelCount);
    QVector<QRgb> getColorTable(const IcnsIconBitDepth & depth);

    QDataStream m_stream;
    QVector<IcnsIconEntry> m_icons;
    QVector<IcnsIconEntry> m_masks;
    bool m_scanned;
};

#endif //ICNSREADER_H
