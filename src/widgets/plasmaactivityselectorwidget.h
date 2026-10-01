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

#include <QString>
#include <QWidget>

class QCheckBox;
class QComboBox;
class QLabel;

/**
 * A checkbox to enable switching on a KDE Plasma activity change and a
 * combo box to select the activity, used in the note folder and layout settings
 */
class PlasmaActivitySelectorWidget : public QWidget {
    Q_OBJECT

   public:
    explicit PlasmaActivitySelectorWidget(const QString &checkBoxText, QWidget *parent = nullptr);

    /**
     * Sets the checkbox state and selected activity without emitting
     * selectionChanged()
     */
    void setSelection(bool enabled, const QString &activityId);

    bool isSwitchingEnabled() const;
    QString activityId() const;

    /**
     * Shows a notice below the activity combo box, an empty text hides it
     */
    void setNotice(const QString &text);

    /**
     * Reloads the activity list from the KDE Plasma activity manager
     */
    void reloadActivities();

   signals:
    void selectionChanged(bool enabled, const QString &activityId);

   private:
    QCheckBox *_checkBox;
    QComboBox *_activityComboBox;
    QLabel *_noticeLabel;
    bool _isAvailable = false;

    void selectActivity(const QString &activityId);
    void updateEnabledState();
    void emitSelectionChanged();
};

#endif    // Q_OS_LINUX
