#ifndef TAGADDDIALOG_H
#define TAGADDDIALOG_H

#include <QDialog>
#include "masterdialog.h"

namespace Ui {
class TagAddDialog;
}

class TagAddDialog : public MasterDialog
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
