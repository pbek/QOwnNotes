#include "cryptoservice.h"

#include <qtkeychain/keychain.h>

#include <QByteArray>
#include <QCoreApplication>
#include <QDebug>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QUuid>

#include "services/settingsservice.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif

namespace {
const auto KeychainMarkerPrefix = QStringLiteral("qtkeychain:");

QString keychainServiceName() {
    QString serviceName = QStringLiteral("QOwnNotes");

#ifdef QT_DEBUG
    serviceName += QStringLiteral("Debug");
#endif

    const QString session = qApp->property("session").toString();
    if (!session.isEmpty()) {
        serviceName += QStringLiteral("-") + session;
    }

    return serviceName;
}

bool executeKeychainJob(QKeychain::Job *job) {
    QEventLoop loop;
    job->setAutoDelete(false);
    QObject::connect(job, &QKeychain::Job::finished, &loop, &QEventLoop::quit);
    job->start();
    loop.exec();
    return job->error() == QKeychain::NoError;
}
}    // namespace

CryptoService::CryptoService(QObject *parent) : QObject(parent), _migrationStarted(false) {
    SettingsService settings;
    qint64 cryptoKey = settings.value(QStringLiteral("cryptoKey")).toUInt();

    // generate a key if we don't have one
    if (cryptoKey == 0) {
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
        cryptoKey = qrand();
#else
        cryptoKey = QRandomGenerator::global()->generate();
#endif
        settings.setValue(QStringLiteral("cryptoKey"), cryptoKey);
    }

    _simpleCrypt = new SimpleCrypt(static_cast<quint64>(cryptoKey));
    migrateSecrets();
}

/**
 * Fetches the global instance of the class
 * The instance will be created if it doesn't exist.
 */
CryptoService *CryptoService::instance() {
    auto *cryptoService = qApp->property("cryptoService").value<CryptoService *>();

    if (cryptoService == nullptr) {
        cryptoService = createInstance(nullptr);
    }

    return cryptoService;
}

/**
 * Creates a global instance of the class
 */
CryptoService *CryptoService::createInstance(QObject *parent) {
    auto *cryptoService = new CryptoService(parent);

    qApp->setProperty("cryptoService", QVariant::fromValue<CryptoService *>(cryptoService));

    return cryptoService;
}

/**
 * Encrypts a string as string
 */
QString CryptoService::encryptToString(const QString &text) {
    if (text.isEmpty()) {
        return QString();
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    const QString uuid = QUuid::createUuid().toString(QUuid::WithoutBraces);
#else
    QString uuid = QUuid::createUuid().toString();
    uuid.remove(QLatin1Char('{'));
    uuid.remove(QLatin1Char('}'));
#endif

    const QString key = QStringLiteral("secrets/") + uuid;
    return storeSecret(key, text);
}

/**
 * Stores a string in the keychain under a stable key and returns the stored reference.
 */
QString CryptoService::encryptToString(const QString &text, const QString &key) {
    if (text.isEmpty()) {
        return QString();
    }

    return storeSecret(key, text);
}

/**
 * Decrypts a string as string
 */
QString CryptoService::decryptToString(const QString &text) {
    const QString key = keyFromMarker(text);

    if (!key.isEmpty()) {
        return readSecret(key);
    }

    return legacyDecryptToString(text);
}

bool CryptoService::isKeychainReference(const QString &text) const {
    return text.startsWith(KeychainMarkerPrefix);
}

QString CryptoService::legacyEncryptToString(const QString &text) {
    return _simpleCrypt->encryptToString(text);
}

QString CryptoService::legacyDecryptToString(const QString &text) {
    return _simpleCrypt->decryptToString(text);
}

QString CryptoService::markerForKey(const QString &key) const {
    return KeychainMarkerPrefix +
           QString::fromLatin1(key.toUtf8().toBase64(QByteArray::Base64UrlEncoding |
                                                     QByteArray::OmitTrailingEquals));
}

QString CryptoService::keyFromMarker(const QString &text) const {
    if (!isKeychainReference(text)) {
        return QString();
    }

    const QByteArray encoded = text.mid(QString(KeychainMarkerPrefix).length()).toLatin1();
    return QString::fromUtf8(QByteArray::fromBase64(
        encoded, QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals));
}

QString CryptoService::storeSecret(const QString &key, const QString &text) {
    if (writeSecret(key, text)) {
        return markerForKey(key);
    }

    qWarning() << "Could not store secret in keychain, falling back to legacy encryption:" << key;
    return legacyEncryptToString(text);
}

QString CryptoService::readSecret(const QString &key) const {
    auto *job = new QKeychain::ReadPasswordJob(keychainServiceName());
    job->setKey(key);

    const bool ok = executeKeychainJob(job);
    const QString text = ok ? job->textData() : QString();

    if (!ok && job->error() != QKeychain::EntryNotFound) {
        qWarning() << "Could not read secret from keychain:" << key << job->errorString();
    }

    delete job;
    return text;
}

bool CryptoService::writeSecret(const QString &key, const QString &text) const {
    auto *job = new QKeychain::WritePasswordJob(keychainServiceName());
    job->setKey(key);
    job->setTextData(text);

    const bool ok = executeKeychainJob(job);

    if (!ok) {
        qWarning() << "Could not write secret to keychain:" << key << job->errorString();
    }

    delete job;
    return ok;
}

bool CryptoService::migrateSecret(QString *storedValue, const QString &key,
                                  bool allowPlaintextFallback) {
    if (storedValue == nullptr || storedValue->isEmpty() || isKeychainReference(*storedValue)) {
        return false;
    }

    QString plainText = legacyDecryptToString(*storedValue);

    if (plainText.isEmpty() && allowPlaintextFallback) {
        plainText = *storedValue;
    }

    if (plainText.isEmpty() || !writeSecret(key, plainText)) {
        return false;
    }

    *storedValue = markerForKey(key);
    return true;
}

void CryptoService::migrateSecrets() {
    if (_migrationStarted) {
        return;
    }

    _migrationStarted = true;
    migrateSettingsSecrets();
    migrateCloudConnectionSecrets();
    migrateScriptSecrets();
}

void CryptoService::migrateSettingsSecrets() {
    const QStringList encryptedSettingsKeys = {
        QStringLiteral("networking/proxyPassword"),
        QStringLiteral("ownCloud/password"),
        QStringLiteral("ownCloud/todoCalendarCalDAVPassword"),
        QStringLiteral("ai/groq/apiKey"),
        QStringLiteral("ai/openai/apiKey"),
    };
    const QStringList plaintextFallbackSettingsKeys = {
        QStringLiteral("webSocketServerService/bookmarkSuggestionApiToken"),
        QStringLiteral("ai/mcpServerToken"),
    };

    SettingsService settings;

    for (const QString &settingsKey : encryptedSettingsKeys) {
        QString value = settings.value(settingsKey).toString();

        if (migrateSecret(&value, QStringLiteral("settings/") + settingsKey, false)) {
            settings.setValue(settingsKey, value);
        }
    }

    for (const QString &settingsKey : plaintextFallbackSettingsKeys) {
        QString value = settings.value(settingsKey).toString();

        if (migrateSecret(&value, QStringLiteral("settings/") + settingsKey, true)) {
            settings.setValue(settingsKey, value);
        }
    }
}

void CryptoService::migrateCloudConnectionSecrets() {
    if (!QSqlDatabase::contains(QStringLiteral("disk"))) {
        return;
    }

    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));

    if (!db.isOpen()) {
        return;
    }

    QSqlQuery selectQuery(db);
    selectQuery.prepare(QStringLiteral("SELECT id, password FROM cloudConnection"));

    if (!selectQuery.exec()) {
        return;
    }

    while (selectQuery.next()) {
        const int id = selectQuery.value(QStringLiteral("id")).toInt();
        QString password = selectQuery.value(QStringLiteral("password")).toString();

        if (!migrateSecret(&password,
                           QStringLiteral("database/cloudConnection/%1/password").arg(id), false)) {
            continue;
        }

        QSqlQuery updateQuery(db);
        updateQuery.prepare(
            QStringLiteral("UPDATE cloudConnection SET password = :password WHERE id = :id"));
        updateQuery.bindValue(QStringLiteral(":password"), password);
        updateQuery.bindValue(QStringLiteral(":id"), id);

        if (!updateQuery.exec()) {
            qWarning() << "Could not migrate cloud connection password to keychain:"
                       << updateQuery.lastError();
        }
    }
}

void CryptoService::migrateScriptSecrets() {
    if (!QSqlDatabase::contains(QStringLiteral("disk"))) {
        return;
    }

    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));

    if (!db.isOpen()) {
        return;
    }

    QSqlQuery selectQuery(db);
    selectQuery.prepare(QStringLiteral("SELECT id, settings_variables_json FROM script"));

    if (!selectQuery.exec()) {
        return;
    }

    while (selectQuery.next()) {
        const int id = selectQuery.value(QStringLiteral("id")).toInt();
        const QString settingsVariablesJson =
            selectQuery.value(QStringLiteral("settings_variables_json")).toString();
        QJsonObject jsonObject = QJsonDocument::fromJson(settingsVariablesJson.toUtf8()).object();
        bool changed = false;

        for (auto it = jsonObject.begin(); it != jsonObject.end(); ++it) {
            if (!it.key().startsWith(QStringLiteral("!"))) {
                continue;
            }

            QString value = it.value().toString();
            const QString secretKey =
                QStringLiteral("database/script/%1/settings/%2").arg(id).arg(it.key());

            if (migrateSecret(&value, secretKey, false)) {
                it.value() = value;
                changed = true;
            }
        }

        if (!changed) {
            continue;
        }

        QSqlQuery updateQuery(db);
        updateQuery.prepare(
            QStringLiteral("UPDATE script SET settings_variables_json = "
                           ":settings_variables_json WHERE id = :id"));
        updateQuery.bindValue(QStringLiteral(":settings_variables_json"),
                              QString::fromUtf8(QJsonDocument(jsonObject).toJson()));
        updateQuery.bindValue(QStringLiteral(":id"), id);

        if (!updateQuery.exec()) {
            qWarning() << "Could not migrate script secret to keychain:" << updateQuery.lastError();
        }
    }
}
