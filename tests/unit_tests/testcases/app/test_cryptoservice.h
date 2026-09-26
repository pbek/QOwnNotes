#pragma once

#include <QObject>

class TestCryptoService : public QObject {
    Q_OBJECT

   private slots:
    void testMigrateEncryptedEmptySecrets();
    void testPreserveUndecryptableSecrets();
};
