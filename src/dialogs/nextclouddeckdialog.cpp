#include "nextclouddeckdialog.h"
#include "ui_nextclouddeckdialog.h"

NextcloudDeckDialog::NextcloudDeckDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NextcloudDeckDialog)
{
    ui->setupUi(this);
}

NextcloudDeckDialog::~NextcloudDeckDialog()
{
    delete ui;
}

void NextcloudDeckDialog::on_saveButton_clicked()
{

}

