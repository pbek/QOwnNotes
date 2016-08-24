#include <QColorDialog>
#include <QDebug>
#include <QTreeWidgetItem>
#include "fontcolorwidget.h"
#include "ui_fontcolorwidget.h"

FontColorWidget::FontColorWidget(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FontColorWidget) {
    ui->setupUi(this);

    ui->colorSchemeComboBox->addItem("Default", 0);
    ui->colorSchemeComboBox->addItem("Dark", 1);

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, tr("Text"));
    ui->textTreeWidget->addTopLevelItem(item);
}

FontColorWidget::~FontColorWidget() {
    delete ui;
}

void FontColorWidget::on_foregroundColorButton_clicked() {
    QColor color = QColorDialog::getColor();
    ui->foregroundColorButton->setStyleSheet(QString("* {background: %1, border: none;}").arg(color.name()));
}
