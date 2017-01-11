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
    ui->textEdit->setText(note.getNoteText());
}

NoteDialog::~NoteDialog() {
    delete ui;
}
