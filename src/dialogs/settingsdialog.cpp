#include "dialogs/settingsdialog.h"

#include <QtNetwork/qnetworkproxy.h>
#include <entities/cloudconnection.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <entities/script.h>
#include <helpers/toolbarcontainer.h>
#include <libraries/qkeysequencewidget/qkeysequencewidget/src/qkeysequencewidget.h>
#include <services/cryptoservice.h>
#include <services/mcpservice.h>
#include <services/metricsservice.h>
#include <services/scriptingservice.h>
#include <services/webappclientservice.h>
#include <services/websocketserverservice.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include <widgets/scriptsettingwidget.h>

#include <QAction>
#include <QButtonGroup>
#include <QClipboard>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFontDialog>
#include <QInputDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeySequence>
#include <QKeySequenceEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPointer>
#include <QRadioButton>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QScrollArea>
#include <QSplitter>
#include <QStatusBar>
#include <QStyleFactory>
#include <QTextBrowser>
#include <QTimer>
#include <QToolBar>
#include <QUrlQuery>
#include <utility>

#include "build_number.h"
#include "dialogs/websockettokendialog.h"
#include "entities/calendaritem.h"
#include "filedialog.h"
#include "helpers/clientproxy.h"
#include "mainwindow.h"
#include "release.h"
#include "scriptrepositorydialog.h"
#include "services/databaseservice.h"
#include "services/openaiservice.h"
#include "services/owncloudservice.h"
#include "services/settingsservice.h"
#include "services/updateservice.h"
#include "ui_settingsdialog.h"
#include "version.h"
#include "widgets/fontcolorwidget.h"
#include "widgets/settings/aisettingswidget.h"
#include "widgets/settings/debugoptionsettingswidget.h"
#include "widgets/settings/debugsettingswidget.h"
#include "widgets/settings/editorfontcolorsettingswidget.h"
#include "widgets/settings/editorsettingswidget.h"
#include "widgets/settings/languagetoolsettingswidget.h"
#include "widgets/settings/networksettingswidget.h"
#include "widgets/settings/todosettingswidget.h"
#include "widgets/settings/webapplicationsettingswidget.h"
#include "widgets/settings/webcompanionsettingswidget.h"

SettingsDialog::SettingsDialog(int page, QWidget *parent)
    : MasterDialog(parent), ui(new Ui::SettingsDialog) {
    ui->setupUi(this);
    afterSetupUI();

    SettingsService settings;
    _initialDarkMode = settings.value(QStringLiteral("darkMode")).toBool();
    _initialDarkModeColors = settings.value(QStringLiteral("darkModeColors")).toBool();
    _initialDarkModeTrayIcon = settings.value(QStringLiteral("darkModeTrayIcon")).toBool();
    _initialDarkModeIconTheme = Utils::Misc::isDarkModeIconTheme();
    _initialSchemaKey = settings.value(QStringLiteral("Editor/CurrentSchemaKey")).toString();

    bool fromWelcomeDialog = parent->objectName() == QLatin1String("WelcomeDialog");

    MainWindow *mainWindow = MainWindow::instance();

    // if there was no size set yet, and we already have a main window we'll
    // mimic that size
    if (mainWindow != nullptr) {
        resize(mainWindow->width(), mainWindow->height());
    } else {
        // we must not use resize(1, 1) because XFCE really resizes the window
        // to 1x1
        resize(800, 600);
    }

    QList<QWidget *> pageWidgets;

    // get a list of every settings page in the correct order
    for (int index = 0; index < ui->settingsStackedWidget->count(); index++) {
        pageWidgets.append(ui->settingsStackedWidget->widget(index));
    }

    Q_FOREACH (QWidget *pageWidget, pageWidgets) {
        // make sure the margin of every page is 0
        QLayout *layout = pageWidget->layout();
        layout->setContentsMargins(0, 0, 0, 0);

        // inject a scroll area to make each page scrollable
        auto *scrollArea = new QScrollArea(ui->settingsStackedWidget);
        scrollArea->setWidget(pageWidget);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        ui->settingsStackedWidget->addWidget(scrollArea);
    }

    ui->loginFlowCancelButton->hide();
    ui->connectionTestLabel->hide();
    ui->connectButton->setDefault(true);
    ui->noteSaveIntervalTime->setToolTip(ui->noteSaveIntervalTimeLabel->toolTip());
    ui->removeCustomNoteFileExtensionButton->setDisabled(true);

    ui->languageToolSettingsWidget->initialize();

    updateSearchLineEditIcons();

    ui->gitSettingsWidget->initialize();

#ifndef Q_OS_WIN32
    ui->automaticNoteFolderDatabaseClosingCheckBox->hide();
#endif

    readSettings();

    // initializes the main splitter
    initMainSplitter();

    // set the current page
    // must be done in the end so that the settings are loaded first when
    // doing a connection test
    setCurrentPage(page);

    // disable the shortcut page if there is no main window yet
    if (mainWindow == nullptr) {
        QTreeWidgetItem *item = findSettingsTreeWidgetItemByPage(ShortcutPage);
        if (item != nullptr) {
            item->setDisabled(true);
        }
    }

    // expand all items in the settings tree widget
    ui->settingsTreeWidget->expandAll();

    // replace the "ownCloud" text by "ownCloud / NextCloud"
    replaceOwnCloudText();

    // declare that we need to restart the application if certain settings
    // are changed
    connect(ui->languageListWidget, SIGNAL(itemSelectionChanged()), this, SLOT(needRestart()));
    connect(ui->hideIconsInMenusCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));
    connect(ui->allowOnlyOneAppInstanceCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));
    connect(ui->showSystemTrayCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));
    connect(ui->startHiddenCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));
    connect(ui->panelsSettingsWidget, &PanelsSettingsWidget::needRestart, this,
            &SettingsDialog::needRestart);
    connect(ui->webApplicationSettingsWidget, &WebApplicationSettingsWidget::needRestart, this,
            &SettingsDialog::needRestart);
    connect(ui->aiAutocompleteCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));

    // Connect the needRestart signal from the editor font color settings widget
    connect(ui->editorFontColorSettingsWidget, &EditorFontColorSettingsWidget::needRestart, this,
            &SettingsDialog::needRestart);

    // Connect debug settings widget signals
    connect(ui->debugSettingsWidget, &DebugSettingsWidget::aboutToOutputSettings,
            ui->ownCloudSettingsWidget, &OwnCloudSettingsWidget::storeOwncloudDebugData);

    // Forward cloud connection updates to note folder and todo widgets
    connect(ui->ownCloudSettingsWidget, &OwnCloudSettingsWidget::cloudConnectionsUpdated, this,
            &SettingsDialog::onCloudConnectionsUpdated);
    connect(ui->debugSettingsWidget, &DebugSettingsWidget::issueAssistantRequested, this, [this]() {
        MainWindow *mainWindow = MainWindow::instance();
        if (mainWindow == nullptr) {
            return;
        }
        storeSettings();
        mainWindow->openIssueAssistantDialog();
        close();
    });

    // Connect note folder settings widget signals
    connect(ui->noteFolderSettingsWidget, &NoteFolderSettingsWidget::storeSettingsRequested, this,
            &SettingsDialog::storeSettings);

    // Connect todo settings widget signals
    connect(ui->todoSettingsWidget, &TodoSettingsWidget::storeSettingsRequested, this,
            &SettingsDialog::storeSettings);
    connect(ui->todoSettingsWidget, &TodoSettingsWidget::reloadCalendarListRequested, this,
            [this]() {
                OwnCloudService *ownCloud = OwnCloudService::instance(true);
                ownCloud->settingsGetCalendarList(this);
            });

    connect(ui->aiSettingsWidget, &AiSettingsWidget::searchScriptRepositoryRequested, this,
            [this]() { ui->scriptingSettingsWidget->searchScriptInRepository(); });

    //    connect(ui->layoutPresetWidget, SIGNAL(layoutStored(QString)),
    //            this, SLOT(needRestart()));
    connect(ui->layoutPresetWidget, &LayoutPresetWidget::layoutStored, this,
            &SettingsDialog::onLayoutStored);

    if (fromWelcomeDialog) {
        // hide the whole left side frame with the settings menu tree
        ui->leftSideFrame->setVisible(false);
    }
}

/**
 * Replaces the "ownCloud" text by "ownCloud / NextCloud"
 */
void SettingsDialog::replaceOwnCloudText() const {
    // Delegate ownCloud UI element text replacement to the widget
    ui->ownCloudSettingsWidget->replaceOwnCloudText();

    // Todo widget handles its own replaceOwnCloudText
    ui->todoSettingsWidget->replaceOwnCloudText();

    QTreeWidgetItem *item = ui->settingsTreeWidget->topLevelItem(OwnCloudPage);
    item->setText(0, Utils::Misc::replaceOwnCloudText(item->text(0)));

    // Note folder settings
    ui->noteFolderSettingsWidget->replaceOwnCloudText();
}

/**
 * Check the _noteNotificationNoneCheckBox when the checkboxes should all be
 * unchecked
 *
 * @param button
 */
void SettingsDialog::noteNotificationButtonGroupPressed(QAbstractButton *button) {
    if (button->isChecked()) {
        QTimer::singleShot(100, this, SLOT(noteNotificationNoneCheckBoxCheck()));
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

SettingsDialog::~SettingsDialog() { delete ui; }

/**
 * Initializes the portable mode page
 */
void SettingsDialog::initPortableModePage() {
    bool isInPortableMode = Utils::Misc::isInPortableMode();
    QString status = isInPortableMode ? tr("enabled") : tr("disabled");

    QString text =
        "<p>" + tr("Portable mode is currently:") + " <strong>" + status + "</strong></p>";

    text += tr("In portable mode") + ":<ul><li>" +
            tr("the internal sqlite database and the settings will be stored "
               "inside a <code>Data</code> folder at the binary's "
               "location") +
            "</li><li>" + tr("the settings will be stored in an ini file") + "</li><li>" +
            tr("the note folders, script paths and path to an external editor "
               "will be automatically stored relative to the "
               "<code>Data</code> folder so that the correct note "
               "folders, scripts and external editor will be loaded "
               "regardless where your QOwnNotes installation is "
               "currently located") +
            "</li></ul>";

    if (!isInPortableMode) {
        text += "<p>" +
                tr("It will be activated if you run QOwnNotes with "
                   "the parameter <code>--portable</code>.") +
                "</p>";

#ifdef Q_OS_WIN32
        text += "<p>" +
                tr("You will find a <code>QOwnNotesPortable.bat</code> "
                   "in your release path to start QOwnNotes in "
                   "portable mode.") +
                "</p>";
#endif
    }

    // inject some generic CSS styles
    ui->portableModeInfoTextBrowser->document()->setDefaultStyleSheet(Utils::Misc::genericCSS());

    ui->portableModeInfoTextBrowser->setHtml(text);
}

void SettingsDialog::storeSettings() {
    SettingsService settings;
    ui->ownCloudSettingsWidget->storeSettings();
    ui->todoSettingsWidget->storeSettings();
    settings.setValue(QStringLiteral("disableAutomaticUpdateDialog"),
                      ui->disableAutomaticUpdateDialogCheckBox->isChecked());
    settings.setValue(QStringLiteral("notifyAllExternalModifications"),
                      ui->notifyAllExternalModificationsCheckBox->isChecked());
    settings.setValue(QStringLiteral("ignoreAllExternalModifications"),
                      ui->ignoreAllExternalModificationsCheckBox->isChecked());
    settings.setValue(QStringLiteral("acceptAllExternalModifications"),
                      ui->acceptAllExternalModificationsCheckBox->isChecked());
    settings.setValue(QStringLiteral("ignoreAllExternalNoteFolderChanges"),
                      ui->ignoreAllExternalNoteFolderChangesCheckBox->isChecked());
    settings.setValue(QStringLiteral("enableNoteChecksumChecks"),
                      ui->enableNoteChecksumChecks->isChecked());
    settings.setValue(QStringLiteral("newNoteAskHeadline"),
                      ui->newNoteAskHeadlineCheckBox->isChecked());
    settings.setValue(QStringLiteral("useUNIXNewline"), ui->useUNIXNewlineCheckBox->isChecked());
    settings.setValue(QStringLiteral("restoreCursorPosition"),
                      ui->restoreCursorPositionCheckBox->isChecked());
    settings.setValue(QStringLiteral("restoreLastNoteAtStartup"),
                      ui->restoreLastNoteAtStartupCheckBox->isChecked());
    settings.setValue(QStringLiteral("noteSaveIntervalTime"), ui->noteSaveIntervalTime->value());
    settings.setValue(QStringLiteral("defaultNoteFileExtension"),
                      ui->defaultNoteFileExtensionListWidget->currentItem()->text());
    ui->localTrashSettingsWidget->storeSettings();
    ui->webCompanionSettingsWidget->storeSettings();
    ui->webApplicationSettingsWidget->storeSettings();

    // make the path relative to the portable data path if we are in
    // portable mode
    settings.setValue(QStringLiteral("externalEditorPath"),
                      Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                          ui->externalEditorPathLineEdit->text()));

    settings.setValue(QStringLiteral("overrideInterfaceFontSize"),
                      ui->overrideInterfaceFontSizeGroupBox->isChecked());
    settings.setValue(QStringLiteral("interfaceFontSize"), ui->interfaceFontSizeSpinBox->value());
    settings.setValue(QStringLiteral("overrideInterfaceScalingFactor"),
                      ui->overrideInterfaceScalingFactorGroupBox->isChecked());
    settings.setValue(QStringLiteral("interfaceScalingFactor"),
                      ui->interfaceScalingFactorSpinBox->value());
    settings.setValue(QStringLiteral("itemHeight"), ui->itemHeightSpinBox->value());
    settings.setValue(QStringLiteral("MainWindow/mainToolBar.iconSize"),
                      ui->toolbarIconSizeSpinBox->value());
    settings.setValue(QStringLiteral("allowOnlyOneAppInstance"),
                      ui->allowOnlyOneAppInstanceCheckBox->isChecked());
    settings.setValue(QStringLiteral("interfaceLanguage"),
                      getSelectedListWidgetValue(ui->languageListWidget));
    ui->previewFontSettingsWidget->storeSettings();
    ui->debugOptionSettingsWidget->storeSettings();
    ui->editorSettingsWidget->storeSettings();
    ui->languageToolSettingsWidget->storeSettings();

    ui->networkSettingsWidget->storeSettings();

    settings.setValue(QStringLiteral("hideIconsInMenus"),
                      ui->hideIconsInMenusCheckBox->isChecked());

    settings.setValue(QStringLiteral("showStatusBarNotePath"),
                      ui->showStatusBarNotePathCheckBox->isChecked());

    settings.setValue(QStringLiteral("showStatusBarRelativeNotePath"),
                      ui->showStatusBarRelativeNotePathCheckBox->isChecked());

    settings.setValue(QStringLiteral("DistractionFreeMode/hideStatusBar"),
                      ui->hideStatusBarInDistractionFreeModeCheckBox->isChecked());
    settings.setValue(QStringLiteral("DistractionFreeMode/openInFullScreen"),
                      ui->openDistractionFreeModeInFullScreenCheckBox->isChecked());

    // store the custom note file extensions
    QStringList noteFileExtensionList;
    for (int i = 0; i < ui->defaultNoteFileExtensionListWidget->count(); i++) {
        QListWidgetItem *item = ui->defaultNoteFileExtensionListWidget->item(i);
        noteFileExtensionList.append(item->text());
    }
    noteFileExtensionList.removeDuplicates();
    settings.setValue(QStringLiteral("noteFileExtensionList"), noteFileExtensionList);

    // Preview font settings are stored in previewFontSettingsWidget->storeSettings()
    ui->editorFontColorSettingsWidget->storeSettings();

    // store the shortcut settings
    storeShortcutSettings();

    // store the splitter settings
    storeSplitterSettings();

    // apply and store the toolbar configuration
    on_applyToolbarButton_clicked();

    // Store the enabled state of the scripts
    ui->scriptingSettingsWidget->storeSettings();

    // store image scaling settings
    settings.setValue(QStringLiteral("imageScaleDown"), ui->imageScaleDownCheckBox->isChecked());
    settings.setValue(QStringLiteral("imageScaleDownMaximumHeight"),
                      ui->maximumImageHeightSpinBox->value());
    settings.setValue(QStringLiteral("imageScaleDownMaximumWidth"),
                      ui->maximumImageWidthSpinBox->value());

    // store git settings
    ui->gitSettingsWidget->storeSettings();

    // Panels settings are stored in panelsSettingsWidget->storeSettings()
    ui->panelsSettingsWidget->storeSettings();

    // store the interface style settings
    if (ui->interfaceStyleComboBox->currentIndex() > 0) {
        settings.setValue(QStringLiteral("interfaceStyle"),
                          ui->interfaceStyleComboBox->currentText());
    } else {
        settings.remove(QStringLiteral("interfaceStyle"));
    }

    // store the cursor width - now handled by editorSettingsWidget->storeSettings()

    settings.setValue(QStringLiteral("SearchEngineId"),
                      ui->searchEngineSelectionComboBox->currentData().toInt());

    settings.setValue(QStringLiteral("ShowSystemTray"), ui->showSystemTrayCheckBox->isChecked());
    settings.setValue(QStringLiteral("StartHidden"), ui->startHiddenCheckBox->isChecked());
    settings.setValue(QStringLiteral("automaticNoteFolderDatabaseClosing"),
                      ui->automaticNoteFolderDatabaseClosingCheckBox->isChecked());
    settings.setValue(QStringLiteral("legacyLinking"), ui->legacyLinkingCheckBox->isChecked());
    settings.setValue(QStringLiteral("enableReadOnlyMode"),
                      ui->enableReadOnlyModeCheckBox->isChecked());
    settings.setValue(QStringLiteral("startInReadOnlyMode"),
                      ui->startInReadOnlyModeCheckBox->isChecked());
    settings.setValue(QStringLiteral("autoReadOnlyMode"),
                      ui->autoReadOnlyModeCheckBox->isChecked());
    settings.setValue(QStringLiteral("autoReadOnlyModeTimeout"),
                      ui->autoReadOnlyModeTimeoutSpinBox->value());

    // Web companion settings are stored in webCompanionSettingsWidget->storeSettings()

    // MCP server settings are stored in mcpServerSettingsWidget->storeSettings()
    ui->mcpServerSettingsWidget->storeSettings();

    // Web application settings are stored in webApplicationSettingsWidget->storeSettings()

    // AI settings are stored in aiSettingsWidget->storeSettings()
    ui->aiSettingsWidget->storeSettings();

    settings.setValue(QStringLiteral("ai/autocompleteEnabled"),
                      ui->aiAutocompleteCheckBox->isChecked());
}

void SettingsDialog::readSettings() {
    SettingsService settings;

    initSearchEngineComboBox();

    // Set current note folder list item via the widget
    ui->noteFolderSettingsWidget->readSettings();

    ui->ownCloudSettingsWidget->readSettings();
    ui->todoSettingsWidget->readSettings();

    // prepend the portable data path if we are in portable mode
    ui->externalEditorPathLineEdit->setText(Utils::Misc::prependPortableDataPathIfNeeded(
        settings.value(QStringLiteral("externalEditorPath")).toString(), true));

    ui->disableAutomaticUpdateDialogCheckBox->setChecked(
        settings.value(QStringLiteral("disableAutomaticUpdateDialog")).toBool());
    ui->notifyAllExternalModificationsCheckBox->setChecked(
        settings.value(QStringLiteral("notifyAllExternalModifications")).toBool());
    ui->ignoreAllExternalModificationsCheckBox->setChecked(
        settings.value(QStringLiteral("ignoreAllExternalModifications")).toBool());
    ui->acceptAllExternalModificationsCheckBox->setChecked(
        settings.value(QStringLiteral("acceptAllExternalModifications")).toBool());
    ui->ignoreAllExternalNoteFolderChangesCheckBox->setChecked(
        settings.value(QStringLiteral("ignoreAllExternalNoteFolderChanges")).toBool());
    ui->enableNoteChecksumChecks->setChecked(
        settings.value(QStringLiteral("enableNoteChecksumChecks"), false).toBool());
    ui->newNoteAskHeadlineCheckBox->setChecked(
        settings.value(QStringLiteral("newNoteAskHeadline")).toBool());
    ui->useUNIXNewlineCheckBox->setChecked(
        settings.value(QStringLiteral("useUNIXNewline")).toBool());
    ui->localTrashSettingsWidget->readSettings();
    ui->webCompanionSettingsWidget->readSettings();
    ui->webApplicationSettingsWidget->readSettings();

#ifdef Q_OS_MAC
    bool restoreCursorPositionDefault = false;
#else
    bool restoreCursorPositionDefault = true;
#endif

    ui->restoreCursorPositionCheckBox->setChecked(
        settings.value(QStringLiteral("restoreCursorPosition"), restoreCursorPositionDefault)
            .toBool());
    ui->restoreLastNoteAtStartupCheckBox->setChecked(
        settings.value(QStringLiteral("restoreLastNoteAtStartup"), true).toBool());
    ui->noteSaveIntervalTime->setValue(
        settings.value(QStringLiteral("noteSaveIntervalTime"), 10).toInt());
    ui->previewFontSettingsWidget->readSettings();
    ui->debugOptionSettingsWidget->readSettings();
    ui->editorSettingsWidget->readSettings();
    ui->languageToolSettingsWidget->readSettings();
    ui->allowOnlyOneAppInstanceCheckBox->setChecked(
        settings.value(QStringLiteral("allowOnlyOneAppInstance")).toBool());
    ui->toolbarIconSizeSpinBox->setValue(
        settings.value(QStringLiteral("MainWindow/mainToolBar.iconSize")).toInt());

    const QSignalBlocker overrideInterfaceFontSizeGroupBoxBlocker(
        ui->overrideInterfaceFontSizeGroupBox);
    Q_UNUSED(overrideInterfaceFontSizeGroupBoxBlocker)
    const QSignalBlocker interfaceFontSizeSpinBoxBlocker(ui->interfaceFontSizeSpinBox);
    Q_UNUSED(interfaceFontSizeSpinBoxBlocker)
    ui->overrideInterfaceFontSizeGroupBox->setChecked(
        settings.value(QStringLiteral("overrideInterfaceFontSize"), false).toBool());
    ui->interfaceFontSizeSpinBox->setValue(
        settings.value(QStringLiteral("interfaceFontSize"), 11).toInt());

    const QSignalBlocker overrideInterfaceScalingFactorGroupBoxBlocker(
        ui->overrideInterfaceScalingFactorGroupBox);
    Q_UNUSED(overrideInterfaceScalingFactorGroupBoxBlocker)
    const QSignalBlocker interfaceScalingFactorSpinBoxBlocker(ui->interfaceScalingFactorSpinBox);
    Q_UNUSED(interfaceScalingFactorSpinBoxBlocker)
    ui->overrideInterfaceScalingFactorGroupBox->setChecked(
        settings.value(QStringLiteral("overrideInterfaceScalingFactor"), false).toBool());
    ui->interfaceScalingFactorSpinBox->setValue(
        settings.value(QStringLiteral("interfaceScalingFactor"), 100).toInt());

    QTreeWidget treeWidget(this);
    auto *treeWidgetItem = new QTreeWidgetItem();
    treeWidget.addTopLevelItem(treeWidgetItem);
    int height = treeWidget.visualItemRect(treeWidgetItem).height();

    ui->itemHeightSpinBox->setValue(settings.value(QStringLiteral("itemHeight"), height).toInt());

    selectListWidgetValue(ui->languageListWidget,
                          settings.value(QStringLiteral("interfaceLanguage")).toString());

    ui->networkSettingsWidget->readSettings();

    ui->hideIconsInMenusCheckBox->setChecked(Utils::Misc::areMenuIconsHidden());

    ui->showStatusBarNotePathCheckBox->setChecked(
        settings.value(QStringLiteral("showStatusBarNotePath"), true).toBool());

    ui->showStatusBarRelativeNotePathCheckBox->setChecked(
        settings.value(QStringLiteral("showStatusBarRelativeNotePath")).toBool());
    ui->showStatusBarRelativeNotePathCheckBox->setEnabled(
        ui->showStatusBarNotePathCheckBox->isChecked());

    ui->hideStatusBarInDistractionFreeModeCheckBox->setChecked(
        settings.value(QStringLiteral("DistractionFreeMode/hideStatusBar")).toBool());
    ui->openDistractionFreeModeInFullScreenCheckBox->setChecked(
        settings.value(QStringLiteral("DistractionFreeMode/openInFullScreen"), true).toBool());

    ui->editorFontColorSettingsWidget->readSettings();

    // loads the custom note file extensions
    QListIterator<QString> itr(Note::noteFileExtensionList());
    while (itr.hasNext()) {
        QString fileExtension = itr.next();
        addCustomNoteFileExtension(fileExtension);
    }

    auto noteFileExtensionItems = ui->defaultNoteFileExtensionListWidget->findItems(
        Note::defaultNoteFileExtension(), Qt::MatchExactly);

    if (noteFileExtensionItems.count() > 0) {
        ui->defaultNoteFileExtensionListWidget->setCurrentItem(noteFileExtensionItems.at(0));
    }

    // load the shortcut settings
    loadShortcutSettings();

    // load image scaling settings
    bool scaleImageDown = settings.value(QStringLiteral("imageScaleDown"), false).toBool();
    ui->maximumImageHeightSpinBox->setValue(
        settings.value(QStringLiteral("imageScaleDownMaximumHeight"), 1024).toInt());
    ui->maximumImageWidthSpinBox->setValue(
        settings.value(QStringLiteral("imageScaleDownMaximumWidth"), 1024).toInt());
    ui->imageScaleDownCheckBox->setChecked(scaleImageDown);
    ui->imageScalingFrame->setVisible(scaleImageDown);

    // load git settings
    ui->gitSettingsWidget->readSettings();

    // Panels settings are read in panelsSettingsWidget->readSettings()
    ui->panelsSettingsWidget->readSettings();

    // load the settings for the interface style combo box
    loadInterfaceStyleComboBox();

    ui->ownCloudSettingsWidget->initCloudConnectionComboBox();

    const QSignalBlocker blocker8(this->ui->showSystemTrayCheckBox);
    Q_UNUSED(blocker8)
    bool showSystemTray = settings.value(QStringLiteral("ShowSystemTray")).toBool();
    ui->showSystemTrayCheckBox->setChecked(showSystemTray);
    ui->startHiddenCheckBox->setEnabled(showSystemTray);
    ui->startHiddenCheckBox->setChecked(settings.value(QStringLiteral("StartHidden")).toBool());
    if (!showSystemTray) {
        ui->startHiddenCheckBox->setChecked(false);
    }

    ui->automaticNoteFolderDatabaseClosingCheckBox->setChecked(
        Utils::Misc::doAutomaticNoteFolderDatabaseClosing());
    ui->legacyLinkingCheckBox->setChecked(settings.value(QStringLiteral("legacyLinking")).toBool());

    const bool enableReadOnlyMode =
        settings.value(QStringLiteral("enableReadOnlyMode"), true).toBool();
    ui->enableReadOnlyModeCheckBox->setChecked(enableReadOnlyMode);
    ui->readOnlyModeSettingsFrame->setEnabled(enableReadOnlyMode);

    ui->startInReadOnlyModeCheckBox->setChecked(
        settings.value(QStringLiteral("startInReadOnlyMode")).toBool());
    ui->autoReadOnlyModeCheckBox->setChecked(
        settings.value(QStringLiteral("autoReadOnlyMode")).toBool());
    ui->autoReadOnlyModeTimeoutSpinBox->setValue(
        settings.value(QStringLiteral("autoReadOnlyModeTimeout"), 30).toInt());

    // Web companion settings are read in webCompanionSettingsWidget->readSettings()

    // MCP server settings are read in mcpServerSettingsWidget->readSettings()
    ui->mcpServerSettingsWidget->readSettings();

    // Web application settings are read in webApplicationSettingsWidget->readSettings()

    // AI settings are read in aiSettingsWidget->readSettings()
    ui->aiSettingsWidget->readSettings();

    ui->aiAutocompleteCheckBox->setChecked(
        settings.value(QStringLiteral("ai/autocompleteEnabled"), false).toBool());
}

/**
 * Does the setup for the search engine combo-box
 */
void SettingsDialog::initSearchEngineComboBox() const {
    SettingsService settings;

    // Iterates over the search engines and adds them
    // to the combobox
    QHash<int, Utils::Misc::SearchEngine> searchEngines = Utils::Misc::getSearchEnginesHashMap();

    ui->searchEngineSelectionComboBox->clear();

    Q_FOREACH (int id, Utils::Misc::getSearchEnginesIds()) {
        Utils::Misc::SearchEngine searchEngine = searchEngines[id];
        ui->searchEngineSelectionComboBox->addItem(searchEngine.name, QString::number(id));
    }

    // Sets the current selected item to the search engine
    // selected previously
    // while also handling the case in which the saved key has
    // been removed from the hash table
    int savedEngineId =
        settings.value(QStringLiteral("SearchEngineId"), Utils::Misc::getDefaultSearchEngineId())
            .toInt();
    int savedEngineIndex =
        ui->searchEngineSelectionComboBox->findData(QVariant(savedEngineId).toString());
    savedEngineIndex = (savedEngineIndex == -1) ? 0 : savedEngineIndex;
    ui->searchEngineSelectionComboBox->setCurrentIndex(savedEngineIndex);
}

/**
 * Loads the settings for the interface style combo box
 */
void SettingsDialog::loadInterfaceStyleComboBox() const {
    const QSignalBlocker blocker(ui->interfaceStyleComboBox);
    Q_UNUSED(blocker)

    ui->interfaceStyleComboBox->clear();
    ui->interfaceStyleComboBox->addItem(tr("Automatic (needs restart)"));

    Q_FOREACH (QString style, QStyleFactory::keys()) {
        ui->interfaceStyleComboBox->addItem(style);
    }

    SettingsService settings;
    QString interfaceStyle = settings.value(QStringLiteral("interfaceStyle")).toString();

    if (!interfaceStyle.isEmpty()) {
        ui->interfaceStyleComboBox->setCurrentText(interfaceStyle);
    } else {
        ui->interfaceStyleComboBox->setCurrentIndex(0);
    }

    Utils::Gui::applyInterfaceStyle();
}

/**
 * Loads the shortcut settings
 */
void SettingsDialog::loadShortcutSettings() {
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow == nullptr) {
        return;
    }

    SettingsService settings;
    bool darkMode = settings.value(QStringLiteral("darkMode")).toBool();

    QPalette palette;
    QColor shortcutButtonActiveColor = darkMode ? Qt::white : palette.color(QPalette::ButtonText);
    QColor shortcutButtonInactiveColor = darkMode ? Qt::darkGray : palette.color(QPalette::Mid);

    const QList<QMenu *> menus = mainWindow->menuList();
    ui->shortcutSearchLineEdit->clear();
    ui->shortcutTreeWidget->clear();
    ui->shortcutTreeWidget->setColumnCount(3);

    // shortcuts on toolbars and note folders don't work yet
    auto disabledMenuNames = QStringList()
                             << QStringLiteral("menuToolbars") << QStringLiteral("noteFoldersMenu");

    const QIcon disableShortcutButtonIcon =
        QIcon::fromTheme(QStringLiteral("dialog-cancel"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/dialog-cancel.svg")));
    const QIcon clearButtonIcon =
        QIcon::fromTheme(QStringLiteral("edit-clear"), QIcon(":/icons/breeze-qownnotes/16x16/"
                                                             "edit-clear.svg"));

    // loop through all menus
    for (const QMenu *menu : menus) {
        if (disabledMenuNames.contains(menu->objectName())) {
            continue;
        }

        auto *menuItem = new QTreeWidgetItem();
        int actionCount = 0;

        // loop through all actions of the menu
        foreach (QAction *action, menu->actions()) {
            const QString &actionObjectName = action->objectName();

            // we don't need empty objects
            if (actionObjectName.isEmpty()) {
                continue;
            }

            // create the tree widget item
            auto *actionItem = new QTreeWidgetItem();
            actionItem->setText(0, action->text().remove(QStringLiteral("&")));
            actionItem->setToolTip(0, actionObjectName);
            actionItem->setData(1, Qt::UserRole, actionObjectName);
            menuItem->addChild(actionItem);

            // create the key widget for the local shortcut
            auto *keyWidget = new QKeySequenceWidget();
            keyWidget->setFixedWidth(240);
            keyWidget->setClearButtonIcon(clearButtonIcon);
            keyWidget->setNoneText(tr("Undefined shortcut"));
            keyWidget->setShortcutButtonActiveColor(shortcutButtonActiveColor);
            keyWidget->setShortcutButtonInactiveColor(shortcutButtonInactiveColor);
            keyWidget->setToolTip(tr("Assign a new shortcut"), tr("Reset to default shortcut"));
            keyWidget->setDefaultKeySequence(action->data().toString());

            const QString &shortcutSettingKey =
                QStringLiteral("Shortcuts/MainWindow-") + action->objectName();
            const bool settingFound = settings.contains(shortcutSettingKey);

            // try to load the key sequence from the settings, because
            // action->shortcut() is empty if menubar was disabled!
            keyWidget->setKeySequence(settingFound ? settings.value(shortcutSettingKey).toString()
                                                   : action->data().toString());

            connect(keyWidget, &QKeySequenceWidget::keySequenceAccepted, this,
                    [this, actionObjectName]() { keySequenceEvent(actionObjectName); });

            auto *disableShortcutButton = new QPushButton();
            disableShortcutButton->setToolTip(tr("Clear shortcut"));
            disableShortcutButton->setIcon(disableShortcutButtonIcon);

            connect(disableShortcutButton, &QPushButton::pressed, this,
                    [keyWidget]() { keyWidget->setKeySequence(QKeySequence("")); });

            // create a frame for the key widget for the local shortcut and
            // the shortcut disabling button
            auto *frame = new QFrame();
            auto *frameLayout = new QHBoxLayout();
            frameLayout->setContentsMargins({});
            frameLayout->setSpacing(2);
            frameLayout->addWidget(keyWidget);
            frameLayout->addWidget(disableShortcutButton);
            frame->setLayout(frameLayout);
            ui->shortcutTreeWidget->setItemWidget(actionItem, 1, frame);

            // create the key widget for the global shortcut
            auto *globalShortcutKeyWidget = new QKeySequenceWidget();
            globalShortcutKeyWidget->setFixedWidth(240);
            globalShortcutKeyWidget->setClearButtonIcon(clearButtonIcon);
            globalShortcutKeyWidget->setNoneText(tr("Undefined shortcut"));
            globalShortcutKeyWidget->setShortcutButtonActiveColor(shortcutButtonActiveColor);
            globalShortcutKeyWidget->setShortcutButtonInactiveColor(shortcutButtonInactiveColor);
            globalShortcutKeyWidget->setToolTip(tr("Assign a new shortcut"),
                                                tr("Reset to default shortcut"));
            globalShortcutKeyWidget->setKeySequence(
                settings.value(QStringLiteral("GlobalShortcuts/MainWindow-") + actionObjectName)
                    .toString());

            ui->shortcutTreeWidget->setItemWidget(actionItem, 2, globalShortcutKeyWidget);

            actionCount++;
        }

        if (actionCount > 0) {
            menuItem->setText(0, menu->title().remove(QStringLiteral("&")));
            menuItem->setToolTip(0, menu->objectName());
            ui->shortcutTreeWidget->addTopLevelItem(menuItem);
            menuItem->setExpanded(true);
        }
    }
}

/**
 * Show an information if a shortcut was already used elsewhere
 *
 * @param objectName
 */
void SettingsDialog::keySequenceEvent(const QString &objectName) {
    QKeySequenceWidget *keySequenceWidget = findKeySequenceWidget(objectName);

    if (keySequenceWidget == nullptr) {
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

            const auto keySequenceWidgets = ui->shortcutTreeWidget->itemWidget(shortcutItem, 1)
                                                ->findChildren<QKeySequenceWidget *>();

            if (keySequenceWidgets.count() == 0) {
                continue;
            }

            auto *keyWidget = keySequenceWidgets.at(0);
            QKeySequence keySequence = keyWidget->keySequence();
            QKeySequence defaultKeySequence = keyWidget->defaultKeySequence();

            // show an information if the shortcut was already used elsewhere
            if (keySequence == eventKeySequence) {
                if (Utils::Gui::information(
                        this, tr("Shortcut already assigned"),
                        tr("The shortcut <strong>%1</strong> is already "
                           "assigned to <strong>%2</strong>! Do you want to "
                           "jump to the shortcut?")
                            .arg(eventKeySequence.toString(), shortcutItem->text(0)),
                        QStringLiteral("settings-shortcut-already-assigned"),
                        QMessageBox::Yes | QMessageBox::Cancel,
                        QMessageBox::Yes) == QMessageBox::Yes) {
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
QKeySequenceWidget *SettingsDialog::findKeySequenceWidget(const QString &objectName) {
    // loop all top level tree widget items (menus)
    for (int i = 0; i < ui->shortcutTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *menuItem = ui->shortcutTreeWidget->topLevelItem(i);

        // loop all tree widget items of the menu (action shortcuts)
        for (int j = 0; j < menuItem->childCount(); j++) {
            QTreeWidgetItem *shortcutItem = menuItem->child(j);
            QString name = shortcutItem->data(1, Qt::UserRole).toString();

            if (name == objectName) {
                const auto keySequenceWidgets = ui->shortcutTreeWidget->itemWidget(shortcutItem, 1)
                                                    ->findChildren<QKeySequenceWidget *>();

                if (keySequenceWidgets.count() > 0) {
                    return keySequenceWidgets.at(0);
                }
            }
        }
    }

    return nullptr;
}

/**
 * Stores the local and global keyboard shortcut settings
 */
void SettingsDialog::storeShortcutSettings() {
    SettingsService settings;

    // loop all top level tree widget items (menus)
    for (int i = 0; i < ui->shortcutTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *menuItem = ui->shortcutTreeWidget->topLevelItem(i);

        // loop all tree widget items of the menu (action shortcuts)
        for (int j = 0; j < menuItem->childCount(); j++) {
            QTreeWidgetItem *shortcutItem = menuItem->child(j);
            const auto keySequenceWidgets = ui->shortcutTreeWidget->itemWidget(shortcutItem, 1)
                                                ->findChildren<QKeySequenceWidget *>();

            if (keySequenceWidgets.count() == 0) {
                continue;
            }

            auto *keyWidget = keySequenceWidgets.at(0);
            auto *globalShortcutKeyWidget = dynamic_cast<QKeySequenceWidget *>(
                ui->shortcutTreeWidget->itemWidget(shortcutItem, 2));

            if (keyWidget == nullptr || globalShortcutKeyWidget == nullptr) {
                continue;
            }

            const QString actionObjectName = shortcutItem->data(1, Qt::UserRole).toString();

            // handle local shortcut
            QKeySequence keySequence = keyWidget->keySequence();
            QKeySequence defaultKeySequence = keyWidget->defaultKeySequence();
            QString settingsKey = "Shortcuts/MainWindow-" + actionObjectName;

            // remove or store the setting for the shortcut if it's not default
            if (keySequence == defaultKeySequence) {
                settings.remove(settingsKey);
            } else {
                // set new key sequence (can also be empty if no key sequence
                // should be used)
                settings.setValue(settingsKey, keySequence);
            }

            // handle global shortcut
            keySequence = globalShortcutKeyWidget->keySequence();
            settingsKey = "GlobalShortcuts/MainWindow-" + actionObjectName;

            // remove or store the setting for the shortcut if it's not empty
            if (keySequence.isEmpty()) {
                settings.remove(settingsKey);
            } else {
                settings.setValue(settingsKey, keySequence);
            }
        }
    }
}

/**
 * Selects a value in a list widget, that is hidden in the whatsThis parameter
 */
void SettingsDialog::selectListWidgetValue(QListWidget *listWidget, const QString &value) {
    // get all items from the list widget
    QList<QListWidgetItem *> items =
        listWidget->findItems(QStringLiteral("*"), Qt::MatchWrap | Qt::MatchWildcard);
    // select the right item in the selector
    Q_FOREACH (QListWidgetItem *item, items) {
        if (item->whatsThis() == value) {
            const QSignalBlocker blocker(listWidget);
            Q_UNUSED(blocker)

            item->setSelected(true);
            break;
        }
    }
}

/**
 * Checks if a value, that is hidden in the whatsThis parameter exists in a
 * list widget
 */
bool SettingsDialog::listWidgetValueExists(QListWidget *listWidget, const QString &value) {
    // get all items from the list widget
    QList<QListWidgetItem *> items =
        listWidget->findItems(QStringLiteral("*"), Qt::MatchWrap | Qt::MatchWildcard);
    // checks if the value exists
    Q_FOREACH (QListWidgetItem *item, items) {
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
QString SettingsDialog::getSelectedListWidgetValue(QListWidget *listWidget) {
    QList<QListWidgetItem *> items = listWidget->selectedItems();

    if (items.count() >= 1) {
        return items.first()->whatsThis();
    }

    return QString();
}

/**
 * Callback function from OwnCloudService to output a success or error message.
 * Forwards to the ownCloud settings widget.
 */
void SettingsDialog::connectTestCallback(bool appIsValid, QString appVersion, QString serverVersion,
                                         QString notesPathExistsText,
                                         QString connectionErrorMessage) {
    ui->ownCloudSettingsWidget->handleConnectTestCallback(
        appIsValid, appVersion, serverVersion, notesPathExistsText, connectionErrorMessage);
}

/**
 * Sets text and color of an ok-label. Delegates to the ownCloud settings widget.
 */
void SettingsDialog::setOKLabelData(int number, const QString &text, OKLabelStatus status) {
    ui->ownCloudSettingsWidget->setOKLabelData(
        number, text, static_cast<OwnCloudSettingsWidget::OKLabelStatus>(status));
}

void SettingsDialog::refreshTodoCalendarList(const QList<CalDAVCalendarData> &items,
                                             bool forceReadCheckedState) {
    ui->todoSettingsWidget->refreshTodoCalendarList(items, ui->ownCloudSettingsWidget->serverUrl(),
                                                    forceReadCheckedState);
}

/* * * * * * * * * * * * * * * *
 *
 * Slot implementations
 *
 * * * * * * * * * * * * * * * */

void SettingsDialog::on_buttonBox_clicked(QAbstractButton *button) {
    if (button == ui->buttonBox->button(QDialogButtonBox::Ok)) {
        const bool darkModeSettingChanged = hasDarkModeSettingChanges();
        storeSettings();

        if (darkModeSettingChanged) {
            applyDarkModeSettings();
        }
    }
}

void SettingsDialog::onLayoutStored(const QString &layoutUuid) {
    auto *mainWindow = MainWindow::instance();
    if ((mainWindow == nullptr) || layoutUuid.isEmpty()) {
        return;
    }

    // Switch to the new layout after creating it from the preset.
    mainWindow->setCurrentLayout(layoutUuid);
}

void SettingsDialog::on_reinitializeDatabaseButton_clicked() {
    if (QMessageBox::question(this, tr("Database"),
                              tr("Do you really want to clear the local database? "
                                 "This will also remove your configured note "
                                 "folders and your cached todo items!"),
                              QMessageBox::Yes | QMessageBox::Cancel,
                              QMessageBox::Cancel) == QMessageBox::Yes) {
        DatabaseService::reinitializeDiskDatabase();
        NoteFolder::migrateToNoteFolders();

        Utils::Gui::information(
            this, tr("Database"),
            tr("The Database was reinitialized. Please restart the application now!"),
            QStringLiteral("database-reinitialized"));
    }
}

/**
 * Allows the user to clear all settings and the database and exit the app
 */
void SettingsDialog::on_clearAppDataAndExitButton_clicked() {
    if (QMessageBox::question(this, tr("Clear app data and exit"),
                              tr("Do you really want to clear all settings, remove the "
                                 "database and exit QOwnNotes?\n\n"
                                 "Your notes will stay intact!"),
                              QMessageBox::Yes | QMessageBox::Cancel,
                              QMessageBox::Cancel) == QMessageBox::Yes) {
        SettingsService settings;
        settings.clear();
        DatabaseService::removeDiskDatabase();

        // remove the log file
        DebugOptionSettingsWidget::removeLogFile();

        // make sure no settings get written after are quitting
        qApp->setProperty("clearAppDataAndExit", true);
        qApp->quit();
    }
}

/**
 * Removes the log file
 */
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
    mimeTypeFilters << QStringLiteral("application/x-executable")
                    << QStringLiteral("application/octet-stream");

    FileDialog dialog(QStringLiteral("ExternalEditor"));

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

        const QString &filePath(fileNames.at(0));
        ui->externalEditorPathLineEdit->setText(filePath);
    }
}

/**
 * Does the note folder page setup
 */
/**
 * Delegates the remote path list callback to the NoteFolderSettingsWidget.
 * Called by OwnCloudService::loadDirectory().
 */
void SettingsDialog::setNoteFolderRemotePathList(QStringList pathList) {
    ui->noteFolderSettingsWidget->setNoteFolderRemotePathList(pathList);
}

/**
 * Adds a custom file extension
 */
void SettingsDialog::on_addCustomNoteFileExtensionButton_clicked() {
    bool ok;
    QString fileExtension;
    fileExtension = QInputDialog::getText(this, tr("File extension"),
                                          tr("Please enter a new note file extension:"),
                                          QLineEdit::Normal, fileExtension, &ok);

    if (!ok) {
        return;
    }

    // make sure the file extension doesn't start with a point
    fileExtension = Utils::Misc::removeIfStartsWith(std::move(fileExtension), QStringLiteral("."));

    QListWidgetItem *item = addCustomNoteFileExtension(fileExtension);

    if (item != nullptr) {
        ui->defaultNoteFileExtensionListWidget->setCurrentItem(item);
    }
}

/**
 * Adds a custom note file extension
 */
QListWidgetItem *SettingsDialog::addCustomNoteFileExtension(QString fileExtension) {
    fileExtension = fileExtension.trimmed();

    if (ui->defaultNoteFileExtensionListWidget->findItems(fileExtension, Qt::MatchExactly).count() >
        0) {
        return nullptr;
    }

    auto *item = new QListWidgetItem(fileExtension);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    if (fileExtension == "md") {
        item->setToolTip(tr("Markdown file"));
    } else if (fileExtension == "txt") {
        item->setToolTip(tr("Plain text file"));
    }

    ui->defaultNoteFileExtensionListWidget->addItem(item);

    return item;
}

/**
 * Removes a custom file extension
 */
void SettingsDialog::on_removeCustomNoteFileExtensionButton_clicked() {
    if (ui->defaultNoteFileExtensionListWidget->count() <= 1) {
        return;
    }

    auto *item = ui->defaultNoteFileExtensionListWidget->currentItem();

    if (Utils::Gui::question(this, tr("Remove note file extension"),
                             tr("Do you really want to remove the note file extension "
                                "<strong>%1</strong>? You will not see files with this "
                                "extension in the note list any more!")
                                 .arg(item->text()),
                             QStringLiteral("remove-note-file-extension")) != QMessageBox::Yes) {
        return;
    }

    delete item;

    ui->removeCustomNoteFileExtensionButton->setEnabled(
        ui->defaultNoteFileExtensionListWidget->count() > 1);
}

/**
 * Updates a custom file extension
 */
void SettingsDialog::on_defaultNoteFileExtensionListWidget_itemChanged(QListWidgetItem *item) {
    // make sure the file extension doesn't start with a point
    QString fileExtension =
        Utils::Misc::removeIfStartsWith(item->text(), QStringLiteral(".")).trimmed();

    if (fileExtension != item->text()) {
        item->setText(fileExtension);
    }
}

/**
 * No-op: dark mode checkbox was moved to the Color modes page
 */
void SettingsDialog::handleDarkModeCheckBoxToggled(bool updateCheckBoxes, bool updateSchema) {
    Q_UNUSED(updateCheckBoxes)
    Q_UNUSED(updateSchema)
}

void SettingsDialog::updateSearchLineEditIcons() {
    const QString searchIconFileName =
        SettingsService().value(QStringLiteral("darkModeColors")).toBool()
            ? QStringLiteral("search-notes-dark.svg")
            : QStringLiteral("search-notes.svg");
    static const QRegularExpression searchIconRegex(
        QStringLiteral("background-image: url\\(:.+\\);"));
    const QString searchIconStyle =
        QStringLiteral("background-image: url(:/images/%1);").arg(searchIconFileName);

    QString styleSheet = ui->searchLineEdit->styleSheet();
    styleSheet.replace(searchIconRegex, searchIconStyle);
    ui->searchLineEdit->setStyleSheet(styleSheet);

    styleSheet = ui->languageSearchLineEdit->styleSheet();
    styleSheet.replace(searchIconRegex, searchIconStyle);
    ui->languageSearchLineEdit->setStyleSheet(styleSheet);

    styleSheet = ui->shortcutSearchLineEdit->styleSheet();
    styleSheet.replace(searchIconRegex, searchIconStyle);
    ui->shortcutSearchLineEdit->setStyleSheet(styleSheet);

    auto *scriptSearchLineEdit =
        ui->scriptingSettingsWidget->findChild<QLineEdit *>(QStringLiteral("scriptSearchLineEdit"));
    if (scriptSearchLineEdit) {
        styleSheet = scriptSearchLineEdit->styleSheet();
        styleSheet.replace(searchIconRegex, searchIconStyle);
        scriptSearchLineEdit->setStyleSheet(styleSheet);
    }
}

/**
 * Checks if dark mode settings changed since the dialog was opened
 */
bool SettingsDialog::hasDarkModeSettingChanges() const {
    const SettingsService settings;
    return settings.value(QStringLiteral("darkMode")).toBool() != _initialDarkMode ||
           settings.value(QStringLiteral("darkModeColors")).toBool() != _initialDarkModeColors ||
           settings.value(QStringLiteral("darkModeTrayIcon")).toBool() !=
               _initialDarkModeTrayIcon ||
           Utils::Misc::isDarkModeIconTheme() != _initialDarkModeIconTheme;
}

/**
 * Applies the current color mode's dark mode settings to the application
 */
void SettingsDialog::applyDarkModeSettings() {
    updateSearchLineEditIcons();
    Utils::Gui::fixDarkModeIcons(this);
    Utils::Gui::applyDarkModeSettings();
}

/**
 * Searches in the description and in the shortcut for a entered text
 *
 * @param arg1
 */
void SettingsDialog::on_shortcutSearchLineEdit_textChanged(const QString &arg1) {
    // get all items
    QList<QTreeWidgetItem *> allItems =
        ui->shortcutTreeWidget->findItems(QString(), Qt::MatchContains | Qt::MatchRecursive);

    // search text if at least one character was entered
    if (arg1.size() >= 1) {
        // search for items in the description
        QList<QTreeWidgetItem *> foundItems =
            ui->shortcutTreeWidget->findItems(arg1, Qt::MatchContains | Qt::MatchRecursive);

        // hide all not found items
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            bool foundKeySequence = false;

            auto *keyWidget =
                dynamic_cast<QKeySequenceWidget *>(ui->shortcutTreeWidget->itemWidget(item, 1));

            // search in the local shortcut text
            if (keyWidget != nullptr) {
                QKeySequence keySequence = keyWidget->keySequence();
                foundKeySequence = keySequence.toString().contains(arg1, Qt::CaseInsensitive);
            }

            // search in the global shortcut text
            if (!foundKeySequence) {
                keyWidget =
                    dynamic_cast<QKeySequenceWidget *>(ui->shortcutTreeWidget->itemWidget(item, 2));

                if (keyWidget != nullptr) {
                    QKeySequence keySequence = keyWidget->keySequence();
                    foundKeySequence = keySequence.toString().contains(arg1, Qt::CaseInsensitive);
                }
            }

            item->setHidden(!foundItems.contains(item) && !foundKeySequence);
        }

        // show items again that have visible children so that they are
        // really shown
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            if (Utils::Gui::isOneTreeWidgetItemChildVisible(item)) {
                item->setHidden(false);
                item->setExpanded(true);
            }
        }
    } else {
        // show all items otherwise
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            item->setHidden(false);
        }
    }
}

void SettingsDialog::on_settingsTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                              QTreeWidgetItem *previous) {
    Q_UNUSED(previous)
    const int currentIndex = current->whatsThis(0).toInt();

    ui->settingsStackedWidget->setCurrentIndex(currentIndex);

    switch (currentIndex) {
        case SettingsPages::LayoutPresetsPage:
            ui->layoutPresetWidget->resizeLayoutPresetImage();
            break;
        case SettingsPages::ShortcutPage:
            ui->shortcutTreeWidget->resizeColumnToContents(0);
            ui->shortcutTreeWidget->resizeColumnToContents(1);
            ui->shortcutTreeWidget->resizeColumnToContents(2);
            break;
    }
}

bool SettingsDialog::initializePage(int index) {
    if (_pageInitialized[index]) {
        return false;
    }

    switch (index) {
        case SettingsPages::NoteFolderPage: {
            // set up the note folder tab
            ui->noteFolderSettingsWidget->initialize();
        } break;
        case SettingsPages::NetworkPage: {
            ui->networkSettingsWidget->initialize();
        } break;
        case SettingsPages::EditorPage: {
            ui->editorSettingsWidget->initialize();
        } break;
        case SettingsPages::WebApplicationPage: {
            ui->webApplicationSettingsWidget->initialize();
        } break;
        case SettingsPages::WebCompanionPage: {
            ui->webCompanionSettingsWidget->initialize();
        } break;
        case SettingsPages::PanelsPage: {
            ui->panelsSettingsWidget->initialize();
        } break;
        case SettingsPages::ToolbarPage: {
            // init the toolbar editor
            ui->toolbarEditor->setTargetWindow(MainWindow::instance());
            ui->toolbarEditor->setCustomToolbarRemovalOnly(true);

            QStringList disabledToolbarNames(QStringList()
                                             << QStringLiteral("windowToolbar")
                                             << QStringLiteral("customActionsToolbar"));
            ui->toolbarEditor->setDisabledToolbarNames(disabledToolbarNames);

            QStringList disabledMenuNames(QStringList() << QStringLiteral("noteFoldersMenu"));
            ui->toolbarEditor->setDisabledMenuNames(disabledMenuNames);

            //    QStringList disabledMenuActionNames(QStringList() << "");
            //    ui->toolbarEditor->setDisabledMenuActionNames(disabledMenuActionNames);

            ui->toolbarEditor->updateBars();
        } break;
        case SettingsPages::PortableModePage: {
            // initialize the portable mode page
            initPortableModePage();
        } break;
        case SettingsPages::InterfacePage: {
            ui->helpTranslateLabel->setText(ui->helpTranslateLabel->text().arg(
                "https://www.qownnotes.org/contributing/translation.html"));
#ifdef Q_OS_MAC
            // there is no system tray in OS X
            ui->systemTrayGroupBox->setTitle(tr("Menu bar"));
            ui->showSystemTrayCheckBox->setText(tr("Show menu bar item"));
#endif
        } break;
        case SettingsPages::GeneralPage: {
            _noteNotificationButtonGroup = new QButtonGroup(this);
            _noteNotificationButtonGroup->addButton(ui->notifyAllExternalModificationsCheckBox);
            _noteNotificationButtonGroup->addButton(ui->ignoreAllExternalModificationsCheckBox);
            _noteNotificationButtonGroup->addButton(ui->acceptAllExternalModificationsCheckBox);

            // create a hidden checkbox so we can un-check above checkboxes
            _noteNotificationNoneCheckBox = new QCheckBox(this);
            _noteNotificationNoneCheckBox->setHidden(true);
            _noteNotificationButtonGroup->addButton(_noteNotificationNoneCheckBox);
            connect(_noteNotificationButtonGroup, SIGNAL(buttonPressed(QAbstractButton *)), this,
                    SLOT(noteNotificationButtonGroupPressed(QAbstractButton *)));

            // set up the search engine combo-box
            initSearchEngineComboBox();

#ifdef Q_OS_MAC
            // we don't need app instance settings on OS X
            ui->appInstanceGroupBox->setVisible(false);
            ui->allowOnlyOneAppInstanceCheckBox->setChecked(false);

            // Qt::TargetMoveAction seems to be broken on macOS, the item vanishes after
            // dropping Qt::CopyAction seens to be the only action that works
            ui->noteFolderSettingsWidget
                ->findChild<QListWidget *>(QStringLiteral("noteFolderListWidget"))
                ->setDefaultDropAction(Qt::CopyAction);
            ui->scriptingSettingsWidget
                ->findChild<QListWidget *>(QStringLiteral("scriptListWidget"))
                ->setDefaultDropAction(Qt::CopyAction);
#endif
        } break;
        case SettingsPages::DebugPage: {
            ui->debugSettingsWidget->initialize();

            // Show the log file path
            ui->debugOptionSettingsWidget->initialize();
        } break;
        case SettingsPages::ScriptingPage: {
            ui->scriptingSettingsWidget->initialize();
        } break;
        case SettingsPages::OwnCloudPage: {
            resetOKLabelData();

            // Store original HTML texts for enable/disable state management
            _installInfoTextLabel1Html = ui->installInfoTextLabel1->text();
            _installInfoTextLabel2Html = ui->installInfoTextLabel2->text();
            _installInfoTextLabel3Html = ui->installInfoTextLabel3->text();

            // add the QOwnNotesAPI minimum version number to the info text
            _installInfoTextLabel1Html.replace(QLatin1String("QOWNNOTESAPI_MIN_VERSION"),
                                               QOWNNOTESAPI_MIN_VERSION);
            ui->installInfoTextLabel1->setText(_installInfoTextLabel1Html);

            if (connectionTestCanBeStarted()) {
                // start a connection test
                startConnectionTest();
            }

            // handle cloud connection storing
            connect(ui->cloudServerConnectionNameLineEdit, SIGNAL(textChanged(QString)), this,
                    SLOT(storeSelectedCloudConnection()));
            connect(ui->serverUrlEdit, SIGNAL(textChanged(QString)), this,
                    SLOT(storeSelectedCloudConnection()));
            connect(ui->userNameEdit, SIGNAL(textChanged(QString)), this,
                    SLOT(storeSelectedCloudConnection()));
            connect(ui->passwordEdit, SIGNAL(textChanged(QString)), this,
                    SLOT(storeSelectedCloudConnection()));
            connect(ui->appQOwnNotesAPICheckBox, SIGNAL(toggled(bool)), this,
                    SLOT(storeSelectedCloudConnection()));
        } break;
        case SettingsPages::AiPage: {
            ui->aiSettingsWidget->initialize();
        } break;
        case SettingsPages::ColorModesPage: {
            ui->colorModeSettingsWidget->initialize();
        } break;
        case SettingsPages::McpServerPage: {
            ui->mcpServerSettingsWidget->initialize();
        } break;
        default:
            break;
    }

    _pageInitialized[index] = true;

    return true;
}

void SettingsDialog::on_settingsStackedWidget_currentChanged(int index) {
    QTreeWidgetItem *item = findSettingsTreeWidgetItemByPage(index);
    if (item != nullptr) {
        const QSignalBlocker blocker(ui->settingsTreeWidget);
        Q_UNUSED(blocker)

        ui->settingsTreeWidget->setCurrentItem(item);
        ui->headlineLabel->setText("<h3>" + item->text(0) + "</h3>");
    }

    // Initialize the page if not already done
    this->initializePage(index);

    if (index == DebugPage) {
        ui->debugSettingsWidget->outputSettings();
    } else if (index == OwnCloudPage) {
        resetOKLabelData();
    } else if (index == AiPage) {
        ui->aiSettingsWidget->buildAiScriptingTreeWidget();
    }

    // turn off the tasks page if no ownCloud settings are available
    //    QTreeWidgetItem *todoItem =
    //    findSettingsTreeWidgetItemByPage(TodoPage); if (todoItem != nullptr)
    //    {
    //        if (OwnCloudService::hasOwnCloudSettings()) {
    //            todoItem->setDisabled(false);
    //            todoItem->setToolTip(0, "");
    //        } else {
    //            todoItem->setDisabled(true);
    //            todoItem->setToolTip(0, tr("Please make sure the connection to
    //            "
    //                                               "your ownCloud server
    //                                               works."));
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
    QList<QTreeWidgetItem *> allItems = ui->settingsTreeWidget->findItems(
        QLatin1String(""), Qt::MatchContains | Qt::MatchRecursive);

    // hide all not found items
    Q_FOREACH (QTreeWidgetItem *item, allItems) {
        int id = item->whatsThis(0).toInt();

        if (id == page) {
            return item;
        }
    }

    return nullptr;
}

/**
 * Does the initialization for the main splitter
 */
void SettingsDialog::initMainSplitter() {
    _mainSplitter = new QSplitter(this);
    _mainSplitter->setOrientation(Qt::Horizontal);
    ui->leftSideFrame->setContentsMargins(0, 0, 5, 0);

    _mainSplitter->addWidget(ui->leftSideFrame);
    _mainSplitter->addWidget(ui->settingsFrame);

    ui->mainFrame->layout()->addWidget(_mainSplitter);

    // restore tag frame splitter state
    SettingsService settings;
    QByteArray state =
        settings.value(QStringLiteral("SettingsDialog/mainSplitterState")).toByteArray();
    _mainSplitter->restoreState(state);
}

void SettingsDialog::closeEvent(QCloseEvent *event) {
    Q_UNUSED(event)

    ui->ownCloudSettingsWidget->cancelConnectionTest();

    // make sure no settings get written after we got the
    // clearAppDataAndExit call
    if (qApp->property("clearAppDataAndExit").toBool()) {
        return;
    }

    if (result() != QDialog::Accepted) {
        SettingsService settings;
        settings.setValue(QStringLiteral("darkMode"), _initialDarkMode);
        settings.setValue(QStringLiteral("darkModeColors"), _initialDarkModeColors);
        settings.setValue(QStringLiteral("darkModeTrayIcon"), _initialDarkModeTrayIcon);
        settings.setValue(QStringLiteral("darkModeIconTheme"), _initialDarkModeIconTheme);
        settings.setValue(QStringLiteral("Editor/CurrentSchemaKey"), _initialSchemaKey);

        // Restore the initial color mode
        const QString initialColorModeId = ui->colorModeSettingsWidget->initialColorModeId();
        if (!initialColorModeId.isEmpty()) {
            settings.setValue(QStringLiteral("ColorModes/currentId"), initialColorModeId);
        }

        Utils::Gui::applyDarkModeSettings();
    }

    // store the splitter settings
    storeSplitterSettings();
}

/**
 * Stores the splitter settings
 */
void SettingsDialog::storeSplitterSettings() {
    SettingsService settings;
    settings.setValue(QStringLiteral("SettingsDialog/mainSplitterState"),
                      _mainSplitter->saveState());
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

void SettingsDialog::on_applyToolbarButton_clicked() {
    ui->toolbarEditor->apply();

    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return;
    }

    // get all available toolbar names from the toolbar editor
    QStringList toolbarObjectNames = ui->toolbarEditor->toolbarObjectNames();

    QList<ToolbarContainer> toolbarContainers;
    foreach (QToolBar *toolbar, mainWindow->findChildren<QToolBar *>()) {
        QString name = toolbar->objectName();

        // Don't store the custom actions toolbar and toolbars that are
        // not in the toolbar edit widget anymore (for some reason they
        // are still found by findChildren)
        if (name == QLatin1String("customActionsToolbar") || !toolbarObjectNames.contains(name)) {
            continue;
        }

        toolbarContainers.append(toolbar);

        // update the icon size
        ToolbarContainer::updateIconSize(toolbar);
    }

    SettingsService settings;

    // remove the current toolbars
    //    settings.beginGroup("toolbar");
    //    settings.remove("");
    //    settings.endGroup();

    int size = static_cast<int>(toolbarContainers.size());

    // For some reason there are no ToolbarContainer sometimes, we don't want to overwrite the
    // toolbar settings then
    if (size > 0) {
        settings.beginWriteArray(QStringLiteral("toolbar"), size);

        for (int i = 0; i < size; i++) {
            settings.setArrayIndex(i);
            settings.setValue(QStringLiteral("name"), toolbarContainers[i].name);
            settings.setValue(QStringLiteral("title"), toolbarContainers[i].title);
            settings.setValue(QStringLiteral("items"), toolbarContainers[i].actions);
        }

        settings.endArray();
    }
}

void SettingsDialog::on_resetToolbarPushButton_clicked() {
    if (QMessageBox::question(this, tr("Reset toolbars and exit"),
                              tr("Do you really want to reset all toolbars? "
                                 "The application will be closed in the process, the "
                                 "default toolbars will be restored when you start it "
                                 "again."),
                              QMessageBox::Yes | QMessageBox::Cancel,
                              QMessageBox::Cancel) == QMessageBox::Yes) {
        SettingsService settings;

        // remove all settings in the group
        settings.beginGroup(QStringLiteral("toolbar"));
        settings.remove(QLatin1String(""));
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

void SettingsDialog::on_enableReadOnlyModeCheckBox_toggled(bool checked) {
    ui->readOnlyModeSettingsFrame->setEnabled(checked);
}

/**
 * Searches for text in the whole settings dialog and filters the settings
 * tree widget
 *
 * @param arg1
 */
void SettingsDialog::on_searchLineEdit_textChanged(const QString &arg1) {
    QList<QTreeWidgetItem *> allItems =
        ui->settingsTreeWidget->findItems(QString(), Qt::MatchContains | Qt::MatchRecursive);

    // Clear highlights from the previous search
    clearSearchHighlights();

    // search text if at least one character was entered
    if (arg1.size() >= 1) {
        QList<int> pageIndexList;

        // search in the tree widget items themselves
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            if (item->text(0).contains(arg1, Qt::CaseInsensitive)) {
                int pageIndex = item->whatsThis(0).toInt();

                if (!pageIndexList.contains(pageIndex)) {
                    pageIndexList << pageIndex;
                }
            }
        }

        // search in all labels
        Q_FOREACH (QLabel *widget, findChildren<QLabel *>()) {
            if (widget->text().contains(arg1, Qt::CaseInsensitive)) {
                addToSearchIndexList(widget, pageIndexList, arg1);
            }
        }

        // search in all push buttons
        Q_FOREACH (QPushButton *widget, findChildren<QPushButton *>()) {
            if (widget->text().contains(arg1, Qt::CaseInsensitive)) {
                addToSearchIndexList(widget, pageIndexList, arg1);
            }
        }

        // search in all checkboxes
        Q_FOREACH (QCheckBox *widget, findChildren<QCheckBox *>()) {
            if (widget->text().contains(arg1, Qt::CaseInsensitive)) {
                addToSearchIndexList(widget, pageIndexList, arg1);
            }
        }

        // search in all radio buttons
        Q_FOREACH (QRadioButton *widget, findChildren<QRadioButton *>()) {
            if (widget->text().contains(arg1, Qt::CaseInsensitive)) {
                addToSearchIndexList(widget, pageIndexList, arg1);
            }
        }

        // search in all group boxes
        Q_FOREACH (QGroupBox *widget, findChildren<QGroupBox *>()) {
            if (widget->title().contains(arg1, Qt::CaseInsensitive)) {
                addToSearchIndexList(widget, pageIndexList, arg1);
            }
        }

        // show and hide items according to if index was found in pageIndexList
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            // get stored index of list widget item
            int pageIndex = item->whatsThis(0).toInt();
            item->setHidden(!pageIndexList.contains(pageIndex));
        }

        // show items again that have visible children so that they are
        // really shown
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            if (Utils::Gui::isOneTreeWidgetItemChildVisible(item)) {
                item->setHidden(false);
                item->setExpanded(true);
            }
        }
    } else {
        // show all items otherwise
        Q_FOREACH (QTreeWidgetItem *item, allItems) {
            item->setHidden(false);
        }
    }
}

/**
 * Adds the page index of a widget to the pageIndexList if not already added,
 * and highlights the matched text in the widget
 *
 * @param widget
 * @param pageIndexList
 * @param searchText
 */
void SettingsDialog::addToSearchIndexList(QWidget *widget, QList<int> &pageIndexList,
                                          const QString &searchText) {
    // get the page id of the widget
    int pageIndex = findSettingsPageIndexOfWidget(widget);

    // add page id if not already added
    if (!pageIndexList.contains(pageIndex)) {
        pageIndexList << pageIndex;
    }

    // highlight the matched widget if a search text is provided
    if (!searchText.isEmpty()) {
        highlightSearchMatchedWidget(widget, searchText);
    }
}

/**
 * Highlights the matched search text inside a widget by wrapping matched
 * substrings in a yellow background span (for widgets supporting rich text)
 * or by applying a stylesheet background color (for others)
 *
 * @param widget
 * @param searchText
 */
void SettingsDialog::highlightSearchMatchedWidget(QWidget *widget, const QString &searchText) {
    // Avoid double-highlighting the same widget
    if (_searchMatchedWidgets.contains(widget)) {
        return;
    }

    _searchMatchedWidgets.append(widget);

    // Choose a highlight color that works for both light and dark mode:
    // a soft warm amber for light mode, a muted dark-gold for dark mode
    const bool isDark = palette().color(QPalette::Window).lightness() < 128;
    const QString highlightColor =
        isDark ? QStringLiteral("#7a6200")     // Muted dark-gold for dark mode
               : QStringLiteral("#fdefb8");    // Soft light-amber for light mode

    // Wraps all case-insensitive occurrences of searchText in a plain text string
    // with a highlight span, escaping the plain text for HTML first
    auto wrapPlainTextMatches = [&highlightColor](const QString &plainText,
                                                  const QString &searchText) -> QString {
        const QString escaped = plainText.toHtmlEscaped();
        const QString escapedSearch = searchText.toHtmlEscaped();
        QString result;
        int pos = 0;
        while (pos < escaped.length()) {
            int idx = escaped.indexOf(escapedSearch, pos, Qt::CaseInsensitive);
            if (idx == -1) {
                result += escaped.mid(pos);
                break;
            }
            result += escaped.mid(pos, idx - pos);
            result += QLatin1String("<span style=\"background-color: ") + highlightColor +
                      QLatin1String(";\">") + escaped.mid(idx, escapedSearch.length()) +
                      QLatin1String("</span>");
            pos = idx + escapedSearch.length();
        }
        return result;
    };

    const QString bgStyle =
        QLatin1String(" background-color: ") + highlightColor + QLatin1String(";");

    if (auto *label = qobject_cast<QLabel *>(widget)) {
        const QString text = label->text();
        // Only highlight labels that do not already contain rich text to avoid
        // corrupting any embedded HTML
        if (!Qt::mightBeRichText(text)) {
            _searchMatchedWidgetOriginalTexts[widget] = text;
            label->setText(wrapPlainTextMatches(text, searchText));
        }
    } else if (auto *checkBox = qobject_cast<QCheckBox *>(widget)) {
        // QCheckBox does not render rich text; use a stylesheet background instead
        _searchMatchedWidgetOriginalTexts[widget] = checkBox->styleSheet();
        checkBox->setStyleSheet(checkBox->styleSheet() + QLatin1String(" QCheckBox {") + bgStyle +
                                QLatin1String(" }"));
    } else if (auto *radioButton = qobject_cast<QRadioButton *>(widget)) {
        // QRadioButton does not render rich text; use a stylesheet background instead
        _searchMatchedWidgetOriginalTexts[widget] = radioButton->styleSheet();
        radioButton->setStyleSheet(radioButton->styleSheet() + QLatin1String(" QRadioButton {") +
                                   bgStyle + QLatin1String(" }"));
    } else if (auto *groupBox = qobject_cast<QGroupBox *>(widget)) {
        // QGroupBox titles do not support rich text; use a stylesheet background instead
        _searchMatchedWidgetOriginalTexts[widget] = groupBox->styleSheet();
        groupBox->setStyleSheet(groupBox->styleSheet() + QLatin1String(" QGroupBox {") + bgStyle +
                                QLatin1String(" }"));
    } else if (auto *pushButton = qobject_cast<QPushButton *>(widget)) {
        // QPushButton text does not support rich text; use a stylesheet background instead
        _searchMatchedWidgetOriginalTexts[widget] = pushButton->styleSheet();
        pushButton->setStyleSheet(pushButton->styleSheet() + QLatin1String(" QPushButton {") +
                                  bgStyle + QLatin1String(" }"));
    }
}

/**
 * Clears all search highlighting applied to matched widgets and restores their
 * original text or stylesheet
 */
void SettingsDialog::clearSearchHighlights() {
    Q_FOREACH (QWidget *widget, _searchMatchedWidgets) {
        if (!_searchMatchedWidgetOriginalTexts.contains(widget)) {
            continue;
        }
        const QString &original = _searchMatchedWidgetOriginalTexts[widget];

        if (auto *label = qobject_cast<QLabel *>(widget)) {
            label->setText(original);
        } else if (auto *checkBox = qobject_cast<QCheckBox *>(widget)) {
            checkBox->setStyleSheet(original);
        } else if (auto *radioButton = qobject_cast<QRadioButton *>(widget)) {
            radioButton->setStyleSheet(original);
        } else if (auto *groupBox = qobject_cast<QGroupBox *>(widget)) {
            groupBox->setStyleSheet(original);
        } else if (auto *pushButton = qobject_cast<QPushButton *>(widget)) {
            pushButton->setStyleSheet(original);
        }
    }

    _searchMatchedWidgets.clear();
    _searchMatchedWidgetOriginalTexts.clear();
}

/**
 * Finds the settings page index of a widget
 *
 * @param widget
 * @return
 */
int SettingsDialog::findSettingsPageIndexOfWidget(QWidget *widget) {
    QWidget *parent = qobject_cast<QWidget *>(widget->parent());

    if (parent == nullptr) {
        return -1;
    }

    // check if the parent is our settings stacked widget
    if (parent->objectName() == QLatin1String("settingsStackedWidget")) {
        // get the index of the object in the settings stacked widget
        return ui->settingsStackedWidget->indexOf(widget);
    }

    // search for the page id in the parent
    return findSettingsPageIndexOfWidget(parent);
}

/**
 * Declares that we need a restart
 */
void SettingsDialog::needRestart() { Utils::Misc::needRestart(); }

void SettingsDialog::on_interfaceStyleComboBox_currentTextChanged(const QString &arg1) {
    Utils::Gui::applyInterfaceStyle(arg1);

    // if the interface style was set to automatic we need a restart
    if (ui->interfaceStyleComboBox->currentIndex() == 0) {
        needRestart();
    }
}

/**
 * Reset the cursor width spin box value - now handled by EditorSettingsWidget
 */

/**
 * Also enable the single instance feature if the system tray icon is turned on
 */
void SettingsDialog::on_showSystemTrayCheckBox_toggled(bool checked) {
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
    if (QMessageBox::question(this, tr("Reset message boxes"),
                              tr("Do you really want to reset the overrides of all message "
                                 "boxes?")) == QMessageBox::Yes) {
        SettingsService settings;

        // remove all settings in the group
        settings.beginGroup(QStringLiteral("MessageBoxOverride"));
        settings.remove(QLatin1String(""));
        settings.endGroup();
    }
}

/**
 * Export settings
 */
void SettingsDialog::on_exportSettingsButton_clicked() {
    FileDialog dialog(QStringLiteral("SettingsExport"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("INI files") + " (*.ini)");
    dialog.setWindowTitle(tr("Export settings"));
    dialog.selectFile(QStringLiteral("QOwnNotes-settings.ini"));
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

            exportSettings.setValue(QStringLiteral("SettingsExport/platform"),
                                    QStringLiteral(PLATFORM));

            SettingsService settings;

            const QStringList keys = settings.allKeys();
            Q_FOREACH (QString key, keys) {
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
                   "\n\n" + tr("The application will be restarted after the import.") +
                   Utils::Misc::appendSingleAppInstanceTextIfNeeded();

    if (QMessageBox::question(this, title, text, QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No) == QMessageBox::No) {
        return;
    }

    FileDialog dialog(QStringLiteral("SettingsExport"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setNameFilter(tr("INI files") + " (*.ini)");
    dialog.setWindowTitle(tr("Import settings"));
    int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return;
    }

    QString fileName = dialog.selectedFile();
    SettingsService settings;
    QSettings importSettings(fileName, QSettings::IniFormat);
    settings.clear();
    DatabaseService::removeDiskDatabase();

    const QStringList keys = importSettings.allKeys();

    Q_FOREACH (QString key, keys) {
        QVariant value = importSettings.value(key);
        settings.setValue(key, value);
    }

    // make sure no settings get written after quitting
    qApp->setProperty("clearAppDataAndExit", true);

    Utils::Misc::restartApplication();
}

void SettingsDialog::on_interfaceFontSizeSpinBox_valueChanged(int arg1) {
    SettingsService settings;
    settings.setValue(QStringLiteral("interfaceFontSize"), arg1);
    Utils::Gui::updateInterfaceFontSize(arg1);
}

void SettingsDialog::on_overrideInterfaceFontSizeGroupBox_toggled(bool arg1) {
    SettingsService settings;
    settings.setValue(QStringLiteral("overrideInterfaceFontSize"), arg1);
    Utils::Gui::updateInterfaceFontSize();
}

void SettingsDialog::on_languageSearchLineEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInListWidget(ui->languageListWidget, arg1, true);
}

void SettingsDialog::on_databaseIntegrityCheckButton_clicked() {
    if (DatabaseService::checkDiskDatabaseIntegrity()) {
        Utils::Gui::information(this, tr("Database"),
                                tr("The integrity of the disk database is valid."),
                                QStringLiteral("database-integrity-check-valid"));
    } else {
        Utils::Gui::warning(this, tr("Database"),
                            tr("The integrity of the disk database is not valid!"),
                            QStringLiteral("database-integrity-check-not-valid"));
    }
}

/**
 * Disables the remove custom file extension button is only one item is left
 *
 * Needs an additional check when an item is deleted, because that seems to
 * happen after the selection is changed!
 */
void SettingsDialog::on_defaultNoteFileExtensionListWidget_itemSelectionChanged() {
    ui->removeCustomNoteFileExtensionButton->setEnabled(
        ui->defaultNoteFileExtensionListWidget->count() > 1);
}

void SettingsDialog::on_showStatusBarNotePathCheckBox_toggled(bool checked) {
    ui->showStatusBarRelativeNotePathCheckBox->setEnabled(checked);
}

void SettingsDialog::on_overrideInterfaceScalingFactorGroupBox_toggled(bool arg1) {
    if (!arg1) {
        Utils::Gui::information(
            this, tr("Override interface scaling factor"),
            tr("If you had this setting enabled, you now need to restart the application manually "
               "so the previous environment variable that overrides the scale factor is not in "
               "your environment again."),
            QStringLiteral("settings-override-interface-scale-factor-off"));
    } else {
        needRestart();
    }
}

void SettingsDialog::on_interfaceScalingFactorSpinBox_valueChanged(int arg1) {
    Q_UNUSED(arg1);
    needRestart();
}
