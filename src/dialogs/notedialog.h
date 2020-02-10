#ifndef NOTEDIALOG_H
#define NOTEDIALOG_H

#include "masterdialog.h"

namespace Ui {
class NoteDialog;
}

class Note;

class NoteDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit NoteDialog(QWidget *parent = 0);
    ~NoteDialog();
    void setNote(Note note);

   private:
    Ui::NoteDialog *ui;
};

#endif    // NOTEDIALOG_H
