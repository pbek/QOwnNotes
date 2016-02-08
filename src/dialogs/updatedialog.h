#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include "masterdialog.h"

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = 0, QString changesHtml = "", QString releaseUrl = "", QString releaseVersionString = "", int releaseBuildNumber = 0);
    ~UpdateDialog();
    static bool isUpdateDialogOpen();
    int exec();

private slots:
    void dialogButtonClicked(QAbstractButton *button);
    void setIsUpdateDialogOpen(bool isOpen);

private:
    Ui::UpdateDialog *ui;
    QString releaseUrl;
    QString releaseVersionString;

    enum ButtonRole {
        Unset,  // nothing was selected
        Download,
        Skip,
        Cancel
    };
};

#endif // UPDATEDIALOG_H
