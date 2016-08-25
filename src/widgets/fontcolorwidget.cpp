#include <QColorDialog>
#include <QDebug>
#include <QTreeWidgetItem>
#include <QSettings>
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
    QStringList defaultSchemes =
            schemaSettings.value("Editor/DefaultColorSchemes")
                    .toStringList();

    _defaultSchemaKeys.clear();
    Q_FOREACH(QString schemaKey, defaultSchemes) {
            QString name = schemaSettings.value(schemaKey + "/Name").toString();
            _defaultSchemaKeys << schemaKey;

            ui->colorSchemeComboBox->addItem(name, schemaKey);
    }
}

FontColorWidget::~FontColorWidget() {
    delete ui;
}

void FontColorWidget::on_foregroundColorButton_clicked() {
    QColor color = QColorDialog::getColor();
    ui->foregroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none}").arg(color.name()));
    qDebug() << __func__ << " - 'ui->foregroundColorButton': "
             << ui->foregroundColorButton->styleSheet();
    QSettings settings;
    settings.setValue("TempColor", color);
}

//void FontColorWidget::createDefaultSetting(int schemeId, ) {
//}

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

void FontColorWidget::updateSchemeEditFrame() {
    QSettings *settings = _currentSchemaIsDefault ?
        new QSettings() :
        new QSettings(":/configurations/schemes.conf", QSettings::IniFormat);

    QColor color =
            settings->value(
                    _currentSchemaKey + "/Text_" + "-1" +
                                    "_ForegroundColor").value<QColor>();
    ui->foregroundColorButton->setStyleSheet(
            QString("* {background: %1; border: none}").arg(color.name()));
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
//    ui->schemeEditFrame->setEnabled(!_currentSchemaIsDefault);

//    updateSchemeEditFrame();
}

// TODO(pbek): updateSchemeEditFrame() when selecting a text
