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

#include <QString>
#include <QStringList>
#include <QWidget>

namespace Ui {
class MarkdownLspSettingsWidget;
}

class MarkdownLspSettingsWidget : public QWidget {
    Q_OBJECT

   public:
    explicit MarkdownLspSettingsWidget(QWidget *parent = nullptr);
    ~MarkdownLspSettingsWidget();

    void initialize();
    void readSettings();
    void storeSettings();

   private slots:
    void on_markdownLspEnabledCheckBox_toggled(bool checked);
    void on_markdownLspAutoDetectButton_clicked();
    void on_markdownLspResetIgnoredRulesButton_clicked();
    void on_markdownLspTestConnectionButton_clicked();

   private:
    Ui::MarkdownLspSettingsWidget *ui;

    QStringList argumentsFromUi() const;
    void updateStatusLabel(const QString &overrideText = QString());
    static QString resolveCommandPath(const QString &command);
};
