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

#include "editorfontcolorsettingswidget.h"

#include <services/settingsservice.h>
#include <utils/gui.h>

#include <QFontDatabase>
#include <QFontDialog>
#include <QLineEdit>
#include <QTextEdit>

#include "ui_editorfontcolorsettingswidget.h"

EditorFontColorSettingsWidget::EditorFontColorSettingsWidget(QWidget *parent)
    : QWidget(parent), ui(new Ui::EditorFontColorSettingsWidget) {
    ui->setupUi(this);

#if QT_VERSION < QT_VERSION_CHECK(6, 7, 0)
    ui->enableFontLigaturesCheckBox->setEnabled(false);
    ui->enableFontLigaturesCheckBox->setToolTip(
        tr("Font ligature control requires Qt 6.7 or newer."));
#endif
    connect(ui->enableFontLigaturesCheckBox, &QCheckBox::clicked, this,
            [this]() { emit needRestart(); });

    // Connect schema change signal to apply dark mode settings
    connect(ui->editorFontColorWidget, &FontColorWidget::schemaChanged, this,
            []() { Utils::Gui::applyDarkModeSettings(); });
}

EditorFontColorSettingsWidget::~EditorFontColorSettingsWidget() { delete ui; }

void EditorFontColorSettingsWidget::readSettings() {
    SettingsService settings;

    noteTextEditFont.fromString(
        settings.value(QStringLiteral("MainWindow/noteTextEdit.font")).toString());
    setFontLabel(ui->noteTextEditFontLabel, noteTextEditFont);

    noteTextEditCodeFont.fromString(
        settings.value(QStringLiteral("MainWindow/noteTextEdit.code.font")).toString());
    setFontLabel(ui->noteTextEditCodeFontLabel, noteTextEditCodeFont);
    ui->enableFontLigaturesCheckBox->setChecked(
        settings.value(QStringLiteral("Editor/enableFontLigatures"), false).toBool());
}

void EditorFontColorSettingsWidget::storeSettings() {
    storeFontSettings();
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    SettingsService().setValue(QStringLiteral("Editor/enableFontLigatures"),
                               ui->enableFontLigaturesCheckBox->isChecked());
#endif
}

void EditorFontColorSettingsWidget::setWikiLinkItemsVisible(bool visible) {
    ui->editorFontColorWidget->setWikiLinkItemsVisible(visible);
}

void EditorFontColorSettingsWidget::storeFontSettings() {
    SettingsService settings;
    settings.setValue(QStringLiteral("MainWindow/noteTextEdit.font"), noteTextEditFont.toString());
    settings.setValue(QStringLiteral("MainWindow/noteTextEdit.code.font"),
                      noteTextEditCodeFont.toString());
}

void EditorFontColorSettingsWidget::setFontLabel(QLineEdit *label, const QFont &font) {
    label->setText(font.family() + " (" + QString::number(font.pointSize()) + ")");
    label->setFont(font);
}

void EditorFontColorSettingsWidget::on_noteTextEditButton_clicked() {
    bool ok;
    QFont font = Utils::Gui::fontDialogGetFont(&ok, noteTextEditFont, this);

    if (ok) {
        noteTextEditFont = font;
        setFontLabel(ui->noteTextEditFontLabel, noteTextEditFont);

        storeFontSettings();

        emit needRestart();

        ui->editorFontColorWidget->updateAllTextItems();
    }
}

void EditorFontColorSettingsWidget::on_noteTextEditCodeButton_clicked() {
    bool ok;
    QFont font = Utils::Gui::fontDialogGetFont(&ok, noteTextEditCodeFont, this, QString(),
                                               QFontDialog::MonospacedFonts);

    if (ok) {
        noteTextEditCodeFont = font;
        setFontLabel(ui->noteTextEditCodeFontLabel, noteTextEditCodeFont);

        storeFontSettings();

        emit needRestart();

        ui->editorFontColorWidget->updateAllTextItems();
    }
}

void EditorFontColorSettingsWidget::on_noteTextEditResetButton_clicked() {
    QTextEdit textEdit;
    noteTextEditFont = textEdit.font();
    setFontLabel(ui->noteTextEditFontLabel, noteTextEditFont);

    storeFontSettings();

    emit needRestart();

    ui->editorFontColorWidget->updateAllTextItems();
}

void EditorFontColorSettingsWidget::on_noteTextEditCodeResetButton_clicked() {
    noteTextEditCodeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFontLabel(ui->noteTextEditCodeFontLabel, noteTextEditCodeFont);

    storeFontSettings();

    emit needRestart();

    ui->editorFontColorWidget->updateAllTextItems();
}
