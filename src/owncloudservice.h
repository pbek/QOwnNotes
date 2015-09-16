#ifndef OWNCLOUDSERVICE_H
#define OWNCLOUDSERVICE_H

#include <QObject>
#include "libraries/simplecrypt/simplecrypt.h"

class OwnCloudService : public QObject
{
    Q_OBJECT
public:
    explicit OwnCloudService(SimpleCrypt *crypto, QObject *parent = 0);
    void connect();
private:
    QString serverName;
    QString userName;
    QString password;
    SimpleCrypt *crypto;
    void readSettings();
signals:

public slots:
};

#endif // OWNCLOUDSERVICE_H
