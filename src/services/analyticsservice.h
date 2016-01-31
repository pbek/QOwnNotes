#ifndef ANALYTICSSERVICE_H
#define ANALYTICSSERVICE_H

#include <QObject>
#include <libraries/qt-google-analytics-collector/ganalytics.hpp>

class AnalyticsService : public QObject
{
    Q_OBJECT

public:
    explicit AnalyticsService(QObject *parent = 0);
    void sendEvent(const QString &eventCategory = QString(), const QString &eventAction = QString(), const QString &eventLabel = QString(), int eventValue = 0);
    void sendAppView(const QString &screenName);

private:
    GAnalytics * analytics;

signals:

public slots:
};

#endif // ANALYTICSSERVICE_H
