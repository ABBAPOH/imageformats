#ifndef DXT_H
#define DXT_H

#include <QImage>

namespace QDXT {

enum Version {
    One = 1,
    Two = 2,
    Three = 3,
    Four = 4,
    Five = 5
};

QImage loadDXT(Version version, QDataStream & s, quint32 width, quint32 height);

} // namespace QDXT

#endif // DXT_H
