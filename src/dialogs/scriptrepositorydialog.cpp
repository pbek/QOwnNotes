#include "scriptrepositorydialog.h"

#include <libraries/versionnumber/versionnumber.h>
#include <services/metricsservice.h>
#include <utils/gui.h>
#include <utils/misc.h>

#include <QDebug>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QScrollBar>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QSettings>
#include <QtMath>
#include <QtNetwork/QNetworkRequest>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSplitter>

#include "ui_scriptrepositorydialog.h"

ScriptRepositoryDialog::ScriptRepositoryDialog(QWidget *parent, bool checkForUpdates)
    : MasterDialog(parent), ui(new Ui::ScriptRepositoryDialog) {
    ui->setupUi(this);
    setupMainSplitter();

    _networkManager = new QNetworkAccessManager(this);
    QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply *)), this,
                     SLOT(slotReplyFinished(QNetworkReply *)));

    _codeSearchUrl = QLatin1String("https://api.github.com/search/code");
    _rawContentUrlPrefix = Script::ScriptRepositoryRawContentUrlPrefix;
    _checkForUpdates = checkForUpdates;
    _searchString.clear();
    _page = 1;
    _totalCount = 0;
    _lastInstalledScript = Script();

    ui->downloadProgressBar->hide();
    ui->loadMoreScriptsButton->hide();
    ui->searchScriptEdit->setFocus();
    ui->scriptTreeWidget->sortByColumn(0, Qt::AscendingOrder);
    enableOverview(true);

    if (checkForUpdates) {
        ui->searchScriptEdit->hide();
        setWindowTitle(tr("Script updates"));
        ui->overviewLabel->setText(tr("All scripts are up-to-date."));
        searchForUpdates();
    } else {
        QObject::connect(ui->scriptTreeWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
                         SLOT(scriptTreeWidgetSliderValueChanged(int)));

        searchScript();
    }
}

ScriptRepositoryDialog::~ScriptRepositoryDialog() {
    storeSettings();
    delete ui;
}

/**
 * Moves the note view scrollbar when the note edit scrollbar was moved
 */
void ScriptRepositoryDialog::scriptTreeWidgetSliderValueChanged(int value) {
    if (ui->scriptTreeWidget->verticalScrollBar()->maximum() == value) {
        loadMoreItems();
    }
}

/**
 * Attempts to load more items
 */
void ScriptRepositoryDialog::loadMoreItems() {
    if (hasMoreItems()) {
        searchScript(_page + 1);
    }
}

/**
 * Checks if there are more items to load
 *
 * @return
 */
bool ScriptRepositoryDialog::hasMoreItems() const {
    bool hasMoreItems = qCeil((qreal)_totalCount / _itemsPerPage) > _page;
    return hasMoreItems;
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
void ScriptRepositoryDialog::searchScript(int page) {
    if (page == 1) {
        _searchString = ui->searchScriptEdit->text();
    }

    QString query = QUrl::toPercentEncoding(_searchString);
    QUrl url(_codeSearchUrl + "?q=" + query +
             "+in:file+language:json"
             "+repo:qownnotes/scripts&page=" +
             QString::number(page));
    QNetworkRequest networkRequest(url);
    _page = page;

#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
#else
    networkRequest.setAttribute(QNetworkRequest::RedirectPolicyAttribute, true);
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // try to ensure the network is accessible
    _networkManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
#endif

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

    Q_FOREACH (Script script, Script::fetchAll()) {
        if (!script.isScriptFromRepository()) {
            continue;
        }

        QUrl url = script.repositoryInfoJsonUrl();
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

        _networkManager->get(networkRequest);
    }
}

/**
 * Does a script search
 */
void ScriptRepositoryDialog::on_searchScriptEdit_returnPressed() { searchScript(); }

/**
 * Handles all the network replies
 * @param reply
 */
void ScriptRepositoryDialog::slotReplyFinished(QNetworkReply *reply) {
    QUrl url = reply->url();
    QString urlPath = url.path();

    qDebug() << "Reply from " << urlPath;

    if (urlPath.endsWith(QLatin1String("/search/code"))) {
        QByteArray arr = reply->readAll();
        qDebug() << "Reply from code search";

        parseCodeSearchReply(arr);
    } else if (urlPath.startsWith(QLatin1String("/qownnotes/scripts/master"))) {
        QByteArray arr = reply->readAll();
        qDebug() << "Reply from info.qml request";

        parseInfoQMLReply(arr);
    }

    reply->deleteLater();
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
    _totalCount = jsonObject.value(QStringLiteral("total_count")).toInt();
    QJsonArray items = jsonObject.value(QStringLiteral("items")).toArray();
    ui->loadMoreScriptsButton->setVisible(hasMoreItems());

    if (_page == 1) {
        ui->scriptTreeWidget->clear();
    }

    enableOverview(_page == 1);

    foreach (const QJsonValue &value, items) {
        QJsonObject obj = value.toObject();
        QString path = obj[QStringLiteral("path")].toString();
        qDebug() << __func__ << " - 'path': " << path;

        QRegularExpressionMatch match =
            QRegularExpression(QStringLiteral("(.+)\\/info\\.json")).match(path);

        if (!match.hasMatch()) {
            continue;
        }

        QString identifier = match.captured(1);
        qDebug() << "Found script: " + identifier;

        // we are ignoring the example-script
        if (identifier == QLatin1String("example-script")) {
            continue;
        }

        QUrl url(_rawContentUrlPrefix + path);
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

        _networkManager->get(networkRequest);
    }

    ui->downloadProgressBar->hide();
}

/**
 * Parses the reply from the info.qml request
 *
 * @param arr
 */
void ScriptRepositoryDialog::parseInfoQMLReply(const QByteArray &arr) {
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

        emit updateFound();

        ui->selectFrame->show();
    }

    QString jsonData = QString(arr);

    // check if script item already exists in tree widget
    if (Utils::Gui::userDataInTreeWidgetExists(ui->scriptTreeWidget, jsonData)) {
        return;
    }

    QString name = infoJson.name;

    auto *item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setData(0, Qt::UserRole, jsonData);

    if (!infoJson.platformSupported || !infoJson.appVersionSupported) {
        item->setForeground(0, QColor("#aaaaaa"));
    }

    ui->scriptTreeWidget->addTopLevelItem(item);
    ui->scriptTreeWidget->resizeColumnToContents(0);

    if (_page == 1) {
        ui->scriptTreeWidget->setCurrentItem(ui->scriptTreeWidget->topLevelItem(0));
    }
}

/**
 * Sets up the main splitter
 */
void ScriptRepositoryDialog::setupMainSplitter() {
    _mainSplitter = new QSplitter(this);

    _mainSplitter->addWidget(ui->selectFrame);
    _mainSplitter->addWidget(ui->infoFrame);

    // restore splitter sizes
    QSettings settings;
    QByteArray state =
        settings.value(QStringLiteral("ScriptRepositoryDialog/mainSplitterState")).toByteArray();
    _mainSplitter->restoreState(state);

    ui->gridLayout->layout()->addWidget(_mainSplitter);
}

/**
 * Stores the settings
 */
void ScriptRepositoryDialog::storeSettings() {
    QSettings settings;
    settings.setValue(QStringLiteral("ScriptRepositoryDialog/mainSplitterState"),
                      _mainSplitter->saveState());
}

/**
 * Shows the currently selected script
 *
 * @param current
 * @param previous
 */
void ScriptRepositoryDialog::on_scriptTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                                    QTreeWidgetItem *previous) {
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
    ui->minAppVersionHeadlineLabel->setVisible(ui->minAppVersionLabel->isVisible());
    ui->descriptionLabel->setText(infoJson.description);
    ui->authorLabel->setText(infoJson.richAuthorText);
    ui->authorHeadlineLabel->setText(
        (infoJson.richAuthorList.count() > 1 ? tr("Authors") : tr("Author")) + ":");
    ui->platformLabel->setText(infoJson.richPlatformText);
    ui->platformHeadlineLabel->setText(
        (infoJson.platformList.count() > 1 ? tr("Supported platforms") : tr("Supported platform")) +
        ":");
    ui->repositoryLinkLabel->setText("<a href=\"https://github.com/qownnotes/scripts/tree/master/" +
                                     infoJson.identifier + "\">" + tr("Open repository") + "</a>");

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
    QJsonDocument jsonResponse = QJsonDocument::fromJson(getCurrentInfoJsonString().toUtf8());
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

    if (item == nullptr) {
        return QString();
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

    QString identifier = jsonObject.value(QStringLiteral("identifier")).toString();

    if (identifier.isEmpty()) {
        return;
    }

    ScriptInfoJson infoJson(jsonObject);
    QString name = infoJson.name;
    QString scriptName = infoJson.script;

    // check if platform is supported
    if (!infoJson.platformSupported &&
        QMessageBox::information(this, tr("Platform not supported!"),
                                 tr("Your platform is not supported by this script!\n"
                                    "Do you want to install it anyway?"),
                                 tr("Install"), tr("Cancel"), QString(), 0, 1) != 0) {
        return;
    }

    // check if app version is supported
    if (!infoJson.appVersionSupported) {
        QMessageBox::information(this, tr("Update app"),
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
        auto *file = new QFile(scriptPath);
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

            auto *file = new QFile(scriptRepositoryPath + "/" + resourceFileName);

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
        MetricsService::instance()->sendVisitIfEnabled("script-repository/install/" + identifier);
        reloadCurrentScriptInfo();
        _lastInstalledScript = script;

        Utils::Gui::information(this, tr("Install successful"),
                                tr("The script was successfully installed!"),
                                QStringLiteral("script-install-successful"));

        if (_checkForUpdates) {
            searchForUpdates();
        }
    } else {
        QMessageBox::warning(this, tr("Download failed"),
                             tr("The script could not be downloaded!"));
    }
}

void ScriptRepositoryDialog::on_searchScriptEdit_textChanged(const QString &arg1) {
    // list all scripts again if the search bar was cleared
    if (!_checkForUpdates && arg1.isEmpty()) {
        searchScript();
    }
}

void ScriptRepositoryDialog::on_loadMoreScriptsButton_clicked() { loadMoreItems(); }

Script ScriptRepositoryDialog::getLastInstalledScript() { return _lastInstalledScript; }
