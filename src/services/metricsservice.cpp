#include "metricsservice.h"

MetricsService::MetricsService(QObject *parent) : QObject(parent)
{
    analytics = new GAnalytics(qApp, "UA-52660882-3");
    analytics->generateUserAgentEtc();
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

void MetricsService::sendEvent(
        const QString& eventCategory,
        const QString& eventAction,
        const QString& eventLabel,
        int eventValue) {
    QSettings settings;
    if (!settings.value("appMetrics/disableTracking").toBool()) {
//        qDebug() << __func__ << " - 'eventAction': " << eventAction;
        analytics->sendEvent(
                eventCategory, eventAction, eventLabel, eventValue);
    }
}

void MetricsService::sendAppView(const QString& screenName) {
    QSettings settings;
    if (!settings.value("appMetrics/disableTracking").toBool()) {
//        qDebug() << __func__ << " - 'screenName': " << screenName;
        analytics->sendAppview2(screenName);
    }
}
