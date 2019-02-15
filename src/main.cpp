#include "dialogs/welcomedialog.h"
#include "mainwindow.h"
#include <QApplication>
#include <QtGui>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>
#include <services/metricsservice.h>
#include <widgets/logwidget.h>
#include <services/databaseservice.h>
#include "libraries/singleapplication/singleapplication.h"
#include "version.h"
#include "release.h"
#include <utils/misc.h>
#include <utils/schema.h>

// define the base class for SingleApplication
#define QAPPLICATION_CLASS QApplication

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
    translator6.load("../share/QOwnNotes/languages/QOwnNotes_" + locale); \
    app.installTranslator(&translator6); \
    translator7.load(appPath + "/../share/QOwnNotes/languages/QOwnNotes_" + locale); \
    app.installTranslator(&translator7); \
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
bool mainStartupMisc(const QStringList &arguments) {
    QCommandLineParser parser;
    parser.setApplicationDescription("QOwnNotes " + QString(VERSION));
    const QCommandLineOption helpOption = parser.addHelpOption();
    const QCommandLineOption portableOption(
            "portable", QCoreApplication::translate("main", "Runs the "
                    "application in portable mode."));
    parser.addOption(portableOption);
    const QCommandLineOption dumpSettingsOption(
            "dump-settings", QCoreApplication::translate("main", "Prints out "
                    "a dump of the settings and other information about the "
                    "application and environment in GitHub Markdown and exits "
                    "the application."));
    parser.addOption(dumpSettingsOption);
    const QCommandLineOption allowMultipleInstancesOption(
            "allow-multiple-instances", QCoreApplication::translate("main",
                    "Allows multiple instances of QOwnNotes to be started "
                    "even if disallowed in the settings."));
    parser.addOption(allowMultipleInstancesOption);
    const QCommandLineOption clearSettingsOption(
            "clear-settings", QCoreApplication::translate("main", "Clears the "
                    "settings and runs the application."));
    parser.addOption(clearSettingsOption);
    const QCommandLineOption sessionOption(
            "session", QCoreApplication::translate("main", "Runs the "
                    "application in a different context for settings and "
                    "internal files."), "name");
    parser.addOption(sessionOption);

    // just parse the arguments, we want no error handling
    parser.parse(arguments);

    // show the help page if the help parameter was provided
    if (parser.isSet(helpOption)) {
        parser.showHelp();
    }

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

    if (parser.isSet(dumpSettingsOption)) {
        fprintf(stdout, "%s\n",
                Utils::Misc::generateDebugInformation().toLocal8Bit().constData());
        exit(0);
    }

    return true;
}

/**
 * Shows the command line help
 */
//void showHelp() {
//    qWarning() << "\nQOwnNotes " << VERSION << "\n";
//    qWarning() << QObject::tr("Application Options") << ":";
//    qWarning() << "  --portable          " <<
//                  QObject::tr("Runs the application in portable mode");
//    qWarning("  --clear-settings    " +
//                     QObject::tr("Clears the settings and runs "
//                                         "the application").toUtf8());
//    qWarning()  <<  QCoreApplication::translate("main", "Copy all source "
//            "files into <directory>.");
//}

/**
 * Temporary log output until LogWidget::logMessageOutput takes over
 */
void tempLogMessageOutput(
        QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();

    switch (type) {
        case QtDebugMsg:
            // only print debug messages in debug-mode, because we don't know
            // yet if the user wants to see them
#ifdef QT_DEBUG
            fprintf(stderr, "Debug: %s\n", localMsg.constData());
#endif
            break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
        case QtInfoMsg:
            fprintf(stderr, "Info: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
            break;
#endif
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(),
                    context.file, context.line, context.function);
    }
}

int main(int argc, char *argv[]) {
    // register NoteHistoryItem so we can store it to the settings
    // we need to do that before we are accessing QSettings or the
    // NoteHistoryItem instances in the settings will get destroyed
    qRegisterMetaTypeStreamOperators<NoteHistoryItem>("NoteHistoryItem");
    qRegisterMetaType<NoteHistoryItem>("NoteHistoryItem");

    // temporary log output until LogWidget::logMessageOutput takes over
    qInstallMessageHandler(tempLogMessageOutput);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QString release = RELEASE;
    bool portable = false;
    bool clearSettings = false;
    bool snap = false;
    bool allowOnlyOneAppInstance = true;
    QStringList arguments;
    QString appNameAdd = "";

#ifdef QT_DEBUG
    appNameAdd = "Debug";
#endif

    for (int i = 0; i < argc; ++i) {
        QString arg(argv[i]);
        arguments << arg;

        if (arg == "--snap") {
            // override the release string for snaps
            release = "Snapcraft";
            snap = true;
        } else if (arg == "--portable") {
            portable = true;
        } else if (arg == "--clear-settings") {
            clearSettings = true;
        } else if (arg == "--help" || arg == "--dump-settings" ||
            arg == "-h" || arg == "--allow-multiple-instances") {
            allowOnlyOneAppInstance = false;
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
        } else if (arg == "--session") {
            // check if there is a 2nd parameter with the session name
            if (argc > (i+1)) {
                appNameAdd += "-" + QString(argv[i+1]).trimmed();
            }
        }
    }

    qDebug() << __func__ << " - 'arguments': " << arguments;

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

    // clear the settings if a --clear-settings parameter was provided
    if (clearSettings) {
        QSettings settings;
        settings.clear();
        qWarning("Your settings are now cleared!");
    }

    QSettings settings;
    QString locale = settings.value("interfaceLanguage").toString();

    if (locale.isEmpty()) {
        locale = QLocale::system().name().section('_', 0, 0);
    }
    qDebug() << __func__ << " - 'locale': " << locale;

    // setup default schema settings
    Utils::Schema::schemaSettings = new Utils::Schema::Settings();

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
    QTranslator translator6;
    QTranslator translator7;
    QTranslator translatorLocal;

#ifdef Q_OS_MAC
    // we don't need this on macOS
    allowOnlyOneAppInstance = false;
#else
    // if allowOnlyOneAppInstance still has the default true let's ask the
    // settings
    if (allowOnlyOneAppInstance) {
        allowOnlyOneAppInstance = settings.value(
                "allowOnlyOneAppInstance", true).toBool();
    }
#endif

    if ( allowOnlyOneAppInstance && !SingleApplication::isSupported() ) {
        allowOnlyOneAppInstance = false;
        settings.setValue("allowOnlyOneAppInstance", false);

        qWarning() << QCoreApplication::translate(
                "main", "Single application mode is not supported on your "
                        "system!");
    }

    // if only one app instance is allowed use SingleApplication
    if (allowOnlyOneAppInstance) {
        SingleApplication app(argc, argv, false, SingleApplication::Mode::User,
                1000, []() {
            qWarning() << QCoreApplication::translate("main",
                       "Another instance of QOwnNotes was already started! "
                       "You can turn off the single instance mode in the settings"
                       " or use the parameter --allow-multiple-instances.");
        });
        app.setProperty("release", release);
        app.setProperty("portable", portable);
        app.setProperty("singleApplication", true);
        app.setProperty("snap", snap);
        app.setProperty("arguments", arguments);

#ifndef QT_DEBUG
        LOAD_RELEASE_TRANSLATIONS(app)
#endif

        LOAD_TRANSLATIONS(app)

#ifdef Q_OS_MAC
        LOAD_MAC_TRANSLATIONS(app)
#endif

        bool result = mainStartupMisc(arguments);
        if (!result) {
            return 0;
        }

        MainWindow w;
        w.show();

        // raise the main window if app was started a 2nd time in single
        // application mode
        QObject::connect(&app, &SingleApplication::instanceStarted, [&] {
            qWarning() << QCoreApplication::translate("main",
                        "A second instance of QOwnNotes was attempted to be "
                        "started!");

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
        app.setProperty("arguments", arguments);

#ifndef QT_DEBUG
        LOAD_RELEASE_TRANSLATIONS(app)
#endif

        LOAD_TRANSLATIONS(app)

#ifdef Q_OS_MAC
        LOAD_MAC_TRANSLATIONS(app)
#endif

        bool result = mainStartupMisc(arguments);
        if (!result) {
            return 0;
        }

        MainWindow w;
        w.show();

        return app.exec();
    }
}
