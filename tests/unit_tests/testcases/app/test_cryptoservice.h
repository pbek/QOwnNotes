#pragma once

#include <QObject>

class TestCryptoService : public QObject {
    Q_OBJECT

   private slots:
    void testKeychainMigrationNoticeAcknowledgment();
    void testAcknowledgmentDoesNotPreventMigration();
    void testMigrateEncryptedEmptySecrets();
    void testPreserveUndecryptableSecrets();
};
