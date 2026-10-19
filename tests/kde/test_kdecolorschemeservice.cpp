#include <services/kdecolorschemeservice.h>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QPalette>
#include <QTemporaryDir>
#include <QtTest>

namespace {
const char schemePathProperty[] = "KDE_COLOR_SCHEME_PATH";

QString dataHome(const QTemporaryDir &dir) { return dir.filePath(QStringLiteral("home")); }
QString dataDir(const QTemporaryDir &dir) { return dir.filePath(QStringLiteral("system")); }

void writeScheme(const QString &dataPath, const QString &id, const QString &name,
                 const QString &windowColor) {
    QDir().mkpath(dataPath + QStringLiteral("/color-schemes"));
    QFile file(dataPath + QStringLiteral("/color-schemes/") + id + QStringLiteral(".colors"));
    QVERIFY(file.open(QIODevice::WriteOnly | QIODevice::Text));
    file.write(QStringLiteral("[General]\nName=%1\n\n"
                              "[Colors:Window]\nBackgroundNormal=%2\n\n"
                              "[Colors:View]\nBackgroundNormal=%2\n")
                   .arg(name, windowColor)
                   .toUtf8());
}
}    // namespace

class TestKdeColorSchemes : public QObject {
    Q_OBJECT

   public:
    explicit TestKdeColorSchemes(const QTemporaryDir &dir) : _dir(dir) {}

   private:
    const QTemporaryDir &_dir;
    QColor _systemWindowColor;

   private slots:
    void initTestCase() {
        writeScheme(dataDir(_dir), QStringLiteral("Zebra"), QStringLiteral("Zebra"),
                    QStringLiteral("1,2,3"));
        writeScheme(dataDir(_dir), QStringLiteral("TestDark"), QStringLiteral("System dark"),
                    QStringLiteral("4,5,6"));
        // The user's scheme overrides the system scheme with the same file name
        writeScheme(dataHome(_dir), QStringLiteral("TestDark"), QStringLiteral("Test dark"),
                    QStringLiteral("10,20,30"));
        _systemWindowColor = QApplication::palette().color(QPalette::Window);
    }

    void listsSchemes() {
        const QList<KdeColorSchemeService::Scheme> schemes =
            KdeColorSchemeService::availableSchemes();
        QCOMPARE(schemes.size(), 2);
        // Sorted by name
        QCOMPARE(schemes.at(0).id, QStringLiteral("TestDark"));
        QCOMPARE(schemes.at(0).name, QStringLiteral("Test dark"));
        QVERIFY(schemes.at(0).path.startsWith(dataHome(_dir)));
        QCOMPARE(schemes.at(1).id, QStringLiteral("Zebra"));
    }

    void keepsSystemPaletteWithoutScheme() {
        QVERIFY(!KdeColorSchemeService::apply(QString(), false));
        QVERIFY(!KdeColorSchemeService::apply(QStringLiteral("Missing"), false));
        QCOMPARE(QApplication::palette().color(QPalette::Window), _systemWindowColor);
        QVERIFY(!qApp->property(schemePathProperty).isValid());
        QVERIFY(KdeColorSchemeService::activeSchemeId().isEmpty());
    }

    void appliesAndRestoresScheme() {
        // Set like a platform theme would do, it needs to be restored afterwards
        qApp->setProperty(schemePathProperty, QStringLiteral("/platform/scheme.colors"));

        QVERIFY(KdeColorSchemeService::apply(QStringLiteral("TestDark"), false));
        QCOMPARE(QApplication::palette().color(QPalette::Window), QColor(10, 20, 30));
        QCOMPARE(QApplication::palette().color(QPalette::Base), QColor(10, 20, 30));
        QVERIFY(qApp->property(schemePathProperty).toString().startsWith(dataHome(_dir)));
        QCOMPARE(KdeColorSchemeService::activeSchemeId(), QStringLiteral("TestDark"));

        QVERIFY(KdeColorSchemeService::apply(QStringLiteral("Zebra"), false));
        QCOMPARE(QApplication::palette().color(QPalette::Window), QColor(1, 2, 3));

        QVERIFY(!KdeColorSchemeService::apply(QString(), false));
        QCOMPARE(QApplication::palette().color(QPalette::Window), _systemWindowColor);
        QCOMPARE(qApp->property(schemePathProperty).toString(),
                 QStringLiteral("/platform/scheme.colors"));
        QVERIFY(KdeColorSchemeService::activeSchemeId().isEmpty());
        qApp->setProperty(schemePathProperty, QVariant());
    }

    void darkModeThemeTakesPrecedence() {
        QVERIFY(KdeColorSchemeService::apply(QStringLiteral("TestDark"), false));
        QVERIFY(!KdeColorSchemeService::apply(QStringLiteral("TestDark"), true));
        QCOMPARE(QApplication::palette().color(QPalette::Window), _systemWindowColor);
        QVERIFY(!qApp->property(schemePathProperty).isValid());
    }
};

int main(int argc, char *argv[]) {
    // Keep the user's color schemes out of the tests
    QTemporaryDir dir;
    if (!dir.isValid()) {
        return 1;
    }
    qputenv("XDG_DATA_HOME", QFile::encodeName(dataHome(dir)));
    qputenv("XDG_DATA_DIRS", QFile::encodeName(dataDir(dir)));
    qputenv("XDG_CONFIG_HOME", QFile::encodeName(dir.filePath(QStringLiteral("config"))));
    QApplication app(argc, argv);
    TestKdeColorSchemes tests(dir);
    return QTest::qExec(&tests, argc, argv);
}

#include "test_kdecolorschemeservice.moc"
