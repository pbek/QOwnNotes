#pragma once

#include <QSqlQuery>
#include <QList>

class Script
{
public:
    explicit Script();

    int getId();
    static bool create(QString name, QString scriptPath);
    static Script fetch(int id);
    static Script scriptFromQuery(QSqlQuery query);
    bool store();
    friend QDebug operator<<(QDebug dbg, const Script &script);
    bool exists();
    bool fillFromQuery(QSqlQuery query);
    bool remove();
    bool isFetched();
    static QList<Script> fetchAll(bool enabledOnly = false);
    QString getName();
    QString getScriptPath();
    int getPriority();
    void setName(QString text);
    void setScriptPath(QString text);
    void setPriority(int value);
    static int countAll();
    bool scriptPathExists();
    void setEnabled(bool value);
    bool getEnabled();
    bool isEnabled();
    static int countEnabled();

private:
    int id;
    QString name;
    QString scriptPath;
    int priority;
    bool enabled;
};
