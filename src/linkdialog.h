#ifndef NOTESEARCHDIALOG_H
#define NOTESEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class LinkDialog;
}

class LinkDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LinkDialog( QString dialogTitle = "", QWidget *parent = 0);
    ~LinkDialog();

    QString getSelectedNoteName();
    QString getURL();
private slots:
    void on_searchLineEdit_textChanged(const QString &arg1);
    void on_notesListWidget_doubleClicked(const QModelIndex &index);

private:
    Ui::LinkDialog *ui;
    int firstVisibleNoteListRow;
    bool eventFilter(QObject *obj, QEvent *event);
    QString selectedNoteText;
};

#endif // NOTESEARCHDIALOG_H
