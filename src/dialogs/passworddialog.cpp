#include "passworddialog.h"
#include "ui_passworddialog.h"

PasswordDialog::PasswordDialog(QWidget *parent, QString labelText) :
    QDialog(parent),
    ui(new Ui::PasswordDialog)
{
    ui->setupUi(this);
    if (!labelText.isEmpty()) {
        ui->label->setText(labelText);
    }
}

PasswordDialog::~PasswordDialog()
{
    delete ui;
}

QString PasswordDialog::password()
{
    return ui->lineEdit->text();
}
