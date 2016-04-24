#ifndef TAGADDDIALOG_H
#define TAGADDDIALOG_H

#include <QDialog>

namespace Ui {
class TagAddDialog;
}

class TagAddDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TagAddDialog(QWidget *parent = 0);
    ~TagAddDialog();
    QString name();

private:
    Ui::TagAddDialog *ui;
};

#endif // TAGADDDIALOG_H
