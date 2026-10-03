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

#pragma once

#include <QtGlobal>

#ifdef Q_OS_LINUX

#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

/**
 * Talks to the KDE Plasma activity manager (kactivitymanagerd) over the
 * session D-Bus and stores which note folder and layout should be activated
 * when a certain Plasma activity becomes the current activity.
 *
 * Only one note folder and one layout can be linked to an activity.
 */
class PlasmaActivityService : public QObject {
    Q_OBJECT

   public:
    struct Activity {
        QString id;
        QString name;
    };

    explicit PlasmaActivityService(QObject *parent = nullptr);

    /**
     * Returns true if the KDE Plasma activity manager is reachable on the
     * session bus
     */
    static bool isAvailable();

    /**
     * Returns the id of the current activity (blocking D-Bus call)
     */
    static QString currentActivityId();

    /**
     * Returns all activities with their names (blocking D-Bus calls)
     */
    static QList<Activity> activities();

    /**
     * Asynchronously requests the current activity, emits
     * currentActivityChanged() when the reply arrives
     */
    void requestCurrentActivity();

    // Note folder <-> activity mapping
    static bool isNoteFolderActivityEnabled(int noteFolderId);
    static QString noteFolderActivityId(int noteFolderId);
    static int noteFolderIdForActivity(const QString &activityId);
    static QList<int> setNoteFolderActivity(int noteFolderId, bool enabled,
                                            const QString &activityId);

    // Layout <-> activity mapping
    static bool isLayoutActivityEnabled(const QString &layoutUuid);
    static QString layoutActivityId(const QString &layoutUuid);
    static QString layoutUuidForActivity(const QString &activityId);
    static QStringList setLayoutActivity(const QString &layoutUuid, bool enabled,
                                         const QString &activityId);

   signals:
    void currentActivityChanged(const QString &activityId);
    void activitiesChanged();

   private slots:
    void onCurrentActivityChanged(const QString &activityId);
    void onActivityAdded(const QString &activityId);
    void onActivityRemoved(const QString &activityId);
    void onActivityNameChanged(const QString &activityId, const QString &name);

   private:
    quint64 _activityChangeSerial = 0;
};

#endif    // Q_OS_LINUX
