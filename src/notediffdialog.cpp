#include "notediffdialog.h"
#include "ui_notediffdialog.h"

NoteDiffDialog::NoteDiffDialog(QWidget *parent, QString html) :
    QDialog(parent),
    ui(new Ui::NoteDiffDialog)
{
    ui->setupUi(this);

    this->ui->textEdit->setHtml( html );
}

NoteDiffDialog::~NoteDiffDialog()
{
    delete ui;
}
