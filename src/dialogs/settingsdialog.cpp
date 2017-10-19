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
#include "filedialog.h"
#include "scriptrepositorydialog.h"
#include <QtNetwork/qnetworkproxy.h>
#include <services/cryptoservice.h>
#include <utils/gui.h>
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
#include <helpers/toolbarcontainer.h>
#include <QProcess>
#include <widgets/scriptsettingwidget.h>
#include <QStyleFactory>

SettingsDialog::SettingsDialog(int page, QWidget *parent) :
        MasterDialog(parent), ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    // we must not do that because XFCE really resizes the window to 1x1
//    resize(1, 1);

    ui->connectionTestLabel->hide();
    ui->darkModeInfoLabel->hide();
    ui->connectButton->setDefault(true);
    ui->noteSaveIntervalTime->setToolTip(
            ui->noteSaveIntervalTimeLabel->toolTip());
    ui->removeCustomNoteFileExtensionButton->setDisabled(true);
    ui->calDavCalendarGroupBox->hide();
    _newScriptName = tr("New script");

#ifdef Q_OS_WIN32
    QString downloadText = tr("You can download your git client here: <a "
            "href=\"%url\">Git for Windows</a>");
    downloadText.replace("%url", "https://git-scm.com/download/win");
    ui->gitDownloadLabel->setText(downloadText);
#else
    ui->gitDownloadLabel->hide();
#endif

    _noteNotificationButtonGroup = new QButtonGroup(this);
    _noteNotificationButtonGroup->addButton(
            ui->notifyAllExternalModificationsCheckBox);
    _noteNotificationButtonGroup->addButton(
            ui->ignoreAllExternalModificationsCheckBox);
    _noteNotificationButtonGroup->addButton(
            ui->acceptAllExternalModificationsCheckBox);

    // create a hidden checkbox so we can un-check above checkboxes
    _noteNotificationNoneCheckBox = new QCheckBox(this);
    _noteNotificationNoneCheckBox->setHidden(true);
    _noteNotificationButtonGroup->addButton(_noteNotificationNoneCheckBox);
    connect(_noteNotificationButtonGroup,
            SIGNAL(buttonPressed(QAbstractButton *)),
            this, SLOT(noteNotificationButtonGroupPressed(QAbstractButton *)));

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

    if (connectionTestCanBeStarted()) {
        // start a connection test
        startConnectionTest();
    }

    // init the debug info search frame
    ui->debugInfoTextEdit->initSearchFrame(ui->debugInfoTextEditSearchFrame);

    // set the current page
    // must be done in the end so that the settings are loaded first when
    // doing a connection test
    setCurrentPage(page);

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

    // initialize the portable mode page
    initPortableModePage();

    // init the toolbar editor
    ui->toolbarEditor->setTargetWindow(MainWindow::instance());
    ui->toolbarEditor->setCustomToolbarRemovalOnly(true);

    QStringList disabledToolbarNames(
            QStringList() << "windowToolbar" << "customActionsToolbar");
    ui->toolbarEditor->setDisabledToolbarNames(disabledToolbarNames);

    QStringList disabledMenuNames(QStringList() << "noteFoldersMenu");
    ui->toolbarEditor->setDisabledMenuNames(disabledMenuNames);

//    QStringList disabledMenuActionNames(QStringList() << "");
//    ui->toolbarEditor->setDisabledMenuActionNames(disabledMenuActionNames);

    ui->toolbarEditor->updateBars();

    // show the log file path
    ui->logFileLabel->setText(QDir::toNativeSeparators(
            Utils::Misc::logFilePath()));

    // replace the "ownCloud" text by "ownCloud / NextCloud"
    replaceOwnCloudText();

    // declare that we need to restart the application if certain settings
    // are changed
    connect(ui->languageListWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(needRestart()));
    connect(ui->internalIconThemeCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->darkModeTrayIconCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->darkModeColorsCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->darkModeCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->allowOnlyOneAppInstanceCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));

    // connect the panel sort radio buttons
    connect(ui->notesPanelSortAlphabeticalRadioButton, SIGNAL(toggled(bool)),
            ui->notesPanelOrderGroupBox, SLOT(setEnabled(bool)));
    connect(ui->noteSubfoldersPanelShowRootFolderNameCheckBox, SIGNAL(toggled(bool)),
            ui->noteSubfoldersPanelShowFullPathCheckBox, SLOT(setEnabled(bool)));
    connect(ui->noteSubfoldersPanelSortAlphabeticalRadioButton, SIGNAL(toggled(bool)),
            ui->noteSubfoldersPanelOrderGroupBox, SLOT(setEnabled(bool)));
    connect(ui->tagsPanelSortAlphabeticalRadioButton, SIGNAL(toggled(bool)),
            ui->tagsPanelOrderGroupBox, SLOT(setEnabled(bool)));
}

/**
 * Returns true if we can start a connection test
 *
 * @return
 */
bool SettingsDialog::connectionTestCanBeStarted() const {
    return ui->ownCloudSupportCheckBox->isChecked() &&
           !ui->serverUrlEdit->text().isEmpty();
}

/**
 * Replaces the "ownCloud" text by "ownCloud / NextCloud"
 */
void SettingsDialog::replaceOwnCloudText() const {
    //
    // ownCloud settings
    //
    ui->ownCloudSupportGroupBox->setTitle(Utils::Misc::replaceOwnCloudText(
            ui->ownCloudSupportGroupBox->title()));
    ui->ownCloudSupportCheckBox->setText(Utils::Misc::replaceOwnCloudText(
            ui->ownCloudSupportCheckBox->text()));
    ui->userNameEdit->setPlaceholderText(Utils::Misc::replaceOwnCloudText(
            ui->userNameEdit->placeholderText()));
    ui->passwordEdit->setPlaceholderText(Utils::Misc::replaceOwnCloudText(
            ui->passwordEdit->placeholderText()));
    ui->ownCloudGroupBox->setTitle(Utils::Misc::replaceOwnCloudText(
            ui->ownCloudGroupBox->title()));
    ui->ownCloudServerUrlLabel->setText(Utils::Misc::replaceOwnCloudText(
            ui->ownCloudServerUrlLabel->text(), true));
    ui->check2Label->setText(Utils::Misc::replaceOwnCloudText(
            ui->check2Label->text()));
    ui->ownCloudServerAppPageButton->setText(Utils::Misc::replaceOwnCloudText(
            ui->ownCloudServerAppPageButton->text(), true));
    ui->connectButton->setText(Utils::Misc::replaceOwnCloudText(
            ui->connectButton->text(), true));
    ui->installInfoTextLabel1->setText(Utils::Misc::replaceOwnCloudText(
            ui->installInfoTextLabel1->text()));
    ui->installInfoTextLabel2->setText(Utils::Misc::replaceOwnCloudText(
            ui->installInfoTextLabel2->text()));

    QTreeWidgetItem *item = ui->settingsTreeWidget->topLevelItem(
            OwnCloudPage);
    item->setText(0, Utils::Misc::replaceOwnCloudText(item->text(0)));

    // note folder settings
    ui->noteFolderRemotePathLabel->setText(Utils::Misc::replaceOwnCloudText(
            ui->noteFolderRemotePathLabel->text()));
    ui->noteFolderRemotePathListLabel->setText(Utils::Misc::replaceOwnCloudText(
            ui->noteFolderRemotePathListLabel->text()));
    ui->useOwnCloudPathButton->setText(Utils::Misc::replaceOwnCloudText(
            ui->useOwnCloudPathButton->text()));
    ui->noteFolderRemotePathButton->setToolTip(Utils::Misc::replaceOwnCloudText(
            ui->noteFolderRemotePathButton->toolTip()));
    ui->noteFolderRemotePathLineEdit->setToolTip(
            Utils::Misc::replaceOwnCloudText(
                    ui->noteFolderRemotePathLineEdit->toolTip()));

    // task settings
    ui->defaultOwnCloudCalendarRadioButton->setText(
            Utils::Misc::replaceOwnCloudText(
                    ui->defaultOwnCloudCalendarRadioButton->text()));
}

/**
 * Check the _noteNotificationNoneCheckBox when the checkboxes should all be
 * unchecked
 *
 * @param button
 */
void SettingsDialog::noteNotificationButtonGroupPressed(
        QAbstractButton *button) {
    if (button->isChecked()) {
        QTimer::singleShot(100, this,
                           SLOT(noteNotificationNoneCheckBoxCheck()));
    }
}

/**
 * Check the _noteNotificationNoneCheckBox
 */
void SettingsDialog::noteNotificationNoneCheckBoxCheck() {
    _noteNotificationNoneCheckBox->setChecked(true);
}

/**
 * Sets the current page
 *
 * @param page
 */
void SettingsDialog::setCurrentPage(int page) {
    ui->settingsStackedWidget->setCurrentIndex(page);

    // update other stuff for the settings tree widget
    if (ui->settingsStackedWidget->currentIndex() == page) {
        on_settingsStackedWidget_currentChanged(page);
    }
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

/**
 * Initializes the portable mode page
 */
void SettingsDialog::initPortableModePage() {
    bool isInPortableMode = Utils::Misc::isInPortableMode();
    QString status = isInPortableMode ? tr("enabled") : tr("disabled");

    QString text =
            "<p>" + tr("Portable mode is currently:") + " <strong>" + status +
            "</strong></p>";

    text += tr("In portable mode") + ":<ul><li>" +
            tr("the internal sqlite database and the settings will be stored "
                       "inside a <code>Data</code> folder at the binary's "
                       "location") + "</li><li>" +
            tr("the settings will be stored in an ini file") + "</li><li>" +
            tr("the note folders, script paths and path to an external editor "
                       "will be automatically stored relative to the "
                       "<code>Data</code> folder so that the correct note "
                       "folders, scripts and external editor will be loaded "
                       "regardless where your QOwnNotes installation is "
                       "currently located") + "</li></ul>";

    if (!isInPortableMode) {
        text += "<p>" + tr("It will be activated if you run QOwnNotes with "
                                   "the parameter <code>--portable</code>.") +
                "</p>";

#ifdef Q_OS_WIN32
        text += "<p>" + tr("You will find a <code>QOwnNotesPortable.bat</code> "
                           "in your release path to start QOwnNotes in "
                           "portable mode.") + "</p>";
#endif
    }

    // inject some generic CSS styles
    ui->portableModeInfoTextBrowser->document()->setDefaultStyleSheet(
            Utils::Misc::genericCSS());

    ui->portableModeInfoTextBrowser->setHtml(text);
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
    OwnCloudService *ownCloud = OwnCloudService::instance();
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

    settings.setValue("ownCloud/supportEnabled",
                      ui->ownCloudSupportCheckBox->isChecked());
    settings.setValue("ownCloud/serverUrl", url);
    settings.setValue("ownCloud/userName", ui->userNameEdit->text());
    settings.setValue("ownCloud/password",
                      CryptoService::instance()->encryptToString(
                              ui->passwordEdit->text()));
    settings.setValue("insertTimeFormat", ui->timeFormatLineEdit->text());
    settings.setValue("disableAutomaticUpdateDialog",
                      ui->disableAutomaticUpdateDialogCheckBox->isChecked());
    settings.setValue("notifyAllExternalModifications",
                      ui->notifyAllExternalModificationsCheckBox->isChecked());
    settings.setValue("ignoreAllExternalModifications",
                      ui->ignoreAllExternalModificationsCheckBox->isChecked());
    settings.setValue("acceptAllExternalModifications",
                      ui->acceptAllExternalModificationsCheckBox->isChecked());
    settings.setValue("ignoreAllExternalNoteFolderChanges",
                      ui->ignoreAllExternalNoteFolderChangesCheckBox
                              ->isChecked());
    settings.setValue("allowDifferentNoteFileName",
                      ui->allowDifferentNoteFileNameCheckBox->isChecked());
    settings.setValue("useUNIXNewline",
                      ui->useUNIXNewlineCheckBox->isChecked());
    settings.setValue("restoreCursorPosition",
                      ui->restoreCursorPositionCheckBox->isChecked());
    settings.setValue("noteSaveIntervalTime",
                      ui->noteSaveIntervalTime->value());
    settings.setValue("defaultNoteFileExtension",
                      getSelectedListWidgetValue(
                              ui->defaultNoteFileExtensionListWidget));

    // make the path relative to the portable data path if we are in
    // portable mode
    settings.setValue("externalEditorPath",
                      Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                              ui->externalEditorPathLineEdit->text()));

    settings.setValue("itemHeight", ui->itemHeightSpinBox->value());
    settings.setValue("MainWindow/mainToolBar.iconSize",
                      ui->toolbarIconSizeSpinBox->value());
    settings.setValue("allowOnlyOneAppInstance",
                      ui->allowOnlyOneAppInstanceCheckBox->isChecked());
    settings.setValue("interfaceLanguage",
                      getSelectedListWidgetValue(ui->languageListWidget));
    settings.setValue("markdownHighlightingEnabled",
                      ui->markdownHighlightingCheckBox->isChecked());
    settings.setValue("MainWindow/noteTextView.rtl",
                      ui->noteTextViewRTLCheckBox->isChecked());
    settings.setValue("Debug/fakeOldVersionNumber",
                      ui->oldVersionNumberCheckBox->isChecked());
    settings.setValue("Debug/fileLogging",
                      ui->fileLoggingCheckBox->isChecked());
    settings.setValue("Editor/autoBracketClosing",
                      ui->autoBracketClosingCheckBox->isChecked());
    settings.setValue("Editor/autoBracketRemoval",
                      ui->autoBracketRemovalCheckBox->isChecked());
    settings.setValue("Editor/highlightCurrentLine",
                      ui->highlightCurrentLineCheckBox->isChecked());

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

    settings.setValue("internalIconTheme",
                      ui->internalIconThemeCheckBox->isChecked());

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
    } else if (ui->legacyOwnCloudCalendarRadioButton->isChecked()) {
        todoCalendarBackend = OwnCloudService::LegacyOwnCloudCalendar;
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
    customNoteFileExtensionList.removeDuplicates();
    settings.setValue("customNoteFileExtensionList",
                      customNoteFileExtensionList);

    // store the font settings
    storeFontSettings();

    // store the proxy settings
    storeProxySettings();

    // store the shortcut settings
    storeShortcutSettings();

    // store the splitter settings
    storeSplitterSettings();

    // apply and store the toolbar configuration
    on_applyToolbarButton_clicked();

    // store the enabled state of the scripts
    storeScriptListEnabledState();

    // store image scaling settings
    settings.setValue("imageScaleDown",
                      ui->imageScaleDownCheckBox->isChecked());
    settings.setValue("imageScaleDownMaximumHeight",
                      ui->maximumImageHeightSpinBox->value());
    settings.setValue("imageScaleDownMaximumWidth",
                      ui->maximumImageWidthSpinBox->value());

    // store git settings
    settings.setValue("gitExecutablePath",
                      Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                              ui->gitPathLineEdit->text()));
    settings.setValue("gitCommitInterval", ui->gitCommitIntervalTime->value());
    settings.setValue("gitLogCommand", ui->gitLogCommandLineEdit->text());

    // store Panels settings
    storePanelSettings();

    // store the interface style settings
    if (ui->interfaceStyleComboBox->currentIndex() > 0) {
        settings.setValue("interfaceStyle",
                          ui->interfaceStyleComboBox->currentText());
    } else {
        settings.remove("interfaceStyle");
    }

    // store the cursor width
    settings.setValue("cursorWidth", ui->cursorWidthSpinBox->value());

    settings.setValue("SearchEngineId",
                    ui->searchEngineSelectionComboBox->currentData().toInt());
}

/**
 * @brief Stores the Panel settings
 */
void SettingsDialog::storePanelSettings() {
    QSettings settings;
    // Notes Panel Options
    ui->notesPanelSortAlphabeticalRadioButton->isChecked() ?
                settings.setValue("notesPanelSort", SORT_ALPHABETICAL) :
                settings.setValue("notesPanelSort", SORT_BY_LAST_CHANGE);
    ui->notesPanelOrderDescendingRadioButton->isChecked() ?
                settings.setValue("notesPanelOrder", ORDER_DESCENDING) :
                settings.setValue("notesPanelOrder", ORDER_ASCENDING);

    // Note Subfolders Panel Options
    settings.setValue("noteSubfoldersPanelHideSearch",
                      ui->noteSubfoldersPanelHideSearchCheckBox->isChecked());

    settings.setValue("noteSubfoldersPanelDisplayAsFullTree",
                      ui->noteSubfoldersPanelDisplayAsFullTreeCheckBox->isChecked());

    settings.setValue("noteSubfoldersPanelShowRootFolderName",
                      ui->noteSubfoldersPanelShowRootFolderNameCheckBox->isChecked());

    settings.setValue("noteSubfoldersPanelShowFullPath",
                      ui->noteSubfoldersPanelShowFullPathCheckBox->isChecked());

    ui->noteSubfoldersPanelSortAlphabeticalRadioButton->isChecked() ?
                settings.setValue("noteSubfoldersPanelSort", SORT_ALPHABETICAL) :
                settings.setValue("noteSubfoldersPanelSort", SORT_BY_LAST_CHANGE);

    ui->noteSubfoldersPanelOrderDescendingRadioButton->isChecked() ?
                settings.setValue("noteSubfoldersPanelOrder", ORDER_DESCENDING) :
                settings.setValue("noteSubfoldersPanelOrder", ORDER_ASCENDING);

    // Tags Panel Options
    settings.setValue("tagsPanelHideSearch", ui->tagsPanelHideSearchCheckBox->isChecked());

    ui->tagsPanelSortAlphabeticalRadioButton->isChecked() ?
                settings.setValue("tagsPanelSort", SORT_ALPHABETICAL) :
                settings.setValue("tagsPanelSort", SORT_BY_LAST_CHANGE);

    ui->tagsPanelOrderDescendingRadioButton->isChecked() ?
                settings.setValue("tagsPanelOrder", ORDER_DESCENDING) :
                settings.setValue("tagsPanelOrder", ORDER_ASCENDING);
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
    ui->ownCloudSupportCheckBox->setChecked(
            OwnCloudService::isOwnCloudSupportEnabled());
    on_ownCloudSupportCheckBox_toggled();
    ui->serverUrlEdit->setText(settings.value("ownCloud/serverUrl").toString());
    ui->userNameEdit->setText(settings.value("ownCloud/userName").toString());
    ui->passwordEdit->setText(CryptoService::instance()->decryptToString(
            settings.value("ownCloud/password").toString()));
    ui->timeFormatLineEdit->setText(
            settings.value("insertTimeFormat").toString());

    // prepend the portable data path if we are in portable mode
    ui->externalEditorPathLineEdit->setText(
            Utils::Misc::prependPortableDataPathIfNeeded(
                    settings.value("externalEditorPath").toString(), true));

    ui->disableAutomaticUpdateDialogCheckBox->setChecked(
            settings.value("disableAutomaticUpdateDialog").toBool());
    ui->notifyAllExternalModificationsCheckBox->setChecked(
            settings.value("notifyAllExternalModifications").toBool());
    ui->ignoreAllExternalModificationsCheckBox->setChecked(
            settings.value("ignoreAllExternalModifications").toBool());
    ui->acceptAllExternalModificationsCheckBox->setChecked(
            settings.value("acceptAllExternalModifications").toBool());
    ui->ignoreAllExternalNoteFolderChangesCheckBox->setChecked(
            settings.value("ignoreAllExternalNoteFolderChanges").toBool());
    ui->allowDifferentNoteFileNameCheckBox->setChecked(
            settings.value("allowDifferentNoteFileName").toBool());
    ui->useUNIXNewlineCheckBox->setChecked(
            settings.value("useUNIXNewline").toBool());

#ifdef Q_OS_MAC
    bool restoreCursorPositionDefault = false;
#else
    bool restoreCursorPositionDefault = true;
#endif

    ui->restoreCursorPositionCheckBox->setChecked(
            settings.value("restoreCursorPosition",
                           restoreCursorPositionDefault).toBool());
    ui->noteSaveIntervalTime->setValue(
            settings.value("noteSaveIntervalTime", 10).toInt());
    ui->noteTextViewRTLCheckBox->setChecked(
            settings.value("MainWindow/noteTextView.rtl").toBool());
    ui->oldVersionNumberCheckBox->setChecked(
            settings.value("Debug/fakeOldVersionNumber").toBool());
    ui->fileLoggingCheckBox->setChecked(
            settings.value("Debug/fileLogging").toBool());
    on_fileLoggingCheckBox_toggled(ui->fileLoggingCheckBox->isChecked());
    ui->autoBracketClosingCheckBox->setChecked(
            settings.value("Editor/autoBracketClosing", true).toBool());
    ui->autoBracketRemovalCheckBox->setChecked(
            settings.value("Editor/autoBracketRemoval", true).toBool());
    ui->highlightCurrentLineCheckBox->setChecked(
            settings.value("Editor/highlightCurrentLine", true).toBool());
    ui->markdownHighlightingCheckBox->setChecked(
            settings.value("markdownHighlightingEnabled", true).toBool());
    ui->allowOnlyOneAppInstanceCheckBox->setChecked(settings.value(
            "allowOnlyOneAppInstance").toBool());
    ui->toolbarIconSizeSpinBox->setValue(
                 settings.value("MainWindow/mainToolBar.iconSize").toInt());

    QTreeWidget treeWidget(this);
    QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();
    treeWidget.addTopLevelItem(treeWidgetItem);
    int height = treeWidget.visualItemRect(treeWidgetItem).height();

    ui->itemHeightSpinBox->setValue(
                 settings.value("itemHeight", height).toInt());

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

    ui->internalIconThemeCheckBox->setChecked(settings.value(
            "internalIconTheme").toBool());

    // toggle the dark mode colors check box with the dark mode checkbox
    on_darkModeCheckBox_toggled();

    noteTextEditFont.fromString(
            settings.value("MainWindow/noteTextEdit.font").toString());
    setFontLabel(ui->noteTextEditFontLabel, noteTextEditFont);

    noteTextEditCodeFont.fromString(
            settings.value("MainWindow/noteTextEdit.code.font").toString());
    setFontLabel(ui->noteTextEditCodeFontLabel, noteTextEditCodeFont);

    // load note text view font
    QString fontString = settings.value("MainWindow/noteTextView.font")
            .toString();

    // store the current font if there isn't any set yet
    if (fontString == "") {
        QTextEdit *textEdit = new QTextEdit();
        fontString = textEdit->font().toString();
        settings.setValue("MainWindow/noteTextView.font", fontString);
    }

    noteTextViewFont.fromString(fontString);
    setFontLabel(ui->noteTextViewFontLabel, noteTextViewFont);


    // load note text view code font
    fontString = settings.value("MainWindow/noteTextView.code.font")
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

    const QSignalBlocker blocker2(ui->defaultOwnCloudCalendarRadioButton);
    Q_UNUSED(blocker2);

    const QSignalBlocker blocker7(ui->legacyOwnCloudCalendarRadioButton);
    Q_UNUSED(blocker7);

    const QSignalBlocker blocker4(ui->calendarPlusRadioButton);
    Q_UNUSED(blocker4);

    const QSignalBlocker blocker5(ui->calDavCalendarRadioButton);
    Q_UNUSED(blocker5);

    switch (settings.value("ownCloud/todoCalendarBackend",
                           OwnCloudService::DefaultOwnCloudCalendar).toInt()) {
        case OwnCloudService::CalendarPlus:
            ui->calendarPlusRadioButton->setChecked(true);
            break;
        case OwnCloudService::CalDAVCalendar:
            ui->calDavCalendarRadioButton->setChecked(true);
            ui->calDavCalendarGroupBox->setVisible(true);
            break;
        case OwnCloudService::DefaultOwnCloudCalendar:
            ui->defaultOwnCloudCalendarRadioButton->setChecked(true);
            break;
        default:
            ui->legacyOwnCloudCalendarRadioButton->setChecked(true);
            break;
    }

    const QSignalBlocker blocker6(this->ui->ignoreNonTodoCalendarsCheckBox);
    Q_UNUSED(blocker6);

    ui->ignoreNonTodoCalendarsCheckBox->setChecked(settings.value(
            "ownCloud/ignoreNonTodoCalendars", true).toBool());

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
    QStringList todoCalendarDisplayNameList = settings.value(
            "ownCloud/todoCalendarDisplayNameList").toStringList();
    int todoCalendarUrlListCount = todoCalendarUrlList.count();
    int todoCalendarDisplayNameListCount = todoCalendarDisplayNameList.count();

    QList<CalDAVCalendarData> calendarDataList;
    for (int i = 0; i < todoCalendarUrlListCount; i++) {
        CalDAVCalendarData data;
        data.url = todoCalendarUrlList.at(i);

        if (todoCalendarUrlListCount == todoCalendarDisplayNameListCount) {
            data.displayName = todoCalendarDisplayNameList.at(i);
        }

        calendarDataList << data;
    }
    // load the tasks calendar list and set the checked state
    refreshTodoCalendarList(calendarDataList, true);

    // loads the custom note file extensions
    QListIterator<QString> itr(Note::customNoteFileExtensionList());
    while (itr.hasNext()) {
        QString fileExtension = itr.next();
        addCustomNoteFileExtension(fileExtension);
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

    // load image scaling settings
    bool scaleImageDown = settings.value("imageScaleDown", false).toBool();
    ui->maximumImageHeightSpinBox->setValue(settings.value(
            "imageScaleDownMaximumHeight", 1024).toInt());
    ui->maximumImageWidthSpinBox->setValue(settings.value(
            "imageScaleDownMaximumWidth", 1024).toInt());
    ui->imageScaleDownCheckBox->setChecked(scaleImageDown);
    ui->imageScalingFrame->setVisible(scaleImageDown);

    // load git settings
    ui->gitPathLineEdit->setText(
            Utils::Misc::prependPortableDataPathIfNeeded(
                    settings.value("gitExecutablePath").toString(), true));
    ui->gitCommitIntervalTime->setValue(
            settings.value("gitCommitInterval", 30).toInt());
    ui->gitLogCommandLineEdit->setText(
            settings.value("gitLogCommand").toString());

    // read panel settings
    readPanelSettings();

    // load the settings for the interface style combo box
    loadInterfaceStyleComboBox();

    // set the cursor width spinbox value
    ui->cursorWidthSpinBox->setValue(
            settings.value("cursorWidth", 1).toInt());

    // Not sure this is the best way to do this
    // but it works
    typedef Utils::Misc::SearchEngine SearchEngine;

    // Iterates over the search engines and adds them
    // to the combobox
    QHash<int, SearchEngine> searchEngines =
            Utils::Misc::getSearchEnginesHashmap();

    ui->searchEngineSelectionComboBox->clear();

    QHash<int, SearchEngine>::const_iterator searchEngineIterator;

    for (searchEngineIterator = searchEngines.begin();
         searchEngineIterator != searchEngines.end();
         searchEngineIterator++) {
        SearchEngine current = searchEngineIterator.value();
        ui->searchEngineSelectionComboBox->addItem(current.name + " : " + QVariant(current.id).toString() , QVariant(current.id).toString());
    }

    // Sets the current selected item to the search engine
    // selected previously
    // while also handling the case in which the saved key has
    // been removed from the hash table
    int savedEngineId = settings.value("SearchEngineId").toInt();
    int savedEngineIndex = ui->searchEngineSelectionComboBox->findData(
                QVariant(savedEngineId).toString());
    savedEngineIndex = (savedEngineIndex == -1) ? 0 : savedEngineIndex;
    ui->searchEngineSelectionComboBox->setCurrentIndex(savedEngineIndex);
}

/**
 * Loads the settings for the interface style combo box
 */
void SettingsDialog::loadInterfaceStyleComboBox() const {
    const QSignalBlocker blocker(ui->interfaceStyleComboBox);
    Q_UNUSED(blocker);

    ui->interfaceStyleComboBox->clear();
    ui->interfaceStyleComboBox->addItem(tr("Automatic (needs restart)"));

    Q_FOREACH(QString style, QStyleFactory::keys()) {
            ui->interfaceStyleComboBox->addItem(style);
        }

    QSettings settings;
    QString interfaceStyle = settings.value("interfaceStyle").toString();

    if (!interfaceStyle.isEmpty()) {
        ui->interfaceStyleComboBox->setCurrentText(interfaceStyle);
        QApplication::setStyle(interfaceStyle);
    } else {
        ui->interfaceStyleComboBox->setCurrentIndex(0);
    }
}

/**
 * @brief Read the Panel Settings
 */
void SettingsDialog::readPanelSettings() {
    QSettings settings;
    // Notes Panel Options
    if (settings.value("notesPanelSort").toInt() == SORT_ALPHABETICAL) {
        ui->notesPanelSortAlphabeticalRadioButton->setChecked(true);
        ui->notesPanelOrderGroupBox->setEnabled(true);
    } else {
        ui->notesPanelSortByLastChangeRadioButton->setChecked(true);
        ui->notesPanelOrderGroupBox->setEnabled(false);
    }
    settings.value("notesPanelOrder").toInt() == ORDER_DESCENDING ?
                ui->notesPanelOrderDescendingRadioButton->setChecked(true) :
                ui->notesPanelOrderAscendingRadioButton->setChecked(true);

    // Note Subfoldes Panel Options
    ui->noteSubfoldersPanelHideSearchCheckBox->setChecked(
                settings.value("noteSubfoldersPanelHideSearch").toBool());

    ui->noteSubfoldersPanelDisplayAsFullTreeCheckBox->setChecked(
                settings.value("noteSubfoldersPanelDisplayAsFullTree", true)
                        .toBool());

    if (settings.value(
            "noteSubfoldersPanelShowRootFolderName", true).toBool()) {
        ui->noteSubfoldersPanelShowRootFolderNameCheckBox->setChecked(true);
        ui->noteSubfoldersPanelShowFullPathCheckBox->setEnabled(true);
    } else {
        ui->noteSubfoldersPanelShowRootFolderNameCheckBox->setChecked(false);
        ui->noteSubfoldersPanelShowFullPathCheckBox->setEnabled(false);
    }

    ui->noteSubfoldersPanelShowFullPathCheckBox->setChecked(
                settings.value("noteSubfoldersPanelShowFullPath").toBool());

    if (settings.value(
            "noteSubfoldersPanelSort").toInt() == SORT_ALPHABETICAL) {
        ui->noteSubfoldersPanelSortAlphabeticalRadioButton->setChecked(true);
        ui->noteSubfoldersPanelOrderGroupBox->setEnabled(true);
    } else {
        ui->noteSubfoldersPanelSortByLastChangeRadioButton->setChecked(true);
        ui->noteSubfoldersPanelOrderGroupBox->setEnabled(false);
    }

    settings.value("noteSubfoldersPanelOrder").toInt() == ORDER_DESCENDING ?
        ui->noteSubfoldersPanelOrderDescendingRadioButton->setChecked(true) :
        ui->noteSubfoldersPanelOrderAscendingRadioButton->setChecked(true);

    // Tags Panel Options
    ui->tagsPanelHideSearchCheckBox->setChecked(settings.value(
            "tagsPanelHideSearch").toBool());

    if (settings.value("tagsPanelSort").toInt() == SORT_ALPHABETICAL) {
        ui->tagsPanelSortAlphabeticalRadioButton->setChecked(true);
        ui->tagsPanelOrderGroupBox->setEnabled(true);
    } else {
        ui->tagsPanelSortByLastChangeRadioButton->setChecked(true);
        ui->tagsPanelOrderGroupBox->setEnabled(false);
    }

    settings.value("tagsPanelOrder").toInt() == ORDER_DESCENDING ?
                ui->tagsPanelOrderDescendingRadioButton->setChecked(true) :
                ui->tagsPanelOrderAscendingRadioButton->setChecked(true);
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
    ui->shortcutSearchLineEdit->clear();
    ui->shortcutTreeWidget->clear();
    ui->shortcutTreeWidget->setColumnCount(2);

    // loop through all menus
    foreach(QMenu* menu, menus) {
            // shortcuts on toolbars don't work yet
            if (menu->objectName() == "menuToolbars") {
                continue;
            }

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
void SettingsDialog::selectListWidgetValue(QListWidget* listWidget,
                                           QString value) {
    // get all items from the list widget
    QList<QListWidgetItem *> items = listWidget->findItems(
                    QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    // select the right item in the selector
    Q_FOREACH(QListWidgetItem *item, items) {
            if (item->whatsThis() == value) {
                const QSignalBlocker blocker(listWidget);
                Q_UNUSED(blocker);

                listWidget->setItemSelected(item, true);
                break;
            }
        }
}

/**
 * Checks if a value, that is hidden in the whatsThis parameter exists in a
 * list widget
 */
bool SettingsDialog::listWidgetValueExists(QListWidget* listWidget,
                                           QString value) {
    // get all items from the list widget
    QList<QListWidgetItem *> items = listWidget->findItems(
                    QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
    // checks if the value exists
    Q_FOREACH(QListWidgetItem *item, items) {
            if (item->whatsThis() == value) {
                return true;
            }
        }

    return false;
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
    output += prepareDebugInformationLine("Portable mode",
                                          Utils::Misc::isInPortableMode() ?
                                          "yes" : "no");
    output += prepareDebugInformationLine("Settings path / key",
                                          settings.fileName());
    output += prepareDebugInformationLine("Application database path",
            QDir::toNativeSeparators(DatabaseService::getDiskDatabasePath()));
    output += prepareDebugInformationLine("Application arguments",
            QApplication::arguments().join("`, `"));

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
    output += prepareDebugInformationLine("notesPathExists",
                                          notesPathExistsText);
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
                output += "\n### Note folder `" + noteFolder.getName() +
                        "`\n\n";
                output += prepareDebugInformationLine(
                        "id", QString::number(noteFolder.getId()));
                output += prepareDebugInformationLine(
                        "isCurrent",
                        noteFolder.isCurrent() ? "yes" : "no");
                output += prepareDebugInformationLine(
                        "activeTagId",
                        QString::number(noteFolder.getActiveTagId()));
                output += prepareDebugInformationLine(
                        "localPath", QDir::toNativeSeparators(
                                noteFolder.getLocalPath()));
                output += prepareDebugInformationLine(
                        "remotePath", noteFolder.getRemotePath());
                output += prepareDebugInformationLine(
                        "isShowSubfolders",
                        noteFolder.isShowSubfolders() ? "yes" : "no");
                output += prepareDebugInformationLine(
                        "isUseGit",
                        noteFolder.isUseGit() ? "yes" : "no");
                output += prepareDebugInformationLine(
                        "activeNoteSubFolder name",
                        noteFolder.getActiveNoteSubFolder().getName());
                output += prepareDebugInformationLine(
                        "database file",
                        QDir::toNativeSeparators(noteFolder.getLocalPath() +
                                                 "/notes.sqlite"));
            }
    }

    // add script information
    output += "\n## Enabled scripts\n";

    QList<Script> scripts = Script::fetchAll(true);
    if (noteFolders.count() > 0) {
        Q_FOREACH(Script script, scripts) {
                output += "\n### Script `" + script.getName() +
                        "`\n\n";
                output += prepareDebugInformationLine(
                        "id", QString::number(script.getId()));
                output += prepareDebugInformationLine(
                        "path", QDir::toNativeSeparators(
                                script.getScriptPath()));
                output += prepareDebugInformationLine(
                        "variablesJson", script.getSettingsVariablesJson());
                if (script.isScriptFromRepository()) {
                    ScriptInfoJson infoJson = script.getScriptInfoJson();

                    output += prepareDebugInformationLine(
                            "identifier", script.getIdentifier());
                    output += prepareDebugInformationLine(
                            "version", infoJson.version);
                    output += prepareDebugInformationLine(
                            "minAppVersion", infoJson.minAppVersion);
                }
            }
    } else {
        output += "\nThere are no enabled scripts.\n";
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

    // add information about the system environment
    output += "\n## System environment\n\n";

    itr = QProcess::systemEnvironment();
    while (itr.hasNext()) {
        QStringList textList = itr.next().split("=");
        QString key = textList.first();
        textList.removeFirst();
        QString value = textList.join("=");
        output += prepareDebugInformationLine(key, value);
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

    if (data.contains("\n")) {
        data = "\n```\n" + data.trimmed() + "\n```";
    } else {
        data = (data.isEmpty()) ? "*empty*" : "`" + data + "`";
    }

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
void SettingsDialog::connectTestCallback(bool appIsValid,
                                         QString appVersion,
                                         QString serverVersion,
                                         QString notesPathExistsText,
                                         QString connectionErrorMessage) {
    this->appIsValid = appIsValid;
    this->appVersion = appVersion;
    this->serverVersion = serverVersion;
    this->notesPathExistsText = notesPathExistsText;
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

void SettingsDialog::refreshTodoCalendarList(QList<CalDAVCalendarData> items,
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

    QListIterator<CalDAVCalendarData> itr(items);
    while (itr.hasNext()) {
        CalDAVCalendarData data = itr.next();
        QString url = data.url;
        QString name = data.displayName;

        // only add the server url if it wasn't already added
        if (!url.startsWith(serverUrlText)) {
            url = serverUrlText + url;
        }

        // get the hash out of the url part
        QRegularExpression regex("\\/([^\\/]*)\\/$");
        QRegularExpressionMatch match = regex.match(url);
        QString hash = match.captured(1);

        // remove percent encoding
        hash = QUrl::fromPercentEncoding(hash.toUtf8());

        // skip the contact birthdays calendar
        if (hash == "contact_birthdays") {
            continue;
        }

        // skip the Calendar Plus birthday calendar
        if (hash.startsWith("bdaycpltocal_")) {
            continue;
        }

        if (name.isEmpty()) {
            name = hash;
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
    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->settingsGetCalendarList(this);
}

void SettingsDialog::on_defaultOwnCloudCalendarRadioButton_toggled(
        bool checked) {
    if (checked) {
        on_reloadCalendarListButton_clicked();
    }
}

void SettingsDialog::on_legacyOwnCloudCalendarRadioButton_toggled(
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
    FileDialog dialog("SaveDebugInfo");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Markdown files") + " (*.md)");
    dialog.setWindowTitle(tr("Save debug information"));
    dialog.selectFile("QOwnNotes Debug Information.md");
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();
        QFile file(fileName);

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
                tr("Disable usage tracking"),
                tr("Anonymous usage data helps to decide what parts of "
                        "QOwnNotes to improve next and to find and fix bugs."
                        "<br />Please disable it only if you really can't live"
                        " with it.<br /><br />Really disable usage tracking?"),
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

        // remove the log file
        removeLogFile();

        // make sure no settings get written after after are quitting
        qApp->setProperty("clearAppDataAndExit", true);
        qApp->quit();
    }
}

/**
 * Removes the log file
 */
void SettingsDialog::removeLogFile() const {
    // remove log file if exists
    QFile file(Utils::Misc::logFilePath());
    if (file.exists()) {
        // remove the file
        bool result = file.remove();
        QString text = result ? "Removed" : "Could not remove";

        // in case that the settings are cleared logging to log file is
        // disabled by default and it will not be created again
        qWarning() << text + " log file: " << file.fileName();
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
    QString path = ui->externalEditorPathLineEdit->text();
    QString dirPath = path;

    // get the path of the directory if a editor path was set
    if (!path.isEmpty()) {
        dirPath = QFileInfo(path).dir().path();
    }

    // in portable mode the data path will be opened if path was empty
    if (path.isEmpty() && Utils::Misc::isInPortableMode()) {
        dirPath = Utils::Misc::portableDataPath();
    }

    QStringList mimeTypeFilters;
    mimeTypeFilters << "application/x-executable" << "application/octet-stream";

    FileDialog dialog("ExternalEditor");

    if (!dirPath.isEmpty()) {
        dialog.setDirectory(dirPath);
    }

    if (!path.isEmpty()) {
        dialog.selectFile(path);
    }

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
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
        ui->noteFolderGitCommitCheckBox->setChecked(
                _selectedNoteFolder.isUseGit());

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
    _selectedNoteFolder.suggestRemotePath();
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

    OwnCloudService *ownCloud = OwnCloudService::instance();
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
        QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    QString folderName =
            generatePathFromCurrentNoteFolderRemotePathItem(current);
    noteFolderRemotePathTreeStatusBar->showMessage(
            tr("Loading folders in '%1' from server").arg(current->text(0)));

    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->settingsGetFileList(this, folderName);
}

void SettingsDialog::on_useOwnCloudPathButton_clicked() {
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
    // reload the script list
    reloadScriptList();

    QString issueUrl = "https://github.com/pbek/QOwnNotes/issues";
    QString documentationUrl =
            "https://docs.qownnotes.org/en/develop/scripting/README.html";
    ui->scriptInfoLabel->setText(
            tr("Take a look at the <a href=\"%1\">Scripting documentation</a> "
                       "to get started fast.").arg(documentationUrl) + "<br>" +
                    tr("If you need access to a certain functionality in "
                               "QOwnNotes please open an issue on the "
                               "<a href=\"%1\"> QOwnNotes issue page</a>.").arg(
                            issueUrl));

    /*
     * Setup the "add script" button menu
     */
    QMenu *addScriptMenu = new QMenu();

    QAction *searchScriptAction = addScriptMenu->addAction(
            tr("Search script repository"));
    searchScriptAction->setIcon(QIcon::fromTheme(
            "edit-find",
            QIcon(":icons/breeze-qownnotes/16x16/edit-find.svg")));
    searchScriptAction->setToolTip(tr("Find a script in the script "
                                              "repository"));
    connect(searchScriptAction, SIGNAL(triggered()),
            this, SLOT(searchScriptInRepository()));

    QAction *addAction = addScriptMenu->addAction(tr("Add local script"));
    addAction->setIcon(QIcon::fromTheme(
            "document-new",
            QIcon(":icons/breeze-qownnotes/16x16/document-new.svg")));
    addAction->setToolTip(tr("Add an existing, local script"));
    connect(addAction, SIGNAL(triggered()), this, SLOT(addLocalScript()));

    ui->scriptAddButton->setMenu(addScriptMenu);
}

/**
 * Reloads the script list
 */
void SettingsDialog::reloadScriptList() const {
    QList<Script> scripts = Script::fetchAll();
    int scriptsCount = scripts.count();
    ui->scriptListWidget->clear();

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
}

/**
 * Adds a new script
 */
void SettingsDialog::addLocalScript() {
    _selectedScript = Script();
    _selectedScript.setName(_newScriptName);
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

        // open the dialog to select the script
        on_scriptPathButton_clicked();
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
    QString path = ui->scriptPathLineEdit->text();
    QString dirPath = path;

    // get the path of the script if a script was set
    if (!path.isEmpty()) {
        dirPath = QFileInfo(path).dir().path();
    }

    // in portable mode the data path will be opened if path was empty
    if (path.isEmpty() && Utils::Misc::isInPortableMode()) {
        dirPath = Utils::Misc::portableDataPath();
    }

    FileDialog dialog("ScriptPath");

    if (!dirPath.isEmpty()) {
        dialog.setDirectory(dirPath);
    }

    if (!path.isEmpty()) {
        dialog.selectFile(path);
    }

    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("QML files") + " (*.qml)");
    dialog.setWindowTitle(tr("Please select your QML file"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        path = dialog.selectedFile();

        QFile file(path);

        if (file.exists() && (path != "")) {
            QString scriptName = _selectedScript.getName();

            // set the script name from the file name if none was set yet
            if (scriptName.isEmpty() || (scriptName == _newScriptName)) {
                scriptName = QFileInfo(file).baseName();
                ui->scriptNameLineEdit->setText(scriptName);
                ui->scriptNameLabel->setText(scriptName);
                _selectedScript.setName(scriptName);

                const QSignalBlocker blocker(ui->scriptListWidget);
                Q_UNUSED(blocker);
                ui->scriptListWidget->currentItem()->setText(scriptName);
            }

            ui->scriptPathLineEdit->setText(path);
            _selectedScript.setScriptPath(path);
            _selectedScript.store();

            // validate the script
            validateCurrentScript();

            // reload the scripting engine
            ScriptingService::instance()->reloadEngine();

            // trigger the item change so that the page is reloaded for
            // script variables
            reloadCurrentScriptPage();
        }
    }
}

/**
 * Loads the current script in the UI when the current item changed
 */
void SettingsDialog::on_scriptListWidget_currentItemChanged(
        QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(current);
    Q_UNUSED(previous);

    reloadCurrentScriptPage();
}

/**
 * Loads the current script in the UI
 */
void SettingsDialog::reloadCurrentScriptPage() {
    QListWidgetItem *item = ui->scriptListWidget->currentItem();

    if (item == Q_NULLPTR) {
        return;
    }

    ui->scriptValidationLabel->clear();

    int scriptId = item->data(Qt::UserRole).toInt();
    _selectedScript = Script::fetch(scriptId);
    if (_selectedScript.isFetched()) {
        ui->scriptNameLabel->setText(
                "<b>" + _selectedScript.getName() + "</b>");
        ui->scriptPathLineEdit->setText(_selectedScript.getScriptPath());
        ui->scriptEditFrame->setEnabled(true);

        bool isScriptFromRepository = _selectedScript.isScriptFromRepository();
        ui->scriptNameLineEdit->setReadOnly(isScriptFromRepository);
        ui->scriptPathButton->setDisabled(isScriptFromRepository);
        ui->scriptRepositoryItemFrame->setVisible(isScriptFromRepository);
        ui->localScriptItemFrame->setHidden(isScriptFromRepository);
        ui->scriptNameLineEdit->setHidden(isScriptFromRepository);
        ui->scriptNameLineEditLabel->setHidden(isScriptFromRepository);

        // add additional information if script was from the script repository
        if (isScriptFromRepository) {
            ScriptInfoJson infoJson = _selectedScript.getScriptInfoJson();

            ui->scriptVersionLabel->setText(infoJson.version);
            ui->scriptDescriptionLabel->setText(infoJson.description);
            ui->scriptAuthorsLabel->setText(infoJson.richAuthorText);
            ui->scriptRepositoryLinkLabel->setText(
                    "<a href=\"https://github.com/qownnotes/scripts/tree/"
                            "master/" + infoJson.identifier + "\">" +
                            tr("Open repository") + "</a>");
        } else {
            ui->scriptNameLineEdit->setText(_selectedScript.getName());
        }

        // get the registered script settings variables
        QList<QVariant> variables = ScriptingService::instance()
                ->getSettingsVariables(_selectedScript.getId());

        bool hasScriptSettings = variables.count() > 0;
        ui->scriptSettingsFrame->setVisible(hasScriptSettings);

        if (hasScriptSettings) {
            // remove the current ScriptSettingWidget widgets in the
            // scriptSettingsFrame
            QList<ScriptSettingWidget *> widgets = ui->scriptSettingsFrame
                    ->findChildren<ScriptSettingWidget *>();
            Q_FOREACH(ScriptSettingWidget *widget, widgets) {
                    delete widget;
                }

            foreach (QVariant variable, variables) {
                    QMap<QString, QVariant> varMap = variable.toMap();

                    // populate the variable UI
                    ScriptSettingWidget *scriptSettingWidget =
                            new ScriptSettingWidget(this, _selectedScript,
                                                    varMap);

                    QString name = varMap["name"].toString();

                    ui->scriptSettingsFrame->layout()->addWidget(
                            scriptSettingWidget);
            }
        }

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
    // store the enabled states and reload the scripting engine
    storeScriptListEnabledState();

    // trigger the item change so that the page is reloaded for
    // script variables
    reloadCurrentScriptPage();
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

    QListWidgetItem *item = addCustomNoteFileExtension(fileExtension);

    if (item != Q_NULLPTR) {
        ui->defaultNoteFileExtensionListWidget->setCurrentItem(item);
    }
}

/**
 * Adds a custom note file extension
 */
QListWidgetItem *SettingsDialog::addCustomNoteFileExtension(
        const QString &fileExtension) {
    if (listWidgetValueExists(ui->defaultNoteFileExtensionListWidget,
                              fileExtension)) {
        return Q_NULLPTR;
    }

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
    ui->darkModeInfoLabel->setVisible(checked);

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
                    foundKeySequence = keySequence.toString().contains(
                            arg1, Qt::CaseInsensitive);
                }

                item->setHidden(!foundItems.contains(item) &&
                                        !foundKeySequence);
            }

        // show items again that have visible children so that they are
        // really shown
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                if (Utils::Gui::isOneTreeWidgetItemChildVisible(item)) {
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
        if (connectionTestCanBeStarted()) {
            on_connectButton_clicked();
        }
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
    Q_UNUSED(event);

    // make sure no settings get written after after we got the
    // clearAppDataAndExit call
    if (qApp->property("clearAppDataAndExit").toBool()) {
        return;
    }

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

/**
 * Removes all calendar items
 */
void SettingsDialog::on_emptyCalendarCachePushButton_clicked() {
    CalendarItem::removeAll();

    QMessageBox::information(this, tr("Calendar cache emptied"),
                             tr("Your calendar cache was emptied."));
}

/**
 * Resets the item height
 */
void SettingsDialog::on_itemHeightResetButton_clicked() {
    QTreeWidget treeWidget(this);
    QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();
    treeWidget.addTopLevelItem(treeWidgetItem);
    int height = treeWidget.visualItemRect(treeWidgetItem).height();
    ui->itemHeightSpinBox->setValue(height);
}

/**
 * Resets the icon seize
 */
void SettingsDialog::on_toolbarIconSizeResetButton_clicked() {
    QToolBar toolbar(this);
    ui->toolbarIconSizeSpinBox->setValue(toolbar.iconSize().height());
}

void SettingsDialog::on_ignoreNonTodoCalendarsCheckBox_toggled(bool checked) {
    QSettings settings;
    settings.setValue("ownCloud/ignoreNonTodoCalendars", checked);
}

void SettingsDialog::on_applyToolbarButton_clicked() {
    ui->toolbarEditor->apply();

    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == Q_NULLPTR) {
        return;
    }

    // get all available toolbar names from the toolbar editor
    QStringList toolbarObjectNames = ui->toolbarEditor->toolbarObjectNames();

    QList<ToolbarContainer> toolbarContainers;
    foreach(QToolBar* toolbar, mainWindow->findChildren<QToolBar*>()) {
            QString name = toolbar->objectName();

            // don't store the custom actions toolbar and toolbars that are
            // not in the toolbar edit widget any more (for some reason they
            // are still found by findChildren)
            if (name == "customActionsToolbar" ||
                    !toolbarObjectNames.contains(name)) {
                continue;
            }

            toolbarContainers.append(toolbar);

            // update the icon size
            ToolbarContainer::updateIconSize(toolbar);
        }

    QSettings settings;

    // remove the current toolbars
//    settings.beginGroup("toolbar");
//    settings.remove("");
//    settings.endGroup();

    settings.beginWriteArray("toolbar", toolbarContainers.size());

    for (int i = 0; i < toolbarContainers.size(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("name", toolbarContainers[i].name);
        settings.setValue("title", toolbarContainers[i].title);
        settings.setValue("items", toolbarContainers[i].actions);
    }

    settings.endArray();
}

void SettingsDialog::on_resetToolbarPushButton_clicked() {
    if (QMessageBox::information(
            this, tr("Reset toolbars and exit"),
            tr("Do you really want to reset all toolbars? "
                       "The application will be closed in the process, the "
                       "default toolbars will be restored when you start it "
                       "again."),
            tr("Reset and &exit"), tr("&Cancel"), QString::null,
            1) == 0) {
        QSettings settings;

        // remove all settings in the group
        settings.beginGroup("toolbar");
        settings.remove("");
        settings.endGroup();

        qApp->quit();
    }
}

/**
 * Toggles the visibility of the image scaling frame
 *
 * @param checked
 */
void SettingsDialog::on_imageScaleDownCheckBox_toggled(bool checked) {
    ui->imageScalingFrame->setVisible(checked);
}

/**
 * Searches for text in the whole settings dialog and filters the settings
 * tree widget
 *
 * @param arg1
 */
void SettingsDialog::on_searchLineEdit_textChanged(const QString &arg1) {
    QList<QTreeWidgetItem*> allItems = ui->settingsTreeWidget->
            findItems("", Qt::MatchContains | Qt::MatchRecursive);

    // search text if at least one character was entered
    if (arg1.count() >= 1) {
        QList<int> pageIndexList;

        // search in the tree widget items themselves
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                if (item->text(0).contains(arg1, Qt::CaseInsensitive)) {
                    int pageIndex = item->whatsThis(0).toInt();

                    if (!pageIndexList.contains(pageIndex)) {
                        pageIndexList << pageIndex;
                    }
                }
            }

        // search in all labels
        Q_FOREACH(QLabel *widget, findChildren<QLabel *>()) {
                if (widget->text().contains(arg1, Qt::CaseInsensitive)) {
                    addToSearchIndexList(widget, pageIndexList);
                }
            }

        // search in all push buttons
        Q_FOREACH(QPushButton *widget, findChildren<QPushButton *>()) {
                if (widget->text().contains(arg1, Qt::CaseInsensitive)) {
                    addToSearchIndexList(widget, pageIndexList);
                }
            }

        // search in all checkboxes
        Q_FOREACH(QCheckBox *widget, findChildren<QCheckBox *>()) {
                if (widget->text().contains(arg1, Qt::CaseInsensitive)) {
                    addToSearchIndexList(widget, pageIndexList);
                }
            }

        // search in all radio buttons
        Q_FOREACH(QRadioButton *widget, findChildren<QRadioButton *>()) {
                if (widget->text().contains(arg1, Qt::CaseInsensitive)) {
                    addToSearchIndexList(widget, pageIndexList);
                }
            }

        // search in all group boxes
        Q_FOREACH(QGroupBox *widget, findChildren<QGroupBox *>()) {
                if (widget->title().contains(arg1, Qt::CaseInsensitive)) {
                    addToSearchIndexList(widget, pageIndexList);
                }
            }

        // show and hide items according of if index was found in pageIndexList
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                // get stored index of list widget item
                int pageIndex = item->whatsThis(0).toInt();
                item->setHidden(!pageIndexList.contains(pageIndex));
            }

        // show items again that have visible children so that they are
        // really shown
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                if (Utils::Gui::isOneTreeWidgetItemChildVisible(item)) {
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
 * Adds the page index of a widget to the pageIndexList if not already added
 *
 * @param widget
 * @param pageIndexList
 */
void SettingsDialog::addToSearchIndexList(QWidget *widget,
                                          QList<int> &pageIndexList) {
    // get the page id of the widget
    int pageIndex = findSettingsPageIndexOfWidget(widget);

    // add page id if not already added
    if (!pageIndexList.contains(pageIndex)) {
        pageIndexList << pageIndex;
    }
}

/**
 * Finds the settings page index of a widget
 *
 * @param widget
 * @return
 */
int SettingsDialog::findSettingsPageIndexOfWidget(QWidget *widget) {
    QWidget *parent = qobject_cast<QWidget*>(widget->parent());

    if (parent == Q_NULLPTR) {
        return -1;
    }

    // check if the parent is our settings stacked widget
    if (parent->objectName() == "settingsStackedWidget") {
        // get the index of the object in the settings stacked widget
        return ui->settingsStackedWidget->indexOf(widget);
    }

    // search for the page id in the parent
    return findSettingsPageIndexOfWidget(parent);
}

/**
 * Toggles the log file frame
 *
 * @param checked
 */
void SettingsDialog::on_fileLoggingCheckBox_toggled(bool checked) {
    ui->logFileFrame->setVisible(checked);
}

/**
 * Removes the log file
 */
void SettingsDialog::on_clearLogFileButton_clicked() {
    // remove the log file
    removeLogFile();

    QMessageBox::information(
            this, tr("Log file cleared"),
            tr("The log file <strong>%1</strong> was cleared"".").arg(
                    Utils::Misc::logFilePath()));
}

/**
 * Declares that we need a restart
 */
void SettingsDialog::needRestart() {
    Utils::Misc::needRestart();
}

void SettingsDialog::on_ownCloudSupportCheckBox_toggled() {
    bool checked = ui->ownCloudSupportCheckBox->isChecked();
    ui->ownCloudGroupBox->setEnabled(checked);
}

/**
 * Toggles whether to use git to store a local history or not
 * @param checked
 */
void SettingsDialog::on_noteFolderGitCommitCheckBox_toggled(bool checked) {
    _selectedNoteFolder.setUseGit(checked);
    _selectedNoteFolder.store();
}

void SettingsDialog::on_setGitPathToolButton_clicked() {
    QString path = ui->gitPathLineEdit->text();
    if (path.isEmpty()) {
#ifdef Q_OS_WIN
        path = "git.exe";
#else
        path = "/usr/bin/git";
#endif
    }

#ifdef Q_OS_WIN
    QStringList filters = QStringList() << tr("Executable files") + " (*.exe)"
                                        << tr("All files") + " (*)";
#else
    QStringList filters = QStringList() << tr("All files") + " (*)";
#endif

    FileDialog dialog("GitExecutable");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilters(filters);
    dialog.selectFile(path);
    dialog.setWindowTitle(tr("Please select the path of your git executable"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        path = dialog.selectedFile();

        if (!path.isEmpty()) {
            ui->gitPathLineEdit->setText(path);
        }
    }
}

/**
 * Opens a dialog to search for scripts in the script repository
 */
void SettingsDialog::searchScriptInRepository() {
    ScriptRepositoryDialog *dialog = new ScriptRepositoryDialog(this);
    dialog->exec();
    delete(dialog);

    // reload the script list
    reloadScriptList();

    // reload the scripting engine
    ScriptingService::instance()->reloadEngine();
}

/**
 * Saves the enabled state of all items and reload the current script page to
 * make the script settings available when a script was enabled or disabled
 *
 * @param item
 */
void SettingsDialog::on_scriptListWidget_itemChanged(QListWidgetItem *item) {
    Q_UNUSED(item);

    storeScriptListEnabledState();
    reloadCurrentScriptPage();
}

void SettingsDialog::on_interfaceStyleComboBox_currentTextChanged(
        const QString &arg1) {
    QApplication::setStyle(arg1);

    // if the interface style was set to automatic we need a restart
    if (ui->interfaceStyleComboBox->currentIndex() == 0) {
        needRestart();
    }
}

/**
 * Reset the cursor width spin box value
 */
void SettingsDialog::on_cursorWidthResetButton_clicked() {
    ui->cursorWidthSpinBox->setValue(1);
}
