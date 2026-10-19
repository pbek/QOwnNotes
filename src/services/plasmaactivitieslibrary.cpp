#include "plasmaactivitieslibrary.h"

#include <PlasmaActivities/Consumer>
#include <PlasmaActivities/Info>
#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusServiceWatcher>
#include <QDebug>
#include <QTimer>
#include <algorithm>
#include <iterator>

namespace {
const QString activityManagerService = QStringLiteral("org.kde.ActivityManager");

// The library reports this id while it doesn't know the real activities
const QString nullActivityId = QStringLiteral("00000000-0000-0000-0000-000000000000");

// Timeout for the version request, so a hanging service doesn't keep us waiting
constexpr int versionCallTimeout = 2000;

bool isRealActivity(const QString &activityId) {
    return !activityId.isEmpty() && activityId != nullActivityId;
}
}    // namespace

PlasmaActivitiesLibrary *PlasmaActivitiesLibrary::instance() {
    static auto *library = new PlasmaActivitiesLibrary(QCoreApplication::instance());
    return library;
}

PlasmaActivitiesLibrary::PlasmaActivitiesLibrary(QObject *parent) : QObject(parent) {
    QDBusConnection bus = QDBusConnection::sessionBus();
    auto *watcher = new QDBusServiceWatcher(activityManagerService, bus,
                                            QDBusServiceWatcher::WatchForRegistration, this);
    connect(watcher, &QDBusServiceWatcher::serviceRegistered, this, [this](const QString &) {
        // An updated activity manager may be supported now. Once the library is used, it
        // follows service restarts itself.
        if (_state == State::Unsupported) {
            setState(State::Unchecked);
        }
        start();
    });

    // The library doesn't forward name changes, so refresh the activity list for them
    bus.connect(activityManagerService, QStringLiteral("/ActivityManager/Activities"),
                QStringLiteral("org.kde.ActivityManager.Activities"),
                QStringLiteral("ActivityNameChanged"), this,
                SLOT(onActivityNameChanged(QString, QString)));
}

/**
 * Returns true if the PlasmaActivities library accepts the activity manager version
 *
 * This mirrors the library's own check, which aborts the application if the service is
 * older than 6.2.0 (even "6.2" counts as older).
 */
bool PlasmaActivitiesLibrary::isSupportedVersion(const QString &version) {
    const QStringList parts = version.split(QLatin1Char('.'));
    QList<int> numbers;
    std::transform(parts.cbegin(), parts.cend(), std::back_inserter(numbers),
                   [](const QString &part) { return part.toInt(); });

    const QList<int> required{6, 2, 0};
    return !std::lexicographical_compare(numbers.cbegin(), numbers.cend(), required.cbegin(),
                                         required.cend());
}

/**
 * Starts using the library if the activity manager is running and supported
 *
 * This doesn't start the activity manager and doesn't block.
 */
void PlasmaActivitiesLibrary::start() {
    if (_state != State::Unchecked) {
        return;
    }

    QDBusConnectionInterface *bus = QDBusConnection::sessionBus().interface();
    if (bus == nullptr || !bus->isServiceRegistered(activityManagerService).value()) {
        return;
    }

    checkVersion();
}

void PlasmaActivitiesLibrary::checkVersion() {
    setState(State::Checking);

    const QDBusMessage message = QDBusMessage::createMethodCall(
        activityManagerService, QStringLiteral("/ActivityManager"),
        QStringLiteral("org.kde.ActivityManager.Application"), QStringLiteral("serviceVersion"));
    auto *watcher = new QDBusPendingCallWatcher(
        QDBusConnection::sessionBus().asyncCall(message, versionCallTimeout), this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this](QDBusPendingCallWatcher *w) {
        w->deleteLater();
        const QDBusPendingReply<QString> reply = *w;

        if (reply.isError()) {
            // Retry when the service is registered again
            qWarning() << "Could not get the KDE Plasma activity manager version:"
                       << reply.error().message();
            setState(State::Unchecked);
        } else if (!isSupportedVersion(reply.value())) {
            qWarning() << "KDE Plasma activity manager" << reply.value()
                       << "is too old for the PlasmaActivities library, using D-Bus";
            setState(State::Unsupported);
        } else {
            createConsumer();
        }
    });
}

void PlasmaActivitiesLibrary::createConsumer() {
    // The library would otherwise try to start the activity manager on other desktops,
    // e.g. after it was stopped
    QCoreApplication::instance()->setProperty("org.kde.KActivities.core.disableAutostart", true);

    _consumer = new KActivities::Consumer(this);

    connect(_consumer, &KActivities::Consumer::currentActivityChanged, this,
            [this](const QString &activityId) {
                if (isRealActivity(activityId)) {
                    emit currentActivityChanged(activityId);
                }
            });
    connect(_consumer, &KActivities::Consumer::activitiesChanged, this,
            [this](const QStringList &) { emit activitiesChanged(); });
    connect(_consumer, &KActivities::Consumer::serviceStatusChanged, this,
            [this](KActivities::Consumer::ServiceStatus status) {
                emit stateChanged();
                emit activitiesChanged();

                if (status == KActivities::Consumer::Running &&
                    isRealActivity(_consumer->currentActivity())) {
                    emit currentActivityChanged(_consumer->currentActivity());
                }
            });

    setState(State::Active);
}

void PlasmaActivitiesLibrary::setState(State state) {
    if (_state == state) {
        return;
    }

    _state = state;
    emit stateChanged();
}

PlasmaActivitiesLibrary::State PlasmaActivitiesLibrary::state() const { return _state; }

/**
 * Returns true if the library has loaded the activities, so its data can be used
 */
bool PlasmaActivitiesLibrary::isReady() const {
    return _state == State::Active && _consumer != nullptr &&
           _consumer->serviceStatus() == KActivities::Consumer::Running;
}

QString PlasmaActivitiesLibrary::currentActivity() const {
    if (!isReady()) {
        return {};
    }

    const QString activityId = _consumer->currentActivity();
    return isRealActivity(activityId) ? activityId : QString();
}

QList<QPair<QString, QString>> PlasmaActivitiesLibrary::activities() const {
    QList<QPair<QString, QString>> result;
    if (!isReady()) {
        return result;
    }

    const QStringList activityIds = _consumer->activities();
    for (const QString &activityId : activityIds) {
        if (isRealActivity(activityId)) {
            // Info reads the name from the library's cache without a D-Bus call
            result.append(qMakePair(activityId, KActivities::Info(activityId).name()));
        }
    }

    return result;
}

void PlasmaActivitiesLibrary::onActivityNameChanged(const QString &activityId,
                                                    const QString &name) {
    Q_UNUSED(activityId)
    Q_UNUSED(name)

    if (_state == State::Active) {
        // Let the library update its cache from the same D-Bus signal first
        QTimer::singleShot(0, this, [this]() { emit activitiesChanged(); });
    }
}
