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
    enum OKLabelStatus {
        Unknown,
        Warning,
        OK,
        Failure,
    };

    explicit SettingsDialog(SimpleCrypt *crypto, QWidget *parent = 0);
    ~SettingsDialog();
    void connectTestCallback(bool appIsValid, QString appVersion, QString serverVersion, QString connectionErrorMessage);
    void setOKLabelData(int number, QString text, OKLabelStatus status);
private slots:
    void on_connectButton_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_localOwnCloudDirectoryButton_clicked();
    void on_ownCloudServerAppPageButton_clicked();

private:
    Ui::SettingsDialog *ui;
    SimpleCrypt *crypto;
    void readSettings();
    void setupCrypto();
    void storeSettings();
    void startConnectionTest();
};

#endif // SETTINGSDIALOG_H
