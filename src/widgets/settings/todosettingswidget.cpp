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

#include "todosettingswidget.h"

#include <QDesktopServices>
#include <QRegularExpression>
#include <QUrl>

#include "entities/calendaritem.h"
#include "services/cloudservice.h"
#include "services/cryptoservice.h"
#include "services/settingsservice.h"
#include "ui_todosettingswidget.h"
#include "utils/gui.h"
#include "utils/misc.h"

TodoSettingsWidget::TodoSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::TodoSettingsWidget) {
    ui->setupUi(this);

    // Hide CalDAV settings by default
    ui->calDavCalendarGroupBox->hide();
}

TodoSettingsWidget::~TodoSettingsWidget() { delete ui; }

void TodoSettingsWidget::initialize() {
    // Currently no lazy initialization needed
}

void TodoSettingsWidget::readSettings() {
    SettingsService settings;

    ui->todoCalendarSupportCheckBox->setChecked(CloudService::isTodoCalendarSupportEnabled());
    on_todoCalendarSupportCheckBox_toggled();

    ui->closeTodoListAfterSaveCheckBox->setChecked(
        settings.value(QStringLiteral("closeTodoListAfterSave")).toBool());

    // Block signals on radio buttons while loading
    const QSignalBlocker blocker1(ui->defaultCloudCalendarRadioButton);
    Q_UNUSED(blocker1)
    const QSignalBlocker blocker2(ui->legacyOwnCloudCalendarRadioButton);
    Q_UNUSED(blocker2)
    const QSignalBlocker blocker3(ui->calendarPlusRadioButton);
    Q_UNUSED(blocker3)
    const QSignalBlocker blocker4(ui->calDavCalendarRadioButton);
    Q_UNUSED(blocker4)

    switch (settings
                .value(QStringLiteral("ownCloud/todoCalendarBackend"),
                       CloudService::DefaultCloudCalendar)
                .toInt()) {
        case CloudService::CalendarPlus:
            ui->calendarPlusRadioButton->setChecked(true);
            break;
        case CloudService::CalDAVCalendar:
            ui->calDavCalendarRadioButton->setChecked(true);
            ui->calDavCalendarGroupBox->setVisible(true);
            break;
        case CloudService::DefaultCloudCalendar:
            ui->defaultCloudCalendarRadioButton->setChecked(true);
            break;
        default:
            ui->legacyOwnCloudCalendarRadioButton->setChecked(true);
            break;
    }

    const QSignalBlocker blocker5(ui->ignoreNonTodoCalendarsCheckBox);
    Q_UNUSED(blocker5)

    ui->ignoreNonTodoCalendarsCheckBox->setChecked(
        settings.value(QStringLiteral("ownCloud/ignoreNonTodoCalendars"), true).toBool());

    ui->calDavServerUrlEdit->setText(
        settings.value(QStringLiteral("ownCloud/todoCalendarCalDAVServerUrl")).toString());
    ui->calDavUsernameEdit->setText(
        settings.value(QStringLiteral("ownCloud/todoCalendarCalDAVUsername")).toString());
    ui->calDavPasswordEdit->setText(CryptoService::instance()->decryptToString(
        settings.value(QStringLiteral("ownCloud/todoCalendarCalDAVPassword")).toString()));

    QStringList todoCalendarUrlList =
        settings.value(QStringLiteral("ownCloud/todoCalendarUrlList")).toStringList();
    QStringList todoCalendarDisplayNameList =
        settings.value(QStringLiteral("ownCloud/todoCalendarDisplayNameList")).toStringList();
    int todoCalendarUrlListCount = todoCalendarUrlList.count();
    int todoCalendarDisplayNameListCount = todoCalendarDisplayNameList.count();

    QList<CalDAVCalendarData> calendarDataList;
    for (int i = 0; i < todoCalendarUrlListCount; i++) {
        CalDAVCalendarData data;
        data.url = todoCalendarUrlList.at(i);

        if (todoCalendarUrlListCount == todoCalendarDisplayNameListCount) {
            data.displayName = todoCalendarDisplayNameList.at(i);
        }

        calendarDataList << data;
    }

    // We need a server URL for refreshTodoCalendarList but we don't have
    // access to the cloud page's serverUrlEdit here. Pass empty string
    // and let the dialog handle the forwarding with the correct URL.
    // This path uses forceReadCheckedState=true so it reads from settings.
    refreshTodoCalendarList(calendarDataList, QString(), true);

    // Reload the calendar list if it was empty
    if (todoCalendarUrlListCount == 0) {
        reloadCalendarList();
    }
}

void TodoSettingsWidget::storeSettings() {
    SettingsService settings;

    settings.setValue(QStringLiteral("todoCalendarSupport"),
                      ui->todoCalendarSupportCheckBox->isChecked());
    settings.setValue(QStringLiteral("closeTodoListAfterSave"),
                      ui->closeTodoListAfterSaveCheckBox->isChecked());

    QStringList todoCalendarUrlList;
    QStringList todoCalendarDisplayNameList;
    QStringList todoCalendarEnabledList;
    QStringList todoCalendarEnabledUrlList;
    for (int i = 0; i < ui->todoCalendarListWidget->count(); i++) {
        QListWidgetItem *item = ui->todoCalendarListWidget->item(i);

        todoCalendarUrlList.append(item->toolTip());
        todoCalendarDisplayNameList.append(item->text());

        if (item->checkState() == Qt::Checked) {
            todoCalendarEnabledList.append(item->text());
            todoCalendarEnabledUrlList.append(item->toolTip());
        }
    }

    // Store the tasks calendar data to the settings
    settings.setValue(QStringLiteral("ownCloud/todoCalendarUrlList"), todoCalendarUrlList);
    settings.setValue(QStringLiteral("ownCloud/todoCalendarDisplayNameList"),
                      todoCalendarDisplayNameList);
    settings.setValue(QStringLiteral("ownCloud/todoCalendarEnabledList"), todoCalendarEnabledList);
    settings.setValue(QStringLiteral("ownCloud/todoCalendarEnabledUrlList"),
                      todoCalendarEnabledUrlList);

    int todoCalendarBackend = CloudService::DefaultCloudCalendar;

    if (ui->calendarPlusRadioButton->isChecked()) {
        todoCalendarBackend = CloudService::CalendarPlus;
    } else if (ui->calDavCalendarRadioButton->isChecked()) {
        todoCalendarBackend = CloudService::CalDAVCalendar;
    } else if (ui->legacyOwnCloudCalendarRadioButton->isChecked()) {
        todoCalendarBackend = CloudService::LegacyOwnCloudCalendar;
    }

    settings.setValue(QStringLiteral("ownCloud/todoCalendarBackend"), todoCalendarBackend);
    settings.setValue(QStringLiteral("ownCloud/todoCalendarCloudConnectionId"),
                      ui->calendarCloudConnectionComboBox->currentData().toInt());
    settings.setValue(QStringLiteral("ownCloud/todoCalendarCalDAVServerUrl"),
                      ui->calDavServerUrlEdit->text());
    settings.setValue(QStringLiteral("ownCloud/todoCalendarCalDAVUsername"),
                      ui->calDavUsernameEdit->text());
    settings.setValue(QStringLiteral("ownCloud/todoCalendarCalDAVPassword"),
                      CryptoService::instance()->encryptToString(
                          ui->calDavPasswordEdit->text(),
                          QStringLiteral("settings/ownCloud/todoCalendarCalDAVPassword")));
}

/**
 * Refreshes the todo calendar list with items from the server
 */
void TodoSettingsWidget::refreshTodoCalendarList(const QList<CalDAVCalendarData> &items,
                                                 const QString &serverUrl,
                                                 bool forceReadCheckedState) {
    // We want to read the checked state from the settings if the
    // tasks calendar list was not empty
    bool readCheckedState = forceReadCheckedState ? true : ui->todoCalendarListWidget->count() > 0;

    // Clear the tasks calendar list
    ui->todoCalendarListWidget->clear();

    if (!CloudService::isTodoCalendarSupportEnabled()) {
        return;
    }

    SettingsService settings;
    QStringList todoCalendarEnabledList =
        settings.value(QStringLiteral("ownCloud/todoCalendarEnabledList")).toStringList();

    // Use CalDAV server URL if CalDAV is selected, otherwise use the provided server URL
    QUrl serverUrlObj(ui->calDavCalendarRadioButton->isChecked() ? ui->calDavServerUrlEdit->text()
                                                                 : serverUrl);

    // Return if server url isn't valid
    if (!serverUrlObj.isValid()) {
        return;
    }

    QString serverUrlText(serverUrlObj.toString());
    QString serverUrlPath = serverUrlObj.path();
    if (!serverUrlPath.isEmpty()) {
        // Remove the path from the end because we already got it in the url
        serverUrlText.replace(QRegularExpression(QRegularExpression::escape(serverUrlPath) + "$"),
                              QLatin1String(""));
    }

    QListIterator<CalDAVCalendarData> itr(items);
    while (itr.hasNext()) {
        CalDAVCalendarData data = itr.next();
        QString url = data.url;
        QString name = data.displayName;

        // Only add the server url if it wasn't already added
        if (!url.startsWith(serverUrlText)) {
            url = serverUrlText + url;
        }

        // Get the hash out of the url part
        QRegularExpression regex(QStringLiteral(R"(\/([^\/]*)\/$)"));
        QRegularExpressionMatch match = regex.match(url);
        QString hash = match.captured(1);

        // Remove percent encoding
        hash = QUrl::fromPercentEncoding(hash.toUtf8());

        // Skip the contact birthdays calendar
        if (hash == QLatin1String("contact_birthdays")) {
            continue;
        }

        // Skip the Calendar Plus birthday calendar
        if (hash.startsWith(QLatin1String("bdaycpltocal_"))) {
            continue;
        }

        if (name.isEmpty()) {
            name = hash;
        }

        // Create the list widget item and add it to the
        // tasks calendar list widget
        auto *item = new QListWidgetItem(name);

        // Eventually check if item was checked
        Qt::CheckState checkedState =
            readCheckedState
                ? (todoCalendarEnabledList.contains(name) ? Qt::Checked : Qt::Unchecked)
                : Qt::Checked;
        item->setCheckState(checkedState);

        item->setFlags(Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled |
                       Qt::ItemIsUserCheckable);
        item->setToolTip(url);
        ui->todoCalendarListWidget->addItem(item);
    }
}

/**
 * Populates the cloud connection combo box for the todo calendar
 */
void TodoSettingsWidget::populateCloudConnectionComboBox(QList<CloudConnection> connections,
                                                         int todoCalendarCloudConnectionId) {
    const QSignalBlocker blocker(ui->calendarCloudConnectionComboBox);
    Q_UNUSED(blocker)

    ui->calendarCloudConnectionComboBox->clear();

    for (auto &cloudConnection : connections) {
        ui->calendarCloudConnectionComboBox->addItem(cloudConnection.getName(),
                                                     cloudConnection.getId());
    }

    Utils::Gui::setComboBoxIndexByUserData(ui->calendarCloudConnectionComboBox,
                                           todoCalendarCloudConnectionId);
}

void TodoSettingsWidget::on_reloadCalendarListButton_clicked() {
    // We need to store the calendar backend via the dialog
    emit storeSettingsRequested();

    // Reload the calendar list
    reloadCalendarList();
}

/**
 * Reloads the calendar list by requesting the dialog to do it
 */
void TodoSettingsWidget::reloadCalendarList() {
    if (!CloudService::isTodoCalendarSupportEnabled()) {
        return;
    }

    emit reloadCalendarListRequested();
}

void TodoSettingsWidget::on_defaultCloudCalendarRadioButton_toggled(bool checked) {
    if (checked) {
        on_reloadCalendarListButton_clicked();
    }
}

void TodoSettingsWidget::on_legacyOwnCloudCalendarRadioButton_toggled(bool checked) {
    if (checked) {
        on_reloadCalendarListButton_clicked();
    }
}

void TodoSettingsWidget::on_calDavCalendarRadioButton_toggled(bool checked) {
    if (checked) {
        on_reloadCalendarListButton_clicked();
    }

    ui->calDavCalendarGroupBox->setVisible(checked);
    ui->calendarCloudConnectionGroupBox->setHidden(checked);
}

void TodoSettingsWidget::on_calendarPlusRadioButton_toggled(bool checked) {
    if (checked) {
        on_reloadCalendarListButton_clicked();
    }
}

/**
 * Removes all calendar items
 */
void TodoSettingsWidget::on_emptyCalendarCachePushButton_clicked() {
    CalendarItem::removeAll();

    Utils::Gui::information(this, tr("Calendar cache emptied"),
                            tr("Your calendar cache was emptied."),
                            QStringLiteral("calendar-cache-emptied"));
}

void TodoSettingsWidget::on_ignoreNonTodoCalendarsCheckBox_toggled(bool checked) {
    SettingsService settings;
    settings.setValue(QStringLiteral("ownCloud/ignoreNonTodoCalendars"), checked);
}

void TodoSettingsWidget::on_calendarCloudConnectionComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index)
    SettingsService settings;
    settings.setValue(QStringLiteral("ownCloud/todoCalendarCloudConnectionId"),
                      ui->calendarCloudConnectionComboBox->currentData().toInt());
    on_reloadCalendarListButton_clicked();
}

void TodoSettingsWidget::on_todoCalendarSupportCheckBox_toggled() {
    bool checked = ui->todoCalendarSupportCheckBox->isChecked();
    ui->calendarBackendGroupBox->setEnabled(checked);
    ui->calDavCalendarGroupBox->setEnabled(checked);
    ui->calendarCloudConnectionGroupBox->setEnabled(checked);
    ui->todoCalendarGroupBox->setEnabled(checked);
    ui->todoListSettingsGroupBox->setEnabled(checked);
}
