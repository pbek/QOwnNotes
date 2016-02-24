#include "cryptoservice.h"
#include <QSettings>
#include <QDebug>
#include <QCoreApplication>

CryptoService::CryptoService(QObject *parent) : QObject(parent)
{
    QSettings settings;
    qint64 cryptoKey = settings.value("cryptoKey").toUInt();

    // generate a key if we don't have one
    if (cryptoKey == 0) {
        cryptoKey = qrand();
        settings.setValue("cryptoKey", cryptoKey);
    }

    _simpleCrypt = new SimpleCrypt(static_cast<quint64>(cryptoKey));
}

/**
 * Fetches a global instance of the class
 */
CryptoService * CryptoService::instance() {
    return qApp->property("cryptoService").value<CryptoService *>();
}

/**
 * Creates a global instance of the class
 */
CryptoService * CryptoService::createInstance(QObject *parent) {
    CryptoService *cryptoService = new CryptoService(parent);

    qApp->setProperty(
            "cryptoService",
            QVariant::fromValue<CryptoService *>(cryptoService));

    return cryptoService;
}

/**
 * Encrypts a string as string
 */
QString CryptoService::encryptToString(QString text) {
    return _simpleCrypt->encryptToString(text);
}

/**
 * Decrypts a string as string
 */
QString CryptoService::decryptToString(QString text) {
    return _simpleCrypt->decryptToString(text);
}
