#include "qicnshandler.h"

#include <QtGui/QImage>
#include <QtCore/QDataStream>
#include <QtCore/QRegularExpression>
#include <QtCore/QRegularExpressionMatch>

#include <QDebug>

QDataStream &operator>>(QDataStream &in, IcnsBlockHeader &p)
{
    in >> p.magic;
    in >> p.length;
    return in;
}

IcnsReader::IcnsReader(QIODevice *iodevice)
{
    Q_ASSERT(iodevice);
    m_stream.setDevice(iodevice);
    m_stream.setByteOrder(QDataStream::BigEndian);
    m_scanned = false;
}

void IcnsReader::parseIconDetails(IcnsIconEntry &icon) {
    qint64 oldPos = m_stream.device()->pos();
    if(m_stream.device()->seek(icon.imageDataOffset)) {
        if(m_stream.device()->peek(8).toHex() == "89504e470d0a1a0a")
            icon.iconFormat = IconPNG;
        else if(m_stream.device()->peek(12).toHex() == "0000000c6a5020200d0a870a")
            icon.iconFormat = IconJP2;
        else {
            icon.iconFormat = IconUncompressed;
            QByteArray magic = QByteArray::fromHex(QByteArray::number(icon.header.magic,16));
            QRegularExpression pattern("^(?<junk>[^0-9]{0,4})(?<family>[a-z|A-Z]{1})(?<depth>\\d{0,2})(?<mask>[#mk]{0,2})$");
            QRegularExpressionMatch match = pattern.match(magic);
            const QString junk = match.captured("junk");
            const QString family = match.captured("family");
            const QString depth = match.captured("depth");
            const QString mask = match.captured("mask");
            icon.iconFamily = family.at(0).toLatin1();
            icon.iconBitDepth = (depth.toUInt() == 0) ? 1 : depth.toUInt(); // mono or <depth>
            icon.iconIsMask = !mask.isEmpty();
            if(match.hasMatch()) {
                qDebug() << "IcnsReader::parseIconDetails() parse:" << junk << family << depth << mask
                         << icon.iconFamily << icon.iconBitDepth << icon.iconIsMask;
            }
            else
                qDebug() << "IcnsReader::parseIconDetails() reg exp: no match for:" << magic;
        }
        m_stream.device()->seek(oldPos);
    }
}

bool IcnsReader::scanBlocks()
{
    Q_ASSERT(m_stream.device());
    m_stream.device()->seek(0);

    IcnsBlockHeader blockHeader;
    while (!m_stream.atEnd()) {

        m_stream >> blockHeader;
        if (m_stream.status() == QDataStream::ReadPastEnd)
            return false;

        switch (blockHeader.magic) {
        case icnsfile:
            if (m_stream.device()->size() != blockHeader.length)
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
                icon.imageDataSize = icon.header.length - IcnsBlockHeaderSize;
                parseIconDetails(icon);
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
            icon.imageDataOffset = m_stream.device()->pos();
            icon.imageDataSize = icon.header.length - IcnsBlockHeaderSize;
            parseIconDetails(icon);
            m_icons << icon;
            m_stream.skipRawData(icon.imageDataSize);
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
    if(!m_scanned)
        m_scanned = scanBlocks();

    QImage img;
    IcnsIconEntry iconEntry = m_icons.at(index);

    if(m_stream.device()->seek(iconEntry.imageDataOffset)) {
        switch(iconEntry.iconFormat) {
        case IconPNG:
            return QImage::fromData(m_stream.device()->peek(iconEntry.imageDataSize), "png");
            break;
        case IconJP2:
            //To do: JPEG 2000 (need another plugin for that?)
            break;
        default:
            //To do: subformats
            break;
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
