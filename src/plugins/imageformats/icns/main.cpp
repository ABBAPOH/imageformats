#include "main.h"

QImageIOPlugin::Capabilities QIcnsPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "icns")
        return Capabilities(CanRead | CanWrite);
    if (!format.isEmpty())
        return 0;
    if (!device->isOpen())
        return 0;

    Capabilities cap;
    if (device->isReadable() && QIcnsHandler::canRead(device))
        cap |= CanRead;
    if (device->isWritable() && QIcnsHandler::canWrite(device))
        cap |= CanWrite;
    return cap;
}

QImageIOHandler *QIcnsPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new QIcnsHandler(device, format);
    return handler;
}

#if QT_VERSION < 0x050000
QStringList QIcnsPlugin::keys() const
{
     return QStringList() << "icns";
}

Q_EXPORT_STATIC_PLUGIN(QIcnsPlugin)
Q_EXPORT_PLUGIN2(qicns, QIcnsPlugin)
#endif
