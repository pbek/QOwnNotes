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

#include "plasmaactivityselectorwidget.h"

#ifdef Q_OS_LINUX

#include <services/plasmaactivityservice.h>

#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QSignalBlocker>
#include <QVBoxLayout>

PlasmaActivitySelectorWidget::PlasmaActivitySelectorWidget(const QString &checkBoxText,
                                                           QWidget *parent)
    : QWidget(parent) {
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    _checkBox = new QCheckBox(checkBoxText, this);
    _checkBox->setObjectName(QStringLiteral("plasmaActivityCheckBox"));
    layout->addWidget(_checkBox);

    _activityComboBox = new QComboBox(this);
    _activityComboBox->setObjectName(QStringLiteral("plasmaActivityComboBox"));
    _activityComboBox->setToolTip(tr("KDE Plasma activity"));
    layout->addWidget(_activityComboBox);

    _noticeLabel = new QLabel(this);
    _noticeLabel->setWordWrap(true);
    _noticeLabel->setVisible(false);
    layout->addWidget(_noticeLabel);

    reloadActivities();

    connect(_checkBox, &QCheckBox::toggled, this, [this](bool checked) {
        // Preselect the current activity when switching is enabled for the first time
        if (checked && activityId().isEmpty()) {
            selectActivity(PlasmaActivityService::currentActivityId());
        }

        updateEnabledState();
        emitSelectionChanged();
    });

    connect(_activityComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            [this](int index) {
                Q_UNUSED(index)
                emitSelectionChanged();
            });
}

void PlasmaActivitySelectorWidget::reloadActivities() {
    _isAvailable = PlasmaActivityService::isAvailable();
    const QString selectedActivityId = activityId();

    {
        const QSignalBlocker blocker(_activityComboBox);
        Q_UNUSED(blocker)
        _activityComboBox->clear();

        const QList<PlasmaActivityService::Activity> activities =
            PlasmaActivityService::activities();

        for (const PlasmaActivityService::Activity &activity : activities) {
            const QString name = activity.name.isEmpty() ? activity.id : activity.name;
            _activityComboBox->addItem(name, activity.id);
        }
    }

    selectActivity(selectedActivityId);

    const QString toolTip =
        _isAvailable ? QString()
                     : tr("The KDE Plasma activity manager is not available on the session D-Bus");
    _checkBox->setToolTip(toolTip);
    updateEnabledState();
}

void PlasmaActivitySelectorWidget::setSelection(bool enabled, const QString &activityId) {
    const QSignalBlocker checkBoxBlocker(_checkBox);
    Q_UNUSED(checkBoxBlocker)
    const QSignalBlocker comboBoxBlocker(_activityComboBox);
    Q_UNUSED(comboBoxBlocker)

    _checkBox->setChecked(enabled);
    selectActivity(activityId);
    setNotice(QString());
    updateEnabledState();
}

bool PlasmaActivitySelectorWidget::isSwitchingEnabled() const { return _checkBox->isChecked(); }

QString PlasmaActivitySelectorWidget::activityId() const {
    return _activityComboBox->currentData().toString();
}

void PlasmaActivitySelectorWidget::setNotice(const QString &text) {
    _noticeLabel->setText(text);
    _noticeLabel->setVisible(!text.isEmpty());
}

/**
 * Selects an activity in the combo box, activities that don't exist
 * anymore are added as unknown activity, so the link isn't lost
 */
void PlasmaActivitySelectorWidget::selectActivity(const QString &activityId) {
    const QSignalBlocker blocker(_activityComboBox);
    Q_UNUSED(blocker)

    if (activityId.isEmpty()) {
        _activityComboBox->setCurrentIndex(-1);
        return;
    }

    int index = _activityComboBox->findData(activityId);

    if (index == -1) {
        _activityComboBox->addItem(tr("Unknown activity (%1)").arg(activityId), activityId);
        index = _activityComboBox->count() - 1;
    }

    _activityComboBox->setCurrentIndex(index);
}

void PlasmaActivitySelectorWidget::updateEnabledState() {
    // Allow to disable a link even if the activity manager isn't available
    _checkBox->setEnabled(_isAvailable || _checkBox->isChecked());
    _activityComboBox->setEnabled(_isAvailable && _checkBox->isChecked());
}

void PlasmaActivitySelectorWidget::emitSelectionChanged() {
    setNotice(QString());
    emit selectionChanged(isSwitchingEnabled(), activityId());
}

#endif    // Q_OS_LINUX
