#include "owncloudservice.h"
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSettings>
#include <QDebug>

SettingsDialog::SettingsDialog(SimpleCrypt *crypto, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->passwordEdit->setEchoMode( QLineEdit::Password );
    this->crypto = crypto;
    readSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_connectButton_clicked()
{
    storeSettings();

    OwnCloudService *ownCloud = new OwnCloudService( crypto, this );
    ownCloud->connect();
}

void SettingsDialog::storeSettings()
{
    QSettings settings;
    settings.setValue( "ownCloud/serverUrl", ui->serverUrlEdit->text() );
    settings.setValue( "ownCloud/userName", ui->userNameEdit->text() );
    settings.setValue( "ownCloud/password", crypto->encryptToString( ui->passwordEdit->text() ) );
}

void SettingsDialog::readSettings()
{
    QSettings settings;
    ui->serverUrlEdit->setText( settings.value( "ownCloud/serverUrl" ).toString() );
    ui->userNameEdit->setText( settings.value( "ownCloud/userName" ).toString() );
    ui->passwordEdit->setText( crypto->decryptToString( settings.value( "ownCloud/password" ).toString() ) );
}

void SettingsDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if( button == ui->buttonBox->button( QDialogButtonBox::Ok ) )
    {
        storeSettings();
    }
}
