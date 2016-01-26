#include "services/updateservice.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QObject>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QMessageBox>
#include <QLibraryInfo>
#include <QUrlQuery>
#include <QSettings>
#include <QApplication>
#include "build_number.h"
#include "dialogs/updatedialog.h"
#include "version.h"
#include "release.h"

UpdateService::UpdateService(QObject *parent) : QObject(parent) {
    this->parent = parent;
}

void UpdateService::checkForUpdates(UpdateMode updateMode) {
    this->updateMode = updateMode;

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(onResult(QNetworkReply *)));

    bool isDebug;

#ifdef QT_NO_DEBUG
    isDebug = false;
#else
    isDebug = true;
#endif

    // there are troubles with https by default on different platforms,
    // so we are using http everywhere for now
    QUrl url("http://www.qownnotes.org/api/v1/last_release/QOwnNotes/" +
             QString(PLATFORM) + ".json");

    QString releaseString = QSysInfo::prettyProductName() + " " +
                            QSysInfo::buildCpuArchitecture();

    if (RELEASE != "") {
        releaseString += ", " + QString(RELEASE);
    }

    // add the current architecture if it differs from the build architecture
    if (QSysInfo::buildCpuArchitecture() !=
            QSysInfo::currentCpuArchitecture()) {
        releaseString += " (" + QSysInfo::currentCpuArchitecture() + ")";
    }

    QUrlQuery q;
    q.addQueryItem("b", QString::number(BUILD));
    q.addQueryItem("v", QString(VERSION));
    q.addQueryItem("r", releaseString);
    q.addQueryItem("d", QString(__DATE__) + " " + QString(__TIME__));
    q.addQueryItem("um", QString::number(updateMode));
    q.addQueryItem("debug", QString::number(isDebug));
    url.setQuery(q);

    manager->get(QNetworkRequest(url));
}

void UpdateService::onResult(QNetworkReply *reply) {
    // abort if reply was null (on OSX after waking from sleep)
    // or there was an error
    if ((reply == NULL) || (reply->error() != QNetworkReply::NoError)) {
        return;
    }

    // we have to add [], so the string can be parsed as JSON
    QString data = QString("[") + (QString) reply->readAll() + QString("]");

    QScriptEngine engine;
    QScriptValue result = engine.evaluate(data);

    // get the information if we should update our app
    QScriptValue shouldUpdate = result.property("0").property("should_update");

    // check if we should update our app
    if (shouldUpdate.toBool()) {
        // get the release url
        QString releaseUrl = result.property("0").property("release").property(
                "assets").property("0").property(
                "browser_download_url").toString();

        // get the release version string
        QString releaseVersionString = result.property("0").property(
                "release_version_string").toString();

        // get the release build number
        int releaseBuildNumber = result.property("0").property(
                "release_build_number").toInteger();

        // get the changes html
        QString changesHtml = result.property("0").property(
                "changes_html").toString();

        bool showUpdateDialog = true;
        if (this->updateMode != UpdateService::Manual) {
            QSettings settings;
            QString skipVersion = settings.value("skipVersion").toString();

            if (releaseVersionString == skipVersion) {
                showUpdateDialog = false;
            } else if (this->updateMode == UpdateService::Periodic) {
                QWidget *widget = QApplication::activeWindow();

                // only load the update dialog if the active window
                // is the MainWindow
                if (QString(widget->metaObject()->className()) !=
                    "MainWindow") {
                    showUpdateDialog = false;
                }
            }
        }

        if (showUpdateDialog) {
            // open the update dialog
            UpdateDialog *dialog = new UpdateDialog(0, changesHtml, releaseUrl,
                                                    releaseVersionString,
                                                    releaseBuildNumber);
            dialog->exec();
        }
    } else if (this->updateMode == UpdateService::Manual) {
        QMessageBox::information(
                0, "No updates",
                "There are no updates available.<br /><strong>" +
                QString(VERSION) +
                "</strong> is the latest version.");
    }
}
