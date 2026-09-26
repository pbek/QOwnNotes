#include "test_cryptoservice.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QtTest>

#include "services/cryptoservice.h"
#include "services/settingsservice.h"

void TestCryptoService::testMigrateEncryptedEmptySecrets() {
    SettingsService settings;
    settings.clear();
    const quint64 key = 1234567;
    settings.setValue(QStringLiteral("cryptoKey"), key);

    SimpleCrypt crypt(key);
    const QString encryptedEmpty = crypt.encryptToString(QString());
    QVERIFY(!encryptedEmpty.isEmpty());
    settings.setValue(QStringLiteral("ownCloud/todoCalendarCalDAVPassword"), encryptedEmpty);
    settings.setValue(QStringLiteral("ai/groq/apiKey"), encryptedEmpty);
    settings.setValue(QStringLiteral("ai/openai/apiKey"), encryptedEmpty);
    settings.setValue(QStringLiteral("webSocketServerService/token"), encryptedEmpty);

    QSqlDatabase db = QSqlDatabase::database(QStringLiteral("disk"));
    QVERIFY(db.isOpen());
    QSqlQuery cloudQuery(db);
    cloudQuery.prepare(QStringLiteral("INSERT INTO cloudConnection (password) VALUES (:password)"));
    cloudQuery.bindValue(QStringLiteral(":password"), encryptedEmpty);
    QVERIFY(cloudQuery.exec());
    const int cloudId = cloudQuery.lastInsertId().toInt();

    QJsonObject scriptSettings;
    scriptSettings.insert(QStringLiteral("!password"), encryptedEmpty);
    QSqlQuery scriptQuery(db);
    scriptQuery.prepare(
        QStringLiteral("INSERT INTO script (settings_variables_json) VALUES (:settings)"));
    scriptQuery.bindValue(QStringLiteral(":settings"),
                          QString::fromUtf8(QJsonDocument(scriptSettings).toJson()));
    QVERIFY(scriptQuery.exec());
    const int scriptId = scriptQuery.lastInsertId().toInt();

    QVERIFY(CryptoService::hasLegacySecretsToMigrate());
    CryptoService service;

    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarCalDAVPassword")).toString(),
             QString());
    QCOMPARE(settings.value(QStringLiteral("ai/groq/apiKey")).toString(), QString());
    QCOMPARE(settings.value(QStringLiteral("ai/openai/apiKey")).toString(), QString());
    QCOMPARE(settings.value(QStringLiteral("webSocketServerService/token")).toString(), QString());
    QSqlQuery verifyQuery(db);
    verifyQuery.prepare(QStringLiteral("SELECT password FROM cloudConnection WHERE id = :id"));
    verifyQuery.bindValue(QStringLiteral(":id"), cloudId);
    QVERIFY(verifyQuery.exec());
    QVERIFY(verifyQuery.next());
    QCOMPARE(verifyQuery.value(0).toString(), QString());

    verifyQuery.prepare(
        QStringLiteral("SELECT settings_variables_json FROM script WHERE id = :id"));
    verifyQuery.bindValue(QStringLiteral(":id"), scriptId);
    QVERIFY(verifyQuery.exec());
    QVERIFY(verifyQuery.next());
    const QJsonObject migratedScriptSettings =
        QJsonDocument::fromJson(verifyQuery.value(0).toString().toUtf8()).object();
    QCOMPARE(migratedScriptSettings.value(QStringLiteral("!password")).toString(), QString());
    QVERIFY(!CryptoService::hasLegacySecretsToMigrate());

    QSqlQuery cleanup(db);
    QVERIFY(cleanup.exec(QStringLiteral("DELETE FROM cloudConnection WHERE id = %1").arg(cloudId)));
    QVERIFY(cleanup.exec(QStringLiteral("DELETE FROM script WHERE id = %1").arg(scriptId)));
    settings.clear();
}

void TestCryptoService::testPreserveUndecryptableSecrets() {
    SettingsService settings;
    settings.clear();
    const quint64 key = 1234567;
    settings.setValue(QStringLiteral("cryptoKey"), key);

    SimpleCrypt crypt(key);
    QByteArray damaged =
        QByteArray::fromBase64(crypt.encryptToString(QStringLiteral("password")).toLatin1());
    damaged[damaged.size() - 1] = damaged.at(damaged.size() - 1) ^ 1;
    const QString corrupted = QString::fromLatin1(damaged.toBase64());
    const QString truncated = QStringLiteral("AwI=");
    const QString unchecked = QStringLiteral("AwAA");
    settings.setValue(QStringLiteral("ownCloud/password"), corrupted);
    settings.setValue(QStringLiteral("ai/groq/apiKey"), truncated);
    settings.setValue(QStringLiteral("ai/openai/apiKey"), unchecked);

    CryptoService service;

    QCOMPARE(settings.value(QStringLiteral("ownCloud/password")).toString(), corrupted);
    QCOMPARE(settings.value(QStringLiteral("ai/groq/apiKey")).toString(), truncated);
    QCOMPARE(settings.value(QStringLiteral("ai/openai/apiKey")).toString(), unchecked);
    QVERIFY(CryptoService::hasLegacySecretsToMigrate());
    settings.clear();
}
