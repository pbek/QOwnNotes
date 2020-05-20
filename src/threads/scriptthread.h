/**
 * ScriptThread header
 */

#pragma once

#include <QObject>
#include <QtCore/QDebug>
#include <QThread>
#include <QString>
#include <services/scriptingservice.h>
#include <utils/misc.h>
class ScriptingService;

struct TerminalCmd {
    QString executablePath;
    QStringList parameters;
    QByteArray data;
    int exitCode;
    QByteArray resultSet;
};

class ScriptThread : public QThread
{
    Q_OBJECT
private:
    static QMap<QString, int> threadCounter;
    TerminalCmd cmd;   // cmd struct
    QString identifier; // id for the callback
    QVariant callbackParameter;      // addition information for the callback

public:
    // constructor
    explicit ScriptThread(ScriptingService *ss, const TerminalCmd &cmd, const QString &identifier, const QVariant callbackParameter = 0);

    // overriding the QThread's run() method
    void run();
    TerminalCmd* getTerminalCmd() {return &cmd;}
    QString getIdentifier() {return identifier;}
    QVariant getIndex() {return callbackParameter;}
    void increaseThreadCounter();
    void decreaseThreadCounter();
    int getThreadCounter();
signals:
    void callBack(ScriptThread*);
public slots:

};
