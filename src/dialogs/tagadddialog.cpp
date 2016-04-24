#include "tagadddialog.h"
#include "ui_tagadddialog.h"

TagAddDialog::TagAddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TagAddDialog)
{
    ui->setupUi(this);
    ui->nameLineEdit->setFocus();
}

TagAddDialog::~TagAddDialog()
{
    delete ui;
}

QString TagAddDialog::name() {
    return ui->nameLineEdit->text();
}
