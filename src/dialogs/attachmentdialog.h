#pragma once

#include "masterdialog.h"

#include <QFile>
#include <QNetworkReply>

namespace Ui {
class AttachmentDialog;
}

class AttachmentDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit AttachmentDialog(QWidget *parent = nullptr);
    ~AttachmentDialog();

    QFile *getFile();

    QString getTitle();

public slots:
    void accept();

private slots:
    void on_openButton_clicked();

    void on_fileEdit_textChanged(const QString &arg1);

    void on_downloadButton_clicked();

    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    void slotReplyFinished(QNetworkReply *reply);

private:
    Ui::AttachmentDialog *ui;
    QNetworkAccessManager *_networkManager;
    bool _accept = false;
};
