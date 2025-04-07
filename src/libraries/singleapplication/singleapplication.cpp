// The MIT License (MIT)
//
// Copyright (c) Itay Grudev 2015 - 2018
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <QElapsedTimer>
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>
#include <QtCore/QSharedMemory>
#include <QtCore/QThread>
#include <QtCore/QTime>
#include <QtCore/QUuid>

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QtCore/QRandomGenerator>
#endif

#include "singleapplication.h"
#include "singleapplication_p.h"

/**
 * @brief Constructor. Checks and fires up LocalServer or closes the program
 * if another instance already exists
 * @param argc
 * @param argv
 * @param {bool} allowSecondaryInstances
 */
SingleApplication::SingleApplication(int &argc, char *argv[], bool allowSecondary, Options options,
                                     int timeout, std::function<void()> instanceAbortedFunction)
    : app_t(argc, argv), d_ptr(new SingleApplicationPrivate(this)) {
    Q_D(SingleApplication);

    // Store the current mode of the program
    d->options = options;

    // Generating an application ID used for identifying the shared memory
    // block and QLocalServer
    d->genBlockServerName();

#ifdef Q_OS_UNIX
    // By explicitly attaching it and then deleting it we make sure that the
    // memory is deleted even after the process has crashed on Unix.
    d->memory = new QSharedMemory(d->blockServerName);
    d->memory->attach();
    delete d->memory;
#endif
    // Guarantee thread safe behaviour with a shared memory block.
    d->memory = new QSharedMemory(d->blockServerName);

    // Create a shared memory block
    if (d->memory->create(sizeof(InstancesInfo))) {
        // Initialize the shared memory block
        d->memory->lock();
        d->initializeMemoryBlock();
        d->memory->unlock();
    } else {
        // Attempt to attach to the memory segment
        if (!d->memory->attach()) {
            qCritical() << "SingleApplication: Unable to attach to shared memory block.";
            qCritical() << d->memory->errorString();
            delete d;
            ::exit(EXIT_FAILURE);
        }
    }

    InstancesInfo *inst = static_cast<InstancesInfo *>(d->memory->data());
    QElapsedTimer time;
    time.start();

    // Make sure the shared memory block is initialised and in consistent state
    while (true) {
        d->memory->lock();

        if (d->blockChecksum() == inst->checksum) break;

        if (time.elapsed() > 5000) {
            qWarning() << "SingleApplication: Shared memory block has been in an inconsistent "
                          "state from more than 5s. Assuming primary instance failure.";
            d->initializeMemoryBlock();
        }

        d->memory->unlock();

        // Random sleep here limits the probability of a colision between two racing apps
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
        qsrand(QDateTime::currentMSecsSinceEpoch() % std::numeric_limits<uint>::max());
        const quint32 number = qrand();
#else
        const quint32 number = QRandomGenerator::global()->generate();
#endif

        QThread::sleep(
            8 + static_cast<unsigned long>(static_cast<float>(number) / (float)RAND_MAX * 10.f));
    }

    if (inst->primary == false) {
        d->startPrimary();
        d->memory->unlock();
        return;
    }

    // Check if another instance can be started
    if (allowSecondary) {
        inst->secondary += 1;
        inst->checksum = d->blockChecksum();
        d->instanceNumber = inst->secondary;
        d->startSecondary();
        if (d->options & Mode::SecondaryNotification) {
            d->connectToPrimary(timeout, SingleApplicationPrivate::SecondaryInstance);
        }
        d->memory->unlock();
        return;
    }

    d->memory->unlock();

    d->connectToPrimary(timeout, SingleApplicationPrivate::NewInstance);

    delete d;

    // Call user function if application will be aborted
    if (instanceAbortedFunction != nullptr) {
        instanceAbortedFunction();
    }

    ::exit(EXIT_SUCCESS);
}

/**
 * @brief Checks if SingleInstance is supported on this system
 * @returns {bool}
 */
bool SingleApplication::isSupported() {
    QString uuid = QUuid::createUuid().toString();
    auto *memory = new QSharedMemory(uuid);

    if (!memory->create(1)) {
        qWarning() << "Unable to create shared memory segment!";
        qWarning() << memory->errorString();
        delete memory;

        return false;
    }

    memory->lock();
    char *to = (char *)memory->data();
    const char from = 'x';
    memcpy(to, &from, 1);
    memory->unlock();

    auto *memory2 = new QSharedMemory(uuid);
    if (!memory2->attach()) {
        qWarning() << "Unable to attach to shared memory segment!";
        qWarning() << memory2->errorString();

        delete memory;
        delete memory2;

        return false;
    }

    memory->detach();
    delete memory;

    memory2->detach();
    delete memory2;

    return true;
}

/**
 * @brief Destructor
 */
SingleApplication::~SingleApplication() {
    Q_D(SingleApplication);
    delete d;
}

bool SingleApplication::isPrimary() {
    Q_D(SingleApplication);
    return d->server != nullptr;
}

bool SingleApplication::isSecondary() {
    Q_D(SingleApplication);
    return d->server == nullptr;
}

quint32 SingleApplication::instanceId() {
    Q_D(SingleApplication);
    return d->instanceNumber;
}

qint64 SingleApplication::primaryPid() {
    Q_D(SingleApplication);
    return d->primaryPid();
}

bool SingleApplication::sendMessage(const QByteArray &message, int timeout) {
    Q_D(SingleApplication);

    // Nobody to connect to
    if (isPrimary()) return false;

    // Make sure the socket is connected
    d->connectToPrimary(timeout, SingleApplicationPrivate::Reconnect);

    d->socket->write(message);
    bool dataWritten = d->socket->flush();
    d->socket->waitForBytesWritten(timeout);
    return dataWritten;
}
