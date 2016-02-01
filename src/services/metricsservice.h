#pragma once

#include <QObject>
#include <libraries/qt-google-analytics-collector/ganalytics.hpp>

class MetricsService : public QObject
{
    Q_OBJECT

public:
    explicit MetricsService(QObject *parent = 0);
    void sendEvent(const QString &eventCategory = QString(), const QString &eventAction = QString(), const QString &eventLabel = QString(), int eventValue = 0);
    void sendAppView(const QString &screenName);

private:
    GAnalytics * analytics;

signals:

public slots:
};
