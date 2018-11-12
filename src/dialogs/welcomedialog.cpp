#include "QDebug"
#include <QSettings>
#include <QFileDialog>
#include <QGraphicsScene>
#include <utils/misc.h>
#include "welcomedialog.h"
#include "ui_welcomedialog.h"
#include "settingsdialog.h"
#include "services/metricsservice.h"

WelcomeDialog::WelcomeDialog(QWidget *parent) :
        MasterDialog(parent),
    ui(new Ui::WelcomeDialog) {
    ui->setupUi(this);

    // replace ownCloud text
    ui->subHeadlineLabel->setText(Utils::Misc::replaceOwnCloudText(
            ui->subHeadlineLabel->text()));
    ui->groupBox_2->setTitle(Utils::Misc::replaceOwnCloudText(
            ui->groupBox_2->title()));
    ui->label->setText(Utils::Misc::replaceOwnCloudText(ui->label->text()));
    ui->label_4->setText(Utils::Misc::replaceOwnCloudText(ui->label_4->text()));
    ui->ownCloudSettingsButton->setText(Utils::Misc::replaceOwnCloudText(
            ui->ownCloudSettingsButton->text()));

    ui->finishButton->setEnabled(false);
    ui->backButton->setEnabled(false);
    ui->errorMessageLabel->setVisible(false);

    _notesPath = Utils::Misc::defaultNotesPath();
    ui->noteFolderLineEdit->setText(_notesPath);

    ui->stackedWidget->setCurrentIndex(WelcomePages::NoteFolderPage);

    loadLayouts();
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

    if (index == WelcomePages::LayoutPage) {
        resizeLayoutImage();
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

        Utils::Misc::printInfo(QString("Note path '%1' exists.").arg(
                _notesPath));
    } else {
        if (ui->createNoteFolderCheckBox->isChecked()) {
            Utils::Misc::printInfo(
                    QString("Note path '%1' doesn't exist yet and will "
                            "be created.").arg(_notesPath));

            // mkpath should only return true if the path was created, but we
            // want to double-check because there were some troubles on Windows
            // see: https://github.com/pbek/QOwnNotes/issues/951
            if (dir.mkpath(_notesPath)) {
                if (dir.exists()) {
                    // everything is all right, the path was now created
                    _allowFinishButton = true;

                    Utils::Misc::printInfo(
                            QString("Note path '%1' was now created.").arg(
                                    _notesPath));
                } else {
                    qWarning() << "Cannot create note path with mkpath!";
                    showNoteFolderErrorMessage(
                            tr("Cannot create note path! You have to create "
                               "the note folder manually!"));
                    MetricsService::instance()->sendVisitIfEnabled(
                            "welcome-dialog/note-folder/cannot-create-mkpath");
                }
            } else {
                qWarning() << "Cannot create note path!";
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
#ifdef Q_OS_WIN32
        // check if user chose a different drive in portable mode
        if (Utils::Misc::isInPortableMode() &&
                Utils::Misc::portableDataPath().toLower().at(0) !=
                dir.toLower().at(0)) {
            QMessageBox::information(
                    this,
                    tr("Note folder"),
                    tr("Keep in mind that the note folder will be "
                       "stored relative to the directory where QOwnNotes "
                       "resides in portable mode! So you need to stay on the "
                       "same drive."));

            return;
        }
#endif

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

void WelcomeDialog::loadLayouts() {
    _layoutSettings = new QSettings(":/configurations/layouts.ini",
            QSettings::IniFormat);
    auto layoutIdentifiers = _layoutSettings->value("LayoutIdentifiers").toStringList();

    {
        const QSignalBlocker blocker(ui->layoutComboBox);
        Q_UNUSED(blocker);
        ui->layoutComboBox->clear();

        for (int i = 0; i < layoutIdentifiers.count(); i++) {
            const QString &layoutIdentifier = layoutIdentifiers.at(i);
            const QString &layoutName = _layoutSettings->value(
                                "Layout-" + layoutIdentifier + "/name").toString();
            ui->layoutComboBox->addItem(layoutName, layoutIdentifier);
        }
    }

    ui->layoutComboBox->setCurrentIndex(0);
    on_layoutComboBox_currentIndexChanged(0);
}

void WelcomeDialog::on_layoutComboBox_currentIndexChanged(int index) {
    Q_UNUSED(index);
    updateCurrentLayout();
}

void WelcomeDialog::updateCurrentLayout() const {
    QString layoutIdentifier = ui->layoutComboBox->currentData().toString();
    QString layoutSettingsPrefix = "Layout-" + layoutIdentifier + "/";
    QString screenshot = _layoutSettings->value(layoutSettingsPrefix +
            "screenshot").toString();

    auto scene = new QGraphicsScene();
    QString filePath(":/images/layouts/" + screenshot);

    scene->addPixmap(QPixmap(filePath));
    ui->layoutGraphicsView->setScene(scene);
    ui->layoutGraphicsView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    QSettings settings;
    auto workspaces = QStringList() << "initial";
    settings.setValue("workspaces", workspaces);
    settings.setValue("currentWorkspace", "initial");
    settings.setValue("noteEditIsCentralWidget", _layoutSettings->value(
            layoutSettingsPrefix + "noteEditIsCentralWidget"));
    settings.setValue("workspace-initial/windowState", _layoutSettings->value(
            layoutSettingsPrefix + "windowState"));
    settings.setValue("workspace-initial/name", _layoutSettings->value(
            layoutSettingsPrefix + "name"));
    settings.setValue("workspace-initial/noteSubFolderDockWidgetVisible",
            _layoutSettings->value(layoutSettingsPrefix +
            "noteSubFolderDockWidgetVisible"));
}

void WelcomeDialog::resizeEvent(QResizeEvent* event) {
    resizeLayoutImage();
    MasterDialog::resizeEvent(event);
}

void WelcomeDialog::resizeLayoutImage() const {
    if (ui->layoutGraphicsView->scene() != nullptr) {
        ui->layoutGraphicsView->fitInView(
                ui->layoutGraphicsView->scene()->sceneRect(),
                Qt::KeepAspectRatio);
    }
}
