#include "passworddialog.h"

#include <QPushButton>

#include "ui_passworddialog.h"

PasswordDialog::PasswordDialog(QWidget *parent, const QString &labelText, bool doubleEnterPassword)
    : MasterDialog(parent), ui(new Ui::PasswordDialog) {
    _doubleEnterPassword = doubleEnterPassword;

    ui->setupUi(this);

    ui->passwordLineEdit2->setVisible(doubleEnterPassword);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!doubleEnterPassword);
    ui->errorLabel->setVisible(false);

    if (!labelText.isEmpty()) {
        ui->headlineLabel->setText(labelText);
    }
}

PasswordDialog::~PasswordDialog() { delete ui; }

QString PasswordDialog::password() { return ui->passwordLineEdit->text(); }

void PasswordDialog::on_passwordLineEdit_textChanged(const QString &arg1) {
    Q_UNUSED(arg1);

    if (_doubleEnterPassword) {
        checkIfPasswordsAreEqual();
    }
}

void PasswordDialog::on_passwordLineEdit2_textChanged(const QString &arg1) {
    on_passwordLineEdit_textChanged(arg1);
}

/**
 * @brief Checks if the two passwords are equal
 * @return
 */
bool PasswordDialog::checkIfPasswordsAreEqual() {
    bool equal = ui->passwordLineEdit->text() == ui->passwordLineEdit2->text();

    ui->errorLabel->setVisible(!equal && !ui->passwordLineEdit2->text().isEmpty());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(equal);

    return equal;
}
