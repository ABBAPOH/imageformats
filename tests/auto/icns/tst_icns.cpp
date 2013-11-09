#include <QtTest/QtTest>
#include <QtGui/QtGui>

class IcnsTest: public QObject
{
    Q_OBJECT

private slots:
    void readIcons();
};

/*static bool compareImages(const QImage &first, const QImage &second)
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
}*/

void IcnsTest::readIcons()
{
    QString input = QFINDTESTDATA("data/test.icns");
    QImageReader reader(input);
    QVERIFY2(reader.canRead(), "Unable to read the file.");
    QVERIFY2(reader.imageCount() > 0, "File does not contain icons or corrupted.");
    for(int i = 0; i < reader.imageCount(); i++) {
        QVERIFY2(reader.jumpToImage(i), "Unable to jump to a next image.");
        QImage icon = reader.read();
        bool readOK = !icon.isNull();
        QVERIFY2(readOK, "Icon could not be read.");
        if(readOK) {
            QFile file(QString("data/test_output%1.icns").arg(i));
            bool fopenOK = file.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QVERIFY2(fopenOK, "Unable to open the file for writing.");
            if(fopenOK) {
                QImageWriter writer(&file, "icns");
                QVERIFY2(writer.canWrite(), "Unable to write to the file.");
                QVERIFY2(writer.write(icon), "Error during writing to file.");
                file.close();
            }
        }
    }
}

QTEST_MAIN(IcnsTest)
#include "tst_icns.moc"
