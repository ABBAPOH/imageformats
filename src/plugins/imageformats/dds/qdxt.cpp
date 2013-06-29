#include "qdxt.h"
#include <qglobal.h>

QDXT::QDXT()
{
}

void decodeColor(quint16 color, quint8 & red, quint8 & green, quint8 & blue)
{
    red = ((color >> 11) & 0x1f) << 3;
    green = ((color >> 5) & 0x3f) << 2;
    blue = (color & 0x1f) << 3;
}

quint8 calcC2(quint8 c0, quint8 c1)
{
    return 2*c0/3 + c1/3;
}

quint8 calcC2a(quint8 c0, quint8 c1)
{
    return c0/2 + c1/2;
}

quint8 calcC3(quint8 c0, quint8 c1)
{
    return c0/3 + 2*c1/3;
}

QRgb qRgba(QRgb rgb, int a)
{
    return qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), a);
}

QRgb * DXTFillColors(quint16 c0, quint16 c1, quint32 table, bool dxt1a = false)
{
    QRgb * result = new QRgb[16];
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
    return result;
}

void setAplphaDXT5(QRgb * rgbArr, quint64 alphas)
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
        rgbArr[i] = qRgba(rgbArr[i], a[index]);
        alphas = alphas >> 3;
    }
}

QImage QDXT::loadDXT1(QDataStream & s, int width, int height)
{
    qDebug("QDXT::loadDXT1");
    QImage img(width, height, QImage::Format_ARGB32);

    for (quint32 i = 0; i < height/4; i++)
        for (quint32 j = 0; j < width/4; j++) {
            quint16 c0, c1;  // color
            quint32 table; // indexes for colors
            s >> c0;
            s >> c1;
            s >> table;
            QRgb * arr = 0;
            if (c0 > c1)
                arr = DXTFillColors(c0, c1, table, false);
            else
                arr = DXTFillColors(c0, c1, table, true);

            for (int k = 0; k < 4; k++)
                for (int l = 0; l < 4; l++) {
                    img.setPixel(j*4+l, i*4+k, arr[k*4+l]);
                }
            delete arr;
        }
    return img;
}

QImage QDXT::loadDXT5(QDataStream & s, int width, int height)
{
    qDebug("QDXT::loadDXT5");
    QImage img(width, height, QImage::Format_ARGB32);

    for (quint32 i = 0; i < height/4; i++)
        for (quint32 j = 0; j < width/4; j++) {
            quint64 alpha; // together 128bit !!! calculate alpha!!!
            quint16 c0, c1;  // color
            quint32 table; // indexes for colors
            s >> alpha;
            s >> c0;
            s >> c1;
            s >> table;

            QRgb * arr = DXTFillColors(c0, c1, table);
            setAplphaDXT5(arr, alpha);

            for (int k = 0; k < 4; k++)
                for (int l = 0; l < 4; l++) {
                   img.setPixel(j*4+l, i*4+k, arr[k*4+l]);
            }
            delete arr;
        }
    return img;
}
