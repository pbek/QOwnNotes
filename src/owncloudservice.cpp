#include "owncloudservice.h"
#include <QSettings>
#include <QDebug>

OwnCloudService::OwnCloudService(SimpleCrypt *crypto, QObject *parent) : QObject(parent)
{
    this->crypto = crypto;
    readSettings();
}

void OwnCloudService::readSettings()
{
    QSettings settings;
    serverName = settings.value( "ownCloud/serverName" ).toString();
    userName = settings.value( "ownCloud/userName" ).toString();

    // TODO: check why password cannot be decrypted
    password = crypto->decryptToString( settings.value( "ownCloud/password" ).toString() );
}

void OwnCloudService::connect()
{
    qDebug() << serverName;
    qDebug() << userName;
    qDebug() << password;
}
