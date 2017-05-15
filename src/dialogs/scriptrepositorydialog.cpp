#include <QtNetwork/QNetworkRequest>
#include <QtWidgets/QSplitter>
#include <QtCore/QSettings>
#include <QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
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
    ui->downloadProgressBar->hide();
}

ScriptRepositoryDialog::~ScriptRepositoryDialog()
{
    storeSettings();
    delete ui;
}

/**
 * Searches for script in the script repository
 */
void ScriptRepositoryDialog::searchScript()
{
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

    QNetworkReply *reply = _networkManager->get(networkRequest);

    ui->downloadProgressBar->show();
    ui->downloadProgressBar->reset();
}

/**
 * Does a script search
 */
void ScriptRepositoryDialog::on_searchScriptEdit_returnPressed() {
    searchScript();
}

void ScriptRepositoryDialog::slotReplyFinished(QNetworkReply *reply) {
    QUrl url = reply->url();
    QString urlPath = url.path();

    qDebug() << "Reply from " << urlPath;

    if (urlPath.endsWith("/search/code")) {
        QByteArray arr = reply->readAll();
        qDebug() << "Reply from code search";

        parseCodeSearchReply(arr);
    }
}

/**
 * Parses the reply from the code search and start the requests to fetch the
 * queries for the results
 *
 * @param arr
 */
void ScriptRepositoryDialog::parseCodeSearchReply(const QByteArray &arr) const {
    QJsonDocument jsonResponse = QJsonDocument::fromJson(arr);
    QJsonObject jsonObject = jsonResponse.object();
    QJsonArray items = jsonObject.value("items").toArray();
    ui->scriptTreeWidget->clear();

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

            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, identifier);
            ui->scriptTreeWidget->addTopLevelItem(item);
        }

    ui->downloadProgressBar->hide();
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
    QByteArray state =
            settings.value("ScriptRepositoryDialog/mainSplitterState").toByteArray();
    _mainSplitter->restoreState(state);

    ui->gridLayout->layout()->addWidget(_mainSplitter);
}

void ScriptRepositoryDialog::storeSettings() {
    QSettings settings;
    settings.setValue("ScriptRepositoryDialog/geometry", saveGeometry());
    settings.setValue("ScriptRepositoryDialog/mainSplitterState",
                      _mainSplitter->saveState());
}
