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

#include <QFont>
#include <QWidget>

class QLineEdit;

namespace Ui {
class PreviewFontSettingsWidget;
}

class PreviewFontSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit PreviewFontSettingsWidget(QWidget *parent = nullptr);
    ~PreviewFontSettingsWidget();

    void readSettings();
    void storeSettings();

   private slots:
    void on_noteTextViewButton_clicked();
    void on_noteTextViewCodeButton_clicked();
    void on_noteTextViewResetButton_clicked();
    void on_noteTextViewCodeResetButton_clicked();
    void on_noteTextViewExportButton_clicked();
    void on_noteTextViewExportResetButton_clicked();
    void on_noteTextViewUseEditorStylesCheckBox_toggled(bool checked);
    void on_noteTextViewRefreshDebounceTimeResetButton_clicked();

   private:
    Ui::PreviewFontSettingsWidget *ui;
    QFont noteTextViewFont;
    QFont noteTextViewCodeFont;
    QFont noteTextViewExportFont;

    void setFontLabel(QLineEdit *label, const QFont &font);
};
