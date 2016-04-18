#include <cstdlib>

#include <QtCore/QSharedMemory>
#include <QtCore/QMutex>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>

#ifdef Q_OS_UNIX
    #include <signal.h>
    #include <unistd.h>
#endif

#include "singleapplication.h"


class SingleApplicationPrivate {
    Q_DECLARE_PUBLIC(SingleApplication)

public:
    SingleApplicationPrivate(SingleApplication *q_ptr) : q_ptr(q_ptr) { }

    void startServer(QString &serverName)
    {
        Q_Q(SingleApplication);

        // Start a QLocalServer to listen for connections
        server = new QLocalServer();
        QLocalServer::removeServer(serverName);
        server->listen(serverName);
        QObject::connect(server, SIGNAL(newConnection()), q, SLOT(slotConnectionEstablished()));
    }

#ifdef Q_OS_UNIX
    void crashHandler()
    {
        // This guarantees the program will work even with multiple
        // instances of SingleApplication in different threads.
        // Which in my opinion is idiotic, but lets handle that too.
        {
            sharedMemMutex.lock();
            sharedMem.append(memory);
            sharedMemMutex.unlock();
        }
        // Handle any further termination signals to ensure the
        // QSharedMemory block is deleted even if the process crashes
        signal(SIGSEGV, SingleApplicationPrivate::terminate);
        signal(SIGABRT, SingleApplicationPrivate::terminate);
        signal(SIGFPE,  SingleApplicationPrivate::terminate);
        signal(SIGILL,  SingleApplicationPrivate::terminate);
        signal(SIGINT,  SingleApplicationPrivate::terminate);
        signal(SIGTERM, SingleApplicationPrivate::terminate);
    }

    static void terminate(int signum)
    {
        while( ! sharedMem.empty() ) {
            delete sharedMem.back();
            sharedMem.pop_back();
        }
        ::exit(128 + signum);
    }

    static QList<QSharedMemory*> sharedMem;
    static QMutex sharedMemMutex;
#endif

    QSharedMemory *memory;
    SingleApplication *q_ptr;
    QLocalServer  *server;
    QLocalSocket  *socket;
};

#ifdef Q_OS_UNIX
    QList<QSharedMemory*> SingleApplicationPrivate::sharedMem;
    QMutex SingleApplicationPrivate::sharedMemMutex;
#endif

/**
 * @brief Constructor. Checks and fires up LocalServer or closes the program
 * if another instance already exists
 * @param argc
 * @param argv
 */
SingleApplication::SingleApplication(int &argc, char *argv[])
    : app_t(argc, argv), d_ptr(new SingleApplicationPrivate(this))
{
    Q_D(SingleApplication);

    QString serverName = app_t::organizationName() + app_t::applicationName();
    serverName.replace(QRegExp("[^\\w\\-. ]"), "");

    // Guarantee thread safe behaviour with a shared memory block
    d->memory = new QSharedMemory(serverName);

    // Create a shared memory block with a minimum size of 1 byte
    if( d->memory->create(1, QSharedMemory::ReadOnly) )
    {
#ifdef Q_OS_UNIX
        // Handle any further termination signals to ensure the
        // QSharedMemory block is deleted even if the process crashes
        d->crashHandler();
#endif
        // Successful creation means that no main process exists
        // So we start a Local Server to listen for connections
        d->startServer(serverName);
    } else {
        // Connect to the Local Server of the main process to notify it
        // that a new process had been started
        d->socket = new QLocalSocket();
        d->socket->connectToServer(serverName);

        // Even though a shared memory block exists, the original application
        // might have crashed.
        // So only after a successful connection is the second instance
        // terminated.
        if( d->socket->waitForConnected(100) )
        {
            // Terminate the program using STDLib's exit function
            ::exit(EXIT_SUCCESS);
        } else {
            delete d->memory;
            ::exit(EXIT_SUCCESS);
        }
    }
}

/**
 * @brief Destructor
 */
SingleApplication::~SingleApplication()
{
    Q_D(SingleApplication);

    delete d->memory;
    d->server->close();
}

/**
 * @brief Executed when the showUp command is sent to LocalServer
 */
void SingleApplication::slotConnectionEstablished()
{
    Q_D(SingleApplication);

    QLocalSocket *socket = d->server->nextPendingConnection();
    socket->close();
    delete socket;
    Q_EMIT showUp();
}
