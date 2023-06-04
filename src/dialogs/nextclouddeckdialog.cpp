#include "nextclouddeckdialog.h"

#include "services/nextclouddeckservice.h"
#include "ui_nextclouddeckdialog.h"

NextcloudDeckDialog::NextcloudDeckDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NextcloudDeckDialog) {
    ui->setupUi(this);
    ui->dueDateTimeEdit->setDateTime(QDateTime::currentDateTime());
}

NextcloudDeckDialog::~NextcloudDeckDialog() {
    delete ui;
}

void NextcloudDeckDialog::on_saveButton_clicked() {
    NextcloudDeckService nextcloudDeckService(CloudConnection::currentCloudConnection().getId(), this);

    auto *dateTime = new QDateTime(ui->dueDateTimeEdit->dateTime());
    nextcloudDeckService.createCard(ui->titleLineEdit->text(),
                                    "",
                                    dateTime);
}

void NextcloudDeckDialog::on_add1HourButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addSecs(3600));
}

void NextcloudDeckDialog::on_add10MinButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addSecs(600));
}

void NextcloudDeckDialog::on_add1DayButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addDays(1));
}

void NextcloudDeckDialog::on_sub10MinButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addSecs(-600));
}

void NextcloudDeckDialog::on_sub1HourButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addSecs(-3600));
}

void NextcloudDeckDialog::on_subd1DayButton_clicked() {
    ui->dueDateTimeEdit->setDateTime(ui->dueDateTimeEdit->dateTime().addDays(-1));
}
