#include <QtNetwork/QNetworkRequest>
#include <QtWidgets/QSplitter>
#include <QtCore/QSettings>
#include <QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <entities/script.h>
#include <QtCore/QFile>
#include <utils/misc.h>
#include <QtWidgets/QMessageBox>
#include <services/metricsservice.h>
#include <libraries/versionnumber/versionnumber.h>
#include <utils/gui.h>
#include "scriptrepositorydialog.h"
#include "ui_scriptrepositorydialog.h"

ScriptRepositoryDialog::ScriptRepositoryDialog(QWidget *parent,
                                               bool checkForUpdates) :
    MasterDialog(parent),
    ui(new Ui::ScriptRepositoryDialog) {
    ui->setupUi(this);
    setupMainSplitter();

    _networkManager = new QNetworkAccessManager();
    QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply *)),
                     this, SLOT(slotReplyFinished(QNetworkReply *)));

    _codeSearchUrl = "https://api.github.com/search/code";
    _rawContentUrlPrefix = Script::ScriptRepositoryRawContentUrlPrefix;
    _checkForUpdates = checkForUpdates;

    ui->downloadProgressBar->hide();
    ui->searchScriptEdit->setFocus();
    ui->scriptTreeWidget->sortByColumn(0, Qt::AscendingOrder);
    enableOverview(true);

    if (checkForUpdates) {
        ui->searchScriptEdit->hide();
        setWindowTitle(tr("Script updates"));
        ui->overviewLabel->setText(tr("All scripts are up-to-date."));
        searchForUpdates();
    } else {
        searchScript();
    }
}

ScriptRepositoryDialog::~ScriptRepositoryDialog() {
    storeSettings();
    delete ui;
}

/**
 * Enables or disables the overview
 *
 * @param enable
 */
void ScriptRepositoryDialog::enableOverview(bool enable) {
    ui->overviewFrame->setVisible(enable);
    ui->scriptInfoFrame->setVisible(!enable);
}

/**
 * Searches for script in the script repository
 */
void ScriptRepositoryDialog::searchScript() {
    QString query = QUrl::toPercentEncoding(ui->searchScriptEdit->text());
    QUrl url(_codeSearchUrl +"?q=" + query +
                     "+in:file+language:json+repo:qownnotes/scripts");
    QNetworkRequest networkRequest(url);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    networkRequest.setAttribute(
            QNetworkRequest::FollowRedirectsAttribute, true);
#endif

    // try to ensure the network is accessible
    _networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);

    _networkManager->get(networkRequest);

    ui->downloadProgressBar->show();
    ui->downloadProgressBar->reset();
}

/**
 * Searches for script updates
 */
void ScriptRepositoryDialog::searchForUpdates() {
    ui->selectFrame->hide();
    ui->scriptTreeWidget->clear();
    enableOverview(true);

    Q_FOREACH(Script script, Script::fetchAll()) {
            if (!script.isScriptFromRepository()) {
                continue;
            }

            QUrl url = script.repositoryInfoJsonUrl();
            QNetworkRequest networkRequest(url);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
            networkRequest.setAttribute(
                    QNetworkRequest::FollowRedirectsAttribute, true);
#endif

            // try to ensure the network is accessible
            _networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);

            _networkManager->get(networkRequest);

        }
}

/**
 * Does a script search
 */
void ScriptRepositoryDialog::on_searchScriptEdit_returnPressed() {
    searchScript();
}

/**
 * Handles all the network replies
 * @param reply
 */
void ScriptRepositoryDialog::slotReplyFinished(QNetworkReply *reply) {
    QUrl url = reply->url();
    QString urlPath = url.path();

    qDebug() << "Reply from " << urlPath;

    if (urlPath.endsWith("/search/code")) {
        QByteArray arr = reply->readAll();
        qDebug() << "Reply from code search";

        parseCodeSearchReply(arr);
    } else if (urlPath.startsWith("/qownnotes/scripts/master")) {
        QByteArray arr = reply->readAll();
        qDebug() << "Reply from info.qml request";

        parseInfoQMLReply(arr);
    }
}

/**
 * Parses the reply from the code search and start the requests to fetch the
 * queries for the results
 *
 * @param arr
 */
void ScriptRepositoryDialog::parseCodeSearchReply(const QByteArray &arr) {
    QJsonDocument jsonResponse = QJsonDocument::fromJson(arr);
    QJsonObject jsonObject = jsonResponse.object();
    QJsonArray items = jsonObject.value("items").toArray();
    ui->scriptTreeWidget->clear();
    enableOverview(true);

    foreach(const QJsonValue &value, items) {
            QJsonObject obj = value.toObject();
            QString path = obj["path"].toString();
            qDebug() << __func__ << " - 'path': " << path;

            QRegularExpressionMatch match =
                    QRegularExpression("(.+)\\/info\\.json").match(path);

            if (!match.hasMatch()) {
                continue;
            }

            QString identifier = match.captured(1);
            qDebug() << "Found script: " + identifier;

            // we are ignoring the example-script
            if (identifier == "example-script") {
                continue;
            }

            QUrl url(_rawContentUrlPrefix + path);
            QNetworkRequest networkRequest(url);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
            networkRequest.setAttribute(
                    QNetworkRequest::FollowRedirectsAttribute, true);
#endif

            // try to ensure the network is accessible
            _networkManager->setNetworkAccessible(
                    QNetworkAccessManager::Accessible);

            _networkManager->get(networkRequest);
        }

    ui->downloadProgressBar->hide();
}

/**
 * Parses the reply from the info.qml request
 *
 * @param arr
 */
void ScriptRepositoryDialog::parseInfoQMLReply(const QByteArray &arr) const {
    QJsonDocument jsonResponse = QJsonDocument::fromJson(arr);
    QJsonObject jsonObject = jsonResponse.object();
    ScriptInfoJson infoJson(jsonObject);

    // if we are doing an update check we only want to view scripts that
    // needs an update
    if (_checkForUpdates) {
        Script script = Script::fetchByIdentifier(infoJson.identifier);
        if (!script.isFetched()) {
            return;
        }
        VersionNumber remoteVersion = VersionNumber(infoJson.version);

        ScriptInfoJson scriptInfoJson = script.getScriptInfoJson();
        VersionNumber localVersion = VersionNumber(scriptInfoJson.version);

        if (localVersion >= remoteVersion) {
            return;
        }

        ui->selectFrame->show();
    }

    QString name = infoJson.name;

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setData(0, Qt::UserRole, QString(arr));

    if (!infoJson.platformSupported || !infoJson.appVersionSupported) {
        item->setTextColor(0, QColor("#aaaaaa"));
    }

    ui->scriptTreeWidget->addTopLevelItem(item);
    ui->scriptTreeWidget->resizeColumnToContents(0);
    ui->scriptTreeWidget->setCurrentItem(ui->scriptTreeWidget->topLevelItem(0));
}

/**
 * Sets up the main splitter
 */
void ScriptRepositoryDialog::setupMainSplitter() {
    _mainSplitter = new QSplitter;

    _mainSplitter->addWidget(ui->selectFrame);
    _mainSplitter->addWidget(ui->infoFrame);

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value(
            "ScriptRepositoryDialog/mainSplitterState").toByteArray();
    _mainSplitter->restoreState(state);

    ui->gridLayout->layout()->addWidget(_mainSplitter);
}

/**
 * Stores the settings
 */
void ScriptRepositoryDialog::storeSettings() {
    QSettings settings;
    settings.setValue("ScriptRepositoryDialog/mainSplitterState",
                      _mainSplitter->saveState());
}

/**
 * Shows the currently selected script
 *
 * @param current
 * @param previous
 */
void ScriptRepositoryDialog::on_scriptTreeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(current);
    Q_UNUSED(previous);

    reloadCurrentScriptInfo();
}

/**
 * Shows the currently selected script
 *
 * @param current
 * @param previous
 */
void ScriptRepositoryDialog::reloadCurrentScriptInfo() {
    QJsonObject jsonObject = getCurrentInfoJsonObject();

    if (jsonObject.isEmpty()) {
        return;
    }

    enableOverview(false);

    ScriptInfoJson infoJson(jsonObject);
    ui->nameLabel->setText("<b>" + infoJson.name + "</b>");
    ui->versionLabel->setText(infoJson.version);
    ui->minAppVersionLabel->setText(infoJson.minAppVersion);
    ui->minAppVersionLabel->setHidden(infoJson.minAppVersion.isEmpty());
    ui->minAppVersionHeadlineLabel->setVisible(
            ui->minAppVersionLabel->isVisible());
    ui->descriptionLabel->setText(infoJson.description);
    ui->authorLabel->setText(infoJson.richAuthorText);
    ui->authorHeadlineLabel->setText((infoJson.richAuthorList.count() > 1 ?
                                        tr("Authors") : tr("Author")) + ":");
    ui->platformLabel->setText(infoJson.richPlatformText);
    ui->platformHeadlineLabel->setText((infoJson.platformList.count() > 1 ?
                                        tr("Supported platforms") :
                                        tr("Supported platform")) + ":");
    ui->repositoryLinkLabel->setText(
            "<a href=\"https://github.com/qownnotes/scripts/tree/master/" +
                    infoJson.identifier + "\">" + tr("Open repository") +
                    "</a>");

    Script script = Script::fetchByIdentifier(infoJson.identifier);
    if (script.isFetched()) {
        VersionNumber remoteVersion = VersionNumber(infoJson.version);

        ScriptInfoJson scriptInfoJson = script.getScriptInfoJson();
        VersionNumber localVersion = VersionNumber(scriptInfoJson.version);

        if (localVersion < remoteVersion) {
            ui->installButton->setText(tr("Update"));
            ui->installButton->setToolTip(tr("Updates the script"));
        } else {
            ui->installButton->setText(tr("Reinstall"));
            ui->installButton->setToolTip(tr("Reinstalls the script"));
        }

        ui->currentlyInstalledVersionLabel->setText(scriptInfoJson.version);
        ui->currentlyInstalledVersionLabel->show();
        ui->currentlyInstalledVersionTextLabel->show();
    } else {
        ui->installButton->setText(tr("Install"));
        ui->installButton->setToolTip(tr("Installs the script"));
        ui->currentlyInstalledVersionLabel->hide();
        ui->currentlyInstalledVersionTextLabel->hide();
    }
}

/**
 * Returns the json object of the currently selected script
 *
 * @return
 */
QJsonObject ScriptRepositoryDialog::getCurrentInfoJsonObject() {
    QJsonDocument jsonResponse = QJsonDocument::fromJson(
            getCurrentInfoJsonString().toUtf8());
    QJsonObject jsonObject = jsonResponse.object();

    return jsonObject;
}

/**
 * Returns the json string of the currently selected script
 *
 * @return
 */
QString ScriptRepositoryDialog::getCurrentInfoJsonString() {
    QTreeWidgetItem *item = ui->scriptTreeWidget->currentItem();

    if (item == Q_NULLPTR) {
        return "";
    }

    QString data = item->data(0, Qt::UserRole).toString();
    return data;
}

/**
 * Installs the currently selected script
 */
void ScriptRepositoryDialog::on_installButton_clicked() {
    QJsonObject jsonObject = getCurrentInfoJsonObject();

    if (jsonObject.isEmpty()) {
        return;
    }

    QString identifier = jsonObject.value("identifier").toString();

    if (identifier.isEmpty()) {
        return;
    }

    ScriptInfoJson infoJson(jsonObject);
    QString name = infoJson.name;
    QString scriptName = infoJson.script;

    // check if platform is supported
    if (!infoJson.platformSupported && QMessageBox::information(
            this, tr("Platform not supported!"),
            tr("Your platform is not supported by this script!\n"
                       "Do you want to install it anyway?"),
            tr("Install"), tr("Cancel"), QString::null,
            0, 1) != 0) {
        return;
    }

    // check if app version is supported
    if (!infoJson.appVersionSupported) {
        QMessageBox::information(
                this, tr("Update app"),
                tr("Please don't forget to update your installation of "
                           "QOwnNotes to make this script work!"));
    }

    ui->installButton->setEnabled(false);

    // create or update the script in the database
    Script script = Script::fetchByIdentifier(identifier);

    if (!script.isFetched()) {
        script.setIdentifier(identifier);
    }

    script.setName(name);
    script.setInfoJson(getCurrentInfoJsonString());

    // this also creates the path after removing it
    QString scriptRepositoryPath = script.scriptRepositoryPath(true);

    if (scriptRepositoryPath.isEmpty()) {
        return;
    }

    QString scriptPath = scriptRepositoryPath + "/" + scriptName;
    script.setScriptPath(scriptPath);
    bool filesWereDownloaded = false;

    QUrl url = script.remoteScriptUrl();

    // download the script
    if (!url.isEmpty()) {
        QFile *file = new QFile(scriptPath);
        qDebug() << "Downloading: " << url;
        filesWereDownloaded = Utils::Misc::downloadUrlToFile(url, file);
        file->close();
    }

    // download resource files (if any)
    if (filesWereDownloaded) {
        ScriptInfoJson infoJson = script.getScriptInfoJson();
        foreach (QString resourceFileName, infoJson.resources) {
                QUrl resourceUrl = script.remoteFileUrl(resourceFileName);
                qDebug() << "Downloading: " << resourceUrl;

                QFile *file = new QFile(scriptRepositoryPath + "/" +
                                                resourceFileName);

                if (!Utils::Misc::downloadUrlToFile(resourceUrl, file)) {
                    filesWereDownloaded = false;
                }

                file->close();

                if (!filesWereDownloaded) {
                    break;
                }
            }
    }

    ui->installButton->setEnabled(true);

    if (filesWereDownloaded) {
        script.store();
        MetricsService::instance()->sendVisitIfEnabled(
                "script-repository/install/" + identifier);
        reloadCurrentScriptInfo();

        Utils::Gui::information(this, tr("Install successful"),
                                tr("The script was successfully installed!"),
                                "script-install-successful");

        if (_checkForUpdates) {
            searchForUpdates();
        }
    } else {
        QMessageBox::warning(this, tr("Download failed"),
                                 tr("The script could not be downloaded!"));
    }
}
