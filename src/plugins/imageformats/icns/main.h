#include <QtGui/QImageIOPlugin>
#include "icnshandler.h"

class QIcnsPlugin : public QImageIOPlugin
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "icns.json")
#endif

public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const;
#if QT_VERSION < 0x050000
    QStringList keys() const;
#endif
};
