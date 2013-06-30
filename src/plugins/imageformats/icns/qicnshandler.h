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
    void scanBlocks();

    QIODevice *m_iodevice;
    QDataStream m_stream;

    bool m_scanned;
    QVector<IcnsIconEntry> m_icons;
    QVector<IcnsTOCEntry> m_toc;
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
