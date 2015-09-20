#ifndef OWNCLOUDSERVICE_H
#define OWNCLOUDSERVICE_H

#include <QNetworkAccessManager>
#include <QAuthenticator>
#include <QNetworkReply>
#include <QObject>
#include "libraries/simplecrypt/simplecrypt.h"
#include "settingsdialog.h"

class OwnCloudService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ isBusy NOTIFY busyChanged)

public:
    explicit OwnCloudService(SimpleCrypt *crypto, QObject *parent = 0);
    void settingsConnectionTest( SettingsDialog *dialog );
    bool isBusy();
    void loadVersions(QString fileName);
private:
    QString serverUrl;
    QString userName;
    QString password;
    SimpleCrypt *crypto;
    QNetworkAccessManager *networkManager;
    static const QString rootPath;
    static const QString format;
    QString versionListPath;
    QString appInfoPath;
    QString capabilitiesPath;
    SettingsDialog *settingsDialog;
    void checkAppInfo(QString string);
    bool busy;
    void readSettings();
    void addAuthHeader(QNetworkRequest *r);

signals:
    void busyChanged(bool busy);

private slots:
    void slotAuthenticationRequired ( QNetworkReply * reply, QAuthenticator * authenticator );
    void slotReplyFinished ( QNetworkReply* );
};

#endif // OWNCLOUDSERVICE_H
