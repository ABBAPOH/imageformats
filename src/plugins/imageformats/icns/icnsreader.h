#ifndef ICNSREADER_H
#define ICNSREADER_H

#include "icnsheader.h"
#include "icnspalette.h"

#include <math.h>
#include <QtGui/QImage>
#include <QtCore/QDataStream>
#include <QtCore/QVector>
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
private:
    bool scanFile();

    bool parseIconDetails(IcnsIconEntry &icon);
    bool addIcon(IcnsIconEntry &icon);

    bool decompressRLE24toR8G8B8(QByteArray &encodedBytes, quint32 expectedPixelCount);
    bool getA8MaskForIcon(const IcnsIconEntry &icon, QByteArray &A8Mask);

    QDataStream m_stream;
    QVector<IcnsIconEntry> m_icons;
    QVector<IcnsIconEntry> m_masks;
    bool m_scanned;
};

#endif //ICNSREADER_H
