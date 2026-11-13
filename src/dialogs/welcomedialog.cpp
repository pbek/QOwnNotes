#include "welcomedialog.h"

#include <utils/misc.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QGraphicsScene>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QVBoxLayout>

#include "QDebug"
#include "entities/note.h"
#include "entities/notefolder.h"
#include "services/metricsservice.h"
#include "services/settingsservice.h"
#include "settingsdialog.h"
#include "ui_welcomedialog.h"
#include "utils/notefolderdetection.h"

namespace {
constexpr int NoteFolderCandidatePathRole = Qt::UserRole;
constexpr int NoteFolderCandidateHasNotesInSubFoldersRole = Qt::UserRole + 1;

// The candidate list shouldn't make the note folder page grow too much
constexpr int MaxVisibleNoteFolderCandidates = 3;

// Marks the labels of a candidate row that show secondary information
constexpr char NoteFolderCandidateMutedProperty[] = "noteFolderCandidateMuted";

QString canonicalPathIfExists(const QString &path) {
    const QString canonicalPath = QFileInfo(path).canonicalFilePath();
    return canonicalPath.isEmpty() ? QDir::cleanPath(path) : canonicalPath;
}
}    // namespace

WelcomeDialog::WelcomeDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::WelcomeDialog) {
    ui->setupUi(this);
    afterSetupUI();
    ui->layoutPresetWidget->setManualSettingsStoring(false);

    // if note layout has already been set, we can finish settings in the first
    // step
    SettingsService settings;
    _allowFinishButton = settings.contains(QStringLiteral("layout-initial/windowState"));
    ui->finishButton->setEnabled(_allowFinishButton);

    ui->backButton->setEnabled(false);
    ui->errorMessageLabel->setVisible(false);

    _notesPath = Utils::Misc::defaultNotesPath();
    ui->noteFolderLineEdit->setText(_notesPath);
    initNoteFolderCandidates();

    ui->stackedWidget->setCurrentIndex(WelcomePages::NoteFolderPage);

    QFont stepFont = ui->stepLabel->font();
    stepFont.setBold(true);
    ui->stepLabel->setFont(stepFont);
    updateStepInfo();
}

WelcomeDialog::~WelcomeDialog() { delete ui; }

/**
 * Shows on which step of the welcome dialog the user is and makes the
 * button for the next step the default button
 */
void WelcomeDialog::updateStepInfo() {
    const int index = ui->stackedWidget->currentIndex();
    const int maxIndex = ui->stackedWidget->count() - 1;
    QString pageName;

    switch (index) {
        case NoteFolderPage:
            pageName = tr("Note folder");
            break;
        case LayoutPresetPage:
            pageName = tr("Layout");
            break;
        case CloudPage:
            pageName = tr("Nextcloud / ownCloud");
            break;
        case MetricsPage:
            pageName = tr("Metrics");
            break;
        default:
            break;
    }

    ui->stepLabel->setText(tr("Step %1 of %2: %3").arg(index + 1).arg(maxIndex + 1).arg(pageName));

    // Pressing return should open the next step, or finish the dialog on the last step
    ui->nextButton->setDefault(index < maxIndex);
    ui->finishButton->setDefault(index >= maxIndex);
}

void WelcomeDialog::on_cancelButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("welcome-dialog/cancel"));

    done(QDialog::Rejected);
}

void WelcomeDialog::on_nextButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("welcome-dialog/next"));

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
        SettingsService settings;
        settings.setValue(QStringLiteral("appMetrics/notificationShown"), true);
    }

    if (index == WelcomePages::LayoutPresetPage) {
        ui->layoutPresetWidget->resizeLayoutPresetImage();
    }

    ui->finishButton->setEnabled(_allowFinishButton);
    ui->backButton->setEnabled(true);
    ui->nextButton->setEnabled(index < maxIndex);
    updateStepInfo();
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

        Utils::Misc::printInfo(QStringLiteral("Note path '%1' exists.").arg(_notesPath));
    } else {
        if (ui->createNoteFolderCheckBox->isChecked()) {
            // Don't create a separate note folder if there already are notes somewhere else,
            // unless the user confirms it
            if (!confirmNewNoteFolderCreation()) {
                return false;
            }

            Utils::Misc::printInfo(QStringLiteral("Note path '%1' doesn't exist yet and will "
                                                  "be created.")
                                       .arg(_notesPath));

            // mkpath should only return true if the path was created, but we
            // want to double-check because there were some troubles on Windows
            // see: https://github.com/pbek/QOwnNotes/issues/951
            if (dir.mkpath(_notesPath)) {
                if (dir.exists()) {
                    // everything is all right, the path was now created
                    _allowFinishButton = true;

                    Utils::Misc::printInfo(
                        QStringLiteral("Note path '%1' was now created.").arg(_notesPath));
                } else {
                    qWarning() << "Cannot create note path with mkpath!";
                    showNoteFolderErrorMessage(
                        tr("Cannot create note path! You have to create "
                           "the note folder manually!"));
                    MetricsService::instance()->sendVisitIfEnabled(
                        QStringLiteral("welcome-dialog/note-folder/cannot-create-mkpath"));
                }
            } else {
                qWarning() << "Cannot create note path!";
                showNoteFolderErrorMessage(tr("Cannot create note path!"));
                MetricsService::instance()->sendVisitIfEnabled(
                    QStringLiteral("welcome-dialog/note-folder/cannot-create"));
            }
        } else {
            showNoteFolderErrorMessage(tr("This note path doesn't exist!"));
            MetricsService::instance()->sendVisitIfEnabled(
                QStringLiteral("welcome-dialog/note-folder/not-exist"));
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
void WelcomeDialog::showNoteFolderErrorMessage(const QString &message) {
    ui->errorMessageLabel->setText(message);
    ui->errorMessageLabel->setVisible(true);
}

/**
 * Stores the note path in the settings
 */
void WelcomeDialog::storeNoteFolderSettings() {
    MetricsService::instance()->sendVisitIfEnabled(
        QStringLiteral("welcome-dialog/note-folder/stored"));

    SettingsService settings;

    // make the path relative to the portable data path if we are in
    // portable mode
    settings.setValue(QStringLiteral("notesPath"),
                      Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(_notesPath));
    settings.setValue(QStringLiteral("showNoteSubFolders"),
                      ui->showNoteSubFoldersCheckBox->isChecked());
}

void WelcomeDialog::on_backButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("welcome-dialog/back"));

    int index = ui->stackedWidget->currentIndex();

    if (index > 0) {
        index--;
        ui->stackedWidget->setCurrentIndex(index);
    }

    ui->nextButton->setEnabled(true);
    ui->backButton->setEnabled(index > 0);
    updateStepInfo();
}

void WelcomeDialog::on_finishButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("welcome-dialog/finished"));
    if (ui->stackedWidget->currentIndex() == WelcomePages::NoteFolderPage) {
        if (!handleNoteFolderSetup()) return;
    } else {
        ui->layoutPresetWidget->storeLayoutPreset();
    }
    storeNoteFolderSettings();
    done(QDialog::Accepted);
}

void WelcomeDialog::on_noteFolderButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled(
        QStringLiteral("welcome-dialog/set-note-folder"));

    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Please select the folder where your notes will get stored to"), _notesPath,
        QFileDialog::ShowDirsOnly);

    QDir d = QDir(dir);

    if (d.exists() && (!dir.isEmpty())) {
#ifdef Q_OS_WIN32
        // check if user chose a different drive in portable mode
        if (Utils::Misc::isInPortableMode() &&
            Utils::Misc::portableDataPath().toLower().at(0) != dir.toLower().at(0)) {
            QMessageBox::information(this, tr("Note folder"),
                                     tr("Keep in mind that the note folder will be "
                                        "stored relative to the directory where QOwnNotes "
                                        "resides in portable mode! So you need to stay on the "
                                        "same drive."));

            return;
        }
#endif

        setNotesPath(dir);
    }
}

/**
 * Searches for local folders that already seem to contain notes, like a localized
 * Nextcloud Notes folder in the Nextcloud sync folder, and lists them on the note folder page
 *
 * There usually is no Nextcloud connection yet, so only local folders are searched.
 * No folder is selected automatically, the user has to choose one.
 */
void WelcomeDialog::initNoteFolderCandidates() {
    ui->noteFolderCandidatesFrame->setVisible(false);

    QStringList noteFileExtensions = Utils::NoteFolderDetection::defaultNoteFileExtensions();
    noteFileExtensions << Note::noteFileExtensionList();
    noteFileExtensions.removeDuplicates();

    // Folders that are already configured as note folders don't need to be suggested
    QStringList excludedPaths;
    const auto noteFolders = NoteFolder::fetchAll();
    for (const NoteFolder &noteFolder : noteFolders) {
        excludedPaths << noteFolder.getLocalPath();
    }

    const auto candidates =
        Utils::NoteFolderDetection::findDefaultCandidates(noteFileExtensions, excludedPaths);

    if (candidates.isEmpty()) {
        return;
    }

    MetricsService::instance()->sendVisitIfEnabled(
        QStringLiteral("welcome-dialog/note-folder/candidates-found"));

    const QLocale locale;
    for (const auto &candidate : candidates) {
        QStringList details;

        if (candidate.noteCountCapped) {
            details << tr("%1+ notes").arg(locale.toString(candidate.noteCount));
        } else if (candidate.noteCount > 0) {
            // English has no plural forms loaded, so avoid showing "note(s)"
            details << (candidate.noteCount == 1 ? tr("1 note")
                                                 : tr("%n notes", "", candidate.noteCount));
        }

        if (candidate.hasNotesInSubFolders) {
            details << tr("with subfolders");
        }

        if (candidate.hasNoteFolderDatabase) {
            details << tr("used by QOwnNotes");
        }

        auto *item = new QListWidgetItem(ui->noteFolderCandidatesListWidget);
        item->setToolTip(QDir::toNativeSeparators(candidate.path));
        item->setData(NoteFolderCandidatePathRole, candidate.path);
        item->setData(NoteFolderCandidateHasNotesInSubFoldersRole, candidate.hasNotesInSubFolders);

        QWidget *widget = createNoteFolderCandidateWidget(candidate.path,
                                                          details.join(QStringLiteral(" \u00b7 ")),
                                                          candidate.hasNoteFolderDatabase);
        item->setSizeHint(widget->sizeHint());
        ui->noteFolderCandidatesListWidget->setItemWidget(item, widget);
    }

    // Show a few candidates at once, more can be scrolled to
    auto *list = ui->noteFolderCandidatesListWidget;
    int listHeight = 2 * list->frameWidth();
    for (int i = 0; i < qMin(list->count(), MaxVisibleNoteFolderCandidates); i++) {
        listHeight += list->item(i)->sizeHint().height() + 2 * list->spacing();
    }
    list->setFixedHeight(listHeight);

    ui->noteFolderCandidatesFrame->setVisible(true);
    selectNoteFolderCandidate(_notesPath);

    // A dialog size stored before the candidates existed may be too small for them now
    layout()->activate();
    resize(size().expandedTo(minimumSizeHint()));
}

/**
 * The list row of a note folder candidate: the folder name, what it contains and where it is
 */
QWidget *WelcomeDialog::createNoteFolderCandidateWidget(const QString &path, const QString &details,
                                                        bool isQOwnNotesFolder) {
    auto *widget = new QWidget(ui->noteFolderCandidatesListWidget);
    // Let clicks reach the list, so the row gets selected
    widget->setAttribute(Qt::WA_TransparentForMouseEvents);

    auto *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(8, 6, 8, 6);
    layout->setSpacing(10);

    const QIcon icon =
        isQOwnNotesFolder
            ? QIcon(QStringLiteral(":/images/icon.png"))
            : QIcon::fromTheme(QStringLiteral("folder"),
                               QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/folder.svg")));
    auto *iconLabel = new QLabel(widget);
    iconLabel->setPixmap(icon.pixmap(32, 32));
    iconLabel->setFixedSize(32, 32);
    layout->addWidget(iconLabel, 0, Qt::AlignVCenter);

    auto *textLayout = new QVBoxLayout();
    textLayout->setSpacing(1);

    auto *titleLayout = new QHBoxLayout();
    titleLayout->setSpacing(8);
    auto *nameLabel = new QLabel(QFileInfo(path).fileName(), widget);
    nameLabel->setTextFormat(Qt::PlainText);
    QFont nameFont = nameLabel->font();
    nameFont.setBold(true);
    nameLabel->setFont(nameFont);
    titleLayout->addWidget(nameLabel);

    auto *detailsLabel = new QLabel(details, widget);
    detailsLabel->setTextFormat(Qt::PlainText);
    detailsLabel->setProperty(NoteFolderCandidateMutedProperty, true);
    titleLayout->addWidget(detailsLabel, 1);
    textLayout->addLayout(titleLayout);

    // The path below the name, relative to the home folder where possible to keep it short
    QString displayPath = QDir::toNativeSeparators(path);
    const QString home = QDir::toNativeSeparators(QDir::homePath());
    if (displayPath.startsWith(home + QDir::separator())) {
        displayPath = QStringLiteral("~") + displayPath.mid(home.length());
    }
    auto *pathLabel = new QLabel(displayPath, widget);
    pathLabel->setTextFormat(Qt::PlainText);
    QFont pathFont = pathLabel->font();
    pathFont.setPointSizeF(pathFont.pointSizeF() * 0.9);
    pathLabel->setFont(pathFont);
    pathLabel->setProperty(NoteFolderCandidateMutedProperty, true);
    textLayout->addWidget(pathLabel);

    layout->addLayout(textLayout, 1);

    return widget;
}

/**
 * Colors the labels of the candidate rows, so they stay readable on the selection color
 */
void WelcomeDialog::updateNoteFolderCandidateColors() {
    auto *list = ui->noteFolderCandidatesListWidget;
    for (int i = 0; i < list->count(); i++) {
        QListWidgetItem *item = list->item(i);
        QWidget *widget = list->itemWidget(item);
        if (widget == nullptr) {
            continue;
        }

        QPalette rowPalette = list->palette();
        const QColor text =
            rowPalette.color(item->isSelected() ? QPalette::HighlightedText : QPalette::Text);
        const QColor background =
            rowPalette.color(item->isSelected() ? QPalette::Highlight : QPalette::Base);
        const QColor muted = item->isSelected() ? text
                                                : QColor((text.red() + background.red()) / 2,
                                                         (text.green() + background.green()) / 2,
                                                         (text.blue() + background.blue()) / 2);

        // A style sheet, because the application style sheet would override a palette
        const auto labels = widget->findChildren<QLabel *>();
        for (QLabel *label : labels) {
            const QColor color =
                label->property(NoteFolderCandidateMutedProperty).toBool() ? muted : text;
            label->setStyleSheet(QStringLiteral("color: %1;").arg(color.name()));
        }
    }
}

/**
 * Sets the note folder path and updates the note folder page
 */
void WelcomeDialog::setNotesPath(const QString &path) {
    _notesPath = path;
    ui->noteFolderLineEdit->setText(path);
    ui->errorMessageLabel->setVisible(false);
    selectNoteFolderCandidate(path);
}

/**
 * Selects the candidate of a path in the list, or nothing if the path isn't a candidate
 */
void WelcomeDialog::selectNoteFolderCandidate(const QString &path) {
    auto *list = ui->noteFolderCandidatesListWidget;
    const QSignalBlocker blocker(list);
    const QString canonicalPath = canonicalPathIfExists(path);

    list->clearSelection();
    list->setCurrentItem(nullptr);
    for (int i = 0; i < list->count(); i++) {
        QListWidgetItem *item = list->item(i);
        if (item->data(NoteFolderCandidatePathRole).toString() == canonicalPath) {
            list->setCurrentItem(item);
            list->scrollToItem(item);
            break;
        }
    }

    updateNoteFolderCandidateColors();
}

/**
 * Uses the folder of a candidate list item as note folder
 */
void WelcomeDialog::useNoteFolderCandidate(QListWidgetItem *item) {
    if (item == nullptr) {
        return;
    }

    MetricsService::instance()->sendVisitIfEnabled(
        QStringLiteral("welcome-dialog/note-folder/candidate-selected"));

    setNotesPath(item->data(NoteFolderCandidatePathRole).toString());

    // Nextcloud Notes stores categories in subfolders, they would be hidden otherwise
    if (item->data(NoteFolderCandidateHasNotesInSubFoldersRole).toBool()) {
        ui->showNoteSubFoldersCheckBox->setChecked(true);
    }
}

/**
 * Asks the user before a new note folder is created while existing note folders were found
 *
 * @return true if the new note folder should be created
 */
bool WelcomeDialog::confirmNewNoteFolderCreation() {
    if (ui->noteFolderCandidatesListWidget->count() == 0) {
        return true;
    }

    QListWidgetItem *item = ui->noteFolderCandidatesListWidget->currentItem();
    if (item == nullptr) {
        item = ui->noteFolderCandidatesListWidget->item(0);
    }

    const QString candidatePath =
        QDir::toNativeSeparators(item->data(NoteFolderCandidatePathRole).toString());

    const QString foundText =
        ui->noteFolderCandidatesListWidget->count() == 1
            ? tr("The folder <strong>%1</strong> on your computer already seems to contain "
                 "notes.")
            : tr("Some folders on your computer already seem to contain notes, for example "
                 "<strong>%1</strong>.");
    const QString text =
        foundText.arg(candidatePath.toHtmlEscaped()) + QStringLiteral("<br /><br />") +
        tr("Creating the new note folder <strong>%1</strong> will keep your notes separate. "
           "If you already use QOwnNotes or Nextcloud Notes on another device or on the web, "
           "you probably want to use the existing folder instead.")
            .arg(QDir::toNativeSeparators(_notesPath).toHtmlEscaped());

    QMessageBox messageBox(QMessageBox::Question, tr("Existing note folders found"), text,
                           QMessageBox::NoButton, this);
    messageBox.setTextFormat(Qt::RichText);
    QPushButton *useExistingButton =
        messageBox.addButton(tr("Use existing folder"), QMessageBox::AcceptRole);
    QPushButton *createButton =
        messageBox.addButton(tr("Create new folder"), QMessageBox::DestructiveRole);
    QPushButton *cancelButton = messageBox.addButton(QMessageBox::Cancel);
    messageBox.setDefaultButton(useExistingButton);
    messageBox.setEscapeButton(cancelButton);
    messageBox.exec();

    if (messageBox.clickedButton() == createButton) {
        MetricsService::instance()->sendVisitIfEnabled(
            QStringLiteral("welcome-dialog/note-folder/candidates-create-new"));
        return true;
    }

    if (messageBox.clickedButton() == useExistingButton) {
        // Stay on the page, so the user can review the chosen folder before going on
        useNoteFolderCandidate(item);
    } else {
        MetricsService::instance()->sendVisitIfEnabled(
            QStringLiteral("welcome-dialog/note-folder/candidates-cancel"));
    }

    ui->noteFolderCandidatesListWidget->setFocus();

    return false;
}

void WelcomeDialog::on_noteFolderCandidatesListWidget_itemClicked(QListWidgetItem *item) {
    useNoteFolderCandidate(item);
}

void WelcomeDialog::on_noteFolderCandidatesListWidget_currentItemChanged(
    QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous)

    updateNoteFolderCandidateColors();

    // Choosing with the keyboard uses the folder right away, like a click
    if (current != nullptr && ui->noteFolderCandidatesListWidget->hasFocus()) {
        useNoteFolderCandidate(current);
    }
}

void WelcomeDialog::on_cloudSettingsButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled(
        QStringLiteral("welcome-dialog/owncloud-settings"));

    auto *dialog = new SettingsDialog(SettingsDialog::CloudPage, this);
    dialog->exec();
}

void WelcomeDialog::on_networkSettingsButton_clicked() {
    MetricsService::instance()->sendVisitIfEnabled(
        QStringLiteral("welcome-dialog/network-settings"));

    auto *dialog = new SettingsDialog(SettingsDialog::NetworkPage, this);
    dialog->exec();
}

void WelcomeDialog::closeEvent(QCloseEvent *event) {
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("welcome-dialog/close"));
    MasterDialog::closeEvent(event);
}
