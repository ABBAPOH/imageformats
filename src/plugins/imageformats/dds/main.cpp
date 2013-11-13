#include "main.h"

#include "qddshandler.h"

#if QT_VERSION < 0x050000
QStringList QDDSPlugin::keys() const
{
    return QStringList() << "dds";
}
#endif

QImageIOPlugin::Capabilities QDDSPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (!device || !device->isOpen())
        return 0;
    if (format.toLower() != "dds")
        return 0;

    Capabilities cap;
    if (device->isReadable() && QDDSHandler::canRead(device))
        cap |= CanRead;
    if (device->isWritable())
        cap |= CanWrite;
    return cap;
}

QImageIOHandler *QDDSPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new QDDSHandler;
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}

#if QT_VERSION < 0x050000
Q_EXPORT_STATIC_PLUGIN(QDDSPlugin)
Q_EXPORT_PLUGIN2(dds, QDDSPlugin)
#endif
