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
    addTextTreeWidgetItem(tr("Text"), Utils::Schema::TextIndex);
    addTextTreeWidgetItem(tr("Explicit link"), pmh_LINK);
    addTextTreeWidgetItem(tr("Implicit URL link"), pmh_AUTO_LINK_URL);
    addTextTreeWidgetItem(tr("Implicit email link"), pmh_AUTO_LINK_EMAIL);
    addTextTreeWidgetItem(tr("Image definition"), pmh_IMAGE);
    addTextTreeWidgetItem(tr("Code (inline)"), pmh_CODE);
    addTextTreeWidgetItem("HTML", pmh_HTML);
    addTextTreeWidgetItem(tr("HTML special entity definition"),
                          pmh_HTML_ENTITY);
    addTextTreeWidgetItem(tr("Emphasized text"), pmh_EMPH);
    addTextTreeWidgetItem(tr("Strong text"), pmh_STRONG);
    addTextTreeWidgetItem(tr("Bullet for an unordered list item"),
                          pmh_LIST_BULLET);
    addTextTreeWidgetItem(tr("Enumerator for an ordered list item"),
                          pmh_LIST_ENUMERATOR);
    addTextTreeWidgetItem(tr("(HTML) Comment"), pmh_COMMENT);
    addTextTreeWidgetItem(tr("Header, level 1"), pmh_H1);
    addTextTreeWidgetItem(tr("Header, level 2"), pmh_H2);
    addTextTreeWidgetItem(tr("Header, level 3"), pmh_H3);
    addTextTreeWidgetItem(tr("Header, level 4"), pmh_H4);
    addTextTreeWidgetItem(tr("Header, level 5"), pmh_H5);
    addTextTreeWidgetItem(tr("Header, level 6"), pmh_H6);
    addTextTreeWidgetItem(tr("Blockquote"), pmh_BLOCKQUOTE);
    addTextTreeWidgetItem(tr("Verbatim (e.g. block of code)"), pmh_VERBATIM);
    addTextTreeWidgetItem(tr("Block of HTML"), pmh_HTMLBLOCK);
    addTextTreeWidgetItem(tr("Horizontal rule"), pmh_HRULE);
    addTextTreeWidgetItem(tr("Reference"), pmh_REFERENCE);
    addTextTreeWidgetItem(tr("Note"), pmh_NOTE);
}

void FontColorWidget::addTextTreeWidgetItem(QString text, int id) {
    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, text);
    item->setData(0, Qt::UserRole, id);

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

    // check if we are not viewing a default schema
    ui->schemeEditFrame->setEnabled(!_currentSchemaIsDefault);

    bool enabled = Utils::Schema::getSchemaValue(
            textSettingsKey("ForegroundColorEnabled")).toBool();
    updateForegroundColorCheckBox(enabled);

    QColor color = Utils::Schema::getForegroundColor(textSettingsIndex());
    ui->foregroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none;}").arg(color.name()));

    enabled = Utils::Schema::getSchemaValue(
            textSettingsKey("BackgroundColorEnabled")).toBool();
    updateBackgroundColorCheckBox(enabled);

    color = Utils::Schema::getBackgroundColor(textSettingsIndex());
    ui->backgroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none;}").arg(color.name()));
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

    // set the foreground color
    QColor color = Utils::Schema::getForegroundColor(textSettingsIndex(item));
    QBrush brush = item->foreground(0);
    brush.setColor(color);
    item->setForeground(0, brush);

    // set the background color
    color = Utils::Schema::getBackgroundColor(textSettingsIndex(item));
    qDebug() << __func__ << " - 'BackgroundColor': " << color;
    brush = item->background(0);
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    item->setBackground(0, brush);
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
    QString schemaKey = "EditorColorSchema-" + uuid;

    // store the new color schema data
    Q_FOREACH(QString key, keys) {
            QVariant value = key == "Name" ?
                             QVariant(name) :
                             Utils::Schema::getSchemaValue(key);
            setSchemaValue(key, value, schemaKey);
        }

    // add the new color schema to the color schemes list in the settings
    QSettings settings;
    QStringList schemes = settings.value("Editor/ColorSchemes").toStringList();
    schemes << schemaKey;
    settings.setValue("Editor/ColorSchemes", schemes);

    // reload the schema selector
    initSchemaSelector();

    // set the index to the (new) last item
    ui->colorSchemeComboBox->setCurrentIndex(
            ui->colorSchemeComboBox->count() - 1);
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
