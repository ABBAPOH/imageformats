#include <QtTest/QtTest>
#include <QtGui/QtGui>

class DDSTest: public QObject
{
    Q_OBJECT

private slots:
    void readImage_data();
    void readImage();
    void testMipmaps_data();
    void testMipmaps();
};

static bool compareImages(const QImage &first, const QImage &second)
{
    QImage a = first.convertToFormat(QImage::Format_ARGB32);
    QImage b = second.convertToFormat(QImage::Format_ARGB32);

    if (a.size() != b.size())
        return false;

    for (int x = 0; x < a.width(); ++x) {
        for (int y = 0; y < a.height(); ++y) {
            if (a.pixel(x, y) != b.pixel(x, y))
                return false;
        }
    }
    return true;
}

void DDSTest::readImage_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QSize>("size");

    QTest::newRow("1") << QString("A1R5G5B5") << QSize(64, 64);
    QTest::newRow("2") << QString("A2B10G10R10") << QSize(64, 64);
    QTest::newRow("3") << QString("A2R10G10B10") << QSize(64, 64);
//    QTest::newRow("4") << QString("A2W10V10U10") << QSize(64, 64);
    QTest::newRow("5") << QString("A4L4") << QSize(64, 64);
    QTest::newRow("6") << QString("A4R4G4B4") << QSize(64, 64);
    QTest::newRow("7") << QString("A8") << QSize(64, 64);
    QTest::newRow("8") << QString("A8B8G8R8") << QSize(64, 64);
    QTest::newRow("9") << QString("A8L8") << QSize(64, 64);
    QTest::newRow("10") << QString("A8R3G3B2") << QSize(64, 64);
    QTest::newRow("11") << QString("A8R8G8B8") << QSize(64, 64);
    QTest::newRow("12") << QString("A16B16G16R16") << QSize(64, 64);
    QTest::newRow("13") << QString("A16B16G16R16F") << QSize(64, 64);
    QTest::newRow("14") << QString("A32B32G32R32F") << QSize(64, 64);
//    QTest::newRow("15") << QString("CxV8U8") << QSize(64, 64);
    QTest::newRow("16") << QString("DXT1") << QSize(50, 50);
    QTest::newRow("17") << QString("DXT2") << QSize(64, 64);
    QTest::newRow("18") << QString("DXT3") << QSize(64, 64);
    QTest::newRow("19") << QString("DXT4") << QSize(64, 64);
    QTest::newRow("20") << QString("DXT5") << QSize(64, 64);
    QTest::newRow("21") << QString("G8R8_G8B8") << QSize(64, 64);
    QTest::newRow("22") << QString("G16R16") << QSize(64, 64);
    QTest::newRow("23") << QString("G16R16F") << QSize(64, 64);
    QTest::newRow("24") << QString("G32R32F") << QSize(64, 64);
//    QTest::newRow("25") << QString("L6V5U5") << QSize(64, 64);
    QTest::newRow("26") << QString("L8") << QSize(64, 64);
    QTest::newRow("27") << QString("L16") << QSize(64, 64);
    QTest::newRow("28") << QString("P8") << QSize(64, 64);
//    QTest::newRow("29") << QString("Q8W8V8U8") << QSize(64, 64);
//    QTest::newRow("30") << QString("Q16W16V16U16") << QSize(64, 64);
    QTest::newRow("31") << QString("R3G3B2") << QSize(64, 64);
    QTest::newRow("32") << QString("R5G6B5") << QSize(64, 64);
    QTest::newRow("33") << QString("R8G8_B8G8") << QSize(64, 64);
    QTest::newRow("34") << QString("R8G8B8") << QSize(64, 64);
    QTest::newRow("35") << QString("R16F") << QSize(64, 64);
    QTest::newRow("36") << QString("R32F") << QSize(64, 64);
    QTest::newRow("37") << QString("UYVY") << QSize(64, 64);
//    QTest::newRow("38") << QString("V8U8") << QSize(64, 64);
//    QTest::newRow("39") << QString("V16U16") << QSize(64, 64);
    QTest::newRow("40") << QString("X1R5G5B5") << QSize(64, 64);
    QTest::newRow("41") << QString("X4R4G4B4") << QSize(64, 64);
    QTest::newRow("42") << QString("X8B8G8R8") << QSize(64, 64);
//    QTest::newRow("43") << QString("X8L8V8U8") << QSize(64, 64);
    QTest::newRow("44") << QString("X8R8G8B8") << QSize(64, 64);
    QTest::newRow("45") << QString("YUY2") << QSize(64, 64);
}

void DDSTest::readImage()
{
    QFETCH(QString, fileName);
    QFETCH(QSize, size);

    QString path = QString(":/data/%1.dds").arg(fileName);
    QString sourcePath = QString(":/data/%1.png").arg(fileName);
    QImageReader reader(path);
    QVERIFY(reader.canRead());
    QImage image = reader.read();
    QVERIFY(!image.isNull());
    QCOMPARE(image.size(), size);
    QVERIFY(compareImages(image, QImage(sourcePath)) == true);
}

void DDSTest::testMipmaps_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QSize>("size");
    QTest::addColumn<int>("imageCount");

    QTest::newRow("1") << QString("mipmaps") << QSize(64, 64) << 7;
}

void DDSTest::testMipmaps()
{
    QFETCH(QString, fileName);
    QFETCH(QSize, size);
    QFETCH(int, imageCount);

    QString path = QString(":/data/%1.dds").arg(fileName);
    QImageReader reader(path);
    QVERIFY(reader.canRead());
    QCOMPARE(reader.imageCount(), imageCount);

    for (int i = 0; i < reader.imageCount(); ++i) {
        reader.jumpToImage(i);
        QImage image = reader.read();
        QCOMPARE(image.size(), size / (1 << i));
        QVERIFY(!image.isNull());
        QString sourcePath = QString(":/data/%1 %2.png").arg(fileName).arg(i);
        QVERIFY(compareImages(image, QImage(sourcePath)) == true);
    }
}

QTEST_MAIN(DDSTest)
#include "tst_dds.moc"
