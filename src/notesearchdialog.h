#ifndef NOTESEARCHDIALOG_H
#define NOTESEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class NoteSearchDialog;
}

class NoteSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoteSearchDialog( QString dialogTitle = "", QWidget *parent = 0);
    ~NoteSearchDialog();

    QString getSelectedNoteName();
private slots:
    void on_searchLineEdit_textChanged(const QString &arg1);
    void on_notesListWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::NoteSearchDialog *ui;
    int firstVisibleNoteListRow;
    bool eventFilter(QObject *obj, QEvent *event);
    QString selectedNoteText;
};

#endif // NOTESEARCHDIALOG_H
