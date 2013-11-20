
#ifndef MAIN_H
#define MAIN_H

#include <QtGui/QImageIOPlugin>

#ifndef QT_NO_IMAGEFORMATPLUGIN

#ifdef QT_NO_IMAGEFORMAT_ICNS
#undef QT_NO_IMAGEFORMAT_ICNS
#endif

QT_BEGIN_NAMESPACE

class QIcnsPlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "icns.json")

public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const;
};

QT_END_NAMESPACE

#endif // QT_NO_IMAGEFORMATPLUGIN

#endif // MAIN_H
