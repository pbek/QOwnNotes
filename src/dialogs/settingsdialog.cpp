#include "services/owncloudservice.h"
#include "services/databaseservice.h"
#include "dialogs/settingsdialog.h"
#include "ui_settingsdialog.h"
#include "build_number.h"
#include "version.h"
#include "release.h"
#include "services/updateservice.h"
#include <QSettings>
#include <QFileDialog>
#include <QDebug>
#include <QDesktopServices>
#include <QFontDialog>
#include <QMessageBox>
#include <services/metricsservice.h>
#include "helpers/clientproxy.h"
#include <QtNetwork/qnetworkproxy.h>
#include <services/cryptoservice.h>
#include <entities/notefolder.h>
#include <QTextBrowser>

SettingsDialog::SettingsDialog(int tab, QWidget *parent) : MasterDialog(parent),
        ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
    resize(1, 1);

    ui->connectionTestLabel->hide();
    ui->connectButton->setDefault(true);
    ui->noteSaveIntervalTime->setToolTip(
            ui->noteSaveIntervalTimeLabel->toolTip());
    ui->tabWidget->setCurrentIndex(tab);

    for (int i = 0; i <= 8; i++) {
        setOKLabelData(i, "unknown", SettingsDialog::Unknown);
    }

    // add the QOwnNotesAPI minimum version number to the info text
    QString html = ui->installInfoTextLabel1->text();
    html.replace("QOWNNOTESAPI_MIN_VERSION", QOWNNOTESAPI_MIN_VERSION);
    ui->installInfoTextLabel1->setText(html);

    // do the network proxy tab setup
    setupProxyTab();

    // setup the note folder tab
    setupNoteFolderTab();

    readSettings();

    if (ui->serverUrlEdit->text() != "") {
        // start a connection test
        startConnectionTest();
    }
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

/**
 * Does the network proxy tab setup
 */
void SettingsDialog::setupProxyTab() {
    ui->hostLineEdit->setPlaceholderText(tr("hostname of proxy server"));
    ui->userLineEdit->setPlaceholderText(tr("username for proxy server"));
    ui->passwordLineEdit->setPlaceholderText(tr("password for proxy server"));

    ui->typeComboBox->addItem(tr("HTTP(S) proxy"), QNetworkProxy::HttpProxy);
    ui->typeComboBox->addItem(tr("SOCKS5 proxy"), QNetworkProxy::Socks5Proxy);

    ui->authRequiredcheckBox->setEnabled(true);

    // Explicitly set up the enabled status of the proxy auth widgets to ensure
    // toggling the parent enables/disables the children
    ui->userLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->authWidgets->setEnabled(ui->authRequiredcheckBox->isChecked());
    connect(ui->authRequiredcheckBox, SIGNAL(toggled(bool)),
            ui->authWidgets, SLOT(setEnabled(bool)));

    connect(ui->manualProxyRadioButton, SIGNAL(toggled(bool)),
            ui->manualSettings, SLOT(setEnabled(bool)));
    connect(ui->manualProxyRadioButton, SIGNAL(toggled(bool)),
            ui->typeComboBox, SLOT(setEnabled(bool)));

    // proxy
//    connect(ui->typeComboBox, SIGNAL(currentIndexChanged(int)),
//            SLOT(storeProxySettings()));
//    connect(ui->proxyButtonGroup, SIGNAL(buttonClicked(int)),
//            SLOT(storeProxySettings()));
//    connect(ui->hostLineEdit, SIGNAL(editingFinished()),
//            SLOT(storeProxySettings()));
//    connect(ui->userLineEdit, SIGNAL(editingFinished()),
//            SLOT(storeProxySettings()));
//    connect(ui->passwordLineEdit, SIGNAL(editingFinished()),
//            SLOT(storeProxySettings()));
//    connect(ui->portSpinBox, SIGNAL(editingFinished()),
//            SLOT(storeProxySettings()));
//    connect(ui->authRequiredcheckBox, SIGNAL(toggled(bool)),
//            SLOT(storeProxySettings()));
}

/**
 * Loads the proxy settings
 */
void SettingsDialog::loadProxySettings() {
    QSettings settings;

    // load current proxy settings
    int type = settings.value("networking/proxyType").toInt();
    switch (type) {
        case QNetworkProxy::NoProxy:
            ui->noProxyRadioButton->setChecked(true);
            break;
        case QNetworkProxy::DefaultProxy:
            ui->systemProxyRadioButton->setChecked(true);
            break;
        case QNetworkProxy::Socks5Proxy:
        case QNetworkProxy::HttpProxy:
            ui->typeComboBox->setCurrentIndex(ui->typeComboBox->findData(type));
            ui->manualProxyRadioButton->setChecked(true);
            break;
        default:
            break;
    }

    ui->hostLineEdit->setText(
            settings.value("networking/proxyHostName").toString());
    ui->portSpinBox->setValue(
            settings.value("networking/proxyPort", 8080).toInt());
    ui->authRequiredcheckBox->setChecked(
            settings.value("networking/proxyNeedsAuth").toBool());
    ui->userLineEdit->setText(
            settings.value("networking/proxyUser").toString());
    ui->passwordLineEdit->setText(CryptoService::instance()->decryptToString(
            settings.value("networking/proxyPassword").toString()));
}

/**
 * Stores the proxy settings
 */
void SettingsDialog::storeProxySettings() {
    QSettings settings;
    int proxyType = QNetworkProxy::DefaultProxy;

    if (ui->noProxyRadioButton->isChecked()) {
        proxyType = QNetworkProxy::NoProxy;
    } else if (ui->systemProxyRadioButton->isChecked()) {
        proxyType = QNetworkProxy::DefaultProxy;
    } else if (ui->manualProxyRadioButton->isChecked()) {
        proxyType = ui->typeComboBox->itemData(
                ui->typeComboBox->currentIndex()).toInt();

        settings.setValue(
                "networking/proxyNeedsAuth",
                ui->authRequiredcheckBox->isChecked());
        settings.setValue("networking/proxyUser", ui->userLineEdit->text());
        settings.setValue(
                "networking/proxyPassword",
                CryptoService::instance()->encryptToString(
                        ui->passwordLineEdit->text()));
        settings.setValue("networking/proxyHostName", ui->hostLineEdit->text());
        settings.setValue("networking/proxyPort", ui->portSpinBox->value());
    }

    settings.setValue("networking/proxyType", proxyType);

    ClientProxy proxy;

    // refresh the Qt proxy settings
    proxy.setupQtProxyFromSettings();
}

/**
 * @brief Starts a connection test
 */
void SettingsDialog::startConnectionTest() {
    ui->connectionTestLabel->hide();
    OwnCloudService *ownCloud = new OwnCloudService(this);
    ownCloud->settingsConnectionTest(this);
}

/**
 * @brief SettingsDialog::on_connectButton_clicked
 */
void SettingsDialog::on_connectButton_clicked() {
    storeSettings();

    // start a connection test
    startConnectionTest();
}

void SettingsDialog::storeSettings() {
    QSettings settings;
    QString url = QString(ui->serverUrlEdit->text());

    // remove trailing "/" of the server url
    if (url.endsWith("/")) {
        url.chop(1);
        ui->serverUrlEdit->setText(url);
    }

    settings.setValue("ownCloud/serverUrl", url);
    settings.setValue("ownCloud/userName", ui->userNameEdit->text());
    settings.setValue("ownCloud/password",
                      CryptoService::instance()->encryptToString(ui->passwordEdit->text()));
    settings.setValue("disableAutomaticUpdateDialog",
                      ui->disableAutomaticUpdateDialogCheckBox->isChecked());
    settings.setValue("notifyAllExternalModifications",
                      ui->notifyAllExternalModificationsCheckBox->isChecked());
    settings.setValue("noteSaveIntervalTime",
                      ui->noteSaveIntervalTime->value());
    settings.setValue("defaultNoteFileExtension",
                      getSelectedListWidgetValue(
                              ui->defaultNoteFileExtensionListWidget));
    settings.setValue("externalEditorPath",
                      ui->externalEditorPathLineEdit->text());
    settings.setValue("MainWindow/noteTextEdit.font",
                      noteTextEditFont.toString());
    settings.setValue("MainWindow/noteTextEdit.code.font",
                      noteTextEditCodeFont.toString());
    settings.setValue("MainWindow/noteTextView.font",
                      noteTextViewFont.toString());
    settings.setValue("MainWindow/noteTextView.code.font",
                      noteTextViewCodeFont.toString());
    settings.setValue("MainWindow/mainToolBar.iconSize",
                      QString::number(ui->toolbarIconSizeSpinBox->value()));
    settings.setValue("MainWindow/showRecentNoteFolderInMainArea",
                      ui->showNoteFolderCheckBox->isChecked());
    settings.setValue("allowOnlyOneAppInstance",
                      ui->allowOnlyOneAppInstanceCheckBox->isChecked());
    settings.setValue("interfaceLanguage",
                      getSelectedListWidgetValue(ui->languageListWidget));

    if (!settings.value("appMetrics/disableTracking").toBool() &&
            ui->appMetricsCheckBox->isChecked()) {
        MetricsService::instance()->sendVisit(
                "settings/app-metrics-disabled");
    }

    settings.setValue("appMetrics/disableTracking",
                      ui->appMetricsCheckBox->isChecked());

    if (!settings.value("appMetrics/disableAppHeartbeat").toBool() &&
        ui->appHeartbeatCheckBox->isChecked()) {
        MetricsService::instance()->sendVisit(
                "settings/app-heartbeat-disabled");
    }

    settings.setValue("appMetrics/disableAppHeartbeat",
                      ui->appHeartbeatCheckBox->isChecked());

    settings.setValue("darkModeColors",
                      ui->darkModeColorsCheckBox->isChecked());

    QStringList todoCalendarUrlList;
    QStringList todoCalendarEnabledList;
    QStringList todoCalendarEnabledUrlList;
    for (int i = 0; i < ui->todoCalendarListWidget->count(); i++) {
        QListWidgetItem *item = ui->todoCalendarListWidget->item(i);

        todoCalendarUrlList.append(item->toolTip());

        if (item->checkState() == Qt::Checked) {
            todoCalendarEnabledList.append(item->text());
            todoCalendarEnabledUrlList.append(item->toolTip());
        }
    }

    // store the todo calendar data to the settings
    settings.setValue("ownCloud/todoCalendarUrlList", todoCalendarUrlList);
    settings.setValue("ownCloud/todoCalendarEnabledList",
                      todoCalendarEnabledList);
    settings.setValue("ownCloud/todoCalendarEnabledUrlList",
                      todoCalendarEnabledUrlList);
    settings.setValue("ownCloud/todoCalendarBackend",
                      ui->calendarPlusRadioButton->isChecked()
                      ? OwnCloudService::CalendarPlus
                      : OwnCloudService::DefaultOwnCloudCalendar);

    settings.setValue("networking/ignoreSSLErrors",
                      ui->ignoreSSLErrorsCheckBox->isChecked());

    // store the proxy settings
    storeProxySettings();
}

void SettingsDialog::readSettings() {
    QSettings settings;
    ui->serverUrlEdit->setText(settings.value("ownCloud/serverUrl").toString());
    ui->userNameEdit->setText(settings.value("ownCloud/userName").toString());
    ui->passwordEdit->setText(CryptoService::instance()->decryptToString(
            settings.value("ownCloud/password").toString()));
    ui->externalEditorPathLineEdit->setText(
            settings.value("externalEditorPath").toString());
    ui->disableAutomaticUpdateDialogCheckBox->setChecked(
            settings.value("disableAutomaticUpdateDialog").toBool());
    ui->notifyAllExternalModificationsCheckBox->setChecked(
            settings.value("notifyAllExternalModifications").toBool());
    ui->noteSaveIntervalTime->setValue(
            settings.value("noteSaveIntervalTime").toInt());
    ui->showNoteFolderCheckBox->setChecked(settings.value(
            "MainWindow/showRecentNoteFolderInMainArea").toBool());
    ui->allowOnlyOneAppInstanceCheckBox->setChecked(settings.value(
            "allowOnlyOneAppInstance").toBool());
    ui->toolbarIconSizeSpinBox->setValue(
                 settings.value("MainWindow/mainToolBar.iconSize").toInt());

    selectListWidgetValue(ui->languageListWidget,
                          settings.value("interfaceLanguage").toString());
    selectListWidgetValue(ui->defaultNoteFileExtensionListWidget,
                          Note::defaultNoteFileExtension());

    const QSignalBlocker blocker(ui->appMetricsCheckBox);
    Q_UNUSED(blocker);
    ui->appMetricsCheckBox->setChecked(settings.value(
            "appMetrics/disableTracking").toBool());

    ui->appHeartbeatCheckBox->setChecked(settings.value(
            "appMetrics/disableAppHeartbeat").toBool());

    ui->darkModeColorsCheckBox->setChecked(settings.value(
            "darkModeColors").toBool());

    noteTextEditFont.fromString(
            settings.value("MainWindow/noteTextEdit.font").toString());
    setFontLabel(ui->noteTextEditFontLabel, noteTextEditFont);

    noteTextEditCodeFont.fromString(
            settings.value("MainWindow/noteTextEdit.code.font").toString());
    setFontLabel(ui->noteTextEditCodeFontLabel, noteTextEditCodeFont);

    noteTextViewFont.fromString(
            settings.value("MainWindow/noteTextView.font").toString());
    setFontLabel(ui->noteTextViewFontLabel, noteTextViewFont);


    // load note text view code font
    QString fontString = settings.value("MainWindow/noteTextView.code.font")
            .toString();

    // set a default note text view code font
    if (fontString == "") {
        // reset the note text view code font
        on_noteTextViewCodeResetButton_clicked();

        fontString = noteTextViewCodeFont.toString();
        settings.setValue("MainWindow/noteTextView.code.font", fontString);
    } else {
        noteTextViewCodeFont.fromString(fontString);
    }

    setFontLabel(ui->noteTextViewCodeFontLabel, noteTextViewCodeFont);

    const QSignalBlocker blocker2(this->ui->defaultOwnCloudCalendarRadioButton);
    Q_UNUSED(blocker2);

    switch (settings.value("ownCloud/todoCalendarBackend").toInt()) {
        case OwnCloudService::CalendarPlus:
            ui->calendarPlusRadioButton->setChecked(true);
            break;
        default:
            ui->defaultOwnCloudCalendarRadioButton->setChecked(true);
            break;
    }

    QStringList todoCalendarUrlList = settings.value(
            "ownCloud/todoCalendarUrlList").toStringList();
    // load the todo calendar list and set the checked state
    refreshTodoCalendarList(todoCalendarUrlList, true);

    bool ignoreSSLErrors =
            settings.value("networking/ignoreSSLErrors", true).toBool();
    ui->ignoreSSLErrorsCheckBox->setChecked(ignoreSSLErrors);
    ui->letsEncryptInfoLabel->setVisible(ignoreSSLErrors);

    // load the proxy settings
    loadProxySettings();
}

/**
 * Selects a value in a list widget, that is hidden in the whatsThis parameter
 */
void SettingsDialog::selectListWidgetValue(
        QListWidget* listWidget, QString value) {
    // get all items from the list widget
    QList<QListWidgetItem *> items =
            listWidget->findItems(
                    QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    // select the right item in the selector
    Q_FOREACH(QListWidgetItem *item, items) {
            if (item->whatsThis() == value) {
                const QSignalBlocker blocker(listWidget);
                Q_UNUSED(blocker);

                item->whatsThis();
                listWidget->setItemSelected(item, true);
                break;
            }
        }
}

/**
 * Returns the selected value in list widget, that is hidden in
 * the whatsThis parameter
 */
QString SettingsDialog::getSelectedListWidgetValue(QListWidget* listWidget) {
    QList<QListWidgetItem*> items = listWidget->selectedItems();

    if (items.count() >= 1) {
        return items.first()->whatsThis();
    }

    return "";
}

void SettingsDialog::setFontLabel(QLineEdit *label, QFont font) {
    label->setText(
            font.family() + " (" + QString::number(font.pointSize()) + ")");
    label->setFont(font);
}

void SettingsDialog::outputSettings() {
    QSettings settings;

    QListIterator<QString> itr(settings.allKeys());
    QString output;

    output += "QOwnNotes Debug Information\n";
    output += "===========================\n";

    QDateTime dateTime = QDateTime::currentDateTime();

    // add information about QOwnNotes
    output += "\n## General Info\n\n";
    output += prepareDebugInformationLine("Current Date", dateTime.toString());
    output += prepareDebugInformationLine("Version", QString(VERSION));
    output += prepareDebugInformationLine("Build date", QString(__DATE__));
    output += prepareDebugInformationLine("Build number",
                                          QString::number(BUILD));
    output += prepareDebugInformationLine("Platform", QString(PLATFORM));

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    output += prepareDebugInformationLine("Operating System",
                                          QSysInfo::prettyProductName());
    output += prepareDebugInformationLine("Build architecture",
                                          QSysInfo::buildCpuArchitecture());
    output += prepareDebugInformationLine("Current architecture",
                                          QSysInfo::currentCpuArchitecture());
#endif
    output += prepareDebugInformationLine("Release", QString(RELEASE));
    output += prepareDebugInformationLine("Qt Version", QT_VERSION_STR);

    // add information about the server
    output += "\n## Server Info\n\n";
    output += prepareDebugInformationLine("serverUrl",
                                          ui->serverUrlEdit->text());
    output += prepareDebugInformationLine("appIsValid",
                                          QString(appIsValid ? "yes" : "no"));
    if (appIsValid) {
        output += prepareDebugInformationLine("serverVersion", serverVersion);
        output += prepareDebugInformationLine("appVersion", appVersion);
    } else {
        output += prepareDebugInformationLine("connectionErrorMessage",
                                              connectionErrorMessage);
    }

    // add information about the settings
    output += "\n## Settings\n\n";

    // hide values of these keys
    QStringList keyHiddenList = (QStringList() <<
            "cryptoKey" <<
            "ownCloud/password" <<
            "PiwikClientId" <<
            "networking/proxyPassword");

    while (itr.hasNext()) {
        QString key = itr.next();
        QVariant value = settings.value(key);

        // hide values of certain keys
        if (keyHiddenList.contains(key)) {
            output += prepareDebugInformationLine(key, "<hidden>");
        } else {
            switch (value.type()) {
                case QVariant::StringList:
                    output += prepareDebugInformationLine(
                            key,
                            value.toStringList().join(", "));
                    break;
                case QVariant::ByteArray:
                    output += prepareDebugInformationLine(key, "<binary data>");
                    break;
                default:
                    output += prepareDebugInformationLine(key,
                                                          value.toString());
            }
        }
    }

    ui->debugInfoTextEdit->setPlainText(output);
}

/**
 * @brief Prepares the debug information to output it as markdown
 * @param headline
 * @param data
 */
QString SettingsDialog::prepareDebugInformationLine(QString headline,
                                                    QString data) {
    data = (data == "") ? "*empty*" : "`" + data + "`";
    return "**" + headline + "**: " + data + "\n";
}

/**
 * Callback function from OwnCloudService to output a success or error message
 *
 * @brief SettingsDialog::connectTestCallback
 * @param appIsValid
 * @param appVersion
 * @param serverVersion
 */
void SettingsDialog::connectTestCallback(bool appIsValid, QString appVersion,
                                         QString serverVersion,
                                         QString connectionErrorMessage) {
    this->appIsValid = appIsValid;
    this->appVersion = appVersion;
    this->serverVersion = serverVersion;
    this->connectionErrorMessage = connectionErrorMessage;

    if (appIsValid) {
        ui->connectionTestLabel->setStyleSheet("color: green;");
        ui->connectionTestLabel->setText(
                tr("The connection was made successfully!\n"
                   "Server version: %1\nQOwnNotesAPI version: %2")
                    .arg(serverVersion).arg(appVersion));
    } else {
        // hide password
        if (!ui->passwordEdit->text().isEmpty()) {
            connectionErrorMessage.replace(ui->passwordEdit->text(), "***");
        }

        ui->connectionTestLabel->setStyleSheet("color: red;");
        ui->connectionTestLabel->setText(
                tr("There was an error connecting to the ownCloud Server!\n"
                        "You also need to have the QOwnNotesAPI app installed "
                        "and enabled!\n\nConnection error message: ") +
                connectionErrorMessage);
    }

    ui->connectionTestLabel->adjustSize();
    ui->connectionTestLabel->show();
}

/**
 * @brief set text and color of an ok-label
 * @param number
 * @param text
 * @param color
 */
void SettingsDialog::setOKLabelData(int number, QString text,
                                    OKLabelStatus status) {
    QLabel *label;

    switch (number) {
        case 1:
            label = ui->ok1Label;
            break;
        case 2:
            label = ui->ok2Label;
            break;
        case 3:
            label = ui->ok3Label;
            break;
        case 4:
            label = ui->ok4Label;
            break;
        case 6:
            label = ui->ok6Label;
            break;
        case 7:
            label = ui->ok7Label;
            break;
        case 8:
            label = ui->ok8Label;
            break;
        default:
            return;
    }

    QString color;
    switch (status) {
        case Unknown:
            color = "gray";
            break;
        case OK:
            color = "green";
            break;
        case Warning:
            color = "orange";
            break;
        case Failure:
            color = "red";
            break;
        default:
            color = "white";
    }

    label->setText(text);
    label->setStyleSheet("color: " + color);
}

void SettingsDialog::refreshTodoCalendarList(QStringList items,
                                             bool forceReadCheckedState) {
    // we want to read the checked state from the settings if the
    // todo calendar list was not empty
    bool readCheckedState = forceReadCheckedState ? true :
                            ui->todoCalendarListWidget->count() > 0;

    // clear the todo calendar list
    ui->todoCalendarListWidget->clear();

    QSettings settings;
    QStringList todoCalendarEnabledList = settings.value(
            "ownCloud/todoCalendarEnabledList").toStringList();

    QListIterator<QString> itr(items);
    while (itr.hasNext()) {
        QString url = itr.next();

        QUrl serverUrl(ui->serverUrlEdit->text());

        // continue if server url isn't valid
        if (!serverUrl.isValid()) {
            continue;
        }

        QString serverUrlText(serverUrl.toString());
        QString serverUrlPath = serverUrl.path();
        if (serverUrlPath != "") {
            // remove the path from the end because we already got it in the url
            serverUrlText.replace(QRegularExpression(
                    QRegularExpression::escape(serverUrlPath) + "$"), "");
        }

        // only add the server url if it wasn't already added
        if (!url.startsWith(serverUrlText)) {
            url = serverUrlText + url;
        }

        // get the name out of the url part
        QRegularExpression regex("\\/([^\\/]*)\\/$");
        QRegularExpressionMatch match = regex.match(url);
        QString name = match.captured(1);

        // remove percent encoding
        name = QUrl::fromPercentEncoding(name.toLatin1());

        // skip the contact birthdays calendar
        if (name == "contact_birthdays") {
            continue;
        }

        // skip the Calendar Plus birthday calendar
        if (name.startsWith("bdaycpltocal_")) {
            continue;
        }

        // create the list widget item and add it to the
        // todo calendar list widget
        QListWidgetItem *item = new QListWidgetItem(name);

        // eventually check if item was checked
        Qt::CheckState checkedState = readCheckedState
                                      ? (todoCalendarEnabledList.contains(name)
                                         ? Qt::Checked : Qt::Unchecked)
                                      : Qt::Checked;
        item->setCheckState(checkedState);

        item->setFlags(Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled |
                       Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
        item->setToolTip(url);
        ui->todoCalendarListWidget->addItem(item);
    }
}


/* * * * * * * * * * * * * * * *
 *
 * Slot implementations
 *
 * * * * * * * * * * * * * * * */

void SettingsDialog::on_buttonBox_clicked(QAbstractButton *button) {
    if (button == ui->buttonBox->button(QDialogButtonBox::Ok)) {
        storeSettings();
    }
}

void SettingsDialog::on_ownCloudServerAppPageButton_clicked() {
    QDesktopServices::openUrl(
            QUrl(ui->serverUrlEdit->text() + "/index.php/settings/apps"));
}

void SettingsDialog::on_noteTextEditButton_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, noteTextEditFont, this);
    if (ok) {
        noteTextEditFont = font;
        setFontLabel(ui->noteTextEditFontLabel, noteTextEditFont);
    }
}

void SettingsDialog::on_noteTextEditCodeButton_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(
            &ok, noteTextEditCodeFont, this,
            "", QFontDialog::MonospacedFonts);
    if (ok) {
        noteTextEditCodeFont = font;
        setFontLabel(ui->noteTextEditCodeFontLabel, noteTextEditCodeFont);
    }
}

void SettingsDialog::on_noteTextViewButton_clicked() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, noteTextViewFont, this);
    if (ok) {
        noteTextViewFont = font;
        setFontLabel(ui->noteTextViewFontLabel, noteTextViewFont);
    }
}

void SettingsDialog::on_noteTextViewCodeButton_clicked()
{
    bool ok;
    QFont font = QFontDialog::getFont(
            &ok, noteTextViewCodeFont, this,
            "", QFontDialog::MonospacedFonts);
    if (ok) {
        noteTextViewCodeFont = font;
        setFontLabel(ui->noteTextViewCodeFontLabel, noteTextViewCodeFont);
    }
}

void SettingsDialog::on_reloadCalendarListButton_clicked() {
    // we need to store the calendar backend
    storeSettings();

    OwnCloudService *ownCloud = new OwnCloudService(this);
    ownCloud->settingsGetCalendarList(this);
}

void SettingsDialog::on_defaultOwnCloudCalendarRadioButton_toggled(
        bool checked) {
    on_reloadCalendarListButton_clicked();
}

void SettingsDialog::on_reinitializeDatabaseButton_clicked() {
    if (QMessageBox::information(
            this, tr("Database"),
            tr("Do you really want to clear the local database? "
                       "This will also remove your configured note "
                       "folders and your cached todo items!"),
            tr("Clear &database"), tr("&Cancel"), QString::null,
            1) == 0) {
        DatabaseService::reinitializeDiskDatabase();
        NoteFolder::migrateToNoteFolders();

        QMessageBox::information(this, tr("Database"),
                                 tr("The Database was reinitialized."));
    }
}

void SettingsDialog::on_tabWidget_currentChanged(int index) {
    if (index == DebugTab) {
        outputSettings();
    } else if (index == OwnCloudTab) {
        on_connectButton_clicked();
    }
}

/**
 * @brief Stores the debug information to a markdown file
 */
void SettingsDialog::on_saveDebugInfoButton_clicked() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("Markdown files (*.md)"));
    dialog.setWindowTitle(tr("Save debug information"));
    dialog.selectFile("QOwnNotes Debug Information.md");
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.size() == 0) {
            return;
        }

        QFile file(fileNames.at(0));

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            qWarning() << file.errorString();
            return;
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");
        out << ui->debugInfoTextEdit->toPlainText();
        file.flush();
        file.close();
    }
}

void SettingsDialog::on_appMetricsCheckBox_toggled(bool checked)
{
    if (checked) {
        int reply;
        reply = QMessageBox::question(
                this,
                "Disable usage tracking",
                "Anonymous usage data helps to decide what parts of "
                        "QOwnNotes to improve next and to find and fix bugs."
                        "<br />Please disable it only if you really can't live"
                        " with it.<br /><br />Really disable usage tracking?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No);
        if (reply == QMessageBox::No) {
            const QSignalBlocker blocker(ui->appMetricsCheckBox);
            Q_UNUSED(blocker);
            ui->appMetricsCheckBox->setChecked(0);
        }
    }
}

/**
 * Allows the user to clear all settings and the database and exit the app
 */
void SettingsDialog::on_clearAppDataAndExitButton_clicked()
{
    if (QMessageBox::information(
            this, tr("Clear app data and exit"),
            tr("Do you really want to clear all settings, remove the "
                       "database and exit QOwnNotes?\n\n"
                       "Your notes will stay intact!"),
            tr("Clear and &exit"), tr("&Cancel"), QString::null,
            1) == 0) {
        QSettings settings;
        settings.clear();
        DatabaseService::removeDiskDatabase();
        qApp->quit();
    }
}

/**
 * Resets the font for the note text edit
 */
void SettingsDialog::on_noteTextEditResetButton_clicked()
{
    QTextEdit textEdit;
    noteTextEditFont = textEdit.font();
    setFontLabel(ui->noteTextEditFontLabel, noteTextEditFont);
}

/**
 * Resets the font for the note text code edit
 */
void SettingsDialog::on_noteTextEditCodeResetButton_clicked()
{
    QTextEdit textEdit;
    noteTextEditCodeFont = textEdit.font();
    noteTextEditCodeFont.setFamily("Courier");
    setFontLabel(ui->noteTextEditCodeFontLabel, noteTextEditCodeFont);
}

/**
 * Resets the font for the note markdown view
 */
void SettingsDialog::on_noteTextViewResetButton_clicked()
{
    QTextBrowser textView;
    noteTextViewFont = textView.font();
    setFontLabel(ui->noteTextViewFontLabel, noteTextViewFont);
}

/**
 * Resets the font for the note markdown code view
 */
void SettingsDialog::on_noteTextViewCodeResetButton_clicked()
{
    QTextBrowser textView;
    noteTextViewCodeFont = textView.font();
    noteTextViewCodeFont.setFamily("Courier");
    setFontLabel(ui->noteTextViewCodeFontLabel, noteTextViewCodeFont);
}

/**
 * Sets a path to an external editor
 */
void SettingsDialog::on_setExternalEditorPathToolButton_clicked()
{
    QStringList mimeTypeFilters;
    mimeTypeFilters << "application/x-executable" << "application/octet-stream";

    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(QDir::homePath());
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.setWindowTitle(tr("Select editor application"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.size() == 0) {
            return;
        }

        QString filePath(fileNames.at(0));
        ui->externalEditorPathLineEdit->setText(filePath);
    }
}

void SettingsDialog::on_ignoreSSLErrorsCheckBox_toggled(bool checked)
{
    ui->letsEncryptInfoLabel->setVisible(checked);
}

/**
 * Does the note folder tab setup
 */
void SettingsDialog::setupNoteFolderTab() {
    // hide the owncloud server settings
    ui->noteFolderOwnCloudServerLabel->setVisible(false);
    ui->noteFolderOwnCloudServerComboBox->setVisible(false);
    ui->noteFolderEditFrame->setEnabled(NoteFolder::countAll() > 0);
    setNoteFolderRemotePathTreeWidgetFrameVisibility(false);

    QList<NoteFolder> noteFolders = NoteFolder::fetchAll();
    int noteFoldersCount = noteFolders.count();

    // populate the note folder list
    if (noteFoldersCount > 0) {
        Q_FOREACH(NoteFolder noteFolder, noteFolders) {
                QListWidgetItem *item =
                        new QListWidgetItem(noteFolder.getName());
                item->setData(Qt::UserRole,
                              noteFolder.getId());
                ui->noteFolderListWidget->addItem(item);
            }

        // set the current row
        ui->noteFolderListWidget->setCurrentRow(0);
    }

    // disable the remove button if there is only one item
    ui->noteFolderRemoveButton->setEnabled(noteFoldersCount > 1);

    // set local path placeholder text
    ui->noteFolderLocalPathLineEdit->setPlaceholderText(
            QDir::homePath() + QDir::separator() + "ownCloud" +
                    QDir::separator() + "Notes");

    noteFolderRemotePathTreeStatusBar = new QStatusBar();
    ui->noteFolderRemotePathTreeWidgetFrame->layout()->addWidget(
            noteFolderRemotePathTreeStatusBar);
}

void SettingsDialog::on_noteFolderListWidget_currentItemChanged(
        QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous);

    setNoteFolderRemotePathTreeWidgetFrameVisibility(false);

    int noteFolderId = current->data(Qt::UserRole).toInt();
    _selectedNoteFolder = NoteFolder::fetch(noteFolderId);
    if (_selectedNoteFolder.isFetched()) {
        ui->noteFolderNameLineEdit->setText(_selectedNoteFolder.getName());
        ui->noteFolderLocalPathLineEdit->setText(
                _selectedNoteFolder.getLocalPath());
        ui->noteFolderRemotePathLineEdit->setText(
                _selectedNoteFolder.getRemotePath());

        const QSignalBlocker blocker(ui->noteFolderActiveCheckBox);
        Q_UNUSED(blocker);
        ui->noteFolderActiveCheckBox->setChecked(
                _selectedNoteFolder.isCurrent());
    }
}

void SettingsDialog::on_noteFolderAddButton_clicked()
{
    _selectedNoteFolder = NoteFolder();
    _selectedNoteFolder.setName(tr("new folder"));
    _selectedNoteFolder.setLocalPath(
            QDir::homePath() + QDir::separator() + "ownCloud" +
                    QDir::separator() + "Notes");
    _selectedNoteFolder.setPriority(ui->noteFolderListWidget->count());
    _selectedNoteFolder.setOwnCloudServerId(1);
    _selectedNoteFolder.store();

    if (_selectedNoteFolder.isFetched()) {
        QListWidgetItem *item =
                new QListWidgetItem(_selectedNoteFolder.getName());
        item->setData(Qt::UserRole, _selectedNoteFolder.getId());
        ui->noteFolderListWidget->addItem(item);

        // set the current row
        ui->noteFolderListWidget->setCurrentRow(
                ui->noteFolderListWidget->count() - 1);

        // enable the remove button
        ui->noteFolderRemoveButton->setEnabled(true);

        // focus the folder name edit and select the text
        ui->noteFolderNameLineEdit->setFocus();
        ui->noteFolderNameLineEdit->selectAll();
    }
}

/**
 * Removes the current note folder
 */
void SettingsDialog::on_noteFolderRemoveButton_clicked()
{
    if (ui->noteFolderListWidget->count() < 2) {
        return;
    }

    if (QMessageBox::information(
            this,
            tr("Remove note folder"),
            tr("Remove the current note folder <strong>%1</strong>?")
                    .arg(_selectedNoteFolder.getName()),
            tr("&Remove"), tr("&Cancel"), QString::null,
            0, 1) == 0) {
        bool wasCurrent = _selectedNoteFolder.isCurrent();

        // remove the note folder from the database
        _selectedNoteFolder.remove();

        // remove the list item
        ui->noteFolderListWidget->takeItem(
                ui->noteFolderListWidget->currentRow());

        // disable the remove button if there is only one item left
        ui->noteFolderRemoveButton->setEnabled(
                ui->noteFolderListWidget->count() > 1);

        // if the removed note folder was the current folder we set the first
        // note folder as new current one
        if (wasCurrent) {
            QList<NoteFolder> noteFolders = NoteFolder::fetchAll();
            if (noteFolders.count() > 0) {
                noteFolders[0].setAsCurrent();
            }
        }
    }
}

/**
 * Updates the name of the current note folder edit
 */
void SettingsDialog::on_noteFolderNameLineEdit_editingFinished()
{
    QString text = ui->noteFolderNameLineEdit->text();
    _selectedNoteFolder.setName(text);
    _selectedNoteFolder.store();

    ui->noteFolderListWidget->currentItem()->setText(text);
}

/**
 * Updates the remote path of the current note folder edit
 */
void SettingsDialog::on_noteFolderRemotePathLineEdit_editingFinished()
{
    QString text = ui->noteFolderRemotePathLineEdit->text();
    _selectedNoteFolder.setRemotePath(text);
    QString remotePath = _selectedNoteFolder.fixRemotePath();
    _selectedNoteFolder.store();

    // set new path if fixed path differs
    if (text != remotePath) {
        const QSignalBlocker blocker(ui->noteFolderRemotePathLineEdit);
        Q_UNUSED(blocker);

        ui->noteFolderRemotePathLineEdit->setText(remotePath);
    }
}

void SettingsDialog::on_noteFolderLocalPathButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(
            this,
            tr("Please select the folder where your notes will get stored to"),
            _selectedNoteFolder.getLocalPath(),
            QFileDialog::ShowDirsOnly);

    QDir d = QDir(dir);

    if (d.exists() && (dir != "")) {
        ui->noteFolderLocalPathLineEdit->setText(dir);
        _selectedNoteFolder.setLocalPath(dir);
        _selectedNoteFolder.store();
    }
}

/**
 * Sets the current note folder as active note folder
 */
void SettingsDialog::on_noteFolderActiveCheckBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1);

    if (!ui->noteFolderActiveCheckBox->isChecked()) {
        const QSignalBlocker blocker(ui->noteFolderActiveCheckBox);
        Q_UNUSED(blocker);
        ui->noteFolderActiveCheckBox->setChecked(true);
    } else {
        _selectedNoteFolder.setAsCurrent();
    }
}

void SettingsDialog::on_noteFolderRemotePathButton_clicked()
{
    // store ownCloud settings
    storeSettings();

    setNoteFolderRemotePathTreeWidgetFrameVisibility(true);

    noteFolderRemotePathTreeStatusBar->showMessage(
            tr("Loading folders from server"));

    OwnCloudService *ownCloud = new OwnCloudService(this);
    ownCloud->settingsGetFileList(this, "");
}

/**
 * Populates the note folder remote path tree with items
 *
 * Callback function from OwnCloudService::loadDirectory()
 */
void SettingsDialog::setNoteFolderRemotePathList(QStringList pathList) {
    if (pathList.count() <= 1) {
        noteFolderRemotePathTreeStatusBar->showMessage(
                tr("No more folders were found in the current folder"), 1000);
    } else {
        noteFolderRemotePathTreeStatusBar->clearMessage();
    }

    Q_FOREACH(QString path, pathList) {
            if (!path.isEmpty()) {
                addPathToNoteFolderRemotePathTreeWidget(NULL, path);
            }
        }
}

void SettingsDialog::addPathToNoteFolderRemotePathTreeWidget(
        QTreeWidgetItem *parent, QString path) {
    if (path.isEmpty()) {
        return;
    }

    QStringList pathPartList = path.split("/");
    QString pathPart = pathPartList.takeFirst();
    QTreeWidgetItem *item = findNoteFolderRemotePathTreeWidgetItem(
            parent, pathPart);

    const QSignalBlocker blocker(ui->noteFolderRemotePathTreeWidget);
    Q_UNUSED(blocker);

    if (item == NULL) {
        item = new QTreeWidgetItem();
        item->setText(0, pathPart);
        if (parent == NULL) {
            ui->noteFolderRemotePathTreeWidget->addTopLevelItem(item);
        } else {
            parent->addChild(item);
            parent->setExpanded(true);
        }
    }

    if (pathPartList.count() > 0) {
        addPathToNoteFolderRemotePathTreeWidget(item, pathPartList.join("/"));
    }
}

QTreeWidgetItem *SettingsDialog::findNoteFolderRemotePathTreeWidgetItem(
        QTreeWidgetItem *parent, QString text) {
    if (parent == NULL) {
        for (int i = 0;
            i < ui->noteFolderRemotePathTreeWidget->topLevelItemCount();
            i++) {
            QTreeWidgetItem *item =
                    ui->noteFolderRemotePathTreeWidget->topLevelItem(i);
            if (item->text(0) == text) {
                return item;
            }
        }
    } else {
        for (int i = 0; i < parent->childCount(); i++) {
            QTreeWidgetItem *item = parent->child(i);
            if (item->text(0) == text) {
                return item;
            }
        }
    }

    return NULL;
}

void SettingsDialog::on_noteFolderRemotePathTreeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);

    QString folderName =
            generatePathFromCurrentNoteFolderRemotePathItem(current);
    noteFolderRemotePathTreeStatusBar->showMessage(
            tr("Loading folders in '%1' from server").arg(current->text(0)));

    OwnCloudService *ownCloud = new OwnCloudService(this);
    ownCloud->settingsGetFileList(this, folderName);
}

void SettingsDialog::on_useOwnCloudPathButton_clicked()
{
    QTreeWidgetItem *item = ui->noteFolderRemotePathTreeWidget->currentItem();
    if (item == NULL) {
        return;
    }

    ui->noteFolderRemotePathLineEdit->clear();
    ui->noteFolderRemotePathLineEdit->setText(
            generatePathFromCurrentNoteFolderRemotePathItem(item));
    setNoteFolderRemotePathTreeWidgetFrameVisibility(false);
    on_noteFolderRemotePathLineEdit_editingFinished();
}

/**
 * Recursively generates the path string from the tree widget items
 */
QString SettingsDialog::generatePathFromCurrentNoteFolderRemotePathItem(
        QTreeWidgetItem *item) {
    if (item == NULL) {
        return "";
    }

    QTreeWidgetItem *parent = item->parent();
    if (parent != NULL) {
        return generatePathFromCurrentNoteFolderRemotePathItem(parent)
               + "/" + item->text(0);
    }

    return item->text(0);
}

void SettingsDialog::setNoteFolderRemotePathTreeWidgetFrameVisibility(
        bool visible) {
    ui->noteFolderRemotePathTreeWidgetFrame->setVisible(visible);
    ui->noteFolderVerticalSpacerFrame->setVisible(!visible);
    if (!visible) {
        const QSignalBlocker blocker(ui->noteFolderRemotePathTreeWidget);
        Q_UNUSED(blocker);

        ui->noteFolderRemotePathTreeWidget->clear();
    }
}
