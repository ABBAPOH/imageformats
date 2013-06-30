#include "qicnshandler.h"

#include <QtGui/QImage>
#include <QtCore/QtEndian>
#include <QtCore/QDataStream>

#include <QDebug>

QDataStream &operator>>(QDataStream &in, IcnsHeader &p)
{
    in >> p.magic[0] >> p.magic[1] >> p.magic[2] >> p.magic[3];
    in >> p.size;
    return in;
}

QDataStream &operator>>(QDataStream &in, IcnsDataBlockHeader &p)
{
    in >> p.ostype[0] >> p.ostype[1] >> p.ostype[2] >> p.ostype[3];
    in >> p.blockLength;
    return in;
}

QDataStream &operator>>(QDataStream &in, IcnsTOCEntry &p)
{
    in >> p.ostype[0] >> p.ostype[1] >> p.ostype[2] >> p.ostype[3];
    in >> p.size;
    return in;
}

IcnsReader::IcnsReader(QIODevice *iodevice)
{
    m_iodevice = iodevice;
    m_scanned = false;
    m_stream.setDevice(m_iodevice);
    m_stream.setByteOrder(QDataStream::BigEndian);
}

void IcnsReader::scanBlocks()
{
    if(m_iodevice) {
        qint64 oldPos = m_iodevice->pos();

        bool canRead = true;
        if(m_iodevice->seek(sizeof(IcnsHeader))) {
            while(canRead) {
                canRead = (!m_stream.atEnd() &&
                           (m_iodevice->size() - m_iodevice->pos()) > sizeof(IcnsDataBlockHeader));
                if(canRead) {
                    IcnsDataBlockHeader blockHeader;
                    m_stream >> blockHeader;

                    QByteArray magic(*blockHeader.ostype,4);
                    if(magic == "TOC ") {
                        // A Table of Contents file. Mmm... A table of contents! *drool*
                        // Actually, it doesn't have any use for now, but who knows?
                        quint32 tocDataLength = blockHeader.blockLength - sizeof(IcnsDataBlockHeader);
                        quint32 tocEntriesNum = tocDataLength / sizeof(IcnsTOCEntry);
                        for(uint i = 0; i < tocEntriesNum; i++) {
                            IcnsTOCEntry tocEntry;
                            m_stream >> tocEntry;
                            m_toc << tocEntry;
                        }
                    }
                    else if(magic == "icnV")
                        // Icon Composer version - is there any use? Information?
                        m_stream.skipRawData(4);
                    else {
                        // Assume it is an image format - map the image and skip to next block
                        IcnsIconEntry icon;
                        icon.header = blockHeader;
                        icon.imageDataOffset = m_iodevice->pos();
                        m_icons << icon;
                        quint32 imageDataLength = icon.header.blockLength - sizeof(IcnsDataBlockHeader);
                        m_stream.skipRawData(m_iodevice->pos() + imageDataLength);
                    }
                }
            }
            m_iodevice->seek(oldPos);
        }
        m_scanned = true;
    }
}

int IcnsReader::count()
{
    if(!m_scanned)
        scanBlocks();
    return m_icons.size();
}

QImage IcnsReader::iconAt(int index)
{
    QImage img;

    if(!m_scanned)
        scanBlocks();

    IcnsIconEntry iconEntry = m_icons.at(index);
    quint32 imageDataSize = iconEntry.header.blockLength - sizeof(iconEntry.header);

    if(m_iodevice->seek(iconEntry.imageDataOffset)) {

        const quint64 pngMagic = 0x89504E470D0A1A0A;
        const quint64 readMagic = qFromBigEndian<quint64>((const uchar*)m_iodevice->peek(8).constData());;
        const bool isPngImage = (readMagic == pngMagic);

        if(isPngImage) {
            QByteArray imageData;
            imageData.resize(imageDataSize);
            m_stream.readRawData(imageData.data(), imageDataSize);
            return QImage::fromData(imageData, "png");
        }
        else
        {
            //To do
        }
    }

    return img;
}


QIcnsHandler::QIcnsHandler(QIODevice *device)
{
    m_reader = new IcnsReader(device);
}

QIcnsHandler::~QIcnsHandler()
{
    delete m_reader;
}

bool QIcnsHandler::read(QImage *outImage)
{
    qDebug() << "QIcnsHandler::read";

    QImage img = m_reader->iconAt(m_currentIconIndex);

    *outImage = img;
    return true;
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
