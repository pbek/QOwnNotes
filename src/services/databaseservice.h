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
    static QString getAppData(QString name);
    static bool setAppData(QString name, QString value);
};

#endif // DATABASESERVICE_H
