#include "notedialog.h"

#include <entities/note.h>
#include <entities/notefolder.h>

#include <QDebug>
#include <QDesktopServices>
#include <QSettings>

#include "ui_notedialog.h"

NoteDialog::NoteDialog(QWidget *parent) : MasterDialog(parent), ui(new Ui::NoteDialog) {
    ui->setupUi(this);
    ui->textEdit->initSearchFrame(ui->searchFrame);
    ui->textEdit->setReadOnly(true);
    ui->tabWidget->setCurrentIndex(QSettings().value("NoteDialog/tabWidgetIndex").toInt());

    // set the note text view font
    QFont font;
    font.fromString(Utils::Misc::previewFontString());
    ui->noteTextView->setFont(font);
}

void NoteDialog::setNote(Note &note) {
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
    const QString scheme = url.scheme();

    if ((scheme == QStringLiteral("note") || scheme == QStringLiteral("noteid") ||
         scheme == QStringLiteral("task") || scheme == QStringLiteral("checkbox")) ||
        (scheme == QStringLiteral("file") && Note::fileUrlIsNoteInCurrentNoteFolder(url))) {
        return;
    }

    QDesktopServices::openUrl(url);
}

void NoteDialog::on_tabWidget_currentChanged(int index) {
    QSettings().setValue("NoteDialog/tabWidgetIndex", index);
}
