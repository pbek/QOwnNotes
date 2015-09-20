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
    ui->connectionTestLabel->hide();
    this->crypto = crypto;
    readSettings();
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

/**
 * Starts a connection test
 *
 * @brief SettingsDialog::on_connectButton_clicked
 */
void SettingsDialog::on_connectButton_clicked()
{
    storeSettings();
    ui->connectionTestLabel->hide();
    OwnCloudService *ownCloud = new OwnCloudService( crypto, this );
    ownCloud->settingsConnectionTest( this );
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

/**
 * Callback function from OwnCloudService to output a success or error message
 *
 * @brief SettingsDialog::connectTestCallback
 * @param appIsValid
 * @param appVersion
 * @param serverVersion
 */
void SettingsDialog::connectTestCallback( bool appIsValid, QString appVersion, QString serverVersion )
{
    if ( appIsValid )
    {
        ui->connectionTestLabel->setStyleSheet( "color: green;" );
        ui->connectionTestLabel->setText( "The connection was made successfully!\nServer version: " + serverVersion + "\nQOwnNotesAPI version: " + appVersion );
    }
    else
    {
        ui->connectionTestLabel->setStyleSheet( "color: red;" );
        ui->connectionTestLabel->setText( "There was an error connecting to the ownCloud Server! You also need to have the QOwnNotesAPI app installed and enabled!" );
    }

    ui->connectionTestLabel->adjustSize();
    ui->connectionTestLabel->show();
}
