#ifndef EVERNOTEIMPORTDIALOG_H
#define EVERNOTEIMPORTDIALOG_H

#include <QDialog>

namespace Ui {
class EvernoteImportDialog;
}

class EvernoteImportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EvernoteImportDialog(QWidget *parent = 0);
    ~EvernoteImportDialog();

private slots:
    void on_fileButton_clicked();

private:
    Ui::EvernoteImportDialog *ui;

    void importNotes(QString data);

    int countNotes(QString data);

    void initNoteCount(QString data);
};

#endif // EVERNOTEIMPORTDIALOG_H
