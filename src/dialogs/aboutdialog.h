#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include "masterdialog.h"

#include <QDialog>

namespace Ui {
class AboutDialog;
}

class AboutDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

private:
    Ui::AboutDialog *ui;
};

#endif // ABOUTDIALOG_H
