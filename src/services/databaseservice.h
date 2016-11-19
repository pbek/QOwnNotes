#ifndef DATABASESERVICE_H
#define DATABASESERVICE_H

#include <QObject>
#include <QString>


class DatabaseService
{

public:
    DatabaseService();
    static bool createConnection();
    static bool setupTables();
    static QString getAppData(QString name,
                              QString connectionName = QString("disk"));
    static bool setAppData(QString name, QString value,
                           QString connectionName = QString("disk"));
    static bool reinitializeDiskDatabase();
    static bool removeDiskDatabase();
    static bool createNoteFolderConnection();
    static bool setupNoteFolderTables();
    static QString getDiskDatabasePath();

private:
    static QString getNoteFolderDatabasePath();
    static bool createMemoryConnection();
    static bool createDiskConnection();
};

#endif // DATABASESERVICE_H
