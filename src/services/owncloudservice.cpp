#include "services/owncloudservice.h"

#include <utils/gui.h>
#include <utils/misc.h>

#include <QAuthenticator>
#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QDomDocument>
#include <QDomNodeList>
#include <QEventLoop>
#include <QJSEngine>
#include <QJSValueIterator>
#include <QJsonDocument>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkCookieJar>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringBuilder>
#include <QTimer>
#include <QUrlQuery>

#include "cryptoservice.h"
#include "dialogs/serverbookmarksimportdialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/sharedialog.h"
#include "dialogs/tododialog.h"
#include "dialogs/trashdialog.h"
#include "dialogs/versiondialog.h"
#include "entities/calendaritem.h"
#include "entities/notesubfolder.h"
#include "libraries/versionnumber/versionnumber.h"
#include "mainwindow.h"
#include "services/settingsservice.h"

const QString OwnCloudService::rootPath = QStringLiteral("/index.php/apps/qownnotesapi/api/v1/");
const QString OwnCloudService::format = QStringLiteral("json");
const QString NS_DAV(QStringLiteral("DAV:"));

OwnCloudService::OwnCloudService(int cloudConnectionId, QObject *parent) : QObject(parent) {
    networkManager = new QNetworkAccessManager(this);
    QObject::connect(networkManager,
                     SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), this,
                     SLOT(slotAuthenticationRequired(QNetworkReply *, QAuthenticator *)));
    QObject::connect(networkManager, SIGNAL(finished(QNetworkReply *)), this,
                     SLOT(slotReplyFinished(QNetworkReply *)));

    readSettings(cloudConnectionId);
    settingsDialog = nullptr;
    todoDialog = nullptr;
    shareDialog = nullptr;
}

void OwnCloudService::resetNetworkManagerCookieJar() {
    networkManager->setCookieJar(new QNetworkCookieJar(this));
}

/**
 * Returns true if ownCloud support was enabled in the settings
 *
 * @return
 */
bool OwnCloudService::isOwnCloudSupportEnabled() {
    SettingsService settings;
    return settings.value(QStringLiteral("ownCloud/supportEnabled")).toBool();
}

/**
 * Returns true if todo calendar support was enabled in the settings
 *
 * @return
 */
bool OwnCloudService::isTodoCalendarSupportEnabled() {
    SettingsService settings;
    return settings.value(QStringLiteral("todoCalendarSupport"), true).toBool();
}

/**
 * Returns true if todo support was enabled in the settings
 *
 * @return
 */
bool OwnCloudService::isTodoSupportEnabled() {
    SettingsService settings;
    int calendarBackend =
        settings.value(QStringLiteral("ownCloud/todoCalendarBackend"), DefaultOwnCloudCalendar)
            .toInt();

    if (calendarBackend == CalDAVCalendar) {
        QString todoCalendarServerUrl =
            settings.value(QStringLiteral("ownCloud/todoCalendarCalDAVServerUrl"))
                .toString()
                .trimmed();
        return !todoCalendarServerUrl.isEmpty();
    } else {
        return isOwnCloudSupportEnabled();
    }
}

void OwnCloudService::readSettings(int cloudConnectionId) {
    SettingsService settings;
    CloudConnection cloudConnection = cloudConnectionId != -1
                                          ? CloudConnection::fetch(cloudConnectionId)
                                          : CloudConnection::currentCloudConnection();

    qDebug() << "cloudConnection: " << cloudConnection;

    serverUrl = cloudConnection.getServerUrl();
    serverUrlPath = cloudConnection.getServerUrlPath();
    serverUrlWithoutPath = cloudConnection.getServerUrlWithoutPath();
    userName = cloudConnection.getUsername();
    password = cloudConnection.getPassword();
    appQOwnNotesAPICheckEnabled = cloudConnection.getAppQOwnNotesAPIEnabled();

    versionListPath = rootPath % QStringLiteral("note/versions");
    trashListPath = rootPath % QStringLiteral("note/trashed");
    appInfoPath = rootPath % QStringLiteral("note/app_info");
    capabilitiesPath = QStringLiteral("/ocs/v1.php/cloud/capabilities");
    ownCloudTestPath = QStringLiteral("/ocs/v1.php");
    restoreTrashedNotePath = rootPath % QStringLiteral("note/restore_trashed");
    //    sharePath = "/ocs/v1.php/apps/files_sharing/api/v1/shares";
    sharePath = QStringLiteral("/ocs/v2.php/apps/files_sharing/api/v1/shares");
    bookmarkPath = QStringLiteral("/apps/bookmarks/public/rest/v2/bookmark");
    trashDeletePath =
        QStringLiteral("/remote.php/dav/trashbin/") % userName % QStringLiteral("/trash");

    int calendarBackend =
        settings.value(QStringLiteral("ownCloud/todoCalendarBackend"), DefaultOwnCloudCalendar)
            .toInt();
    QString calendarBackendString;

    switch (calendarBackend) {
        case CalendarPlus:
            calendarBackendString = QStringLiteral("calendarplus");
            break;
        case LegacyOwnCloudCalendar:
            // for older versions of ownCloud
            calendarBackendString = QStringLiteral("caldav");
            break;
        default:
            // Nextcloud 11 uses this string and has problems with the legacy
            // "caldav" url
            calendarBackendString = QStringLiteral("dav");
            break;
    }

    CloudConnection todoCalendarCloudConnection =
        CloudConnection::currentTodoCalendarCloudConnection();

    QString todoCalendarAccountId = todoCalendarCloudConnection.getAccountId();

    if (todoCalendarAccountId.isEmpty()) {
        todoCalendarAccountId = todoCalendarCloudConnection.getUsername();
    }

    QString calendarPath = QStringLiteral("/remote.php/") % calendarBackendString %
                           QStringLiteral("/calendars/") % todoCalendarAccountId;
    todoCalendarServerUrl = todoCalendarCloudConnection.getServerUrl().isEmpty()
                                ? QString()
                                : todoCalendarCloudConnection.getServerUrl() % calendarPath;
    todoCalendarServerUrlWithoutPath = todoCalendarCloudConnection.getServerUrlWithoutPath();
    todoCalendarServerUrlPath = todoCalendarCloudConnection.getServerUrlPath() % calendarPath;
    todoCalendarUsername = todoCalendarCloudConnection.getUsername();
    todoCalendarPassword = todoCalendarCloudConnection.getPassword();

    // if we are using a custom CalDAV server set the settings for it
    if (calendarBackend == OwnCloudService::CalDAVCalendar) {
        todoCalendarServerUrl =
            settings.value(QStringLiteral("ownCloud/todoCalendarCalDAVServerUrl"))
                .toString()
                .trimmed();
        todoCalendarServerUrlPath = QUrl(todoCalendarServerUrl).path();
        todoCalendarUsername =
            settings.value(QStringLiteral("ownCloud/todoCalendarCalDAVUsername")).toString();
        todoCalendarPassword = CryptoService::instance()->decryptToString(
            settings.value(QStringLiteral("ownCloud/todoCalendarCalDAVPassword")).toString());

        todoCalendarServerUrlWithoutPath = todoCalendarServerUrl;
        if (!todoCalendarServerUrlPath.isEmpty()) {
            // remove the path from the calendar server url
            todoCalendarServerUrlWithoutPath.replace(
                QRegularExpression(QRegularExpression::escape(todoCalendarServerUrlPath) %
                                   QStringLiteral("$")),
                QString());
        }
    }
}

QString OwnCloudService::webdavPath() {
    return QStringLiteral("/remote.php/dav/files/") + userName;
}

void OwnCloudService::slotAuthenticationRequired(QNetworkReply *reply,
                                                 QAuthenticator *authenticator) {
    Q_UNUSED(authenticator)
    qWarning() << "Username and/or password incorrect";

#ifndef INTEGRATION_TESTS
    if (settingsDialog != nullptr) {
        settingsDialog->setOKLabelData(3, tr("incorrect"), SettingsDialog::Failure);
        settingsDialog->setOKLabelData(4, tr("not connected"), SettingsDialog::Failure);
    }
#endif

    reply->abort();
}

void OwnCloudService::slotCalendarAuthenticationRequired(QNetworkReply *reply,
                                                         QAuthenticator *authenticator) {
    Q_UNUSED(authenticator)
    qWarning() << "Calendar username and/or password incorrect";

    if (!Utils::Gui::isMessageBoxPresent()) {
        QMessageBox::warning(nullptr, tr("Username / password error"),
                             tr("Your calendar username or password is incorrect!"));
    }

    reply->abort();
}

/**
 * Handle all replies of the network requests
 *
 * @param reply
 */
void OwnCloudService::slotReplyFinished(QNetworkReply *reply) {
#ifndef INTEGRATION_TESTS
    QUrl url = reply->url();
    QString urlPath = url.path();

    qDebug() << "Reply from " << urlPath;

    // output a warning if there is an error
    if (reply->error() != QNetworkReply::NoError) {
        // for error codes see
        // http://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum
        qWarning() << "QNetworkReply error " + QString::number(reply->error()) + " from url " +
                          url.toString() + ": "
                   << reply->errorString();
    }

    // this should only be called from the settings dialog
    if (urlPath.endsWith(appInfoPath)) {
        if (url.query().contains(QStringLiteral("version_test"))) {
            qDebug() << "Reply from app version test";

            checkAppVersion(reply);
        } else {
            qDebug() << "Reply from app info";

            // check if everything is all right and call the callback method
            checkAppInfo(reply);
        }
    } else {
        QByteArray arr = reply->readAll();
        QString data = QString(arr);
        //        qDebug() << __func__ << " - 'data': " << data;

        if (urlPath.endsWith(versionListPath)) {
            qDebug() << "Reply from version list";
            // qDebug() << data;

            // handle the versions loading
            handleVersionsLoading(data);
        } else if (urlPath.endsWith(trashListPath)) {
            qDebug() << "Reply from trash list";
            // qDebug() << data;

            // handle the loading of trashed notes
            handleTrashedLoading(data);
        } else if (urlPath.endsWith(capabilitiesPath)) {
            qDebug() << "Reply from capabilities page";

            if (data.startsWith(QStringLiteral("<?xml version="))) {
                settingsDialog->setOKLabelData(3, tr("ok"), SettingsDialog::OK);
                settingsDialog->setOKLabelData(1, tr("ok"), SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(3, tr("not correct"), SettingsDialog::Failure);
            }
        } else if (urlPath.endsWith(ownCloudTestPath)) {
            qDebug() << "Reply from ownCloud test page";

            if (data.startsWith(QStringLiteral("<?xml version="))) {
                settingsDialog->setOKLabelData(2, tr("ok"), SettingsDialog::OK);
                settingsDialog->setOKLabelData(1, tr("ok"), SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(2, tr("not detected"), SettingsDialog::Failure);
            }
        } else if (urlPath.endsWith(restoreTrashedNotePath)) {
            qDebug() << "Reply from ownCloud restore trashed note page";
            // qDebug() << data;
        } else if (!todoCalendarServerUrlPath.isEmpty() &&
                   urlPath.endsWith(todoCalendarServerUrlPath)) {
            qDebug() << "Reply from ownCloud calendar page" << data;

            QList<CalDAVCalendarData> calendarDataList = parseCalendarData(data);

            //            qInfo() << "calendarDataList: " << calendarDataList;

            if (settingsDialog != nullptr) {
                settingsDialog->refreshTodoCalendarList(calendarDataList);
            }
        } else if (urlPath.startsWith(serverUrlPath % webdavPath())) {
            // this should be the reply of a calendar item list request
            qDebug() << "Reply from ownCloud webdav";

            // load the directories
            loadDirectory(data);
        } else if (urlPath.endsWith(sharePath)) {
            qDebug() << "Reply from share api";

            // update the share status of the notes
            handleNoteShareReply(data);
        } else if (urlPath.startsWith(sharePath)) {
            qDebug() << "Reply from update share api";

            // update the share status of the note
            handleUpdateNoteShareReply(urlPath, data);
        } else if (urlPath.startsWith(bookmarkPath)) {
            qDebug() << "Reply from bookmark api";

            handleImportBookmarksReply(data);
        } else if (url.toString() == serverUrl) {
            qDebug() << "Reply from main server url";
            if (!settingsDialog) {
                return;
            }

            if (!data.isEmpty()) {
                settingsDialog->setOKLabelData(1, tr("ok"), SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(1, tr("not found"), SettingsDialog::Failure);
            }
        }
    }
#endif

    reply->deleteLater();
}

void OwnCloudService::checkAppInfo(QNetworkReply *reply) {
    QString data = QString(reply->readAll());
    // qDebug() << data;

    // we have to add [], so the string can be parsed as JSON
    data = QStringLiteral("[") % data % QStringLiteral("]");

    QJSEngine engine;
    QJSValue result = engine.evaluate(data);

    QString notesPathExistsText = QStringLiteral("unknown");
#ifndef INTEGRATION_TESTS
    bool appIsValid = result.property(0).property(QStringLiteral("versioning")).toBool();
#endif
    QString appVersion =
        result.property(0).property(QStringLiteral("app_version")).toVariant().toString();
    QString serverVersion =
        result.property(0).property(QStringLiteral("server_version")).toVariant().toString();

#ifndef INTEGRATION_TESTS
    if (!settingsDialog) {
        return;
    }
    // reset to "unknown" in case we can't test if versions
    // and trash app are enabled
    settingsDialog->setOKLabelData(6, tr("unknown"), SettingsDialog::Unknown);
    settingsDialog->setOKLabelData(7, tr("unknown"), SettingsDialog::Unknown);

    if (!serverVersion.isEmpty()) {
        VersionNumber serverAppVersion = VersionNumber(appVersion);
        VersionNumber minAppVersion = VersionNumber(QOWNNOTESAPI_MIN_VERSION);

        if (minAppVersion > serverAppVersion) {
            settingsDialog->setOKLabelData(4, tr("version %1 too low").arg(appVersion),
                                           SettingsDialog::Warning);
        } else {
            settingsDialog->setOKLabelData(4, tr("ok"), SettingsDialog::OK);
        }

        // check if versions and trash app are enabled after QOwnNotesAPI v0.3.1
        if (serverAppVersion >= VersionNumber(QStringLiteral("0.3.1"))) {
            bool versionsAppEnabled =
                result.property(0).property(QStringLiteral("versions_app")).toBool();
            bool trashAppEnabled =
                result.property(0).property(QStringLiteral("trash_app")).toBool();

            if (versionsAppEnabled) {
                settingsDialog->setOKLabelData(6, tr("ok"), SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(6, tr("not enabled"), SettingsDialog::Failure);
            }

            if (trashAppEnabled) {
                settingsDialog->setOKLabelData(7, tr("ok"), SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(7, tr("not enabled"), SettingsDialog::Failure);
            }
        }

        // check if notes path was found after QOwnNotesAPI v0.4.
        if (serverAppVersion >= VersionNumber(QStringLiteral("0.4.1"))) {
            bool notesPathExists =
                result.property(0).property(QStringLiteral("notes_path_exists")).toBool();
            notesPathExistsText =
                notesPathExists ? QStringLiteral("yes") : QStringLiteral("not found");

            if (notesPathExists) {
                settingsDialog->setOKLabelData(8, tr("ok"), SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(8, tr("not found"), SettingsDialog::Failure);
                qDebug() << __func__ << " - 'data': " << data;
            }
        }
    } else {
        settingsDialog->setOKLabelData(4, tr("not connected"), SettingsDialog::Failure);
    }

    // call callback in settings dialog
    settingsDialog->connectTestCallback(appIsValid, appVersion, serverVersion, notesPathExistsText,
                                        reply->errorString());
#endif

    reply->deleteLater();
}

/**
 * Checks the api app version from the reply of startAppVersionTest()
 *
 * @param reply
 */
void OwnCloudService::checkAppVersion(QNetworkReply *reply) {
    QString data = QString(reply->readAll());
    // qDebug() << data;

    // we have to add [], so the string can be parsed as JSON
    data = QStringLiteral("[") % data % QStringLiteral("]");

    QJSEngine engine;
    QJSValue result = engine.evaluate(data);

    QString appVersion =
        result.property(0).property(QStringLiteral("app_version")).toVariant().toString();

    if (appVersion.isEmpty()) {
        return;
    }

#ifndef INTEGRATION_TESTS
    VersionNumber serverAppVersion = VersionNumber(appVersion);
    VersionNumber minAppVersion = VersionNumber(QOWNNOTESAPI_MIN_VERSION);

    // show a warning if app version is too low
    if (serverAppVersion < minAppVersion) {
        MainWindow *mainWindow = MainWindow::instance();

        if (mainWindow == nullptr) {
            return;
        }

        QMessageBox::warning(mainWindow, tr("API app version too low"),
                             tr("Please consider updating your QOwnNotesAPI app on your "
                                "server, your app version %1 is too low and may "
                                "cause troubles in QOwnNotes.")
                                 .arg(appVersion));
    }
#endif

    reply->deleteLater();
}

/**
 * @brief OwnCloudService::connectionTest
 */
void OwnCloudService::settingsConnectionTest(SettingsDialog *dialog) {
    settingsDialog = dialog;

    // qDebug() << serverUrl;
    // qDebug() << userName;
    // qDebug() << password;

    QUrl url(serverUrl);
    QNetworkRequest r(url);

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    // direct server url request without auth header
    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);

    QUrlQuery q;
    q.addQueryItem(QStringLiteral("format"), format);
    url.setQuery(q);

    addAuthHeader(&r);

    url.setUrl(serverUrl % capabilitiesPath);
    r.setUrl(url);
    reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);

    url.setUrl(serverUrl % ownCloudTestPath);
    r.setUrl(url);
    reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);

    if (appQOwnNotesAPICheckEnabled) {
        url.setUrl(serverUrl % appInfoPath);
        QString serverNotesPath = NoteFolder::currentRemotePath();
        q.addQueryItem(QStringLiteral("notes_path"), serverNotesPath);
        url.setQuery(q);
        r.setUrl(url);
        reply = networkManager->get(r);
        ignoreSslErrorsIfAllowed(reply);
    }
}

/**
 * Starts an api app version test
 */
void OwnCloudService::startAppVersionTest() {
#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QUrl url(serverUrl % appInfoPath);
    QString serverNotesPath = NoteFolder::currentRemotePath();
    QUrlQuery q;

    q.addQueryItem(QStringLiteral("format"), format);
    q.addQueryItem(QStringLiteral("notes_path"), serverNotesPath);
    q.addQueryItem(QStringLiteral("version_test"), QStringLiteral("1"));
    url.setQuery(q);

    QNetworkRequest r(url);
    addAuthHeader(&r);

    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * Ignores ssl errors for a QNetworkReply if allowed
 */
void OwnCloudService::ignoreSslErrorsIfAllowed(QNetworkReply *reply) {
    SettingsService settings;
    if (settings.value(QStringLiteral("networking/ignoreSSLErrors"), true).toBool()) {
        QObject::connect(reply, SIGNAL(sslErrors(QList<QSslError>)), reply,
                         SLOT(ignoreSslErrors()));
    }
}

/**
 * Gets the calendar list from the ownCloud server
 * for the settings dialog
 */
void OwnCloudService::settingsGetCalendarList(SettingsDialog *dialog) {
    if (!isTodoSupportEnabled()) {
        return;
    }

    if (todoCalendarServerUrl.isEmpty()) {
        return;
    }

    settingsDialog = dialog;

    QUrl url(todoCalendarServerUrl);
    QNetworkRequest r(url);
    addCalendarAuthHeader(&r);

    // build the request body
    QString body = QStringLiteral(
        "<d:propfind xmlns:d=\"DAV:\" "
        "xmlns:cs=\"http://sabredav.org/ns\" xmlns:c=\"urn:ietf:params:xml:ns:caldav\"> \
            <d:prop> \
               <d:resourcetype /> \
               <d:displayname /> \
               <cs:getctag /> \
               <c:supported-calendar-component-set /> \
            </d:prop> \
          </d:propfind>");

    auto *dataToSend = new QByteArray(body.toUtf8());
    r.setHeader(QNetworkRequest::ContentLengthHeader, dataToSend->size());
    r.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/xml"));
    r.setRawHeader(QByteArray("Depth"), QByteArray("1"));
    auto *buffer = new QBuffer(dataToSend);
    auto *calNetworkManager = new QNetworkAccessManager(this);

    QObject::connect(calNetworkManager,
                     SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), this,
                     SLOT(slotCalendarAuthenticationRequired(QNetworkReply *, QAuthenticator *)));

    // Connect the finished signal to a lambda function
    connect(calNetworkManager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QString data = QString(responseData);
            qDebug() << "Reply from calendar page after PROPFIND" << data;

            QList<CalDAVCalendarData> calendarDataList = parseCalendarData(data);

#ifndef INTEGRATION_TESTS
            if (settingsDialog != nullptr) {
                settingsDialog->refreshTodoCalendarList(calendarDataList);
            }
#endif
        } else {
            // For error codes see http://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum
            qWarning() << "QNetworkReply error " + QString::number(reply->error()) + " from url " +
                              url.toString() + ":"
                       << reply->errorString();
        }

        reply->deleteLater();
        calNetworkManager->deleteLater();
    });

    QNetworkReply *reply = calNetworkManager->sendCustomRequest(r, "PROPFIND", buffer);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * @brief Gets the task list from the ownCloud server for the task list dialog
 */
void OwnCloudService::todoGetTodoList(const QString &calendarName, TodoDialog *dialog) {
    this->todoDialog = dialog;
    SettingsService settings;
    QStringList todoCalendarEnabledList =
        settings.value(QStringLiteral("ownCloud/todoCalendarEnabledList")).toStringList();
    int index = todoCalendarEnabledList.indexOf(calendarName);

    // return if we didn't find the calendar, this should not happen
    if (index == -1) {
        return;
    }

    QStringList todoCalendarEnabledUrlList =
        settings.value(QStringLiteral("ownCloud/todoCalendarEnabledUrlList")).toStringList();

    // return if there are to few items in the url list
    if (todoCalendarEnabledUrlList.size() < todoCalendarEnabledList.size()) {
        return;
    }

    QString calendarUrl = settings.value(QStringLiteral("ownCloud/todoCalendarEnabledUrlList"))
                              .toStringList()
                              .at(index);
    qDebug() << __func__ << " - 'calendarUrl': " << calendarUrl;

    QUrl url(calendarUrl);
    QNetworkRequest r(url);
    addCalendarAuthHeader(&r);

    // ownCloud needs depth to be set to 1
    r.setRawHeader(QByteArray("DEPTH"), QByteArray("1"));

    // build the request body, we only want VTODO items
    QString body = QStringLiteral(
        "<c:calendar-query xmlns:d=\"DAV:\" "
        "xmlns:c=\"urn:ietf:params:xml:ns:caldav\"> \
            <d:prop> \
                <d:getetag /> \
                <d:getlastmodified /> \
            </d:prop> \
            <c:filter> \
                <c:comp-filter name=\"VCALENDAR\"> \
                    <c:comp-filter name=\"VTODO\" /> \
                </c:comp-filter> \
            </c:filter> \
        </c:calendar-query>");

    auto *dataToSend = new QByteArray(body.toUtf8());
    r.setHeader(QNetworkRequest::ContentLengthHeader, dataToSend->size());
    r.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/xml"));
    auto *buffer = new QBuffer(dataToSend);

    auto *calNetworkManager = new QNetworkAccessManager(this);

    QObject::connect(calNetworkManager,
                     SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), this,
                     SLOT(slotCalendarAuthenticationRequired(QNetworkReply *, QAuthenticator *)));

    // Connect the finished signal to a lambda function
    connect(calNetworkManager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QString data = QString(responseData);
            qDebug() << "Reply from calendar page after REPORT" << data;

            // Parse the responseData to extract the to-do items
            loadTodoItems(calendarName, data);
        } else {
            // For error codes see http://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum
            qWarning() << "QNetworkReply error " + QString::number(reply->error()) + " from url " +
                              url.toString() + ":"
                       << reply->errorString();
        }

        reply->deleteLater();
        calNetworkManager->deleteLater();
    });

    QNetworkReply *reply = calNetworkManager->sendCustomRequest(r, "REPORT", buffer);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * Shares a note on ownCloud
 */
void OwnCloudService::shareNote(const Note &note, ShareDialog *dialog) {
    this->shareDialog = dialog;
    qDebug() << __func__ << " - 'note': " << note;

    // return if no settings are set
    if (!hasOwnCloudSettings()) {
        showOwnCloudMessage(QString(), QStringLiteral("You need to setup your ownCloud server "
                                                      "to share notes"));
        return;
    }

    QUrl url(serverUrl % sharePath);
    QString path = NoteFolder::currentRemotePath() + note.relativeNoteFilePath(QStringLiteral("/"));

    QByteArray postData;
    // set to public link
    postData.append(QStringLiteral("shareType=3&").toLatin1());
    postData.append((QStringLiteral("path=") % QUrl::toPercentEncoding(path)).toLatin1());

    qDebug() << __func__ << " - 'url': " << url;
    qDebug() << __func__ << " - 'postData': " << postData;

    QNetworkRequest r(url);
    addAuthHeader(&r);
    r.setHeader(QNetworkRequest::ContentTypeHeader,
                QStringLiteral("application/x-www-form-urlencoded"));

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QNetworkReply *reply = networkManager->post(r, postData);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * Allow note editing on a shared note on Nextcloud
 */
void OwnCloudService::setPermissionsOnSharedNote(const Note &note, ShareDialog *dialog) {
    this->shareDialog = dialog;
    qDebug() << __func__ << " - 'note': " << note;

    // return if no settings are set
    if (!hasOwnCloudSettings()) {
        showOwnCloudMessage();
        return;
    }

    QUrl url(serverUrl % sharePath % QStringLiteral("/") % QString::number(note.getShareId()) %
             QStringLiteral("?format=xml"));
    //    QString path = NoteFolder::currentRemotePath() +
    //                   note.relativeNoteFilePath(QStringLiteral("/"));

    QUrlQuery params;
    params.addQueryItem(QStringLiteral("permissions"), QString::number(note.getSharePermissions()));
    params.addQueryItem(QStringLiteral("cid"), QString::number(note.getShareId()));

    qDebug() << __func__ << " - 'url': " << url;
    qDebug() << __func__ << " - 'params': " << params.query();

    QNetworkRequest r(url);
    addAuthHeader(&r);
    r.setHeader(QNetworkRequest::ContentTypeHeader,
                QStringLiteral("application/x-www-form-urlencoded"));

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QNetworkReply *reply = networkManager->put(r, params.query().toUtf8());
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * Removes a note shares on ownCloud
 */
void OwnCloudService::removeNoteShare(const Note &note, ShareDialog *dialog) {
    this->shareDialog = dialog;
    qDebug() << __func__ << " - 'note': " << note;

    // return if no settings are set
    if (!hasOwnCloudSettings()) {
        showOwnCloudMessage(QString(), QStringLiteral("You need to setup your ownCloud server "
                                                      "to remove a note share"));
        return;
    }

    QUrl url(serverUrl % sharePath % QStringLiteral("/") % QString::number(note.getShareId()));

    qDebug() << __func__ << " - 'url': " << url;

    QNetworkRequest r(url);
    addAuthHeader(&r);

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QNetworkReply *reply = networkManager->sendCustomRequest(r, "DELETE");
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * Fetches the shares from ownCloud
 *
 * To fetch the shares of all notes we have to fetch all shares from the
 * account and search for our note folder
 */
void OwnCloudService::fetchShares(const QString &path) {
    // return if no settings are set
    if (!hasOwnCloudSettings()) {
        return;
    }

    QUrl url(serverUrl % sharePath);

    if (!path.isEmpty()) {
        QUrlQuery q;
        q.addQueryItem(QStringLiteral("path"), path);
        url.setQuery(q);
    }

    qDebug() << __func__ << " - 'url': " << url;

    QNetworkRequest r(url);
    addAuthHeader(&r);

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * Fetches the bookmarks from Nextcloud Bookmarks
 */
void OwnCloudService::fetchBookmarks() {
    // return if no settings are set
    if (!hasOwnCloudSettings()) {
        return;
    }

    QUrl url(serverUrl % bookmarkPath % QStringLiteral("?page=-1"));

    qDebug() << __func__ << " - 'url': " << url;

    QNetworkRequest r(url);
    addAuthHeader(&r);

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * @brief Removes a task list item from the ownCloud server
 */
void OwnCloudService::removeCalendarItem(CalendarItem calItem, TodoDialog *dialog) {
    this->todoDialog = dialog;

    QUrl url(calItem.getUrl());
    QNetworkRequest r(url);
    addCalendarAuthHeader(&r);
    auto *calNetworkManager = new QNetworkAccessManager(this);

    QObject::connect(calNetworkManager,
                     SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), this,
                     SLOT(slotCalendarAuthenticationRequired(QNetworkReply *, QAuthenticator *)));

    // Connect the finished signal to a lambda function
    connect(calNetworkManager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Reply from calendar page after DELETE" << url;

#ifndef INTEGRATION_TESTS
            if (todoDialog != nullptr) {
                // Reload the task list from server
                todoDialog->reloadTodoList();
            }
#endif
        } else {
            // For error codes see http://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum
            qWarning() << "QNetworkReply error " + QString::number(reply->error()) + " from url " +
                              url.toString() + ":"
                       << reply->errorString();
        }

        reply->deleteLater();
        calNetworkManager->deleteLater();
    });

    QNetworkReply *reply = calNetworkManager->sendCustomRequest(r, "DELETE");
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * @brief Restores a note on the server
 */
void OwnCloudService::restoreTrashedNoteOnServer(const QString &fileName, int timestamp) {
    QUrl url(serverUrl % restoreTrashedNotePath);
    QString serverNotesPath = Utils::Misc::appendIfDoesNotEndWith(
        NoteFolder::currentRemotePath() + NoteSubFolder::activeNoteSubFolder().relativePath('/'),
        QStringLiteral("/"));

    url.setUserName(userName);
    url.setPassword(password);

    QUrlQuery q;
    q.addQueryItem(QStringLiteral("format"), format);
    q.addQueryItem(QStringLiteral("file_name"), serverNotesPath % fileName);
    q.addQueryItem(QStringLiteral("timestamp"), QString::number(timestamp));
    url.setQuery(q);

    qDebug() << url;

    QNetworkRequest r(url);
    addAuthHeader(&r);

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * @brief Deletes a trashed note on the server and returns the status code
 */
int OwnCloudService::deleteTrashedNoteOnServer(const QString &fileName, int timestamp) {
    auto *manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QUrl url(serverUrl % trashDeletePath % QStringLiteral("/") % QUrl::toPercentEncoding(fileName) %
             QStringLiteral(".d") % QString::number(timestamp));

    qDebug() << __func__ << " - 'url': " << url;

    QNetworkRequest networkRequest = QNetworkRequest(url);
    addAuthHeader(&networkRequest);

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    int statusCode = -1;
    QNetworkReply *reply = manager->deleteResource(networkRequest);
    ignoreSslErrorsIfAllowed(reply);
    loop.exec();

    // if we didn't get a timeout let us return the content
    if (timer.isActive()) {
        statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    }

    reply->deleteLater();
    delete (manager);

    return statusCode;
}

/**
 * @brief OwnCloudService::loadVersions
 */
void OwnCloudService::loadVersions(const QString &fileName) {
    QUrl url(serverUrl % versionListPath);
    QString serverPath = NoteFolder::currentRemotePath() % fileName;
    qDebug() << __func__ << " - 'serverPath': " << serverPath;

    url.setUserName(userName);
    url.setPassword(password);

    QUrlQuery q;
    q.addQueryItem(QStringLiteral("format"), format);
    q.addQueryItem(QStringLiteral("file_name"), serverPath);
    url.setQuery(q);

    QNetworkRequest r(url);
    addAuthHeader(&r);

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * @brief OwnCloudService::loadTrash
 */
void OwnCloudService::loadTrash() {
    QUrl url(serverUrl % trashListPath);
    QString serverNotesPath =
        NoteFolder::currentRemotePath() + NoteSubFolder::activeNoteSubFolder().relativePath('/');

    url.setUserName(userName);
    url.setPassword(password);

    QUrlQuery q;
    q.addQueryItem(QStringLiteral("format"), format);
    q.addQueryItem(QStringLiteral("dir"), serverNotesPath);

    QStringList noteFileExtensionList = Note::noteFileExtensionList();

    // add the custom note extensions
    if (noteFileExtensionList.count() > 0) {
        QListIterator<QString> itr(noteFileExtensionList);
        while (itr.hasNext()) {
            QString fileExtension = itr.next();
            q.addQueryItem(QStringLiteral("extensions[]"), fileExtension);
        }
    }

    url.setQuery(q);

    // qDebug() << url;

    QNetworkRequest r(url);
    addAuthHeader(&r);

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);
}

void OwnCloudService::addAuthHeader(QNetworkRequest *r) {
    addGenericAuthHeader(r, userName, password);
}

void OwnCloudService::addGenericAuthHeader(QNetworkRequest *r, const QString &userName,
                                           const QString &password) {
    if (r) {
        QString concatenated = userName % QStringLiteral(":") % password;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        QString headerData = QStringLiteral("Basic ") % data;
        r->setRawHeader("Authorization", headerData.toLocal8Bit());

        // Nextcloud 11+ needs that
        r->setRawHeader("OCS-APIRequest", "true");

        // we set a user agent to prevent troubles with some ownCloud /
        // Nextcloud server hosting providers
        r->setRawHeader("User-Agent", OWNCLOUD_SERVICE_USER_AGENT);
    }
}

void OwnCloudService::addCalendarAuthHeader(QNetworkRequest *r) {
    if (r) {
        QString concatenated = todoCalendarUsername % QStringLiteral(":") % todoCalendarPassword;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        QString headerData = QStringLiteral("Basic ") % data;
        r->setRawHeader("Authorization", headerData.toLocal8Bit());

        // we set a user agent to prevent troubles with some ownCloud /
        // Nextcloud server hosting providers
        r->setRawHeader("User-Agent", OWNCLOUD_SERVICE_USER_AGENT);
    }
}

/**
 * Checks if ownCloud settings are set
 */
bool OwnCloudService::hasOwnCloudSettings(bool withEnabledCheck, bool ignoreTableWarning) {
    if (withEnabledCheck && !isOwnCloudSupportEnabled()) {
        return false;
    }

    SettingsService settings;
    CloudConnection cloudConnection = CloudConnection::currentCloudConnection(ignoreTableWarning);

    QString serverUrl = cloudConnection.getServerUrl();
    QString userName = cloudConnection.getUsername();
    QString password = cloudConnection.getPassword();

    return !(serverUrl.isEmpty() || userName.isEmpty() || password.isEmpty());
}

/**
 * Shows a message dialog with a ownCloud server error
 */
void OwnCloudService::showOwnCloudServerErrorMessage(const QString &message,
                                                     bool withSettingsButton) {
    QString headline = Utils::Misc::replaceOwnCloudText(tr("ownCloud server connection error"));
    QString text = message.isEmpty() ? QStringLiteral(
                                           "Cannot connect to your ownCloud server! "
                                           "Please check your ownCloud configuration.")
                                     : tr("ownCloud server error: <strong>%1</strong><br />"
                                          "Please check your ownCloud configuration.")
                                           .arg(message);

    text = Utils::Misc::replaceOwnCloudText(text);

    showOwnCloudMessage(headline, text, withSettingsButton);
}

/**
 * Shows a ownCloud message dialog
 */
void OwnCloudService::showOwnCloudMessage(QString headline, QString message,
                                          bool withSettingsButton) {
    if (headline.isEmpty()) {
        headline = Utils::Misc::replaceOwnCloudText(QStringLiteral("ownCloud"));
    }

    if (message.isEmpty()) {
        message = Utils::Misc::replaceOwnCloudText(tr("You need to setup your ownCloud server!"));
    }

    // don't show an actual message box if there already is one present on
    // the screen
    if (Utils::Gui::isMessageBoxPresent()) {
        qWarning() << headline << ": " << message;
        return;
    }

    if (withSettingsButton) {
        if (QMessageBox::warning(nullptr, headline, message, tr("Open &settings"), tr("&Cancel"),
                                 QString(), 0, 1) == 0) {
#ifndef INTEGRATION_TESTS
            MainWindow *mainWindow = MainWindow::instance();

            if (mainWindow != nullptr) {
                mainWindow->openSettingsDialog(SettingsDialog::OwnCloudPage);
            }
#endif
        }
    } else {
        QMessageBox::warning(nullptr, headline, message);
    }
}

/**
 * Returns the global OwnCloudService instance
 */
OwnCloudService *OwnCloudService::instance(bool reset, int cloudConnectionId) {
    auto *instance = qApp->property("ownCloudService").value<OwnCloudService *>();

    // OwnCloudService::settingsGetFileList would need the reset, but clearing
    // the cookie jar also helped
    /*
    if (reset) {
        delete instance;
        instance = nullptr;
        qDebug() << "OwnCloudService::instance was reset";
    }
    */
    Q_UNUSED(reset)

    if (instance == nullptr) {
        instance = new OwnCloudService(cloudConnectionId);

        qApp->setProperty("ownCloudService", QVariant::fromValue<OwnCloudService *>(instance));
    } else {
        if (reset) {
            instance->resetNetworkManagerCookieJar();
        }

        // we need to read the settings in case something has changed
        instance->readSettings(cloudConnectionId);
    }

    return instance;
}

/**
 * Handles the versions loading
 *
 * @brief OwnCloudService::handleVersionsLoading
 * @param data
 */
void OwnCloudService::handleVersionsLoading(QString data) {
#ifndef INTEGRATION_TESTS
    MainWindow::instance()->enableShowVersionsButton();
    MainWindow::instance()->showStatusBarMessage(tr("Done with loading note versions"),
                                                 QStringLiteral("🕒"), 2000);
#endif

    // check if we get any data at all
    if (data.isEmpty()) {
        showOwnCloudServerErrorMessage();
        return;
    }

    // we have to add [], so the string can be parsed as JSON
    data = QStringLiteral("[") % data % QStringLiteral("]");
    qDebug() << __func__ << " - 'data': " << data;

    QJSEngine engine;
    QJSValue result = engine.evaluate(data);

    // get the information if versioning is available
    // we are casting to QVariant first because otherwise we might get a
    // "undefined" string if "message" is not set
    QString message = result.property(0).property(QStringLiteral("message")).toVariant().toString();

    // check if we got an error message
    if (!message.isEmpty()) {
        showOwnCloudServerErrorMessage(message);
        return;
    }

    // get the filename to check if everything is all right
    QString fileName =
        result.property(0).property(QStringLiteral("file_name")).toVariant().toString();

    // get the versions
    QJSValue versions = result.property(0).property(QStringLiteral("versions"));
    QJSValueIterator versionsIterator(versions);

    // check if we got no useful data, we also need to do this to prevent
    // crashes
    if (fileName.isEmpty() || !versionsIterator.hasNext() || versions.toString().isEmpty()) {
        QMessageBox::information(nullptr, tr("No versions found"),
                                 tr("There are no versions for this note or the note wasn't "
                                    "found on the server."));
        return;
    }

#ifndef INTEGRATION_TESTS
    qDebug() << __func__ << " - 'versions': " << Utils::Misc::jsValueToJsonString(versions);

    auto *dialog = new VersionDialog(versions);
    dialog->exec();
#endif
}

/**
 * Handles the loading of trashed notes
 *
 * @brief OwnCloudService::handleTrashedLoading
 * @param data
 */
void OwnCloudService::handleTrashedLoading(QString data) {
#ifndef INTEGRATION_TESTS
    MainWindow::instance()->enableShowTrashButton();
    MainWindow::instance()->showStatusBarMessage(tr("Done with loading trashed notes"),
                                                 QStringLiteral("🗑️"), 2000);
#endif

    // check if we get any data at all
    if (data == QString()) {
        showOwnCloudServerErrorMessage();
        return;
    }

    // we have to add [], so the string can be parsed as JSON
    data = QStringLiteral("[") % data % QStringLiteral("]");

    QJSEngine engine;
    QJSValue result = engine.evaluate(data);

    // get a possible error messages
    // we are casting to QVariant first because otherwise we might get a
    // "undefined" string if "message" is not set
    QString message = result.property(0).property(QStringLiteral("message")).toVariant().toString();

    // check if we got an error message
    if (!message.isEmpty()) {
        showOwnCloudServerErrorMessage(message);
        return;
    }

    // get the directory to check if everything is all right
    QString directory = result.property(0).property(QStringLiteral("directory")).toString();

    // check if we got no useful data
    if (directory.isEmpty()) {
        showOwnCloudServerErrorMessage();
        return;
    }

    // get the notes
    QJSValue notes = result.property(0).property(QStringLiteral("notes"));

    // check if we got no useful data
    if (notes.toString().isEmpty()) {
        QMessageBox::information(0, tr("No trashed notes"),
                                 tr("No trashed notes were found on the "
                                    "server."));
        return;
    }

#ifndef INTEGRATION_TESTS
    TrashDialog *dialog = new TrashDialog(notes);
    dialog->exec();
#endif
}

/**
 * Parses the calendar data to return the calendar url and display name
 *
 * @param data
 * @return
 */
QList<CalDAVCalendarData> OwnCloudService::parseCalendarData(QString &data) {
    QList<CalDAVCalendarData> resultList;

    // check if there was nothing returned at all from the CalDAV server
    if (data.isEmpty()) {
        QMessageBox::critical(nullptr, tr("Error while loading todo lists!"),
                              tr("Your CalDAV server didn't reply anything!"));

        return resultList;
    }

    QDomDocument doc;
    qDebug() << __func__ << " - 'data': " << data;

    doc.setContent(data, true);

    QDomNodeList errorNodes = doc.elementsByTagNameNS(NS_DAV, QStringLiteral("error"));

    // check if there was an error returned by the CalDAV server
    for (int i = 0; i < errorNodes.length(); ++i) {
        QDomNode responseNode = errorNodes.at(i);
        if (responseNode.isElement()) {
            QDomElement elem = responseNode.toElement();

            if (elem.childNodes().length()) {
                QDomNodeList typeNodes = elem.childNodes();
                for (int j = 0; j < typeNodes.length(); ++j) {
                    QDomNode typeNode = typeNodes.at(j);
                    QString typeString = typeNode.toElement().tagName();
                    if (typeString == QStringLiteral("message")) {
                        QMessageBox::critical(nullptr, tr("Error while loading todo lists!"),
                                              tr("Error message from your "
                                                 "CalDAV server: <strong>%1</strong>")
                                                  .arg(typeNode.toElement().text()));

                        return resultList;
                    }
                }
            }
        }
    }

    SettingsService settings;
    bool ignoreNonTodoCalendars =
        settings.value(QStringLiteral("ownCloud/ignoreNonTodoCalendars"), true).toBool();

    // loop all response blocks
    QDomNodeList responseNodes = doc.elementsByTagNameNS(NS_DAV, QStringLiteral("response"));
    for (int i = 0; i < responseNodes.length(); ++i) {
        QDomNode responseNode = responseNodes.at(i);
        if (responseNode.isElement()) {
            QDomElement elem = responseNode.toElement();
            QDomNodeList resourceTypeNodes =
                elem.elementsByTagNameNS(NS_DAV, QStringLiteral("resourcetype"));
            if (resourceTypeNodes.length()) {
                QDomNodeList typeNodes = resourceTypeNodes.at(0).childNodes();
                for (int j = 0; j < typeNodes.length(); ++j) {
                    QDomNode typeNode = typeNodes.at(j);
                    QString typeString = typeNode.toElement().tagName();

                    // check if we found a calendar
                    if (typeString != QStringLiteral("calendar")) {
                        continue;
                    }

                    // check if the calendar is a VTODO calendar
                    if (ignoreNonTodoCalendars) {
                        QDomNodeList componentSetNodes = elem.elementsByTagName(
                            QStringLiteral("supported-calendar-component-set"));
                        bool isTodoCalendar = false;
                        if (componentSetNodes.length()) {
                            for (int k = 0; k < componentSetNodes.length(); ++k) {
                                QDomNodeList componentSets = componentSetNodes.at(k).childNodes();

                                if (componentSets.length()) {
                                    for (int l = 0; l < componentSets.length(); ++l) {
                                        QDomNode componentSet = componentSets.at(l);
                                        QString componentSetString =
                                            componentSet.toElement().attribute(
                                                QStringLiteral("name"));
                                        if (componentSetString == QStringLiteral("VTODO")) {
                                            isTodoCalendar = true;
                                        }
                                    }
                                }
                            }
                        }

                        // we only want VTODO calendars
                        if (!isTodoCalendar) {
                            continue;
                        }
                    }

                    CalDAVCalendarData calendarData = CalDAVCalendarData();
                    // add the href to our result list
                    QDomNodeList hrefNodes =
                        elem.elementsByTagNameNS(NS_DAV, QStringLiteral("href"));
                    if (hrefNodes.length()) {
                        const QString href = hrefNodes.at(0).toElement().text();

                        if (href.isEmpty()) {
                            continue;
                        }

                        calendarData.url = href;
                    }

                    QDomNodeList displayNameNodes =
                        elem.elementsByTagNameNS(NS_DAV, QStringLiteral("displayname"));
                    if (displayNameNodes.length()) {
                        const QString displayName = displayNameNodes.at(0).toElement().text();
                        calendarData.displayName = displayName;
                    }

                    if (calendarData.displayName.isEmpty()) {
                        calendarData.displayName = QStringLiteral("Unknown");
                    }

                    resultList << calendarData;
                }
            }
        }
    }

    return resultList;
}

void OwnCloudService::loadTodoItems(const QString &calendarName, QString &data) {
    QDomDocument doc;
    doc.setContent(data, true);

    // fetch all urls that are currently in the calendar
    QList<QUrl> calendarItemUrlRemoveList = CalendarItem::fetchAllUrlsByCalendar(calendarName);

    QDomNodeList responseNodes = doc.elementsByTagNameNS(NS_DAV, QStringLiteral("response"));
    int responseNodesCount = responseNodes.length();
    int requestCount = 0;

#ifndef INTEGRATION_TESTS
    // set the preliminary maximum of the progress bar
    if (todoDialog != nullptr) {
        todoDialog->todoItemLoadingProgressBarSetMaximum(responseNodesCount);
    }
#endif

    // loop all response blocks
    for (int i = 0; i < responseNodesCount; ++i) {
        // update the UI
        QCoreApplication::processEvents();

        QDomNode responseNode = responseNodes.at(i);
        if (responseNode.isElement()) {
            QDomElement elem = responseNode.toElement();

            // check if we have an url
            QDomNodeList urlPartNodes = elem.elementsByTagNameNS(NS_DAV, QStringLiteral("href"));
            if (urlPartNodes.length()) {
                QString urlPart = urlPartNodes.at(0).toElement().text();

                if (urlPart.isEmpty()) {
                    continue;
                }

                QUrl calendarItemUrl = QUrl(todoCalendarServerUrlWithoutPath % urlPart);

                // check if we have an etag
                QDomNodeList etagNodes =
                    elem.elementsByTagNameNS(NS_DAV, QStringLiteral("getetag"));
                if (etagNodes.length()) {
                    QString etag = etagNodes.at(0).toElement().text();
                    etag.replace(QStringLiteral("\""), QString());
                    //                    qDebug() << __func__ << " - 'etag': "
                    //                    << etag;

                    // check if we have a last modified date
                    QDomNodeList lastModifiedNodes =
                        elem.elementsByTagNameNS(NS_DAV, QStringLiteral("getlastmodified"));
                    if (lastModifiedNodes.length()) {
                        const QString lastModified = lastModifiedNodes.at(0).toElement().text();
                        bool fetchItem = false;

                        // try to fetch the calendar item by url
                        CalendarItem calItem = CalendarItem::fetchByUrl(calendarItemUrl);
                        if (calItem.isFetched()) {
                            // check if calendar item was modified
                            if (calItem.getETag() != etag) {
                                // store etag and last modified date
                                calItem.setETag(etag);
                                calItem.setLastModifiedString(lastModified);
                                calItem.store();

                                // we want to update the item from server
                                fetchItem = true;
                            }
                        } else {
                            // calendar item was not found
                            // create calendar item for fetching
                            CalendarItem::addCalendarItemForRequest(calendarName, calendarItemUrl,
                                                                    etag, lastModified);
                            fetchItem = true;
                        }

                        // remove the url from the list of calendar item urls
                        // to remove
                        if (calendarItemUrlRemoveList.contains(calendarItemUrl)) {
                            calendarItemUrlRemoveList.removeAll(calendarItemUrl);
                        }

                        // fetch the calendar item
                        if (fetchItem) {
                            QNetworkRequest r(calendarItemUrl);
                            addCalendarAuthHeader(&r);

                            auto *calNetworkManager = new QNetworkAccessManager(this);

                            QObject::connect(
                                calNetworkManager,
                                SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),
                                this,
                                SLOT(slotCalendarAuthenticationRequired(QNetworkReply *,
                                                                        QAuthenticator *)));

                            // Connect the finished signal to a lambda function
                            connect(
                                calNetworkManager, &QNetworkAccessManager::finished,
                                [=](QNetworkReply *reply) {
                                    QUrl url = reply->url();

                                    if (reply->error() == QNetworkReply::NoError) {
                                        QByteArray responseData = reply->readAll();
                                        QString data = QString(responseData);
                                        qDebug() << "Reply from ownCloud calendar item ics page "
                                                    "after GET"
                                                 << calendarItemUrl;
                                        // qDebug() << data;

                                        // Workaround for a ownCloud error message
                                        if (data.indexOf(QStringLiteral(
                                                "<s:message>Unable to generate a URL for the named"
                                                " route \"tasksplus.page.index\" as such route"
                                                " does not exist.</s:message>")) > 20) {
                                            data = QString();
                                        }

#ifndef INTEGRATION_TESTS
                                        if (todoDialog != nullptr) {
                                            // This will mostly happen after the PUT request to
                                            // update or create a task item
                                            if (data.isEmpty()) {
                                                // Reload the task list from server
                                                todoDialog->reloadTodoList();
                                            }

                                            // Increment the progress bar
                                            todoDialog->todoItemLoadingProgressBarIncrement();
                                        }
#endif

                                        // Fetch the calendar item, that was already stored by
                                        // loadTodoItems()
                                        CalendarItem calItem = CalendarItem::fetchByUrlAndCalendar(
                                            url.toString(), calendarName);
                                        if (calItem.isFetched()) {
                                            // Update the item with the ics data
                                            bool wasUpdated = calItem.updateWithICSData(data);

                                            // If item wasn't updated (for example because it was no
                                            // VTODO item) we will remove it
                                            if (!wasUpdated) {
                                                calItem.remove();
                                            } else if (todoDialog != nullptr) {
#ifndef INTEGRATION_TESTS
                                                // reload the task list items
                                                todoDialog->reloadTodoListItems();
#endif
                                            }
                                        }
                                    } else {
                                        // For error codes see
                                        // http://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum
                                        qWarning() << "QNetworkReply error " +
                                                          QString::number(reply->error()) +
                                                          " from url " + url.toString() + ":"
                                                   << reply->errorString();
                                    }

                                    reply->deleteLater();
                                    calNetworkManager->deleteLater();
                                });

                            QNetworkReply *reply = calNetworkManager->get(r);
                            ignoreSslErrorsIfAllowed(reply);

                            requestCount++;
                        }
                    }
                }
            }
        }
    }

#ifndef INTEGRATION_TESTS
    if (todoDialog != nullptr) {
        // set the real maximum of the progress bar
        todoDialog->todoItemLoadingProgressBarSetMaximum(requestCount);

        // hide progress bar if there were no updates
        if (requestCount == 0) {
            todoDialog->todoItemLoadingProgressBarHide();
        }
    }
#endif

    // remove all not found items
    for (int i = 0; i < calendarItemUrlRemoveList.length(); ++i) {
        const QUrl &url = calendarItemUrlRemoveList.at(i);
        CalendarItem calItem = CalendarItem::fetchByUrl(url);

        if (calItem.isFetched()) {
            calItem.remove();
        }
    }

#ifndef INTEGRATION_TESTS
    if (todoDialog != nullptr) {
        // reload the existing items
        todoDialog->reloadTodoListItems();
    }
#endif
}

/**
 * Updates the share status of the notes
 *
 * @param data
 */
void OwnCloudService::handleNoteShareReply(QString &data) {
    // return if we didn't get any data
    if (data.isEmpty()) {
        return;
    }

    updateNoteShareStatusFromShare(data);
    updateNoteShareStatusFromFetchAll(data);
}

/**
 * Updates the share status of the note
 */
void OwnCloudService::handleUpdateNoteShareReply(const QString &urlPart, const QString &data) {
    // return if we didn't get any data
    if (data.isEmpty()) {
        return;
    }

    qDebug() << __func__ << " - 'data': " << data;

    QRegularExpression re(QRegularExpression::escape(sharePath) % QStringLiteral("\\/(\\d+)$"));

    QRegularExpressionMatch match = re.match(urlPart);
    int shareId = match.hasMatch() ? match.captured(1).toInt() : 0;
    qDebug() << __func__ << " - 'shareId': " << shareId;

    if (shareId == 0) {
        return;
    }

    Note note = Note::fetchByShareId(shareId);
    qDebug() << __func__ << " - 'note': " << note;

    if (!note.isFetched()) {
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(data)) {
        qCritical() << __func__ << " - 'doc.setContent(data)' failed";
        return;
    }

    QString status = doc.firstChildElement(QStringLiteral("ocs"))
                         .firstChildElement(QStringLiteral("meta"))
                         .firstChildElement(QStringLiteral("status"))
                         .text();

    qDebug() << __func__ << " - 'status': " << status;

    if (status.trimmed() != QStringLiteral("ok")) {
        QString message = doc.firstChildElement(QStringLiteral("ocs"))
                              .firstChildElement(QStringLiteral("meta"))
                              .firstChildElement(QStringLiteral("message"))
                              .text();
        showOwnCloudServerErrorMessage(message.trimmed());

        return;
    }

    QString permissions = doc.firstChildElement(QStringLiteral("ocs"))
                              .firstChildElement(QStringLiteral("data"))
                              .firstChildElement(QStringLiteral("permissions"))
                              .text();

    qDebug() << __func__ << " - 'permissions': " << permissions;

    // if permissions are empty we assume there was not "ocs/data", which means
    // the share was deleted
    if (permissions.trimmed().isEmpty()) {
        note.setShareUrl(QString());
        note.setShareId(0);
    }

    note.setSharePermissions(permissions.toInt());
    note.store();

#ifndef INTEGRATION_TESTS
    // update the share dialog to show the share url
    if (shareDialog != nullptr) {
        shareDialog->updateDialog();
    }
#endif
}

/**
 * Updates the share status of the notes
 *
 * @param data
 */
void OwnCloudService::updateNoteShareStatusFromShare(QString &data) {
    // return if we didn't get any data
    if (data.isEmpty()) {
        return;
    }

    qDebug() << __func__ << " - 'data': " << data;

    QDomDocument doc;
    if (!doc.setContent(data)) {
        qCritical() << __func__ << " - 'doc.setContent(data)' failed";
        return;
    }

    QString status = doc.firstChildElement(QStringLiteral("ocs"))
                         .firstChildElement(QStringLiteral("meta"))
                         .firstChildElement(QStringLiteral("status"))
                         .text();

    qDebug() << __func__ << " - 'status': " << status;

    if (status.trimmed() != QStringLiteral("ok")) {
        QString message = doc.firstChildElement(QStringLiteral("ocs"))
                              .firstChildElement(QStringLiteral("meta"))
                              .firstChildElement(QStringLiteral("message"))
                              .text();

        showOwnCloudServerErrorMessage(message.trimmed());
        return;
    }

    QDomNodeList dataElements = doc.firstChildElement(QStringLiteral("ocs"))
                                    .firstChildElement(QStringLiteral("data"))
                                    .elementsByTagName(QStringLiteral("element"));

    if (dataElements.count() == 0) {
        dataElements =
            doc.firstChildElement(QStringLiteral("ocs")).elementsByTagName(QStringLiteral("data"));
    }

    updateNoteShareStatus(dataElements, true);
}

/**
 * Updates the share status of the notes
 *
 * @param data
 */
void OwnCloudService::updateNoteShareStatusFromFetchAll(QString &data) {
    // return if we didn't get any data
    if (data.isEmpty()) {
        return;
    }

    qDebug() << __func__ << " - 'data': " << data;

    QDomDocument doc;
    if (!doc.setContent(data)) {
        qCritical() << __func__ << " - 'doc.setContent(data)' failed";
        return;
    }

    QDomNodeList dataElements = doc.firstChildElement(QStringLiteral("ocs"))
                                    .firstChildElement(QStringLiteral("data"))
                                    .elementsByTagName(QStringLiteral("element"));

    if (dataElements.count() == 0) {
        dataElements =
            doc.firstChildElement(QStringLiteral("ocs")).elementsByTagName(QStringLiteral("data"));
    }

    updateNoteShareStatus(dataElements);
}

void OwnCloudService::updateNoteShareStatus(QDomNodeList &dataElements, bool updateShareDialog) {
    qDebug() << __func__ << " - 'dataElements.count()': " << dataElements.count();

    if (dataElements.isEmpty()) {
        return;
    }

    QString serverNotesPath = NoteFolder::currentRemotePath();

    for (int i = 0; i < dataElements.count(); i++) {
        QDomNode elm = dataElements.at(i);
        if (!elm.isElement()) {
            continue;
        }

        QString shareType = elm.firstChildElement(QStringLiteral("share_type")).text();

        qDebug() << __func__ << " - 'shareType': " << shareType;

        // we only want public shares
        if (shareType.trimmed() != QStringLiteral("3")) {
            continue;
        }

        QString itemType = elm.firstChildElement(QStringLiteral("item_type")).text();

        qDebug() << __func__ << " - 'itemType': " << itemType;

        // we only want file shares
        if (itemType.trimmed() != QStringLiteral("file")) {
            continue;
        }

        QString path = elm.firstChildElement(QStringLiteral("path")).text().trimmed();

        // we only want shares in our note folder
        if (!path.startsWith(serverNotesPath)) {
            continue;
        }

        // remove the note path from the path
        path = Utils::Misc::removeIfStartsWith(std::move(path), serverNotesPath);

        QFileInfo fileInfo(path);
        QString fileName = fileInfo.fileName();
        QString fileParentPath = fileInfo.dir().path();
        if (fileParentPath == QStringLiteral(".")) {
            fileParentPath = QString();
        }

        // fetch the note sub folder of the note
        NoteSubFolder noteSubFolder =
            NoteSubFolder::fetchByPathData(std::move(fileParentPath), QStringLiteral("/"));

        // fetch the note
        Note note = Note::fetchByFileName(fileName, noteSubFolder.getId());
        //        qDebug() << __func__ << " - 'note': " << note;

        // store the share url for the note
        if (note.isFetched()) {
            // get the share id
            int id = elm.firstChildElement(QStringLiteral("id")).text().trimmed().toInt();
            note.setShareId(id);

            // get the share url
            QString url = elm.firstChildElement(QStringLiteral("url")).text().trimmed();
            note.setShareUrl(url);

            // get the share permissions
            int permissions =
                elm.firstChildElement(QStringLiteral("permissions")).text().trimmed().toInt();
            note.setSharePermissions(permissions);

            note.store();

#ifndef INTEGRATION_TESTS
            // update the share dialog to show the share url
            if (updateShareDialog && (shareDialog != nullptr)) {
                shareDialog->updateDialog();
            }
#else
            Q_UNUSED(updateShareDialog);
#endif
        }
    }
}

void OwnCloudService::loadDirectory(QString &data) {
    QDomDocument doc;
    doc.setContent(data, true);

    if (data.isEmpty()) {
        showOwnCloudServerErrorMessage(QString(), false);
    }

    QStringList pathList;
    QDomNodeList responseNodes = doc.elementsByTagNameNS(NS_DAV, QStringLiteral("response"));

    for (int i = 0; i < responseNodes.count(); i++) {
        QDomNode responseNode = responseNodes.at(i);
        if (responseNode.isElement()) {
            QDomElement elem = responseNode.toElement();

            bool isFolder = false;
            QDomNodeList resourceTypeNodes =
                elem.elementsByTagNameNS(NS_DAV, QStringLiteral("resourcetype"));
            if (resourceTypeNodes.length()) {
                QDomNodeList typeNodes = resourceTypeNodes.at(0).childNodes();
                for (int j = 0; j < typeNodes.length(); ++j) {
                    QDomNode typeNode = typeNodes.at(j);
                    QString typeString = typeNode.toElement().tagName();

                    if (typeString == QStringLiteral("collection")) {
                        isFolder = true;
                        break;
                    }
                }
            }

            if (!isFolder) {
                continue;
            }

            // check if we have an url
            QDomNodeList urlPartNodes = elem.elementsByTagNameNS(NS_DAV, QStringLiteral("href"));
            if (urlPartNodes.length()) {
                QString urlPart = urlPartNodes.at(0).toElement().text();

                QRegularExpression re(QRegularExpression::escape(webdavPath()) %
                                      QStringLiteral("\\/(.+)\\/$"));

                QRegularExpressionMatch match = re.match(urlPart);
                QString folderString = match.hasMatch() ? match.captured(1) : QString();

                if (!folderString.isEmpty()) {
                    pathList << QUrl::fromPercentEncoding(folderString.toUtf8());
                }
            }
        }
    }

#ifndef INTEGRATION_TESTS
    if (settingsDialog) {
        settingsDialog->setNoteFolderRemotePathList(pathList);
    }
#endif
}

/**
 * Updates a calendar item on the CalDAV server
 *
 * @param calendarItem
 * @param dialog
 */
void OwnCloudService::postCalendarItemToServer(CalendarItem calendarItem, TodoDialog *dialog) {
    this->todoDialog = dialog;

    calendarItem.generateNewICSData();

    QUrl url(calendarItem.getUrl());
    QNetworkRequest r;
    addCalendarAuthHeader(&r);
    r.setUrl(url);

    // build the request body
    QString body = calendarItem.getICSData();

    qDebug() << __func__ << " - 'body': " << body;

    auto dataToSend = new QByteArray(body.toUtf8());
    r.setHeader(QNetworkRequest::ContentLengthHeader, dataToSend->size());
    r.setHeader(QNetworkRequest::ContentTypeHeader,
                QStringLiteral("application/x-www-form-urlencoded"));
    auto *buffer = new QBuffer(dataToSend);
    auto *calNetworkManager = new QNetworkAccessManager(this);

    QObject::connect(calNetworkManager,
                     SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), this,
                     SLOT(slotCalendarAuthenticationRequired(QNetworkReply *, QAuthenticator *)));

    // Connect the finished signal to a lambda function
    connect(calNetworkManager, &QNetworkAccessManager::finished, [=](QNetworkReply *reply) {
        if (reply->error() == QNetworkReply::NoError) {
            qDebug() << "Reply from calendar page after PUT" << url;

#ifndef INTEGRATION_TESTS
            if (todoDialog != nullptr) {
                // Reload the task list from server
                todoDialog->reloadTodoList();
            }
#endif
        } else {
            // For error codes see http://doc.qt.io/qt-5/qnetworkreply.html#NetworkError-enum
            qWarning() << "QNetworkReply error " + QString::number(reply->error()) + " from url " +
                              url.toString() + ":"
                       << reply->errorString();
        }

        reply->deleteLater();
        calNetworkManager->deleteLater();
    });

    QNetworkReply *reply = calNetworkManager->sendCustomRequest(r, "PUT", buffer);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * Loads and updates the icsData of a calendar item with data from the
 * calendar server
 *
 * @return
 */
bool OwnCloudService::updateICSDataOfCalendarItem(CalendarItem *calItem) {
    auto *manager = new QNetworkAccessManager(this);

    QUrl url(calItem->getUrl());
    QNetworkRequest r;

    addCalendarAuthHeader(&r);
    r.setUrl(url);

    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(manager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));

    // 5 sec timeout for the request
    timer.start(5000);

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    r.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    r.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

    QNetworkReply *reply = manager->get(r);
    ignoreSslErrorsIfAllowed(reply);

    loop.exec();
    bool result = false;

    if (timer.isActive()) {
        // get the text from the network reply
        QString icsData = reply->readAll();

        // set the new ics data
        calItem->setICSData(icsData);

        result = true;
    }

    reply->deleteLater();
    delete (manager);

    // if false: timer elapsed, no reply from network request
    return result;
}

/**
 * Gets the file list from the ownCloud server
 * for the settings dialog
 */
void OwnCloudService::settingsGetFileList(SettingsDialog *dialog, const QString &path) {
    settingsDialog = dialog;

    QUrl url(serverUrl % webdavPath() % QStringLiteral("/") % path);
    QNetworkRequest r(url);

    // Keep in mind that we need to clear the cookie jar in
    // OwnCloudService::instance or else we will get an error
    // "CSRF check not passed." from Nextcloud
    // Really resetting the instance in OwnCloudService::instance also helped
    addAuthHeader(&r);

    // build the request body
    QString body = QStringLiteral(
        "<?xml version=\"1.0\"?>"
        "<a:propfind xmlns:a=\"DAV:\">"
        "<a:prop><a:resourcetype />"
        "</a:prop>"
        "</a:propfind>");

    auto dataToSend = new QByteArray(body.toUtf8());
    r.setHeader(QNetworkRequest::ContentLengthHeader, dataToSend->size());
    r.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/xml"));
    auto *buffer = new QBuffer(dataToSend);

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QNetworkReply *reply = networkManager->sendCustomRequest(r, "PROPFIND", buffer);
    ignoreSslErrorsIfAllowed(reply);
}

void OwnCloudService::handleImportBookmarksReply(QString &data) {
    qDebug() << __func__ << " - 'data': " << data;

    // we have to add [], so the string can be parsed as JSON
    data = QStringLiteral("[") % data % QStringLiteral("]");

    QJSEngine engine;
    QJSValue result = engine.evaluate(data);

    QJSValue bookmarks = result.property(0).property(QStringLiteral("data"));

    // check if we got no useful data
    if (bookmarks.toString().isEmpty()) {
        QMessageBox::information(0, tr("No bookmarks"),
                                 tr("No bookmarks were found on the server."));
        return;
    }

#ifndef INTEGRATION_TESTS
    auto *dialog = new ServerBookmarksImportDialog(bookmarks);
    dialog->exec();
#endif
}

/**
 * Downloads an image from a Nextcloud preview path and returns the data
 *
 * @param url
 * @return {QByteArray} the content of the downloaded url
 */
QByteArray OwnCloudService::downloadNextcloudPreviewImage(const QString &path) {
    auto *manager = new QNetworkAccessManager(this);
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QUrl url(serverUrl % path);
    qDebug() << __func__ << " - 'url': " << url;

    QNetworkRequest networkRequest = QNetworkRequest(url);
    addAuthHeader(&networkRequest);

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

    QByteArray data;
    QNetworkReply *reply = manager->get(networkRequest);
    ignoreSslErrorsIfAllowed(reply);
    loop.exec();

    // if we didn't get a timeout let us return the content
    if (timer.isActive()) {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // only get the data if the status code was "success"
        // see: https://en.wikipedia.org/wiki/List_of_HTTP_status_codes
        if (statusCode >= 200 && statusCode < 300) {
            // get the data from the network reply
            data = reply->readAll();
        }
    }

    reply->deleteLater();
    delete (manager);

    return data;
}

/**
 * Transforms a Nextcloud preview image tag to an inline image tag
 *
 * @param data
 * @param imageSuffix
 * @return
 */
QString OwnCloudService::nextcloudPreviewImageTagToInlineImageTag(QString imageTag,
                                                                  int &imageWidth) {
    imageTag.replace(QStringLiteral("&amp;"), QStringLiteral("&"));
    //    qDebug() << __func__ << " - 'imageTag': " << imageTag;

    QRegularExpression re(
        QStringLiteral(
            R"(<img src=\"(\/core\/preview\?fileId=(\d+)&x=(\d+)&y=(\d+)&a=(\w+)#mimetype=([\w\d%]+)&.+)\" alt=\"(.+)\"\/?>)"),
        QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatch match = re.match(imageTag);
    if (!match.hasMatch()) {
        return imageTag;
    }

    QString path = match.captured(1);
    QString mimeType = QUrl::fromPercentEncoding(match.captured(6).toLatin1());
    QString alt = match.captured(7);

    QByteArray data = downloadNextcloudPreviewImage(path);
    auto image = QImage::fromData(data);
    imageWidth = image.width();

    // for now we do no caching, because we don't know when to invalidate the
    // cache
    QString inlineImageTag =
        QStringLiteral(R"(<img class="remote-img" src="data:)") % mimeType %
            QStringLiteral(";base64,") +
        data.toBase64() % QStringLiteral("\" alt=\"") % alt % QStringLiteral("\"/>");

    return inlineImageTag;
}

/**
 * Initiates the Login flow v2 for Nextcloud
 * https://docs.nextcloud.com/server/latest/developer_manual/client_apis/LoginFlow/index.html#login-flow-v2
 *
 * @param serverUrl
 */
bool OwnCloudService::initiateLoginFlowV2(const QString &serverUrl, QJsonObject &pollData) {
    const QString loginFlowUrl = serverUrl + "/index.php/login/v2";
    auto data = Utils::Misc::downloadUrl(loginFlowUrl, true);

    if (data.isEmpty() || !data.startsWith('{')) {
        QMessageBox::warning(
            nullptr, QObject::tr("Login flow failed"),
            QObject::tr("Nextcloud login flow v2 could not be initiated. "
                        "Make sure the server url is correct and you are using a "
                        "Nextcloud server!<br /><br />If your are still having issues "
                        "please create an app password by hand on your server's admin page."));

        return false;
    }

    // Parse data
    auto jsonObject = QJsonDocument::fromJson(data).object();
    auto loginUrl = jsonObject.value(QStringLiteral("login")).toString();

    if (loginUrl.isEmpty()) {
        QMessageBox::warning(nullptr, QObject::tr("Login flow failed"),
                             QObject::tr("Could not parse login url!"));

        return false;
    }

    // Open browser with login url
    QDesktopServices::openUrl(loginUrl);

    pollData = jsonObject.value(QStringLiteral("poll")).toObject();

    return true;
}

/**
 * Fetches the Nextcloud Account Id (to be used as WebDav username) as part of the login flow v2
 * https://docs.nextcloud.com/server/latest/developer_manual/client_apis/LoginFlow/index.html#obtaining-the-login-credentials
 *
 * @param serverUrl
 * @param userName
 * @param password
 * @return
 */
QString OwnCloudService::fetchNextcloudAccountId(const QString &serverUrl, const QString &userName,
                                                 const QString &password) {
    auto *manager = new QNetworkAccessManager();
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    QObject::connect(manager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));

    // 5 sec timeout for the request
    timer.start(5000);

    QUrl url(serverUrl % QStringLiteral("/ocs/v1.php/cloud/user"));

    //    qDebug() << __func__ << " - 'url': " << url;
    //    qDebug() << __func__ << " - 'userName': " << userName;
    //    qDebug() << __func__ << " - 'password': " << password;

    QNetworkRequest networkRequest = QNetworkRequest(url);
    addGenericAuthHeader(&networkRequest, userName, password);

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    manager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QNetworkReply *reply = manager->get(networkRequest);
    ignoreSslErrorsIfAllowed(reply);
    loop.exec();

    // if we didn't get a timeout let us return the content
    if (timer.isActive()) {
        if (reply->error() != QNetworkReply::NoError) {
            qDebug() << __func__ << " - 'reply->error()': " << reply->error();
            qDebug() << __func__ << " - 'reply->errorString()': " << reply->errorString();
            qDebug() << __func__ << " - 'reply->readAll()': " << reply->readAll();

            return {};
        }

        QString data = QString(reply->readAll());
        //        qDebug() << __func__ << " - 'data': " << data;

        QDomDocument doc;
        if (!doc.setContent(data)) {
            qCritical() << __func__ << " - 'doc.setContent(data)' failed";
            return {};
        }

        QString id = doc.firstChildElement(QStringLiteral("ocs"))
                         .firstChildElement(QStringLiteral("data"))
                         .firstChildElement(QStringLiteral("id"))
                         .text()
                         .trimmed();

        qDebug() << __func__ << " - 'id': " << id;

        return id;
    }

    reply->deleteLater();
    delete (manager);

    return {};
}

void OwnCloudService::unsetShareDialog() { shareDialog = nullptr; }
