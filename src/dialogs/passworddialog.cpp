#include "passworddialog.h"

#include <QPushButton>

#include "ui_passworddialog.h"

PasswordDialog::PasswordDialog(QWidget *parent, const QString &labelText, bool doubleEnterPassword)
    : MasterDialog(parent), ui(new Ui::PasswordDialog) {
    _doubleEnterPassword = doubleEnterPassword;

    ui->setupUi(this);
    afterSetupUI();

    // Set focus to the password line edit manually, because focus by tab order
    // was not working for release builds
    ui->passwordLineEdit->setFocus();

    ui->passwordLineEdit2->setVisible(doubleEnterPassword);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->errorLabel->setVisible(false);

    if (!labelText.isEmpty()) {
        ui->headlineLabel->setText(labelText);
    }

    // Only take as much height as needed, there is nothing that can grow
    resize(width(), sizeHint().height());
}

PasswordDialog::~PasswordDialog() { delete ui; }

QString PasswordDialog::password() { return ui->passwordLineEdit->text(); }

void PasswordDialog::on_passwordLineEdit_textChanged(const QString &arg1) {
    Q_UNUSED(arg1);

    if (_doubleEnterPassword) {
        checkIfPasswordsAreEqual();
    } else {
        // An empty password can't be correct
        ui->buttonBox->button(QDialogButtonBox::Ok)
            ->setEnabled(!ui->passwordLineEdit->text().isEmpty());
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
    ui->buttonBox->button(QDialogButtonBox::Ok)
        ->setEnabled(equal && !ui->passwordLineEdit->text().isEmpty());

    return equal;
}
