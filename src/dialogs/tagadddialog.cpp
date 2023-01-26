#include "tagadddialog.h"

#include "ui_tagadddialog.h"

TagAddDialog::TagAddDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::TagAddDialog) {
    ui->setupUi(this);
    ui->nameLineEdit->setFocus();
    resize(1, 1);
}

TagAddDialog::~TagAddDialog() { delete ui; }

QString TagAddDialog::name() { return ui->nameLineEdit->text(); }
