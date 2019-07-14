#include <utility>
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
#include <QButtonGroup>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

SettingsDialog::SettingsDialog(int page, QWidget *parent) :
        MasterDialog(parent), ui(new Ui::SettingsDialog) {
    ui->setupUi(this);

    bool fromWelcomeDialog = parent->objectName() == "WelcomeDialog";

    MainWindow *mainWindow = MainWindow::instance();

    // if there was no size set yet and we already have a main window we'll
    // mimic that size
    if (mainWindow != Q_NULLPTR) {
        resize(mainWindow->width(), mainWindow->height());
    } else {
        // we must not use resize(1, 1) because XFCE really resizes the window to 1x1
        resize(800, 600);
    }

    QList<QWidget*> pageWidgets;

    // get a list of every settings page in the correct order
    for (int index = 0; index < ui->settingsStackedWidget->count(); index++) {
        pageWidgets.append(ui->settingsStackedWidget->widget(index));
    }

    Q_FOREACH(QWidget* pageWidget, pageWidgets) {
            // make sure the margin of every page is 0
            QLayout *layout = pageWidget->layout();
            layout->setMargin(0);

            // inject a scroll area to make each page scrollable
            auto *scrollArea = new QScrollArea(
                    ui->settingsStackedWidget);
            scrollArea->setWidget(pageWidget);
            scrollArea->setWidgetResizable(true);
            scrollArea->setFrameShape(QFrame::NoFrame);
            ui->settingsStackedWidget->addWidget(scrollArea);
        }

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
    ui->automaticNoteFolderDatabaseClosingCheckBox->hide();
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

    if (!fromWelcomeDialog) {
        // setup the note folder tab
        setupNoteFolderPage();

        // setup the scripting tab
        setupScriptingPage();
    }

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
    connect(ui->systemIconThemeCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->darkModeTrayIconCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->darkModeIconThemeCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->darkModeColorsCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->darkModeCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->allowOnlyOneAppInstanceCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->showSystemTrayCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->startHiddenCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->fullyHighlightedBlockquotesCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->noteEditCentralWidgetCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->noteListPreviewCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->vimModeCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
    connect(ui->ignoreNoteSubFoldersLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(needRestart()));
    connect(ui->enableSocketServerCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(needRestart()));
//    connect(ui->layoutWidget, SIGNAL(settingsStored()),
//            this, SLOT(needRestart()));

    // connect the panel sort radio buttons
    connect(ui->notesPanelSortAlphabeticalRadioButton, SIGNAL(toggled(bool)),
            ui->notesPanelOrderGroupBox, SLOT(setEnabled(bool)));
    connect(ui->noteSubfoldersPanelShowRootFolderNameCheckBox, SIGNAL(toggled(bool)),
            ui->noteSubfoldersPanelShowFullPathCheckBox, SLOT(setEnabled(bool)));
    connect(ui->noteSubfoldersPanelSortAlphabeticalRadioButton, SIGNAL(toggled(bool)),
            ui->noteSubfoldersPanelOrderGroupBox, SLOT(setEnabled(bool)));
    connect(ui->tagsPanelSortAlphabeticalRadioButton, SIGNAL(toggled(bool)),
            ui->tagsPanelOrderGroupBox, SLOT(setEnabled(bool)));

    // setup the search engine combo-box
    initSearchEngineComboBox();

#ifdef Q_OS_MAC
    // there is no system tray in OS X
    ui->systemTrayGroupBox->setTitle(tr("Menu bar"));
    ui->showSystemTrayCheckBox->setText(tr("Show menu bar item"));
#endif

    if (fromWelcomeDialog) {
        // hide the whole left side frame with the settings menu tree
        ui->leftSideFrame->setVisible(false);
    }

    if (!ui->noteListPreviewCheckBox->text().contains("(experimental)")) {
        ui->noteListPreviewCheckBox->setText(
                ui->noteListPreviewCheckBox->text() + " (experimental)");
    }

    ui->webCompannionLabel->setText(ui->webCompannionLabel->text().arg(
            "https://github.com/qownnotes/web-companion",
            "https://chrome.google.com/webstore/detail/qownnotes-web-companion/"
            "pkgkfnampapjbopomdpnkckbjdnpkbkp",
            "https://addons.mozilla.org/firefox/addon/qownnotes-web-companion"));
    ui->bookmarkTagLabel->setText(ui->bookmarkTagLabel->text().arg(
            "https://www.qownnotes.org/Knowledge-base/QOwnNotes-Web-Companion-browser-extension"));

#ifndef Q_OS_LINUX
    ui->systemIconThemeCheckBox->setHidden(true);
    ui->systemIconThemeCheckBox->setChecked(false);
#endif
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

    // general settings
    ui->allowDifferentNoteFileNameCheckBox->setToolTip(
            Utils::Misc::replaceOwnCloudText(
                    ui->allowDifferentNoteFileNameCheckBox->toolTip()));

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
    settings.setValue("newNoteAskHeadline",
                      ui->newNoteAskHeadlineCheckBox->isChecked());
    settings.setValue("useUNIXNewline",
                      ui->useUNIXNewlineCheckBox->isChecked());
    settings.setValue("restoreCursorPosition",
                      ui->restoreCursorPositionCheckBox->isChecked());
    settings.setValue("noteSaveIntervalTime",
                      ui->noteSaveIntervalTime->value());
    settings.setValue("defaultNoteFileExtension",
                      getSelectedListWidgetValue(
                              ui->defaultNoteFileExtensionListWidget));
    settings.setValue("localTrash/supportEnabled",
                      ui->localTrashEnabledCheckBox->isChecked());
    settings.setValue("localTrash/autoCleanupEnabled",
                      ui->localTrashClearCheckBox->isChecked());
    settings.setValue("localTrash/autoCleanupDays",
                      ui->localTrashClearTimeSpinBox->value());
    settings.setValue("enableSocketServer",
                      ui->enableSocketServerCheckBox->isChecked());

    // make the path relative to the portable data path if we are in
    // portable mode
    settings.setValue("externalEditorPath",
                      Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                              ui->externalEditorPathLineEdit->text()));

    settings.setValue("overrideInterfaceFontSize",
                      ui->overrideInterfaceFontSizeGroupBox->isChecked());
    settings.setValue("interfaceFontSize", ui->interfaceFontSizeSpinBox->value());
    settings.setValue("itemHeight", ui->itemHeightSpinBox->value());
    settings.setValue("MainWindow/mainToolBar.iconSize",
                      ui->toolbarIconSizeSpinBox->value());
    settings.setValue("allowOnlyOneAppInstance",
                      ui->allowOnlyOneAppInstanceCheckBox->isChecked());
    settings.setValue("closeTodoListAfterSave",
                      ui->closeTodoListAfterSaveCheckBox->isChecked());
    settings.setValue("interfaceLanguage",
                      getSelectedListWidgetValue(ui->languageListWidget));
    settings.setValue("markdownHighlightingEnabled",
                      ui->markdownHighlightingCheckBox->isChecked());
    settings.setValue("fullyHighlightedBlockquotes",
                      ui->fullyHighlightedBlockquotesCheckBox->isChecked());
    settings.setValue("noteEditIsCentralWidget",
                      ui->noteEditCentralWidgetCheckBox->isChecked());
    settings.setValue("MainWindow/noteTextView.rtl",
                      ui->noteTextViewRTLCheckBox->isChecked());
    settings.setValue("MainWindow/noteTextView.ignoreCodeFontSize",
                      ui->noteTextViewIgnoreCodeFontSizeCheckBox->isChecked());
    settings.setValue("MainWindow/noteTextView.underline",
                      ui->noteTextViewUnderlineCheckBox->isChecked());
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
    settings.setValue("Editor/editorWidthInDFMOnly",
                      ui->editorWidthInDFMOnlyCheckBox->isChecked());
    settings.setValue("Editor/vimMode",
                      ui->vimModeCheckBox->isChecked());

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

    settings.setValue("darkModeIconTheme",
                      ui->darkModeIconThemeCheckBox->isChecked());

    settings.setValue("internalIconTheme",
                      ui->internalIconThemeCheckBox->isChecked());

    settings.setValue("systemIconTheme",
                      ui->systemIconThemeCheckBox->isChecked());

    QStringList todoCalendarUrlList;
    QStringList todoCalendarDisplayNameList;
    QStringList todoCalendarEnabledList;
    QStringList todoCalendarEnabledUrlList;
    for (int i = 0; i < ui->todoCalendarListWidget->count(); i++) {
        QListWidgetItem *item = ui->todoCalendarListWidget->item(i);

        todoCalendarUrlList.append(item->toolTip());
        todoCalendarDisplayNameList.append(item->text());

        if (item->checkState() == Qt::Checked) {
            todoCalendarEnabledList.append(item->text());
            todoCalendarEnabledUrlList.append(item->toolTip());
        }
    }

    // store the tasks calendar data to the settings
    settings.setValue("ownCloud/todoCalendarUrlList", todoCalendarUrlList);
    settings.setValue("ownCloud/todoCalendarDisplayNameList",
                      todoCalendarDisplayNameList);
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

    settings.setValue("ShowSystemTray",
                      ui->showSystemTrayCheckBox->isChecked());
    settings.setValue("StartHidden",
                      ui->startHiddenCheckBox->isChecked());
    settings.setValue("automaticNoteFolderDatabaseClosing",
                      ui->automaticNoteFolderDatabaseClosingCheckBox->
                              isChecked());

    settings.setValue("webSocketServerService/port",
                      ui->webSocketServerServicePortSpinBox->value());
    settings.setValue("webSocketServerService/bookmarksTag",
                      ui->bookmarksTagLineEdit->text());
    settings.setValue("webSocketServerService/bookmarksNoteName",
                      ui->bookmarksNoteNameLineEdit->text());
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
                      ui->noteSubfoldersPanelDisplayAsFullTreeCheckBox
                              ->isChecked());

    settings.setValue("noteSubfoldersPanelShowRootFolderName",
                      ui->noteSubfoldersPanelShowRootFolderNameCheckBox
                              ->isChecked());

    settings.setValue("noteSubfoldersPanelShowNotesRecursively",
                      ui->noteSubfoldersPanelShowNotesRecursivelyCheckBox
                              ->isChecked());

    settings.setValue("disableSavedSearchesAutoCompletion",
                      ui->disableSavedSearchesAutoCompletionCheckBox
                      ->isChecked());

    settings.setValue("noteSubfoldersPanelShowFullPath",
                      ui->noteSubfoldersPanelShowFullPathCheckBox->isChecked());

    ui->noteSubfoldersPanelSortAlphabeticalRadioButton->isChecked() ?
                settings.setValue("noteSubfoldersPanelSort", SORT_ALPHABETICAL) :
                settings.setValue("noteSubfoldersPanelSort", SORT_BY_LAST_CHANGE);

    ui->noteSubfoldersPanelOrderDescendingRadioButton->isChecked() ?
                settings.setValue("noteSubfoldersPanelOrder", ORDER_DESCENDING) :
                settings.setValue("noteSubfoldersPanelOrder", ORDER_ASCENDING);

    const QSignalBlocker blocker(ui->ignoreNoteSubFoldersLineEdit);
    settings.setValue("ignoreNoteSubFolders",
            ui->ignoreNoteSubFoldersLineEdit->text());

    // Tags Panel Options
    settings.setValue("tagsPanelHideSearch", ui->tagsPanelHideSearchCheckBox
            ->isChecked());

    settings.setValue("taggingShowNotesRecursively",
                      ui->taggingShowNotesRecursivelyCheckBox->isChecked());
    settings.setValue("noteListPreview",
                      ui->noteListPreviewCheckBox->isChecked());

    ui->tagsPanelSortAlphabeticalRadioButton->isChecked() ?
                settings.setValue("tagsPanelSort", SORT_ALPHABETICAL) :
                settings.setValue("tagsPanelSort", SORT_BY_LAST_CHANGE);

    ui->tagsPanelOrderDescendingRadioButton->isChecked() ?
                settings.setValue("tagsPanelOrder", ORDER_DESCENDING) :
                settings.setValue("tagsPanelOrder", ORDER_ASCENDING);

    // Navigation Panel Options
    settings.setValue("navigationPanelHideSearch",
            ui->navigationPanelHideSearchCheckBox->isChecked());
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
    ui->newNoteAskHeadlineCheckBox->setChecked(
            settings.value("newNoteAskHeadline").toBool());
    ui->useUNIXNewlineCheckBox->setChecked(
            settings.value("useUNIXNewline").toBool());
    ui->localTrashEnabledCheckBox->setChecked(
            settings.value("localTrash/supportEnabled", true).toBool());
    ui->localTrashClearCheckBox->setChecked(
            settings.value("localTrash/autoCleanupEnabled", true).toBool());
    ui->localTrashClearTimeSpinBox->setValue(
            settings.value("localTrash/autoCleanupDays", 30).toInt());
    ui->enableSocketServerCheckBox->setChecked(
            Utils::Misc::isSocketServerEnabled());
    on_enableSocketServerCheckBox_toggled();

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
    ui->noteTextViewIgnoreCodeFontSizeCheckBox->setChecked(
            settings.value("MainWindow/noteTextView.ignoreCodeFontSize", true).toBool());
    ui->noteTextViewUnderlineCheckBox->setChecked(
            settings.value("MainWindow/noteTextView.underline", true).toBool());
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
    ui->editorWidthInDFMOnlyCheckBox->setChecked(
            settings.value("Editor/editorWidthInDFMOnly", true).toBool());
    ui->vimModeCheckBox->setChecked(settings.value("Editor/vimMode").toBool());
    ui->markdownHighlightingCheckBox->setChecked(
            settings.value("markdownHighlightingEnabled", true).toBool());
    ui->fullyHighlightedBlockquotesCheckBox->setChecked(
            settings.value("fullyHighlightedBlockquotes").toBool());
    ui->noteEditCentralWidgetCheckBox->setChecked(
            settings.value("noteEditIsCentralWidget", true).toBool());
    ui->allowOnlyOneAppInstanceCheckBox->setChecked(settings.value(
            "allowOnlyOneAppInstance").toBool());
    ui->closeTodoListAfterSaveCheckBox->setChecked(settings.value(
            "closeTodoListAfterSave").toBool());
    ui->toolbarIconSizeSpinBox->setValue(
                 settings.value("MainWindow/mainToolBar.iconSize").toInt());

    const QSignalBlocker overrideInterfaceFontSizeGroupBoxBlocker(
            ui->overrideInterfaceFontSizeGroupBox);
    Q_UNUSED(overrideInterfaceFontSizeGroupBoxBlocker);
    const QSignalBlocker interfaceFontSizeSpinBoxBlocker(
            ui->interfaceFontSizeSpinBox);
    Q_UNUSED(interfaceFontSizeSpinBoxBlocker);
    ui->overrideInterfaceFontSizeGroupBox->setChecked(
            settings.value("overrideInterfaceFontSize", false).toBool());
    ui->interfaceFontSizeSpinBox->setValue(
            settings.value("interfaceFontSize", 11).toInt());

    QTreeWidget treeWidget(this);
    auto *treeWidgetItem = new QTreeWidgetItem();
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

    ui->darkModeIconThemeCheckBox->setChecked(Utils::Misc::isDarkModeIconTheme());

    ui->internalIconThemeCheckBox->setChecked(settings.value(
            "internalIconTheme").toBool());

    ui->systemIconThemeCheckBox->setChecked(settings.value(
            "systemIconTheme").toBool());

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
        auto *textEdit = new QTextEdit();
        fontString = textEdit->font().toString();
        settings.setValue("MainWindow/noteTextView.font", fontString);
        delete textEdit;
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

    // reload the calendar list if it was empty
    if (todoCalendarUrlListCount == 0) {
        reloadCalendarList();
    }

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

    const QSignalBlocker blocker8(this->ui->showSystemTrayCheckBox);
    Q_UNUSED(blocker8);
    bool showSystemTray = settings.value("ShowSystemTray").toBool();
    ui->showSystemTrayCheckBox->setChecked(showSystemTray);
    ui->startHiddenCheckBox->setEnabled(showSystemTray);
    ui->startHiddenCheckBox->setChecked(settings.value("StartHidden").toBool());
    if (!showSystemTray) {
        ui->startHiddenCheckBox->setChecked(false);
    }

    ui->automaticNoteFolderDatabaseClosingCheckBox->setChecked(
            Utils::Misc::doAutomaticNoteFolderDatabaseClosing());

    ui->webSocketServerServicePortSpinBox->setValue(
            WebSocketServerService::getSettingsPort());
    ui->bookmarksTagLineEdit->setText(WebSocketServerService::getBookmarksTag());
    ui->bookmarksNoteNameLineEdit->setText(
            WebSocketServerService::getBookmarksNoteName());
}

/**
 * Does the setup for the search engine combo-box
 */
void SettingsDialog::initSearchEngineComboBox() const {
    QSettings settings;

    // Iterates over the search engines and adds them
    // to the combobox
    QHash<int, Utils::Misc::SearchEngine> searchEngines =
            Utils::Misc::getSearchEnginesHashMap();

    ui->searchEngineSelectionComboBox->clear();

    Q_FOREACH(int id, Utils::Misc::getSearchEnginesIds()) {
            Utils::Misc::SearchEngine searchEngine = searchEngines[id];
            ui->searchEngineSelectionComboBox->addItem(searchEngine.name,
                                                       QString::number(id));
        }

    // Sets the current selected item to the search engine
    // selected previously
    // while also handling the case in which the saved key has
    // been removed from the hash table
    int savedEngineId = settings.value(
            "SearchEngineId", Utils::Misc::getDefaultSearchEngineId()).toInt();
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
    if (settings.value("notesPanelSort", SORT_BY_LAST_CHANGE).toInt() == SORT_ALPHABETICAL) {
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

    ui->noteSubfoldersPanelShowNotesRecursivelyCheckBox->setChecked(
            settings.value("noteSubfoldersPanelShowNotesRecursively"
            ).toBool());

    ui->disableSavedSearchesAutoCompletionCheckBox->setChecked(
            settings.value("disableSavedSearchesAutoCompletion").toBool());

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

    ui->taggingShowNotesRecursivelyCheckBox->setChecked(settings.value(
            "taggingShowNotesRecursively").toBool());
    ui->noteListPreviewCheckBox->setChecked(Utils::Misc::isNoteListPreview());

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

    ui->ignoreNoteSubFoldersLineEdit->setText(settings.value(
            "ignoreNoteSubFolders", IGNORED_NOTE_SUBFOLDERS_DEFAULT).toString());

    // Navigation Panel Options
    ui->navigationPanelHideSearchCheckBox->setChecked(settings.value(
            "navigationPanelHideSearch").toBool());
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

    // shortcuts on toolbars and note folders don't work yet
    auto disabledMenuNames = QStringList() << "menuToolbars" << "noteFoldersMenu";

    // loop through all menus
    foreach(QMenu* menu, menus) {
            if (disabledMenuNames.contains(menu->objectName())) {
                continue;
            }

            auto *menuItem = new QTreeWidgetItem();
            int actionCount = 0;

            // loop through all actions of the menu
            foreach(QAction* action, menu->actions()) {
                    // we don't need empty objects
                    if (action->objectName().isEmpty()) {
                        continue;
                    }

                    // create the tree widget item
                    auto *actionItem = new QTreeWidgetItem();
                    actionItem->setText(0, action->text().remove("&"));
                    actionItem->setToolTip(0, action->objectName());
                    actionItem->setData(1, Qt::UserRole, action->objectName());
                    menuItem->addChild(actionItem);

                    // create the key widget
                    auto *keyWidget = new QKeySequenceWidget();
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

                    QObject::connect(keyWidget,
                                     SIGNAL(keySequenceAccepted(QKeySequence)),
                                     _keyWidgetSignalMapper, SLOT(map()));

                    // add a parameter to the signal mapper
                    _keyWidgetSignalMapper->setMapping(
                            keyWidget, action->objectName());

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

    QObject::connect(_keyWidgetSignalMapper,
                     SIGNAL(mapped(QString)),
                     this,
                     SLOT(keySequenceEvent(QString)));

    ui->shortcutTreeWidget->resizeColumnToContents(0);
}

/**
 * Show an information if a shortcut was already used elsewhere
 *
 * @param objectName
 */
void SettingsDialog::keySequenceEvent(const QString& objectName) {
    QKeySequenceWidget *keySequenceWidget = findKeySequenceWidget(objectName);

    if (keySequenceWidget == Q_NULLPTR) {
        return;
    }

    QKeySequence eventKeySequence = keySequenceWidget->keySequence();

    // skip events with empty key sequence
    if (eventKeySequence.isEmpty()) {
        return;
    }

    // loop all top level tree widget items (menus)
    for (int i = 0; i < ui->shortcutTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *menuItem = ui->shortcutTreeWidget->topLevelItem(i);

        // loop all tree widget items of the menu (action shortcuts)
        for (int j = 0; j < menuItem->childCount(); j++) {
            QTreeWidgetItem *shortcutItem = menuItem->child(j);

            // skip the item that threw the event
            if (shortcutItem->data(1, Qt::UserRole).toString() == objectName) {
                continue;
            }

            auto keyWidget = static_cast<QKeySequenceWidget *>(
                    ui->shortcutTreeWidget->itemWidget(shortcutItem, 1));

            if (keyWidget == Q_NULLPTR) {
                continue;
            }

            QKeySequence keySequence = keyWidget->keySequence();
            QKeySequence defaultKeySequence = keyWidget->defaultKeySequence();

            // show an information if the shortcut was already used elsewhere
            if (keySequence == eventKeySequence) {
                if (Utils::Gui::information(
                        this, tr("Shortcut already assigned"),
                        tr("The shortcut <strong>%1</strong> is already "
                           "assigned to <strong>%2</strong>! Do you want to "
                           "jump to the shortcut?").arg(
                                eventKeySequence.toString(),
                                shortcutItem->text(0)),
                        "settings-shortcut-already-assigned",
                        QMessageBox::Yes | QMessageBox::Cancel,
                        QMessageBox::Yes) ==
                    QMessageBox::Yes) {
                    ui->shortcutTreeWidget->scrollToItem(shortcutItem);
                    ui->shortcutTreeWidget->clearSelection();
                    shortcutItem->setSelected(true);
                }

                return;
            }
        }
    }
}

/**
 * Finds a QKeySequenceWidget in the shortcutTreeWidget by the objectName
 * of the assigned menu action
 */
QKeySequenceWidget *SettingsDialog::findKeySequenceWidget(const QString& objectName) {
    // loop all top level tree widget items (menus)
    for (int i = 0; i < ui->shortcutTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *menuItem = ui->shortcutTreeWidget->topLevelItem(i);

        // loop all tree widget items of the menu (action shortcuts)
        for (int j = 0; j < menuItem->childCount(); j++) {
            QTreeWidgetItem *shortcutItem = menuItem->child(j);

            QString name = shortcutItem->data(1, Qt::UserRole).toString();

            if (name == objectName) {
                return static_cast <QKeySequenceWidget *>(
                        ui->shortcutTreeWidget->itemWidget(shortcutItem, 1));
            }
        }
    }

    return Q_NULLPTR;
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
            auto *keyWidget = static_cast<QKeySequenceWidget *>(
                    ui->shortcutTreeWidget->itemWidget(shortcutItem, 1));

            if (keyWidget == Q_NULLPTR) {
                continue;
            }

            QKeySequence keySequence = keyWidget->keySequence();
            QKeySequence defaultKeySequence = keyWidget->defaultKeySequence();
            QString actionObjectName =
                    shortcutItem->data(1, Qt::UserRole).toString();

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
                                           const QString& value) {
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
                                           const QString& value) {
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

void SettingsDialog::setFontLabel(QLineEdit *label, const QFont& font) {
    label->setText(
            font.family() + " (" + QString::number(font.pointSize()) + ")");
    label->setFont(font);
}

void SettingsDialog::outputSettings() {
    // store some data for Utils::Misc::generateDebugInformation
    storeOwncloudDebugData();

    QString output = Utils::Misc::generateDebugInformation(
            ui->gitHubLineBreaksCheckBox->isChecked());

    ui->debugInfoTextEdit->setPlainText(output);
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
    this->notesPathExistsText = std::move(notesPathExistsText);
    this->connectionErrorMessage = connectionErrorMessage;

    // store some data for Utils::Misc::generateDebugInformation
    storeOwncloudDebugData();

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
void SettingsDialog::setOKLabelData(int number, const QString& text,
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

void SettingsDialog::refreshTodoCalendarList(const QList<CalDAVCalendarData>& items,
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
        QRegularExpression regex(R"(\/([^\/]*)\/$)");
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
        auto *item = new QListWidgetItem(name);

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
    QFont font = Utils::Gui::fontDialogGetFont(&ok, noteTextEditFont, this);

    qDebug() << __func__ << " - 'font': " << font;

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
    QFont font = Utils::Gui::fontDialogGetFont(
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
    QFont font = Utils::Gui::fontDialogGetFont(&ok, noteTextViewFont, this);
    if (ok) {
        noteTextViewFont = font;
        setFontLabel(ui->noteTextViewFontLabel, noteTextViewFont);
    }
}

void SettingsDialog::on_noteTextViewCodeButton_clicked()
{
    bool ok;
    QFont font = Utils::Gui::fontDialogGetFont(
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

        Utils::Gui::information(this, tr("Database"),
                                 tr("The Database was reinitialized."),
                                "database-reinitialized");
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
        if (fileNames.empty()) {
            return;
        }

        const QString& filePath(fileNames.at(0));
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

    noteFolderRemotePathTreeStatusBar = new QStatusBar(this);
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

    if (Utils::Gui::question(
            this,
            tr("Remove note folder"),
            tr("Remove the current note folder <strong>%1</strong>?")
                    .arg(_selectedNoteFolder.getName()),
            "remove-note-folder") == QMessageBox::Yes) {
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
    QString text = ui->noteFolderNameLineEdit->text().remove("\n").trimmed();
    text.truncate(50);
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
                addPathToNoteFolderRemotePathTreeWidget(nullptr, path);
            }
        }
}

void SettingsDialog::addPathToNoteFolderRemotePathTreeWidget(
        QTreeWidgetItem *parent, const QString& path) {
    if (path.isEmpty()) {
        return;
    }

    QStringList pathPartList = path.split("/");
    QString pathPart = pathPartList.takeFirst();
    QTreeWidgetItem *item = findNoteFolderRemotePathTreeWidgetItem(
            parent, pathPart);

    const QSignalBlocker blocker(ui->noteFolderRemotePathTreeWidget);
    Q_UNUSED(blocker);

    if (item == nullptr) {
        item = new QTreeWidgetItem();
        item->setText(0, pathPart);
        if (parent == nullptr) {
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
        QTreeWidgetItem *parent, const QString& text) {
    if (parent == nullptr) {
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

    return nullptr;
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
    if (item == nullptr) {
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
    if (item == nullptr) {
        return "";
    }

    QTreeWidgetItem *parent = item->parent();
    if (parent != nullptr) {
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
    auto *addScriptMenu = new QMenu(this);

    QAction *searchScriptAction = addScriptMenu->addAction(
            tr("Search script repository"));
    searchScriptAction->setIcon(QIcon::fromTheme(
            "edit-find",
            QIcon(":icons/breeze-qownnotes/16x16/edit-find.svg")));
    searchScriptAction->setToolTip(tr("Find a script in the script "
                                              "repository"));
    connect(searchScriptAction, SIGNAL(triggered()),
            this, SLOT(searchScriptInRepository()));

    QAction *updateScriptAction = addScriptMenu->addAction(
            tr("Check for script updates"));
    updateScriptAction->setIcon(QIcon::fromTheme(
            "svn-update",
            QIcon(":icons/breeze-qownnotes/16x16/svn-update.svg")));
    connect(updateScriptAction, SIGNAL(triggered()),
            this, SLOT(checkForScriptUpdates()));

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

    if (Utils::Gui::question(
            this,
            tr("Remove script"),
            tr("Remove the current script <strong>%1</strong>?")
                    .arg(_selectedScript.getName()),
            "remove-script") == QMessageBox::Yes) {
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

    auto *item = new QListWidgetItem(fileExtension);
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
        ui->darkModeIconThemeCheckBox->setChecked(true);
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

                auto *keyWidget = static_cast<QKeySequenceWidget *>(
                        ui->shortcutTreeWidget->itemWidget(item, 1));

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
    const int currentIndex = current->whatsThis(0).toInt();

    ui->settingsStackedWidget->setCurrentIndex(currentIndex);

    if (currentIndex == SettingsPages::LayoutPage) {
        ui->layoutWidget->resizeLayoutImage();
    }
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
    _mainSplitter = new QSplitter(this);
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
 * Stores some data for Utils::Misc::generateDebugInformation
 */
void SettingsDialog::storeOwncloudDebugData() const {
    QSettings settings;
    settings.setValue("ownCloudInfo/appIsValid", appIsValid);
    settings.setValue("ownCloudInfo/notesPathExistsText", notesPathExistsText);
    settings.setValue("ownCloudInfo/serverVersion", serverVersion);
    settings.setValue("ownCloudInfo/connectionErrorMessage",
                      connectionErrorMessage);
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

    Utils::Gui::information(this, tr("Calendar cache emptied"),
                             tr("Your calendar cache was emptied."),
                            "calendar-cache-emptied");
}

/**
 * Resets the item height
 */
void SettingsDialog::on_itemHeightResetButton_clicked() {
    QTreeWidget treeWidget(this);
    auto *treeWidgetItem = new QTreeWidgetItem();
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

    Utils::Gui::information(
            this, tr("Log file cleared"),
            tr("The log file <strong>%1</strong> was cleared"".").arg(
                    Utils::Misc::logFilePath()), "log-file-cleared");
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
void SettingsDialog::searchScriptInRepository(bool checkForUpdates) {
    auto *dialog = new ScriptRepositoryDialog(this, checkForUpdates);
    dialog->exec();
    delete(dialog);

    // reload the script list
    reloadScriptList();

    // reload the scripting engine
    ScriptingService::instance()->reloadEngine();
}

/**
 * Opens a dialog to check for script updates
 */
void SettingsDialog::checkForScriptUpdates() {
    searchScriptInRepository(true);
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

/**
 * Also enable the single instance feature if the system tray icon is turned on
 */
void SettingsDialog::on_showSystemTrayCheckBox_toggled(bool checked)
{
    // we don't need to do that on macOS
#ifndef Q_OS_MAC
    if (checked) {
        ui->allowOnlyOneAppInstanceCheckBox->setChecked(true);
    }
#endif

    ui->startHiddenCheckBox->setEnabled(checked);

    if (!checked) {
        ui->startHiddenCheckBox->setChecked(false);
    }
}

/**
 * Resets the overrides for all message boxes
 */
void SettingsDialog::on_resetMessageBoxesButton_clicked() {
    if (QMessageBox::question(
            this, tr("Reset message boxes"),
            tr("Do you really want to reset the overrides of all message "
                       "boxes?")) == QMessageBox::Yes) {
        QSettings settings;

        // remove all settings in the group
        settings.beginGroup("MessageBoxOverride");
        settings.remove("");
        settings.endGroup();
    }
}

void SettingsDialog::on_markdownHighlightingCheckBox_toggled(bool checked) {
    ui->markdownHighlightingFrame->setEnabled(checked);
}

void SettingsDialog::on_localTrashEnabledCheckBox_toggled(bool checked) {
    ui->localTrashGroupBox->setEnabled(checked);
}

void SettingsDialog::on_localTrashClearCheckBox_toggled(bool checked) {
    ui->localTrashClearFrame->setEnabled(checked);
}

/**
 * Export settings
 */
void SettingsDialog::on_exportSettingsButton_clicked() {
    FileDialog dialog("SettingsExport");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("INI files") + " (*.ini)");
    dialog.setWindowTitle(tr("Export settings"));
    dialog.selectFile("QOwnNotes-settings.ini");
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(".ini");
            }

            QSettings exportSettings(fileName, QSettings::IniFormat);

            // clear the settings in case the settings file already existed
            exportSettings.clear();

            exportSettings.setValue("SettingsExport/platform",
                    QString(PLATFORM));

            QSettings settings;

            const QStringList keys = settings.allKeys();
            Q_FOREACH(QString key, keys) {
                    exportSettings.setValue(key, settings.value(key));
                }
        }
    }
}

/**
 * Import settings
 */
void SettingsDialog::on_importSettingsButton_clicked() {
    QString title = tr("Import settings");
    QString text = tr("Do you really want to import settings? Your current "
                      "settings will get removed and not every setting may "
                      "get restored, like the note folder settings and which "
                      "scripts you were using. "
                      "You also will need to adjust some settings, especially "
                      "across platforms, but your notes will stay intact!") +
                              "\n\n";
    bool singleApplication = qApp->property("singleApplication").toBool();

    text += singleApplication ?
            tr("The application will be quit after the import.") :
            tr("The application will be restarted after the import.");

    if (QMessageBox::question(this, title, text, QMessageBox::Yes |
    QMessageBox::No, QMessageBox::No) == QMessageBox::No) {
        return;
    }

    FileDialog dialog("SettingsExport");
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("INI files") + " (*.ini)");
    dialog.setWindowTitle(tr("Import settings"));
    int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return;
    }

    QString fileName = dialog.selectedFile();
    QSettings settings;
    QSettings importSettings(fileName, QSettings::IniFormat);
    settings.clear();
    DatabaseService::removeDiskDatabase();

    const QStringList keys = importSettings.allKeys();

    Q_FOREACH(QString key, keys) {
            QVariant value = importSettings.value(key);
            settings.setValue(key, value);
        }

    // make sure no settings get written after quitting
    qApp->setProperty("clearAppDataAndExit", true);

    if (singleApplication) {
        qApp->quit();
    } else {
        Utils::Misc::restartApplication();
    }
}

void SettingsDialog::on_issueAssistantPushButton_clicked() {
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow == nullptr) {
        return;
    }

    storeSettings();
    mainWindow->openIssueAssistantDialog();

    // we need to close the modal settings dialog so the issue assistant
    // dialog can be shown on the front
    close();
}

void SettingsDialog::on_ignoreNoteSubFoldersResetButton_clicked() {
    ui->ignoreNoteSubFoldersLineEdit->setText(IGNORED_NOTE_SUBFOLDERS_DEFAULT);
}

void SettingsDialog::on_interfaceFontSizeSpinBox_valueChanged(int arg1) {
    QSettings settings;
    settings.setValue("interfaceFontSize", arg1);
    Utils::Gui::updateInterfaceFontSize(arg1);
}

void SettingsDialog::on_overrideInterfaceFontSizeGroupBox_toggled(bool arg1) {
    QSettings settings;
    settings.setValue("overrideInterfaceFontSize", arg1);
    Utils::Gui::updateInterfaceFontSize();
}

void SettingsDialog::on_webSocketServerServicePortResetButton_clicked() {
    ui->webSocketServerServicePortSpinBox->setValue(WebSocketServerService::getDefaultPort());
}

void SettingsDialog::on_enableSocketServerCheckBox_toggled() {
    bool checked = ui->enableSocketServerCheckBox->isChecked();
    ui->browserExtensionFrame->setEnabled(checked);
}

void SettingsDialog::on_internalIconThemeCheckBox_toggled(bool checked) {
    if (checked) {
        const QSignalBlocker blocker(ui->systemIconThemeCheckBox);
        ui->systemIconThemeCheckBox->setChecked(false);
    }

    ui->systemIconThemeCheckBox->setDisabled(checked);
}

void SettingsDialog::on_systemIconThemeCheckBox_toggled(bool checked) {
    if (checked) {
        const QSignalBlocker blocker(ui->internalIconThemeCheckBox);
        ui->internalIconThemeCheckBox->setChecked(false);
    }

    ui->internalIconThemeCheckBox->setDisabled(checked);
    ui->darkModeIconThemeCheckBox->setDisabled(checked);
}

void SettingsDialog::on_webSocketTokenButton_clicked() {
    auto webSocketTokenDialog = new WebSocketTokenDialog();
    webSocketTokenDialog->exec();
    delete(webSocketTokenDialog);
}
