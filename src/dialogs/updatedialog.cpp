#include "updatedialog.h"

#include <release.h>
#include <services/metricsservice.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcessEnvironment>
#include <QPushButton>
#include <QTemporaryFile>
#include <QUrl>

#include "services/settingsservice.h"
#include "ui_updatedialog.h"

UpdateDialog::UpdateDialog(QWidget *parent, const QString &changesHtml, const QString &releaseUrl,
                           const QString &releaseVersionString)
    : MasterDialog(parent), ui(new Ui::UpdateDialog) {
    ui->setupUi(this);
    afterSetupUI();
    ui->downloadProgressBar->hide();

#if !defined(Q_OS_WIN)
    ui->permissionLabel->hide();
#endif

    ui->getInvolvedLabel->setText(ui->getInvolvedLabel->text().arg(
        "https://www.qownnotes.org/contributing/get-involved.html"));

    // inject some generic CSS styles
    ui->changeLogEdit->document()->setDefaultStyleSheet(Utils::Misc::genericCSS());
    //    ui->label_4->setText("<style>" + Utils::Misc::genericCSS() +
    //                                 "</style>" + ui->label_4->text());

    ui->changeLogEdit->setHtml(changesHtml);
    ui->versionLabel->setText("Version " + releaseVersionString);
    this->releaseVersionString = releaseVersionString;
    this->releaseUrl = releaseUrl;

    ui->buttonBox->clear();

    auto button = new QPushButton(tr("Just download"));
    button->setProperty("ActionRole", Download);

    _updateButton = new QPushButton(tr("&Update"));
    _updateButton->setProperty("ActionRole", Update);
    _updateButton->setDefault(true);
    _updateButton->setIcon(
        QIcon::fromTheme("svn-update", QIcon(":/icons/breeze-qownnotes/16x16/svn-update.svg")));
    ui->buttonBox->addButton(_updateButton, QDialogButtonBox::ActionRole);

#ifdef Q_OS_LINUX
    QString release = qApp->property("release").toString();

    // under Linux automatic updates are only available for the AppImage
    if (release != "AppImage") {
        ui->buttonBox->removeButton(_updateButton);
        button->setText(tr("&Download latest"));
        button->setDefault(true);
    }
#endif

    button->setIcon(QIcon::fromTheme(QStringLiteral("edit-download"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/edit-download.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Skip version"));
    button->setProperty("ActionRole", Skip);
    button->setDefault(false);
    button->setIcon(QIcon::fromTheme(QStringLiteral("media-skip-forward"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/"
                                           "media-skip-forward.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Disable dialog"));
    button->setToolTip(tr("Don't show this dialog automatically"));
    button->setProperty("ActionRole", Disable);
    button->setDefault(false);
    button->setIcon(
        QIcon::fromTheme(QStringLiteral("window-close"), QIcon(":/icons/breeze-qownnotes/16x16/"
                                                               "window-close.svg")));
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    button = new QPushButton(tr("&Cancel"), this);
    button->setProperty("ActionRole", Cancel);
    button->setIcon(QIcon::fromTheme(QStringLiteral("dialog-cancel"),
                                     QIcon(":/icons/breeze-qownnotes/16x16/dialog-cancel.svg")));
    button->setDefault(false);
    ui->buttonBox->addButton(button, QDialogButtonBox::ActionRole);

    connect(this->ui->buttonBox, SIGNAL(clicked(QAbstractButton *)),
            SLOT(dialogButtonClicked(QAbstractButton *)));

    _networkManager = new QNetworkAccessManager(this);
    QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply *)), this,
                     SLOT(slotReplyFinished(QNetworkReply *)));
}

UpdateDialog::~UpdateDialog() { delete ui; }

int UpdateDialog::exec() {
    setIsUpdateDialogOpen(true);
    int result = MasterDialog::exec();
    setIsUpdateDialogOpen(false);
    return result;
}

void UpdateDialog::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event);
    setIsUpdateDialogOpen(false);
}

void UpdateDialog::show() {
    setIsUpdateDialogOpen(true);
    MasterDialog::show();
}

void UpdateDialog::dialogButtonClicked(QAbstractButton *button) {
    int actionRole = button->property("ActionRole").toInt();

    switch (actionRole) {
        case Skip: {
            SettingsService settings;
            settings.setValue(QStringLiteral("skipVersion"), this->releaseVersionString);
            qDebug() << "skip version";
            break;
        }
        case Disable: {
            SettingsService settings;
            settings.setValue(QStringLiteral("disableAutomaticUpdateDialog"), true);
            qDebug() << "disable dialog";
            break;
        }
        case Download: {
            QString release = qApp->property("release").toString();
            // if the release was build by the CI systems download the new
            // release
            if ((release == QLatin1String("Travis CI")) || (release == QLatin1String("AppVeyor"))) {
                // download the new release
                QDesktopServices::openUrl(QUrl(releaseUrl.toUtf8()));
            } else if ((release == QLatin1String("AppImage"))) {
                // download the new release
                QDesktopServices::openUrl(QUrl(releaseUrl.toUtf8()));
            } else {
                // open the installation page
                QDesktopServices::openUrl(
                    QUrl(QStringLiteral("https://www.qownnotes.org/installation")));
            }
            break;
        }
        case Update: {
            qDebug() << __func__ << " - 'releaseUrl': " << releaseUrl;
            QString productName = QStringLiteral("unknown");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
            productName = QSysInfo::prettyProductName();
#endif

            MetricsService::instance()->sendEventIfEnabled(
                QStringLiteral("app/auto-update"), QStringLiteral("app"),
                QStringLiteral("auto update"), productName);

#if defined(Q_OS_MAC)
            // under macOS we will just start the update.sh
            initializeMacOSUpdateProcess(releaseUrl);
#else
            // we will download the latest release
            ui->downloadProgressBar->show();
            _updateButton->setDisabled(true);

            QUrl url(releaseUrl);
            QNetworkRequest networkRequest(url);

#if (QT_VERSION < QT_VERSION_CHECK(5, 9, 0))
            // we really need redirects for GitHub urls!
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
                    SLOT(releaseDownloadProgress(qint64, qint64)));
#endif

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
void UpdateDialog::releaseDownloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    ui->downloadProgressBar->setMaximum(static_cast<int>(bytesTotal / 1000));
    ui->downloadProgressBar->setValue(static_cast<int>(bytesReceived / 1000));
    ui->downloadProgressBar->setToolTip(Utils::Misc::toHumanReadableByteSize(bytesReceived) +
                                        " / " + Utils::Misc::toHumanReadableByteSize(bytesTotal));
}

/**
 * Stores the downloaded file and starts updater script
 *
 * @param reply
 */
void UpdateDialog::slotReplyFinished(QNetworkReply *reply) {
    if (reply == nullptr) {
        return;
    }

    reply->deleteLater();
    ui->downloadProgressBar->hide();

    qDebug() << "Reply from " << reply->url().path();
    QByteArray data = reply->readAll();

    //    qDebug() << __func__ << " - 'data': " << data;
    qDebug() << __func__ << " - 'data.size': " << data.size();

    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::critical(nullptr, tr("Download error"),
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
    QString suffix = QStringLiteral("AppImage");
#else
    QString suffix = "download";
#endif

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
    destroy(tempFile);

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
    initializeUpdateProcess(filePath);
}

/**
 * Initializes the update process
 */
bool UpdateDialog::initializeUpdateProcess(const QString &filePath) {
#if defined(Q_OS_MAC)
    // the OS X updater initializeMacOSUpdateProcess will be started
    // from dialogButtonClicked
    Q_UNUSED(filePath);
#elif defined(Q_OS_WIN)
    return initializeWindowsUpdateProcess(filePath);
#elif defined(Q_OS_LINUX)
    return initializeLinuxUpdateProcess(filePath);
#endif

    return true;
}

/**
 * Initializes the macOS update process
 */
bool UpdateDialog::initializeMacOSUpdateProcess(const QString &releaseUrl) {
    // find out the /Applications folder
    QString applicationDirPath = QCoreApplication::applicationDirPath();
    QString appPathPart = QStringLiteral("/QOwnNotes.app/Contents/MacOS");
    QString applicationsPath = QStringLiteral("/Applications");

    if (applicationDirPath.endsWith(appPathPart)) {
        applicationsPath =
            Utils::Misc::removeIfEndsWith(std::move(applicationDirPath), appPathPart);
    }

    if (Utils::Gui::question(this, tr("Proceed with update"),
                             tr("Do you want to update and restart QOwnNotes?"),
                             QStringLiteral("update-and-restart")) != QMessageBox::Yes) {
        return false;
    }

    // macOS only opens command files in a terminal
    QString updaterPath = QStringLiteral(":/scripts/macos/update.command");

    // just for debugging
    //    updaterPath = "/Users/omega/Code/QOwnNotes/travis/osx/update.command";

    QFile f(updaterPath);

    if (!f.exists()) {
        QMessageBox::critical(nullptr, tr("Error"),
                              tr("Could not find updater file:\n%1").arg(f.fileName()));
        return false;
    }

    // read the content of the updater script
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qWarning() << "Failed to open file:" << f.fileName();
        return false;
    }

    QTextStream ts(&f);
    QString scriptContent = ts.readAll();
    f.close();
    scriptContent.replace(QLatin1String("\"$QOWNNOTES_RELEASE_URL\""), "\"" + releaseUrl + "\"");
    scriptContent.replace(QLatin1String("\"$QOWNNOTES_APPLICATIONS_PATH\""),
                          "\"" + QDir::toNativeSeparators(applicationsPath) + "\"");

    // we need a temporary script file with hardcoded parameters because some
    // users were not able to get parameters via the system environment
    QTemporaryFile *tempFile =
        new QTemporaryFile(QDir::tempPath() + "/QOwnNotes-Updater-XXXXXX.command");

    // we want to keep the file to be used in the update process
    tempFile->setAutoRemove(false);

    // get a temporary file
    if (!tempFile->open()) {
        QMessageBox::critical(
            0, tr("File error"),
            tr("Could not open temporary file:\n%1").arg(tempFile->errorString()));
        return false;
    }

    // write the script content
    tempFile->write(scriptContent.toLatin1());

    // setting executable permissions to the updater script
    tempFile->setPermissions(QFile::ExeUser | QFile::ReadUser | QFile::WriteUser);

    // file->fileName() only holds a value after file->open()
    QString updaterFilePath = tempFile->fileName();
    tempFile->close();
    // we need to delete the object so the later written file
    // can be read again while the app is open
    tempFile->deleteLater();
    destroy(tempFile);

    // this is needed so deleteLater() really deletes the object
    QCoreApplication::processEvents();

    //    // we have to pass the parameters via environment because Qt
    //    // or macOS wasn't able to pass them in any other way
    //    qputenv("QOWNNOTES_RELEASE_URL", releaseUrl.toLatin1());
    //    qputenv("QOWNNOTES_APPLICATIONS_PATH",
    //            QDir::toNativeSeparators(applicationsPath).toLatin1());
    //
    //    // start updater script
    //    Utils::Misc::startDetachedProcess(updaterPath);

    qDebug() << __func__ << " - 'updaterFilePath': " << updaterFilePath;

    // start updater script
    Utils::Misc::startDetachedProcess(updaterFilePath);

    qApp->quit();
    return true;
}

/**
 * Initializes the Linux update process
 */
bool UpdateDialog::initializeLinuxUpdateProcess(const QString &filePath) {
    const QString appPath = Utils::Misc::applicationPath();

    qDebug() << __func__ << " - 'filePath': " << filePath;
    qDebug() << __func__ << " - 'appPath': " << appPath;

    QFile file(appPath);
    QFileInfo fileInfo(appPath);

    if (!fileInfo.isWritable()) {
        qCritical() << __func__ << " - 'appPath' is not writable: " << appPath;

        QMessageBox::critical(nullptr, tr("Permission error"),
                              tr("Your QOwnNotes executable '%1' is not writable! It must be "
                                 "writable by the current user in order to be updated.")
                                  .arg(appPath));

        return false;
    }

    QFile updateFile(filePath);

    // make the new AppImage executable
    if (!updateFile.setPermissions(updateFile.permissions() | QFileDevice::ExeOwner)) {
        qCritical() << __func__ << " - 'updateFile' could not be made executable: " << filePath;

        QMessageBox::critical(nullptr, tr("Permission error"),
                              tr("The temporary file '%1' could not be made executable! "
                                 "You need to replace '%2' yourself.")
                                  .arg(filePath, appPath));

        return false;
    }

    // remove the current binary
    if (!file.remove()) {
        qCritical() << __func__ << " - 'file' could not be removed: " << appPath;

        QMessageBox::critical(nullptr, tr("File error"),
                              tr("Your old QOwnNotes executable '%1' could not be removed! "
                                 "You need to replace it yourself with '%2'.")
                                  .arg(appPath, filePath));

        return false;
    }

    // rename the new AppImage to the path of the current binary
    if (!updateFile.rename(appPath)) {
        qCritical() << __func__ << " - 'updateFile' could not be renamed: " << filePath;

        QMessageBox::critical(nullptr, tr("File error"),
                              tr("Your old QOwnNotes executable '%1' could not be replaced "
                                 "by the new file '%2'! You need to replace it yourself.")
                                  .arg(appPath, filePath));

        return false;
    }

    QFile appFile(appPath);

    // make the new AppImage executable (2nd attempt)
    if (!appFile.setPermissions(appFile.permissions() | QFileDevice::ExeOwner)) {
        qCritical() << __func__
                    << " - 'appFile' could not be made executable after update: " << appPath;

        QMessageBox::critical(nullptr, tr("Permission error"),
                              tr("The app file '%1' could not be made executable! "
                                 "You need to make it executable yourself.")
                                  .arg(appPath));

        return false;
    }

    if (QMessageBox::information(
            this, tr("Restart application"),
            tr("You now can restart the application to complete the update process.") +
                Utils::Misc::appendSingleAppInstanceTextIfNeeded(),
            tr("Restart"), tr("Cancel"), QString(), 0, 1) == 0) {
        Utils::Misc::restartApplication();
    }

    // we want to close the update dialog after the last information
    return false;
}

/**
 * Initializes the Windows update process
 */
bool UpdateDialog::initializeWindowsUpdateProcess(const QString &filePath) {
    // get the folder path from the file path
    int lastPoint = filePath.lastIndexOf(QLatin1String("."));
    QString pathPrefix = filePath.left(lastPoint);
    QString updaterPath = pathPrefix + "-updater.bat";
    QString unzipPath = pathPrefix + "-unzip.exe";

    QFile updaterSourceFile(QCoreApplication::applicationDirPath() + "/update.bat");
    QFile unzipSourceFile(QCoreApplication::applicationDirPath() + "/unzip.exe");

    // check if updater script exists
    if (!updaterSourceFile.exists()) {
        QMessageBox::critical(
            0, tr("Error"),
            tr("Updater script '%1' doesn't exist!").arg(updaterSourceFile.fileName()));
        return false;
    }

    // check if unzip executable exists
    if (!unzipSourceFile.exists()) {
        QMessageBox::critical(
            0, tr("Error"),
            tr("Updater unzip executable '%1' doesn't exist!").arg(unzipSourceFile.fileName()));
        return false;
    }

    // copy updater script
    if (!updaterSourceFile.copy(updaterPath)) {
        QMessageBox::critical(nullptr, tr("Error"),
                              tr("Could not copy updater script '%1' to '%2'!")
                                  .arg(updaterSourceFile.fileName(), unzipPath));
        return false;
    }

    // copy unzip executable script
    if (!unzipSourceFile.copy(unzipPath)) {
        QMessageBox::critical(nullptr, tr("Error"),
                              tr("Could not copy updater script '%1' to '%2'!")
                                  .arg(updaterSourceFile.fileName(), unzipPath));
        return false;
    }

    // check if updater script exists in temporary folder
    QFile updaterFile(updaterPath);
    if (!updaterFile.exists()) {
        QMessageBox::critical(nullptr, tr("Error"),
                              tr("Couldn't find updater script: %1").arg(updaterPath));
        return false;
    }

    // check if unzip executable exists in temporary folder
    QFile unzipFile(unzipPath);
    if (!unzipFile.exists()) {
        QMessageBox::critical(nullptr, tr("Error"),
                              tr("Couldn't find unzip executable: %1").arg(unzipPath));
        return false;
    }

    if (Utils::Gui::question(this, tr("Proceed with update"),
                             tr("The download is now finished. Do you want to update and "
                                "restart QOwnNotes?"),
                             QStringLiteral("update-and-restart")) != QMessageBox::Yes) {
        return false;
    }

    QStringList parameters(QStringList()
                           << QDir::toNativeSeparators(filePath)
                           << QDir::toNativeSeparators(QCoreApplication::applicationDirPath())
                           << QDir::toNativeSeparators(unzipPath));

    if (Utils::Misc::isInPortableMode()) {
        parameters << QStringLiteral("--portable");
    }

    qDebug() << __func__ << " - 'parameters': " << parameters;

    // start updater script
    Utils::Misc::startDetachedProcess(updaterPath, parameters);

    qApp->quit();
    return true;
}

/**
 * Checks if the update dialog is open
 */
bool UpdateDialog::isUpdateDialogOpen() { return qApp->property("isUpdateDialogOpen").toBool(); }

/**
 * Sets if the update dialog is open
 */
void UpdateDialog::setIsUpdateDialogOpen(bool isOpen) {
    qApp->setProperty("isUpdateDialogOpen", QVariant::fromValue<bool>(isOpen));
}
