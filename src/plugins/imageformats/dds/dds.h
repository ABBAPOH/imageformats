
#ifndef DDS_H
#define DDS_H

#include <QtGui/QImageIOPlugin>
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
    int m_currentImage;
    bool m_headerCached;
};

// ===================== DDSPlugin =====================

class DDSPlugin : public QImageIOPlugin
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "dds.json")
#endif

public:
#if QT_VERSION < 0x050000
    QStringList keys() const { return QStringList() << "dds"; };
#endif
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const;
};


#endif // DDS
