/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 */

#pragma once

#include <QWidget>

class QComboBox;
class QListWidget;
class QListWidgetItem;
class QPushButton;

class LayoutsSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit LayoutsSettingsWidget(QWidget *parent = nullptr);

    void refreshLayouts(const QString &selectedUuid = QString());

   signals:
    void layoutStored(const QString &layoutUuid);

   private:
    QListWidget *_layoutListWidget;
    QPushButton *_switchButton;
    QPushButton *_removeButton;
    QPushButton *_renameButton;
    QPushButton *_moveUpButton;
    QPushButton *_moveDownButton;
    QComboBox *_centralWidgetComboBox;
    bool _loadingSelection = false;

    QString selectedLayoutUuid() const;
    void updateSelectedLayout();
    void updateCurrentLayout(QListWidgetItem *item);
    void addLayout();
    void addLayoutFromPreset();
    void switchToLayout(const QString &uuid = QString());
    void removeLayout();
    void renameLayout();
    void moveLayout(int offset);
};
