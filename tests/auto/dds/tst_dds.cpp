#include <QtTest/QtTest>
#include <QtGui/QtGui>

class DDSTest: public QObject
{
    Q_OBJECT

private slots:
    void readImage_data();
    void readImage();
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

    QTest::newRow("1") << QString("DXT1") << QSize(64, 64);
    QTest::newRow("2") << QString("DXT2") << QSize(64, 64);
    QTest::newRow("3") << QString("DXT3") << QSize(64, 64);
    QTest::newRow("4") << QString("DXT4") << QSize(64, 64);
    QTest::newRow("5") << QString("DXT5") << QSize(64, 64);
    QTest::newRow("6") << QString("RGBA4444") << QSize(64, 64);
    QTest::newRow("7") << QString("RGBA8") << QSize(64, 64);
    QTest::newRow("8") << QString("RGB565") << QSize(64, 64);
    QTest::newRow("9") << QString("RGBA5551") << QSize(64, 64);
    QTest::newRow("10") << QString("RGB10A2") << QSize(64, 64);
    QTest::newRow("11") << QString("L8") << QSize(64, 64);
    QTest::newRow("12") << QString("L8A8") << QSize(64, 64);
    QTest::newRow("13") << QString("RGB8") << QSize(64, 64);
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

QTEST_MAIN(DDSTest)
#include "tst_dds.moc"
