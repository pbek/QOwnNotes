#ifndef NOTEDIFFDIALOG_H
#define NOTEDIFFDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include "masterdialog.h"

namespace Ui {
class NoteDiffDialog;
}

class NoteDiffDialog : public MasterDialog
{
    Q_OBJECT

public:
    enum ButtonRole {
        Unset,  // nothing was selected
        Overwrite,
        Reload,
        Ignore,
        Cancel
    };

    explicit NoteDiffDialog(QWidget *parent = 0, QString html = "");
    ~NoteDiffDialog();
    int resultActionRole();

private slots:
    void dialogButtonClicked( QAbstractButton *button );

private:
    Ui::NoteDiffDialog *ui;
    int actionRole;
};

#endif // NOTEDIFFDIALOG_H
