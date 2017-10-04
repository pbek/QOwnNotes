#include "dialogs/welcomedialog.h"
#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QSettings>
#include <QTranslator>
#include <services/metricsservice.h>
#include <widgets/logwidget.h>
#include <services/databaseservice.h>
#include <utils/misc.h>
#include <QStyleFactory>
#include "libraries/singleapplication/singleapplication.h"
#include "version.h"
#include "release.h"

/**
 * Macro for loading the translations
 */
#define LOAD_TRANSLATIONS(app) \
    qtTranslator.load("qt_" + QLocale::system().name(), \
    QLibraryInfo::location(QLibraryInfo::TranslationsPath)); \
    app.installTranslator(&qtTranslator); \
    qtTranslator2.load("qt_" + locale, \
    QLibraryInfo::location(QLibraryInfo::TranslationsPath)); \
    app.installTranslator(&qtTranslator2); \
    QString appPath = QCoreApplication::applicationDirPath(); \
    qtTranslator3.load("qt_" + locale, appPath + "/translations"); \
    app.installTranslator(&qtTranslator3); \
    translator1.load(appPath + "/../src/languages/QOwnNotes_" + locale); \
    app.installTranslator(&translator1); \
    translator2.load(appPath + "/../languages/QOwnNotes_" + locale); \
    app.installTranslator(&translator2); \
    translator3.load(appPath + "/languages/QOwnNotes_" + locale); \
    app.installTranslator(&translator3); \
    translator4.load(appPath + "/QOwnNotes_" + locale); \
    app.installTranslator(&translator4); \
    translator5.load("../src/languages/QOwnNotes_" + locale); \
    app.installTranslator(&translator5); \
    translatorLocal.load("QOwnNotes_" + locale); \
    app.installTranslator(&translatorLocal);

/**
 * Macro for loading the release translations
 */
#define LOAD_RELEASE_TRANSLATIONS(app) \
    translatorRelease.load( \
    "/usr/share/QOwnNotes/languages/QOwnNotes_" + locale); \
    app.installTranslator(&translatorRelease);

/**
 * Macro for loading the translations on OS X
 */
#define LOAD_MAC_TRANSLATIONS(app) \
    translatorOSX.load(appPath + "/../Resources/QOwnNotes_" + locale); \
    app.installTranslator(&translatorOSX); \
    translatorOSX2.load("../Resources/QOwnNotes_" + locale); \
    app.installTranslator(&translatorOSX2);


/**
 * Does the miscellaneous startup
 * If false is returned the app is supposed to quit
 */
bool mainStartupMisc() {
    QSettings settings;
    QString interfaceStyle = settings.value("interfaceStyle").toString();

    // restore the interface style
    if (!interfaceStyle.isEmpty()) {
        QApplication::setStyle(interfaceStyle);
    }

    bool internalIconTheme = settings.value("internalIconTheme").toBool();

    if (QIcon::themeName() == "" || internalIconTheme) {
        QIcon::setThemeName("breeze-qownnotes");
    }

    bool darkMode = settings.value("darkMode").toBool();
    if (darkMode) {
        QIcon::setThemeName("breeze-dark-qownnotes");
    }

    MetricsService *metricsService = MetricsService::createInstance();
    metricsService->sendVisitIfEnabled("app/start", "App Start");
    metricsService->sendEventIfEnabled(
            "app/qt-version-build", "app", "qt version build",
            QString(QT_VERSION_STR));
    metricsService->sendEventIfEnabled(
            "app/qt-version-runtime", "app", "qt version runtime",
            qVersion());
    metricsService->sendEventIfEnabled(
            "app/theme", "app", "theme", QIcon::themeName());
    metricsService->sendEventIfEnabled(
            "app/release", "app", "release",
            qApp->property("release").toString());
    metricsService->sendEventIfEnabled(
            "app/portable", "app", "portable",
            Utils::Misc::isInPortableMode() ? "yes" : "no");

    if (qApp->property("snap").toBool()) {
        metricsService->sendEventIfEnabled(
                "app/styles", "app", "styles",
                QStyleFactory::keys().join(" "));
    }

    QString productType;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    productType = QSysInfo::productType();
#else
    productType += " Qt " + QString(QT_VERSION_STR);
#endif

    metricsService->sendEventIfEnabled(
            "app/product-type", "app", "product-type", productType);

    QString platform = "other";
#ifdef Q_OS_LINUX
    platform = "linux";
#endif
#ifdef Q_OS_MAC
    platform = "mac";
#endif
#ifdef Q_OS_WIN
    platform = "windows";
#endif

    // disable the automatic update dialog per default for repositories and
    // self-builds if nothing is already set
    Utils::Misc::presetDisableAutomaticUpdateDialog();

    metricsService->sendEventIfEnabled(
            "app/platform", "app", "platform", platform);

    // sends locale information
    metricsService->sendLocaleEvent();

    // check legacy setting
    QString notesPath = settings.value("General/notesPath").toString();

    // migration: remove old setting if we found one and store new one
    if (!notesPath.isEmpty()) {
        settings.setValue("notesPath", notesPath);
        settings.remove("General/notesPath");
    } else {
        // load the notes path
        notesPath = settings.value("notesPath").toString();
    }

    if (!notesPath.isEmpty()) {
        // prepend the portable data path if we are in portable mode
        notesPath = Utils::Misc::prependPortableDataPathIfNeeded(notesPath);
    }

    QDir dir(notesPath);

    // if the notes path is empty or doesn't exist open the welcome dialog
    if (notesPath.isEmpty() || !dir.exists()) {
        WelcomeDialog welcomeDialog;
        // exit QOwnNotes if the welcome dialog was canceled
        if (welcomeDialog.exec() != QDialog::Accepted) {
            return false;
        }
    }

    DatabaseService::createConnection();
    DatabaseService::setupTables();

    // try to create note folders if they are missing
    NoteFolder::migrateToNoteFolders();

    return true;
}

int main(int argc, char *argv[]) {
    // register NoteHistoryItem so we can store it to the settings
    // we need to do that before we are accessing QSettings or the
    // NoteHistoryItem instances in the settings will get destroyed
    qRegisterMetaTypeStreamOperators<NoteHistoryItem>("NoteHistoryItem");
    qRegisterMetaType<NoteHistoryItem>("NoteHistoryItem");

    QString release = RELEASE;
    bool portable = false;
    bool snap = false;

    for (int i = 0; i < argc; ++i) {
        QString arg(argv[i]);
        if (arg == "--snap") {
            // override the release string for snaps
            release = "Snapcraft";
            snap = true;
        } else if (arg == "--portable") {
            portable = true;
        } else if (arg == "--after-update") {
            qWarning() << __func__ << " - 'arg': " << arg;
#if not defined(Q_OS_WIN)
            // check if there is a 2nd parameter with the script path
            if (argc > (i+1)) {
                QString scriptPath(argv[i+1]);

                // remove the updater script file
                QFile file(scriptPath);
                file.remove();
            }
#endif
        }
    }

    // TODO(pbek): remove
//    portable = true;

    // don't log SSL warnings in releases on OS X
#if defined(QT_NO_DEBUG) && defined(Q_OS_MAC)
    qputenv("QT_LOGGING_RULES", "qt.network.ssl.warning=false");
#endif

    // fixing some troubles in Windows 8.1
#ifdef Q_OS_WIN32
    QCoreApplication::addLibraryPath("./");
#endif

    QString appNameAdd = "";

#ifdef QT_DEBUG
    appNameAdd = "Debug";
#endif

    QCoreApplication::setOrganizationDomain("PBE");
    QCoreApplication::setOrganizationName("PBE");
    QCoreApplication::setApplicationName("QOwnNotes" + appNameAdd);

    QString appVersion = QString(VERSION);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    appVersion += " " + QSysInfo::prettyProductName();

    if (!appVersion.contains(QSysInfo::currentCpuArchitecture())) {
        appVersion += " " + QSysInfo::currentCpuArchitecture();
    }
#else
    appVersion += " Qt " + QString(QT_VERSION_STR);
#endif

    appVersion += " " + QString(release);

#ifdef QT_DEBUG
    appVersion += " Debug";
#endif

    QCoreApplication::setApplicationVersion(appVersion);

    // set the settings format to ini format and the settings path inside the
    // path of the application in portable mode
    if (portable) {
        QSettings::setDefaultFormat(QSettings::IniFormat);
        QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                           Utils::Misc::portableDataPath());
        QSettings settings;
        qDebug() << "settings fileName: " << settings.fileName();
    }

    QSettings settings;
    QString locale = settings.value("interfaceLanguage").toString();

    if (locale.isEmpty()) {
        locale = QLocale::system().name().section('_', 0, 0);
    }
    qDebug() << __func__ << " - 'locale': " << locale;

#ifndef QT_DEBUG
    QTranslator translatorRelease;
#endif

#ifdef Q_OS_MAC
    QTranslator translatorOSX;
    QTranslator translatorOSX2;
#endif

    QTranslator qtTranslator;
    QTranslator qtTranslator2;
    QTranslator qtTranslator3;
    QTranslator translator1;
    QTranslator translator2;
    QTranslator translator3;
    QTranslator translator4;
    QTranslator translator5;
    QTranslator translatorLocal;

    bool allowOnlyOneAppInstance = settings.value(
            "allowOnlyOneAppInstance", false).toBool();

    // if only one app instance is allowed use SingleApplication
    if (allowOnlyOneAppInstance) {
        SingleApplication app(argc, argv);
        app.setProperty("release", release);
        app.setProperty("portable", portable);
        app.setProperty("singleApplication", true);
        app.setProperty("snap", snap);

#ifndef QT_DEBUG
        LOAD_RELEASE_TRANSLATIONS(app)
#endif

        LOAD_TRANSLATIONS(app)

#ifdef Q_OS_MAC
        LOAD_MAC_TRANSLATIONS(app)
#endif

        bool result = mainStartupMisc();
        if (!result) {
            return 0;
        }

        MainWindow w;
        w.show();

        // raise the main window if app was started a 2nd time in single
        // application mode
        QObject::connect(&app, &SingleApplication::showUp, [&] {
            w.show();
            w.raise();
            w.activateWindow();

            // in case the window was minimized show it normal again
            // (it didn't came up when it was minimized on KDE)
            if (w.isMinimized()) {
                w.showNormal();
            }
        });

        return app.exec();
    } else {
        // use a normal QApplication if multiple instances of the app are
        // allowed
        QApplication app(argc, argv);
        app.setProperty("release", release);
        app.setProperty("portable", portable);
        app.setProperty("snap", snap);

#ifndef QT_DEBUG
        LOAD_RELEASE_TRANSLATIONS(app)
#endif

        LOAD_TRANSLATIONS(app)

#ifdef Q_OS_MAC
        LOAD_MAC_TRANSLATIONS(app)
#endif

        bool result = mainStartupMisc();
        if (!result) {
            return 0;
        }

        MainWindow w;
        w.show();

        return app.exec();
    }
}
