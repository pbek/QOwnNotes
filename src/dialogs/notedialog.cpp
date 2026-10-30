#include "notedialog.h"

#include <entities/note.h>
#include <entities/notefolder.h>

#include <QDebug>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QLocale>
#include <QPushButton>

#include "mainwindow.h"
#include "services/settingsservice.h"
#include "ui_notedialog.h"
#include "utils/gui.h"
#include "utils/urlhandler.h"

NoteDialog::NoteDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::NoteDialog) {
    ui->setupUi(this);
    afterSetupUI();
    ui->textEdit->initSearchFrame(ui->searchFrame);
    ui->textEdit->setReadOnly(true);
    Utils::Gui::setMutedTextColor(ui->noteInfoLabel);
    ui->tabWidget->setCurrentIndex(SettingsService().value("NoteDialog/tabWidgetIndex").toInt());

    // set the note text view font
    QFont font;
    font.fromString(Utils::Misc::previewFontString());
    ui->noteTextView->setFont(font);

    // Add custom buttons to the button box
    QPushButton *reloadButton =
        ui->buttonBox->addButton(tr("Reload"), QDialogButtonBox::ActionRole);
    reloadButton->setIcon(
        QIcon::fromTheme(QStringLiteral("view-refresh"),
                         QIcon(QStringLiteral(":/icons/breeze-qownnotes/16x16/view-refresh.svg"))));
    reloadButton->setToolTip(tr("Reload the note text"));

    QPushButton *jumpToNoteButton =
        ui->buttonBox->addButton(tr("Jump to note"), QDialogButtonBox::ActionRole);
    jumpToNoteButton->setIcon(
        QIcon::fromTheme(QStringLiteral("go-next"),
                         QIcon(QStringLiteral(":/icons/breeze-qownnotes/16x16/go-next.svg"))));
    jumpToNoteButton->setToolTip(tr("Jump to the note in the main window"));

    // Connect the custom buttons to their slots
    connect(reloadButton, &QPushButton::clicked, this, &NoteDialog::onReloadButtonClicked);
    connect(jumpToNoteButton, &QPushButton::clicked, this, &NoteDialog::onJumpToNoteButtonClicked);
}

void NoteDialog::setNote(Note &note) {
    _note = note;    // Store the note for later use
    setWindowTitle(note.getName());

    // show the decrypted text if possible
    const bool canDecrypt = note.hasEncryptedNoteText() && note.canDecryptNoteText();
    QString text = canDecrypt ? note.fetchDecryptedNoteText() : note.getNoteText();
    ui->textEdit->setPlainText(text);

    // Show where the note is stored and when it was modified
    QStringList infos;
    const QString subFolderPath = note.relativeNoteSubFolderPath();

    if (!subFolderPath.isEmpty()) {
        infos << tr("Folder: %1").arg(subFolderPath);
    }

    infos << tr("Last modified at %1")
                 .arg(QLocale().toString(note.getFileLastModified(), QLocale::ShortFormat));

    if (note.hasEncryptedNoteText() && !canDecrypt) {
        infos << tr("The note is encrypted and can't be decrypted");
    }

    ui->noteInfoLabel->setText(infos.join(QStringLiteral(" · ")));
    ui->noteInfoLabel->setToolTip(note.fullNoteFilePath());

    ui->noteTextView->setHtml(note.toMarkdownHtml(NoteFolder::currentLocalPath()),
                              note.fullNoteFilePath());
}

NoteDialog::~NoteDialog() { delete ui; }

void NoteDialog::on_noteTextView_previewLinkClicked(const QUrl &url) {
    qDebug() << __func__ << " - 'url': " << url;

    if (UrlHandler::isUrlSchemeLocal(url)) {
        return;
    }

    QDesktopServices::openUrl(url);
}

void NoteDialog::on_tabWidget_currentChanged(int index) {
    SettingsService().setValue("NoteDialog/tabWidgetIndex", index);
}

void NoteDialog::onReloadButtonClicked() {
    // Refetch the note from the database
    if (_note.refetch()) {
        // Update the display with the refreshed note
        setNote(_note);
    }
}

void NoteDialog::onJumpToNoteButtonClicked() {
    // Get the MainWindow instance and jump to the note
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow) {
        mainWindow->setCurrentNote(_note);
    }
}
