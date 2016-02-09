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

SettingsDialog::SettingsDialog(SimpleCrypt *crypto, QWidget *parent) :
        MasterDialog(parent),
        ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    ui->connectionTestLabel->hide();
    ui->connectButton->setDefault(true);
    ui->noteSaveIntervalTime->setToolTip(
            ui->noteSaveIntervalTimeLabel->toolTip());
    ui->tabWidget->setCurrentIndex(0);

    for (int i = 0; i <= 8; i++) {
        setOKLabelData(i, "unknown", SettingsDialog::Unknown);
    }

    QString html = ui->installInfotextBrowser->toHtml();

    // increase font size for OSX
#ifdef Q_OS_MAC
    QString styleSheet = ui->installInfotextBrowser->styleSheet();
    styleSheet.replace("font-size: 9pt;", "font-size: 13pt;");
    ui->installInfotextBrowser->setStyleSheet(styleSheet);

    html.replace("font-size:9pt", "font-size:13pt;");
#endif

    // add information text for Windows users to install OpenSSL
#ifdef Q_OS_WIN32
    //    html += "<p>If your ownCloud server uses SSL (https) you also need to install <a href='https://www.openssl.org'>OpenSSL</a> into your system directory.</p>";
    //    html += "<p>You can find binary packages of OpenSSL <a href='http://slproweb.com/products/Win32OpenSSL.html'>here</a>.</p>";
#endif

    html.replace("QOWNNOTESAPI_MIN_VERSION", QOWNNOTESAPI_MIN_VERSION);
    ui->installInfotextBrowser->setHtml(html);

    this->crypto = crypto;
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
 * @brief Starts a connection test
 */
void SettingsDialog::startConnectionTest() {
    ui->connectionTestLabel->hide();
    OwnCloudService *ownCloud = new OwnCloudService(crypto, this);
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
                      crypto->encryptToString(ui->passwordEdit->text()));
    settings.setValue("ownCloud/localOwnCloudPath",
                      ui->localOwnCloudPathEdit->text());
    settings.setValue("notifyAllExternalModifications",
                      ui->notifyAllExternalModificationsCheckBox->isChecked());
    settings.setValue("noteSaveIntervalTime",
                      ui->noteSaveIntervalTime->value());
    settings.setValue("defaultNoteFileExtension",
                      getSelectedListWidgetValue(
                              ui->defaultNoteFileExtensionListWidget));
    settings.setValue("MainWindow/noteTextEdit.font",
                      noteTextEditFont.toString());
    settings.setValue("MainWindow/noteTextEdit.code.font",
                      noteTextEditCodeFont.toString());
    settings.setValue("MainWindow/noteTextView.font",
                      noteTextViewFont.toString());
    settings.setValue("MainWindow/mainToolBar.iconSize",
                      QString::number(ui->toolbarIconSizeSpinBox->value()));
    settings.setValue("MainWindow/showRecentNoteFolderInMainArea",
                      ui->showRecentNoteFolderCheckBox->isChecked());
    settings.setValue("MainWindow/markdownDefaultViewMode",
                      ui->markdownDefaultViewModeCheckBox->isChecked());
    settings.setValue("interfaceLanguage",
                      getSelectedListWidgetValue(ui->languageListWidget));

    if (!settings.value("appMetrics/disableTracking").toBool() &&
            ui->appMetricsCheckBox->isChecked()) {
        MetricsService::instance()->sendEvent("settings", "app metrics "
                "disabled");
    }

    settings.setValue("appMetrics/disableTracking",
                      ui->appMetricsCheckBox->isChecked());

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
}

void SettingsDialog::readSettings() {
    QSettings settings;
    ui->serverUrlEdit->setText(settings.value("ownCloud/serverUrl").toString());
    ui->userNameEdit->setText(settings.value("ownCloud/userName").toString());
    ui->passwordEdit->setText(crypto->decryptToString(
            settings.value("ownCloud/password").toString()));
    ui->localOwnCloudPathEdit->setText(
            settings.value("ownCloud/localOwnCloudPath").toString());
    ui->notifyAllExternalModificationsCheckBox->setChecked(
            settings.value("notifyAllExternalModifications").toBool());
    ui->noteSaveIntervalTime->setValue(
            settings.value("noteSaveIntervalTime").toInt());
    ui->showRecentNoteFolderCheckBox->setChecked(settings.value(
            "MainWindow/showRecentNoteFolderInMainArea").toBool());
    ui->toolbarIconSizeSpinBox->setValue(
                 settings.value("MainWindow/mainToolBar.iconSize").toInt());
    ui->markdownDefaultViewModeCheckBox->setChecked(
            settings.value("MainWindow/markdownDefaultViewMode").toBool());

    selectListWidgetValue(ui->languageListWidget,
                          settings.value("interfaceLanguage").toString());
    selectListWidgetValue(ui->defaultNoteFileExtensionListWidget,
                          Note::defaultNoteFileExtension());

    const QSignalBlocker blocker(ui->appMetricsCheckBox);
    Q_UNUSED(blocker);
    ui->appMetricsCheckBox->setChecked(settings.value(
            "appMetrics/disableTracking").toBool());

    noteTextEditFont.fromString(
            settings.value("MainWindow/noteTextEdit.font").toString());
    setFontLabel(ui->noteTextEditFontLabel, noteTextEditFont);

    noteTextEditCodeFont.fromString(
            settings.value("MainWindow/noteTextEdit.code.font").toString());
    setFontLabel(ui->noteTextEditCodeFontLabel, noteTextEditCodeFont);

    noteTextViewFont.fromString(
            settings.value("MainWindow/noteTextView.font").toString());
    setFontLabel(ui->noteTextViewFontLabel, noteTextViewFont);

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

void SettingsDialog::setFontLabel(QLabel *label, QFont font) {
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
            "GAnalytics-cid");

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
        connectionErrorMessage.replace(ui->passwordEdit->text(), "***");

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
        case 5:
            label = ui->ok5Label;
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

/**
 * select the local ownCloud directory
 */
void SettingsDialog::on_localOwnCloudDirectoryButton_clicked() {
    QString path = ui->localOwnCloudPathEdit->text();

    if (path == "") {
        path = QDir::homePath() + QDir::separator() + "ownCloud";
    }

    QString dir = QFileDialog::getExistingDirectory(
            this,
            tr("Select ownCloud base directory"),
            path,
            QFileDialog::ShowDirsOnly);

    if (dir != "") {
        ui->localOwnCloudPathEdit->setText(dir);
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

void SettingsDialog::on_reloadCalendarListButton_clicked() {
    // we need to store the calendar backend
    storeSettings();

    OwnCloudService *ownCloud = new OwnCloudService(crypto, this);
    ownCloud->settingsGetCalendarList(this);
}

void SettingsDialog::on_defaultOwnCloudCalendarRadioButton_toggled(
        bool checked) {
    on_reloadCalendarListButton_clicked();
}

void SettingsDialog::on_reinitializeDatabaseButton_clicked() {
    DatabaseService::reinitializeDiskDatabase();
    QMessageBox::information(this, tr("Database"),
                             tr("The Database was reinitialized."));
}

/**
 * @brief Clears the recent note folder history in the settings
 */
void SettingsDialog::on_clearRecentNotesFoldersHistoryButton_clicked() {
    QSettings settings;
    settings.remove("recentNoteFolders");

    QMessageBox::information(this, tr("Recent note folders history"),
                             tr("The history was cleared."));
}

void SettingsDialog::on_tabWidget_currentChanged(int index) {
    if (index == DebugTab) {
        outputSettings();
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
                QMessageBox::Yes|QMessageBox::No,
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
