#include "updatedialog.h"
#include "ui_updatedialog.h"
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QPushButton>
#include <QDebug>
#include <QSettings>

UpdateDialog::UpdateDialog(QWidget *parent, QString changesText, QString releaseUrl, QString releaseVersionString, int releaseBuildNumber) :
    QDialog(parent),
    ui(new Ui::UpdateDialog)
{
    ui->setupUi(this);
    ui->changeLogEdit->setText( changesText );
    ui->versionLabel->setText( "Version " + releaseVersionString + " - build " + QString::number( releaseBuildNumber ) );
    this->releaseVersionString = releaseVersionString;
    this->releaseUrl = &releaseUrl;

    QPushButton *button;
    ui->buttonBox->clear();

    button = new QPushButton( tr( "&Download latest" ) );
    button->setProperty( "ActionRole", Download );
    button->setDefault( false );
    button->setIcon( QIcon( ":/images/breeze/edit-download.svg" ) );
    ui->buttonBox->addButton( button, QDialogButtonBox::ActionRole );

    button = new QPushButton( tr( "&Skip version" ) );
    button->setProperty( "ActionRole", Skip );
    button->setDefault( false );
    button->setIcon( QIcon( ":/images/breeze/media-skip-forward.svg" ) );
    ui->buttonBox->addButton( button, QDialogButtonBox::ActionRole );

    button = new QPushButton( tr( "&Cancel" ) );
    button->setProperty( "ActionRole", Cancel );
    button->setIcon( QIcon( ":/images/breeze/dialog-cancel.svg" ) );
    button->setDefault( true );
    ui->buttonBox->addButton( button, QDialogButtonBox::ActionRole );

    connect( this->ui->buttonBox, SIGNAL( clicked( QAbstractButton* ) ), SLOT( dialogButtonClicked( QAbstractButton* ) ) );

}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

void UpdateDialog::dialogButtonClicked( QAbstractButton *button )
{
    int actionRole = button->property("ActionRole").toInt();

    switch( actionRole )
    {
    case Download:
        // download the new release
        QDesktopServices::openUrl ( QUrl( this->releaseUrl->toUtf8() ) );
        // QMessageBox::information( 0, "Browser opened", "A browser window should habe been opened to download the new version!" );
        break;
    case Skip:
        QSettings settings;
        settings.setValue( "skipVersion", this->releaseVersionString );
        qDebug() << "skip version";
        break;
    }

    this->close();
}
