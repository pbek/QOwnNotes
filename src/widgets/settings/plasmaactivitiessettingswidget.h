/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

#pragma once

#include <QWidget>

class QLabel;
class QComboBox;
class QFrame;
class QListWidget;

#ifdef Q_OS_LINUX
class PlasmaActivityService;
#endif

class PlasmaActivitiesSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit PlasmaActivitiesSettingsWidget(QWidget *parent = nullptr);

    void initialize();

   private:
    QLabel *_statusLabel;
    QListWidget *_activityListWidget;
    QFrame *_activityEditFrame;
    QComboBox *_noteFolderComboBox;
    QComboBox *_layoutComboBox;
    bool _initialized = false;
    bool _loading = false;

#ifdef Q_OS_LINUX
    PlasmaActivityService *_activityService;

    void setNoteFolderForActivity(const QString &activityId, int noteFolderId);
    void setLayoutForActivity(const QString &activityId, const QString &layoutUuid);
#endif

    void reloadActivities();
    void updateSelectedActivity();
    QString selectedActivityId() const;
};
