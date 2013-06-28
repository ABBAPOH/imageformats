
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

#endif // DDS
