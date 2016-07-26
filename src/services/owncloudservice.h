#ifndef OWNCLOUDSERVICE_H
#define OWNCLOUDSERVICE_H

#include <QNetworkAccessManager>
#include <QAuthenticator>
#include <QNetworkReply>
#include <QObject>
#include "mainwindow.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/tododialog.h"

#define QOWNNOTESAPI_MIN_VERSION "0.4.2"

class OwnCloudService : public QObject {
Q_OBJECT

public:
    enum CalendarBackend {
        DefaultOwnCloudCalendar = 0,
        CalendarPlus
    };
    Q_ENUMS(CalendarBackend)

    explicit OwnCloudService(QObject *parent = 0);

    void settingsConnectionTest(SettingsDialog *dialog);

    void loadVersions(QString fileName, MainWindow *mainWindow);

    void loadTrash(MainWindow *mainWindow);

    void restoreTrashedNoteOnServer(QString fileName,
                                    int timestamp, MainWindow *mainWindow);

    void settingsGetCalendarList(SettingsDialog *dialog);

    void todoGetTodoList(QString calendarName, TodoDialog *dialog);

    void postCalendarItemToServer(CalendarItem calendarItem,
                                  TodoDialog *dialog);

    bool updateICSDataOfCalendarItem(CalendarItem *calItem);

    void removeCalendarItem(CalendarItem calItem, TodoDialog *dialog);

    void settingsGetFileList(SettingsDialog *dialog, QString path);

    static bool hasOwnCloudSettings();

    void shareNote(Note note);

private:
    QString serverUrl;
    QString serverUrlPath;
    QString serverUrlWithoutPath;
    QString userName;
    QString password;
    QNetworkAccessManager *networkManager;
    MainWindow *mainWindow;
    static const QString rootPath;
    static const QString format;
    QString versionListPath;
    QString trashListPath;
    QString appInfoPath;
    QString capabilitiesPath;
    QString ownCloudTestPath;
    QString restoreTrashedNotePath;
    QString calendarPath;
    QString webdavPath;
    SettingsDialog *settingsDialog;
    TodoDialog *todoDialog;
    QString calendarName;

    void checkAppInfo(QNetworkReply *reply);

    void readSettings();

    void addAuthHeader(QNetworkRequest *r);

    void handleVersionsLoading(QString data);

    void handleTrashedLoading(QString data);

    QStringList parseCalendarHrefList(QString &data);

    void loadTodoItems(QString &data);

    void ignoreSslErrorsIfAllowed(QNetworkReply *reply);

    void loadDirectory(QString &data);

    void showOwnCloudServerErrorMessage(
            QString message = QString(""), bool withSettingsButton = true);

signals:

private slots:

    void slotAuthenticationRequired(QNetworkReply *reply,
                                    QAuthenticator *authenticator);

    void slotReplyFinished(QNetworkReply *);
};

#endif // OWNCLOUDSERVICE_H
