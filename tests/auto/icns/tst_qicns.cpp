#include <QtTest/QtTest>
#include <QtGui/QtGui>

class tst_qicns: public QObject
{
    Q_OBJECT

private slots:
    void readIcons_data();
    void readIcons();
};

void tst_qicns::readIcons_data()
{
    QTest::addColumn<QString>("fileName");
    QTest::addColumn<QSize>("size");
    QTest::addColumn<int>("imageCount");

    QTest::newRow("1") << QStringLiteral("test-png") << QSize(128, 128) << 6;
    QTest::newRow("2") << QStringLiteral("test-jp2") << QSize(128, 128) << 6;
    QTest::newRow("3") << QStringLiteral("test-32bit") << QSize(128, 128) << 4;
    QTest::newRow("4") << QStringLiteral("test-legacy") << QSize(48, 48) << 12;
}

void tst_qicns::readIcons()
{
    QFETCH(QString, fileName);
    QFETCH(QSize, size);
    QFETCH(int, imageCount);

    const QString path = QStringLiteral(":/data/") + fileName + QStringLiteral(".icns");
    QImageReader reader(path);
    QVERIFY(reader.canRead());
    QCOMPARE(reader.imageCount(), imageCount);

    for (int i = 0; i < reader.imageCount(); ++i) {
        QVERIFY2(reader.jumpToImage(i), qPrintable(reader.errorString()));
        QImage image = reader.read();
        if (i == 0)
            QCOMPARE(image.size(), size);
        QVERIFY2(!image.isNull(), qPrintable(reader.errorString()));
    }
}

QTEST_MAIN(tst_qicns)
#include "tst_qicns.moc"
