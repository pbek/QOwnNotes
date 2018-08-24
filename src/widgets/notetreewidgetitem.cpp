#include "notetreewidgetitem.h"
#include "ui_notetreewidgetitem.h"

NoteTreeWidgetItem::NoteTreeWidgetItem(Note note, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NoteTreeWidgetItem) {
    ui->setupUi(this);
    updateUserInterface(note);
}

void NoteTreeWidgetItem::updateUserInterface(Note &note) const {
    ui->nameLabel->setText(note.getName());

    QDateTime modified = note.getFileLastModified();
    ui->dateLabel->setText(modified.toString());

    QString noteText = note.getNotePreviewText();
    ui->textLabel->setText(noteText);
    ui->textLabel->setStyleSheet("* {color: gray}");
}

NoteTreeWidgetItem::~NoteTreeWidgetItem() {
    delete ui;
}
