#include "metricsservice.h"
#include "version.h"
#include "release.h"
#include <QSettings>

MetricsService::MetricsService(QObject *parent) : QObject(parent)
{
    QString debug = "0";
#ifdef QT_DEBUG
    debug = "1";
#endif

    _piwikTracker = new PiwikTracker(qApp, QUrl("https://p.qownnotes.org"), 5);
    _piwikTracker->setCustomDimension(1, QString(VERSION));
    _piwikTracker->setCustomDimension(2, QLocale::system().name());
    _piwikTracker->setCustomDimension(3, debug);
    _piwikTracker->setCustomDimension(9, QString(RELEASE));

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
    MetricsService *metricsService = new MetricsService(parent);

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
        QString path,
        QString eventCategory,
        QString eventAction,
        QString eventName,
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
        _piwikTracker->sendVisit("app/heartbeat", "Heartbeat");
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
