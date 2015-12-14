#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QAbstractButton>
#include <QDialog>

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = 0, QString changesHtml = "", QString releaseUrl = "", QString releaseVersionString = "", int releaseBuildNumber = 0);
    ~UpdateDialog();

private slots:
    void dialogButtonClicked(QAbstractButton *button);
private:
    Ui::UpdateDialog *ui;
    QString *releaseUrl;
    QString releaseVersionString;

    enum ButtonRole {
        Unset,  // nothing was selected
        Download,
        Skip,
        Cancel
    };
};

#endif // UPDATEDIALOG_H
