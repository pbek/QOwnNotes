#include "owncloudservice.h"
#include <QSettings>
#include <QDebug>
#include <QUrlQuery>

const QString OwnCloudService::rootPath = "/index.php/apps/qownnotes-api/api/v1/";
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

    versionListPath = rootPath + "versions";
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

    if ( reply->url().path().endsWith( capabilitiesPath ) )
    {
        qDebug() << "Reply from capabilities";
        QByteArray arr = reply->readAll();
        QString string = QString( arr );
        qDebug() << string;
        return;
    }
    else if ( reply->url().path().endsWith( versionListPath ) )
    {
        qDebug() << "Reply from version list";
        QByteArray arr = reply->readAll();
        QString string = QString( arr );
        qDebug() << string;
        return;
    }

    busy = false;
    emit(busyChanged(busy));
}


void OwnCloudService::connect()
{
    qDebug() << serverUrl;
    qDebug() << userName;
    qDebug() << password;

    if ( !busy )
    {
        busy = true;
        emit(busyChanged(busy));

        QUrl url( serverUrl + capabilitiesPath );
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
