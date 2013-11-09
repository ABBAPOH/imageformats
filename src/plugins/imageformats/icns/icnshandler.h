#ifndef ICNSHANDLER_H
#define ICNSHANDLER_H

#include <QtGui/QImageIOHandler>
#include "icnsreader.h"

class QIcnsHandler : public QImageIOHandler
{
public:
    QIcnsHandler(QIODevice *device);
    ~QIcnsHandler();

    bool canRead() const;
    bool read(QImage *image);
    bool write(const QImage &image);

    QByteArray name() const;

    int imageCount() const;
    bool jumpToImage(int imageNumber);
    bool jumpToNextImage();

    static bool canRead(QIODevice *device);
    static bool canWrite(QIODevice *device);

private:
    int m_currentIconIndex;
    IcnsReader* m_reader;
};

#endif //ICNSHANDLER_H
