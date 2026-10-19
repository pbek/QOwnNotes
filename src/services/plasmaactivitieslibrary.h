#pragma once

#include <QList>
#include <QObject>
#include <QPair>
#include <QString>

namespace KActivities {
class Consumer;
}

/**
 * Provides the KDE Plasma activities from the PlasmaActivities library (KDE builds only)
 *
 * The library caches activities asynchronously, so no blocking D-Bus calls are needed.
 * It is only used after the activity manager reported a supported version, because the
 * library aborts the application on older services, and it never starts the activity
 * manager itself. Until the library is ready, PlasmaActivityService uses D-Bus directly.
 */
class PlasmaActivitiesLibrary : public QObject {
    Q_OBJECT

   public:
    enum class State {
        // The activity manager wasn't checked yet, e.g. because it isn't running
        Unchecked,
        // The version of the activity manager is being requested
        Checking,
        // The library is used
        Active,
        // The activity manager is too old for the library
        Unsupported,
    };

    static PlasmaActivitiesLibrary *instance();

    explicit PlasmaActivitiesLibrary(QObject *parent = nullptr);

    static bool isSupportedVersion(const QString &version);

    void start();
    State state() const;
    bool isReady() const;
    QString currentActivity() const;
    // Pairs of activity id and name
    QList<QPair<QString, QString>> activities() const;

   signals:
    void currentActivityChanged(const QString &activityId);
    void activitiesChanged();
    void stateChanged();

   private slots:
    void onActivityNameChanged(const QString &activityId, const QString &name);

   private:
    void checkVersion();
    void createConsumer();
    void setState(State state);

    KActivities::Consumer *_consumer = nullptr;
    State _state = State::Unchecked;
};
