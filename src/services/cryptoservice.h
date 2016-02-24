#pragma once

// enable debugging for the Piwik tracker
#define PIWIK_TRACKER_DEBUG 1

#include <QObject>
#include <libraries/simplecrypt/simplecrypt.h>

class CryptoService : public QObject
{
    Q_OBJECT

public:
    explicit CryptoService(QObject *parent = 0);
    static CryptoService *instance();
    static CryptoService *createInstance(QObject *parent);
    QString encryptToString(QString text);
    QString decryptToString(QString text);

private:
    SimpleCrypt * _simpleCrypt;

signals:

public slots:
};
