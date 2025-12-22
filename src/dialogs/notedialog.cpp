#include "notedialog.h"

#include <entities/note.h>
#include <entities/notefolder.h>

#include <QDebug>
#include <QDesktopServices>
#include <QDialogButtonBox>
#include <QPushButton>

#include "mainwindow.h"
#include "services/settingsservice.h"
#include "ui_notedialog.h"
#include "utils/urlhandler.h"

NoteDialog::NoteDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::NoteDialog) {
    ui->setupUi(this);
    afterSetupUI();
    ui->textEdit->initSearchFrame(ui->searchFrame);
    ui->textEdit->setReadOnly(true);
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
    QString text = note.hasEncryptedNoteText() && note.canDecryptNoteText()
                       ? note.fetchDecryptedNoteText()
                       : note.getNoteText();
    ui->textEdit->setPlainText(text);

    ui->noteTextView->setHtml(note.toMarkdownHtml(NoteFolder::currentLocalPath()));
}

NoteDialog::~NoteDialog() { delete ui; }

void NoteDialog::on_noteTextView_anchorClicked(const QUrl &url) {
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
