#include "qicnshandler.h"

#include <QtGui/QImage>
#include <QtCore/QtEndian>
#include <QtCore/QDataStream>

#include <QDebug>

QDataStream &operator>>(QDataStream &in, IcnsBlockHeader &p)
{
    in >> p.magic;
    in >> p.length;
    return in;
}

IcnsReader::IcnsReader(QIODevice *iodevice)
{
    m_iodevice = iodevice;
    m_stream.setDevice(m_iodevice);
    m_stream.setByteOrder(QDataStream::BigEndian);
    m_scanned = false;
}

bool IcnsReader::scanBlocks()
{
    Q_ASSERT(m_iodevice);
    m_iodevice->seek(0);

    IcnsBlockHeader blockHeader;

    while (!m_stream.atEnd()) {

        m_stream >> blockHeader;
        if (m_stream.status() == QDataStream::ReadPastEnd)
            return false;

        switch (blockHeader.magic) {
        case icns:
            if (m_iodevice->size() != blockHeader.length)
                return false;
            break;
        case TOC_: {
            const quint32 tocEntriesN = (blockHeader.length - IcnsBlockHeaderSize) / IcnsBlockHeaderSize;
            for(uint i = 0; i < tocEntriesN; i++) {
                IcnsBlockHeader tocEntry;
                m_stream >> tocEntry;

                IcnsIconEntry icon;
                icon.header = tocEntry;

                const quint32 imgDataBaseOffset = (i == 0) ?
                            (blockHeader.length + IcnsBlockHeaderSize*2) : // offset for the first icon
                            (blockHeader.length + IcnsBlockHeaderSize);    // offset of the first block
                quint32 imgDataOffset = imgDataBaseOffset;
                for(uint n = 0; n < i; n++)
                    imgDataOffset += m_icons.at(n).header.length;

                icon.imageDataOffset = (i == 0) ? imgDataOffset : imgDataOffset + IcnsBlockHeaderSize;
                m_icons << icon;
            }
            return true; // TOC scan gives enough data to discard scan of other blocks
        }
        case icnV:
            m_stream.skipRawData(4);
            break;
        default: {
            IcnsIconEntry icon;
            icon.header = blockHeader;
            icon.imageDataOffset = m_iodevice->pos();
            m_icons << icon;
            quint32 imageDataLength = icon.header.length - IcnsBlockHeaderSize;
            m_stream.skipRawData(imageDataLength);
            break;
        }
        }
    }
    return true;
}

int IcnsReader::count()
{
    if(!m_scanned)
        m_scanned = scanBlocks();
    return m_icons.size();
}

QImage IcnsReader::iconAt(int index)
{
    QImage img;

    if(!m_scanned)
        m_scanned = scanBlocks();

    IcnsIconEntry iconEntry = m_icons.at(index);
    quint32 imageDataSize = iconEntry.header.length - IcnsBlockHeaderSize;

    if(m_iodevice->seek(iconEntry.imageDataOffset)) {

        QByteArray read8bytesMagic = m_iodevice->peek(8).toHex().toUpper();
        QByteArray read12bytesMagic = m_iodevice->peek(12).toHex().toUpper();

        const char* jp2Magic = "0000000C6A5020200D0A870A";
        const char* pngMagic = "89504E470D0A1A0A";
        const bool isPngImage = (read8bytesMagic == pngMagic);
        const bool isJP2Image = (read12bytesMagic == jp2Magic);

        if(isPngImage)
            return QImage::fromData(m_iodevice->peek(imageDataSize), "png");
        else if(isJP2Image) {
            //To do: JPEG 2000 (need another plugin for that?)
        }
        else {
            //To do: subformats
        }
    }

    return img;
}


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
