#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include "libraries/simplecrypt/simplecrypt.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0, SimpleCrypt crypto = SimpleCrypt());
    ~SettingsDialog();

private slots:
    void on_connectButton_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::SettingsDialog *ui;
    SimpleCrypt crypto;
    void readSettings();
    void setupCrypto();
    void storeSettings();
};

#endif // SETTINGSDIALOG_H
