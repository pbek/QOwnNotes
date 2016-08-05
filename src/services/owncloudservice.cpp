#include "mainwindow.h"
#include "services/owncloudservice.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/trashdialog.h"
#include "dialogs/versiondialog.h"
#include <QSettings>
#include <QDebug>
#include <QUrlQuery>
#include <QDir>
#include <QMessageBox>
#include <QDomDocument>
#include <QDomNodeList>
#include <QBuffer>
#include <QEventLoop>
#include <QTimer>
#include <utils/misc.h>
#include <QJSEngine>
#include <QJSValueIterator>
#include <QCoreApplication>
#include <QXmlQuery>
#include <QXmlResultItems>
#include "libraries/versionnumber/versionnumber.h"
#include "entities/calendaritem.h"
#include "cryptoservice.h"

const QString OwnCloudService::rootPath =
        "/index.php/apps/qownnotesapi/api/v1/";
const QString OwnCloudService::format = "json";
const QString NS_DAV("DAV:");

OwnCloudService::OwnCloudService(QObject *parent)
        : QObject(parent) {
    readSettings();
    settingsDialog = Q_NULLPTR;
    todoDialog = Q_NULLPTR;
}

void OwnCloudService::readSettings() {
    QSettings settings;
    serverUrl = settings.value("ownCloud/serverUrl").toString();
    serverUrlPath = QUrl(serverUrl).path();

    serverUrlWithoutPath = serverUrl;
    if (serverUrlPath != "") {
        // remove the path from the server url
        serverUrlWithoutPath.replace(QRegularExpression(
                QRegularExpression::escape(serverUrlPath) + "$"), "");
    }

    userName = settings.value("ownCloud/userName").toString();
    password = CryptoService::instance()->decryptToString(
            settings.value("ownCloud/password").toString());

    networkManager = new QNetworkAccessManager();

    versionListPath = rootPath + "note/versions";
    trashListPath = rootPath + "note/trashed";
    appInfoPath = rootPath + "note/app_info";
    capabilitiesPath = "/ocs/v1.php/cloud/capabilities";
    ownCloudTestPath = "/ocs/v1.php";
    restoreTrashedNotePath = rootPath + "note/restore_trashed";
    webdavPath = "/remote.php/webdav";
    sharePath = "/ocs/v1.php/apps/files_sharing/api/v1/shares";

    int calendarBackend = settings.value(
            "ownCloud/todoCalendarBackend").toInt();
    QString calendarBackendString =
            calendarBackend == CalendarPlus ? "calendarplus" : "caldav";
    calendarPath =
            "/remote.php/" + calendarBackendString + "/calendars/" + userName;

    QObject::connect(networkManager,
                     SIGNAL(authenticationRequired(QNetworkReply * ,
                                                   QAuthenticator *)), this,
                     SLOT(slotAuthenticationRequired(QNetworkReply * ,
                                                     QAuthenticator *)));
    QObject::connect(networkManager, SIGNAL(finished(QNetworkReply *)), this,
                     SLOT(slotReplyFinished(QNetworkReply *)));
}

void OwnCloudService::slotAuthenticationRequired(
        QNetworkReply *reply, QAuthenticator *authenticator) {
    Q_UNUSED(authenticator);
    qWarning() << "Username and/or password incorrect";

    if (settingsDialog != Q_NULLPTR) {
        settingsDialog->setOKLabelData(3, "incorrect", SettingsDialog::Failure);
        settingsDialog->setOKLabelData(4, "not connected", SettingsDialog::Failure);
    }

    reply->abort();
}

void OwnCloudService::slotReplyFinished(QNetworkReply *reply) {
    qDebug() << "Reply from " << reply->url().path();
    // qDebug() << reply->errorString();

    // this should only be called from the settings dialog
    if (reply->url().path().endsWith(appInfoPath)) {
        qDebug() << "Reply from app info";

        // check if everything is all right and call the callback method
        checkAppInfo(reply);

        return;
    } else {
        QByteArray arr = reply->readAll();
        QString data = QString(arr);

        if (reply->url().path().endsWith(versionListPath)) {
            qDebug() << "Reply from version list";
            // qDebug() << data;

            // handle the versions loading
            handleVersionsLoading(data);
            return;
        } else if (reply->url().path().endsWith(trashListPath)) {
            qDebug() << "Reply from trash list";
            // qDebug() << data;

            // handle the loading of trashed notes
            handleTrashedLoading(data);
            return;
        } else if (reply->url().path().endsWith(capabilitiesPath)) {
            qDebug() << "Reply from capabilities page";

            if (data.startsWith("<?xml version=")) {
                settingsDialog->setOKLabelData(3, "ok", SettingsDialog::OK);
                settingsDialog->setOKLabelData(1, "ok", SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(3, "not correct",
                                               SettingsDialog::Failure);
            }

            return;
        } else if (reply->url().path().endsWith(ownCloudTestPath)) {
            qDebug() << "Reply from ownCloud test page";

            if (data.startsWith("<?xml version=")) {
                settingsDialog->setOKLabelData(2, "ok", SettingsDialog::OK);
                settingsDialog->setOKLabelData(1, "ok", SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(2, "not detected",
                                               SettingsDialog::Failure);
            }

            return;
        } else if (reply->url().path().endsWith(restoreTrashedNotePath)) {
            qDebug() << "Reply from ownCloud restore trashed note page";
            // qDebug() << data;

            return;
        } else if (reply->url().path().endsWith(serverUrlPath + calendarPath)) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
            qInfo() << "Reply from ownCloud calendar page: " << data;
#else
            qDebug() << "Reply from ownCloud calendar page" << data;
#endif

            QStringList calendarHrefList = parseCalendarHrefList(data);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 5, 0))
            qInfo() << "calendarHrefList: " << calendarHrefList;
#else
            qDebug() << "calendarHrefList: " << calendarHrefList;
#endif

            settingsDialog->refreshTodoCalendarList(calendarHrefList);
            return;
        } else if (reply->url().path()
                .startsWith(serverUrlPath + calendarPath)) {
            // check if we have a reply from a calendar item request
            if (reply->url().path().endsWith(".ics")) {
                qDebug() << "Reply from ownCloud calendar item ics page";
                // qDebug() << data;

                // a workaround for a ownCloud error message
                if (data.indexOf(
                        "<s:message>Unable to generate a URL for the named"
                                " route \"tasksplus.page.index\" as such route"
                                " does not exist.</s:message>") >
                    20) {
                    data = "";
                }

                if (todoDialog != Q_NULLPTR) {
                    // this will mostly happen after the PUT request to update
                    // or create a todo item
                    if (data == "") {
                        // reload the todo list from server
                        todoDialog->reloadTodoList();
                    }

                    // increment the progress bar
                    todoDialog->todoItemLoadingProgressBarIncrement();
                }

                // fetch the calendar item, that was already stored
                // by loadTodoItems()
                CalendarItem calItem = CalendarItem::fetchByUrlAndCalendar(
                        reply->url().toString(), calendarName);
                if (calItem.isFetched()) {
                    // update the item with the ics data
                    bool wasUpdated = calItem.updateWithICSData(data);

                    // if item wasn't updated (for example because it was no
                    // VTODO item) we will remove it
                    if (!wasUpdated) {
                        calItem.remove();
                        return;
                    }

                    if (todoDialog != Q_NULLPTR) {
                        // reload the todo list items
                        todoDialog->reloadTodoListItems();
                    }

//                    qDebug() << __func__ << " - 'calItem': " << calItem;
                }
            } else {
                // this should be the reply of a calendar item list request
                qDebug() << "Reply from ownCloud calendar todo list page";
//                qDebug() << data;

                // load the Todo items
                loadTodoItems(data);
            }
        } else if (reply->url().path()
            .startsWith(serverUrlPath + webdavPath)) {
            // this should be the reply of a calendar item list request
            qDebug() << "Reply from ownCloud webdav";

            // load the directories
            loadDirectory(data);

            return;
        } else if (reply->url().path().endsWith(sharePath)) {
            qDebug() << "Reply from share api";

            qDebug() << __func__ << " - 'data': " << data;

            // update the share status of the notes
            updateNoteShareStatus(data);

            return;
        } else if (reply->url().toString() == serverUrl) {
            qDebug() << "Reply from main server url";

            if (data != "") {
                settingsDialog->setOKLabelData(1, "ok", SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(1, "not found",
                                               SettingsDialog::Failure);
            }

            return;
        }
    }
}

void OwnCloudService::checkAppInfo(QNetworkReply *reply) {
    QString data = QString(reply->readAll());
    // qDebug() << data;

    // we have to add [], so the string can be parsed as JSON
    data = QString("[") + data + QString("]");

    QJSEngine engine;
    QJSValue result = engine.evaluate(data);

    bool appIsValid = result.property(0).property("versioning").toBool();
    QString appVersion = result.property(0).property("app_version")
            .toVariant().toString();
    QString serverVersion = result.property(0).property("server_version")
            .toVariant().toString();

    // reset to "unknown" in case we can't test if versions
    // and trash app are enabled
    settingsDialog->setOKLabelData(6, "unknown", SettingsDialog::Unknown);
    settingsDialog->setOKLabelData(7, "unknown", SettingsDialog::Unknown);

    if (serverVersion != "") {
        VersionNumber serverAppVersion = VersionNumber(appVersion);
        VersionNumber minAppVersion = VersionNumber(QOWNNOTESAPI_MIN_VERSION);

        if (minAppVersion > serverAppVersion) {
            settingsDialog->setOKLabelData(4,
                                           "version " + appVersion + " too low",
                                           SettingsDialog::Warning);
        } else {
            settingsDialog->setOKLabelData(4, "ok", SettingsDialog::OK);
        }

        // check if versions and trash app are enabled after QOwnNotesAPI v0.3.1
        if (serverAppVersion >= VersionNumber("0.3.1")) {
            bool versionsAppEnabled = result.property(0).property(
                    "versions_app").toBool();
            bool trashAppEnabled = result.property(0).property(
                    "trash_app").toBool();

            if (versionsAppEnabled) {
                settingsDialog->setOKLabelData(6, "ok", SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(6, "not enabled",
                                               SettingsDialog::Failure);
            }

            if (trashAppEnabled) {
                settingsDialog->setOKLabelData(7, "ok", SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(7, "not enabled",
                                               SettingsDialog::Failure);
            }
        }

        // check if notes path was found after QOwnNotesAPI v0.4.
        if (serverAppVersion >= VersionNumber("0.4.1")) {
            bool notesPathExists = result.property(0).property(
                    "notes_path_exists").toBool();

            if (notesPathExists) {
                settingsDialog->setOKLabelData(8, "ok", SettingsDialog::OK);
            } else {
                settingsDialog->setOKLabelData(8, "not found",
                                               SettingsDialog::Failure);
            }
        }
    } else {
        settingsDialog->setOKLabelData(4, "not connected",
                                       SettingsDialog::Failure);
    }

    // call callback in settings dialog
    settingsDialog->connectTestCallback(appIsValid, appVersion, serverVersion,
                                        reply->errorString());
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

    // direct server url request without auth header
    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);

    QUrlQuery q;
    q.addQueryItem("format", format);
    url.setQuery(q);

    addAuthHeader(&r);

    url.setUrl(serverUrl + capabilitiesPath);
    r.setUrl(url);
    reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);

    url.setUrl(serverUrl + ownCloudTestPath);
    r.setUrl(url);
    reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);

    url.setUrl(serverUrl + appInfoPath);
    QString serverNotesPath = NoteFolder::currentRemotePath();
    q.addQueryItem("notes_path", serverNotesPath);
    url.setQuery(q);
    r.setUrl(url);
    reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * Ignores ssl errors for a QNetworkReply if allowed
 */
void OwnCloudService::ignoreSslErrorsIfAllowed(QNetworkReply *reply) {
    QSettings settings;
    if (settings.value("networking/ignoreSSLErrors", true).toBool()) {
        QObject::connect(reply, SIGNAL(sslErrors(QList<QSslError>)), reply,
                         SLOT(ignoreSslErrors()));
    }
}

/**
 * Gets the calendar list from the ownCloud server
 * for the settings dialog
 */
void OwnCloudService::settingsGetCalendarList(SettingsDialog *dialog) {
    settingsDialog = dialog;

    QUrl url(serverUrl + calendarPath);
    QNetworkRequest r(url);
    addAuthHeader(&r);

    // build the request body
    QString body = "<d:propfind xmlns:d=\"DAV:\" "
            "xmlns:cs=\"http://sabredav.org/ns\" xmlns:c=\"urn:ietf:params:xml:ns:caldav\"> \
            <d:prop> \
               <d:resourcetype /> \
               <d:displayname /> \
               <cs:getctag /> \
               <c:supported-calendar-component-set /> \
            </d:prop> \
          </d:propfind>";

    QByteArray *dataToSend = new QByteArray(body.toUtf8());
    r.setHeader(QNetworkRequest::ContentLengthHeader, dataToSend->size());
    QBuffer *buffer = new QBuffer(dataToSend);

    QNetworkReply *reply = networkManager->sendCustomRequest(r, "PROPFIND",
                                                             buffer);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * @brief Gets the todo list from the ownCloud server for the todo list dialog
 */
void OwnCloudService::todoGetTodoList(QString calendarName,
                                      TodoDialog *dialog) {
    this->todoDialog = dialog;
    this->calendarName = calendarName;

    QSettings settings;
    QStringList todoCalendarEnabledList = settings.value(
            "ownCloud/todoCalendarEnabledList").toStringList();
    int index = todoCalendarEnabledList.indexOf(calendarName);

    // return if we did't find the calendar, this should not happen
    if (index == -1) {
        return;
    }

    QStringList todoCalendarEnabledUrlList = settings.value(
            "ownCloud/todoCalendarEnabledUrlList").toStringList();

    // return if there are to few items in the url list
    if (todoCalendarEnabledUrlList.size() < todoCalendarEnabledList.size()) {
        return;
    }

    QString calendarUrl = settings.value(
            "ownCloud/todoCalendarEnabledUrlList").toStringList().at(index);

    QUrl url(calendarUrl);
    QNetworkRequest r(url);
    addAuthHeader(&r);

    // ownCloud needs depth to be set to 1
    r.setRawHeader(QByteArray("DEPTH"), QByteArray("1"));

    // build the request body, we only want VTODO items
    QString body = "<c:calendar-query xmlns:d=\"DAV:\" "
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
        </c:calendar-query>";

    QByteArray *dataToSend = new QByteArray(body.toUtf8());
    r.setHeader(QNetworkRequest::ContentLengthHeader, dataToSend->size());
    QBuffer *buffer = new QBuffer(dataToSend);

    QNetworkReply *reply = networkManager->sendCustomRequest(r, "REPORT",
                                                             buffer);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * Shares a note on ownCloud
 */
void OwnCloudService::shareNote(Note note) {
    qDebug() << __func__ << " - 'note': " << note;
}

/**
 * Fetches the shares from ownCloud
 *
 * To fetch the shares of all notes we have to fetch all shares from the
 * account and search for our note folder
 */
void OwnCloudService::fetchShares(QString path) {
    QUrl url(serverUrl + sharePath);

    if (!path.isEmpty()) {
        QUrlQuery q;
        q.addQueryItem("path", path);
        url.setQuery(q);
    }

    qDebug() << url;

    QNetworkRequest r(url);
    addAuthHeader(&r);

    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * @brief Removes a task list item from the ownCloud server
 */
void OwnCloudService::removeCalendarItem(CalendarItem calItem,
                                         TodoDialog *dialog) {
    this->todoDialog = dialog;
    this->calendarName = calendarName;

    QUrl url(calItem.getUrl());
    QNetworkRequest r(url);
    addAuthHeader(&r);

    QNetworkReply *reply = networkManager->sendCustomRequest(r, "DELETE");
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * @brief Restores a note on the server
 */
void OwnCloudService::restoreTrashedNoteOnServer(QString fileName,
                                                 int timestamp,
                                                 MainWindow *mainWindow) {
    this->mainWindow = mainWindow;

    QUrl url(serverUrl + restoreTrashedNotePath);
    QString serverNotesPath = Utils::Misc::appendIfDoesNotEndWith(
            NoteFolder::currentRemotePath() +
            NoteSubFolder::activeNoteSubFolder().relativePath("/"), "/");

    url.setUserName(userName);
    url.setPassword(password);

    QUrlQuery q;
    q.addQueryItem("format", format);
    q.addQueryItem("file_name", serverNotesPath + fileName);
    q.addQueryItem("timestamp", QString::number(timestamp));
    url.setQuery(q);

    qDebug() << url;

    QNetworkRequest r(url);
    addAuthHeader(&r);

    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * @brief OwnCloudService::loadVersions
 */
void OwnCloudService::loadVersions(QString fileName, MainWindow *mainWindow) {
    this->mainWindow = mainWindow;

    QUrl url(serverUrl + versionListPath);
    QString serverPath = NoteFolder::currentRemotePath() + fileName;
    qDebug() << __func__ << " - 'serverPath': " << serverPath;

    url.setUserName(userName);
    url.setPassword(password);

    QUrlQuery q;
    q.addQueryItem("format", format);
    q.addQueryItem("file_name", serverPath);
    url.setQuery(q);

    QNetworkRequest r(url);
    addAuthHeader(&r);

    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * @brief OwnCloudService::loadTrash
 */
void OwnCloudService::loadTrash(MainWindow *mainWindow) {
    this->mainWindow = mainWindow;

    QUrl url(serverUrl + trashListPath);
    QString serverNotesPath = NoteFolder::currentRemotePath() +
        NoteSubFolder::activeNoteSubFolder().relativePath("/");

    url.setUserName(userName);
    url.setPassword(password);

    QUrlQuery q;
    q.addQueryItem("format", format);
    q.addQueryItem("dir", serverNotesPath);

    QStringList customNoteFileExtensionList =
            Note::customNoteFileExtensionList();

    // add the custom note extensions
    if (customNoteFileExtensionList.count() > 0) {
        QListIterator<QString> itr(customNoteFileExtensionList);
        while (itr.hasNext()) {
            QString fileExtension = itr.next();
            q.addQueryItem("extensions[]", fileExtension);
        }
    }

    url.setQuery(q);

    // qDebug() << url;

    QNetworkRequest r(url);
    addAuthHeader(&r);

    QNetworkReply *reply = networkManager->get(r);
    ignoreSslErrorsIfAllowed(reply);
}

void OwnCloudService::addAuthHeader(QNetworkRequest *r) {
    if (r) {
        QString concatenated = userName + ":" + password;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        QString headerData = "Basic " + data;
        r->setRawHeader("Authorization", headerData.toLocal8Bit());
    }
}

/**
 * Checks if ownCloud settings are set
 */
bool OwnCloudService::hasOwnCloudSettings() {
    QSettings settings;
    QString serverUrl =
            settings.value("ownCloud/serverUrl").toString().trimmed();
    QString userName =
            settings.value("ownCloud/userName").toString().trimmed();
    QString password =
            settings.value("ownCloud/password").toString().trimmed();

    return !(serverUrl.isEmpty() || userName.isEmpty() || password.isEmpty());
}

/**
 * Shows a message dialog with a ownCloud server error
 */
void OwnCloudService::showOwnCloudServerErrorMessage(
        QString message, bool withSettingsButton) {
    QString headline = tr("ownCloud server connection error");
    QString text = message.isEmpty() ?
            "Cannot connect to your ownCloud server! "
            "Please check your ownCloud configuration." :
            tr("ownCloud server error: <strong>%1</strong><br />"
            "Please check your ownCloud configuration.").arg(message);

    if (withSettingsButton) {
        if (QMessageBox::warning(
                0, headline, text,
                tr("Open &settings"), tr("&Cancel"),
                QString::null, 0, 1) == 0) {
            if (mainWindow != NULL) {
                mainWindow->openSettingsDialog(SettingsDialog::OwnCloudTab);
            }
        }
    } else {
        QMessageBox::warning(0, headline, text);
    }
}

/**
 * Handles the versions loading
 *
 * @brief OwnCloudService::handleVersionsLoading
 * @param data
 */
void OwnCloudService::handleVersionsLoading(QString data) {
    mainWindow->enableShowVersionsButton();
    mainWindow->showStatusBarMessage(
            tr("done with loading note versions"), 2000);

    // check if we get any data at all
    if (data.isEmpty()) {
        showOwnCloudServerErrorMessage();
        return;
    }

    // we have to add [], so the string can be parsed as JSON
    data = QString("[") + data + QString("]");

    QJSEngine engine;
    QJSValue result = engine.evaluate(data);

    // get the information if versioning is available
    // we are casting to QVariant first because otherwise we might get a
    // "undefined" string if "message" is not set
    QString message = result.property(0).property("message").toVariant()
            .toString();

    // check if we got an error message
    if (!message.isEmpty()) {
        showOwnCloudServerErrorMessage(message);
        return;
    }

    // get the filename to check if everything is all right
    QString fileName = result.property(0).property("file_name").toVariant()
            .toString();

    // get the versions
    QJSValue versions = result.property(0).property("versions");
    QJSValueIterator versionsIterator(versions);

    // check if we got no useful data, we also need to do this to prevent crashs
    if (fileName.isEmpty() || !versionsIterator.hasNext() ||
            versions.toString().isEmpty()) {
        QMessageBox::information(
                0, tr("no other version"),
                tr("There are no other versions on the server for this note."));
        return;
    }

    VersionDialog *dialog = new VersionDialog(versions, mainWindow);
    dialog->exec();
}

/**
 * Handles the loading of trashed notes
 *
 * @brief OwnCloudService::handleTrashedLoading
 * @param data
 */
void OwnCloudService::handleTrashedLoading(QString data) {
    mainWindow->enableShowTrashButton();
    mainWindow->showStatusBarMessage(
            tr("done with loading trashed notes"), 2000);

    // check if we get any data at all
    if (data == "") {
        showOwnCloudServerErrorMessage();
        return;
    }

    // we have to add [], so the string can be parsed as JSON
    data = QString("[") + data + QString("]");

    QJSEngine engine;
    QJSValue result = engine.evaluate(data);

    // get a possible error messages
    // we are casting to QVariant first because otherwise we might get a
    // "undefined" string if "message" is not set
    QString message = result.property(0).property("message").toVariant()
            .toString();

    // check if we got an error message
    if (!message.isEmpty()) {
        showOwnCloudServerErrorMessage(message);
        return;
    }

    // get the directory to check if everything is all right
    QString directory = result.property(0).property("directory").toString();

    // check if we got no useful data
    if (directory == "") {
        showOwnCloudServerErrorMessage();
        return;
    }

    // get the notes
    QJSValue notes = result.property(0).property("notes");

    // check if we got no useful data
    if (notes.toString() == "") {
        QMessageBox::information(0, "no other version",
                                 "There are no trashed notes on the server.");
        return;
    }

    TrashDialog *dialog = new TrashDialog(notes, mainWindow);
    dialog->exec();
}

QStringList OwnCloudService::parseCalendarHrefList(QString &data) {
    QStringList resultList;

    // check if there was nothing returned at all from the CalDAV server
    if (data.isEmpty()) {
        QMessageBox::critical(
                0, tr("Error while loading todo lists!"),
                tr("Your ownCloud CalDAV server didn't reply anything!"));

        return resultList;
    }

    QDomDocument doc;
    doc.setContent(data, true);

    QDomNodeList errorNodes = doc.elementsByTagNameNS(NS_DAV, "error");

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
                    if (typeString == "message") {
                        QMessageBox::critical(
                                0, tr("Error while loading todo lists!"),
                                tr("Error message from your ownCloud "
                                           "CalDAV server: <strong>%1</strong>")
                                        .arg(typeNode.toElement().text()));

                        return resultList;
                    }
                }
            }
        }
    }

    // loop all response blocks
    QDomNodeList responseNodes = doc.elementsByTagNameNS(NS_DAV, "response");
    for (int i = 0; i < responseNodes.length(); ++i) {
        QDomNode responseNode = responseNodes.at(i);
        if (responseNode.isElement()) {
            QDomElement elem = responseNode.toElement();
            QDomNodeList resourceTypeNodes =
                    elem.elementsByTagNameNS(NS_DAV, "resourcetype");
            if (resourceTypeNodes.length()) {
                QDomNodeList typeNodes = resourceTypeNodes.at(0).childNodes();
                for (int j = 0; j < typeNodes.length(); ++j) {
                    QDomNode typeNode = typeNodes.at(j);
                    QString typeString = typeNode.toElement().tagName();

                    // did we find a calendar?
                    // ideally we should check the
                    // "supported-calendar-component-set" for "VTODO"
                    if (typeString == "calendar") {
                        // add the href to our result list
                        QDomNodeList hrefNodes = elem.elementsByTagNameNS(
                                NS_DAV, "href");
                        if (hrefNodes.length()) {
                            const QString href = hrefNodes.at(
                                    0).toElement().text();
                            resultList << href;
                        }

                        QDomNodeList displayNameNodes =
                                elem.elementsByTagNameNS(NS_DAV, "displayname");
                        if (displayNameNodes.length()) {
                            // TODO(pbek): we want to use this display name in
                            // the future!
                            const QString displayName = displayNameNodes.at(
                                    0).toElement().text();
                            qDebug() << __func__ << " - 'displayName': " <<
                            displayName;
                        }
                    }
                }
            }
        }
    }

    return resultList;
}

void OwnCloudService::loadTodoItems(QString &data) {
    QDomDocument doc;
    doc.setContent(data, true);

    // fetch all urls that are currently in the calendar
    QList<QUrl> calendarItemUrlRemoveList =
            CalendarItem::fetchAllUrlsByCalendar(calendarName);

    QDomNodeList responseNodes = doc.elementsByTagNameNS(NS_DAV, "response");
    int responseNodesCount = responseNodes.length();
    int requestCount = 0;

    // set the preliminary maximum of the progress bar
    if (todoDialog != Q_NULLPTR) {
        todoDialog->todoItemLoadingProgressBarSetMaximum(responseNodesCount);
    }

    // loop all response blocks
    for (int i = 0; i < responseNodesCount; ++i) {
        // update the UI
        QCoreApplication::processEvents();

        QDomNode responseNode = responseNodes.at(i);
        if (responseNode.isElement()) {
            QDomElement elem = responseNode.toElement();

            // check if we have an url
            QDomNodeList urlPartNodes = elem.elementsByTagNameNS(NS_DAV,
                                                                 "href");
            if (urlPartNodes.length()) {
                QString urlPart = urlPartNodes.at(0).toElement().text();

                if (urlPart == "") {
                    continue;
                }

                QUrl calendarItemUrl = QUrl(serverUrlWithoutPath + urlPart);

                // check if we have an etag
                QDomNodeList etagNodes = elem.elementsByTagNameNS(NS_DAV,
                                                                  "getetag");
                if (etagNodes.length()) {
                    QString etag = etagNodes.at(0).toElement().text();
                    etag.replace("\"", "");
//                    qDebug() << __func__ << " - 'etag': " << etag;

                    // check if we have a last modified date
                    QDomNodeList lastModifiedNodes = elem.elementsByTagNameNS(
                            NS_DAV, "getlastmodified");
                    if (lastModifiedNodes.length()) {
                        const QString lastModified = lastModifiedNodes.at(
                                0).toElement().text();
                        bool fetchItem = false;

                        // try to fetch the calendar item by url
                        CalendarItem calItem = CalendarItem::fetchByUrl(
                                calendarItemUrl);
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
                            CalendarItem::addCalendarItemForRequest(
                                    calendarName, calendarItemUrl, etag,
                                    lastModified);
                            fetchItem = true;
                        }

                        // remove the url from the list of calendar item urls
                        // to remove
                        if (calendarItemUrlRemoveList.contains(
                                calendarItemUrl)) {
                            calendarItemUrlRemoveList.removeAll(
                                    calendarItemUrl);
                        }

                        // fetch the calendar item
                        if (fetchItem) {
                            QNetworkRequest r(calendarItemUrl);
                            addAuthHeader(&r);

                            QNetworkReply *reply = networkManager->get(r);
                            ignoreSslErrorsIfAllowed(reply);

                            requestCount++;
                        }
                    }
                }
            }
        }
    }

    if (todoDialog != Q_NULLPTR) {
        // set the real maximum of the progress bar
        todoDialog->todoItemLoadingProgressBarSetMaximum(requestCount);

        // hide progress bar if there were no updates
        if (requestCount == 0) {
            todoDialog->todoItemLoadingProgressBarHide();
        }
    }

    // remove all not found items
    for (int i = 0; i < calendarItemUrlRemoveList.length(); ++i) {
        QUrl url = calendarItemUrlRemoveList.at(i);
        CalendarItem calItem = CalendarItem::fetchByUrl(url);

        if (calItem.isFetched()) {
            calItem.remove();
        }
    }

    if (todoDialog != Q_NULLPTR) {
        // reload the existing items
        todoDialog->reloadTodoListItems();
    }
}

/**
 * Updates the share status of the notes
 *
 * @param data
 */
void OwnCloudService::updateNoteShareStatus(QString &data) {
    QXmlQuery query;
    QDomDocument doc;
    doc.setContent(data, true);

    if (data.isEmpty()) {
        showOwnCloudServerErrorMessage("", false);
    }

    QStringList pathList;
    QDomNodeList responseNodes = doc.elementsByTagNameNS(NS_DAV, "element");

//    QJSEngine engine;
//    QJSValue result = engine.evaluate(data);
//
//
//    QJSValue versions = result.property(0).property("data");
//
//
//    qDebug() << __func__ << " - 'versions': " << versions.toString();

    query.setFocus(data);
    query.setQuery("ocs/data/element");

    if (!query.isValid()) {
        return;
    }

    qDebug() << __func__ << " - 'query.isValid()': " << query.isValid();
    QXmlResultItems results;
    query.evaluateTo(&results);

    if (results.hasError()) {
        return;
    }

    QString serverNotesPath = NoteFolder::currentRemotePath();
    qDebug() << __func__ << " - 'serverNotesPath': " << serverNotesPath;


    while (!results.next().isNull()) {
        query.setFocus(results.current());

        query.setQuery("share_type/text()");
        QString shareType;
        query.evaluateTo(&shareType);

        // we only want public shares
        if (shareType.trimmed() != "3") {
            continue;
        }

        query.setQuery("item_type/text()");
        QString itemType;
        query.evaluateTo(&itemType);

        // we only want file shares
        if (itemType.trimmed() != "file") {
            continue;
        }

        query.setQuery("path/text()");
        QString path;
        query.evaluateTo(&path);
        path = path.trimmed();

        // we only want shares in our note folder
        if (!path.startsWith(serverNotesPath)) {
            continue;
        }

        // remove the note path from the path
        path = Utils::Misc::removeIfStartsWith(path, serverNotesPath);

        qDebug() << __func__ << " - 'path': " << path;
        QFileInfo fileInfo(path);

        QString fileName = fileInfo.fileName();
        QString fileParentPath = fileInfo.dir().path();
        if (fileParentPath == ".") {
            fileParentPath = "";
        }

        qDebug() << __func__ << " - 'fileName': "
                 << fileName;
        qDebug() << __func__ << " - 'fileParentPath': "
                 << fileParentPath;
    }

//    if (output == Q_NULLPTR) {
//        return;
//    }
//
//    qDebug() << __func__ << " - 'output': " << output;

    return;


    for (int i = 0; i < responseNodes.count(); i++) {
        QDomNode responseNode = responseNodes.at(i);
        if (responseNode.isElement()) {
            QDomElement elem = responseNode.toElement();

            bool isFolder = false;
            QDomNodeList resourceTypeNodes =
                    elem.elementsByTagNameNS(NS_DAV, "resourcetype");
            if (resourceTypeNodes.length()) {
                QDomNodeList typeNodes = resourceTypeNodes.at(0).childNodes();
                for (int j = 0; j < typeNodes.length(); ++j) {
                    QDomNode typeNode = typeNodes.at(j);
                    QString typeString = typeNode.toElement().tagName();

                    if (typeString == "collection") {
                        isFolder = true;
                        break;
                    }
                }
            }

            if (!isFolder) {
                continue;
            }

            // check if we have an url
            QDomNodeList urlPartNodes = elem.elementsByTagNameNS(NS_DAV,
                                                                 "href");
            if (urlPartNodes.length()) {
                QString urlPart = urlPartNodes.at(0).toElement().text();

                QRegularExpression re(
                    QRegularExpression::escape(webdavPath) + "\\/(.+)\\/$");

                QRegularExpressionMatch match = re.match(urlPart);
                QString folderString =
                        match.hasMatch() ? match.captured(1) : "";

                if (!folderString.isEmpty()) {
                    pathList << QUrl::fromPercentEncoding(
                            folderString.toUtf8());
                }
            }
        }
    }

    settingsDialog->setNoteFolderRemotePathList(pathList);
}

void OwnCloudService::loadDirectory(QString &data) {
    QDomDocument doc;
    doc.setContent(data, true);

    if (data.isEmpty()) {
        showOwnCloudServerErrorMessage("", false);
    }

    QStringList pathList;
    QDomNodeList responseNodes = doc.elementsByTagNameNS(NS_DAV, "response");

    for (int i = 0; i < responseNodes.count(); i++) {
        QDomNode responseNode = responseNodes.at(i);
        if (responseNode.isElement()) {
            QDomElement elem = responseNode.toElement();

            bool isFolder = false;
            QDomNodeList resourceTypeNodes =
                    elem.elementsByTagNameNS(NS_DAV, "resourcetype");
            if (resourceTypeNodes.length()) {
                QDomNodeList typeNodes = resourceTypeNodes.at(0).childNodes();
                for (int j = 0; j < typeNodes.length(); ++j) {
                    QDomNode typeNode = typeNodes.at(j);
                    QString typeString = typeNode.toElement().tagName();

                    if (typeString == "collection") {
                        isFolder = true;
                        break;
                    }
                }
            }

            if (!isFolder) {
                continue;
            }

            // check if we have an url
            QDomNodeList urlPartNodes = elem.elementsByTagNameNS(NS_DAV,
                                                                 "href");
            if (urlPartNodes.length()) {
                QString urlPart = urlPartNodes.at(0).toElement().text();

                QRegularExpression re(
                    QRegularExpression::escape(webdavPath) + "\\/(.+)\\/$");

                QRegularExpressionMatch match = re.match(urlPart);
                QString folderString =
                        match.hasMatch() ? match.captured(1) : "";

                if (!folderString.isEmpty()) {
                    pathList << QUrl::fromPercentEncoding(
                            folderString.toUtf8());
                }
            }
        }
    }

    settingsDialog->setNoteFolderRemotePathList(pathList);
}

void OwnCloudService::postCalendarItemToServer(CalendarItem calendarItem,
                                               TodoDialog *dialog) {
    this->todoDialog = dialog;

    calendarItem.generateNewICSData();

    QUrl url(calendarItem.getUrl());
    QNetworkRequest r;
    addAuthHeader(&r);
    r.setUrl(url);

    // build the request body
    QString body = calendarItem.getICSData();

    QByteArray *dataToSend = new QByteArray(body.toUtf8());
    r.setHeader(QNetworkRequest::ContentLengthHeader, dataToSend->size());
    QBuffer *buffer = new QBuffer(dataToSend);

    QNetworkReply *reply = networkManager->sendCustomRequest(r, "PUT", buffer);
    ignoreSslErrorsIfAllowed(reply);
}

/**
 * @brief Loads and updates the icsData of a calendar item with data from the calendar server
 * @return
 */
bool OwnCloudService::updateICSDataOfCalendarItem(CalendarItem *calItem) {
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QUrl url(calItem->getUrl());
    QNetworkRequest r;

    addAuthHeader(&r);
    r.setUrl(url);

    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(manager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));

    // 5 sec timeout for the request
    timer.start(5000);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    r.setAttribute(QNetworkRequest::FollowRedirectsAttribute,
                                true);
#endif

    QNetworkReply *reply = manager->get(r);
    ignoreSslErrorsIfAllowed(reply);

    loop.exec();

    if (timer.isActive()) {
        // get the text from the network reply
        QString icsData = reply->readAll();

        // set the new ics data
        calItem->setICSData(icsData);

        return true;
    }

    // timer elapsed, no reply from network request
    return false;
}

/**
 * Gets the file list from the ownCloud server
 * for the settings dialog
 */
void OwnCloudService::settingsGetFileList(
        SettingsDialog *dialog, QString path) {
    settingsDialog = dialog;

    QUrl url(serverUrl + webdavPath + "/" + path);
    QNetworkRequest r(url);
    addAuthHeader(&r);

    // build the request body
    QString body = "<?xml version=\"1.0\"?>"
            "<a:propfind xmlns:a=\"DAV:\">"
                "<a:prop><a:resourcetype />"
                "</a:prop>"
            "</a:propfind>";

    QByteArray *dataToSend = new QByteArray(body.toUtf8());
    r.setHeader(QNetworkRequest::ContentLengthHeader, dataToSend->size());
    QBuffer *buffer = new QBuffer(dataToSend);

    QNetworkReply *reply = networkManager->sendCustomRequest(
            r, "PROPFIND", buffer);
    ignoreSslErrorsIfAllowed(reply);
}
