#include <services/databaseservice.h>
#include <services/metricsservice.h>
#include <utils/cli.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include <utils/schema.h>

#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QStyleFactory>
#include <QTranslator>
#include <QtGui>
#include <iostream>

#include "dialogs/welcomedialog.h"
#include "entities/notefolder.h"
#include "helpers/nomenuiconstyle.h"
#include "libraries/singleapplication/singleapplication.h"
#include "mainwindow.h"
#include "release.h"
#include "version.h"

// define the base class for SingleApplication
#define QAPPLICATION_CLASS QApplication

/**
 * Function for loading a translation with debug output
 */
void loadTranslation(QTranslator &translator, const QString &fileName,
                     const QString &directory = QString()) {
    bool isLoaded = translator.load(fileName, directory);
    bool isInstalled = QCoreApplication::installTranslator(&translator);

    qDebug() << "Translation " << fileName << "in" << directory << "isLoaded:" << isLoaded
             << ", isInstalled:" << isInstalled;
}

/**
 * Function for loading the translations
 */
void loadTranslations(QTranslator *translator, const QString &locale) {
    //    loadTranslation(translator[0], "qt_" + QLocale::system().name(),
    //                       QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    loadTranslation(translator[1], "qt_" + locale,
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                    QLibraryInfo::path(QLibraryInfo::TranslationsPath));
#else
                    QLibraryInfo::location(QLibraryInfo::TranslationsPath));
#endif
    QString appPath = QCoreApplication::applicationDirPath();
    loadTranslation(translator[2], "qt_" + locale, appPath + "/translations");
    loadTranslation(translator[3], appPath + "/../src/languages/QOwnNotes_" + locale);
    loadTranslation(translator[4], appPath + "/../languages/QOwnNotes_" + locale);
    loadTranslation(translator[5], appPath + "/languages/QOwnNotes_" + locale);
    loadTranslation(translator[6], appPath + "/QOwnNotes_" + locale);
    loadTranslation(translator[7], "../src/languages/QOwnNotes_" + locale);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    loadTranslation(translator[8], "../share/qt6/translations/QOwnNotes_" + locale);
    loadTranslation(translator[9], appPath + "/../share/qt6/translations/QOwnNotes_" + locale);
#else
    loadTranslation(translator[8], "../share/qt5/translations/QOwnNotes_" + locale);
    loadTranslation(translator[9], appPath + "/../share/qt5/translations/QOwnNotes_" + locale);
#endif
    loadTranslation(translator[10], "QOwnNotes_" + locale);
    loadTranslation(translator[11], "../share/QOwnNotes/translations/QOwnNotes_" + locale);
    loadTranslation(translator[12],
                    appPath + "/../share/QOwnNotes/translations/QOwnNotes_" + locale);
}

/**
 * Function for loading the release translations
 */
inline void loadReleaseTranslations(QTranslator *translatorsRelease, const QString &locale) {
    // The qt5/qt6 paths qre needed by the Fedora and openSUSE builds on OBS
    loadTranslation(translatorsRelease[0],
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                    "/usr/share/qt6/translations/"
#else
                    "/usr/share/qt5/translations/"
#endif
                    "QOwnNotes_" +
                        locale);
    // Debian and Ubuntu don't work with qt5/qt6 paths with cmake and Qt6
    loadTranslation(translatorsRelease[1], "/usr/share/QOwnNotes/translations/QOwnNotes_" + locale);
}

/**
 * Function for loading the translations on OS X
 */
inline void loadMacTranslations(QTranslator &translatorOSX, QTranslator &translatorOSX2,
                                QTranslator &translatorOSX3, QTranslator &translatorOSX4,
                                const QString &appPath, const QString &locale) {
    loadTranslation(translatorOSX, appPath + "/../Resources/QOwnNotes_" + locale);
    loadTranslation(translatorOSX2, "../Resources/QOwnNotes_" + locale);
    loadTranslation(translatorOSX3, appPath + "/../Resources/qtbase_" + locale);
    loadTranslation(translatorOSX4, "../Resources/qtbase_" + locale);
}

/**
 * Does the miscellaneous startup
 * If false is returned the app is supposed to quit
 */
int mainStartupMisc(const QStringList &arguments) {
    QCommandLineParser parser;
    QList<QCommandLineOption> allOptions;

    parser.setApplicationDescription("QOwnNotes " + QString(VERSION));

    // We don't use parser.addHelpOption(), because it added a --help-all option that didn't work
    const QCommandLineOption helpOption(
        QStringList() << "h" << "help",
        QCoreApplication::translate("main", "Displays help on commandline options."));
    parser.addOption(helpOption);

    const QCommandLineOption portableOption(
        QStringLiteral("portable"), QCoreApplication::translate("main",
                                                                "Runs the "
                                                                "application in portable mode."));
    parser.addOption(portableOption);
    const QCommandLineOption dumpSettingsOption(
        QStringLiteral("dump-settings"),
        QCoreApplication::translate("main",
                                    "Prints out "
                                    "a dump of the settings and other information about the "
                                    "application and environment in GitHub Markdown and exits "
                                    "the application."));
    parser.addOption(dumpSettingsOption);
    const QCommandLineOption versionOption(
        QStringLiteral("version"),
        QCoreApplication::translate("main", "Prints out the version number."));
    parser.addOption(versionOption);
    const QCommandLineOption allowMultipleInstancesOption(
        QStringLiteral("allow-multiple-instances"),
        QCoreApplication::translate("main",
                                    "Allows multiple instances of QOwnNotes to be started "
                                    "even if disallowed in the settings."));
    parser.addOption(allowMultipleInstancesOption);
    const QCommandLineOption clearSettingsOption(
        QStringLiteral("clear-settings"),
        QCoreApplication::translate("main",
                                    "Clears the "
                                    "settings and runs the application."));
    parser.addOption(clearSettingsOption);
    const QCommandLineOption sessionOption(
        QStringLiteral("session"),
        QCoreApplication::translate("main",
                                    "Runs the "
                                    "application in a different context for settings and "
                                    "internal files."),
        "name");
    parser.addOption(sessionOption);
    const QCommandLineOption actionOption(
        QStringLiteral("action"),
        QCoreApplication::translate("main",
                                    "Triggers a menu action after the application was started."),
        "name");
    parser.addOption(actionOption);

    const QCommandLineOption completionOption(
        QStringList() << "completion",
        QCoreApplication::translate("main",
                                    "Generate shell completion code. Supports `fish`, `bash`."),
        "shell");
    parser.addOption(completionOption);

    allOptions << helpOption << portableOption << dumpSettingsOption << versionOption
               << allowMultipleInstancesOption << clearSettingsOption << sessionOption
               << actionOption << completionOption;

    // just parse the arguments, we want no error handling
    parser.parse(arguments);

    // show the help page if the help parameter was provided
    if (parser.isSet(helpOption)) {
        parser.showHelp();
    }

    // Check for completion option and generate script
    if (parser.isSet(completionOption)) {
        QString shell = parser.value(completionOption);
        if (shell == "fish") {
            Utils::Cli::generateFishCompletionScript(allOptions, QStringLiteral("QOwnNotes"));
        } else if (shell == "bash") {
            Utils::Cli::generateBashCompletionScript(allOptions, QStringLiteral("QOwnNotes"));
            //        } else if (shell == "zsh") {
            //            Utils::Cli::generateZshCompletionScript(allOptions,
            //            QStringLiteral("QOwnNotes"));
        } else {
            std::cerr << "Unsupported shell type specified." << std::endl;
            return 1;
        }

        return 0;    // Exit after generating the completion script
    }

    Utils::Gui::applyInterfaceStyle();

#ifdef Q_OS_WIN32
    Utils::Gui::doWindowsDarkModeCheck();
#endif

#ifdef Q_OS_LINUX
    Utils::Gui::doLinuxDarkModeCheck();
#endif

    QSettings settings;
    bool systemIconTheme = settings.value(QStringLiteral("systemIconTheme")).toBool();

    if (!systemIconTheme) {
        bool internalIconTheme = settings.value(QStringLiteral("internalIconTheme")).toBool();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
        if (!internalIconTheme && QIcon::themeName().isEmpty()) {
            QIcon::setThemeName(QIcon::fallbackThemeName());
        }
#endif

        if (QIcon::themeName().isEmpty() || internalIconTheme) {
            QIcon::setThemeName(QStringLiteral("breeze-qownnotes"));
        }

        if (Utils::Misc::isDarkModeIconTheme()) {
            QIcon::setThemeName(QStringLiteral("breeze-dark-qownnotes"));
        }
    }

    MetricsService *metricsService = MetricsService::createInstance();
    metricsService->sendVisitIfEnabled(QStringLiteral("app/start"), QStringLiteral("App Start"));
    metricsService->sendEventIfEnabled(QStringLiteral("app/qt-version-build"),
                                       QStringLiteral("app"), QStringLiteral("qt version build"),
                                       QStringLiteral(QT_VERSION_STR));
    metricsService->sendEventIfEnabled(QStringLiteral("app/qt-version-runtime"),
                                       QStringLiteral("app"), QStringLiteral("qt version runtime"),
                                       qVersion());
    metricsService->sendEventIfEnabled(QStringLiteral("app/theme"), QStringLiteral("app"),
                                       QStringLiteral("theme"), QIcon::themeName());
    metricsService->sendEventIfEnabled(QStringLiteral("app/release"), QStringLiteral("app"),
                                       QStringLiteral("release"),
                                       qApp->property("release").toString());
    metricsService->sendEventIfEnabled(
        QStringLiteral("app/portable"), QStringLiteral("app"), QStringLiteral("portable"),
        Utils::Misc::isInPortableMode() ? QStringLiteral("yes") : QStringLiteral("no"));

    if (qApp->property("snap").toBool()) {
        metricsService->sendEventIfEnabled(QStringLiteral("app/styles"), QStringLiteral("app"),
                                           QStringLiteral("styles"),
                                           QStyleFactory::keys().join(QChar(' ')));
    }

    QString productType;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    productType = QSysInfo::productType();
#else
    productType += " Qt " + QString(QT_VERSION_STR);
#endif

    metricsService->sendEventIfEnabled(QStringLiteral("app/product-type"), QStringLiteral("app"),
                                       QStringLiteral("product-type"), productType);

    QString platform = QStringLiteral("other");
#ifdef Q_OS_LINUX
    platform = QStringLiteral("linux");
#endif
#ifdef Q_OS_MAC
    platform = QStringLiteral("mac");
#endif
#ifdef Q_OS_WIN
    platform = QStringLiteral("windows");
#endif

    // disable the automatic update dialog per default for repositories and
    // self-builds if nothing is already set
    Utils::Misc::presetDisableAutomaticUpdateDialog();

    metricsService->sendEventIfEnabled(QStringLiteral("app/platform"), QStringLiteral("app"),
                                       QStringLiteral("platform"), platform);

    // sends locale information
    metricsService->sendLocaleEvent();

    // check legacy setting
    QString notesPath = settings.value(QStringLiteral("General/notesPath")).toString();

    // migration: remove old setting if we found one and store new one
    if (!notesPath.isEmpty()) {
        settings.setValue(QStringLiteral("notesPath"), notesPath);
        settings.remove(QStringLiteral("General/notesPath"));
    } else {
        // load the notes path
        notesPath = settings.value(QStringLiteral("notesPath")).toString();
    }

    if (!notesPath.isEmpty()) {
        // prepend the portable data path if we are in portable mode
        notesPath = Utils::Misc::prependPortableDataPathIfNeeded(notesPath);
    }

    DatabaseService::createConnection();
    DatabaseService::setupTables();
    QDir dir(notesPath);
    bool existingNotesPathNotFound = !notesPath.isEmpty() && !dir.exists();

    // If this isn't the first run and the note folder doesn't exist anymore look for another one
    if (existingNotesPathNotFound) {
        notesPath = QString();

        // Loop through all note folders and select the first existing one
        auto noteFolders = NoteFolder::fetchAll();
        for (const auto &noteFolder : noteFolders) {
            dir = QDir(noteFolder.getLocalPath());
            if (dir.exists()) {
                notesPath = noteFolder.getLocalPath();
                noteFolder.setAsCurrent();
                existingNotesPathNotFound = false;
                break;
            }
        }
    }

    // If there still was no existing note folder found let the user select another one
    if (existingNotesPathNotFound) {
        if (QMessageBox::question(nullptr, QObject::tr("Note folder not found!"),
                                  QObject::tr("Your note folder <b>%1</b> was not found any more! "
                                              "Do you want to select a new one?")
                                      .arg(notesPath)) != QMessageBox::Yes) {
            return 1;
        }

        notesPath = QFileDialog::getExistingDirectory(
            nullptr, QObject::tr("Please select the folder where your notes will get stored to"),
            notesPath, QFileDialog::ShowDirsOnly);

        dir = QDir(notesPath);

        if (notesPath.isEmpty() || !dir.exists()) {
            return 1;
        }

        settings.setValue(QStringLiteral("notesPath"), notesPath);

        // prepend the portable data path if we are in portable mode
        notesPath = Utils::Misc::prependPortableDataPathIfNeeded(notesPath);
        dir = QDir(notesPath);
    }

    // if the notes path is empty or doesn't exist open the welcome dialog
    if (notesPath.isEmpty() || !dir.exists()) {
        WelcomeDialog welcomeDialog;
        // exit QOwnNotes if the welcome dialog was canceled
        if (welcomeDialog.exec() != QDialog::Accepted) {
            settings.clear();
            DatabaseService::removeDiskDatabase();

            return 0;
        }
    }

    // try to create note folders if they are missing
    NoteFolder::migrateToNoteFolders();

    if (parser.isSet(dumpSettingsOption)) {
        const auto dump = Utils::Misc::generateDebugInformation();

        // Copy dump to the clipboard (doesn't seem to work yet when the app is quit)
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(dump);
        QCoreApplication::processEvents();

        // Print the dump to stdout and copy it to the clipboard
        fprintf(stdout, "%s\n", dump.toLocal8Bit().constData());

        return 0;
    }

    return -1;
}

/**
 * Shows the command line help
 */
// void showHelp() {
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
void tempLogMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {
    QByteArray localMsg = msg.toLocal8Bit();
    auto typeText = Utils::Misc::logMsgTypeText(type);
    auto message = QStringLiteral("%1 (%2:%3, %4)")
                       .arg(msg, context.file, QString::number(context.line), context.function);
    auto messageWithType = QStringLiteral("%1: %2\n").arg(typeText, message);

    switch (type) {
        case QtDebugMsg:
            if (SettingsService().value(QStringLiteral("Debug/fileLogging")).toBool()) {
                fprintf(stderr, "Debug: %s\n", localMsg.constData());
            }
            Utils::Misc::logToFileIfAllowed(type, msg);
            break;
        case QtInfoMsg:
            fprintf(stderr, "%s", messageWithType.toLocal8Bit().constData());
            Utils::Misc::logToFileIfAllowed(type, message);
            break;
        case QtWarningMsg:
        case QtCriticalMsg:
        case QtFatalMsg:
            fprintf(stderr, "%s", messageWithType.toLocal8Bit().constData());
            Utils::Misc::logToFileIfAllowed(type, message);
    }
}

inline void setAppProperties(QCoreApplication &app, const QString &release,
                             const QStringList &arguments, bool singleApp, bool snap, bool portable,
                             const QString &action) {
    app.setProperty("release", release);
    app.setProperty("portable", portable);
    if (singleApp) app.setProperty("singleApplication", true);
    app.setProperty("snap", snap);
    app.setProperty("arguments", arguments);
    app.setProperty("startupAction", action);
}

int main(int argc, char *argv[]) {
    // register NoteHistoryItem, so we can store it to the settings
    // we need to do that before we are accessing QSettings or the
    // NoteHistoryItem instances in the settings will get destroyed
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qRegisterMetaTypeStreamOperators<NoteHistoryItem>("NoteHistoryItem");
#endif
    qRegisterMetaType<NoteHistoryItem>("NoteHistoryItem");

    // temporary log output until LogWidget::logMessageOutput takes over
    qInstallMessageHandler(tempLogMessageOutput);

    // enabled by default on Qt6
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
    // Help Wayland to find the desktop file for the taskbar icon
    QGuiApplication::setDesktopFileName(QStringLiteral("PBE.QOwnNotes"));

    QString release = RELEASE;
    bool portable = false;
    bool clearSettings = false;
    bool snap = false;
    bool allowOnlyOneAppInstance = true;
    QStringList arguments;
    QString appNameAdd = QString();
    QString session = QString();
    QString action = QString();

#ifdef QT_DEBUG
    appNameAdd = QStringLiteral("Debug");
#endif

    for (int i = 0; i < argc; ++i) {
        QString arg(argv[i]);
        arguments << arg;

        if (arg == QStringLiteral("--snap")) {
            // override the release string for snaps
            release = QStringLiteral("Snapcraft");
            snap = true;
        } else if (arg == QStringLiteral("--portable")) {
            portable = true;
        } else if (arg == QStringLiteral("--clear-settings")) {
            clearSettings = true;
        } else if (arg == QStringLiteral("--help") || arg == QStringLiteral("--dump-settings") ||
                   arg == QStringLiteral("--completion") || arg == QStringLiteral("-h") ||
                   arg == QStringLiteral("--allow-multiple-instances")) {
            allowOnlyOneAppInstance = false;
        } else if (arg == QStringLiteral("--after-update")) {
            qWarning() << __func__ << " - 'arg': " << arg;
#if not defined(Q_OS_WIN)
            // check if there is a 2nd parameter with the script path
            if (argc > (i + 1)) {
                QString scriptPath(argv[i + 1]);

                // remove the updater script file
                QFile file(scriptPath);
                file.remove();
            }
#endif
        } else if (arg == QStringLiteral("--session")) {
            // check if there is a 2nd parameter with the session name
            if (argc > (i + 1)) {
                session = QString(argv[i + 1]).trimmed();
                appNameAdd += QStringLiteral("-") + session;
            }
        } else if (arg == QStringLiteral("--action")) {
            // check if there is a 2nd parameter with the action name
            if (argc > (i + 1)) {
                action = QString(argv[i + 1]).trimmed();
            }
        } else if (arg == QStringLiteral("--version")) {
            fprintf(stdout, "QOwnNotes %s\n", VERSION);
            exit(0);
        }
    }

    qDebug() << __func__ << " - 'arguments': " << arguments;

    // TODO(pbek): remove
    //    portable = true;

    // disable QML caching in portable mode because the QML cache path cannot be
    // configured see: https://github.com/pbek/QOwnNotes/issues/1284
    if (portable) {
        qputenv("QML_DISABLE_DISK_CACHE", "true");
    }

    // don't log SSL warnings in releases on OS X
#if defined(QT_NO_DEBUG) && defined(Q_OS_MAC)
    qputenv("QT_LOGGING_RULES", "qt.network.ssl.warning=false");
#endif

    // fixing some troubles in Windows 8.1
#ifdef Q_OS_WIN32
    QCoreApplication::addLibraryPath(QStringLiteral("./"));
#endif

    QCoreApplication::setOrganizationDomain(QStringLiteral("PBE"));
    QCoreApplication::setOrganizationName(QStringLiteral("PBE"));
    QCoreApplication::setApplicationName(QStringLiteral("QOwnNotes") + appNameAdd);

    QString appVersion = QStringLiteral(VERSION);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    appVersion += QStringLiteral(" ") + QSysInfo::prettyProductName();

    if (!appVersion.contains(QSysInfo::currentCpuArchitecture())) {
        appVersion += QStringLiteral(" ") + QSysInfo::currentCpuArchitecture();
    }
#else
    appVersion += QStringLiteral(" Qt ") + QStringLiteral(QT_VERSION_STR);
#endif

    appVersion += QStringLiteral(" ") + QString(release);

#ifdef QT_DEBUG
    appVersion += QStringLiteral(" Debug");
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

        if (!portable) {
            DatabaseService::removeDiskDatabase();
        }

        qWarning("Your settings are now cleared!");
    }

    QSettings settings;

    // Override the interface scale factor if the setting is enabled
    if (settings.value(QStringLiteral("overrideInterfaceScalingFactor")).toBool()) {
        qputenv("QT_SCALE_FACTOR",
                QString::number(
                    settings.value(QStringLiteral("interfaceScalingFactor"), 100).toDouble() / 100,
                    'f', 1)
                    .toUtf8());
    }

    QString locale = settings.value(QStringLiteral("interfaceLanguage")).toString();

    if (locale.isEmpty()) {
        locale = QLocale::system().name().section('_', 0, 0);
    }
    qDebug() << __func__ << " - 'locale': " << locale;

    // setup default schema settings
    Utils::Schema::schemaSettings = new Utils::Schema::Settings();

#ifndef QT_DEBUG
    QTranslator translatorsRelease[2];
#endif
    QTranslator translators[13];
#ifdef Q_OS_MAC
    QTranslator translatorOSX;
    QTranslator translatorOSX2;
    QTranslator translatorOSX3;
    QTranslator translatorOSX4;
    // we don't need this on macOS
    allowOnlyOneAppInstance = false;
#else
    // if allowOnlyOneAppInstance still has the default true let's ask the
    // settings
    if (allowOnlyOneAppInstance) {
        allowOnlyOneAppInstance =
            settings.value(QStringLiteral("allowOnlyOneAppInstance"), true).toBool();
    }
#endif

    if (allowOnlyOneAppInstance && !SingleApplication::isSupported()) {
        allowOnlyOneAppInstance = false;
        settings.setValue(QStringLiteral("allowOnlyOneAppInstance"), false);

        qWarning() << QCoreApplication::translate(
            "main",
            "Single application mode is not supported on your "
            "system!");
    }

    // if only one app instance is allowed use SingleApplication
    if (allowOnlyOneAppInstance) {
        SingleApplication app(
            argc, argv, true,
            SingleApplication::Mode::User | SingleApplication::Mode::SecondaryNotification);

        // quit app if it was already started
        if (app.isSecondary()) {
            qWarning() << QCoreApplication::translate(
                "main",
                "Another instance of QOwnNotes was already started! "
                "You can turn off the single instance mode in the settings"
                " or use the parameter --allow-multiple-instances.");

            // send message if an action was set
            if (!action.isEmpty()) {
                app.sendMessage(QStringLiteral("startupAction:%1").arg(action).toUtf8());
            }

            app.exit(0);

            return 0;
        }

        setAppProperties(app, release, arguments, true, snap, portable, action);
#ifndef QT_DEBUG
        loadReleaseTranslations(translatorsRelease, locale);
#endif

        loadTranslations(translators, locale);

#ifdef Q_OS_MAC
        loadMacTranslations(translatorOSX, translatorOSX2, translatorOSX3, translatorOSX4,
                            QCoreApplication::applicationDirPath(), locale);
#endif
        const int result = mainStartupMisc(arguments);
        if (result != -1) {
            return result;
        }

        MainWindow w;
        w.show();

        // receive messages from the primary app
        QObject::connect(&app, &SingleApplication::receivedMessage,
                         [&](quint32 instanceId, QByteArray message) {
                             Q_UNUSED(instanceId)
                             qDebug() << __func__ << " - 'message': " << message;

                             // trigger the startup menu action
                             if (message.startsWith("startupAction:")) {
                                 message.remove(0, 14);
                                 app.setProperty("startupAction", message);
                                 w.triggerStartupMenuAction();
                             }
                         });

        // raise the main window if app was started a 2nd time in single
        // application mode
        QObject::connect(&app, &SingleApplication::instanceStarted, [&] {
            qWarning() << QCoreApplication::translate(
                "main",
                "A second instance of QOwnNotes was attempted to be "
                "started!");

            w.show();
            w.raise();
            w.activateWindow();

            // in case the window was minimized show it normal again
            // (it didn't come up when it was minimized on KDE)
            if (w.isMinimized()) {
                w.showNormal();
            }
        });

        return app.exec();
    } else {
        // use a normal QApplication if multiple instances of the app are
        // allowed
        QApplication app(argc, argv);
        setAppProperties(app, release, arguments, false, snap, portable, action);

#ifndef QT_DEBUG
        loadReleaseTranslations(translatorsRelease, locale);
#endif

        loadTranslations(translators, locale);

#ifdef Q_OS_MAC
        loadMacTranslations(translatorOSX, translatorOSX2, translatorOSX3, translatorOSX4,
                            QCoreApplication::applicationDirPath(), locale);
#endif

        const int result = mainStartupMisc(arguments);
        if (result != -1) {
            return result;
        }

        MainWindow w;
        w.show();

        return app.exec();
    }
}
