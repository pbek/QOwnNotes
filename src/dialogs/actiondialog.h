#ifndef ACTIONDIALOG_H
#define ACTIONDIALOG_H

#include <QDialog>
#include <QMenuBar>
#include "masterdialog.h"

namespace Ui {
class ActionDialog;
}

class ActionDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit ActionDialog(QMenuBar *menuBar, QWidget *parent = 0);
    ~ActionDialog();

private:
    Ui::ActionDialog *ui;
};

#endif // ACTIONDIALOG_H
