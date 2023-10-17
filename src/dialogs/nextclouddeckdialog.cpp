#include "nextclouddeckdialog.h"

#include <QTimeZone>

#include "mainwindow.h"
#include "services/nextclouddeckservice.h"
#include "ui_nextclouddeckdialog.h"

NextcloudDeckDialog::NextcloudDeckDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::NextcloudDeckDialog) {
    ui->setupUi(this);
    ui->dueDateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->saveButton->setEnabled(false);
    ui->dueDateTimeCheckBox->setChecked(true);
    ui->titleLineEdit->setFocus();
}

NextcloudDeckDialog::~NextcloudDeckDialog() { delete ui; }

void NextcloudDeckDialog::on_saveButton_clicked() {
    ui->saveButton->setEnabled(false);
    NextcloudDeckService nextcloudDeckService(this);

    auto *dateTime = new QDateTime(ui->dueDateTimeEdit->dateTime());
    dateTime->setTimeZone(QTimeZone::systemTimeZone());
    // We want to set the seconds to 0
    dateTime->setTime(dateTime->time().addSecs(0 - dateTime->time().second()));
    const QString &title = ui->titleLineEdit->text();
    int cardId =
        nextcloudDeckService.createCard(title, ui->descriptionTextEdit->toPlainText(),
                                        ui->dueDateTimeCheckBox->isChecked() ? dateTime : nullptr);

    if (cardId > 0) {
        auto linkText =
            QString("[%1](%2)").arg(title, nextcloudDeckService.getCardLinkForId(cardId));

#ifndef INTEGRATION_TESTS
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow != nullptr) {
            mainWindow->activeNoteTextEdit()->insertPlainText(linkText);
        }
#endif
    }

    close();
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

void NextcloudDeckDialog::on_titleLineEdit_textChanged(const QString &arg1) {
    ui->saveButton->setEnabled(!arg1.isEmpty());
}

void NextcloudDeckDialog::on_dueDateTimeCheckBox_toggled(bool checked) {
    ui->dueDateTimeEdit->setEnabled(checked);
}

void NextcloudDeckDialog::setTitle(const QString &title) { ui->titleLineEdit->setText(title); }
