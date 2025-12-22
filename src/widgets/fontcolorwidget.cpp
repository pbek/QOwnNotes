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

#include "fontcolorwidget.h"

#include <dialogs/filedialog.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QColorDialog>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMessageBox>
#include <QTextEdit>
#include <QTreeWidgetItem>
#include <QUuid>

#include "services/settingsservice.h"
#include "ui_fontcolorwidget.h"
#include "utils/schema.h"

FontColorWidget::FontColorWidget(QWidget* parent) : QFrame(parent), ui(new Ui::FontColorWidget) {
    ui->setupUi(this);

    // initialize the schema selector
    initSchemaSelector();

    // initialize the text tree widget items
    initTextTreeWidgetItems();

    // we currently are using the font selector from the settings dialog
    ui->fontGroupBox->setVisible(false);

    // disable the button under macOS for Qt < 5.8 because of a Qt bug
    // see: https://github.com/pbek/QOwnNotes/issues/503
    // see: https://bugreports.qt.io/browse/QTBUG-56565
#ifdef Q_OS_MAC
#if (QT_VERSION < QT_VERSION_CHECK(5, 8, 0))
    ui->copySchemeButton->setEnabled(false);
    ui->copySchemeButton->setToolTip(
        "Disabled because of a Qt bug, see "
        "https://github.com/pbek/QOwnNotes/issues/503");
#endif
#endif

    // initialize the font selectors
    //    initFontSelectors();

    // declare that we need to restart the application if certain settings
    // are changed
    connect(ui->colorSchemeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(needRestart()));
    connect(ui->fontCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));
    connect(ui->foregroundColorCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));
    connect(ui->backgroundColorCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));
    connect(ui->boldCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));
    connect(ui->italicCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));
    connect(ui->underlineCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));
    connect(ui->foregroundColorButton, SIGNAL(clicked()), this, SLOT(needRestart()));
    connect(ui->backgroundColorButton, SIGNAL(clicked()), this, SLOT(needRestart()));
    connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(needRestart()));
}

/**
 * Initializes the schema selector
 */
void FontColorWidget::initSchemaSelector() {
    ui->colorSchemeComboBox->clear();
    ui->fontComboBox->setEnabled(false);

    //
    // Load the default schemes
    //

    _defaultSchemaKeys = Utils::Schema::schemaSettings->defaultSchemaKeys();
    // QMaps are sorted by key automatically
    QMap<QString, QString> defaultSchemaNameKeys;

    const QSettings& defaultSchemaSettings = Utils::Schema::schemaSettings->defaultSchemaSettings();

    // Gather the default schema names and keys in correctly sorted order
    Q_FOREACH (const QString& schemaKey, _defaultSchemaKeys) {
        QString name = defaultSchemaSettings.value(schemaKey + "/Name").toString();

        // Enforce the light theme to be on top
        if (schemaKey == QStringLiteral("EditorColorSchema-6033d61b-cb96-46d5-a3a8-20d5172017eb")) {
            name = QStringLiteral("  ") + name;
        }

        // Enforce the dark theme to be 2nd
        if (schemaKey == QStringLiteral("EditorColorSchema-cdbf28fc-1ddc-4d13-bb21-6a4043316a2f")) {
            name = QStringLiteral(" ") + name;
        }

        defaultSchemaNameKeys.insert(name, schemaKey);
    }

    SettingsService settings;
    QString currentSchemaKey =
        settings
            .value(QStringLiteral("Editor/CurrentSchemaKey"),
                   _defaultSchemaKeys.length() > 0 ? _defaultSchemaKeys[0] : QString())
            .toString();
    int index = 0;
    int currentIndex = 0;

    Q_FOREACH (const QString& schemaKey, defaultSchemaNameKeys.values()) {
        const QString name = defaultSchemaSettings.value(schemaKey + "/Name").toString();
        ui->colorSchemeComboBox->addItem(name.trimmed(), schemaKey);

        if (currentSchemaKey == schemaKey) {
            currentIndex = index;
        }

        index++;
    }

    //
    // Load the custom schemes
    //
    QStringList schemes = settings.value(QStringLiteral("Editor/ColorSchemes")).toStringList();
    Q_FOREACH (QString schemaKey, schemes) {
        settings.beginGroup(schemaKey);
        QString name = settings.value(QStringLiteral("Name")).toString();
        ui->colorSchemeComboBox->addItem(name, schemaKey);
        settings.endGroup();

        if (currentSchemaKey == schemaKey) {
            currentIndex = index;
        }

        index++;
    }

    // Set the current color schema
    ui->colorSchemeComboBox->setCurrentIndex(currentIndex);
}

FontColorWidget::~FontColorWidget() { delete ui; }

/**
 * Sets a new foreground color
 */
void FontColorWidget::on_foregroundColorButton_clicked() {
    int index = textSettingsIndex();
    QColor color = Utils::Schema::schemaSettings->getForegroundColor(index);
    QColor newColor = QColorDialog::getColor(color);

    if (newColor.isValid()) {
        color = newColor;
    }

    ui->foregroundColorButton->setStyleSheet(
        QStringLiteral("* {background: %1; border: none}").arg(color.name()));

    setSchemaValue(textSettingsKey(QStringLiteral("ForegroundColor")), color);

    // update the current or all text items, depending on the index
    updateTextItems(index);
}

/**
 * Sets a new background color
 */
void FontColorWidget::on_backgroundColorButton_clicked() {
    int index = textSettingsIndex();
    QColor color = Utils::Schema::schemaSettings->getBackgroundColor(index);
    QColor newColor = QColorDialog::getColor(color);

    if (newColor.isValid()) {
        color = newColor;
    }

    ui->backgroundColorButton->setStyleSheet(
        QStringLiteral("* {background: %1; border: none}").arg(color.name()));

    setSchemaValue(textSettingsKey(QStringLiteral("BackgroundColor")), color);

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
 */
void FontColorWidget::initTextTreeWidgetItems() {
    addTextTreeWidgetItem(tr("Text preset"), Utils::Schema::TextPresetIndex);
    addTextTreeWidgetItem(tr("Emphasized text"), MarkdownHighlighter::Italic);
    addTextTreeWidgetItem(tr("Underlined text"), MarkdownHighlighter::StUnderline);
    addTextTreeWidgetItem(tr("Strong text"), MarkdownHighlighter::Bold);
    addTextTreeWidgetItem(tr("Link"), MarkdownHighlighter::Link);
    addTextTreeWidgetItem(tr("Image"), MarkdownHighlighter::Image);
    addTextTreeWidgetItem(tr("Code (block)"), MarkdownHighlighter::CodeBlock);
    addTextTreeWidgetItem(tr("Code (inline)"), MarkdownHighlighter::InlineCodeBlock);

    addTextTreeWidgetItem(tr("Code (keyword)"), MarkdownHighlighter::CodeKeyWord);
    addTextTreeWidgetItem(tr("Code (type)"), MarkdownHighlighter::CodeType);
    addTextTreeWidgetItem(tr("Code (comment)"), MarkdownHighlighter::CodeComment);
    addTextTreeWidgetItem(tr("Code (string)"), MarkdownHighlighter::CodeString);
    addTextTreeWidgetItem(tr("Code (built in)"), MarkdownHighlighter::CodeBuiltIn);
    addTextTreeWidgetItem(tr("Code (num literal)"), MarkdownHighlighter::CodeNumLiteral);
    addTextTreeWidgetItem(tr("Code (other)"), MarkdownHighlighter::CodeOther);

    addTextTreeWidgetItem(tr("List item"), MarkdownHighlighter::List);
    addTextTreeWidgetItem(tr("Checkbox unchecked"), MarkdownHighlighter::CheckBoxUnChecked);
    addTextTreeWidgetItem(tr("Checkbox checked"), MarkdownHighlighter::CheckBoxChecked);
    addTextTreeWidgetItem(tr("Header, level 1"), MarkdownHighlighter::H1);
    addTextTreeWidgetItem(tr("Header, level 2"), MarkdownHighlighter::H2);
    addTextTreeWidgetItem(tr("Header, level 3"), MarkdownHighlighter::H3);
    addTextTreeWidgetItem(tr("Header, level 4"), MarkdownHighlighter::H4);
    addTextTreeWidgetItem(tr("Header, level 5"), MarkdownHighlighter::H5);
    addTextTreeWidgetItem(tr("Header, level 6"), MarkdownHighlighter::H6);
    addTextTreeWidgetItem(tr("Horizontal rule"), MarkdownHighlighter::HorizontalRuler);
    addTextTreeWidgetItem(tr("Block quote"), MarkdownHighlighter::BlockQuote);
    addTextTreeWidgetItem(tr("Table"), MarkdownHighlighter::Table);
    addTextTreeWidgetItem(tr("(HTML) Comment"), MarkdownHighlighter::Comment);
    addTextTreeWidgetItem(tr("Masked syntax",
                             "text that will highlighted in "
                             "a way that it's barely visible"),
                          MarkdownHighlighter::MaskedSyntax);
    addTextTreeWidgetItem(tr("Current line background color"),
                          MarkdownHighlighter::CurrentLineBackgroundColor);
    addTextTreeWidgetItem(tr("Broken link"), MarkdownHighlighter::BrokenLink);
    addTextTreeWidgetItem(tr("Trailing space"), MarkdownHighlighter::TrailingSpace);

    // jump to the top of the list so that an item is selected
    auto* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
    QApplication::postEvent(ui->textTreeWidget, event);
}

void FontColorWidget::addTextTreeWidgetItem(const QString& text, int index) {
    auto* item = new QTreeWidgetItem();
    item->setText(0, text);
    item->setData(0, Qt::UserRole, index);

    if (index == Utils::Schema::TextPresetIndex) {
        item->setToolTip(0, tr("This item will be used to preset colors for the other items"));
    }

    // update the styling of the text tree widget item
    updateTextItem(item);

    ui->textTreeWidget->addTopLevelItem(item);
}

/**
 * Updates the schema edit frame for the currently selected text item
 */
void FontColorWidget::updateSchemeEditFrame() {
    QTreeWidgetItem* item = ui->textTreeWidget->currentItem();

    if (item == nullptr) {
        // select a current item if none was selected
        ui->textTreeWidget->setCurrentItem(ui->textTreeWidget->topLevelItem(0));
        return;
    }

    int index = textSettingsIndex();

    // check if we are not viewing a default schema
    ui->schemeEditFrame->setEnabled(!_currentSchemaIsDefault);

    bool enabled = Utils::Schema::schemaSettings
                       ->getSchemaValue(textSettingsKey(QStringLiteral("FontEnabled")))
                       .toBool();
    updateFontCheckBox(enabled);

    enabled = Utils::Schema::schemaSettings
                  ->getSchemaValue(textSettingsKey(QStringLiteral("ForegroundColorEnabled")))
                  .toBool();
    updateForegroundColorCheckBox(enabled);

    QColor color = Utils::Schema::schemaSettings->getForegroundColor(index);
    ui->foregroundColorButton->setStyleSheet(
        QStringLiteral("* {background: %1; border: none;}").arg(color.name()));

    enabled = Utils::Schema::schemaSettings
                  ->getSchemaValue(textSettingsKey(QStringLiteral("BackgroundColorEnabled")))
                  .toBool();
    updateBackgroundColorCheckBox(enabled);

    color = Utils::Schema::schemaSettings->getBackgroundColor(index);
    ui->backgroundColorButton->setStyleSheet(
        QStringLiteral("* {background: %1; border: none;}").arg(color.name()));

    bool isCurrentLineBackgroundColorIndex =
        index == MarkdownHighlighter::HighlighterState::CurrentLineBackgroundColor;

    ui->boldCheckBox->setVisible(index >= 0 && !isCurrentLineBackgroundColorIndex);
    ui->italicCheckBox->setVisible(index >= 0 && !isCurrentLineBackgroundColorIndex);
    ui->underlineCheckBox->setVisible(index >= 0 && !isCurrentLineBackgroundColorIndex);
    ui->fontSizeAdaptionSpinBox->setVisible(index >= 0 && !isCurrentLineBackgroundColorIndex);
    ui->fontCheckBox->setVisible(index >= 0 && !isCurrentLineBackgroundColorIndex);
    ui->fontComboBox->setVisible(index >= 0 && !isCurrentLineBackgroundColorIndex);
    ui->foregroundColorCheckBox->setVisible(!isCurrentLineBackgroundColorIndex);
    ui->foregroundColorButton->setVisible(!isCurrentLineBackgroundColorIndex);
    ui->label->setVisible(!isCurrentLineBackgroundColorIndex);
    ui->fontSizeAdaptionSpinBox->setVisible(!isCurrentLineBackgroundColorIndex);

    if (index >= 0) {
        const QSignalBlocker blocker(ui->boldCheckBox);
        Q_UNUSED(blocker)

        ui->boldCheckBox->setChecked(
            Utils::Schema::schemaSettings->getSchemaValue(textSettingsKey(QStringLiteral("Bold")))
                .toBool());

        const QSignalBlocker blocker2(ui->italicCheckBox);
        Q_UNUSED(blocker2)

        ui->italicCheckBox->setChecked(
            Utils::Schema::schemaSettings->getSchemaValue(textSettingsKey(QStringLiteral("Italic")))
                .toBool());

        const QSignalBlocker blocker3(ui->underlineCheckBox);
        Q_UNUSED(blocker3)

        ui->underlineCheckBox->setChecked(
            Utils::Schema::schemaSettings
                ->getSchemaValue(textSettingsKey(QStringLiteral("Underline")))
                .toBool());

        const QSignalBlocker blocker4(ui->fontSizeAdaptionSpinBox);
        Q_UNUSED(blocker4)

        ui->fontSizeAdaptionSpinBox->setValue(
            Utils::Schema::schemaSettings
                ->getSchemaValue(textSettingsKey(QStringLiteral("FontSizeAdaption")), 100)
                .toInt());

        const QSignalBlocker blocker5(ui->fontComboBox);
        Q_UNUSED(blocker5)

        QFont font = Utils::Schema::schemaSettings
                         ->getSchemaValue(textSettingsKey(QStringLiteral("Font")),
                                          Utils::Schema::schemaSettings->getEditorTextFont())
                         .value<QFont>();

        ui->fontComboBox->setCurrentFont(font);
    }
}

/**
 * Returns the text settings key for the currently selected text
 *
 * @param key
 * @param item
 * @return
 */
QString FontColorWidget::textSettingsKey(const QString& key, QTreeWidgetItem* item) {
    return Utils::Schema::textSettingsKey(key, textSettingsIndex(item));
}

/**
 * Returns the text settings index for the currently selected text
 *
 * @param item
 * @return
 */
int FontColorWidget::textSettingsIndex(QTreeWidgetItem* item) {
    if (item == nullptr) {
        item = ui->textTreeWidget->currentItem();
    }

    return item == nullptr ? -1000 : item->data(0, Qt::UserRole).toInt();
}

/**
 * Sets a schema value
 *
 * @param schemaKey
 * @param key
 * @param value
 */
void FontColorWidget::setSchemaValue(const QString& key, const QVariant& value, QString schemaKey) {
    if (schemaKey.isEmpty()) {
        schemaKey = _currentSchemaKey;
    }

    SettingsService settings;
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

    SettingsService settings;
    settings.setValue(QStringLiteral("Editor/CurrentSchemaKey"), _currentSchemaKey);

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
        QTreeWidgetItem* item = ui->textTreeWidget->topLevelItem(i);
        updateTextItem(item);
    }
}

bool FontColorWidget::selectFirstLightSchema() {
    if (ui->colorSchemeComboBox->count() >= 1) {
        ui->colorSchemeComboBox->setCurrentIndex(0);
        return true;
    }

    return false;
}

bool FontColorWidget::selectFirstDarkSchema() {
    if (ui->colorSchemeComboBox->count() >= 2) {
        ui->colorSchemeComboBox->setCurrentIndex(1);
        return true;
    }

    return false;
}

/**
 * Updates the styling of certain items when the current text tree widget
 * item changes
 *
 * @param current
 * @param previous
 */
void FontColorWidget::on_textTreeWidget_currentItemChanged(QTreeWidgetItem* current,
                                                           QTreeWidgetItem* previous) {
    Q_UNUSED(current)
    Q_UNUSED(previous)

    // update the schema edit frame for the current item
    updateSchemeEditFrame();

    // update the styling of the current text tree widget item
    updateTextItem();
}

/**
 * Updates the styling of a text tree widget item
 * @param item
 */
void FontColorWidget::updateTextItem(QTreeWidgetItem* item) {
    if (item == nullptr) {
        item = ui->textTreeWidget->currentItem();
    }

    if (item == nullptr) {
        return;
    }

    int index = textSettingsIndex(item);

    // set the foreground color
    QColor color = Utils::Schema::schemaSettings->getForegroundColor(index);
    QBrush brush = item->foreground(0);
    brush.setColor(color);
    brush.setStyle(Qt::BrushStyle::SolidPattern);
    item->setForeground(0, brush);

    // set the background color
    color = Utils::Schema::schemaSettings->getBackgroundColor(index);
    brush = item->background(0);
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    item->setBackground(0, brush);

    QFont font = Utils::Schema::schemaSettings->getFont(index);
    font.setBold(
        Utils::Schema::schemaSettings->getSchemaValue(textSettingsKey("Bold", item)).toBool());
    font.setItalic(
        Utils::Schema::schemaSettings->getSchemaValue(textSettingsKey("Italic", item)).toBool());
    font.setUnderline(
        Utils::Schema::schemaSettings->getSchemaValue(textSettingsKey("Underline", item)).toBool());

    // adapt the font size
    Utils::Schema::schemaSettings->adaptFontSize(index, font);

    item->setFont(0, font);
}

/**
 * Copies a color schema
 */
void FontColorWidget::on_copySchemeButton_clicked() {
    // ask the user for a new schema name
    QString name = QInputDialog::getText(
        this, tr("Copy color schema"), tr("Color schema name"), QLineEdit::Normal,
        ui->colorSchemeComboBox->currentText() + " (" + tr("Copy", "as noun") + ")");

    if (name.isEmpty()) {
        return;
    }

    const QStringList& keys = Utils::Schema::schemaSettings->getSchemaKeys(_currentSchemaKey);
    QString uuid = Utils::Misc::createUuidString();
    _currentSchemaKey = "EditorColorSchema-" + uuid;

    // store the new color schema data
    Q_FOREACH (const QString& key, keys) {
        QVariant value =
            key == "Name" ? QVariant(name) : Utils::Schema::schemaSettings->getSchemaValue(key);
        setSchemaValue(key, value, _currentSchemaKey);
    }

    // add the new color schema to the color schemes list in the settings
    SettingsService settings;
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
    // reload the schema selector
    initSchemaSelector();

#ifdef Q_OS_MAC
    // under OS X we have to use a workaround to select the newly created schema
    QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_End, Qt::NoModifier);
    QApplication::postEvent(ui->colorSchemeComboBox, event);
#else
    // set the index to the (new) last item
    ui->colorSchemeComboBox->setCurrentIndex(ui->colorSchemeComboBox->count() - 1);
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
    Q_UNUSED(blocker)

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
    Q_UNUSED(blocker)

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
    if (_currentSchemaKey.isEmpty()) {
        return;
    }

    if (Utils::Gui::question(this, tr("Remove schema"),
                             tr("Remove current schema? This cannot be undone!"),
                             "remove-color-schema") != QMessageBox::Yes) {
        return;
    }

    SettingsService settings;
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

void FontColorWidget::storeCheckBoxState(const QString& name, bool checked) {
    if (!_currentSchemaIsDefault) {
        setSchemaValue(textSettingsKey(name), checked);
    }

    // update the styling of the current text tree widget item
    updateTextItem();
}

void FontColorWidget::on_boldCheckBox_toggled(bool checked) { storeCheckBoxState("Bold", checked); }

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
    FileDialog dialog("SchemaExport");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("INI files") + " (*.ini)");
    dialog.setWindowTitle(tr("Export schema"));
    dialog.selectFile(ui->colorSchemeComboBox->currentText() + ".ini");
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(".ini");
            }

            QSettings exportSettings(fileName, QSettings::IniFormat);

            // clear the settings in case the settings file already existed
            exportSettings.clear();

            // store the schema key
            exportSettings.setValue("Export/SchemaKey", _currentSchemaKey);

            exportSettings.beginGroup(_currentSchemaKey);
            const QStringList& keys =
                Utils::Schema::schemaSettings->getSchemaKeys(_currentSchemaKey);

            // store the color schema data to the export settings
            Q_FOREACH (const QString& key, keys) {
                QVariant value = Utils::Schema::schemaSettings->getSchemaValue(key);
                exportSettings.setValue(key, value);
            }

            Utils::Misc::openFolderSelect(fileName);
        }
    }
}

/**
 * Imports a schema from a file
 */
void FontColorWidget::on_importSchemeButton_clicked() {
    FileDialog dialog("SchemaImport");
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("INI files") + " (*.ini)");
    dialog.setWindowTitle(tr("Import schema"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.count() > 0) {
            Q_FOREACH (QString fileName, fileNames) {
                SettingsService settings;
                QSettings importSettings(fileName, QSettings::IniFormat);
                QString schemaKey = importSettings.value("Export/SchemaKey").toString();

                // create a new schema key for the import
                QString uuid = Utils::Misc::createUuidString();
                _currentSchemaKey = "EditorColorSchema-" + uuid;

                QStringList schemes = settings.value("Editor/ColorSchemes").toStringList();
                schemes << _currentSchemaKey;
                settings.setValue("Editor/ColorSchemes", schemes);
                settings.setValue("Editor/CurrentSchemaKey", _currentSchemaKey);

                settings.beginGroup(_currentSchemaKey);
                importSettings.beginGroup(schemaKey);
                QStringList keys = importSettings.allKeys();

                // store the color schema data to the settings
                Q_FOREACH (QString key, keys) {
                    QVariant value = importSettings.value(key);
                    settings.setValue(key, value);
                }

                // select the last schema
                selectLastSchema();

                // just to make sure
                settings.endGroup();
                importSettings.endGroup();
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
    SettingsService settings;
    QString fontString = settings.value("MainWindow/noteTextEdit.font").toString();

    if (!fontString.isEmpty()) {
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
 * Opens a new GitHub issue to share a schema
 */
void FontColorWidget::on_shareSchemaPushButton_clicked() {
    QDesktopServices::openUrl(QUrl("https://github.com/pbek/QOwnNotes/issues/new/choose"));
}

/**
 * Declares that we need a restart
 */
void FontColorWidget::needRestart() { Utils::Misc::needRestart(); }

void FontColorWidget::on_fontCheckBox_toggled(bool checked) {
    updateFontCheckBox(checked, true);

    // update the current or all text items, depending on the index
    updateTextItems(textSettingsIndex());

    // update the scheme edit frame
    updateSchemeEditFrame();
}

void FontColorWidget::updateFontCheckBox(bool checked, bool store) {
    const QSignalBlocker blocker(ui->fontCheckBox);
    Q_UNUSED(blocker)

    ui->fontCheckBox->setChecked(checked);
    ui->fontComboBox->setEnabled(checked);

    // update the styling of the current text tree widget item
    updateTextItem();

    if (store && !_currentSchemaIsDefault) {
        setSchemaValue(textSettingsKey("FontEnabled"), checked);
    }
}

void FontColorWidget::on_fontComboBox_currentFontChanged(const QFont& f) {
    setSchemaValue(textSettingsKey("Font"), f);

    // update the styling of the current text tree widget item
    updateTextItem();
}
