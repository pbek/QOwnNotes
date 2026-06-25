#pragma once

// enable debugging for the Piwik tracker
#define PIWIK_TRACKER_DEBUG 1

#include <libraries/simplecrypt/simplecrypt.h>

#include <QObject>
#include <QSettings>
#include <QStringList>

class CryptoService : public QObject {
    Q_OBJECT

   public:
    explicit CryptoService(QObject *parent = 0);
    static CryptoService *instance();
    static CryptoService *createInstance(QObject *parent);
    QString encryptToString(const QString &text);
    QString encryptToString(const QString &text, const QString &key);
    QString decryptToString(const QString &text);
    QString decryptToStringWithPlaintextFallback(const QString &text);
    static bool isKeychainReference(const QString &text);
    bool deleteSecret(const QString &storedValueOrKey) const;
    void deleteSecrets(const QStringList &storedValuesOrKeys) const;
    static QStringList keychainReferencesFromSettings(const QSettings &settings);
    static QStringList keychainReferencesFromDiskDatabase();

   private:
    SimpleCrypt *_simpleCrypt;
    bool _migrationStarted;

    QString legacyEncryptToString(const QString &text);
    QString legacyDecryptToString(const QString &text);
    QString markerForKey(const QString &key) const;
    QString keyFromMarker(const QString &text) const;
    QString storeSecret(const QString &key, const QString &text);
    QString readSecret(const QString &key) const;
    bool writeSecret(const QString &key, const QString &text) const;
    bool migrateSecret(QString *storedValue, const QString &key, bool allowPlaintextFallback);
    void migrateSecrets();
    void migrateSettingsSecrets();
    void migrateCloudConnectionSecrets();
    void migrateScriptSecrets();

   signals:

   public slots:
};
