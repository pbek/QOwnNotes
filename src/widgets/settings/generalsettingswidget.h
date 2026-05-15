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

class QAbstractButton;
class QButtonGroup;
class QCheckBox;
class QListWidgetItem;

namespace Ui {
class GeneralSettingsWidget;
}

class GeneralSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit GeneralSettingsWidget(QWidget *parent = nullptr);
    ~GeneralSettingsWidget();

    void initialize();
    void readSettings();
    void storeSettings();

   public slots:
    void setAllowOnlyOneAppInstance(bool checked);

   signals:
    void needRestart();

   private slots:
    void on_reinitializeDatabaseButton_clicked();
    void on_databaseIntegrityCheckButton_clicked();
    void on_clearAppDataAndExitButton_clicked();
    void on_setExternalEditorPathToolButton_clicked();
    void on_setExternalDiffToolPathToolButton_clicked();
    void on_addCustomNoteFileExtensionButton_clicked();
    void on_removeCustomNoteFileExtensionButton_clicked();
    void on_defaultNoteFileExtensionListWidget_itemChanged(QListWidgetItem *item);
    void on_defaultNoteFileExtensionListWidget_itemSelectionChanged();
    void on_resetMessageBoxesButton_clicked();
    void on_exportSettingsButton_clicked();
    void on_importSettingsButton_clicked();
    void on_imageScaleDownCheckBox_toggled(bool checked);
    void on_enableReadOnlyModeCheckBox_toggled(bool checked);
    void noteNotificationButtonGroupPressed(QAbstractButton *button);
    void noteNotificationNoneCheckBoxCheck();

   private:
    Ui::GeneralSettingsWidget *ui;
    QButtonGroup *_noteNotificationButtonGroup = nullptr;
    QCheckBox *_noteNotificationNoneCheckBox = nullptr;

    QListWidgetItem *addCustomNoteFileExtension(const QString &fileExtension);
    void initSearchEngineComboBox();
    QString selectExecutablePath(const QString &path, const QString &dialogName,
                                 const QString &windowTitle);
};
