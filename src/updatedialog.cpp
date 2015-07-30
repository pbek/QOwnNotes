#include "updatedialog.h"
#include "ui_updatedialog.h"
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>

UpdateDialog::UpdateDialog(QWidget *parent, QString changesText, QString releaseUrl, QString releaseVersionString, int releaseBuildNumber) :
    QDialog(parent),
    ui(new Ui::UpdateDialog)
{
    ui->setupUi(this);
    ui->changeLogEdit->setText( changesText );
    ui->versionLabel->setText( "Version " + releaseVersionString + " - build " + QString::number( releaseBuildNumber ) );

    this->releaseUrl = &releaseUrl;

    connect(this->ui->buttonBox, SIGNAL(accepted()), this, SLOT(downloadRelease()));
}

UpdateDialog::~UpdateDialog()
{
    delete ui;
}

// download the new release
void UpdateDialog::downloadRelease()
{
    QDesktopServices::openUrl ( QUrl( this->releaseUrl->toUtf8() ) );
    QMessageBox::information( 0, "Browser opened", "A browser window should habe been opened to download the new version!" );
}
