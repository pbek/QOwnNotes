/*
 * Copyright (C) 2016 Patrizio Bekerle -- http://www.bekerle.com
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

#include <QColorDialog>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QSettings>
#include <QInputDialog>
#include <QUuid>
#include <QTextEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QKeyEvent>
#include <QDesktopServices>
#include "fontcolorwidget.h"
#include "ui_fontcolorwidget.h"
#include "utils/schema.h"
#include "libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_definitions.h"

FontColorWidget::FontColorWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FontColorWidget) {
    ui->setupUi(this);

    // initialize the schema selector
    initSchemaSelector();

    // initialize the text tree widget items
    initTextTreeWidgetItems();

    // we currently are using the font selector from the settings dialog
    ui->fontGroupBox->setVisible(false);

    // initialize the font selectors
//    initFontSelectors();
}

/**
 * Initializes the schema selector
 */
void FontColorWidget::initSchemaSelector() {
    ui->colorSchemeComboBox->clear();

    //
    // load the default schemes
    //
    QSettings schemaSettings(":/configurations/schemes.conf",
                             QSettings::IniFormat);
    _defaultSchemaKeys = schemaSettings.value("Editor/DefaultColorSchemes")
            .toStringList();

    QSettings settings;
    QString currentSchemaKey = settings.value("Editor/CurrentSchemaKey",
                                       _defaultSchemaKeys.length() > 0 ?
                                       _defaultSchemaKeys[0] : "").toString();
    int index = 0;
    int currentIndex = 0;

    Q_FOREACH(QString schemaKey, _defaultSchemaKeys) {
            schemaSettings.beginGroup(schemaKey);
            QString name = schemaSettings.value("Name").toString();
            ui->colorSchemeComboBox->addItem(name, schemaKey);
            schemaSettings.endGroup();

            if (currentSchemaKey == schemaKey) {
                currentIndex = index;
            }

            index++;
    }

    //
    // load the custom schemes
    //
    QStringList schemes = settings.value("Editor/ColorSchemes").toStringList();
    Q_FOREACH(QString schemaKey, schemes) {
            settings.beginGroup(schemaKey);
            QString name = settings.value("Name").toString();
            ui->colorSchemeComboBox->addItem(name, schemaKey);
            settings.endGroup();

            if (currentSchemaKey == schemaKey) {
                currentIndex = index;
            }

            index++;
    }

    // set the current color schema
    ui->colorSchemeComboBox->setCurrentIndex(currentIndex);
}

FontColorWidget::~FontColorWidget() {
    delete ui;
}

/**
 * Sets a new foreground color
 */
void FontColorWidget::on_foregroundColorButton_clicked() {
    int index = textSettingsIndex();
    QColor color = Utils::Schema::getForegroundColor(index);
    QColor newColor = QColorDialog::getColor(color);

    if (newColor.isValid()) {
        color = newColor;
    }

    ui->foregroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none}").arg(color.name()));

    setSchemaValue(textSettingsKey("ForegroundColor"), color);

    // update the current or all text items, depending on the index
    updateTextItems(index);
}

/**
 * Sets a new background color
 */
void FontColorWidget::on_backgroundColorButton_clicked() {
    int index = textSettingsIndex();
    QColor color = Utils::Schema::getBackgroundColor(index);
    QColor newColor = QColorDialog::getColor(color);

    if (newColor.isValid()) {
        color = newColor;
    }

    ui->backgroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none}").arg(color.name()));

    setSchemaValue(textSettingsKey("BackgroundColor"), color);

    // update the current or all text items, depending on the index
    updateTextItems(index);
}

/**
 * Updates the current or all text items, depending on the index
 *
 * @param index
 */
void FontColorWidget::updateTextItems(int index) {
    if (index < 0) {
        // update all text items
        updateAllTextItems();
    } else {
        // update the styling of the current text tree widget item
        updateTextItem();
    }
}

/**
 * Initializes the text tree widget items
 *
 * @see src/libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_definitions.h
 */
void FontColorWidget::initTextTreeWidgetItems() {
    addTextTreeWidgetItem(tr("Text preset"), Utils::Schema::TextPresetIndex);
    addTextTreeWidgetItem(tr("Emphasized text"), pmh_EMPH);
    addTextTreeWidgetItem(tr("Strong text"), pmh_STRONG);
    addTextTreeWidgetItem(tr("Explicit link"), pmh_LINK);
    addTextTreeWidgetItem(tr("Implicit URL link"), pmh_AUTO_LINK_URL);
    addTextTreeWidgetItem(tr("Implicit email link"), pmh_AUTO_LINK_EMAIL);
    addTextTreeWidgetItem(tr("Image definition"), pmh_IMAGE);
    addTextTreeWidgetItem(tr("Code"), pmh_CODE);
    addTextTreeWidgetItem(tr("Bullet for an unordered list item"),
                          pmh_LIST_BULLET);
    addTextTreeWidgetItem(tr("Enumerator for an ordered list item"),
                          pmh_LIST_ENUMERATOR);
    addTextTreeWidgetItem(tr("Header, level 1"), pmh_H1);
    addTextTreeWidgetItem(tr("Header, level 2"), pmh_H2);
    addTextTreeWidgetItem(tr("Header, level 3"), pmh_H3);
    addTextTreeWidgetItem(tr("Header, level 4"), pmh_H4);
    addTextTreeWidgetItem(tr("Header, level 5"), pmh_H5);
    addTextTreeWidgetItem(tr("Header, level 6"), pmh_H6);
    addTextTreeWidgetItem(tr("Horizontal rule"), pmh_HRULE);
    addTextTreeWidgetItem(tr("Blockquote"), pmh_BLOCKQUOTE);
    addTextTreeWidgetItem(tr("Verbatim (e.g. block of code)"), pmh_VERBATIM);
    addTextTreeWidgetItem("HTML", pmh_HTML);
    addTextTreeWidgetItem(tr("HTML special entity definition"),
                          pmh_HTML_ENTITY);
    addTextTreeWidgetItem(tr("(HTML) Comment"), pmh_COMMENT);
    addTextTreeWidgetItem(tr("Block of HTML"), pmh_HTMLBLOCK);
    addTextTreeWidgetItem(tr("Reference"), pmh_REFERENCE);
    addTextTreeWidgetItem(tr("Note"), pmh_NOTE);
}

void FontColorWidget::addTextTreeWidgetItem(QString text, int index) {
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, text);
    item->setData(0, Qt::UserRole, index);

    if (index == Utils::Schema::TextPresetIndex) {
        item->setToolTip(0, tr(
                "This item will be used to preset colors for the other items"));
    }

    // update the styling of the text tree widget item
    updateTextItem(item);

    ui->textTreeWidget->addTopLevelItem(item);
}

/**
 * Updates the schema edit frame for the currently selected text item
 */
void FontColorWidget::updateSchemeEditFrame() {
    QTreeWidgetItem *item = ui->textTreeWidget->currentItem();

    if (item == Q_NULLPTR) {
        // select a current item if none was selected
        ui->textTreeWidget->setCurrentItem(ui->textTreeWidget->topLevelItem(0));
        return;
    }

    int index = textSettingsIndex();

    // check if we are not viewing a default schema
    ui->schemeEditFrame->setEnabled(!_currentSchemaIsDefault);

    bool enabled = Utils::Schema::getSchemaValue(
            textSettingsKey("ForegroundColorEnabled")).toBool();
    updateForegroundColorCheckBox(enabled);

    QColor color = Utils::Schema::getForegroundColor(index);
    ui->foregroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none;}").arg(color.name()));

    enabled = Utils::Schema::getSchemaValue(
            textSettingsKey("BackgroundColorEnabled")).toBool();
    updateBackgroundColorCheckBox(enabled);

    color = Utils::Schema::getBackgroundColor(index);
    ui->backgroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none;}").arg(color.name()));

    ui->boldCheckBox->setVisible(index >= 0);
    ui->italicCheckBox->setVisible(index >= 0);
    ui->underlineCheckBox->setVisible(index >= 0);
    ui->fontSizeAdaptionSpinBox->setVisible(index >= 0);

    if (index >= 0) {
        const QSignalBlocker blocker(ui->boldCheckBox);
        Q_UNUSED(blocker);

        ui->boldCheckBox->setChecked(
                Utils::Schema::getSchemaValue(
                        textSettingsKey("Bold")).toBool());

        const QSignalBlocker blocker2(ui->italicCheckBox);
        Q_UNUSED(blocker2);

        ui->italicCheckBox->setChecked(
                Utils::Schema::getSchemaValue(
                        textSettingsKey("Italic")).toBool());

        const QSignalBlocker blocker3(ui->underlineCheckBox);
        Q_UNUSED(blocker3);

        ui->underlineCheckBox->setChecked(
                Utils::Schema::getSchemaValue(
                        textSettingsKey("Underline")).toBool());

        const QSignalBlocker blocker4(ui->fontSizeAdaptionSpinBox);
        Q_UNUSED(blocker4);

        ui->fontSizeAdaptionSpinBox->setValue(Utils::Schema::getSchemaValue(
                        textSettingsKey("FontSizeAdaption"), 100).toInt());
    }
}

/**
 * Returns the text settings key for the currently selected text
 *
 * @param key
 * @param item
 * @return
 */
QString FontColorWidget::textSettingsKey(QString key, QTreeWidgetItem *item) {
    return Utils::Schema::textSettingsKey(key, textSettingsIndex(item));
}

/**
 * Returns the text settings index for the currently selected text
 *
 * @param item
 * @return
 */
int FontColorWidget::textSettingsIndex(QTreeWidgetItem *item) {
    if (item == Q_NULLPTR) {
        item = ui->textTreeWidget->currentItem();
    }

    return item == Q_NULLPTR ? -1000 : item->data(0, Qt::UserRole).toInt();
}

/**
 * Sets a schema value
 *
 * @param schemaKey
 * @param key
 * @param value
 */
void FontColorWidget::setSchemaValue(QString key, QVariant value,
                                     QString schemaKey) {
    if (schemaKey.isEmpty()) {
        schemaKey = _currentSchemaKey;
    }

    QSettings(settings);
    settings.beginGroup(schemaKey);
    return settings.setValue(key, value);
}

/**
 * Selects the current scheme
 *
 * @param index
 */
void FontColorWidget::on_colorSchemeComboBox_currentIndexChanged(int index) {
    _currentSchemaKey = ui->colorSchemeComboBox->itemData(index).toString();
    _currentSchemaIsDefault = _defaultSchemaKeys.contains(_currentSchemaKey);

    ui->deleteSchemeButton->setEnabled(!_currentSchemaIsDefault);
    ui->schemeEditFrame->setEnabled(!_currentSchemaIsDefault);

    QSettings settings;
    settings.setValue("Editor/CurrentSchemaKey", _currentSchemaKey);

    updateSchemeEditFrame();

    // update all text items
    updateAllTextItems();
}

/**
 * Updates all text items
 */
void FontColorWidget::updateAllTextItems() {
    // update all text items
    for (int i = 0; i < ui->textTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = ui->textTreeWidget->topLevelItem(i);
        updateTextItem(item);
    }
}

/**
 * Updates the styling of certain items when the current text tree widget
 * item changes
 *
 * @param current
 * @param previous
 */
void FontColorWidget::on_textTreeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(current);
    Q_UNUSED(previous);

    // update the schema edit frame for the current item
    updateSchemeEditFrame();

    // update the styling of the current text tree widget item
    updateTextItem();
}

/**
 * Updates the styling of a text tree widget item
 * @param item
 */
void FontColorWidget::updateTextItem(QTreeWidgetItem *item) {
    if (item == Q_NULLPTR) {
        item = ui->textTreeWidget->currentItem();
    }

    if (item == Q_NULLPTR) {
        return;
    }

    int index = textSettingsIndex(item);

    // set the foreground color
    QColor color = Utils::Schema::getForegroundColor(index);
    QBrush brush = item->foreground(0);
    brush.setColor(color);
    item->setForeground(0, brush);

    // set the background color
    color = Utils::Schema::getBackgroundColor(index);
    brush = item->background(0);
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    item->setBackground(0, brush);

    QFont font = Utils::Schema::getEditorFont(index);
    font.setBold(Utils::Schema::getSchemaValue(
            textSettingsKey("Bold", item)).toBool());
    font.setItalic(Utils::Schema::getSchemaValue(
            textSettingsKey("Italic", item)).toBool());
    font.setUnderline(Utils::Schema::getSchemaValue(
                    textSettingsKey("Underline", item)).toBool());

    // adapt the font size
    Utils::Schema::adaptFontSize(index, font);

    item->setFont(0, font);
}

/**
 * Copies a color schema
 */
void FontColorWidget::on_copySchemeButton_clicked() {
    // ask the user for a new schema name
    QString name = QInputDialog::getText(this, tr("Copy color schema"),
                          tr("Color schema name"), QLineEdit::Normal,
                          ui->colorSchemeComboBox->currentText() + " (" +
                                  tr("Copy") + ")");

    if (name.isEmpty()) {
        return;
    }

    QSettings *schemaSettings = Utils::Schema::getSchemaSettings();
    schemaSettings->beginGroup(_currentSchemaKey);

    QStringList keys = schemaSettings->allKeys();
    QString uuid = QUuid::createUuid().toString();
    uuid.replace("{", "").replace("}", "");
    _currentSchemaKey = "EditorColorSchema-" + uuid;

    // store the new color schema data
    Q_FOREACH(QString key, keys) {
            QVariant value = key == "Name" ?
                             QVariant(name) :
                             Utils::Schema::getSchemaValue(key);
            setSchemaValue(key, value, _currentSchemaKey);
        }

    // add the new color schema to the color schemes list in the settings
    QSettings settings;
    QStringList schemes = settings.value("Editor/ColorSchemes").toStringList();
    schemes << _currentSchemaKey;
    settings.setValue("Editor/ColorSchemes", schemes);
    settings.setValue("Editor/CurrentSchemaKey", _currentSchemaKey);

    // select the last schema
    selectLastSchema();
}

/**
 * Selects the last schema
 */
void FontColorWidget::selectLastSchema() {
#ifdef Q_OS_MAC
    // under OS X we have to use a workaround to select the newly created schema
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_End,
                                     Qt::NoModifier);
    QApplication::postEvent(ui->colorSchemeComboBox, event);
#else
    // reload the schema selector
    initSchemaSelector();

    // set the index to the (new) last item
    ui->colorSchemeComboBox->setCurrentIndex(
            ui->colorSchemeComboBox->count() - 1);
#endif
}

/**
 * Takes the proper actions if the foreground enabler was toggled
 *
 * @param checked
 */
void FontColorWidget::on_foregroundColorCheckBox_toggled(bool checked) {
    updateForegroundColorCheckBox(checked, true);

    // update the current or all text items, depending on the index
    updateTextItems(textSettingsIndex());

    // update the scheme edit frame
    updateSchemeEditFrame();
}

void FontColorWidget::updateForegroundColorCheckBox(bool checked, bool store) {
    const QSignalBlocker blocker(ui->foregroundColorCheckBox);
    Q_UNUSED(blocker);

    ui->foregroundColorCheckBox->setChecked(checked);
    ui->foregroundColorButton->setEnabled(checked);

    // update the styling of the current text tree widget item
    updateTextItem();

    if (store && !_currentSchemaIsDefault) {
        setSchemaValue(textSettingsKey("ForegroundColorEnabled"), checked);
    }
}

/**
 * Takes the proper actions if the background enabler was toggled
 *
 * @param checked
 */
void FontColorWidget::on_backgroundColorCheckBox_toggled(bool checked) {
    updateBackgroundColorCheckBox(checked, true);

    // update the current or all text items, depending on the index
    updateTextItems(textSettingsIndex());

    // update the scheme edit frame
    updateSchemeEditFrame();
}

void FontColorWidget::updateBackgroundColorCheckBox(bool checked, bool store) {
    const QSignalBlocker blocker(ui->backgroundColorCheckBox);
    Q_UNUSED(blocker);

    ui->backgroundColorCheckBox->setChecked(checked);
    ui->backgroundColorButton->setEnabled(checked);

    // update the styling of the current text tree widget item
    updateTextItem();

    if (store && !_currentSchemaIsDefault) {
        setSchemaValue(textSettingsKey("BackgroundColorEnabled"), checked);
    }
}

/**
 * Removes the current schema
 */
void FontColorWidget::on_deleteSchemeButton_clicked() {
    if (_currentSchemaKey == "") {
        return;
    }

    if (QMessageBox::information(
            this,
            tr("Remove schema"),
            tr("Remove current schema? This cannot be undone!"),
             tr("Remove"), tr("Cancel"), QString::null,
             0, 1) != 0) {
        return;
    }

    QSettings settings;
    settings.beginGroup(_currentSchemaKey);
    // remove the group and all its keys
    settings.remove("");
    settings.endGroup();

    // remove the current schema from the list of schemas
    QStringList schemes = settings.value("Editor/ColorSchemes").toStringList();
    schemes.removeAll(_currentSchemaKey);
    settings.setValue("Editor/ColorSchemes", schemes);

    initSchemaSelector();
}

void FontColorWidget::storeCheckBoxState(QString name, bool checked) {
    if (!_currentSchemaIsDefault) {
        setSchemaValue(textSettingsKey(name), checked);
    }

    // update the styling of the current text tree widget item
    updateTextItem();
}

void FontColorWidget::on_boldCheckBox_toggled(bool checked) {
    storeCheckBoxState("Bold", checked);
}

void FontColorWidget::on_italicCheckBox_toggled(bool checked) {
    storeCheckBoxState("Italic", checked);
}

void FontColorWidget::on_underlineCheckBox_toggled(bool checked) {
    storeCheckBoxState("Underline", checked);
}

/**
 * Exports the current color schema to a file
 */
void FontColorWidget::on_exportSchemeButton_clicked() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("INI files") + " (*.ini)");
    dialog.setWindowTitle(tr("Export schema"));
    dialog.selectFile(ui->colorSchemeComboBox->currentText() + ".ini");
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.count() > 0) {
            QString fileName = fileNames.at(0);

            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(".ini");
            }

            QSettings *exportSettings =
                    new QSettings(fileName, QSettings::IniFormat);
            QSettings *schemaSettings = Utils::Schema::getSchemaSettings();

            // store the schema key
            exportSettings->setValue("Export/SchemaKey", _currentSchemaKey);

            schemaSettings->beginGroup(_currentSchemaKey);
            exportSettings->beginGroup(_currentSchemaKey);
            QStringList keys = schemaSettings->allKeys();

            // store the color schema data to the export settings
            Q_FOREACH(QString key, keys) {
                    QVariant value = Utils::Schema::getSchemaValue(key);
                    exportSettings->setValue(key, value);
                }
        }
    }
}

/**
 * Imports a schema from a file
 */
void FontColorWidget::on_importSchemeButton_clicked() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("INI files") + " (*.ini)");
    dialog.setWindowTitle(tr("Import schema"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.count() > 0) {
            Q_FOREACH(QString fileName, fileNames) {
                    QSettings *settings = new QSettings();
                    QSettings *importSettings =
                            new QSettings(fileName, QSettings::IniFormat);
                    QString schemaKey = importSettings->value(
                            "Export/SchemaKey").toString();

                    // create a new schema key for the import
                    QString uuid = QUuid::createUuid().toString();
                    uuid.replace("{", "").replace("}", "");
                    _currentSchemaKey = "EditorColorSchema-" + uuid;

                    QStringList schemes = settings->value(
                            "Editor/ColorSchemes").toStringList();
                    schemes << _currentSchemaKey;
                    settings->setValue("Editor/ColorSchemes", schemes);
                    settings->setValue("Editor/CurrentSchemaKey",
                                       _currentSchemaKey);

                    settings->beginGroup(_currentSchemaKey);
                    importSettings->beginGroup(schemaKey);
                    QStringList keys = importSettings->allKeys();

                    // store the color schema data to the settings
                    Q_FOREACH(QString key, keys) {
                            QVariant value = importSettings->value(key);
                            settings->setValue(key, value);
                        }

                    // select the last schema
                    selectLastSchema();
                }
        }
    }
}

/**
 * Initializes the font selectors
 */
void FontColorWidget::initFontSelectors() {
    QTextEdit textEdit;
    QFont font = textEdit.font();
    QSettings settings;
    QString fontString = settings.value(
            "MainWindow/noteTextEdit.font").toString();

    if (fontString != "") {
        // set the note text edit font
        font.fromString(fontString);
    } else {
        // store the default settings
        fontString = textEdit.font().toString();
        settings.setValue("MainWindow/noteTextEdit.font", fontString);
    }

    ui->fontFamilyComboBox->setCurrentFont(font);
    ui->fontSizeSpinBox->setValue(font.pointSize());
}

/**
 * Stores the font size adaption value
 *
 * @param value
 */
void FontColorWidget::on_fontSizeAdaptionSpinBox_valueChanged(int value) {
    if (!_currentSchemaIsDefault) {
        setSchemaValue(textSettingsKey("FontSizeAdaption"), value);
    }

    // update the styling of the current text tree widget item
    updateTextItem();
}

/**
 * Open the GitHub issues page to share a schema
 */
void FontColorWidget::on_shareSchemaPushButton_clicked() {
    QDesktopServices::openUrl(QUrl("https://github.com/pbek/QOwnNotes/issues"));
}
