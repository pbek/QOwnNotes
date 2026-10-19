/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "plasmaactivityservice.h"

#ifdef Q_OS_LINUX

#include <entities/notefolder.h>

#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QDBusServiceWatcher>
#include <QDebug>

#include "settingsservice.h"

#ifdef QON_KDE_INTEGRATION
#include <QTimer>

#include "plasmaactivitieslibrary.h"
#endif

namespace {
const QString activityManagerService = QStringLiteral("org.kde.ActivityManager");
const QString activitiesPath = QStringLiteral("/ActivityManager/Activities");
const QString activitiesInterface = QStringLiteral("org.kde.ActivityManager.Activities");

// Timeout for the blocking D-Bus calls, so a hanging service can't freeze the UI for long
constexpr int blockingCallTimeout = 1000;

QString noteFolderKey(int noteFolderId, const QString &key) {
    return QStringLiteral("NoteFolder-%1/%2").arg(QString::number(noteFolderId), key);
}

QString layoutKey(const QString &layoutUuid, const QString &key) {
    return QStringLiteral("layout-%1/%2").arg(layoutUuid, key);
}

QDBusMessage blockingCall(const QString &method, const QVariantList &arguments = {}) {
    QDBusMessage message = QDBusMessage::createMethodCall(activityManagerService, activitiesPath,
                                                          activitiesInterface, method);
    message.setArguments(arguments);

    return QDBusConnection::sessionBus().call(message, QDBus::Block, blockingCallTimeout);
}
}    // namespace

PlasmaActivityService::PlasmaActivityService(QObject *parent) : QObject(parent) {
    QDBusConnection bus = QDBusConnection::sessionBus();

    // Connecting by service name also works if kactivitymanagerd is started later
    bus.connect(activityManagerService, activitiesPath, activitiesInterface,
                QStringLiteral("CurrentActivityChanged"), this,
                SLOT(onCurrentActivityChanged(QString)));
    bus.connect(activityManagerService, activitiesPath, activitiesInterface,
                QStringLiteral("ActivityAdded"), this, SLOT(onActivityAdded(QString)));
    bus.connect(activityManagerService, activitiesPath, activitiesInterface,
                QStringLiteral("ActivityRemoved"), this, SLOT(onActivityRemoved(QString)));
    bus.connect(activityManagerService, activitiesPath, activitiesInterface,
                QStringLiteral("ActivityNameChanged"), this,
                SLOT(onActivityNameChanged(QString, QString)));

    auto *serviceWatcher = new QDBusServiceWatcher(activityManagerService, bus,
                                                   QDBusServiceWatcher::WatchForRegistration, this);
    connect(serviceWatcher, &QDBusServiceWatcher::serviceRegistered, this, [this](const QString &) {
        requestCurrentActivity();
        emit activitiesChanged();
    });

#ifdef QON_KDE_INTEGRATION
    // Use the cached activities of the PlasmaActivities library once it is ready
    PlasmaActivitiesLibrary *library = PlasmaActivitiesLibrary::instance();
    connect(library, &PlasmaActivitiesLibrary::currentActivityChanged, this,
            [this](const QString &activityId) {
                ++_activityChangeSerial;
                emit currentActivityChanged(activityId);
            });
    connect(library, &PlasmaActivitiesLibrary::activitiesChanged, this,
            &PlasmaActivityService::activitiesChanged);
    library->start();
#endif
}

/**
 * Returns true if the activities are provided by the PlasmaActivities library, in which
 * case the direct D-Bus signals are ignored to avoid duplicate notifications
 */
bool PlasmaActivityService::usesLibrary() {
#ifdef QON_KDE_INTEGRATION
    return PlasmaActivitiesLibrary::instance()->isReady();
#else
    return false;
#endif
}

bool PlasmaActivityService::isAvailable() {
    QDBusConnection bus = QDBusConnection::sessionBus();

    if (!bus.isConnected() || bus.interface() == nullptr) {
        return false;
    }

    return bus.interface()->isServiceRegistered(activityManagerService).value();
}

QString PlasmaActivityService::currentActivityId() {
    if (!isAvailable()) {
        return {};
    }

#ifdef QON_KDE_INTEGRATION
    if (usesLibrary()) {
        return PlasmaActivitiesLibrary::instance()->currentActivity();
    }
#endif

    const QDBusReply<QString> reply = blockingCall(QStringLiteral("CurrentActivity"));
    return reply.isValid() ? reply.value() : QString();
}

QList<PlasmaActivityService::Activity> PlasmaActivityService::activities() {
    QList<Activity> result;

    if (!isAvailable()) {
        return result;
    }

#ifdef QON_KDE_INTEGRATION
    if (usesLibrary()) {
        const auto libraryActivities = PlasmaActivitiesLibrary::instance()->activities();
        for (const auto &libraryActivity : libraryActivities) {
            Activity activity;
            activity.id = libraryActivity.first;
            activity.name = libraryActivity.second;
            result.append(activity);
        }
        return result;
    }
#endif

    // We don't use ListActivitiesWithInformation, because its reply signature
    // differs between Plasma versions
    const QDBusReply<QStringList> listReply = blockingCall(QStringLiteral("ListActivities"));

    if (!listReply.isValid()) {
        qWarning() << "Could not list KDE Plasma activities:" << listReply.error().message();
        return result;
    }

    const QStringList activityIds = listReply.value();

    for (const QString &activityId : activityIds) {
        const QDBusReply<QString> nameReply =
            blockingCall(QStringLiteral("ActivityName"), {activityId});

        Activity activity;
        activity.id = activityId;
        activity.name = nameReply.isValid() ? nameReply.value() : QString();
        result.append(activity);
    }

    return result;
}

void PlasmaActivityService::requestCurrentActivity() {
    if (!isAvailable()) {
        return;
    }

    const quint64 requestSerial = _activityChangeSerial;

#ifdef QON_KDE_INTEGRATION
    if (usesLibrary()) {
        // Emit asynchronously like the D-Bus request, unless the activity changed meanwhile
        QTimer::singleShot(0, this, [this, requestSerial]() {
            const QString activityId = PlasmaActivitiesLibrary::instance()->currentActivity();
            if (requestSerial == _activityChangeSerial && !activityId.isEmpty()) {
                emit currentActivityChanged(activityId);
            }
        });
        return;
    }
#endif

    const QDBusMessage message =
        QDBusMessage::createMethodCall(activityManagerService, activitiesPath, activitiesInterface,
                                       QStringLiteral("CurrentActivity"));
    auto *watcher =
        new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(message), this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [this, requestSerial](QDBusPendingCallWatcher *w) {
                const QDBusPendingReply<QString> reply = *w;

                if (reply.isError()) {
                    qWarning() << "Could not get current KDE Plasma activity:"
                               << reply.error().message();
                } else if (requestSerial == _activityChangeSerial && !reply.value().isEmpty()) {
                    emit currentActivityChanged(reply.value());
                }

                w->deleteLater();
            });
}

bool PlasmaActivityService::isNoteFolderActivityEnabled(int noteFolderId) {
    const SettingsService settings;
    return settings.value(noteFolderKey(noteFolderId, QStringLiteral("plasmaActivityEnabled")))
        .toBool();
}

QString PlasmaActivityService::noteFolderActivityId(int noteFolderId) {
    const SettingsService settings;
    return settings.value(noteFolderKey(noteFolderId, QStringLiteral("plasmaActivityId")))
        .toString();
}

/**
 * Returns the id of the note folder linked to the activity or 0 if there is none
 */
int PlasmaActivityService::noteFolderIdForActivity(const QString &activityId) {
    if (activityId.isEmpty()) {
        return 0;
    }

    const QList<NoteFolder> noteFolders = NoteFolder::fetchAll();

    for (const NoteFolder &noteFolder : noteFolders) {
        const int id = noteFolder.getId();

        if (isNoteFolderActivityEnabled(id) && noteFolderActivityId(id) == activityId) {
            return id;
        }
    }

    return 0;
}

/**
 * Stores the activity link of a note folder
 *
 * Only one note folder can be linked to an activity, so the link is removed
 * from other note folders that were linked to the same activity.
 *
 * @return the ids of the note folders that lost their link
 */
QList<int> PlasmaActivityService::setNoteFolderActivity(int noteFolderId, bool enabled,
                                                        const QString &activityId) {
    QList<int> clearedIds;
    SettingsService settings;

    if (enabled && !activityId.isEmpty()) {
        const QList<NoteFolder> noteFolders = NoteFolder::fetchAll();

        for (const NoteFolder &noteFolder : noteFolders) {
            const int id = noteFolder.getId();

            if (id != noteFolderId && isNoteFolderActivityEnabled(id) &&
                noteFolderActivityId(id) == activityId) {
                settings.setValue(noteFolderKey(id, QStringLiteral("plasmaActivityEnabled")),
                                  false);
                clearedIds.append(id);
            }
        }
    }

    settings.setValue(noteFolderKey(noteFolderId, QStringLiteral("plasmaActivityEnabled")),
                      enabled);
    settings.setValue(noteFolderKey(noteFolderId, QStringLiteral("plasmaActivityId")), activityId);

    return clearedIds;
}

bool PlasmaActivityService::isLayoutActivityEnabled(const QString &layoutUuid) {
    const SettingsService settings;
    return settings.value(layoutKey(layoutUuid, QStringLiteral("plasmaActivityEnabled"))).toBool();
}

QString PlasmaActivityService::layoutActivityId(const QString &layoutUuid) {
    const SettingsService settings;
    return settings.value(layoutKey(layoutUuid, QStringLiteral("plasmaActivityId"))).toString();
}

/**
 * Returns the uuid of the layout linked to the activity or an empty string
 * if there is none
 */
QString PlasmaActivityService::layoutUuidForActivity(const QString &activityId) {
    if (activityId.isEmpty()) {
        return {};
    }

    const SettingsService settings;
    const QStringList layoutUuids = settings.value(QStringLiteral("layouts")).toStringList();

    for (const QString &uuid : layoutUuids) {
        if (isLayoutActivityEnabled(uuid) && layoutActivityId(uuid) == activityId) {
            return uuid;
        }
    }

    return {};
}

/**
 * Stores the activity link of a layout
 *
 * Only one layout can be linked to an activity, so the link is removed
 * from other layouts that were linked to the same activity.
 *
 * @return the uuids of the layouts that lost their link
 */
QStringList PlasmaActivityService::setLayoutActivity(const QString &layoutUuid, bool enabled,
                                                     const QString &activityId) {
    QStringList clearedUuids;
    SettingsService settings;

    if (enabled && !activityId.isEmpty()) {
        const QStringList layoutUuids = settings.value(QStringLiteral("layouts")).toStringList();

        for (const QString &uuid : layoutUuids) {
            if (uuid != layoutUuid && isLayoutActivityEnabled(uuid) &&
                layoutActivityId(uuid) == activityId) {
                settings.setValue(layoutKey(uuid, QStringLiteral("plasmaActivityEnabled")), false);
                clearedUuids.append(uuid);
            }
        }
    }

    settings.setValue(layoutKey(layoutUuid, QStringLiteral("plasmaActivityEnabled")), enabled);
    settings.setValue(layoutKey(layoutUuid, QStringLiteral("plasmaActivityId")), activityId);

    return clearedUuids;
}

void PlasmaActivityService::onCurrentActivityChanged(const QString &activityId) {
    if (usesLibrary()) {
        return;
    }

    ++_activityChangeSerial;
    qDebug() << "KDE Plasma activity changed:" << activityId;
    emit currentActivityChanged(activityId);
}

void PlasmaActivityService::onActivityAdded(const QString &activityId) {
    Q_UNUSED(activityId)
    if (!usesLibrary()) {
        emit activitiesChanged();
    }
}

void PlasmaActivityService::onActivityRemoved(const QString &activityId) {
    Q_UNUSED(activityId)
    if (!usesLibrary()) {
        emit activitiesChanged();
    }
}

void PlasmaActivityService::onActivityNameChanged(const QString &activityId, const QString &name) {
    Q_UNUSED(activityId)
    Q_UNUSED(name)
    if (!usesLibrary()) {
        emit activitiesChanged();
    }
}

#endif    // Q_OS_LINUX
