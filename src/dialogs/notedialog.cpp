#include <entities/note.h>
#include "notedialog.h"
#include "ui_notedialog.h"

NoteDialog::NoteDialog(QWidget *parent) :
    MasterDialog(parent),
    ui(new Ui::NoteDialog) {
    ui->setupUi(this);
    ui->textEdit->initSearchFrame(ui->searchFrame);
    ui->textEdit->setReadOnly(true);
}

void NoteDialog::setNote(Note note) {
    setWindowTitle(note.getName());

    // show the decrypted text if possible
    QString text = note.hasEncryptedNoteText() && note.canDecryptNoteText() ?
                   note.getDecryptedNoteText() : note.getNoteText();
    ui->textEdit->setPlainText(text);
}

NoteDialog::~NoteDialog() {
    delete ui;
}
