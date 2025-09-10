#ifndef DATABASESERVICE_H
#define DATABASESERVICE_H

#include <QObject>
#include <QString>
#include <QtSql/QSqlDatabase>

class DatabaseService {
   public:
    DatabaseService();
    static bool createConnection();
    static bool setupTables();
    static QString getAppData(const QString& name,
                              const QString& connectionName = QStringLiteral("disk"));
    static bool setAppData(const QString& name, const QString& value,
                           const QString& connectionName = QStringLiteral("disk"));
    static bool reinitializeDiskDatabase();
    static bool removeDiskDatabase();
    static bool createNoteFolderConnection();
    static bool setupNoteFolderTables();
    static QString getDiskDatabasePath();
    static QSqlDatabase getNoteFolderDatabase();
    static void closeDatabaseConnection(QSqlDatabase& db, QSqlQuery& query);
    static QString getNoteFolderDatabasePath();
    static bool mergeNoteFolderDatabase(const QString& path);
    static QByteArray generateDatabaseTableSha1Signature(QSqlDatabase& db, const QString& table);
    static bool checkDiskDatabaseIntegrity();
    static QString generateConnectionName();
    static QSqlDatabase createSharedMemoryDatabase(
        const QString& connectionName = QStringLiteral("memory"));
    static QSqlDatabase getSharedMemoryDatabase(
        const QString& connectionName = QStringLiteral("memory"));

   private:
    static bool createMemoryConnection();
    static bool createDiskConnection();
};

#endif    // DATABASESERVICE_H
