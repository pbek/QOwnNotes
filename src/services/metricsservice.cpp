#include <utility>
#include "metricsservice.h"
#include "version.h"
#include "release.h"
#include <QSettings>

MetricsService::MetricsService(QObject *parent) : QObject(parent)
{
    _firstHeartbeat = true;

    QString debug = "0";
    int siteId = 5;
#ifdef QT_DEBUG
    debug = "1";
    siteId = 6;
#endif

    _piwikTracker = new PiwikTracker(
            qApp, QUrl("https://p.qownnotes.org"), siteId);
    _piwikTracker->setCustomDimension(1, QString(VERSION));
    _piwikTracker->setCustomDimension(2, QLocale::system().name());
    _piwikTracker->setCustomDimension(3, debug);
    _piwikTracker->setCustomDimension(9, qApp->property("release").toString());

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    _piwikTracker->setCustomDimension(
            7, QSysInfo::prettyProductName() + " (" +
            QSysInfo::currentCpuArchitecture() + ")");
#endif
}

/**
 * Fetches a global instance of the class
 */
MetricsService * MetricsService::instance() {
    return qApp->property("metricsService").value<MetricsService *>();
}

/**
 * Creates a global instance of the class
 */
MetricsService * MetricsService::createInstance(QObject *parent) {
    auto *metricsService = new MetricsService(parent);

    qApp->setProperty(
            "metricsService",
            QVariant::fromValue<MetricsService *>(metricsService));

    return metricsService;
}

void MetricsService::sendVisit(
        const QString &path,
        const QString &actionName) {
    _piwikTracker->sendVisit(path, actionName);
}

void MetricsService::sendVisitIfEnabled(
        const QString &path,
        const QString &actionName) {
    QSettings settings;
    if (!settings.value("appMetrics/disableTracking").toBool()) {
        sendVisit(path, actionName);
    }
}

void MetricsService::sendEventIfEnabled(
        const QString& path,
        const QString& eventCategory,
        const QString& eventAction,
        const QString& eventName,
        int eventValue) {
    QSettings settings;
    if (!settings.value("appMetrics/disableTracking").toBool()) {
        _piwikTracker->sendEvent(
                path,
                eventCategory,
                eventAction,
                eventName,
                eventValue);
    }
}

/**
 * Sends a heartbeat if not disabled
 */
void MetricsService::sendHeartbeat() {
    QSettings settings;
    if (!settings.value("appMetrics/disableAppHeartbeat").toBool()) {
        // send a normal event the first time
        if (_firstHeartbeat) {
            _piwikTracker->sendVisit("app/heartbeat");
            _firstHeartbeat = false;
        } else {
            _piwikTracker->sendPing();
        }
    }
}

/**
 * Sends locale information
 */
void MetricsService::sendLocaleEvent() {

    QSettings settings;
    QString eventText = QLocale::system().name();
    QString settingsLocale = settings.value("interfaceLanguage").toString();
    if (!settingsLocale.isEmpty()) {
        eventText += " (" + settingsLocale + ")";
    }

    sendEventIfEnabled("app/locale", "app", "locale", eventText);
}
