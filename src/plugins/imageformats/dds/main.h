#ifndef MAIN_H
#define MAIN_H

#include <QtGui/QImageIOPlugin>

class QDDSPlugin : public QImageIOPlugin
{
    Q_OBJECT

#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QImageIOHandlerFactoryInterface" FILE "dds.json")
#endif

public:
#if QT_VERSION < 0x050000
    QStringList keys() const;
#endif
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const;
    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const;
};

#endif // MAIN_H
