#include "dialogs/welcomedialog.h"
#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QSettings>
#include <QTranslator>
#include <services/metricsservice.h>
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
    QString appPath = QCoreApplication::applicationDirPath(); \
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

    if (QIcon::themeName() == "") {
        QIcon::setThemeName("breeze-qownnotes");
    }

    MetricsService *metricsService = MetricsService::createInstance();
    metricsService->sendVisitIfEnabled("app/start", "App Start");

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

    QDir dir(notesPath);

    // if the notes path is empty or doesn't exist open the welcome dialog
    if (notesPath.isEmpty() || !dir.exists()) {
        WelcomeDialog welcomeDialog;
        // exit QOwnNotes if the welcome dialog was canceled
        if (welcomeDialog.exec() != QDialog::Accepted) {
            return false;
        }
    }

    return true;
}

int main(int argc, char *argv[]) {
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

    appVersion += " " + QString(RELEASE);

#ifdef QT_DEBUG
    appVersion += " Debug";
#endif

    QCoreApplication::setApplicationVersion(appVersion);

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
