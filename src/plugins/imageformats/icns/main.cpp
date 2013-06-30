#include "main.h"

QImageIOPlugin::Capabilities QIcnsPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (format == "icns")
        return Capabilities(CanRead);
    if (!format.isEmpty())
        return 0;
    if (!device->isOpen())
        return 0;

    Capabilities cap;
    if (device->isReadable() && QIcnsHandler::canRead(device))
        cap |= CanRead;
    //if (device->isWritable())
        //cap |= CanWrite;
    return cap;
}

QImageIOHandler *QIcnsPlugin::create(QIODevice *device, const QByteArray &format) const
{
    QImageIOHandler *handler = new QIcnsHandler(device);
    handler->setDevice(device);
    handler->setFormat(format);
    return handler;
}
