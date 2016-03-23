#include "welcomedialog.h"
#include "ui_welcomedialog.h"
#include "QDebug"
#include "settingsdialog.h"

WelcomeDialog::WelcomeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WelcomeDialog)
{
    ui->setupUi(this);
    ui->finishButton->setVisible(false);
    ui->backButton->setEnabled(false);
    ui->stackedWidget->setCurrentIndex(WelcomePages::NoteFolderPage);
}

WelcomeDialog::~WelcomeDialog()
{
    delete ui;
}

void WelcomeDialog::on_cancelButton_clicked()
{
    setResult(QDialog::Rejected);
    close();
}

void WelcomeDialog::on_nextButton_clicked()
{
    int index = ui->stackedWidget->currentIndex();
    int maxIndex = ui->stackedWidget->count() - 1;
    qDebug() << __func__ << " - 'index': " << index;
    qDebug() << __func__ << " - 'maxIndex': " << maxIndex;

    if (index < maxIndex) {
        index++;
        qDebug() << __func__ << " - 'index': " << index;
        ui->stackedWidget->setCurrentIndex(index);
    }

    ui->backButton->setEnabled(true);
    ui->nextButton->setEnabled(index < maxIndex);
}

void WelcomeDialog::on_backButton_clicked()
{
    int index = ui->stackedWidget->currentIndex();
    qDebug() << __func__ << " - 'index': " << index;

    if (index > 0) {
        index--;
        ui->stackedWidget->setCurrentIndex(index);
    }

    ui->nextButton->setEnabled(true);
    ui->backButton->setEnabled(index > 0);
}

void WelcomeDialog::on_finishButton_clicked()
{
    setResult(QDialog::Accepted);
    close();
}

void WelcomeDialog::on_noteFolderButton_clicked()
{

}

void WelcomeDialog::on_ownCloudSettingsButton_clicked()
{
    SettingsDialog *dialog = new SettingsDialog(
            SettingsDialog::OwnCloudTab, this);
    int dialogResult = dialog->exec();
}
