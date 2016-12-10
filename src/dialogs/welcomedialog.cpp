#include "QDebug"
#include <QSettings>
#include <QFileDialog>
#include <utils/misc.h>
#include "welcomedialog.h"
#include "ui_welcomedialog.h"
#include "settingsdialog.h"
#include "services/metricsservice.h"

WelcomeDialog::WelcomeDialog(QWidget *parent) :
        MasterDialog(parent),
    ui(new Ui::WelcomeDialog) {
    ui->setupUi(this);
    ui->finishButton->setEnabled(false);
    ui->backButton->setEnabled(false);
    ui->errorMessageLabel->setVisible(false);

    _notesPath = Utils::Misc::defaultNotesPath();
    ui->noteFolderLineEdit->setText(_notesPath);

    ui->stackedWidget->setCurrentIndex(WelcomePages::NoteFolderPage);
}

WelcomeDialog::~WelcomeDialog() {
    delete ui;
}

void WelcomeDialog::on_cancelButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled("welcome-dialog/cancel");

    done(QDialog::Rejected);
}

void WelcomeDialog::on_nextButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled("welcome-dialog/next");

    int index = ui->stackedWidget->currentIndex();
    int maxIndex = ui->stackedWidget->count() - 1;

    if (index == WelcomePages::NoteFolderPage) {
        // try to create the note folder, if it failed stay on page
        if (!handleNoteFolderSetup()) {
            return;
        }
    }

    if (index < maxIndex) {
        index++;
        ui->stackedWidget->setCurrentIndex(index);
    }

    if (index == WelcomePages::MetricsPage) {
        QSettings settings;
        settings.setValue("appMetrics/notificationShown", true);
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
                MetricsService::instance()->sendVisitIfEnabled(
                        "welcome-dialog/note-folder/cannot-create");
            }
        } else {
            showNoteFolderErrorMessage(tr("This note path doesn't exist!"));
            MetricsService::instance()->sendVisitIfEnabled(
                    "welcome-dialog/note-folder/not-exist");
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
    MetricsService::instance()
            ->sendVisitIfEnabled("welcome-dialog/note-folder/stored");

    QSettings settings;

    // make the path relative to the portable data path if we are in
    // portable mode
    settings.setValue("notesPath",
                      Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                              _notesPath));
}

void WelcomeDialog::on_backButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled("welcome-dialog/back");

    int index = ui->stackedWidget->currentIndex();

    if (index > 0) {
        index--;
        ui->stackedWidget->setCurrentIndex(index);
    }

    ui->nextButton->setEnabled(true);
    ui->backButton->setEnabled(index > 0);
}

void WelcomeDialog::on_finishButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled("welcome-dialog/finished");
    storeNoteFolderSettings();

    // create a note folder for the notes path
    NoteFolder::migrateToNoteFolders();

    done(QDialog::Accepted);
}

void WelcomeDialog::on_noteFolderButton_clicked() {
    MetricsService::instance()
            ->sendVisitIfEnabled("welcome-dialog/set-note-folder");

    QString dir = QFileDialog::getExistingDirectory(
            this,
            tr("Please select the folder where your notes will get stored to"),
            _notesPath,
            QFileDialog::ShowDirsOnly);

    QDir d = QDir(dir);

    if (d.exists() && (dir != "")) {
        _notesPath = dir;
        ui->noteFolderLineEdit->setText(dir);
    }
}

void WelcomeDialog::on_ownCloudSettingsButton_clicked() {
    MetricsService::instance()
            ->sendVisitIfEnabled("welcome-dialog/owncloud-settings");

    SettingsDialog *dialog = new SettingsDialog(
            SettingsDialog::OwnCloudPage, this);
    dialog->exec();
}

void WelcomeDialog::on_networkSettingsButton_clicked() {
    MetricsService::instance()
            ->sendVisitIfEnabled("welcome-dialog/network-settings");

    SettingsDialog *dialog = new SettingsDialog(
            SettingsDialog::NetworkPage, this);
    dialog->exec();
}

void WelcomeDialog::closeEvent(QCloseEvent *event) {
    MetricsService::instance()->sendVisitIfEnabled("welcome-dialog/close");
    MasterDialog::closeEvent(event);
}
