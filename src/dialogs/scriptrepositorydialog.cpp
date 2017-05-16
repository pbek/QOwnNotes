#include <QtNetwork/QNetworkRequest>
#include <QtWidgets/QSplitter>
#include <QtCore/QSettings>
#include <QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <entities/script.h>
#include "scriptrepositorydialog.h"
#include "ui_scriptrepositorydialog.h"

ScriptRepositoryDialog::ScriptRepositoryDialog(QWidget *parent) :
    MasterDialog(parent),
    ui(new Ui::ScriptRepositoryDialog) {
    ui->setupUi(this);
    setupMainSplitter();

    _networkManager = new QNetworkAccessManager();
    QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply *)),
                     this, SLOT(slotReplyFinished(QNetworkReply *)));

    _codeSearchUrl = "https://api.github.com/search/code";
    _rawContentUrlPrefix = "https://raw.githubusercontent.com/qownnotes/"
            "scripts/master/";

    ui->downloadProgressBar->hide();
    ui->searchScriptEdit->setFocus();
    enableOverview(true);
    searchScript();
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
    QString name = jsonObject.value("name").toString();

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setData(0, Qt::UserRole, QString(arr));
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

    QJsonObject jsonObject = getCurrentInfoJsonObject();

    if (jsonObject.isEmpty()) {
        return;
    }

    QString name = jsonObject.value("name").toString();
    QString identifier = jsonObject.value("identifier").toString();
    QString version = jsonObject.value("version").toString();
    QString description = jsonObject.value("description").toString();
    QString script = jsonObject.value("script").toString();

    enableOverview(false);
    ui->nameLabel->setText("<b>" + name + "</b>");
    ui->versionLabel->setText(version);
    ui->descriptionLabel->setText(description);
    ui->repositoryLinkLabel->setText(
            "<a href=\"https://github.com/qownnotes/scripts/tree/master/" +
                    identifier + "\">" + tr("Open repository") + "</a>");
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
 * Installs a script
 *
 * @param identifier
 */
void ScriptRepositoryDialog::installScript(QString identifier) {
    qDebug() << __func__ << " - 'identifier': " << identifier;

    if (identifier.isEmpty()) {
        return;
    }

    Script *script = new Script();
    script->setIdentifier(identifier);
    // TODO(pbek): remove old script folder
    // TODO(pbek): create new script folder
    // TODO(pbek): download script
    // TODO(pbek): create script in database
}

/**
 * Installs the currently selected script
 */
void ScriptRepositoryDialog::on_installButton_clicked()
{
    QJsonObject jsonObject = getCurrentInfoJsonObject();

    if (jsonObject.isEmpty()) {
        return;
    }

    QString identifier = jsonObject.value("identifier").toString();

    if (identifier.isEmpty()) {
        return;
    }

    QString name = jsonObject.value("name").toString();
    QString scriptName = jsonObject.value("script").toString();

    Script *script = new Script();
    script->setIdentifier(identifier);
    script->setName(name);
    script->setScriptPath(scriptName);
    script->setInfoJson(getCurrentInfoJsonString());
    script->store();
}
