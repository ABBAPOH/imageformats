
#include "main.h"

#ifndef QT_NO_IMAGEFORMATPLUGIN

#include "qicnshandler.h"

QT_BEGIN_NAMESPACE

QImageIOPlugin::Capabilities QIcnsPlugin::capabilities(QIODevice *device, const QByteArray &format) const
{
    if (!device || !device->isOpen() || format != "icns")
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

QT_END_NAMESPACE

#endif // QT_NO_IMAGEFORMATPLUGIN
