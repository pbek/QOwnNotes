#pragma once

#include <QObject>
#include <QNetworkProxy>
#include <QRunnable>
#include <QUrl>


/**
 * @brief The ClientProxy class
 */
class ClientProxy : public QObject
{
    Q_OBJECT
public:
    explicit ClientProxy(QObject *parent = 0);

    static bool isUsingSystemDefault();
    static void lookupSystemProxyAsync(
            const QUrl &url, QObject *dst, const char *slot);
    static QNetworkProxy proxyFromSettings();

public slots:
    void setupQtProxyFromSettings();

private:
    const char* proxyTypeToCStr(QNetworkProxy::ProxyType type);
};

class SystemProxyRunnable : public QObject, public QRunnable {
    Q_OBJECT
public:
    SystemProxyRunnable(const QUrl &url);
    void run();
signals:
    void systemProxyLookedUp(const QNetworkProxy &url);
private:
    QUrl _url;
};

QString printQNetworkProxy(const QNetworkProxy &proxy);
