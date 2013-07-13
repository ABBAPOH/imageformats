#include "qicnshandler.h"

QIcnsHandler::QIcnsHandler(QIODevice *device)
{
    m_reader = new IcnsReader(device);
    m_currentIconIndex = 0;
}

QIcnsHandler::~QIcnsHandler()
{
    delete m_reader;
}

bool QIcnsHandler::read(QImage *outImage)
{
    qDebug("QIcnsHandler::read() call, m_currentIconIndex=%i", m_currentIconIndex);
    QImage img = m_reader->iconAt(m_currentIconIndex);    
    *outImage = img;
    return !img.isNull();
}

QByteArray QIcnsHandler::name() const
{
    return "icns";
}

bool QIcnsHandler::canRead(QIODevice *device)
{
    if (!device) {
        qWarning("QIcnsHandler::canRead() called with no device");
        return false;
    }
    if(device->isSequential()) {
        qWarning("QIcnsHandler::canRead() called on sequential device (NYI)");
        return false;
    }
    return device->peek(4) == "icns";
}

bool QIcnsHandler::canRead() const
{
    if (canRead(device())) {
        setFormat("icns");
        return true;
    }
    return false;
}

int QIcnsHandler::imageCount() const
{
    return m_reader->count();
}

bool QIcnsHandler::jumpToImage(int imageNumber)
{
    if (imageNumber < imageCount())
        m_currentIconIndex = imageNumber;
    return (imageNumber < imageCount()) ? true : false;
}

bool QIcnsHandler::jumpToNextImage()
{
    return jumpToImage(m_currentIconIndex + 1);
}
