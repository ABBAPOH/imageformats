
#ifndef DDS_H
#define DDS_H

#include <QtGui/QImageIOPlugin>

class DDSHandler : public QImageIOHandler
{
public:
    DDSHandler();
	
	bool canRead() const;
	bool read(QImage *image);
	bool write(const QImage &image);
	
	QByteArray name() const;
	
	static bool canRead(QIODevice *device);
};

// ===================== DDSPlugin =====================

class DDSPlugin : public QImageIOPlugin
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.preveder.imageformats.DDSPlugin")
#endif

public:
    QStringList keys() const;
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const;
};


#endif // DDS
