#include "owncloudservice.h"
#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSettings>
#include <QFileDialog>
#include <QDebug>

SettingsDialog::SettingsDialog(SimpleCrypt *crypto, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->passwordEdit->setEchoMode( QLineEdit::Password );
    ui->connectionTestLabel->hide();

#ifdef Q_OS_MAC
    QString styleSheet = ui->installInfotextBrowser->styleSheet();
    styleSheet = styleSheet.replace( "font-size: 9pt;", "font-size: 13pt;" );
    ui->installInfotextBrowser->setStyleSheet( styleSheet );

    QString html = ui->installInfotextBrowser->toHtml();
    html = html.replace( "font-size:9pt", "font-size:13pt;" );
    ui->installInfotextBrowser->setHtml(  html );
#endif

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
    settings.setValue( "ownCloud/localOwnCloudPath", ui->localOwnCloudPathEdit->text() );
}

void SettingsDialog::readSettings()
{
    QSettings settings;
    ui->serverUrlEdit->setText( settings.value( "ownCloud/serverUrl" ).toString() );
    ui->userNameEdit->setText( settings.value( "ownCloud/userName" ).toString() );
    ui->passwordEdit->setText( crypto->decryptToString( settings.value( "ownCloud/password" ).toString() ) );
    ui->localOwnCloudPathEdit->setText( settings.value( "ownCloud/localOwnCloudPath" ).toString() );
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

/**
 * select the local ownCloud directory
 */
void SettingsDialog::on_localOwnCloudDirectoryButton_clicked()
{
    QString path = ui->localOwnCloudPathEdit->text();

    if ( path == "" )
    {
        path = QDir::homePath() + QDir::separator() + "ownCloud";
    }

    QString dir = QFileDialog::getExistingDirectory( this, tr( "Select ownCloud base directory" ),
                                                 path,
                                                 QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog );

    ui->localOwnCloudPathEdit->setText( dir );
}
