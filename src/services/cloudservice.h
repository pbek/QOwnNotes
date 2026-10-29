#pragma once

#include <QDomNodeList>
#include <QObject>
#include <QPointer>
#include <functional>

#define QOWNNOTESAPI_MIN_VERSION "0.4.2"

// We set a user agent to prevent troubles with some Nextcloud / ownCloud
// server hosting providers
// see: https://github.com/pbek/QOwnNotes/issues/541
#define CLOUD_SERVICE_USER_AGENT \
    "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9a3pre) Gecko/20070330"

struct CalDAVCalendarData {
    QString url;
    QString displayName;
};

class SettingsDialog;
class MainWindow;
class TodoDialog;
class CalendarItem;
class Note;
class ShareDialog;
class QNetworkReply;
class QNetworkAccessManager;
class QNetworkRequest;
class QAuthenticator;
class QString;

class CloudService : public QObject {
    Q_OBJECT

   public:
    enum CalendarBackend {
        LegacyOwnCloudCalendar = 0,
        CalendarPlus,
        CalDAVCalendar,
        DefaultCloudCalendar
    };
    Q_ENUMS(CalendarBackend)

    explicit CloudService(int cloudConnectionId = -1, QObject *parent = nullptr);

    void settingsConnectionTest(SettingsDialog *dialog);

    void loadVersions(const QString &fileName);

    void loadTrash();

    void restoreTrashedNoteOnServer(const QString &fileName, int timestamp);

    int deleteTrashedNoteOnServer(const QString &fileName, int timestamp);

    void settingsGetCalendarList(SettingsDialog *dialog);

    void todoGetTodoList(const QString &calendarName, TodoDialog *dialog);

    void postCalendarItemToServer(CalendarItem calendarItem, TodoDialog *dialog,
                                  const std::function<void()> &onSuccess = {});

    bool updateICSDataOfCalendarItem(CalendarItem *calItem);

    void removeCalendarItem(CalendarItem calItem, TodoDialog *dialog);

    void settingsGetFileList(SettingsDialog *dialog, const QString &path);

    void abortSettingsConnectionTest();

    static bool hasCloudSettings(bool withEnabledCheck = true, bool ignoreTableWarning = false);

    void shareNote(const Note &note, ShareDialog *dialog, const QString &password = QString());

    void setPermissionsOnSharedNote(const Note &note, ShareDialog *dialog);

    void fetchShares(const QString &path = QString());

    void fetchBookmarks();

    void removeNoteShare(const Note &note, ShareDialog *dialog);

    static CloudService *instance(bool reset = false, int cloudConnectionId = -1);

    static CloudService *currentInstance();

    static bool isCloudSupportEnabled();

    static bool isTodoCalendarSupportEnabled();

    static bool isTodoSupportEnabled();

    void startAppVersionTest();

    QString nextcloudPreviewImageTagToInlineImageTag(QString imageTag, int &imageWidth);

    static bool initiateLoginFlowV2(const QString &serverUrl, QJsonObject &pollData);

    static QString fetchNextcloudAccountId(const QString &serverUrl, const QString &userName,
                                           const QString &password);

    void unsetShareDialog();

    QString fetchNoteFileId(const Note &note);

    static QString apiBaseFromCapabilities(const QByteArray &data);

    QString apiAppName() const;

   private:
    QString serverUrl;
    QString todoCalendarServerUrl;
    QString serverUrlPath;
    QString todoCalendarServerUrlPath;
    QString serverUrlWithoutPath;
    QString todoCalendarServerUrlWithoutPath;
    QString userName;
    QString todoCalendarUsername;
    QString password;
    bool appQOwnNotesAPICheckEnabled;
    QString todoCalendarPassword;
    QNetworkAccessManager *networkManager;
    ShareDialog *shareDialog;
    QString rootPath;
    bool apiDiscovered = false;
    bool apiDiscoveryPending = false;
    int apiDiscoveryGeneration = 0;
    int settingsConnectionTestGeneration = 0;
    QList<std::function<void()>> apiDiscoveryCallbacks;
    static const QString format;
    int _settingsCalendarListRequestId = 0;
    QString versionListPath;
    QString trashListPath;
    QString appInfoPath;
    QString capabilitiesPath;
    QString cloudTestPath;
    QString restoreTrashedNotePath;
    QString sharePath;
    QString bookmarkPath;
    QString trashDeletePath;
    QPointer<SettingsDialog> settingsDialog;
    QPointer<TodoDialog> todoDialog;

    void checkAppInfo(QNetworkReply *reply);

    void readSettings(int cloudConnectionId = -1);

    void setApiBase(const QString &path);

    void discoverApi(const std::function<void()> &callback);

    static void addGenericAuthHeader(QNetworkRequest *r, const QString &userName,
                                     const QString &password);

    void addAuthHeader(QNetworkRequest *r);

    void addCalendarAuthHeader(QNetworkRequest *r);

    void handleVersionsLoading(QString data);

    void handleTrashedLoading(QString data);

    QList<CalDAVCalendarData> parseCalendarData(QString &data);

    void loadTodoItems(const QString &calendarName, QString &data);

    static void ignoreSslErrorsIfAllowed(QNetworkReply *reply);

    void loadDirectory(QString &data);

    void showCloudServerErrorMessage(const QString &message = QString(),
                                     bool withSettingsButton = true);

    void showCloudMessage(QString headline = QString(), QString message = QString(),
                          bool withSettingsButton = true);

    void updateNoteShareStatusFromShare(QString &data);

    void updateNoteShareStatusFromFetchAll(QString &data);

    void handleNoteShareReply(QString &data);

    void updateNoteShareStatus(QDomNodeList &dataElements, bool updateShareDialog = false);

    void handleUpdateNoteShareReply(const QString &urlPart, const QString &data);

    static void checkAppVersion(QNetworkReply *reply);

    void handleImportBookmarksReply(QString &data);

    QByteArray downloadNextcloudPreviewImage(const QString &path);

    void resetNetworkManagerCookieJar();

    QString webdavPath();

   signals:
    void settingsConnectionTestFinished();

    void calendarItemWriteFinished(const QString &url, bool success, const QString &error);

   private slots:

    void slotAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);

    void slotCalendarAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);

    void slotReplyFinished(QNetworkReply *);
};
