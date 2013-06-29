#ifndef DXT_H
#define DXT_H

#include <QImage>

namespace QDXT {

QImage loadDXT1(QDataStream & s, quint32 width, quint32 height);
QImage loadDXT2(QDataStream & s, quint32 width, quint32 height);
QImage loadDXT3(QDataStream & s, quint32 width, quint32 height);
QImage loadDXT5(QDataStream & s, quint32 width, quint32 height);

} // namespace QDXT

#endif // DXT_H
