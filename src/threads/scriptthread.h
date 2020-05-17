/**
 * ScriptThread header
 */

#pragma once

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QThread>
#include <QString>

class ScriptThread : public QThread
{
    Q_OBJECT
public:
    // constructor
    // set name using initializer
    explicit ScriptThread(QString s, QObject *start){
        //calling "start" will automatically run the `run` function on the new thread
        ScriptThread::connect(start, SIGNAL(clicked()), this, SLOT(start()));
       //use queued connection, this way the slot will be executed on the handlers thread
       ScriptThread::connect(this, SIGNAL(sendPacket(Packet*)),
                              handler, SLOT(receivePacket(Packet*)), Qt::QueuedConnection);
     }

    // overriding the QThread's run() method
    void run();
public slots:
    void acceptConnection(QString port, int current);

signals:
    void newConnection(QString, int);
    void callBack(QString);
private:
    QString name;
};
