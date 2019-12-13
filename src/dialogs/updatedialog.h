#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include <QAbstractButton>
#include <QDialog>
#include <QNetworkAccessManager>
#include <QFile>
#include "masterdialog.h"

namespace Ui {
class UpdateDialog;
}

class UpdateDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit UpdateDialog(
            QWidget *parent = nullptr, QString changesHtml = QString(),
            QString releaseUrl = QString(), QString releaseVersionString = QString(),
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
        Disable,
        Cancel
    };

    void closeEvent(QCloseEvent *event);

    bool initializeUpdateProcess(const QString& filePath);

    bool initializeWindowsUpdateProcess(const QString& filePath);

    bool initializeMacOSUpdateProcess(QString releaseUrl);
};

#endif // UPDATEDIALOG_H
