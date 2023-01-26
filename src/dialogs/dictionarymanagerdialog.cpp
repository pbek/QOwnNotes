#include "dictionarymanagerdialog.h"

#include <utils/gui.h>
#include <utils/misc.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QSettings>
#include <QSplitter>

#include "ui_dictionarymanagerdialog.h"

DictionaryManagerDialog::DictionaryManagerDialog(QWidget *parent)
    : MasterDialog(parent), ui(new Ui::DictionaryManagerDialog) {
    ui->setupUi(this);
    setupMainSplitter();

    _networkManager = new QNetworkAccessManager(this);
    QObject::connect(_networkManager, SIGNAL(finished(QNetworkReply *)), this,
                     SLOT(slotReplyFinished(QNetworkReply *)));

    // you can add more dictionaries from
    // https://github.com/qownnotes/dictionaries/tree/master
    addDictionaryItem(tr("Afrikaans"), QStringLiteral("af_ZA"));
    addDictionaryItem(tr("Amharic"), QStringLiteral("am_ET"));
    addDictionaryItem(tr("Ancient Greek"), QStringLiteral("grc_GR"));
    addDictionaryItem(tr("Arabic"), QStringLiteral("ar"));
    addDictionaryItem(tr("Aragonese"), QStringLiteral("an_ES"), QStringLiteral("Aragonés"));
    addDictionaryItem(tr("Armenian"), QStringLiteral("hy"));
    addDictionaryItem(tr("Albanian"), QStringLiteral("sq_AL"));
    addDictionaryItem(tr("Basque"), QStringLiteral("eu"));
    addDictionaryItem(tr("Belarusian"), QStringLiteral("be_BY"));
    addDictionaryItem(tr("Bulgarian"), QStringLiteral("bg_BG"));
    addDictionaryItem(tr("Bengali"), QStringLiteral("bn_BD"));
    addDictionaryItem(tr("Breton"), QStringLiteral("br_FR"));
    addDictionaryItem(tr("Catalan"), QStringLiteral("ca"));
    addDictionaryItem(tr("Catalan (Valencian)"), QStringLiteral("ca"),
                      QStringLiteral("ca-valencia"));
    addDictionaryItem(tr("Chuvash (Russia)"), QStringLiteral("cv_RU"));
    addDictionaryItem(tr("Croatian"), QStringLiteral("hr_HR"));
    addDictionaryItem(tr("Czech"), QStringLiteral("cs_CZ"));
    addDictionaryItem(tr("Danish"), QStringLiteral("da_DK"));
    addDictionaryItem(tr("Dutch"), QStringLiteral("nl_NL"));
    addDictionaryItem(tr("English (American)"), QStringLiteral("en"), QStringLiteral("en_US"));
    addDictionaryItem(tr("English (Australian)"), QStringLiteral("en"), QStringLiteral("en_AU"));
    addDictionaryItem(tr("English (British)"), QStringLiteral("en"), QStringLiteral("en_GB"));
    addDictionaryItem(tr("English (Canadian)"), QStringLiteral("en"), QStringLiteral("en_CA"));
    addDictionaryItem(tr("English (Medical Dictionary)"), QStringLiteral("en"),
                      QStringLiteral("en-medical"));
    addDictionaryItem(tr("English (South African)"), QStringLiteral("en"), QStringLiteral("en_ZA"));
    addDictionaryItem(tr("Esperanto"), QStringLiteral("eo"));
    addDictionaryItem(tr("Estonian"), QStringLiteral("et_EE"));
    addDictionaryItem(tr("Faroese"), QStringLiteral("fo"));
    addDictionaryItem(tr("French"), QStringLiteral("fr_FR"), QStringLiteral("fr"));
    addDictionaryItem(tr("Friulian"), QStringLiteral("fur_IT"));
    addDictionaryItem(tr("Gaelic"), QStringLiteral("gd_GB"));
    addDictionaryItem(tr("Galician"), QStringLiteral("gl"), QStringLiteral("gl_ES"));
    addDictionaryItem(tr("Georgian"), QStringLiteral("ka_GE"));
    addDictionaryItem(tr("German (Austrian)"), QStringLiteral("de"), QStringLiteral("de_AT_frami"));
    addDictionaryItem(tr("German (Classical Spelling)"), QStringLiteral("de"),
                      QStringLiteral("de_DE-oldspell"));
    addDictionaryItem(tr("German (German)"), QStringLiteral("de"), QStringLiteral("de_DE_frami"));
    addDictionaryItem(tr("German (Swiss)"), QStringLiteral("de"), QStringLiteral("de_CH_frami"));
    addDictionaryItem(tr("German (Medical Dictionary)"), QStringLiteral("de"),
                      QStringLiteral("de-medical"));
    addDictionaryItem(tr("Greek"), QStringLiteral("el_GR"));
    addDictionaryItem(tr("Modern Greek (Polytonic Greek)"), QStringLiteral("el_GR"),
                      QStringLiteral("el-polyton"));
    addDictionaryItem(tr("Gujarati"), QStringLiteral("gu_IN"));
    addDictionaryItem(tr("Gurani"), QStringLiteral("gug"));
    addDictionaryItem(tr("Hebrew"), QStringLiteral("he_IL"));
    addDictionaryItem(tr("Hindi"), QStringLiteral("hi_IN"));
    addDictionaryItem(tr("Hungarian"), QStringLiteral("hu_HU"));
    addDictionaryItem(tr("Icelandic"), QStringLiteral("is"));
    addDictionaryItem(tr("Indonesian"), QStringLiteral("id"), QStringLiteral("id_ID"));
    addDictionaryItem(tr("Italian"), QStringLiteral("it_IT"));
    addDictionaryItem(tr("Kaszebsczi"), QStringLiteral("csb_PL"), QStringLiteral("Kaszebsczi"));
    addDictionaryItem(tr("Kazakh"), QStringLiteral("kk"));
    addDictionaryItem(tr("Khmer"), QStringLiteral("km_KH"));
    addDictionaryItem(tr("Komi (Russia)"), QStringLiteral("koi"));
    addDictionaryItem(tr("Korean"), QStringLiteral("ko_KR"));
    addDictionaryItem(tr("Kurdish (Turkey)"), QStringLiteral("ku_TR"));
    addDictionaryItem(tr("Lao"), QStringLiteral("lo_LA"));
    addDictionaryItem(tr("Latgalian"), QStringLiteral("ltg"));
    addDictionaryItem(tr("Latin"), QStringLiteral("la"));
    addDictionaryItem(tr("Latvian"), QStringLiteral("lv_LV"));
    addDictionaryItem(tr("Lithuanian"), QStringLiteral("lt_LT"));
    addDictionaryItem(tr("Low German / Low Saxon"), QStringLiteral("nds"));
    addDictionaryItem(tr("Luxembourgish (Letzeburgesch)"), QStringLiteral("lb"));
    addDictionaryItem(tr("Macedonian"), QStringLiteral("mk"));
    addDictionaryItem(tr("Malayalam"), QStringLiteral("ml_IN"));
    addDictionaryItem(tr("Mongolian"), QStringLiteral("mn"));
    addDictionaryItem(tr("Nepali"), QStringLiteral("ne_NP"));
    addDictionaryItem(tr("Norwegian (Bokmål)"), QStringLiteral("no"), QStringLiteral("nb_NO"));
    addDictionaryItem(tr("Norwegian (Nynorsk)"), QStringLiteral("no"), QStringLiteral("nn_NO"));
    addDictionaryItem(tr("Occitan"), QStringLiteral("oc_FR"));
    addDictionaryItem(tr("Papiamentu"), QStringLiteral("pap_CW"));
    addDictionaryItem(tr("Persian"), QStringLiteral("fa_IR"));
    addDictionaryItem(tr("Polish"), QStringLiteral("pl_PL"));
    addDictionaryItem(tr("Portugese (Brazilian)"), QStringLiteral("pt_BR"));
    addDictionaryItem(tr("Portugese"), QStringLiteral("pt_PT"));
    addDictionaryItem(tr("Romanian"), QStringLiteral("ro"), QStringLiteral("ro_RO"));
    addDictionaryItem(tr("Romansh"), QStringLiteral("rm"));
    addDictionaryItem(tr("Russian"), QStringLiteral("ru_RU"));
    addDictionaryItem(tr("Russian Medical Dictionary"), QStringLiteral("ru_RU"),
                      QStringLiteral("ru_RU-medicine"));
    addDictionaryItem(tr("Rusyn / Ruthene (Slovakia)"), QStringLiteral("sk_SK"),
                      QStringLiteral("rue_SK"));
    addDictionaryItem(tr("Serbian (Cyrillic)"), QStringLiteral("sr"));
    addDictionaryItem(tr("Serbian (Latin)"), QStringLiteral("sr"), QStringLiteral("sr-Latn"));
    addDictionaryItem(tr("Shona"), QStringLiteral("sn_ZW"));
    addDictionaryItem(tr("Sindhi"), QStringLiteral("sd"));
    addDictionaryItem(tr("Sinhala"), QStringLiteral("si_LK"));
    addDictionaryItem(tr("Slovak"), QStringLiteral("sk_SK"));
    addDictionaryItem(tr("Slovenian"), QStringLiteral("sl_SI"));
    addDictionaryItem(tr("Spanish"), QStringLiteral("es"), QStringLiteral("es_ANY"));
    addDictionaryItem(tr("Swahili"), QStringLiteral("sw_TZ"));
    addDictionaryItem(tr("Swedish"), QStringLiteral("sv_SE"));
    addDictionaryItem(tr("Tagalog"), QStringLiteral("tl_PH"));
    addDictionaryItem(tr("Tajik"), QStringLiteral("tg_TG"));
    addDictionaryItem(tr("Tatar (Russia)"), QStringLiteral("tt_RU"));
    addDictionaryItem(tr("Telugu"), QStringLiteral("te_IN"));
    addDictionaryItem(tr("Thai"), QStringLiteral("th_TH"));
    addDictionaryItem(tr("Turkish"), QStringLiteral("tr_TR"));
    addDictionaryItem(tr("Turkmen"), QStringLiteral("tk"));
    addDictionaryItem(tr("Udmurt (Russia)"), QStringLiteral("udm"));
    addDictionaryItem(tr("Ukrainian"), QStringLiteral("uk_UA"));
    addDictionaryItem(tr("Urdu"), QStringLiteral("ur_PK"));
    addDictionaryItem(tr("Uzbek"), QStringLiteral("uz_UZ"));
    addDictionaryItem(tr("Venda"), QStringLiteral("ve_ZA"));
    addDictionaryItem(tr("Venetian"), QStringLiteral("vec_IT"));
    addDictionaryItem(tr("Vietnamese"), QStringLiteral("vi"), QStringLiteral("vi_VN"));
    addDictionaryItem(tr("Welsh"), QStringLiteral("cy_GB"));
    addDictionaryItem(tr("Xhosa"), QStringLiteral("xh_ZA"));
    addDictionaryItem(tr("Zulu"), QStringLiteral("zu_ZA"));

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

void DictionaryManagerDialog::addDictionaryItem(const QString &name, const QString &pathPart,
                                                QString fileNamePart) {
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
    item->setData(0, Qt::UserRole,
                  QStringLiteral("https://raw.githubusercontent.com/qownnotes/"
                                 "dictionaries/master/") +
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
    QByteArray state =
        settings.value(QStringLiteral("DictionaryManagerDialog/mainSplitterState")).toByteArray();
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
        QMessageBox::critical(nullptr, tr("Download error"),
                              tr("Error while downloading:\n%1").arg(reply->errorString()));

        qWarning() << QStringLiteral("Network error: %1").arg(reply->errorString());

        return;
    }

    QFileInfo info(urlPath);
    QString suffix = info.completeSuffix();

    QFile file(Utils::Misc::localDictionariesPath() + QStringLiteral("/") + info.fileName());

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(nullptr, tr("File error"),
                              tr("Could not store downloaded file:\n%1").arg(file.errorString()));
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
void DictionaryManagerDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    QString text = Utils::Misc::toHumanReadableByteSize(bytesReceived);

    // bytesTotal can be set to -1 if not available
    if (bytesTotal > -1) {
        text += QStringLiteral(" / ") + Utils::Misc::toHumanReadableByteSize(bytesTotal);
    } else {
        // set to 4 MB
        bytesTotal = 4194304;
    }

    ui->downloadProgressBar->setMaximum(static_cast<int>(bytesTotal));
    ui->downloadProgressBar->setValue(static_cast<int>(bytesReceived));
    ui->downloadSizeLabel->setText(text);
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
    foreach (QTreeWidgetItem *item, ui->localDictionaryTreeWidget->selectedItems()) {
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
        QMessageBox::critical(nullptr, tr("File error"),
                              tr("Could not remove dictionary file:\n%1").arg(fileName));
        return false;
    }

    return true;
}

void DictionaryManagerDialog::on_remoteDictionaryTreeWidget_itemSelectionChanged() {
    ui->downloadButton->setDisabled(ui->remoteDictionaryTreeWidget->selectedItems().count() == 0);
}

void DictionaryManagerDialog::on_localDictionaryTreeWidget_itemSelectionChanged() {
    ui->deleteLocalDictionaryButton->setDisabled(
        ui->localDictionaryTreeWidget->selectedItems().count() == 0);
}

void DictionaryManagerDialog::on_searchDictionaryEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInTreeWidget(ui->remoteDictionaryTreeWidget, arg1);
}

void DictionaryManagerDialog::on_remoteDictionaryTreeWidget_itemDoubleClicked(QTreeWidgetItem *item,
                                                                              int column) {
    Q_UNUSED(item)
    Q_UNUSED(column)
    on_downloadButton_clicked();
}

void DictionaryManagerDialog::on_disableExternalDictionariesCheckBox_toggled(bool checked) {
    QSettings settings;
    settings.setValue(QStringLiteral("disableExternalDictionaries"), checked);
    qApp->setProperty("needsRestart", true);
}
