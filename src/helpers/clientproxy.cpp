#include "clientproxy.h"
#include "services/cryptoservice.h"
#include <QUrl>
#include <QThreadPool>
#include <QSettings>

ClientProxy::ClientProxy(QObject *parent) :
    QObject(parent)
{
}

QNetworkProxy ClientProxy::proxyFromSettings()
{
    QNetworkProxy proxy;
    QSettings settings;
    QString proxyHostName =
            settings.value("networking/proxyHostName").toString();

    if (proxyHostName.isEmpty()) {
        return QNetworkProxy();
    }

    int proxyPort = settings.value("networking/proxyPort").toInt();
    int proxyNeedsAuth = settings.value("networking/proxyNeedsAuth").toBool();

    proxy.setHostName(proxyHostName);
    proxy.setPort(proxyPort);

    if (proxyNeedsAuth) {
        QString proxyUser = settings.value("networking/proxyUser").toString();
        QString proxyPassword =
                settings.value("networking/proxyPassword").toString();
        proxy.setUser(proxyUser);
        proxy.setPassword(
                CryptoService::instance()->decryptToString(proxyPassword));
    }

    return proxy;
}

bool ClientProxy::isUsingSystemDefault() {
    QSettings settings;
    int proxyType = settings.value("networking/proxyType",
                                   QNetworkProxy::DefaultProxy).toInt();
    return proxyType == QNetworkProxy::DefaultProxy;
}

QString printQNetworkProxy(const QNetworkProxy &proxy)
{
    return QString("%1://%2:%3").arg(proxy.type()).arg(proxy.hostName())
            .arg(proxy.port());
}

void ClientProxy::setupQtProxyFromSettings()
{
    QSettings settings;
    int proxyType = settings.value("networking/proxyType",
                                   QNetworkProxy::DefaultProxy).toInt();
    QNetworkProxy proxy;

    switch (proxyType) {
    case QNetworkProxy::NoProxy:
        qDebug() << "Set proxy configuration to use NO proxy";
        QNetworkProxyFactory::setUseSystemConfiguration(false);
        QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
        break;
    case QNetworkProxy::DefaultProxy:
        qDebug() << "Set proxy configuration to use system configuration";
        QNetworkProxyFactory::setUseSystemConfiguration(true);
        break;
    case QNetworkProxy::Socks5Proxy:
        proxy.setType(QNetworkProxy::Socks5Proxy);
        qDebug() << "Set proxy configuration to SOCKS5"
            << printQNetworkProxy(proxy);
        QNetworkProxyFactory::setUseSystemConfiguration(false);
        QNetworkProxy::setApplicationProxy(proxy);
        break;
    case QNetworkProxy::HttpProxy:
        proxy.setType(QNetworkProxy::HttpProxy);
        qDebug() << "Set proxy configuration to HTTP"
            << printQNetworkProxy(proxy);
        QNetworkProxyFactory::setUseSystemConfiguration(false);
        QNetworkProxy::setApplicationProxy(proxy);
        break;
    default:
        break;
    }
}

const char* ClientProxy::proxyTypeToCStr(QNetworkProxy::ProxyType type)
{
    switch (type) {
    case QNetworkProxy::NoProxy:
        return "NoProxy";
    case QNetworkProxy::DefaultProxy:
        return "DefaultProxy";
    case QNetworkProxy::Socks5Proxy:
        return "Socks5Proxy";
    case QNetworkProxy::HttpProxy:
        return "HttpProxy";
    case QNetworkProxy::HttpCachingProxy:
        return "HttpCachingProxy";
    case QNetworkProxy::FtpCachingProxy:
        return "FtpCachingProxy";
    default:
        return "NoProxy";
    }
}

void ClientProxy::lookupSystemProxyAsync(
        const QUrl &url, QObject *dst, const char *slot)
{
    SystemProxyRunnable *runnable = new SystemProxyRunnable(url);
    QObject::connect(
            runnable,
            SIGNAL(systemProxyLookedUp(QNetworkProxy)),
            dst,
            slot);

    // takes ownership and deletes
    QThreadPool::globalInstance()->start(runnable);
}

SystemProxyRunnable::SystemProxyRunnable(
        const QUrl &url) : QObject(), QRunnable(), _url(url)
{

}

void SystemProxyRunnable::run()
{
    qDebug() << Q_FUNC_INFO << "Starting system proxy lookup";
    qRegisterMetaType<QNetworkProxy>("QNetworkProxy");
    QList<QNetworkProxy> proxies =
            QNetworkProxyFactory::systemProxyForQuery(QNetworkProxyQuery(_url));

    if (proxies.isEmpty()) {
        emit systemProxyLookedUp(QNetworkProxy(QNetworkProxy::NoProxy));
    } else {
        emit systemProxyLookedUp(proxies.first());
    }
}
