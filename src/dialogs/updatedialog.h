#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include <QNetworkAccessManager>
#include "masterdialog.h"

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(
            QWidget *parent = 0, QString changesHtml = "",
            QString releaseUrl = "", QString releaseVersionString = "",
            int releaseBuildNumber = 0);
    ~UpdateDialog();
    static bool isUpdateDialogOpen();
    int exec();

public slots:
    void show();

private slots:
    void dialogButtonClicked(QAbstractButton *button);
    void setIsUpdateDialogOpen(bool isOpen);
    void releaseDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void slotReplyFinished(QNetworkReply *reply);

private:
    Ui::UpdateDialog *ui;
    QString releaseUrl;
    QString releaseVersionString;
    QNetworkAccessManager *_networkManager;
    QPushButton *_updateButton;

    enum ButtonRole {
        Unset,  // nothing was selected
        Update,
        Download,
        Skip,
        Cancel
    };

    void closeEvent(QCloseEvent *event);
};

#endif // UPDATEDIALOG_H
