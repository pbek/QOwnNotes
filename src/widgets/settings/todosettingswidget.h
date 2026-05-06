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

#include <QWidget>

#include "entities/cloudconnection.h"
#include "services/cloudservice.h"

struct CalDAVCalendarData;

namespace Ui {
class TodoSettingsWidget;
}

class TodoSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit TodoSettingsWidget(QWidget *parent = nullptr);
    ~TodoSettingsWidget();

    void initialize();
    void readSettings();
    void storeSettings();
    void refreshTodoCalendarList(const QList<CalDAVCalendarData> &items, const QString &serverUrl,
                                 bool forceReadCheckedState = false);
    void populateCloudConnectionComboBox(QList<CloudConnection> connections,
                                         int todoCalendarCloudConnectionId);

   signals:
    void storeSettingsRequested();
    void reloadCalendarListRequested();

   private slots:
    void on_reloadCalendarListButton_clicked();
    void on_defaultCloudCalendarRadioButton_toggled(bool checked);
    void on_legacyOwnCloudCalendarRadioButton_toggled(bool checked);
    void on_calDavCalendarRadioButton_toggled(bool checked);
    void on_calendarPlusRadioButton_toggled(bool checked);
    void on_emptyCalendarCachePushButton_clicked();
    void on_ignoreNonTodoCalendarsCheckBox_toggled(bool checked);
    void on_calendarCloudConnectionComboBox_currentIndexChanged(int index);
    void on_todoCalendarSupportCheckBox_toggled();

   private:
    Ui::TodoSettingsWidget *ui;
    void reloadCalendarList();
};
