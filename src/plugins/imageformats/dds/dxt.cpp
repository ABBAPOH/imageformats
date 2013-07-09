#include "dxt.h"

#include <qglobal.h>

enum Version {
    One = 1,
    Two = 2,
    Three = 3,
    Four = 4,
    Five = 5,
    RXGB = 6
};

static inline void decodeColor(quint16 color, quint8 & red, quint8 & green, quint8 & blue)
{
    red = ((color >> 11) & 0x1f) << 3;
    green = ((color >> 5) & 0x3f) << 2;
    blue = (color & 0x1f) << 3;
}

static inline quint8 calcC2(quint8 c0, quint8 c1)
{
    return 2*c0/3 + c1/3;
}

static inline quint8 calcC2a(quint8 c0, quint8 c1)
{
    return c0/2 + c1/2;
}

static inline quint8 calcC3(quint8 c0, quint8 c1)
{
    return c0/3 + 2*c1/3;
}

static void DXTFillColors(QRgb * result, quint16 c0, quint16 c1, quint32 table, bool dxt1a = false)
{
    quint8 r[4];
    quint8 g[4];
    quint8 b[4];
    quint8 a[4];
    for (int i = 0; i < 4; i++)
        a[i] = 255;
    decodeColor(c0, r[0], g[0], b[0]);
    decodeColor(c1, r[1], g[1], b[1]);
    if (!dxt1a) {
        r[2] = calcC2(r[0], r[1]);
        g[2] = calcC2(g[0], g[1]);
        b[2] = calcC2(b[0], b[1]);
        r[3] = calcC3(r[0], r[1]);
        g[3] = calcC3(g[0], g[1]);
        b[3] = calcC3(b[0], b[1]);
    } else {
        r[2] = calcC2a(r[0], r[1]);
        g[2] = calcC2a(g[0], g[1]);
        b[2] = calcC2a(b[0], b[1]);
        r[3] = 0;
        g[3] = 0;
        b[3] = 0;
        a[3] = 0;
    }

    for (int k = 0; k < 4; k++)
        for (int l = 0; l < 4; l++) {
        unsigned index = table & 0x0003;
        table = table >> 2;

        int red = r[index];
        int green = g[index];
        int blue = b[index];
        int alpha = a[index];
        result[k*4+l] = qRgba(red, green, blue, alpha);
    }
}

static void setAplphaDXT23(QRgb * rgbArr, quint64 alphas, bool premultiplied)
{
    for (int i = 0; i < 16; i++) {
        quint8 alpha = 16*(alphas & 0x0f);
        QRgb rgb = rgbArr[i];
        if (premultiplied) // DXT2
            rgbArr[i] = qRgba(qRed(rgb)*alpha/0xff, qGreen(rgb)*alpha/0xff, qBlue(rgb)*alpha/0xff, alpha);
        else // DXT3
            rgbArr[i] = qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), alpha);
        alphas = alphas >> 4;
    }
}

static void setAplphaDXT45(QRgb * rgbArr, quint64 alphas, bool premultiplied)
{
    quint8 a[8];
    a[0] = alphas & 0xff;
    a[1] = (alphas >> 8) & 0xff;
    if (a[0] > a[1]) {
        a[2] = (6*a[0] + 1*a[1])/7;
        a[3] = (5*a[0] + 2*a[1])/7;
        a[4] = (4*a[0] + 3*a[1])/7;
        a[5] = (3*a[0] + 4*a[1])/7;
        a[6] = (2*a[0] + 5*a[1])/7;
        a[7] = (1*a[0] + 6*a[1])/7;
    } else {
        a[2] = (4*a[0] + 1*a[1])/5;
        a[3] = (3*a[0] + 2*a[1])/5;
        a[4] = (2*a[0] + 3*a[1])/5;
        a[5] = (1*a[0] + 4*a[1])/5;
        a[6] = 0;
        a[7] = 255;
    }
    alphas >>= 16;
    for (int i = 0; i < 16; i++) {
        quint8 index = alphas & 0x07;
        quint8 alpha = a[index];
        QRgb rgb = rgbArr[i];
        if (premultiplied) // DXT4
            rgbArr[i] = qRgba(qRed(rgb)*alpha/0xff, qGreen(rgb)*alpha/0xff, qBlue(rgb)*alpha/0xff, alpha);
        else // DXT5
            rgbArr[i] = qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), alpha);
        alphas = alphas >> 3;
    }
}

static QRgb invertRXGBColors(QRgb pixel)
{
    quint8 g = qGreen(pixel);
    quint8 b = qBlue(pixel);
    quint8 a = qAlpha(pixel);
    return qRgb(a, g, b);
}

static QImage loadDXT(Version version, QDataStream &s, quint32 width, quint32 height)
{
    QImage::Format format = (version == Two || version == Four) ?
                QImage::Format_ARGB32_Premultiplied : QImage::Format_ARGB32;

    QImage img(width, height, format);

    for (quint32 i = 0; i < height; i += 4) {
        for (quint32 j = 0; j < width; j += 4) {
            quint64 alpha;
            quint16 c0, c1;
            quint32 table;
            if (version != One)
                s >> alpha;
            s >> c0;
            s >> c1;
            s >> table;

            QRgb arr[16];

            DXTFillColors(arr, c0, c1, table, version == One && c0 <= c1);
            switch (version) {
            case Two:
                setAplphaDXT23(arr, alpha, true);
                break;
            case Three:
                setAplphaDXT23(arr, alpha, false);
                break;
            case Four:
                setAplphaDXT45(arr, alpha, true);
                break;
            case Five:
            case RXGB:
                setAplphaDXT45(arr, alpha, false);
                break;
            default:
                break;
            }

            for (int k = 0; k < 4; k++)
                for (int l = 0; l < 4; l++) {
                    quint32 x = j + l, y = i + k;
                    if (x < width && y < height) {
                        QRgb pixel = arr[k*4+l];
                        if (version == RXGB)
                            pixel = invertRXGBColors(pixel);
                        img.setPixel(x, y, pixel);
                    }
            }
        }
    }
    return img;
}

QImage QDXT::loadDXT1(QDataStream &s, quint32 width, quint32 height)
{
    return loadDXT(One, s, width, height);
}

QImage QDXT::loadDXT2(QDataStream &s, quint32 width, quint32 height)
{
    return loadDXT(Two, s, width, height);
}

QImage QDXT::loadDXT3(QDataStream &s, quint32 width, quint32 height)
{
    return loadDXT(Three, s, width, height);
}

QImage QDXT::loadDXT4(QDataStream &s, quint32 width, quint32 height)
{
    return loadDXT(Four, s, width, height);
}

QImage QDXT::loadDXT5(QDataStream &s, quint32 width, quint32 height)
{
    return loadDXT(Five, s, width, height);
}

QImage QDXT::loadRXGB(QDataStream &s, quint32 width, quint32 height)
{
    return loadDXT(RXGB, s, width, height);
}
