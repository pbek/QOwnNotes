/*
 * Copyright (c) 2014-2025 Patrizio Bekerle -- <patrizio@bekerle.com>
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

QMap<QString, int> ScriptThread::threadCounter;

ScriptThread::ScriptThread(ScriptingService *ss, const TerminalCmd &cmd, const QString &identifier,
                           const QVariant &callbackParameter)
    : QThread() {
    this->cmd = cmd;
    this->identifier = identifier;
    this->callbackParameter = callbackParameter;
    increaseThreadCounter();
    ScriptThread::connect(this, SIGNAL(callBack(ScriptThread *)), ss,
                          SLOT(onScriptThreadDone(ScriptThread *)));
}

void ScriptThread::increaseThreadCounter() { threadCounter[identifier]++; }

void ScriptThread::decreaseThreadCounter() { threadCounter[identifier]--; }

int ScriptThread::getThreadCounter() { return threadCounter[identifier]; }

// We override the QThread's run() method here
// run() will be called when a thread starts
// the code will be shared by all threads
void ScriptThread::run() {
    Utils::Misc::startSynchronousResultProcess(cmd);
    decreaseThreadCounter();
    emit callBack(this);    // emit is not a function
}
