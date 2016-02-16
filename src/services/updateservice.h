#ifndef UPDATESERVICE_H
#define UPDATESERVICE_H

#include <qnetworkreply.h>
class MainWindow;

#ifdef Q_OS_LINUX
#define PLATFORM "linux"
#endif

#ifdef Q_OS_MAC
#define PLATFORM "macosx"
#endif

#ifdef Q_OS_WIN32
#define PLATFORM "windows"
#endif

#ifndef PLATFORM
#define PLATFORM "other"
#endif

class UpdateService : public QObject {
Q_OBJECT

public:
    enum UpdateMode {
        AppStart = 1,
        Manual,
        Periodic
    };
    Q_ENUMS(UpdateMode)

    explicit UpdateService(QObject *parent = 0);

    void checkForUpdates(
            MainWindow *mainWindow, UpdateMode updateMode = AppStart);

private:
    MainWindow *mainWindow;
    UpdateMode updateMode;

private slots:

    void onResult(QNetworkReply *reply);
};

#endif // UPDATESERVICE_H
