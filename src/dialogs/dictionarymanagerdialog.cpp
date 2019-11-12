#include <QNetworkRequest>
#include <QSplitter>
#include <QSettings>
#include <QFile>
#include <utils/misc.h>
#include <QMessageBox>
#include <utils/gui.h>
#include "dictionarymanagerdialog.h"
#include "ui_dictionarymanagerdialog.h"
#include <QDir>
#include <QFileInfo>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

DictionaryManagerDialog::DictionaryManagerDialog(QWidget *parent) :
    MasterDialog(parent),
    ui(new Ui::DictionaryManagerDialog) {
    ui->setupUi(this);
    setupMainSplitter();

    _networkManager = new QNetworkAccessManager(this);
    QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply *)),
                     this, SLOT(slotReplyFinished(QNetworkReply *)));

    // you can add more dictionaries from https://github.com/qownnotes/dictionaries/tree/master
    addDictionaryItem(tr("English (British)"), QStringLiteral("en"), QStringLiteral("en_GB"));
    addDictionaryItem(tr("English (American)"), QStringLiteral("en"), QStringLiteral("en_US"));
    addDictionaryItem(tr("German (Austrian)"), QStringLiteral("de"), QStringLiteral("de_AT_frami"));
    addDictionaryItem(tr("German (German)"), QStringLiteral("de"), QStringLiteral("de_DE_frami"));
    addDictionaryItem(tr("German (Swiss)"), QStringLiteral("de"), QStringLiteral("de_CH_frami"));
    addDictionaryItem(tr("French"), QStringLiteral("fr_FR"), QStringLiteral("fr"));
    addDictionaryItem(tr("Spanish"), QStringLiteral("es"), QStringLiteral("es_ANY"));
    addDictionaryItem(tr("Russian"), QStringLiteral("ru_RU"));
    addDictionaryItem(tr("Polish"), QStringLiteral("pl_PL"));
    addDictionaryItem(tr("Italian"), QStringLiteral("it_IT"));
    addDictionaryItem(tr("Portugese (Brazilian)"), QStringLiteral("pt_BR"));
    addDictionaryItem(tr("Portugese"), QStringLiteral("pt_PT"));
    addDictionaryItem(tr("Danish"), QStringLiteral("da_DK"));
    addDictionaryItem(tr("Swedish"), QStringLiteral("sv_SE"));
    addDictionaryItem(tr("Czech"), QStringLiteral("cs_CZ"));
    addDictionaryItem(tr("Urdu"), QStringLiteral("ur_PK"));

    ui->downloadButton->setDisabled(true);
    ui->deleteLocalDictionaryButton->setDisabled(true);
    ui->downloadFrame->hide();
    ui->searchDictionaryEdit->setFocus();
    ui->remoteDictionaryTreeWidget->sortByColumn(0, Qt::AscendingOrder);

    QSettings settings;
    const QSignalBlocker blocker(ui->disableExternalDictionariesCheckBox);
    Q_UNUSED(blocker)
    ui->disableExternalDictionariesCheckBox->setChecked(
                settings.value(QStringLiteral("disableExternalDictionaries")).toBool());

    loadLocalDictionaries();
    ui->localDictionaryTreeWidget->sortByColumn(0, Qt::AscendingOrder);
}

DictionaryManagerDialog::~DictionaryManagerDialog() {
    storeSettings();
    delete ui;
}

void DictionaryManagerDialog::addDictionaryItem(const QString &name, const QString &pathPart, QString fileNamePart) {
    if (fileNamePart.isEmpty()) {
        fileNamePart = pathPart;
    }

    Dictionary dict;
    dict.name = name;
    dict.pathPart = pathPart;
    dict.fileNamePart = fileNamePart;
    _dictionaryList.append(dict);

    auto *item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setData(0, Qt::UserRole, QStringLiteral("https://raw.githubusercontent.com/qownnotes/dictionaries/master/") +
                  pathPart + QStringLiteral("/") + fileNamePart);
    ui->remoteDictionaryTreeWidget->addTopLevelItem(item);
}

/**
 * Sets up the main splitter
 */
void DictionaryManagerDialog::setupMainSplitter() {
    _mainSplitter = new QSplitter(this);

    _mainSplitter->addWidget(ui->selectFrame);
    _mainSplitter->addWidget(ui->localDictionariesGroupBox);

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value(
            QStringLiteral("DictionaryManagerDialog/mainSplitterState")).toByteArray();
    _mainSplitter->restoreState(state);

    ui->gridLayout->layout()->addWidget(_mainSplitter);
}

/**
 * Stores the settings
 */
void DictionaryManagerDialog::storeSettings() {
    QSettings settings;
    settings.setValue(QStringLiteral("DictionaryManagerDialog/mainSplitterState"),
                      _mainSplitter->saveState());
}

void DictionaryManagerDialog::on_downloadButton_clicked() {
    ui->remoteDictionaryTreeWidget->setDisabled(true);
    ui->downloadFrame->show();
    ui->downloadButton->setDisabled(true);
    ui->downloadProgressBar->setValue(0);
    ui->downloadSizeLabel->clear();

    auto *item = ui->remoteDictionaryTreeWidget->currentItem();

    if (item == nullptr) {
        return;
    }

    QString url = item->data(0, Qt::UserRole).toString() + QStringLiteral(".dic");
    downloadFile(url);
}

void DictionaryManagerDialog::downloadFile(const QString &url) {
    QNetworkRequest networkRequest(url);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    networkRequest.setAttribute(
            QNetworkRequest::FollowRedirectsAttribute, true);
#endif

    // try to ensure the network is accessible
    _networkManager->setNetworkAccessible(
            QNetworkAccessManager::Accessible);

    QNetworkReply *reply = _networkManager->get(networkRequest);

    connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SLOT(downloadProgress(qint64, qint64)));
    connect(ui->downloadCancelButton, SIGNAL(clicked()),
            reply, SLOT(abort()));
}

/**
 * Stores the downloaded file
 *
 * @param reply
 */
void DictionaryManagerDialog::slotReplyFinished(QNetworkReply *reply) {
    if (reply == nullptr) {
        return;
    }

    reply->deleteLater();
    on_downloadCancelButton_clicked();

    QString urlPath = reply->url().path();
    qDebug() << "Reply from " << urlPath;
    QByteArray data = reply->readAll();
    qDebug() << __func__ << " - 'data.size': " << data.size();

    if (reply->error() != QNetworkReply::NoError &&
            reply->error() != QNetworkReply::OperationCanceledError) {
        QMessageBox::critical(
                    nullptr, tr("Download error"),
                tr("Error while downloading:\n%1").arg(reply->errorString()));

        qWarning() << QString("Network error: %1").arg(reply->errorString());

        return;
    }

    QFileInfo info(urlPath);
    QString suffix = info.completeSuffix();

    QFile file(Utils::Misc::localDictionariesPath() + QStringLiteral("/") + info.fileName());

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(
                    nullptr, tr("File error"),
                tr("Could not store downloaded file:\n%1")
                        .arg(file.errorString()));
        return;
    }

    // store the data in the file
    file.write(data);
    file.close();

    // if the "dic" download is done start the "aff"
    if (suffix == QStringLiteral("dic")) {
        auto *item = ui->remoteDictionaryTreeWidget->currentItem();

        if (item != nullptr) {
            QString url = item->data(0, Qt::UserRole).toString() + QStringLiteral(".aff");
            downloadFile(url);
        }
    } else {
        loadLocalDictionaries();
        qApp->setProperty("needsRestart", true);
    }
}
void DictionaryManagerDialog::on_downloadCancelButton_clicked() {
    ui->downloadFrame->hide();
    ui->remoteDictionaryTreeWidget->setEnabled(true);
    ui->downloadButton->setEnabled(true);
}

/**
 * Shows the download progress
 */
void DictionaryManagerDialog::downloadProgress(
        qint64 bytesReceived, qint64 bytesTotal) {
    ui->downloadProgressBar->setMaximum(static_cast <int>(bytesTotal));
    ui->downloadProgressBar->setValue(static_cast <int>(bytesReceived));
    ui->downloadSizeLabel->setText(Utils::Misc::toHumanReadableByteSize(bytesReceived) + QStringLiteral(" / ")
                                   + Utils::Misc::toHumanReadableByteSize(bytesTotal));
}

void DictionaryManagerDialog::loadLocalDictionaries() {
//    ui->localDictionaryTreeWidget

    ui->localDictionaryTreeWidget->clear();
    QDir dir(Utils::Misc::localDictionariesPath());

    QStringList filters;
    filters << QStringLiteral("*.aff");

    // show newest entry first
    QStringList files = dir.entryList(filters, QDir::Files);

    foreach (QString fileName, files) {
        QFileInfo info(fileName);
        QString fileNamePart = info.baseName();

        QString name = getDictionaryName(fileNamePart);
        if (name.isEmpty()) {
            continue;
        }

        auto *item = new QTreeWidgetItem();
        item->setText(0, name);
        item->setData(0, Qt::UserRole, fileNamePart);
        ui->localDictionaryTreeWidget->addTopLevelItem(item);
    }

    qDebug() << files;
}

QString DictionaryManagerDialog::getDictionaryName(const QString &fileNamePart) {
    foreach (Dictionary dict, _dictionaryList) {
        if (dict.fileNamePart == fileNamePart) {
            return dict.name;
        }
    }

    return QString();
}

void DictionaryManagerDialog::on_deleteLocalDictionaryButton_clicked() {
    foreach(QTreeWidgetItem *item, ui->localDictionaryTreeWidget->selectedItems()) {
        const QString &fileNamePart = item->data(0, Qt::UserRole).toString();

        if (deleteLocalDictionaryFile(fileNamePart + QStringLiteral(".aff"))) {
            deleteLocalDictionaryFile(fileNamePart + QStringLiteral(".dic"));
        }
    }

    loadLocalDictionaries();
    qApp->setProperty("needsRestart", true);
}

bool DictionaryManagerDialog::deleteLocalDictionaryFile(const QString &fileName) {
    QFile file(Utils::Misc::localDictionariesPath() + QStringLiteral("/") + fileName);

    if (!file.remove()) {
        QMessageBox::critical(
                    nullptr, tr("File error"),
                    tr("Could not remove dictionary file:\n%1").arg(fileName));
        return false;
    }

    return true;
}

void DictionaryManagerDialog::on_remoteDictionaryTreeWidget_itemSelectionChanged() {
    ui->downloadButton->setDisabled(ui->remoteDictionaryTreeWidget->selectedItems().count() == 0);
}

void DictionaryManagerDialog::on_localDictionaryTreeWidget_itemSelectionChanged() {
    ui->deleteLocalDictionaryButton->setDisabled(ui->localDictionaryTreeWidget->selectedItems().count() == 0);
}

void DictionaryManagerDialog::on_searchDictionaryEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInTreeWidget(ui->remoteDictionaryTreeWidget, arg1);
}

void DictionaryManagerDialog::on_remoteDictionaryTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    Q_UNUSED(item)
    Q_UNUSED(column)
    on_downloadButton_clicked();
}

void DictionaryManagerDialog::on_disableExternalDictionariesCheckBox_toggled(bool checked) {
    QSettings settings;
    settings.setValue(QStringLiteral("disableExternalDictionaries"), checked);
    qApp->setProperty("needsRestart", true);
}
