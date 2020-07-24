#include "services/updateservice.h"

#include <QApplication>
#include <QJSEngine>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QSettings>
#include <QUrl>
#include <QUrlQuery>

#include "build_number.h"
#include "dialogs/updatedialog.h"
#include "mainwindow.h"
#include "release.h"
#include "version.h"

UpdateService::UpdateService(QObject *parent) : QObject(parent) {}

void UpdateService::checkForUpdates(MainWindow *mainWindow,
                                    UpdateMode updateMode) {
    this->mainWindow = mainWindow;
    this->updateMode = updateMode;

    QSettings settings;
    if (updateMode != Manual) {
        settings.setValue(QStringLiteral("LastUpdateCheck"),
                          QDateTime::currentDateTime());
    }

    auto *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)), this,
            SLOT(onResult(QNetworkReply *)));

    bool isDebug;

#ifdef QT_NO_DEBUG
    isDebug = false;
#else
    isDebug = true;
#endif

    // there were troubles with https by default on different platforms,
    // so we were using http until now
    QUrl url("https://www.qownnotes.org/api/v1/last_release/QOwnNotes/" +
             QStringLiteral(PLATFORM) + ".json");

    QUrlQuery q;
    QString version(VERSION);

    // check if we want to fake the version number to trigger an update
    if (settings.value(QStringLiteral("Debug/fakeOldVersionNumber")).toBool()) {
        version = QLatin1String("20.7.0");
        isDebug = true;
    }

    q.addQueryItem(QStringLiteral("b"), QString::number(BUILD));
    q.addQueryItem(QStringLiteral("v"), version);
    q.addQueryItem(QStringLiteral("d"),
                   QStringLiteral(__DATE__) + " " + QStringLiteral(__TIME__));
    q.addQueryItem(QStringLiteral("um"), QString::number(updateMode));
    q.addQueryItem(QStringLiteral("debug"), QString::number(isDebug));

    if (!settings.value(QStringLiteral("appMetrics/disableTracking"))
             .toBool() ||
        !settings.value(QStringLiteral("appMetrics/disableAppHeartbeat"))
             .toBool()) {
        q.addQueryItem(
            QStringLiteral("cid"),
            settings.value(QStringLiteral("PiwikClientId")).toString());
    }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    q.addQueryItem(QStringLiteral("r"),
                   qApp->property("release").toString() + " (" +
                       QSysInfo::buildCpuArchitecture() + ")");

    QString operatingSystem = QSysInfo::prettyProductName();
    if (!operatingSystem.contains(QSysInfo::currentCpuArchitecture())) {
        operatingSystem += " (" + QSysInfo::currentCpuArchitecture() + ")";
    }

    q.addQueryItem(QStringLiteral("o"), operatingSystem);
#else
    q.addQueryItem("r", qApp->property("release").toString());
#endif

    url.setQuery(q);

//    QNetworkRequest r(url);
//    QString concatenated(QStringLiteral("xxx:xxx"));
//    QByteArray data = concatenated.toLocal8Bit().toBase64();
//    QString headerData = QStringLiteral("Basic ") + data;
//    r.setRawHeader("Authorization", headerData.toLocal8Bit());
//    manager->get(r);

    manager->get(QNetworkRequest(url));
}

void UpdateService::onResult(QNetworkReply *reply) {
    // abort if reply was null
    if (reply == Q_NULLPTR) {
        return;
    }

    reply->deleteLater();

    // abort if there was an error
    if (reply->error() != QNetworkReply::NoError) {
        qWarning() << __func__
                   << " - 'network reply error': " << reply->error();

        if (this->updateMode == UpdateService::Manual) {
            QMessageBox::warning(
                Q_NULLPTR, tr("Update-checker error"),
                tr("Network reply error: %1").arg(reply->error()));
        }

        return;
    }

    QString allData = reply->readAll();
    if (allData.isEmpty()) {
        qWarning() << __func__
                   << " - 'no data was received by the network "
                      "request'";

        if (this->updateMode == UpdateService::Manual) {
            QMessageBox::warning(
                Q_NULLPTR, tr("Update-checker error"),
                tr("No data was received by the network request!"));
        }

        return;
    }

    // we have to add [], so the string can be parsed as JSON
    QString data = QStringLiteral("[") + allData + QStringLiteral("]");

    QJSEngine engine;
    QJSValue result = engine.evaluate(data);

    if (result.property(QStringLiteral("0")).isNull()) {
        qWarning() << __func__
                   << " - 'the data from the network request "
                      "could not be interpreted': "
                   << allData;

        if (this->updateMode == UpdateService::Manual) {
            QMessageBox::warning(
                Q_NULLPTR, tr("Update-checker error"),
                tr("The data from the network request could not be "
                   "interpreted!"));
        }

        return;
    }

    // get the information if we should update our app
    bool shouldUpdate = result.property(QStringLiteral("0"))
                            .property(QStringLiteral("should_update"))
                            .toBool();

    // check if we should update our app
    if (shouldUpdate) {
        // get the release url
        QString releaseUrl =
            result.property(QStringLiteral("0"))
                .property(QStringLiteral("release"))
                .property(QStringLiteral("assets"))
                .property(QStringLiteral("0"))
                .property(QStringLiteral("browser_download_url"))
                .toString();

        // get the release version string
        QString releaseVersionString =
            result.property(QStringLiteral("0"))
                .property(QStringLiteral("release_version_string"))
                .toString();

        // get the release build number
        int releaseBuildNumber =
            result.property(QStringLiteral("0"))
                .property(QStringLiteral("release_build_number"))
                .toInt();

        // get the changes html
        QString changesHtml = result.property(QStringLiteral("0"))
                                  .property(QStringLiteral("changes_html"))
                                  .toString();

        // show the update available button
        mainWindow->showUpdateAvailableButton(releaseVersionString);

        bool showUpdateDialog = true;

        // do some more checks for non manual update requests
        if (updateMode != UpdateService::Manual) {
            QSettings settings;
            QString skipVersion =
                settings.value(QStringLiteral("skipVersion")).toString();

            // check if this version should be skipped
            if (releaseVersionString == skipVersion) {
                showUpdateDialog = false;
            } else if (updateMode == UpdateService::Periodic) {
                // check if the update dialog is already open
                showUpdateDialog = !UpdateDialog::isUpdateDialogOpen();

                // if the dialog is not open but there is a new release version
                // string open it anyway
                if (!showUpdateDialog &&
                    releaseVersionString != _currentReleaseVersionString) {
                    showUpdateDialog = true;
                }
            }

            // check if the update dialog was disabled
            if (showUpdateDialog) {
                showUpdateDialog =
                    !settings
                         .value(QStringLiteral("disableAutomaticUpdateDialog"))
                         .toBool();
            }
        }

        if (showUpdateDialog) {
            // if there already is an update dialog and if it is open
            // then close and remove the old one
            if (_updateDialog != Q_NULLPTR &&
                UpdateDialog::isUpdateDialogOpen()) {
                _updateDialog->close();
                _updateDialog->deleteLater();
                delete _updateDialog;
            }

            // set the current release version string
            _currentReleaseVersionString = releaseVersionString;

#ifdef Q_OS_WIN32
            // on Windows the application quits if the update dialog is closed
            // while the application is hidden
            if (mainWindow->isHidden()) {
                mainWindow->show();
            }
#endif

            // open the update dialog
            _updateDialog =
                new UpdateDialog(Q_NULLPTR, changesHtml, releaseUrl,
                                 releaseVersionString, releaseBuildNumber);

            // try to prevent stealing of focus on periodic checks
            if (this->updateMode == UpdateService::Periodic) {
                _updateDialog->setAttribute(Qt::WA_ShowWithoutActivating);
            }

            _updateDialog->show();
        }
    } else {
        mainWindow->hideUpdateAvailableButton();

        if (this->updateMode == UpdateService::Manual) {
            QMessageBox::information(
                Q_NULLPTR, tr("No updates"),
                tr("There are no updates available.<br /><strong>%1"
                   "</strong> is the latest version.")
                    .arg(QStringLiteral(VERSION)));
        }
    }
}
