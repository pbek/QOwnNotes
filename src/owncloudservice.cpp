#include "mainwindow.h"
#include "owncloudservice.h"
#include "settingsdialog.h"
#include "trashdialog.h"
#include "versiondialog.h"
#include <QSettings>
#include <QDebug>
#include <QUrlQuery>
#include <QScriptEngine>
#include <QDir>
#include <QMessageBox>
#include <QScriptValueIterator>
#include "libraries/versionnumber/versionnumber.h"

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
    trashListPath = rootPath + "note/trashed";
    appInfoPath = rootPath + "note/app_info";
    capabilitiesPath = "/ocs/v1.php/cloud/capabilities";
    ownCloudTestPath = "/ocs/v1.php";

    QObject::connect( networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)), this, SLOT(slotAuthenticationRequired(QNetworkReply*,QAuthenticator*)) );
    QObject::connect( networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotReplyFinished(QNetworkReply*)) );
}

void OwnCloudService::slotAuthenticationRequired( QNetworkReply* reply, QAuthenticator* authenticator )
{
    qDebug() << "Username and/or password incorrect";

    settingsDialog->setOKLabelData( 3, "incorrect", SettingsDialog::Failure );
    settingsDialog->setOKLabelData( 4, "not connected", SettingsDialog::Failure );

    reply->abort();
}

void OwnCloudService::slotReplyFinished( QNetworkReply* reply )
{
    qDebug() << "Reply from " << reply->url().path();
    // qDebug() << reply->errorString();

    // this only should called from the settings dialog
    if ( reply->url().path().endsWith( appInfoPath ) )
    {
        qDebug() << "Reply from app info";

        // check if everything is all right and call the callback method
        checkAppInfo( reply );

        return;
    }
    else
    {
        QByteArray arr = reply->readAll();
        QString data = QString( arr );

        if ( reply->url().path().endsWith( versionListPath ) )
        {
            qDebug() << "Reply from version list";
            // qDebug() << data;

            // handle the versions loading
            handleVersionsLoading( data );
            return;
        }
        else if ( reply->url().path().endsWith( trashListPath ) )
        {
            qDebug() << "Reply from trash list";
            // qDebug() << data;

            // handle the loading of trashed notes
            handleTrashedLoading( data );
            return;
        }
        else if ( reply->url().path().endsWith( capabilitiesPath ) )
        {
            qDebug() << "Reply from capabilites page";

            if ( data.startsWith( "<?xml version=" ) )
            {
                settingsDialog->setOKLabelData( 3, "ok", SettingsDialog::OK );
            }
            else
            {
                settingsDialog->setOKLabelData( 3, "not correct", SettingsDialog::Failure );
            }

            return;
        }
        else if ( reply->url().path().endsWith( ownCloudTestPath ) )
        {
            qDebug() << "Reply from ownCloud test page";

            if ( data.startsWith( "<?xml version=" ) )
            {
                settingsDialog->setOKLabelData( 2, "ok", SettingsDialog::OK );
            }
            else
            {
                settingsDialog->setOKLabelData( 2, "not detected", SettingsDialog::Failure );
            }

            return;
        }
        else if ( reply->url().path() == "" )
        {
            qDebug() << "Reply from main server url";

            if ( data != "" )
            {
                settingsDialog->setOKLabelData( 1, "ok", SettingsDialog::OK );
            }
            else
            {
                settingsDialog->setOKLabelData( 1, "not found", SettingsDialog::Failure );
            }

            return;
        }
    }

    busy = false;
    emit(busyChanged(busy));
}

void OwnCloudService::checkAppInfo( QNetworkReply* reply )
{
    QString data = QString( reply->readAll() );
    // qDebug() << data;

    // we have to add [], so the string can be parsed as JSON
    data = QString("[") + data + QString("]");

    QScriptEngine engine;
    QScriptValue result = engine.evaluate(data);

    bool appIsValid = result.property(0).property("versioning").toBool();

    QString appVersion = result.property(0).property("app_version").toString();
    QString serverVersion = result.property(0).property("server_version").toString();

    // reset to "unknown" in case we can't test if versions and trash app are enabled
    settingsDialog->setOKLabelData( 6, "unknown", SettingsDialog::Unknown );
    settingsDialog->setOKLabelData( 7, "unknown", SettingsDialog::Unknown );

    if ( serverVersion != "" )
    {
        VersionNumber serverAppVersion = VersionNumber( appVersion );
        VersionNumber minAppVersion = VersionNumber( QOWNNOTESAPI_MIN_VERSION );

        if ( minAppVersion > serverAppVersion )
        {
            settingsDialog->setOKLabelData( 4, "version " + appVersion + " too low", SettingsDialog::Warning );
        }
        else
        {
            settingsDialog->setOKLabelData( 4, "ok", SettingsDialog::OK );
        }

        // check if versions and trash app are enabled after QOwnNotesAPI v0.3.1
        if ( serverAppVersion >= VersionNumber( "0.3.1" ) )
        {
            bool versionsAppEnabled = result.property(0).property("versions_app").toBool();
            bool trashAppEnabled = result.property(0).property("trash_app").toBool();

            if ( versionsAppEnabled )
            {
                settingsDialog->setOKLabelData( 6, "ok", SettingsDialog::OK );
            }
            else
            {
                settingsDialog->setOKLabelData( 6, "not enabled", SettingsDialog::Failure );
            }

            if ( trashAppEnabled )
            {
                settingsDialog->setOKLabelData( 7, "ok", SettingsDialog::OK );
            }
            else
            {
                settingsDialog->setOKLabelData( 7, "not enabled", SettingsDialog::Failure );
            }
        }
    }
    else
    {
        settingsDialog->setOKLabelData( 4, "not connected", SettingsDialog::Failure );
    }

    // call callback in settings dialog
    settingsDialog->connectTestCallback( appIsValid, appVersion, serverVersion, reply->errorString() );
}

/**
 * @brief OwnCloudService::connectionTest
 */
void OwnCloudService::settingsConnectionTest( SettingsDialog *dialog )
{
    settingsDialog = dialog;

    // qDebug() << serverUrl;
    // qDebug() << userName;
    // qDebug() << password;

    if ( !busy )
    {
        busy = true;
        emit( busyChanged( busy ) );

        QUrl url( serverUrl );
        QNetworkRequest r( url );

        // direct server url request without auth header
        QNetworkReply *reply = networkManager->get( r );
        QObject::connect(reply, SIGNAL(sslErrors(QList<QSslError>)), reply, SLOT(ignoreSslErrors()));

        QUrlQuery q;
        q.addQueryItem( "format", format );
        url.setQuery(q);

        addAuthHeader(&r);

        url.setUrl( serverUrl + appInfoPath );
        r.setUrl( url );
        reply = networkManager->get(r);

        url.setUrl( serverUrl + capabilitiesPath );
        r.setUrl( url );
        reply = networkManager->get(r);

        url.setUrl( serverUrl + ownCloudTestPath );
        r.setUrl( url );
        reply = networkManager->get(r);
    }

    QSettings settings;
    QString localOwnCloudPath = settings.value( "ownCloud/localOwnCloudPath" ).toString();

    if ( localOwnCloudPath != "" )
    {
        QDir d = QDir( localOwnCloudPath );
        if ( d.exists() )
        {
            QString notesPath = settings.value( "notesPath" ).toString();

            if ( notesPath.startsWith( localOwnCloudPath ) )
            {
                if ( notesPath != localOwnCloudPath )
                {
                    settingsDialog->setOKLabelData( 5, "ok", SettingsDialog::OK );
                }
                else
                {
                    settingsDialog->setOKLabelData( 5, "note path and ownCloud path are equal", SettingsDialog::Warning );
                }
            }
            else
            {
                settingsDialog->setOKLabelData( 5, "note path not in ownCloud path", SettingsDialog::Failure );
            }
        }
        else
        {
            settingsDialog->setOKLabelData( 5, "does not exist", SettingsDialog::Failure );
        }
    }
    else
    {
        settingsDialog->setOKLabelData( 5, "empty", SettingsDialog::Failure );
    }
}

/**
 * @brief OwnCloudService::loadVersions
 */
void OwnCloudService::loadVersions( QString notesPath, QString fileName, MainWindow *mainWindow )
{
    this->mainWindow = mainWindow;

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

        // qDebug() << url;

        QNetworkRequest r(url);
        addAuthHeader(&r);

        QNetworkReply *reply = networkManager->get(r);
        QObject::connect(reply, SIGNAL(sslErrors(QList<QSslError>)), reply, SLOT(ignoreSslErrors()));
    }
}

/**
 * @brief OwnCloudService::loadTrash
 */
void OwnCloudService::loadTrash( QString notesPath, MainWindow *mainWindow )
{
    this->mainWindow = mainWindow;

    if ( !busy )
    {
        busy = true;
        emit(busyChanged(busy));

        QUrl url( serverUrl + trashListPath );
        QString serverNotesPath = getServerNotesPath( notesPath );

        url.setUserName( userName );
        url.setPassword( password );

        QUrlQuery q;
        q.addQueryItem( "format", format );
        q.addQueryItem( "dir", serverNotesPath );
        url.setQuery( q );

        // qDebug() << url;

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
        if ( QMessageBox::critical( 0, "ownCloud server connection error!",
                                          "ownCloud server error: <strong>" + message + "</strong><br />Please check your configuration in the settings!",
                                          "Open &settings", "&Cancel", QString::null,
                                          0, 1 ) == 0 )
        {
            mainWindow->openSettingsDialog();
        }

        return;
    }

    // get the filename to check if everything is all right
    QScriptValue fileName = result.property(0).property("file_name");

    // check if we got no usefull data
    if ( fileName.toString() == "" )
    {
        if ( QMessageBox::critical( 0, "ownCloud server connection error!",
                                          "QOwnNotes was unable to connect to your ownCloud server! Please check the configuration in the settings!",
                                          "Open &settings", "&Cancel", QString::null,
                                          0, 1 ) == 0 )
        {
            mainWindow->openSettingsDialog();
        }

        return;
    }

    // get the versions
    QScriptValue versions = result.property(0).property("versions");

    // check if we got no usefull data
    if ( versions.toString() == "" )
    {
        QMessageBox::information( 0, "no other version", "There are no other versions on the server for this note." );
        return;
    }

    VersionDialog *dialog = new VersionDialog( versions, mainWindow );
    dialog->exec();
}

/**
 * Handles the loading of trashed notes
 *
 * @brief OwnCloudService::handleTrashedLoading
 * @param data
 */
void OwnCloudService::handleTrashedLoading( QString data )
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

    // get a possible error messages
    QString message = result.property(0).property("message").toString();

    // check if we got an error message
    if ( message != "" )
    {
        if ( QMessageBox::critical( 0, "ownCloud server connection error!",
                                          "ownCloud server error: <strong>" + message + "</strong><br />Please check your configuration in the settings!",
                                          "Open &settings", "&Cancel", QString::null,
                                          0, 1 ) == 0 )
        {
            mainWindow->openSettingsDialog();
        }

        return;
    }

    // get the directory to check if everything is all right
    QString directory = result.property(0).property("directory").toString();

    // check if we got no usefull data
    if ( directory == "" )
    {
        if ( QMessageBox::critical( 0, "ownCloud server connection error!",
                                          "QOwnNotes was unable to connect to your ownCloud server! Please check the configuration in the settings!",
                                          "Open &settings", "&Cancel", QString::null,
                                          0, 1 ) == 0 )
        {
            mainWindow->openSettingsDialog();
        }

        return;
    }

    // get the versions
    QScriptValue notes = result.property(0).property("notes");

    // check if we got no usefull data
    if ( notes.toString() == "" )
    {
        QMessageBox::information( 0, "no other version", "There are no trashed notes on the server." );
        return;
    }

    TrashDialog *dialog = new TrashDialog( notes, mainWindow );
    dialog->exec();
}
