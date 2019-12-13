#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>
#include "masterdialog.h"

namespace Ui {
class PasswordDialog;
}

class PasswordDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit PasswordDialog(
            QWidget *parent = nullptr,
            QString labelText = QString(),
            bool doubleEnterPassword = false);
    ~PasswordDialog();
    QString password();

private slots:
    bool checkIfPasswordsAreEqual();
    void on_passwordLineEdit_textChanged(const QString &arg1);
    void on_passwordLineEdit2_textChanged(const QString &arg1);

private:
    Ui::PasswordDialog *ui;
    bool _doubleEnterPassword;

};

#endif // PASSWORDDIALOG_H
