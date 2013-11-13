#ifndef QDDSHANDLER_H
#define QDDSHANDLER_H

#include <QtGui/QImageIOHandler>
#include "ddsheader.h"

class QDDSHandler : public QImageIOHandler
{
public:
    QDDSHandler();

    bool canRead() const;
    bool read(QImage *image);
    bool write(const QImage &image);

    int imageCount() const;
    bool jumpToImage(int imageNumber);

    QByteArray name() const;

    static bool canRead(QIODevice *device);

private:
    bool ensureHeaderCached() const;
    bool verifyHeader(const DDSHeader &dds) const;

private:
    DDSHeader header;
    int m_format;
    DDSHeaderDX10 header10;
    int m_currentImage;
    mutable bool m_headerCached;
};

#endif // QDDSHANDLER_H
