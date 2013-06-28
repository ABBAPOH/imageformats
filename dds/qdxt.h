#ifndef QDXT_H
#define QDXT_H

#include <QImage>

class QDXT
{
public:
    QDXT();
    static QImage loadDXT1(QDataStream & s, int width, int height);
    static QImage loadDXT5(QDataStream & s, int width, int height);
};

#endif // QDXT_H
