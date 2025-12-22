#ifndef NOTEDIALOG_H
#define NOTEDIALOG_H

#include "entities/note.h"
#include "masterdialog.h"

namespace Ui {
class NoteDialog;
}

class NoteDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit NoteDialog(QWidget *parent = 0);
    ~NoteDialog();
    void setNote(Note &note);

   private slots:
    void on_noteTextView_anchorClicked(const QUrl &url);

    void on_tabWidget_currentChanged(int index);

    void onReloadButtonClicked();

    void onJumpToNoteButtonClicked();

   private:
    Ui::NoteDialog *ui;
    Note _note;
};

#endif    // NOTEDIALOG_H
