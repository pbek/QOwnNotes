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
    void setNote(Note &note);

   private slots:
    void on_noteTextView_anchorClicked(const QUrl &url);

    void on_tabWidget_currentChanged(int index);

   private:
    Ui::NoteDialog *ui;
};

#endif    // NOTEDIALOG_H
