#ifndef QDXT_H
#define QDXT_H

#include <QImage>

class QDXT
{
public:
    QDXT();
    static QImage loadDXT1(QDataStream & s, quint32 width, quint32 height);
    static QImage loadDXT5(QDataStream & s, quint32 width, quint32 height);
};

#endif // QDXT_H
