/*
 * Copyright (c) 2014-2020 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "scriptthread.h"
#include <QDebug>

ScriptThread::ScriptThread(QString s) : name(s)
{
   // connect(this, SIGNAL(newConnection(QString,int)), &myClass, SLOT(acceptConnection(QString,int)));
    connect(this, SIGNAL(newConnection(QString,int)),this, SLOT(acceptConnection(QString,int)));
      emit newConnection("test", 1234);
}

// We overrides the QThread's run() method here
// run() will be called when a thread starts
// the code will be shared by all threads

void ScriptThread::run()
{
    for(int i = 0; i <= 100; i++)
    {
        qDebug() << this->name << " " << i;
    }

    emit callBack("done") ;//emit is not a function
    qDebug() << "Sending packet!";//you can use qDebug
}

void ScriptThread::acceptConnection(QString port, int current) {
    qDebug() << "received data for port " << port;
}
