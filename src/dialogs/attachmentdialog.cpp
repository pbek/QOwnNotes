#include "attachmentdialog.h"

#include <QClipboard>
#include <QDebug>
#include <QMessageBox>
#include <QMimeDatabase>
#include <QTemporaryFile>

#include "filedialog.h"
#include "ui_attachmentdialog.h"
#include "utils/misc.h"

AttachmentDialog::AttachmentDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::AttachmentDialog) {
    ui->setupUi(this);
    afterSetupUI();
    ui->fileEdit->setFocus();
    ui->downloadButton->hide();
    ui->downloadFrame->hide();
    ui->infoFrame->hide();

    _networkManager = new QNetworkAccessManager(this);
    QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply *)), this,
                     SLOT(slotReplyFinished(QNetworkReply *)));

    QClipboard *clipboard = QApplication::clipboard();
    QString text = clipboard->text();
    const QUrl url(text);
    const QFile file(text);

    // set text from clipboard
    if (url.isValid() && (!url.scheme().isEmpty() || file.exists())) {
        ui->fileEdit->setText(text);
    }
}

AttachmentDialog::~AttachmentDialog() { delete ui; }

void AttachmentDialog::on_openButton_clicked() {
    FileDialog dialog(QStringLiteral("InsertAttachment"));
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setWindowTitle(tr("Select file to insert"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            ui->fileEdit->setText(fileName);
        }
    }
}

void AttachmentDialog::on_fileEdit_textChanged(const QString &arg1) {
    auto pathOrUrl = arg1.trimmed();

    // Clear info when path is empty
    if (pathOrUrl.isEmpty()) {
        ui->infoFrame->hide();
        ui->downloadButton->hide();
        return;
    }

    auto url = QUrl(pathOrUrl);
    ui->infoFrame->hide();

    if (!url.isValid()) {
        ui->downloadButton->hide();

        return;
    }

    ui->downloadButton->setVisible(url.scheme().startsWith(QLatin1String("http")));

    if (url.scheme() == QLatin1String("file")) {
        ui->fileEdit->setText(url.toLocalFile());
    } else if (url.scheme().isEmpty()) {
        ui->infoFrame->show();
        QFileInfo fileInfo(pathOrUrl);

        QMimeDatabase db;
        QMimeType type = db.mimeTypeForFile(pathOrUrl);

        ui->infoLabel->setText(Utils::Misc::toHumanReadableByteSize(fileInfo.size()) + " - " +
                               type.comment());
    }
}

void AttachmentDialog::on_downloadButton_clicked() {
    ui->downloadButton->setDisabled(true);
    ui->downloadProgressBar->setValue(0);
    ui->downloadSizeLabel->setText(QString());
    ui->downloadFrame->show();

    QUrl url(ui->fileEdit->text());
    QNetworkRequest networkRequest(url);

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    _networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

    QNetworkReply *reply = _networkManager->get(networkRequest);

    connect(reply, SIGNAL(downloadProgress(qint64, qint64)), this,
            SLOT(downloadProgress(qint64, qint64)));
    connect(ui->downloadCancelButton, SIGNAL(clicked()), reply, SLOT(abort()));
}

/**
 * Shows the download progress
 */
void AttachmentDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    ui->downloadProgressBar->setMaximum(static_cast<int>(bytesTotal / 1000));
    ui->downloadProgressBar->setValue(static_cast<int>(bytesReceived / 1000));
    ui->downloadSizeLabel->setText(Utils::Misc::toHumanReadableByteSize(bytesReceived) + " / " +
                                   Utils::Misc::toHumanReadableByteSize(bytesTotal));
}

/**
 * Stores the downloaded file
 *
 * @param reply
 */
void AttachmentDialog::slotReplyFinished(QNetworkReply *reply) {
    if (reply == nullptr) {
        return;
    }

    reply->deleteLater();
    ui->downloadFrame->hide();
    ui->downloadButton->setDisabled(false);

    qDebug() << "Reply from " << reply->url().path();
    QByteArray data = reply->readAll();
    qDebug() << __func__ << " - 'data.size': " << data.size();

    if (reply->error() != QNetworkReply::NoError &&
        reply->error() != QNetworkReply::OperationCanceledError) {
        _accept = false;
        QMessageBox::critical(nullptr, tr("Download error"),
                              tr("Error while downloading:\n%1").arg(reply->errorString()));

        qWarning() << QStringLiteral("Network error: %1").arg(reply->errorString());

        return;
    }

    QMimeDatabase db;
    QMimeType type = db.mimeTypeForData(data);
    QString suffix;

    // try to detect the mime type of the file and use a proper file suffix
    if (type.isValid()) {
        QStringList suffixes = type.suffixes();
        if (suffixes.count() > 0) {
            suffix = suffixes[0];
        }
    }

    auto *tempFile = new QTemporaryFile(QDir::tempPath() + "/QOwnNotes-XXXXXX." + suffix);

    // we want to keep the file to be used in the update process
    tempFile->setAutoRemove(false);

    // get a temporary file
    if (!tempFile->open()) {
        QMessageBox::critical(
            nullptr, tr("File error"),
            tr("Could not open temporary file:\n%1").arg(tempFile->errorString()));
        return;
    }

    // file->fileName() only holds a value after file->open()
    QString filePath = tempFile->fileName();
    tempFile->close();

    // we need to delete the object so the later written file
    // can be read again while the app is open
    tempFile->deleteLater();

    // this "destroy" closes the dialog!
    // destroy(tempFile);

    // unfortunately if you write to a QTemporaryFile under Windows the data
    // only gets written when the application quits,
    // so we need a QFile to write to
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(nullptr, tr("File error"),
                              tr("Could not store downloaded file:\n%1").arg(file.errorString()));
        return;
    }

    // store the data in the file
    file.write(data);
    file.close();
    // we need to delete the object so the written file
    // can be read again while the app is open
    file.deleteLater();

    // this is needed so deleteLater() really deletes the object
    // and the downloaded file can be accessed
    QCoreApplication::processEvents();

    // initialize the update process
    ui->fileEdit->setText(filePath);

    if (_accept) {
        _accept = false;
        MasterDialog::accept();
    }
}

QFile *AttachmentDialog::getFile() { return new QFile(ui->fileEdit->text()); }

QString AttachmentDialog::getTitle() { return ui->titleEdit->text(); }

void AttachmentDialog::accept() {
    if (QUrl(ui->fileEdit->text()).scheme().startsWith(QLatin1String("http"))) {
        _accept = true;
        on_downloadButton_clicked();
    } else {
        MasterDialog::accept();
    }
}
