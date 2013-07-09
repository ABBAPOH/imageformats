#ifndef QICNSHANDLER_H
#define QICNSHANDLER_H

#include <QtGui/QImageIOHandler>
#include <QtCore/QVector>

#include "icnsheader.h"

class IcnsReader
{
public:
    IcnsReader(QIODevice *iodevice);

    int count();
    QImage iconAt(int index);

private:
    bool scanBlocks();

    bool parseIconDetails(IcnsIconEntry &icon);

    QByteArray decompressRLE24(const QByteArray &encodedBytes, quint32 expectedPixelCount);
    bool initIconPalette(QImage &img, quint8 depth);

    QDataStream m_stream;
    bool m_scanned;
    QVector<IcnsIconEntry> m_icons;
};

class QIcnsHandler : public QImageIOHandler
{
public:
    QIcnsHandler(QIODevice *device);
    ~QIcnsHandler();

    bool canRead() const;
    bool read(QImage *image);
	
    QByteArray name() const;

    int imageCount() const;
    bool jumpToImage(int imageNumber);
    bool jumpToNextImage();

    static bool canRead(QIODevice *device);

private:

    int m_currentIconIndex;
    IcnsReader* m_reader;
};

#endif // QICNSHANDLER
