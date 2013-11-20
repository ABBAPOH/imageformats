
#include <QtGui/QImageIOPlugin>

#ifndef QT_NO_IMAGEFORMATPLUGIN

#ifdef QT_NO_IMAGEFORMAT_ICNS
#undef QT_NO_IMAGEFORMAT_ICNS
#endif

QT_BEGIN_NAMESPACE

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

QT_END_NAMESPACE

#endif // QT_NO_IMAGEFORMATPLUGIN
