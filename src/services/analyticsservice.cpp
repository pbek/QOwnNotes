#include "analyticsservice.h"

AnalyticsService::AnalyticsService(QObject *parent) : QObject(parent)
{
    analytics = new GAnalytics(qApp, "UA-52660882-3" );
    analytics->generateUserAgentEtc();
}

void AnalyticsService::sendEvent(
        const QString& eventCategory,
        const QString& eventAction,
        const QString& eventLabel,
        int eventValue) {
    qDebug() << __func__ << " - 'eventAction': " << eventAction;

    analytics->sendEvent(eventCategory, eventAction, eventLabel, eventValue);
}

void AnalyticsService::sendAppView(const QString& screenName) {
    qDebug() << __func__ << " - 'screenName': " << screenName;

    analytics->sendAppview2(screenName);
}
