#ifndef QICNSHANDLER_H
#define QICNSHANDLER_H

#include "icnsreader.h"
#include <QtGui/QImageIOHandler>

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
