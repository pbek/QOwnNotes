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

    QString noteText = getNotePreviewText(note);
    ui->textLabel->setText(noteText);
    ui->textLabel->setStyleSheet("* {color: gray}");
}

/**
 * Generates the preview text of a note
 *
 * @param note
 * @return
 */
QString NoteTreeWidgetItem::getNotePreviewText(Note &note) const {
    QString noteText = note.getNoteText();

    // remove headlines
    noteText.remove(QRegularExpression("^.+\n=+\n+"));
    noteText.remove(QRegularExpression("^# .+\n+"));

    // only take the first three lines
    noteText = noteText.split("\n").mid(0, 3).join("\n");

    return noteText;
}

NoteTreeWidgetItem::~NoteTreeWidgetItem() {
    delete ui;
}
