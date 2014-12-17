#ifndef NOTEDIFFDIALOG_H
#define NOTEDIFFDIALOG_H

#include <QDialog>

namespace Ui {
class NoteDiffDialog;
}

class NoteDiffDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NoteDiffDialog(QWidget *parent = 0, QString html = "");
    ~NoteDiffDialog();

private slots:

private:
    Ui::NoteDiffDialog *ui;
};

#endif // NOTEDIFFDIALOG_H
