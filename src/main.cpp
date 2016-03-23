#include "dialogs/welcomedialog.h"
#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QSettings>
#include <QTranslator>
#include <services/metricsservice.h>
#include "version.h"
#include "release.h"


int main(int argc, char *argv[])
{
    // fixing some troubles in Windows 8.1
#ifdef Q_OS_WIN32
    QCoreApplication::addLibraryPath("./");
#endif

    QApplication app(argc, argv);
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

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    QSettings settings;
    QString locale = settings.value("interfaceLanguage").toString();

    if (locale.isEmpty()) {
        locale = QLocale::system().name().section('_', 0, 0);
    }
    qDebug() << __func__ << " - 'locale': " << locale;

#ifndef QT_DEBUG
    QTranslator translatorRelease;
    translatorRelease.load(
        "/usr/share/QOwnNotes/languages/QOwnNotes_" + locale);
    app.installTranslator(&translatorRelease);
#endif

    QString appPath = QCoreApplication::applicationDirPath();

    QTranslator translator1;
    translator1.load(appPath + "/../src/languages/QOwnNotes_" + locale);
    app.installTranslator(&translator1);

    QTranslator translator2;
    translator2.load(appPath + "/../languages/QOwnNotes_" + locale);
    app.installTranslator(&translator2);

    QTranslator translator3;
    translator3.load(appPath + "/languages/QOwnNotes_" + locale);
    app.installTranslator(&translator3);

    QTranslator translator4;
    translator4.load(appPath + "/QOwnNotes_" + locale);
    app.installTranslator(&translator4);

    QTranslator translator5;
    translator5.load("../src/languages/QOwnNotes_" + locale);
    app.installTranslator(&translator5);

    QTranslator translatorLocal;
    translatorLocal.load("QOwnNotes_" + locale);
    app.installTranslator(&translatorLocal);

#ifdef Q_OS_MAC
    QTranslator translatorOSX;
    translatorOSX.load(appPath + "/../Resources/QOwnNotes_" + locale);
    app.installTranslator(&translatorOSX);

    QTranslator translatorOSX2;
    translatorOSX2.load("../Resources/QOwnNotes_" + locale);
    app.installTranslator(&translatorOSX2);
#endif

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
        notesPath = settings.value("notesPath").toString();
    }

    // for developing
    notesPath = "";

    // if the notes path is empty open the welcome dialog
    if (notesPath.isEmpty()) {
        WelcomeDialog welcomeDialog;
        // exit QOwnNotes if the welcome dialog was canceled
        if (welcomeDialog.exec() != 1) {
            return 0;
        }
    }

    MainWindow w;
    w.show();

    return app.exec();
}
