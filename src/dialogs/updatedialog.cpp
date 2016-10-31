#include "updatedialog.h"
#include "ui_updatedialog.h"
#include <QDesktopServices>
#include <QUrl>
#include <QPushButton>
#include <QDebug>
#include <QSettings>
#include <release.h>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMessageBox>
#include <utils/misc.h>
#include <QDir>
#include <QTemporaryFile>

UpdateDialog::UpdateDialog(QWidget *parent, QString changesHtml,
                           QString releaseUrl, QString releaseVersionString,
                           int releaseBuildNumber) :
        MasterDialog(parent),
        ui(new Ui::UpdateDialog) {
    ui->setupUi(this);
    ui->downloadProgressBar->hide();

    ui->changeLogEdit->setHtml(changesHtml);
    ui->versionLabel->setText("Version " + releaseVersionString + " - build " +
                              QString::number(releaseBuildNumber));
    this->releaseVersionString = releaseVersionString;
    this->releaseUrl = releaseUrl;

    QPushButton *button;
    ui->buttonBox->clear();

    _updateButton = new QPushButton(tr("&Update"));
    _updateButton->setProperty("ActionRole", Update);
    _updateButton->setDefault(true);
    // TODO(pbek): use other icon
    _updateButton->setIcon(
            QIcon::fromTheme(
                    "edit-download",
                    QIcon(":/icons/breeze-qownnotes/16x16/edit-download.svg")));
    ui->buttonBox->addButton(_updateButton, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Download latest"));
    button->setProperty("ActionRole", Download);
    button->setDefault(true);
    button->setIcon(
            QIcon::fromTheme(
                    "edit-download",
                    QIcon(":/icons/breeze-qownnotes/16x16/edit-download.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Skip version"));
    button->setProperty("ActionRole", Skip);
    button->setDefault(false);
    button->setIcon(
            QIcon::fromTheme(
                    "media-skip-forward",
                    QIcon(":/icons/breeze-qownnotes/16x16/"
                                  "media-skip-forward.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Cancel"));
    button->setProperty("ActionRole", Cancel);
    button->setIcon(
            QIcon::fromTheme(
                    "dialog-cancel",
                    QIcon(":/icons/breeze-qownnotes/16x16/dialog-cancel.svg")));
    button->setDefault(false);
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    connect(this->ui->buttonBox, SIGNAL(clicked(QAbstractButton *)),
            SLOT(dialogButtonClicked(QAbstractButton *)));

    _networkManager = new QNetworkAccessManager();
    QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply *)),
                     this, SLOT(slotReplyFinished(QNetworkReply *)));
}

UpdateDialog::~UpdateDialog() {
    delete ui;
}

int UpdateDialog::exec() {
    setIsUpdateDialogOpen(true);
    int result = MasterDialog::exec();
    setIsUpdateDialogOpen(false);
    return result;
}

void UpdateDialog::closeEvent(QCloseEvent *event) {
    setIsUpdateDialogOpen(false);
}

void UpdateDialog::show() {
    setIsUpdateDialogOpen(true);
    MasterDialog::show();
}

void UpdateDialog::dialogButtonClicked(QAbstractButton *button) {
    int actionRole = button->property("ActionRole").toInt();

    switch (actionRole) {
        case Skip:
        {
            QSettings settings;
            settings.setValue("skipVersion", this->releaseVersionString);
            qDebug() << "skip version";
            break;
        }
        case Download:
        {
            QString release = qApp->property("release").toString();
            // if the release was build by the CI systems download the new
            // release
            if ((release == "Travis CI") || (release == "AppVeyor")) {
                // download the new release
                QDesktopServices::openUrl(QUrl(releaseUrl.toUtf8()));
            } else {
                // open the installation page
                QDesktopServices::openUrl(
                        QUrl("http://www.qownnotes.org/installation"));
            }
            break;
        }
        case Update:
        {
            qDebug() << __func__ << " - 'releaseUrl': " << releaseUrl;

            // TODO(pbek): url for debugging
            //releaseUrl = "http://downloads.sourceforge.net/project/qownnotes/test/QOwnNotes.zip";
            //releaseUrl = "http://heanet.dl.sourceforge.net/project/qownnotes/test/x_QOwnNotes.zip";
            //releaseUrl = "http://heanet.dl.sourceforge.net/project/qownnotes/test/QOwnNotes.zip";

            ui->downloadProgressBar->show();
            _updateButton->setDisabled(true);

            QUrl url(releaseUrl);
            QNetworkRequest networkRequest(url);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
            networkRequest.setAttribute(
                    QNetworkRequest::FollowRedirectsAttribute, true);
#endif

            QNetworkReply *reply = _networkManager->get(networkRequest);

            connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
                    this, SLOT(releaseDownloadProgress(qint64, qint64)));

            break;
        }
        default:
            break;
    }

    if (actionRole != Update) {
        this->close();
    }
}

/**
 * Shows the download progress
 */
void UpdateDialog::releaseDownloadProgress(
        qint64 bytesReceived, qint64 bytesTotal) {
    ui->downloadProgressBar->setMaximum(bytesTotal);
    ui->downloadProgressBar->setValue(bytesReceived);
}

/**
 * Stores the downloaded file
 *
 * @param reply
 */
void UpdateDialog::slotReplyFinished(QNetworkReply *reply) {
    if (reply == NULL) {
        return;
    }

    ui->downloadProgressBar->hide();

    qDebug() << "Reply from " << reply->url().path();
    QByteArray data = reply->readAll();

//    qDebug() << __func__ << " - 'data': " << data;
     qDebug() << __func__ << " - 'data.size': " << data.size();

    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(
                0, tr("Download error"),
                tr("Error while downloading:\n%1").arg(reply->errorString()));

        qWarning() << tr("network error: %1").arg(reply->errorString());
        _updateButton->setDisabled(false);
        return;
    }
#if defined(Q_OS_MAC)
    QString suffix = "dmg";
#elif defined(Q_OS_WIN)
    QString suffix = "zip";
#elif defined(Q_OS_LINUX)
    QString suffix = "deb";
#else
    QString suffix = "download";
#endif

    QTemporaryFile *tempFile = new QTemporaryFile(
            QDir::tempPath() + "/QOwnNotes-XXXXXX." + suffix);

    // we want to keep the file to be used in the update process
    tempFile->setAutoRemove(false);

    // get a temporary file
    if (!tempFile->open()) {
        QMessageBox::critical(
                    0, tr("File error"),
                    tr("Could not open temporary file:\n%1")
                    .arg(tempFile->errorString()));
        return;
    }

    // file->fileName() only holds a value after file->open()
    QString filePath = tempFile->fileName();
    tempFile->close();
    // we need to delete the object so the later written file
    // can be read again while the app is open
    tempFile->deleteLater();
    destroy(tempFile);

    // unfortunately if you write to a QTemporaryFile under Windows the data
    // only gets written when the application quits, so we need a QFile to write to
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(
                0, tr("File error"),
                tr("Could not store downloaded file:\n%1")
                        .arg(file.errorString()));
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
    initializeUpdateProcess(filePath);
}

/**
 * Initializes the update process
 */
bool UpdateDialog::initializeUpdateProcess(QString filePath) {
#if defined(Q_OS_MAC)
    // TODO(pbek): implement OS X updater
#elif defined(Q_OS_WIN)
    initializeWindowsUpdateProcess(filePath);
#endif
}

/**
 * Initializes the Windows update process
 */
bool UpdateDialog::initializeWindowsUpdateProcess(QString filePath) {
    // get the folder path from the file path
    int lastPoint = filePath.lastIndexOf(".");
    QString folderPath = filePath.left(lastPoint);

    // create a new folder
    QDir dir;
    if (!dir.mkpath(folderPath)) {
        QMessageBox::critical(0, tr("Error"),
                              tr("Couldn't create folder: %1").arg(folderPath));
        return false;
    }

    QStringList parameters;
    parameters << QDir::toNativeSeparators(filePath)
               << "-d"
               << QDir::toNativeSeparators(folderPath);

    // uncompress the zip file
    // TODO(pbek): for testing
    QString result = Utils::Misc::startSynchronousProcess("C:\\Users\\omega\\Code\\QOwnNotes\\appveyor\\unzip.exe", parameters);
//    QString result = Utils::Misc::startSynchronousProcess("unzip.exe", parameters);
    qDebug() << __func__ << " - 'result': " << result;

    QString updaterPath = folderPath + "/QOwnNotes.exe";

    // check if updater QOwnNotes executable exists
    QFile updaterFile(updaterPath);
    if (!updaterFile.exists()) {
        QMessageBox::critical(
                0, tr("Error"),
                tr("Couldn't find updater executable: %1").arg(updaterPath));
        return false;
    }

    if (QMessageBox::information(
            this,
            tr("Proceed with update"),
            tr("The download is now finished. Do you want to update and "
                       "restart QOwnNotes?"),
            tr("&Update and restart"), tr("&Cancel"), QString::null,
            0, 1) == 1) {
        return false;
    }

    parameters.clear();
    parameters << "--update" << filePath
               << QCoreApplication::applicationDirPath();
    qDebug() << __func__ << " - 'parameters': " << parameters;

    // start QOwnNotes as updater
    Utils::Misc::startDetachedProcess(updaterPath, parameters);

    qApp->quit();
    return true;
}

/**
 * Checks if the update dialog is open
 */
bool UpdateDialog::isUpdateDialogOpen() {
    return qApp->property("isUpdateDialogOpen").value<bool>();
}

/**
 * Sets if the update dialog is open
 */
void UpdateDialog::setIsUpdateDialogOpen(bool isOpen) {
    qApp->setProperty("isUpdateDialogOpen", QVariant::fromValue<bool>(isOpen));
}
