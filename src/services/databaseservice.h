#ifndef DATABASESERVICE_H
#define DATABASESERVICE_H

#include <QObject>
#include <QString>
#include <QtSql/QSqlDatabase>


class DatabaseService
{

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
    static void closeDatabaseConnection(QSqlDatabase &db, QSqlQuery &query);

private:
    static QString getNoteFolderDatabasePath();
    static bool createMemoryConnection();
    static bool createDiskConnection();
};

#endif // DATABASESERVICE_H
