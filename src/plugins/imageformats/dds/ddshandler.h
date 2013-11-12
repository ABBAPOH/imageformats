#ifndef DDSHANDLER_H
#define DDSHANDLER_H

#include <QtGui/QImageIOHandler>
#include "ddsheader.h"

class DDSHandler : public QImageIOHandler
{
public:
    DDSHandler();

    bool canRead() const;
    bool read(QImage *image);
    bool write(const QImage &image);

    int imageCount() const;
    bool jumpToImage(int imageNumber);

    QByteArray name() const;

    static bool canRead(QIODevice *device);

private:
    void ensureHeaderCached() const;

private:
    DDSHeader header;
    int m_format;
    DDSHeaderDX10 header10;
    int m_currentImage;
    mutable bool m_headerCached;
};

#endif // DDSHANDLER_H
