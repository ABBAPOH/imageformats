#include <QtGui/QImageIOPlugin>

#include "qicnshandler.h"

class QIcnsPlugin : public QImageIOPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "com.preveder.imageformats.IcnsPlugin")
#endif

public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const;
    QStringList keys() const { return QStringList() << "icns"; }
};
