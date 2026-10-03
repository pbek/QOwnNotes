/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

#include "plasmaactivitiessettingswidget.h"

#include <QComboBox>
#include <QFont>
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QIcon>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>

#ifdef Q_OS_LINUX
#include <entities/notefolder.h>
#include <services/plasmaactivityservice.h>
#include <services/settingsservice.h>
#endif

PlasmaActivitiesSettingsWidget::PlasmaActivitiesSettingsWidget(QWidget *parent) : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);

    auto *informationLabel = new QLabel(
        tr("QOwnNotes must be shown on all KDE Plasma activities. Otherwise, activating its "
           "window from another activity can make Plasma switch back and restore the previous "
           "note folder and layout. Open the QOwnNotes window menu with Alt+F3 and select "
           "\"Move to Activity > All Activities\"."),
        this);
    informationLabel->setObjectName(QStringLiteral("plasmaActivitiesInformationLabel"));
    informationLabel->setWordWrap(true);
    layout->addWidget(informationLabel);

    _statusLabel = new QLabel(this);
    _statusLabel->setObjectName(QStringLiteral("plasmaActivitiesStatusLabel"));
    _statusLabel->setWordWrap(true);
    layout->addWidget(_statusLabel);

    auto *activitiesGroupBox = new QGroupBox(tr("KDE Plasma activities"), this);
    auto *activitiesLayout = new QGridLayout(activitiesGroupBox);

    auto *selectionFrame = new QFrame(activitiesGroupBox);
    selectionFrame->setFrameShape(QFrame::NoFrame);
    auto *selectionLayout = new QVBoxLayout(selectionFrame);
    selectionLayout->setContentsMargins(0, 0, 0, 0);

    _activityListWidget = new QListWidget(selectionFrame);
    _activityListWidget->setObjectName(QStringLiteral("plasmaActivityListWidget"));
    selectionLayout->addWidget(_activityListWidget);

    auto *refreshButton = new QPushButton(tr("Refresh activities"), selectionFrame);
    refreshButton->setObjectName(QStringLiteral("refreshPlasmaActivitiesButton"));
    refreshButton->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));
    selectionLayout->addWidget(refreshButton, 0, Qt::AlignRight);
    activitiesLayout->addWidget(selectionFrame, 0, 0);

    _activityEditFrame = new QFrame(activitiesGroupBox);
    _activityEditFrame->setFrameShape(QFrame::NoFrame);
    auto *editLayout = new QGridLayout(_activityEditFrame);
    editLayout->setContentsMargins(0, 0, 0, 0);

    editLayout->addWidget(new QLabel(tr("Note folder:"), _activityEditFrame), 0, 0);
    _noteFolderComboBox = new QComboBox(_activityEditFrame);
    _noteFolderComboBox->setObjectName(QStringLiteral("plasmaActivityNoteFolderComboBox"));
    editLayout->addWidget(_noteFolderComboBox, 1, 0);

    editLayout->addWidget(new QLabel(tr("Layout:"), _activityEditFrame), 2, 0);
    _layoutComboBox = new QComboBox(_activityEditFrame);
    _layoutComboBox->setObjectName(QStringLiteral("plasmaActivityLayoutComboBox"));
    editLayout->addWidget(_layoutComboBox, 3, 0);
    editLayout->setRowStretch(4, 1);
    activitiesLayout->addWidget(_activityEditFrame, 0, 1);
    activitiesLayout->setColumnStretch(0, 2);
    activitiesLayout->setColumnStretch(1, 3);
    layout->addWidget(activitiesGroupBox, 1);

    connect(refreshButton, &QPushButton::clicked, this,
            &PlasmaActivitiesSettingsWidget::reloadActivities);
    connect(_activityListWidget, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem *, QListWidgetItem *) { updateSelectedActivity(); });
#ifdef Q_OS_LINUX
    connect(_noteFolderComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            [this](int) {
                if (!_loading) {
                    setNoteFolderForActivity(selectedActivityId(),
                                             _noteFolderComboBox->currentData().toInt());
                }
            });
    connect(_layoutComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, [this](int) {
                if (!_loading) {
                    setLayoutForActivity(selectedActivityId(),
                                         _layoutComboBox->currentData().toString());
                }
            });

    _activityService = new PlasmaActivityService(this);
    connect(_activityService, &PlasmaActivityService::activitiesChanged, this, [this]() {
        if (_initialized) {
            reloadActivities();
        }
    });
    connect(_activityService, &PlasmaActivityService::currentActivityChanged, this, [this]() {
        if (_initialized) {
            reloadActivities();
        }
    });
#endif
}

void PlasmaActivitiesSettingsWidget::initialize() {
    _initialized = true;
    reloadActivities();
}

QString PlasmaActivitiesSettingsWidget::selectedActivityId() const {
    const QListWidgetItem *item = _activityListWidget->currentItem();
    return item == nullptr ? QString() : item->data(Qt::UserRole).toString();
}

void PlasmaActivitiesSettingsWidget::reloadActivities() {
#ifdef Q_OS_LINUX
    _loading = true;
    const QString selectedId = selectedActivityId();
    _activityListWidget->clear();

    if (!PlasmaActivityService::isAvailable()) {
        _activityEditFrame->setEnabled(false);
        _statusLabel->setText(
            tr("The KDE Plasma activity manager is not available on the session D-Bus."));
        _loading = false;
        return;
    }

    const QString currentActivityId = PlasmaActivityService::currentActivityId();
    const QList<PlasmaActivityService::Activity> activities = PlasmaActivityService::activities();
    QString currentActivityName = currentActivityId;

    for (const auto &activity : activities) {
        const QString activityName = activity.name.isEmpty() ? activity.id : activity.name;
        auto *item = new QListWidgetItem(activityName, _activityListWidget);
        item->setData(Qt::UserRole, activity.id);

        if (activity.id == currentActivityId) {
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
            currentActivityName = activityName;
        }

        if (activity.id == selectedId ||
            (selectedId.isEmpty() && activity.id == currentActivityId)) {
            _activityListWidget->setCurrentItem(item);
        }
    }

    if (_activityListWidget->currentItem() == nullptr && _activityListWidget->count() > 0) {
        _activityListWidget->setCurrentRow(0);
    }

    _statusLabel->setText(
        tr("Activity manager connected. Current activity: %1").arg(currentActivityName));
    _loading = false;
    updateSelectedActivity();
#else
    _activityEditFrame->setEnabled(false);
    _statusLabel->setText(tr("KDE Plasma activities are only supported on Linux."));
#endif
}

void PlasmaActivitiesSettingsWidget::updateSelectedActivity() {
#ifdef Q_OS_LINUX
    const QString activityId = selectedActivityId();
    _activityEditFrame->setEnabled(!activityId.isEmpty());

    const QSignalBlocker noteFolderBlocker(_noteFolderComboBox);
    const QSignalBlocker layoutBlocker(_layoutComboBox);
    Q_UNUSED(noteFolderBlocker)
    Q_UNUSED(layoutBlocker)

    _noteFolderComboBox->clear();
    _noteFolderComboBox->addItem(tr("Do not change"), 0);
    for (const NoteFolder &noteFolder : NoteFolder::fetchAll()) {
        _noteFolderComboBox->addItem(noteFolder.getName(), noteFolder.getId());
    }
    _noteFolderComboBox->setCurrentIndex(
        _noteFolderComboBox->findData(PlasmaActivityService::noteFolderIdForActivity(activityId)));

    _layoutComboBox->clear();
    _layoutComboBox->addItem(tr("Do not change"), QString());
    const SettingsService settings;
    const QStringList layoutUuids = settings.value(QStringLiteral("layouts")).toStringList();
    for (const QString &uuid : layoutUuids) {
        _layoutComboBox->addItem(
            settings.value(QStringLiteral("layout-") + uuid + QStringLiteral("/name")).toString(),
            uuid);
    }
    _layoutComboBox->setCurrentIndex(
        _layoutComboBox->findData(PlasmaActivityService::layoutUuidForActivity(activityId)));
#endif
}

#ifdef Q_OS_LINUX
void PlasmaActivitiesSettingsWidget::setNoteFolderForActivity(const QString &activityId,
                                                              int noteFolderId) {
    if (activityId.isEmpty()) {
        return;
    }

    const int previousNoteFolderId = PlasmaActivityService::noteFolderIdForActivity(activityId);
    if (previousNoteFolderId > 0 && previousNoteFolderId != noteFolderId) {
        PlasmaActivityService::setNoteFolderActivity(
            previousNoteFolderId, false,
            PlasmaActivityService::noteFolderActivityId(previousNoteFolderId));
    }

    if (noteFolderId > 0) {
        PlasmaActivityService::setNoteFolderActivity(noteFolderId, true, activityId);
    }
}

void PlasmaActivitiesSettingsWidget::setLayoutForActivity(const QString &activityId,
                                                          const QString &layoutUuid) {
    if (activityId.isEmpty()) {
        return;
    }

    const QString previousLayoutUuid = PlasmaActivityService::layoutUuidForActivity(activityId);
    if (!previousLayoutUuid.isEmpty() && previousLayoutUuid != layoutUuid) {
        PlasmaActivityService::setLayoutActivity(
            previousLayoutUuid, false, PlasmaActivityService::layoutActivityId(previousLayoutUuid));
    }

    if (!layoutUuid.isEmpty()) {
        PlasmaActivityService::setLayoutActivity(layoutUuid, true, activityId);
    }
}
#endif
