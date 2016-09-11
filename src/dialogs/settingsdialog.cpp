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
#include <QMenu>
#include <QAction>
#include <services/metricsservice.h>
#include "helpers/clientproxy.h"
#include <QtNetwork/qnetworkproxy.h>
#include <services/cryptoservice.h>
#include <entities/notefolder.h>
#include <QTextBrowser>
#include <entities/script.h>
#include <services/scriptingservice.h>
#include <QInputDialog>
#include <QFontDatabase>
#include <utils/misc.h>
#include <QKeySequence>
#include <QKeySequenceEdit>
#include <libraries/qkeysequencewidget/qkeysequencewidget/src/qkeysequencewidget.h>

SettingsDialog::SettingsDialog(int page, QWidget *parent) :
        MasterDialog(parent), ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    // we must not do that because XFCE really resizes the window to 1x1
//    resize(1, 1);

    ui->connectionTestLabel->hide();
    ui->connectButton->setDefault(true);
    ui->noteSaveIntervalTime->setToolTip(
            ui->noteSaveIntervalTimeLabel->toolTip());
    ui->removeCustomNoteFileExtensionButton->setDisabled(true);
    ui->calDavCalendarGroupBox->setVisible(false);

    for (int i = 0; i <= 8; i++) {
        setOKLabelData(i, "unknown", SettingsDialog::Unknown);
    }

    // add the QOwnNotesAPI minimum version number to the info text
    QString html = ui->installInfoTextLabel1->text();
    html.replace("QOWNNOTESAPI_MIN_VERSION", QOWNNOTESAPI_MIN_VERSION);
    ui->installInfoTextLabel1->setText(html);

    // do the network proxy tab setup
    setupProxyPage();

    // setup the note folder tab
    setupNoteFolderPage();

    // setup the scripting tab
    setupScriptingPage();

    readSettings();

    // initializes the main splitter
    initMainSplitter();

    if (ui->serverUrlEdit->text() != "") {
        // start a connection test
        startConnectionTest();
    }

    // init the debug info search frame
    ui->debugInfoTextEdit->initSearchFrame(ui->debugInfoTextEditSearchFrame);

    // must be done in the end so that the settings are loaded first when
    // doing a connection test
    ui->settingsStackedWidget->setCurrentIndex(page);

    // update other stuff for the settings tree widget
    if (ui->settingsStackedWidget->currentIndex() == page) {
        on_settingsStackedWidget_currentChanged(page);
    }

#ifdef Q_OS_MAC
    // we don't need app instance settings on OS X
    ui->appInstanceGroupBox->setVisible(false);
    ui->allowOnlyOneAppInstanceCheckBox->setChecked(false);
#endif

    MainWindow *mainWindow = MainWindow::instance();

    // disable the shortcut page if there is no main window yet
    if (mainWindow == Q_NULLPTR) {
        QTreeWidgetItem *item = findSettingsTreeWidgetItemByPage(ShortcutPage);
        if (item != Q_NULLPTR) {
            item->setDisabled(true);
        }
    }

    // expand all items in the settings tree widget
    ui->settingsTreeWidget->expandAll();

    // TODO(pbek): remove if everything is working
    ui->calDavCalendarRadioButton->setText(
            ui->calDavCalendarRadioButton->text() +
            " (this is still in early Alpha!)");
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

/**
 * Does the network proxy page setup
 */
void SettingsDialog::setupProxyPage() {
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
    int type = settings.value("networking/proxyType",
                              QNetworkProxy::NoProxy).toInt();
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
                      CryptoService::instance()->encryptToString(
                              ui->passwordEdit->text()));
    settings.setValue("disableAutomaticUpdateDialog",
                      ui->disableAutomaticUpdateDialogCheckBox->isChecked());
    settings.setValue("notifyAllExternalModifications",
                      ui->notifyAllExternalModificationsCheckBox->isChecked());
    settings.setValue("ignoreAllExternalModifications",
                      ui->ignoreAllExternalModificationsCheckBox->isChecked());
    settings.setValue("ignoreAllExternalNoteFolderChanges",
                      ui->ignoreAllExternalNoteFolderChangesCheckBox
                              ->isChecked());
    settings.setValue("allowDifferentNoteFileName",
                      ui->allowDifferentNoteFileNameCheckBox->isChecked());
    settings.setValue("noteSaveIntervalTime",
                      ui->noteSaveIntervalTime->value());
    settings.setValue("defaultNoteFileExtension",
                      getSelectedListWidgetValue(
                              ui->defaultNoteFileExtensionListWidget));
    settings.setValue("externalEditorPath",
                      ui->externalEditorPathLineEdit->text());
    settings.setValue("MainWindow/mainToolBar.iconSize",
                      QString::number(ui->toolbarIconSizeSpinBox->value()));
    settings.setValue("MainWindow/showRecentNoteFolderInMainArea",
                      ui->showNoteFolderCheckBox->isChecked());
    settings.setValue("allowOnlyOneAppInstance",
                      ui->allowOnlyOneAppInstanceCheckBox->isChecked());
    settings.setValue("interfaceLanguage",
                      getSelectedListWidgetValue(ui->languageListWidget));
    settings.setValue("markdownHighlightingInterval",
                      ui->markdownHighlightingCheckBox->isChecked() ?
                      ui->markdownHighlightingIntervalSpinBox->value() : 0);

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

    settings.setValue("darkMode",
                      ui->darkModeCheckBox->isChecked());

    settings.setValue("darkModeTrayIcon",
                      ui->darkModeTrayIconCheckBox->isChecked());

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

    // store the tasks calendar data to the settings
    settings.setValue("ownCloud/todoCalendarUrlList", todoCalendarUrlList);
    settings.setValue("ownCloud/todoCalendarEnabledList",
                      todoCalendarEnabledList);
    settings.setValue("ownCloud/todoCalendarEnabledUrlList",
                      todoCalendarEnabledUrlList);

    int todoCalendarBackend = OwnCloudService::DefaultOwnCloudCalendar;

    if (ui->calendarPlusRadioButton->isChecked()) {
        todoCalendarBackend = OwnCloudService::CalendarPlus;
    } else if (ui->calDavCalendarRadioButton->isChecked()) {
        todoCalendarBackend = OwnCloudService::CalDAVCalendar;
    }

    settings.setValue("ownCloud/todoCalendarBackend", todoCalendarBackend);

    settings.setValue("ownCloud/todoCalendarCalDAVServerUrl",
                      ui->calDavServerUrlEdit->text());
    settings.setValue("ownCloud/todoCalendarCalDAVUsername",
                      ui->calDavUsernameEdit->text());
    settings.setValue("ownCloud/todoCalendarCalDAVPassword",
                      CryptoService::instance()->encryptToString(
                              ui->calDavPasswordEdit->text()));

    settings.setValue("networking/ignoreSSLErrors",
                      ui->ignoreSSLErrorsCheckBox->isChecked());

    // store the custom note file extensions
    QStringList customNoteFileExtensionList;
    for (int i = 2; i < ui->defaultNoteFileExtensionListWidget->count(); i++) {
        QListWidgetItem *item = ui->defaultNoteFileExtensionListWidget->item(i);

        customNoteFileExtensionList.append(item->whatsThis());
    }
    settings.setValue("customNoteFileExtensionList",
                      customNoteFileExtensionList);

    // store the font settings
    storeFontSettings();

    // store the proxy settings
    storeProxySettings();

    // store the enabled state of the scripts
    storeScriptListEnabledState();

    // store the shortcut settings
    storeShortcutSettings();

    // store the splitter settings
    storeSplitterSettings();
}

/**
 * Stores the font settings
 */
void SettingsDialog::storeFontSettings() {
    QSettings settings;
    settings.setValue("MainWindow/noteTextEdit.font",
                      noteTextEditFont.toString());
    settings.setValue("MainWindow/noteTextEdit.code.font",
                      noteTextEditCodeFont.toString());
    settings.setValue("MainWindow/noteTextView.font",
                      noteTextViewFont.toString());
    settings.setValue("MainWindow/noteTextView.code.font",
                      noteTextViewCodeFont.toString());
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
    ui->ignoreAllExternalModificationsCheckBox->setChecked(
            settings.value("ignoreAllExternalModifications").toBool());
    ui->ignoreAllExternalNoteFolderChangesCheckBox->setChecked(
            settings.value("ignoreAllExternalNoteFolderChanges").toBool());
    ui->allowDifferentNoteFileNameCheckBox->setChecked(
            settings.value("allowDifferentNoteFileName").toBool());
    ui->noteSaveIntervalTime->setValue(
            settings.value("noteSaveIntervalTime", 10).toInt());

    const QSignalBlocker blocker3(ui->markdownHighlightingCheckBox);
    Q_UNUSED(blocker3);
    int markdownHighlightingInterval =
            settings.value("markdownHighlightingInterval",
                           _defaultMarkdownHighlightingInterval).toInt();
    bool markdownHighlightingEnabled = markdownHighlightingInterval > 0;
    ui->markdownHighlightingCheckBox->setChecked(markdownHighlightingEnabled);
    ui->markdownHighlightingIntervalSpinBox->setValue(
            markdownHighlightingInterval);
    on_markdownHighlightingCheckBox_toggled(markdownHighlightingEnabled);

    ui->showNoteFolderCheckBox->setChecked(settings.value(
            "MainWindow/showRecentNoteFolderInMainArea").toBool());
    ui->allowOnlyOneAppInstanceCheckBox->setChecked(settings.value(
            "allowOnlyOneAppInstance").toBool());
    ui->toolbarIconSizeSpinBox->setValue(
                 settings.value("MainWindow/mainToolBar.iconSize").toInt());

    selectListWidgetValue(ui->languageListWidget,
                          settings.value("interfaceLanguage").toString());

    const QSignalBlocker blocker(ui->appMetricsCheckBox);
    Q_UNUSED(blocker);
    ui->appMetricsCheckBox->setChecked(settings.value(
            "appMetrics/disableTracking").toBool());

    ui->appHeartbeatCheckBox->setChecked(settings.value(
            "appMetrics/disableAppHeartbeat").toBool());

    ui->darkModeColorsCheckBox->setChecked(settings.value(
            "darkModeColors").toBool());

    ui->darkModeCheckBox->setChecked(settings.value(
            "darkMode").toBool());

    ui->darkModeTrayIconCheckBox->setChecked(settings.value(
            "darkModeTrayIcon").toBool());

    // toggle the dark mode colors check box with the dark mode checkbox
    on_darkModeCheckBox_toggled();

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

    const QSignalBlocker blocker4(this->ui->calendarPlusRadioButton);
    Q_UNUSED(blocker4);

    const QSignalBlocker blocker5(this->ui->calDavCalendarRadioButton);
    Q_UNUSED(blocker5);

    switch (settings.value("ownCloud/todoCalendarBackend").toInt()) {
        case OwnCloudService::CalendarPlus:
            ui->calendarPlusRadioButton->setChecked(true);
            break;
        case OwnCloudService::CalDAVCalendar:
            ui->calDavCalendarRadioButton->setChecked(true);
            ui->calDavCalendarGroupBox->setVisible(true);
            break;
        default:
            ui->defaultOwnCloudCalendarRadioButton->setChecked(true);
            break;
    }

    // reload the calendar list
    reloadCalendarList();

    ui->calDavServerUrlEdit->setText(settings.value(
            "ownCloud/todoCalendarCalDAVServerUrl").toString());
    ui->calDavUsernameEdit->setText(settings.value(
            "ownCloud/todoCalendarCalDAVUsername").toString());
    ui->calDavPasswordEdit->setText(CryptoService::instance()->decryptToString(
            settings.value("ownCloud/todoCalendarCalDAVPassword").toString()));

    QStringList todoCalendarUrlList = settings.value(
            "ownCloud/todoCalendarUrlList").toStringList();
    // load the tasks calendar list and set the checked state
    refreshTodoCalendarList(todoCalendarUrlList, true);

    // loads the custom note file extensions
    QListIterator<QString> itr(Note::customNoteFileExtensionList());
    while (itr.hasNext()) {
        QString fileExtension = itr.next();
        addCustomeNoteFileExtension(fileExtension);
    }

    selectListWidgetValue(ui->defaultNoteFileExtensionListWidget,
                          Note::defaultNoteFileExtension());

    bool ignoreSSLErrors =
            settings.value("networking/ignoreSSLErrors", true).toBool();
    ui->ignoreSSLErrorsCheckBox->setChecked(ignoreSSLErrors);
    ui->letsEncryptInfoLabel->setVisible(ignoreSSLErrors);

    // load the proxy settings
    loadProxySettings();

    // load the shortcut settings
    loadShortcutSettings();
}

/**
 * Loads the shortcut settings
 */
void SettingsDialog::loadShortcutSettings() {
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow == Q_NULLPTR) {
        return;
    }

    QSettings settings;
    bool darkMode = settings.value("darkMode").toBool();

    QPalette palette;
    QColor shortcutButtonActiveColor = darkMode ?
                                       Qt::white :
                                       palette.color(QPalette::ButtonText);
    QColor shortcutButtonInactiveColor = darkMode ?
                                         Qt::darkGray :
                                         palette.color(QPalette::Mid);

    _keyWidgetSignalMapper = new QSignalMapper(this);
    QList<QMenu*> menus = mainWindow->menuList();
    ui->shortcutTreeWidget->setColumnCount(2);

    // loop through all menus
    foreach(QMenu* menu, menus) {
            QTreeWidgetItem *menuItem = new QTreeWidgetItem();
            int actionCount = 0;

            // loop through all actions of the menu
            foreach(QAction* action, menu->actions()) {
                    // we don't need empty objects
                    if (action->objectName().isEmpty()) {
                        continue;
                    }

                    // create the tree widget item
                    QTreeWidgetItem *actionItem = new QTreeWidgetItem();
                    actionItem->setText(0, action->text().remove("&"));
                    actionItem->setToolTip(0, action->objectName());
                    actionItem->setData(Qt::UserRole, 1, action->objectName());
                    menuItem->addChild(actionItem);

                    // create the key widget
                    QKeySequenceWidget *keyWidget = new QKeySequenceWidget();
                    keyWidget->setFixedWidth(300);
                    keyWidget->setClearButtonIcon(QIcon::fromTheme(
                            "edit-clear",
                            QIcon(":/icons/breeze-qownnotes/16x16/"
                                          "edit-clear.svg")));
                    keyWidget->setNoneText(tr("Undefined key"));
                    keyWidget->setShortcutButtonActiveColor(
                            shortcutButtonActiveColor);
                    keyWidget->setShortcutButtonInactiveColor(
                            shortcutButtonInactiveColor);
                    keyWidget->setToolTip(tr("Assign a new key"),
                                          tr("Reset to default key"));
                    keyWidget->setDefaultKeySequence(action->data().toString());
                    keyWidget->setKeySequence(action->shortcut());

                    ui->shortcutTreeWidget->setItemWidget(
                            actionItem, 1, keyWidget);

                    actionCount++;
            }

            if (actionCount > 0) {
                menuItem->setText(0, menu->title().remove("&"));
                menuItem->setToolTip(0, menu->objectName());
                ui->shortcutTreeWidget->addTopLevelItem(menuItem);
                menuItem->setExpanded(true);
            }
    }

    ui->shortcutTreeWidget->resizeColumnToContents(0);
}

/**
 * Stores the shortcut settings
 */
void SettingsDialog::storeShortcutSettings() {
    QSettings settings;

    // loop all top level tree widget items (menus)
    for (int i = 0; i < ui->shortcutTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *menuItem = ui->shortcutTreeWidget->topLevelItem(i);

        // loop all tree widget items of the menu (action shortcuts)
        for (int j = 0; j < menuItem->childCount(); j++) {
            QTreeWidgetItem *shortcutItem = menuItem->child(j);
            QKeySequenceWidget *keyWidget =
                    static_cast<QKeySequenceWidget *>(
                            ui->shortcutTreeWidget->itemWidget(
                                    shortcutItem, 1));

            if (keyWidget == Q_NULLPTR) {
                continue;
            }

            QKeySequence keySequence = keyWidget->keySequence();
            QKeySequence defaultKeySequence = keyWidget->defaultKeySequence();
            QString actionObjectName =
                    shortcutItem->data(Qt::UserRole, 1).toString();

            QString settingsKey = "Shortcuts/MainWindow-" + actionObjectName;

            // remove or store the setting for the shortcut if it's not default
            if (keySequence == defaultKeySequence) {
                settings.remove(settingsKey);
            } else if (!keySequence.isEmpty()) {
                settings.setValue(settingsKey, keySequence);
            }
        }
    }
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
    output += prepareDebugInformationLine("Release",
                                          qApp->property("release").toString());
    output += prepareDebugInformationLine("Qt Version (build)", QT_VERSION_STR);
    output += prepareDebugInformationLine("Qt Version (runtime)", qVersion());

    QString debug = "0";
#ifdef QT_DEBUG
    debug = "1";
#endif

    output += prepareDebugInformationLine("Qt Debug", debug);

    output += prepareDebugInformationLine("Locale (system)",
                                          QLocale::system().name());
    output += prepareDebugInformationLine("Locale (interface)",
                                          settings.value("interfaceLanguage")
                                                  .toString());

    output += prepareDebugInformationLine("Icon theme",
                                          QIcon::themeName());
    output += prepareDebugInformationLine("Notes in current note folder",
                                          QString::number(Note::countAll()));
    output += prepareDebugInformationLine("Enabled scripts",
                                          QString::number(
                                                  Script::countEnabled()));

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

    // add note folder information
    output += "\n## Note folders\n\n";
    output += prepareDebugInformationLine(
            "currentNoteFolderId",
            QString::number(NoteFolder::currentNoteFolderId()));

    QList<NoteFolder> noteFolders = NoteFolder::fetchAll();
    if (noteFolders.count() > 0) {
        Q_FOREACH(NoteFolder noteFolder, noteFolders) {
                output += "\n### " + noteFolder.getName() + "\n\n";
                output += prepareDebugInformationLine(
                        "id", QString::number(noteFolder.getId()));
                output += prepareDebugInformationLine(
                        "activeTagId",
                        QString::number(noteFolder.getActiveTagId()));
                output += prepareDebugInformationLine(
                        "localPath", noteFolder.getLocalPath());
                output += prepareDebugInformationLine(
                        "remotePath", noteFolder.getRemotePath());
            }
    }

    // add information about the settings
    output += "\n## Settings\n\n";

    // hide values of these keys
    QStringList keyHiddenList = (QStringList() <<
            "cryptoKey" <<
            "ownCloud/password" <<
            "ownCloud/todoCalendarCalDAVPassword" <<
            "PiwikClientId" <<
            "networking/proxyPassword");

    // under OS X we have to ignore some keys
#ifdef Q_OS_MAC
    QStringList keyIgnoreList;
    keyIgnoreList << "AKDeviceUnlockState" << "Apple" << "NS" << "NavPanel"
    << "com/apple";
#endif

    QListIterator<QString> itr(settings.allKeys());
    while (itr.hasNext()) {
        QString key = itr.next();
        QVariant value = settings.value(key);

        // under OS X we have to ignore some keys
#ifdef Q_OS_MAC
        bool ignoreKey = false;

        // ignore values of certain keys
        QListIterator<QString> itr2(keyIgnoreList);
        while (itr2.hasNext()) {
            QString pattern = itr2.next();
            if (key.startsWith(pattern)) {
                ignoreKey = true;
                break;
            }
        }

        // check if key has to be ignored
        if (ignoreKey) {
            continue;
        }
#endif

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
    // add two spaces if we don't want GitHub line breaks
    QString spaces = ui->gitHubLineBreaksCheckBox->isChecked() ? "" : "  ";

    data = (data == "") ? "*empty*" : "`" + data + "`";
    return "**" + headline + "**: " + data + spaces + "\n";
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
    // tasks calendar list was not empty
    bool readCheckedState = forceReadCheckedState ? true :
                            ui->todoCalendarListWidget->count() > 0;

    // clear the tasks calendar list
    ui->todoCalendarListWidget->clear();

    QSettings settings;
    QStringList todoCalendarEnabledList = settings.value(
            "ownCloud/todoCalendarEnabledList").toStringList();

    QUrl serverUrl(ui->calDavCalendarRadioButton->isChecked() ?
                   ui->calDavServerUrlEdit->text() :
                   ui->serverUrlEdit->text());

    // return if server url isn't valid
    if (!serverUrl.isValid()) {
        return;
    }

    QString serverUrlText(serverUrl.toString());
    QString serverUrlPath = serverUrl.path();
    if (serverUrlPath != "") {
        // remove the path from the end because we already got it in the url
        serverUrlText.replace(QRegularExpression(
                QRegularExpression::escape(serverUrlPath) + "$"), "");
    }

    QListIterator<QString> itr(items);
    while (itr.hasNext()) {
        QString url = itr.next();

        // only add the server url if it wasn't already added
        if (!url.startsWith(serverUrlText)) {
            url = serverUrlText + url;
        }

        // get the name out of the url part
        QRegularExpression regex("\\/([^\\/]*)\\/$");
        QRegularExpressionMatch match = regex.match(url);
        QString name = match.captured(1);

        // remove percent encoding
        name = QUrl::fromPercentEncoding(name.toUtf8());

        // skip the contact birthdays calendar
        if (name == "contact_birthdays") {
            continue;
        }

        // skip the Calendar Plus birthday calendar
        if (name.startsWith("bdaycpltocal_")) {
            continue;
        }

        // create the list widget item and add it to the
        // tasks calendar list widget
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

        // store the font settings
        storeFontSettings();

        // update the text items after the font was changed
        ui->editorFontColorWidget->updateAllTextItems();
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

        // store the font settings
        storeFontSettings();

        // update the text items after the font was changed
        ui->editorFontColorWidget->updateAllTextItems();
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

    // reload the calendar list
    reloadCalendarList();
}

/**
 * Reloads the calendar list
 */
void SettingsDialog::reloadCalendarList() {
    OwnCloudService *ownCloud = new OwnCloudService(this);
    ownCloud->settingsGetCalendarList(this);
}

void SettingsDialog::on_defaultOwnCloudCalendarRadioButton_toggled(
        bool checked) {
    if (checked) {
        on_reloadCalendarListButton_clicked();
    }
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

void SettingsDialog::on_appMetricsCheckBox_toggled(bool checked) {
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
void SettingsDialog::on_clearAppDataAndExitButton_clicked() {
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
void SettingsDialog::on_noteTextEditResetButton_clicked() {
    QTextEdit textEdit;
    noteTextEditFont = textEdit.font();
    setFontLabel(ui->noteTextEditFontLabel, noteTextEditFont);

    // store the font settings
    storeFontSettings();

    // update the text items after the font was changed
    ui->editorFontColorWidget->updateAllTextItems();
}

/**
 * Resets the font for the note text code edit
 */
void SettingsDialog::on_noteTextEditCodeResetButton_clicked() {
    noteTextEditCodeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFontLabel(ui->noteTextEditCodeFontLabel, noteTextEditCodeFont);

    // store the font settings
    storeFontSettings();

    // update the text items after the font was changed
    ui->editorFontColorWidget->updateAllTextItems();
}

/**
 * Resets the font for the note markdown view
 */
void SettingsDialog::on_noteTextViewResetButton_clicked() {
    QTextBrowser textView;
    noteTextViewFont = textView.font();
    setFontLabel(ui->noteTextViewFontLabel, noteTextViewFont);
}

/**
 * Resets the font for the note markdown code view
 */
void SettingsDialog::on_noteTextViewCodeResetButton_clicked() {
    noteTextViewCodeFont = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    setFontLabel(ui->noteTextViewCodeFontLabel, noteTextViewCodeFont);
}

/**
 * Sets a path to an external editor
 */
void SettingsDialog::on_setExternalEditorPathToolButton_clicked() {
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

void SettingsDialog::on_ignoreSSLErrorsCheckBox_toggled(bool checked) {
    ui->letsEncryptInfoLabel->setVisible(checked);
}

/**
 * Does the note folder page setup
 */
void SettingsDialog::setupNoteFolderPage() {
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
            Utils::Misc::defaultNotesPath());

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
        ui->noteFolderShowSubfoldersCheckBox->setChecked(
                _selectedNoteFolder.isShowSubfolders());

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
            Utils::Misc::defaultNotesPath());
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

        QSettings settings;

        // remove saved searches
        QString settingsKey = "savedSearches/noteFolder-"
                              + QString::number(_selectedNoteFolder.getId());
        settings.remove(settingsKey);

        // remove tree widget expand state setting
        settingsKey = NoteSubFolder::treeWidgetExpandStateSettingsKey(
                _selectedNoteFolder.getId());
        settings.remove(settingsKey);

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

/**
 * Does the scripting page setup
 */
void SettingsDialog::setupScriptingPage() {
    QList<Script> scripts = Script::fetchAll();
    int scriptsCount = scripts.count();

    // populate the script list
    if (scriptsCount > 0) {
        Q_FOREACH(Script script, scripts) {
                QListWidgetItem *item =
                        new QListWidgetItem(script.getName());
                item->setData(Qt::UserRole,
                              script.getId());
                item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
                item->setCheckState(
                        script.getEnabled() ? Qt::Checked : Qt::Unchecked);
                ui->scriptListWidget->addItem(item);
            }

        // set the current row
        ui->scriptListWidget->setCurrentRow(0);
    }

    // disable the edit frame if there is no item
    ui->scriptEditFrame->setEnabled(scriptsCount > 0);

    // disable the remove button if there is no item
    ui->scriptRemoveButton->setEnabled(scriptsCount > 0);

    QString issueUrl = "https://github.com/pbek/QOwnNotes/issues";
    QString exampleUrl =
            "https://github.com/pbek/QOwnNotes/tree/develop/doc/scripting";
    ui->scriptInfoLabel->setText(
            tr("If you need access to a certain functionality in QOwnNotes "
                       "please open an issue on the <a href=\"%1\">"
                       "QOwnNotes issue page</a>. Click <a href=\"%2\">"
                       "QML examples</a> for a place to start with your "
                       "scripting.").arg(issueUrl, exampleUrl));
}

/**
 * Adds a new script
 */
void SettingsDialog::on_scriptAddButton_clicked() {
    _selectedScript = Script();
    _selectedScript.setName(tr("new script"));
    _selectedScript.setPriority(ui->scriptListWidget->count());
    _selectedScript.store();

    if (_selectedScript.isFetched()) {
        QListWidgetItem *item =
                new QListWidgetItem(_selectedScript.getName());
        item->setData(Qt::UserRole, _selectedScript.getId());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(Qt::Checked);
        ui->scriptListWidget->addItem(item);

        // set the current row
        ui->scriptListWidget->setCurrentRow(
                ui->scriptListWidget->count() - 1);

        // enable the remove button
        ui->scriptRemoveButton->setEnabled(true);

        // focus the script name edit and select the text
        ui->scriptNameLineEdit->setFocus();
        ui->scriptNameLineEdit->selectAll();
    }
}

/**
 * Removes the current script
 */
void SettingsDialog::on_scriptRemoveButton_clicked() {
    if (ui->scriptListWidget->count() < 1) {
        return;
    }

    if (QMessageBox::information(
            this,
            tr("Remove script"),
            tr("Remove the current script <strong>%1</strong>?")
                    .arg(_selectedScript.getName()),
            tr("&Remove"), tr("&Cancel"), QString::null,
            0, 1) == 0) {
        // remove the script from the database
        _selectedScript.remove();

        // remove the list item
        ui->scriptListWidget->takeItem(
                ui->scriptListWidget->currentRow());

        bool scriptsAvailable = ui->scriptListWidget->count() > 0;
        // disable the remove button if there is only no item left
        ui->scriptRemoveButton->setEnabled(scriptsAvailable);

        // disable the edit frame if there is no item
        ui->scriptEditFrame->setEnabled(scriptsAvailable);

        // reload the scripting engine
        ScriptingService::instance()->reloadEngine();
    }
}

/**
 * Allows to choose the script path
 */
void SettingsDialog::on_scriptPathButton_clicked() {
    QString path = QFileDialog::getOpenFileName(
            this, tr("Please select your QML file"),
            QDir::homePath(), tr("QML Files (*.qml)"));

    QFile file(path);

    if (file.exists() && (path != "")) {
        ui->scriptPathLineEdit->setText(path);
        _selectedScript.setScriptPath(path);
        _selectedScript.store();

        // validate the script
        validateCurrentScript();

        // reload the scripting engine
        ScriptingService::instance()->reloadEngine();
    }
}

/**
 * Loads the current script in the UI
 */
void SettingsDialog::on_scriptListWidget_currentItemChanged(
        QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);

    if (current == Q_NULLPTR) {
        return;
    }

    ui->scriptValidationLabel->clear();

    int scriptId = current->data(Qt::UserRole).toInt();
    _selectedScript = Script::fetch(scriptId);
    if (_selectedScript.isFetched()) {
        ui->scriptNameLineEdit->setText(_selectedScript.getName());
        ui->scriptPathLineEdit->setText(_selectedScript.getScriptPath());
        ui->scriptEditFrame->setEnabled(true);

        // validate the script
        validateCurrentScript();
    } else {
        ui->scriptEditFrame->setEnabled(false);
        ui->scriptNameLineEdit->clear();
        ui->scriptPathLineEdit->clear();
    }
}

/**
 * Validates the current script
 */
void SettingsDialog::validateCurrentScript() {
    ui->scriptValidationLabel->clear();

    if (_selectedScript.isFetched()) {
        QString path = _selectedScript.getScriptPath();

        // check the script validity if the path is not empty
        if (!path.isEmpty()) {
            QString errorMessage;
            bool result = ScriptingService::validateScript(
                    _selectedScript, errorMessage);
            QString validationText = result ?
                                     tr("Your script seems to be valid") :
                                     tr("There were script errors:\n%1")
                                             .arg(errorMessage);
            ui->scriptValidationLabel->setText(validationText);
            ui->scriptValidationLabel->setStyleSheet(
                    QString("color: %1;").arg(result ? "green" : "red"));
        }
    }
}

/**
 * Stores a script name after it was edited
 */
void SettingsDialog::on_scriptNameLineEdit_editingFinished() {
    QString text = ui->scriptNameLineEdit->text();
    _selectedScript.setName(text);
    _selectedScript.store();

    ui->scriptListWidget->currentItem()->setText(text);
}

/**
 * Stores the enabled states of the scripts
 */
void SettingsDialog::storeScriptListEnabledState() {
    for (int i = 0; i < ui->scriptListWidget->count(); i++) {
        QListWidgetItem *item = ui->scriptListWidget->item(i);
        bool enabled = item->checkState() == Qt::Checked;
        int scriptId = item->data(Qt::UserRole).toInt();

        Script script = Script::fetch(scriptId);
        if (script.isFetched()) {
            if (script.getEnabled() != enabled) {
                script.setEnabled(enabled);
                script.store();
            }
        }
    }

    // reload the scripting engine
    ScriptingService::instance()->reloadEngine();
}

/**
 * Validates the current script
 */
void SettingsDialog::on_scriptValidationButton_clicked() {
    // validate the script
    validateCurrentScript();
}

/**
 * Reloads the scripting engine
 */
void SettingsDialog::on_scriptReloadEngineButton_clicked() {
    // reload the scripting engine
    ScriptingService::instance()->reloadEngine();
}

/**
 * Turns off "ignoring all external modifications" if "notify all external
 * modifications" is checked
 *
 * @param checked
 */
void SettingsDialog::on_notifyAllExternalModificationsCheckBox_toggled(
        bool checked) {
    if (checked) {
        ui->ignoreAllExternalModificationsCheckBox->setChecked(false);
    }
}

/**
 * Turns off "notify all external modifications" if "ignoring all external
 * modifications" is checked
 *
 * @param checked
 */
void SettingsDialog::on_ignoreAllExternalModificationsCheckBox_toggled(
        bool checked) {
    if (checked) {
        ui->notifyAllExternalModificationsCheckBox->setChecked(false);
    }
}

/**
 * Adds a custom file extension
 */
void SettingsDialog::on_addCustomNoteFileExtensionButton_clicked() {
    bool ok;
    QString fileExtension;
    fileExtension = QInputDialog::getText(
            this, tr("File extension"),
            tr("Enter your custom file extension:"), QLineEdit::Normal,
            fileExtension, &ok);

    if (!ok) {
        return;
    }

    // make sure the file extension doesn't start with a point
    fileExtension = Utils::Misc::removeIfStartsWith(fileExtension, ".");

    QListWidgetItem *item = addCustomeNoteFileExtension(fileExtension);
    ui->defaultNoteFileExtensionListWidget->setCurrentItem(item);
}

/**
 * Adds a custom note file extension
 */
QListWidgetItem *SettingsDialog::addCustomeNoteFileExtension(
        const QString &fileExtension) {
    QListWidgetItem *item = new QListWidgetItem(fileExtension);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    item->setWhatsThis(fileExtension);
    ui->defaultNoteFileExtensionListWidget->addItem(item);

    return item;
}

/**
 * Removes a custom file extension
 */
void SettingsDialog::on_removeCustomNoteFileExtensionButton_clicked() {
    delete(ui->defaultNoteFileExtensionListWidget->currentItem());
}

/**
 * Updates a custom file extension
 */
void SettingsDialog::on_defaultNoteFileExtensionListWidget_itemChanged(
        QListWidgetItem *item) {
    // make sure the file extension doesn't start with a point
    QString fileExtension = Utils::Misc::removeIfStartsWith(item->text(), ".");

    if (fileExtension != item->text()) {
        item->setText(fileExtension);
    }

    item->setWhatsThis(fileExtension);
}

/**
 * Disables the remove custom file extension button for the first two rows
 */
void SettingsDialog::on_defaultNoteFileExtensionListWidget_currentRowChanged(
        int currentRow) {
    ui->removeCustomNoteFileExtensionButton->setEnabled(currentRow > 1);
}

/**
 * Toggles the dark mode colors check box with the dark mode checkbox
 */
void SettingsDialog::on_darkModeCheckBox_toggled() {
    bool checked = ui->darkModeCheckBox->isChecked();

    ui->darkModeColorsCheckBox->setEnabled(!checked);

    if (checked) {
        ui->darkModeColorsCheckBox->setChecked(true);
    }
}

void SettingsDialog::on_noteFolderShowSubfoldersCheckBox_toggled(bool checked) {
    _selectedNoteFolder.setShowSubfolders(checked);

    // reset the active note subfolder if showing subfolders was turned off
    if (!checked) {
        _selectedNoteFolder.resetActiveNoteSubFolder();
    }

    _selectedNoteFolder.store();
}

/**
 * Toggles the line breaks in the debug output
 */
void SettingsDialog::on_gitHubLineBreaksCheckBox_toggled(bool checked) {
    Q_UNUSED(checked);
    outputSettings();
}

/**
 * Searches in the description and in the shortcut for a entered text
 *
 * @param arg1
 */
void SettingsDialog::on_shortcutSearchLineEdit_textChanged(
        const QString &arg1) {
    // get all items
    QList<QTreeWidgetItem*> allItems = ui->shortcutTreeWidget->
            findItems("", Qt::MatchContains | Qt::MatchRecursive);

    // search text if at least one character was entered
    if (arg1.count() >= 1) {
        // search for items in the description
        QList<QTreeWidgetItem*> foundItems = ui->shortcutTreeWidget->
                findItems(arg1, Qt::MatchContains | Qt::MatchRecursive);

        // hide all not found items
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                bool foundKeySequence = false;

                QKeySequenceWidget *keyWidget =
                        static_cast<QKeySequenceWidget *>(
                                ui->shortcutTreeWidget->itemWidget(
                                        item, 1));

                // search in the shortcut text
                if (keyWidget != Q_NULLPTR) {
                    QKeySequence keySequence = keyWidget->keySequence();
                    foundKeySequence = keySequence.toString().contains(arg1);
                }

                item->setHidden(!foundItems.contains(item) &&
                                        !foundKeySequence);
            }

        // show items again that have visible children so that they are
        // really shown
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                if (MainWindow::isOneTreeWidgetItemChildVisible(item)) {
                    item->setHidden(false);
                    item->setExpanded(true);
                }
            }
    } else {
        // show all items otherwise
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                item->setHidden(false);
            }
    }
}

/**
 * Turn the markdown highlighter interval spin box on or off
 *
 * @param checked
 */
void SettingsDialog::on_markdownHighlightingCheckBox_toggled(bool checked) {
    ui->markdownHighlightingIntervalSpinBox->setEnabled(checked);
    ui->markdownHighlightingIntervalLabel->setEnabled(checked);

    if (checked && (ui->markdownHighlightingIntervalSpinBox->value() == 0)) {
        ui->markdownHighlightingIntervalSpinBox->setValue(
                _defaultMarkdownHighlightingInterval);
    }
}

void SettingsDialog::on_settingsTreeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);
    ui->settingsStackedWidget->setCurrentIndex(current->whatsThis(0).toInt());
}

void SettingsDialog::on_settingsStackedWidget_currentChanged(int index) {
    QTreeWidgetItem *item = findSettingsTreeWidgetItemByPage(index);
    if (item != Q_NULLPTR) {
        const QSignalBlocker blocker(ui->settingsTreeWidget);
        Q_UNUSED(blocker);

        ui->settingsTreeWidget->setCurrentItem(item);
        ui->headlineLabel->setText("<h3>" + item->text(0) + "</h3>");
    }

    if (index == DebugPage) {
        outputSettings();
    } else if (index == OwnCloudPage) {
        on_connectButton_clicked();
    }

    // turn off the tasks page if no ownCloud settings are available
//    QTreeWidgetItem *todoItem = findSettingsTreeWidgetItemByPage(TodoPage);
//    if (todoItem != Q_NULLPTR) {
//        if (OwnCloudService::hasOwnCloudSettings()) {
//            todoItem->setDisabled(false);
//            todoItem->setToolTip(0, "");
//        } else {
//            todoItem->setDisabled(true);
//            todoItem->setToolTip(0, tr("Please make sure the connection to "
//                                               "your ownCloud server works."));
//        }
//    }
}

/**
 * Returns the settings tree widget item corresponding to a page
 *
 * @param page
 * @return
 */
QTreeWidgetItem *SettingsDialog::findSettingsTreeWidgetItemByPage(int page) {
    // search for items
    QList<QTreeWidgetItem*> allItems = ui->settingsTreeWidget->
            findItems("", Qt::MatchContains | Qt::MatchRecursive);

    // hide all not found items
    Q_FOREACH(QTreeWidgetItem *item, allItems) {
            int id = item->whatsThis(0).toInt();

            if (id == page) {
                return item;
            }
        }

    return Q_NULLPTR;
}

/**
 * Does the initialization for the main splitter
 */
void SettingsDialog::initMainSplitter() {
    _mainSplitter = new QSplitter();
    _mainSplitter->setOrientation(Qt::Horizontal);
    ui->leftSideFrame->setStyleSheet("#leftSideFrame {margin-right: 5px;}");

    _mainSplitter->addWidget(ui->leftSideFrame);
    _mainSplitter->addWidget(ui->settingsFrame);

    ui->mainFrame->layout()->addWidget(_mainSplitter);

    // restore tag frame splitter state
    QSettings settings;
    QByteArray state = settings.value(
            "SettingsDialog/mainSplitterState").toByteArray();
    _mainSplitter->restoreState(state);
}

void SettingsDialog::closeEvent(QCloseEvent *event) {
    // store the splitter settings
    storeSplitterSettings();
}

/**
 * Stores the splitter settings
 */
void SettingsDialog::storeSplitterSettings() {
    QSettings settings;
    settings.setValue("SettingsDialog/mainSplitterState",
                      _mainSplitter->saveState());
}

void SettingsDialog::on_calDavCalendarRadioButton_toggled(bool checked) {
    if (checked) {
        on_reloadCalendarListButton_clicked();
    }

    ui->calDavCalendarGroupBox->setVisible(checked);
}

void SettingsDialog::on_calendarPlusRadioButton_toggled(bool checked) {
    if (checked) {
        on_reloadCalendarListButton_clicked();
    }
}
