#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QDialog>

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(QWidget *parent = 0, QString changesText = "", QString releaseUrl = "", QString releaseVersionString = "", int releaseBuildNumber = 0);
    ~UpdateDialog();

private slots:
    void downloadRelease();
private:
    Ui::UpdateDialog *ui;
    QString *releaseUrl;
};

#endif // UPDATEDIALOG_H
