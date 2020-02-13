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
                              const QString& connectionName = QString("disk"));
    static bool setAppData(const QString& name, const QString& value,
                           const QString& connectionName = QString("disk"));
    static bool reinitializeDiskDatabase();
    static bool removeDiskDatabase();
    static bool createNoteFolderConnection();
    static bool setupNoteFolderTables();
    static QString getDiskDatabasePath();
    static QSqlDatabase getNoteFolderDatabase();
    static void closeDatabaseConnection(const QSqlDatabase& db,
                                        QSqlQuery& query);
    static QString getNoteFolderDatabasePath();

   private:
    static bool createMemoryConnection();
    static bool createDiskConnection();
    static bool mergeNoteFolderDatabase(QString path);
};

#endif    // DATABASESERVICE_H
