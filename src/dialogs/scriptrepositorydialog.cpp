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
#include <QtMath>
#include <QtNetwork/QNetworkRequest>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSplitter>

#include "services/scriptingservice.h"
#include "services/settingsservice.h"
#include "ui_scriptrepositorydialog.h"

ScriptRepositoryDialog::ScriptRepositoryDialog(QWidget *parent, bool checkForUpdates)
    : MasterDialog(parent), ui(new Ui::ScriptRepositoryDialog) {
    ui->setupUi(this);
    afterSetupUI();
    setupMainSplitter();

    _codeSearchUrl = QLatin1String("https://api.github.com/search/code");
    _rawContentUrlPrefix = Script::ScriptRepositoryRawContentUrlPrefix;
    _checkForUpdates = checkForUpdates;
    _searchString.clear();
    _lastInstalledScript = Script();
    // Set in the past so that the cache is always expired
    _lastScriptMetaDataCacheUpdateTime = QDateTime::currentDateTime().addDays(-1);

    ui->searchScriptEdit->setFocus();
    ui->scriptTreeWidget->sortByColumn(0, Qt::AscendingOrder);
    enableOverview(true);

    if (!checkForUpdates) {
        searchScript();
    }
}

ScriptRepositoryDialog::~ScriptRepositoryDialog() {
    storeSettings();
    delete ui;
}

bool ScriptRepositoryDialog::isScriptCacheExpired() {
    return _lastScriptMetaDataCacheUpdateTime.secsTo(QDateTime::currentDateTime()) > 60;
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

bool ScriptRepositoryDialog::loadScriptRepositoryMetaData() {
    if (!isScriptCacheExpired()) {
        return false;
    }

    _lastScriptMetaDataCacheUpdateTime = QDateTime::currentDateTime();
    const auto url =
        QUrl("https://github.com/qownnotes/scripts/releases/download/metadata-index/index.json");

    int statusCode;
    // This should have a 10-second timeout
    auto arr = Utils::Misc::downloadUrlWithStatusCode(url, statusCode);

    if (statusCode != 200) {
        qCritical() << __func__ << "Error (" << statusCode << "): " << arr;
        return false;
    }

    parseScriptRepositoryMetaData(arr);

    return true;
}

/**
 * Searches for script in the script repository
 */
void ScriptRepositoryDialog::searchScript() {
    _searchString = ui->searchScriptEdit->text();
    loadScriptRepositoryMetaData();

    QHashIterator<QString, ScriptInfoJson> scriptMetaDataIterator(_scriptMetaDataCache);
    ui->scriptTreeWidget->clear();
    while (scriptMetaDataIterator.hasNext()) {
        scriptMetaDataIterator.next();

        auto scriptInfoJson = scriptMetaDataIterator.value();

        if (scriptInfoJson.name.contains(_searchString, Qt::CaseInsensitive) ||
            scriptInfoJson.description.contains(_searchString, Qt::CaseInsensitive)) {
            addScriptTreeWidgetItem(scriptInfoJson);
        }
    }

    ui->scriptTreeWidget->setCurrentItem(ui->scriptTreeWidget->topLevelItem(0));
}

void ScriptRepositoryDialog::addScriptTreeWidgetItem(const ScriptInfoJson &scriptInfoJson) {
    // Check if script item already exists in tree widget
    if (Utils::Gui::userDataInTreeWidgetExists(ui->scriptTreeWidget, scriptInfoJson.identifier)) {
        return;
    }

    QString name = scriptInfoJson.name;

    // We can't use `std::unique_ptr<QTreeWidgetItem> item(new QTreeWidgetItem())` anymore, because
    // of the use of `QMetaObject::invokeMethod()`
    auto item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setData(0, Qt::UserRole, scriptInfoJson.identifier);

    if (!scriptInfoJson.platformSupported || !scriptInfoJson.appVersionSupported) {
        item->setForeground(0, QColor("#aaaaaa"));
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    // Each of these two lines "addTopLevelItem" and "resizeColumnToContents" result in this warning
    // when the method was called from another thread: `QBasicTimer::start: Timers cannot be started
    // from another thread` So we need to use QMetaObject::invokeMethod()
    QMetaObject::invokeMethod(
        ui->scriptTreeWidget,
        [this, item]() {
            ui->scriptTreeWidget->addTopLevelItem(item);
            ui->scriptTreeWidget->resizeColumnToContents(0);
        },
        Qt::QueuedConnection);
#else
    ui->scriptTreeWidget->addTopLevelItem(item);
    ui->scriptTreeWidget->resizeColumnToContents(0);
#endif
}

void ScriptRepositoryDialog::searchForUpdatesForScripts(const QList<Script> &scripts) {
    ui->searchScriptEdit->hide();
    setWindowTitle(tr("Script updates"));
    ui->overviewLabel->setText(tr("All scripts are up-to-date."));

    ui->selectFrame->hide();
    ui->scriptTreeWidget->clear();
    enableOverview(true);
    loadScriptRepositoryMetaData();
    bool scriptUpdateFound = false;

    Q_FOREACH (Script script, scripts) {
        if (!script.isScriptFromRepository()) {
            continue;
        }

        // May not show up in the log dialog when run in another thread
        qDebug() << "Checking for script update: " << script.remoteScriptUrl();

        auto infoJson = _scriptMetaDataCache.value(script.getIdentifier());
        VersionNumber remoteVersion = VersionNumber(infoJson.version);

        ScriptInfoJson scriptInfoJson = script.getScriptInfoJson();
        VersionNumber localVersion = VersionNumber(scriptInfoJson.version);

        if (localVersion >= remoteVersion) {
            continue;
        }

        if (!scriptUpdateFound) {
            emit updateFound();
        }

        scriptUpdateFound = true;
        addScriptTreeWidgetItem(scriptInfoJson);
        ui->selectFrame->show();
    }

    if (!scriptUpdateFound) {
        emit noUpdateFound();
    }
}

/**
 * Searches for script updates
 */
void ScriptRepositoryDialog::searchForUpdates() { searchForUpdatesForScripts(Script::fetchAll()); }

void ScriptRepositoryDialog::parseScriptRepositoryMetaData(const QByteArray &arr) {
    QJsonDocument doc = QJsonDocument::fromJson(arr);
    QJsonArray jsonArray = doc.array();
    _scriptMetaDataCache.clear();
    _scriptMetaDataJsonTextCache.clear();
    qDebug() << "Found scripts: " + QString::number(jsonArray.count());

    for (const auto &value : jsonArray) {
        ScriptInfoJson infoJson(value.toObject());

        //        qDebug() << "Found script: " + infoJson.identifier;

        // We are ignoring the example-script
        if (infoJson.identifier == QLatin1String("example-script")) {
            continue;
        }

        // Debug
        //        infoJson.version = "1.0.8";

        // We store the ScriptInfoJson in a cache for the user interface
        _scriptMetaDataCache.insert(infoJson.identifier, infoJson);

        // We need to store the json text for the scripts that are stored in the database
        QJsonDocument valueDoc(value.toObject());
        QByteArray docByteArray = valueDoc.toJson(QJsonDocument::Compact);
        QString strJson = QLatin1String(docByteArray);

        // Debug
        //        strJson.replace(QLatin1String(R"("version":"0.0.5")"),
        //        QLatin1String(R"("version":"1.0.8")"));
        //        strJson.replace(QLatin1String(R"("version":"0.0.4")"),
        //        QLatin1String(R"("version":"1.0.8")"));
        //        strJson.replace(QLatin1String(R"("version":"1.0.5")"),
        //        QLatin1String(R"("version":"1.0.8")"));

        _scriptMetaDataJsonTextCache.insert(infoJson.identifier, strJson);
    }

    _lastScriptMetaDataCacheUpdateTime = QDateTime::currentDateTime();
}

/**
 * Sets up the main splitter
 */
void ScriptRepositoryDialog::setupMainSplitter() {
    _mainSplitter = new QSplitter(this);

    _mainSplitter->addWidget(ui->selectFrame);
    _mainSplitter->addWidget(ui->infoFrame);

    // restore splitter sizes
    SettingsService settings;
    QByteArray state =
        settings.value(QStringLiteral("ScriptRepositoryDialog/mainSplitterState")).toByteArray();
    _mainSplitter->restoreState(state);

    ui->gridLayout->layout()->addWidget(_mainSplitter);
}

/**
 * Stores the settings
 */
void ScriptRepositoryDialog::storeSettings() {
    SettingsService settings;
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
    auto infoJson = getCurrentScriptInfoJson();

    if (infoJson.isEmpty()) {
        return;
    }

    enableOverview(false);

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
ScriptInfoJson ScriptRepositoryDialog::getCurrentScriptInfoJson() {
    QTreeWidgetItem *item = ui->scriptTreeWidget->currentItem();

    if (item == nullptr) {
        return ScriptInfoJson();
    }

    QString identifier = item->data(0, Qt::UserRole).toString();

    return _scriptMetaDataCache.value(identifier);
}

/**
 * Returns the json string of the currently selected script
 *
 * @return
 */
QString ScriptRepositoryDialog::getCurrentInfoJsonString() {
    QTreeWidgetItem *item = ui->scriptTreeWidget->currentItem();

    if (item == nullptr) {
        return {};
    }

    QString identifier = item->data(0, Qt::UserRole).toString();

    return _scriptMetaDataJsonTextCache.value(identifier);
}

/**
 * Installs the currently selected script
 */
void ScriptRepositoryDialog::on_installButton_clicked() {
    auto infoJson = getCurrentScriptInfoJson();

    if (infoJson.isEmpty()) {
        return;
    }

    QString identifier = infoJson.identifier;
    QString name = infoJson.name;
    QString scriptName = infoJson.script;

    // check if platform is supported
    if (!infoJson.platformSupported &&
        QMessageBox::question(this, tr("Platform not supported!"),
                              tr("Your platform is not supported by this script!\n"
                                 "Do you want to install it anyway?"),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No) == QMessageBox::No) {
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
    script.setInfoJson(_scriptMetaDataJsonTextCache.value(identifier));

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
        ScriptInfoJson scriptInfoJson = script.getScriptInfoJson();
        foreach (QString resourceFileName, scriptInfoJson.resources) {
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
    Q_UNUSED(arg1)

    if (!_checkForUpdates) {
        searchScript();
    }
}

Script ScriptRepositoryDialog::getLastInstalledScript() { return _lastInstalledScript; }

void ScriptRepositoryDialog::checkForScriptUpdates(QWidget *parent) {
    auto *dialog = new ScriptRepositoryDialog(parent, true);
    dialog->searchForUpdates();
    dialog->exec();
    delete (dialog);

    // Reload the scripting engine
    ScriptingService::instance()->reloadEngine();
}
