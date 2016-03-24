#include "QDebug"
#include <QSettings>
#include <QFileDialog>
#include "welcomedialog.h"
#include "ui_welcomedialog.h"
#include "settingsdialog.h"

WelcomeDialog::WelcomeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WelcomeDialog)
{
    ui->setupUi(this);
    ui->finishButton->setEnabled(false);
    ui->backButton->setEnabled(false);
    ui->errorMessageLabel->setVisible(false);

    _notesPath = QDir::homePath() + QDir::separator() +
           "ownCloud" + QDir::separator() + "Notes";
    ui->noteFolderLabel->setText(_notesPath);

    ui->stackedWidget->setCurrentIndex(WelcomePages::NoteFolderPage);
}

WelcomeDialog::~WelcomeDialog()
{
    delete ui;
}

void WelcomeDialog::on_cancelButton_clicked()
{
    done(QDialog::Rejected);
}

void WelcomeDialog::on_nextButton_clicked()
{
    int index = ui->stackedWidget->currentIndex();
    int maxIndex = ui->stackedWidget->count() - 1;

    if (index == WelcomePages::NoteFolderPage) {
        // try to create the note folder, if it failed stay on page
        if (!handleNoteFolderSetup()) {
            return;
        };
    }

    if (index < maxIndex) {
        index++;
        ui->stackedWidget->setCurrentIndex(index);
    }

    ui->finishButton->setEnabled(_allowFinishButton);
    ui->backButton->setEnabled(true);
    ui->nextButton->setEnabled(index < maxIndex);
}

/**
 * Tries to create the note folder and store it in the settings
 */
bool WelcomeDialog::handleNoteFolderSetup() {
    ui->errorMessageLabel->setVisible(false);
    _allowFinishButton = false;
    QDir dir(_notesPath);
    if (dir.exists()) {
        // everything is all right, the path already exists
        _allowFinishButton = true;
    } else {
        if (ui->createNoteFolderCheckBox->isChecked()) {
            if (dir.mkpath(_notesPath)) {
                // everything is all right, the path was now created
                _allowFinishButton = true;
            } else {
                showNoteFolderErrorMessage(tr("Cannot create note path!"));
            }
        } else {
            showNoteFolderErrorMessage(tr("This note path doesn't exist!"));
        }
    }

    if (_allowFinishButton) {
        storeNoteFolderSettings();
    }

    return _allowFinishButton;
}

/**
 * Shows an error message on the note path page
 */
void WelcomeDialog::showNoteFolderErrorMessage(QString message) {
    ui->errorMessageLabel->setText(message);
    ui->errorMessageLabel->setVisible(true);
}

/**
 * Stores the note path in the settings
 */
void WelcomeDialog::storeNoteFolderSettings() {
    QSettings settings;
    settings.setValue("notesPath", _notesPath);
}

void WelcomeDialog::on_backButton_clicked()
{
    int index = ui->stackedWidget->currentIndex();

    if (index > 0) {
        index--;
        ui->stackedWidget->setCurrentIndex(index);
    }

    ui->nextButton->setEnabled(true);
    ui->backButton->setEnabled(index > 0);
}

void WelcomeDialog::on_finishButton_clicked()
{
    done(QDialog::Accepted);
}

void WelcomeDialog::on_noteFolderButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
            this,
            tr("Please select the folder where your notes will get stored to"),
            _notesPath,
            QFileDialog::ShowDirsOnly);

    QDir d = QDir(dir);

    if (d.exists() && (dir != "")) {
        _notesPath = dir;
        ui->noteFolderLabel->setText(dir);
    }
}

void WelcomeDialog::on_ownCloudSettingsButton_clicked()
{
    SettingsDialog *dialog = new SettingsDialog(
            SettingsDialog::OwnCloudTab, this);
    dialog->exec();
}

void WelcomeDialog::on_generalSettingsButton_clicked()
{
    SettingsDialog *dialog = new SettingsDialog(
            SettingsDialog::GeneralTab, this);
    dialog->exec();
}
