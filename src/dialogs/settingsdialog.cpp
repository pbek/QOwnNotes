#include "services/owncloudservice.h"
#include "services/databaseservice.h"
#include "dialogs/settingsdialog.h"
#include "ui_settingsdialog.h"
#include <QSettings>
#include <QFileDialog>
#include <QDebug>
#include <QDesktopServices>
#include <QFontDialog>
#include <QMessageBox>

SettingsDialog::SettingsDialog(SimpleCrypt *crypto, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->connectionTestLabel->hide();
    ui->connectButton->setDefault( true );
    ui->noteSaveIntervalTime->setToolTip( ui->noteSaveIntervalTimeLabel->toolTip() );
    ui->tabWidget->setCurrentIndex( 0 );

    for ( int i = 0; i <= 8; i++ )
    {
        setOKLabelData( i, "unknown", SettingsDialog::Unknown );
    }

    QString html = ui->installInfotextBrowser->toHtml();

    // increase font size for OSX
#ifdef Q_OS_MAC
    QString styleSheet = ui->installInfotextBrowser->styleSheet();
    styleSheet.replace( "font-size: 9pt;", "font-size: 13pt;" );
    ui->installInfotextBrowser->setStyleSheet( styleSheet );

    html.replace( "font-size:9pt", "font-size:13pt;" );
#endif

    // add information text for Windows users to install OpenSSL
#ifdef Q_OS_WIN32
    html += "<p>If your ownCloud server uses SSL (https) you also need to install <a href='https://www.openssl.org'>OpenSSL</a> into your system directory.</p>";
    html += "<p>You can find binary packages of OpenSSL <a href='http://slproweb.com/products/Win32OpenSSL.html'>here</a>.</p>";
#endif

    html.replace( "QOWNNOTESAPI_MIN_VERSION", QOWNNOTESAPI_MIN_VERSION );
    ui->installInfotextBrowser->setHtml( html );

    this->crypto = crypto;
    readSettings();

    if ( ui->serverUrlEdit->text() != "" )
    {
        // start a connection test
        startConnectionTest();
    }
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

/**
 * @brief Starts a connection test
 */
void SettingsDialog::startConnectionTest()
{
    ui->connectionTestLabel->hide();
    OwnCloudService *ownCloud = new OwnCloudService( crypto, this );
    ownCloud->settingsConnectionTest( this );
}

/**
 * @brief SettingsDialog::on_connectButton_clicked
 */
void SettingsDialog::on_connectButton_clicked()
{
    storeSettings();

    // start a connection test
    startConnectionTest();
}

void SettingsDialog::storeSettings()
{
    QSettings settings;
    QString url = QString( ui->serverUrlEdit->text() );

    // remove trailing "/" of the server url
    if ( url.endsWith( "/" ) )
    {
        url.chop( 1 );
        ui->serverUrlEdit->setText( url );
    }

    settings.setValue( "ownCloud/serverUrl", url );
    settings.setValue( "ownCloud/userName", ui->userNameEdit->text() );
    settings.setValue( "ownCloud/password", crypto->encryptToString( ui->passwordEdit->text() ) );
    settings.setValue( "ownCloud/localOwnCloudPath", ui->localOwnCloudPathEdit->text() );
    settings.setValue( "notifyAllExternalModifications", ui->notifyAllExternalModificationsCheckBox->isChecked() );
    settings.setValue( "noteSaveIntervalTime", ui->noteSaveIntervalTime->value() );
    settings.setValue( "MainWindow/noteTextEdit.font", noteTextEditFont.toString() );
    settings.setValue( "MainWindow/noteTextView.font", noteTextViewFont.toString() );

    QStringList todoCalendarUrlList;
    QStringList todoCalendarEnabledList;
    QStringList todoCalendarEnabledUrlList;
    for ( int i = 0; i < ui->todoCalendarListWidget->count(); i++ )
    {
        QListWidgetItem * item = ui->todoCalendarListWidget->item( i );

        todoCalendarUrlList.append( item->toolTip() );

        if ( item->checkState() == Qt::Checked ) {
            todoCalendarEnabledList.append( item->text() );
            todoCalendarEnabledUrlList.append( item->toolTip() );
        }
    }

    // store the todo calendar data to the settings
    settings.setValue( "ownCloud/todoCalendarUrlList", todoCalendarUrlList );
    settings.setValue( "ownCloud/todoCalendarEnabledList", todoCalendarEnabledList );
    settings.setValue( "ownCloud/todoCalendarEnabledUrlList", todoCalendarEnabledUrlList );
    settings.setValue( "ownCloud/todoCalendarBackend", ui->calendarPlusRadioButton->isChecked() ? OwnCloudService::CalendarPlus : OwnCloudService::DefaultOwnCloudCalendar );
}

void SettingsDialog::readSettings()
{
    QSettings settings;
    ui->serverUrlEdit->setText( settings.value( "ownCloud/serverUrl" ).toString() );
    ui->userNameEdit->setText( settings.value( "ownCloud/userName" ).toString() );
    ui->passwordEdit->setText( crypto->decryptToString( settings.value( "ownCloud/password" ).toString() ) );
    ui->localOwnCloudPathEdit->setText( settings.value( "ownCloud/localOwnCloudPath" ).toString() );
    ui->notifyAllExternalModificationsCheckBox->setChecked( settings.value( "notifyAllExternalModifications" ).toBool() );
    ui->noteSaveIntervalTime->setValue( settings.value( "noteSaveIntervalTime" ).toInt() );

    noteTextEditFont.fromString( settings.value( "MainWindow/noteTextEdit.font" ).toString() );
    setFontLabel( ui->noteTextEditFontLabel, noteTextEditFont );

    noteTextViewFont.fromString( settings.value( "MainWindow/noteTextView.font" ).toString() );
    setFontLabel( ui->noteTextViewFontLabel, noteTextViewFont );

    const QSignalBlocker blocker( this->ui->defaultOwnCloudCalendarRadioButton );

    switch( settings.value( "ownCloud/todoCalendarBackend" ).toInt() )
    {
    case OwnCloudService::CalendarPlus:
        ui->calendarPlusRadioButton->setChecked( true );
        break;
    default:
        ui->defaultOwnCloudCalendarRadioButton->setChecked( true );
        break;
    }

    QStringList todoCalendarUrlList = settings.value( "ownCloud/todoCalendarUrlList" ).toStringList();
    // load the todo calendar list and set the checked state
    refreshTodoCalendarList( todoCalendarUrlList, true );
}

void SettingsDialog::setFontLabel( QLabel *label, QFont font )
{
    label->setText( font.family() + " (" + QString::number( font.pointSize() ) + ")" );
    label->setFont( font );
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
void SettingsDialog::connectTestCallback( bool appIsValid, QString appVersion, QString serverVersion, QString connectionErrorMessage )
{
    if ( appIsValid )
    {
        ui->connectionTestLabel->setStyleSheet( "color: green;" );
        ui->connectionTestLabel->setText( "The connection was made successfully!\nServer version: " + serverVersion + "\nQOwnNotesAPI version: " + appVersion );
    }
    else
    {
        // hide password
        connectionErrorMessage.replace( ui->passwordEdit->text(), "***" );

        ui->connectionTestLabel->setStyleSheet( "color: red;" );
        ui->connectionTestLabel->setText( "There was an error connecting to the ownCloud Server!\nYou also need to have the QOwnNotesAPI app installed and enabled!\n\nConnection error message: " + connectionErrorMessage );
    }

    ui->connectionTestLabel->adjustSize();
    ui->connectionTestLabel->show();
}

/**
 * @brief set text and color of an ok-label
 * @param number
 * @param text
 * @param color
 */
void SettingsDialog::setOKLabelData( int number, QString text, OKLabelStatus status )
{
    QLabel *label;

    switch( number )
    {
    case 1:
        label = ui->ok1Label;
        break;
    case 2:
        label = ui->ok2Label;
        break;
    case 3:
        label = ui->ok3Label;
        break;
    case 4:
        label = ui->ok4Label;
        break;
    case 5:
        label = ui->ok5Label;
        break;
    case 6:
        label = ui->ok6Label;
        break;
    case 7:
        label = ui->ok7Label;
        break;
    case 8:
        label = ui->ok8Label;
        break;
    default:
        return;
    }

    QString color;
    switch( status )
    {
    case Unknown:
        color = "gray";
        break;
    case OK:
        color = "green";
        break;
    case Warning:
        color = "orange";
        break;
    case Failure:
        color = "red";
        break;
    default:
        color = "white";
    }

    label->setText( text );
    label->setStyleSheet( "color: " + color );
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
                                                 QFileDialog::ShowDirsOnly );

    if ( dir != "" )
    {
        ui->localOwnCloudPathEdit->setText( dir );
    }
}

void SettingsDialog::on_ownCloudServerAppPageButton_clicked()
{
    QDesktopServices::openUrl( QUrl( ui->serverUrlEdit->text() + "/index.php/settings/apps" ) );
}

void SettingsDialog::on_noteTextEditButton_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont( &ok, noteTextEditFont, this);
    if ( ok )
    {
        noteTextEditFont = font;
        setFontLabel( ui->noteTextEditFontLabel, noteTextEditFont );
    }
}

void SettingsDialog::on_noteTextViewButton_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont( &ok, noteTextViewFont, this);
    if ( ok )
    {
        noteTextViewFont = font;
        setFontLabel( ui->noteTextViewFontLabel, noteTextViewFont );
    }
}

void SettingsDialog::on_reloadCalendarListButton_clicked()
{
    // we need to store the calendar backend
    storeSettings();

    OwnCloudService *ownCloud = new OwnCloudService( crypto, this );
    ownCloud->settingsGetCalendarList( this );
}

void SettingsDialog::refreshTodoCalendarList( QStringList items, bool forceReadCheckedState )
{
    // we want to read the checked state from the settings if the todo calendar list was not emptry
    bool readCheckedState = forceReadCheckedState ? true : ui->todoCalendarListWidget->count() > 0;

    // clear the todo calendar list
    ui->todoCalendarListWidget->clear();

    QSettings settings;
    QStringList todoCalendarEnabledList = settings.value( "ownCloud/todoCalendarEnabledList" ).toStringList();

    QListIterator<QString> itr ( items );
    while ( itr.hasNext() )
    {
        QString url = itr.next();

        // only add the server url if it wasn't already added
        if ( !url.startsWith( ui->serverUrlEdit->text() ) )
        {
            url = ui->serverUrlEdit->text() + url;
        }

        // get the name out of the url part
        QRegularExpression regex( "\\/([^\\/]*)\\/$" );
        QRegularExpressionMatch match = regex.match( url );
        QString name = match.captured(1);

        // remove percent encoding
        name = QUrl::fromPercentEncoding( name.toLatin1() );

        // skip the contact birthdays calendar
        if ( name == "contact_birthdays" ) {
            continue;
        }

        // skip the Calendar Plus birthday calendar
        if ( name.startsWith( "bdaycpltocal_" ) ) {
            continue;
        }

        // create the list widget item and add it to the todo calendar list widget
        QListWidgetItem *item = new QListWidgetItem( name );

        // eventually check if item was checked
        Qt::CheckState checkedState = readCheckedState ? ( todoCalendarEnabledList.contains( name ) ? Qt::Checked : Qt::Unchecked ) : Qt::Checked;
        item->setCheckState( checkedState );

        item->setFlags( Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable );
        item->setToolTip( url );
        ui->todoCalendarListWidget->addItem( item );
    }
}

void SettingsDialog::on_defaultOwnCloudCalendarRadioButton_toggled(bool checked)
{
    on_reloadCalendarListButton_clicked();
}

void SettingsDialog::on_reinitializeDatabaseButton_clicked()
{
    DatabaseService::reinitializeDiskDatabase();
    QMessageBox::information( this, "Database", "The Database was reinitialized." );
}

/**
 * @brief Clears the recent note folder history in the settings
 */
void SettingsDialog::on_clearRecentNotesFoldersHistoryButton_clicked()
{
    QSettings settings;
    settings.remove( "recentNoteFolders" );

    QMessageBox::information( this, "Recent note folders history", "The history was cleared." );
}
