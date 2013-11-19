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
    QTest::addColumn<int>("iconN");

    QTest::newRow("1") << 1;
    QTest::newRow("2") << 2;
    QTest::newRow("3") << 3;
    QTest::newRow("4") << 4;
    QTest::newRow("5") << 5;
    QTest::newRow("6") << 6;
    QTest::newRow("7") << 7;
    QTest::newRow("8") << 8;
    QTest::newRow("9") << 9;
    QTest::newRow("10") << 10;
    QTest::newRow("11") << 11;
    QTest::newRow("12") << 12;
}

void tst_qicns::readIcons()
{
    const QString path = QFINDTESTDATA(":/data/andromeda-nojp2.icns");
    QImageReader reader(path);
    QVERIFY(reader.canRead());
    QFETCH(int, iconN);
    QVERIFY2(reader.jumpToImage(iconN), qPrintable(reader.errorString()));
    QVERIFY2(!reader.read().isNull(), qPrintable(reader.errorString()));
}

QTEST_MAIN(tst_qicns)
#include "tst_qicns.moc"
