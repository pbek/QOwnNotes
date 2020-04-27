#include "notedialog.h"

#include <entities/note.h>
#include <entities/notefolder.h>

#include "ui_notedialog.h"

#include <QDesktopServices>
#include <QDebug>

NoteDialog::NoteDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::NoteDialog) {
    ui->setupUi(this);
    ui->textEdit->initSearchFrame(ui->searchFrame);
    ui->textEdit->setReadOnly(true);
}

void NoteDialog::setNote(Note &note) {
    setWindowTitle(note.getName());

    // show the decrypted text if possible
    QString text = note.hasEncryptedNoteText() && note.canDecryptNoteText()
                       ? note.getDecryptedNoteText()
                       : note.getNoteText();
    ui->textEdit->setPlainText(text);

    ui->noteTextView->setHtml(note.toMarkdownHtml(NoteFolder::currentLocalPath()));
}

NoteDialog::~NoteDialog() { delete ui; }

void NoteDialog::on_noteTextView_anchorClicked(const QUrl &url) {
    qDebug() << __func__ << " - 'url': " << url;
    const QString scheme = url.scheme();

    if ((scheme == QStringLiteral("note") ||
         scheme == QStringLiteral("noteid") ||
         scheme == QStringLiteral("task") ||
         scheme == QStringLiteral("checkbox")) ||
        (scheme == QStringLiteral("file") &&
         Note::fileUrlIsNoteInCurrentNoteFolder(url))) {
        return;
    }

    QDesktopServices::openUrl(url);
}
