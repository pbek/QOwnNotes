#include "owncloudservice.h"
#include "settingsdialog.h"
#include <QSettings>
#include <QDebug>
#include <QUrlQuery>
#include <QScriptEngine>
#include <QDir>
#include <QMessageBox>
#include <QScriptValueIterator>

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
    password = crypto->decryptToString( settings.value( "ownCloud/password" ).toString() );
    localOwnCloudPath = settings.value( "ownCloud/localOwnCloudPath" ).toString();

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

        // handle the versions loading
        handleVersionsLoading( data );
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

/**
 * @brief OwnCloudService::loadVersions
 */
void OwnCloudService::loadVersions( QString notesPath, QString fileName )
{
    if ( !busy )
    {
        busy = true;
        emit(busyChanged(busy));

        QUrl url( serverUrl + versionListPath );
        QString serverNotesPath = getServerNotesPath( notesPath );

        url.setUserName( userName );
        url.setPassword( password );

        QUrlQuery q;
        q.addQueryItem( "format", format );
        q.addQueryItem( "file_name", serverNotesPath + fileName );
        url.setQuery( q );

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

/**
 * Try to find the server notes path with the help of the notes path and the local ownCloud path
 *
 * @brief OwnCloudService::getServerNotesPath
 * @param notesPath
 * @return QString
 */
QString OwnCloudService::getServerNotesPath( QString notesPath )
{
    QString path = this->localOwnCloudPath;

    // try to assume local ownCloud directory if not set
    if ( path == "" )
    {
        path = QDir::homePath() + QDir::separator() + "ownCloud";
    }

    QString serverNotesPath = "";
    if ( notesPath.contains( path ) )
    {
        // get the server notes path out of the notes path
        QStringList list = notesPath.split( path );
        serverNotesPath = ( list.count() > 1 ) ? list.at( 1 ) : "/";

        // translate the directory separators to "/"
        serverNotesPath = serverNotesPath.split( QDir::separator() ).join( "/" );

        // add a leading "/"
        if ( serverNotesPath.at( 0 ) != '/' )
        {
            serverNotesPath.prepend( "/" );
        }

        // add a trailing "/"
        if ( serverNotesPath.at( serverNotesPath.size() - 1 ) != '/' )
        {
            serverNotesPath.append( "/" );
        }
    }

    return serverNotesPath;
}

/**
 * Handles the versions loading
 *
 * @brief OwnCloudService::handleVersionsLoading
 * @param data
 */
void OwnCloudService::handleVersionsLoading( QString data )
{
    // check if we get any data at all
    if ( data == "" )
    {
        QMessageBox::critical( 0, "ownCloud server connection error!", "Cannot connect to the ownCloud server! Please check your ownCloud server configuration." );
        return;
    }

    // we have to add [], so the string can be parsed as JSON
    data = QString("[") + data + QString("]");

    QScriptEngine engine;
    QScriptValue result = engine.evaluate(data);

    // get the information if versioning is available
    QString message = result.property(0).property("message").toString();

    // check if we got an error message
    if ( message != "" )
    {
        QMessageBox::critical( 0, "ownCloud server connection error!", "ownCloud server error: " + message );
        return;
    }

    // get the versions
    QScriptValue versions = result.property(0).property("versions");

    // check if we got no usefull data
    if ( versions.toString() == "" )
    {
        QMessageBox::critical( 0, "ownCloud server connection error!", "Cannot connect to the ownCloud server! Please check your ownCloud server configuration." );
        return;
    }

    // init the iterator for the verions
    QScriptValueIterator versionsIterator( versions );

    // iterate over the versions
    while ( versionsIterator.hasNext() ) {
        versionsIterator.next();
        qDebug() << versionsIterator.name() << ": " << versionsIterator.value().property( "timestamp" ).toString() << " - " << versionsIterator.value().property( "humanReadableTimestamp" ).toString() << " - " << versionsIterator.value().property( "diffHtml" ).toString();
    }
}
