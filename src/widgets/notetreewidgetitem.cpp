#include "notetreewidgetitem.h"

#include "ui_notetreewidgetitem.h"

NoteTreeWidgetItem::NoteTreeWidgetItem(const Note &note, QWidget *parent)
    : QWidget(parent), ui(new Ui::NoteTreeWidgetItem) {
    ui->setupUi(this);
    updateUserInterface(note);
}

void NoteTreeWidgetItem::updateUserInterface(const Note &note) const {
    const QString name = note.isConflictedCopy()
                             ? Note::conflictedCopyEmoji() + QStringLiteral(" ") + note.getName()
                             : note.getName();
    ui->nameLabel->setText(name);

    QDateTime modified = note.getFileLastModified();
    ui->dateLabel->setText(modified.toString());

    QString noteText = note.getNotePreviewText();
    ui->textLabel->setText(noteText);
    ui->textLabel->setStyleSheet(QStringLiteral("* {color: gray}"));
}

NoteTreeWidgetItem::~NoteTreeWidgetItem() { delete ui; }
