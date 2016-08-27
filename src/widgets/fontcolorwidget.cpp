#include <QColorDialog>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QSettings>
#include <QtWidgets/QInputDialog>
#include <QUuid>
#include "fontcolorwidget.h"
#include "ui_fontcolorwidget.h"
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
            schemaSettings.endGroup();

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
    QString key = "ForegroundColor";
    QColor color = getSchemaValue(textSettingsKey(key)).value<QColor>();
    color = QColorDialog::getColor(color);
    ui->foregroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none}").arg(color.name()));
    setSchemaValue(textSettingsKey(key), color);
}

/**
 * Sets a new background color
 */
void FontColorWidget::on_backgroundColorButton_clicked() {
    QString key = "BackgroundColor";
    QColor color = getSchemaValue(textSettingsKey(key)).value<QColor>();
    color = QColorDialog::getColor(color);
    ui->backgroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none}").arg(color.name()));
    setSchemaValue(textSettingsKey(key), color);
}

/**
 * Initializes the text tree widget items
 *
 * @see src/libraries/qmarkdowntextedit/lib/peg-markdown-highlight/pmh_definitions.h
 */
void FontColorWidget::initTextTreeWidgetItems() {
    addTextTreeWidgetItem(tr("Text"), -1);
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
    ui->textTreeWidget->addTopLevelItem(item);
}

/**
 * Updates the schema edit frame for the currently selected text item
 */
void FontColorWidget::updateSchemeEditFrame() {
    QColor color = getSchemaValue(textSettingsKey(
            "ForegroundColor")).value<QColor>();
    ui->foregroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none;}").arg(color.name()));

    color = getSchemaValue(textSettingsKey(
            "BackgroundColor")).value<QColor>();
    ui->backgroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none;}").arg(color.name()));
}

/**
 * Returns the text settings key for the currently selected text
 *
 * @param key
 * @return
 */
QString FontColorWidget::textSettingsKey(QString key) {
    QTreeWidgetItem* item = ui->textTreeWidget->currentItem();
    int index = item == Q_NULLPTR ? -1000 : item->data(0, Qt::UserRole).toInt();
    return key + "_" + QString::number(index);
}

/**
 * Returns a value of the current schema
 *
 * @param key
 * @param defaultValue
 * @return
 */
QVariant FontColorWidget::getSchemaValue(QString key, QVariant defaultValue) {
    QSettings *settings = getSchemaSettings();
    settings->beginGroup(_currentSchemaKey);
    return settings->value(key, defaultValue);
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
 * Returns the current schema settings
 *
 * @return
 */
QSettings* FontColorWidget::getSchemaSettings() {
    return _currentSchemaIsDefault ?
        new QSettings(":/configurations/schemes.conf", QSettings::IniFormat) :
        new QSettings();
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
}

// TODO(pbek): updateSchemeEditFrame() when selecting a text

void FontColorWidget::on_textTreeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(current);
    Q_UNUSED(previous);
    updateSchemeEditFrame();
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

    QSettings *schemaSettings = getSchemaSettings();
    schemaSettings->beginGroup(_currentSchemaKey);

    QStringList keys = schemaSettings->allKeys();
    QString uuid = QUuid::createUuid().toString();
    uuid.replace("{", "").replace("}", "");
    QString schemaKey = "EditorColorSchema-" + uuid;

    // store the new color schema data
    Q_FOREACH(QString key, keys) {
            QVariant value = key == "Name" ?
                             QVariant(name) : getSchemaValue(key);
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

