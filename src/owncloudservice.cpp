#include "owncloudservice.h"
#include "settingsdialog.h"
#include <QSettings>
#include <QDebug>
#include <QUrlQuery>
#include <QScriptEngine>

const QString OwnCloudService::rootPath = "/index.php/apps/qownnotesapi/api/v1/";
const QString OwnCloudService::format = "json";

OwnCloudService::OwnCloudService(SimpleCrypt *crypto, QObject *parent) : QObject(parent)
{
    this->crypto = crypto;
    readSettings();
}

void OwnCloudService::readSettings()
{
    QSettings settings;
    serverUrl = settings.value( "ownCloud/serverUrl" ).toString();
    userName = settings.value( "ownCloud/userName" ).toString();

    // TODO: check why password cannot be decrypted
    password = crypto->decryptToString( settings.value( "ownCloud/password" ).toString() );

    networkManager = new QNetworkAccessManager();
    busy = false;

    versionListPath = rootPath + "note/versions";
    appInfoPath = rootPath + "note/app_info";
    capabilitiesPath = "/ocs/v1.php/cloud/capabilities";

    QObject::connect( networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(slotAuthenticationRequired(QNetworkReply*,QAuthenticator*)) );
    QObject::connect( networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotReplyFinished(QNetworkReply*)) );
}

void OwnCloudService::slotAuthenticationRequired( QNetworkReply* reply, QAuthenticator* authenticator )
{
    // TODO: output error message
    qDebug() << "Asked to authenticate";
    authenticator->setUser(userName);
    authenticator->setPassword(password);
}

void OwnCloudService::slotReplyFinished( QNetworkReply* reply )
{
    qDebug() << "Reply from " << reply->url().path();

    // this only should called from the settings dialog
    if ( reply->url().path().endsWith( appInfoPath ) )
    {
        qDebug() << "Reply from app info";
        QString data = QString( reply->readAll() );
        qDebug() << data;

        // check if everything is all right and call the callback method
        checkAppInfo( data );

        return;
    }
    else if ( reply->url().path().endsWith( versionListPath ) )
    {
        qDebug() << "Reply from version list";
        QByteArray arr = reply->readAll();
        QString data = QString( arr );
        qDebug() << data;
        return;
    }

    busy = false;
    emit(busyChanged(busy));
}

void OwnCloudService::checkAppInfo( QString data )
{
    // we have to add [], so the string can be parsed as JSON
    data = QString("[") + data + QString("]");

    QScriptEngine engine;
    QScriptValue result = engine.evaluate(data);

    // get the information if versioning is available
    bool appIsValid = result.property(0).property("versioning").toBool();

    QString appVersion = result.property(0).property("app_version").toString();
    QString serverVersion = result.property(0).property("server_version").toString();

    // call callback in settings dialog
    settingsDialog->connectTestCallback( appIsValid, appVersion, serverVersion );
}

/**
 * @brief OwnCloudService::connectionTest
 */
void OwnCloudService::settingsConnectionTest( SettingsDialog *dialog )
{
    settingsDialog = dialog;

    qDebug() << serverUrl;
    qDebug() << userName;
    qDebug() << password;

    if ( !busy )
    {
        busy = true;
        emit(busyChanged(busy));

        QUrl url( serverUrl + appInfoPath );
//        QUrl url( serverUrl + versionListPath );

        url.setUserName( userName );
        url.setPassword( password );

        QUrlQuery q;
        q.addQueryItem("format", format);
        url.setQuery(q);

        qDebug() << url;

        QNetworkRequest r(url);
        addAuthHeader(&r);

        QNetworkReply *reply = networkManager->get(r);
        QObject::connect(reply, SIGNAL(sslErrors(QList<QSslError>)), reply, SLOT(ignoreSslErrors()));
    }
}

void OwnCloudService::addAuthHeader(QNetworkRequest *r)
{
    if (r)
    {
        QString concatenated = userName + ":" + password;
        QByteArray data = concatenated.toLocal8Bit().toBase64();
        QString headerData = "Basic " + data;
        r->setRawHeader("Authorization", headerData.toLocal8Bit());
    }
}

bool OwnCloudService::isBusy()
{
    qDebug() << "Busy: " << busy;
    return busy;
}
