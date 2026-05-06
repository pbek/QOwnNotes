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
#include <QClipboard>
#include <QDebug>
#include <QDesktopServices>
#include <QFileDialog>
#include <QFontDatabase>
#include <QFontDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeySequence>
#include <QKeySequenceEdit>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPointer>
#include <QRadioButton>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QScrollArea>
#include <QSplitter>
#include <QStatusBar>
#include <QTextBrowser>
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
#include "services/cloudservice.h"
#include "services/openaiservice.h"
#include "services/settingsservice.h"
#include "ui_settingsdialog.h"
#include "version.h"
#include "widgets/fontcolorwidget.h"
#include "widgets/settings/aisettingswidget.h"
#include "widgets/settings/cloudsettingswidget.h"
#include "widgets/settings/debugoptionsettingswidget.h"
#include "widgets/settings/debugsettingswidget.h"
#include "widgets/settings/editorfontcolorsettingswidget.h"
#include "widgets/settings/editorsettingswidget.h"
#include "widgets/settings/harpersettingswidget.h"
#include "widgets/settings/languagetoolsettingswidget.h"
#include "widgets/settings/markdownlspsettingswidget.h"
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

    ui->languageToolSettingsWidget->initialize();
    ui->harperSettingsWidget->initialize();

    updateSearchLineEditIcons();

    ui->gitSettingsWidget->initialize();

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

    // Declare that we need to restart the application if certain settings are changed
    connect(ui->panelsSettingsWidget, &PanelsSettingsWidget::needRestart, this,
            &SettingsDialog::needRestart);
    connect(ui->interfaceSettingsWidget, &InterfaceSettingsWidget::needRestart, this,
            &SettingsDialog::needRestart);
    connect(ui->generalSettingsWidget, &GeneralSettingsWidget::needRestart, this,
            &SettingsDialog::needRestart);
    connect(ui->interfaceSettingsWidget, &InterfaceSettingsWidget::systemTrayToggled,
            ui->generalSettingsWidget, &GeneralSettingsWidget::setAllowOnlyOneAppInstance);
    connect(ui->webApplicationSettingsWidget, &WebApplicationSettingsWidget::needRestart, this,
            &SettingsDialog::needRestart);
    connect(ui->aiAutocompleteCheckBox, SIGNAL(toggled(bool)), this, SLOT(needRestart()));

    // Connect the needRestart signal from the editor font color settings widget
    connect(ui->editorFontColorSettingsWidget, &EditorFontColorSettingsWidget::needRestart, this,
            &SettingsDialog::needRestart);

    // Connect debug settings widget signals
    connect(ui->debugSettingsWidget, &DebugSettingsWidget::aboutToOutputSettings,
            ui->cloudSettingsWidget, &CloudSettingsWidget::storeCloudDebugData);
    // Set back-pointer so CloudSettingsWidget can pass SettingsDialog* to CloudService
    ui->cloudSettingsWidget->setSettingsDialog(this);
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

    // Update cloud connection combo boxes when cloud settings change
    connect(ui->cloudSettingsWidget, &CloudSettingsWidget::cloudConnectionsChanged,
            [this](const QList<CloudConnection> &connections) {
                ui->noteFolderSettingsWidget->populateCloudConnectionComboBox(
                    connections, NoteFolder::currentNoteFolder().getCloudConnectionId());
                ui->todoSettingsWidget->populateCloudConnectionComboBox(
                    connections, CloudConnection::currentTodoCalendarCloudConnection().getId());
            });

    // Connect todo settings widget signals
    connect(ui->todoSettingsWidget, &TodoSettingsWidget::storeSettingsRequested, this,
            &SettingsDialog::storeSettings);
    connect(ui->todoSettingsWidget, &TodoSettingsWidget::reloadCalendarListRequested, this,
            [this]() {
                CloudService *cloud = CloudService::instance(true);
                cloud->settingsGetCalendarList(this);
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

void SettingsDialog::searchScriptInRepository() {
    ui->scriptingSettingsWidget->searchScriptInRepository();
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
    ui->cloudSettingsWidget->storeSettings();

    ui->todoSettingsWidget->storeSettings();
    ui->localTrashSettingsWidget->storeSettings();
    ui->webCompanionSettingsWidget->storeSettings();
    ui->webApplicationSettingsWidget->storeSettings();

    ui->previewFontSettingsWidget->storeSettings();
    ui->debugOptionSettingsWidget->storeSettings();
    ui->editorSettingsWidget->storeSettings();
    ui->languageToolSettingsWidget->storeSettings();
    ui->harperSettingsWidget->storeSettings();
    ui->markdownLspSettingsWidget->storeSettings();

    ui->networkSettingsWidget->storeSettings();

    // Preview font settings are stored in previewFontSettingsWidget->storeSettings()
    ui->editorFontColorSettingsWidget->storeSettings();

    // Store the shortcut settings
    storeShortcutSettings();

    // Store the splitter settings
    storeSplitterSettings();

    // Apply and store the toolbar configuration
    on_applyToolbarButton_clicked();

    // Store the enabled state of the scripts
    ui->scriptingSettingsWidget->storeSettings();

    // Store git settings
    ui->gitSettingsWidget->storeSettings();

    // Panels settings are stored in panelsSettingsWidget->storeSettings()
    ui->panelsSettingsWidget->storeSettings();

    // Interface settings are stored in interfaceSettingsWidget->storeSettings()
    ui->interfaceSettingsWidget->storeSettings();

    // General settings are stored in generalSettingsWidget->storeSettings()
    ui->generalSettingsWidget->storeSettings();

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

    // Set current note folder list item via the widget
    ui->noteFolderSettingsWidget->readSettings();

    // Read cloud settings via the widget
    ui->cloudSettingsWidget->readSettings();

    ui->todoSettingsWidget->readSettings();

    ui->localTrashSettingsWidget->readSettings();
    ui->webCompanionSettingsWidget->readSettings();
    ui->webApplicationSettingsWidget->readSettings();

    ui->previewFontSettingsWidget->readSettings();
    ui->debugOptionSettingsWidget->readSettings();
    ui->editorSettingsWidget->readSettings();
    ui->languageToolSettingsWidget->readSettings();
    ui->harperSettingsWidget->readSettings();
    ui->markdownLspSettingsWidget->readSettings();
    ui->networkSettingsWidget->readSettings();
    ui->editorFontColorSettingsWidget->readSettings();

    // Load the shortcut settings
    loadShortcutSettings();

    // Load git settings
    ui->gitSettingsWidget->readSettings();

    // Panels settings are read in panelsSettingsWidget->readSettings()
    ui->panelsSettingsWidget->readSettings();

    // Interface settings are read in interfaceSettingsWidget->readSettings()
    ui->interfaceSettingsWidget->readSettings();

    // General settings are read in generalSettingsWidget->readSettings()
    ui->generalSettingsWidget->readSettings();

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

    ui->shortcutSearchLineEdit->clear();
    ui->shortcutTreeWidget->clear();
    ui->shortcutTreeWidget->setColumnCount(3);

    // shortcuts on toolbars and note folders don't work yet
    const QStringList disabledMenuNames = QStringList() << QStringLiteral("menuToolbars")
                                                        << QStringLiteral("noteFoldersMenu");

    const QIcon disableShortcutButtonIcon =
        QIcon::fromTheme(QStringLiteral("dialog-cancel"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/dialog-cancel.svg")));
    const QIcon clearButtonIcon =
        QIcon::fromTheme(QStringLiteral("edit-clear"), QIcon(":/icons/breeze-qownnotes/16x16/"
                                                             "edit-clear.svg"));

    const QList<QMenu *> menus =
        mainWindow->menuBar()->findChildren<QMenu *>(QString(), Qt::FindDirectChildrenOnly);

    // loop through all top-level menus and build the tree recursively
    for (const QMenu *menu : menus) {
        if (disabledMenuNames.contains(menu->objectName())) {
            continue;
        }

        buildShortcutTreeForMenu(menu, nullptr, settings, shortcutButtonActiveColor,
                                 shortcutButtonInactiveColor, disableShortcutButtonIcon,
                                 clearButtonIcon, disabledMenuNames);
    }

    Utils::Gui::initTreeWidgetHeaderOrderPersistence(
        ui->shortcutTreeWidget, QStringLiteral("SettingsDialog/shortcutTreeWidgetHeaderOrder"));
}

/**
 * Recursively builds the shortcut tree for the given menu, handling submenus
 * to show the full menu hierarchy in the shortcuts settings
 *
 * @param menu             The menu to process
 * @param parentItem       The parent tree widget item, or nullptr for top-level menus
 * @param settings         The settings service instance
 * @param shortcutButtonActiveColor   Color for active shortcut buttons
 * @param shortcutButtonInactiveColor Color for inactive shortcut buttons
 * @param disableShortcutButtonIcon   Icon for the disable shortcut button
 * @param clearButtonIcon             Icon for the clear button
 * @param disabledMenuNames           List of menu object names to skip
 */
void SettingsDialog::buildShortcutTreeForMenu(const QMenu *menu, QTreeWidgetItem *parentItem,
                                              SettingsService &settings,
                                              const QColor &shortcutButtonActiveColor,
                                              const QColor &shortcutButtonInactiveColor,
                                              const QIcon &disableShortcutButtonIcon,
                                              const QIcon &clearButtonIcon,
                                              const QStringList &disabledMenuNames) {
    // Count eligible actions first to decide whether to add this menu at all
    int actionCount = 0;

    foreach (QAction *action, menu->actions()) {
        if (action->menu() != nullptr) {
            if (!disabledMenuNames.contains(action->menu()->objectName())) {
                actionCount++;
            }
        } else if (!action->objectName().isEmpty()) {
            actionCount++;
        }
    }

    if (actionCount == 0) {
        return;
    }

    // Add the menu item to the tree before populating children so that
    // setItemWidget() works correctly (items must be in the tree first)
    auto *menuItem = new QTreeWidgetItem();
    menuItem->setText(0, menu->title().remove(QStringLiteral("&")));
    menuItem->setToolTip(0, menu->objectName());

    if (parentItem == nullptr) {
        ui->shortcutTreeWidget->addTopLevelItem(menuItem);
    } else {
        parentItem->addChild(menuItem);
    }

    menuItem->setExpanded(true);

    // loop through all actions of the menu
    foreach (QAction *action, menu->actions()) {
        // Handle submenus recursively so the full menu structure is visible
        if (action->menu() != nullptr) {
            const QMenu *subMenu = action->menu();

            if (disabledMenuNames.contains(subMenu->objectName())) {
                continue;
            }

            buildShortcutTreeForMenu(subMenu, menuItem, settings, shortcutButtonActiveColor,
                                     shortcutButtonInactiveColor, disableShortcutButtonIcon,
                                     clearButtonIcon, disabledMenuNames);
            continue;
        }

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

    // Search all action items recursively for a conflicting shortcut
    std::function<bool(QTreeWidgetItem *)> checkItem = [&](QTreeWidgetItem *item) -> bool {
        const QString itemObjectName = item->data(1, Qt::UserRole).toString();

        if (!itemObjectName.isEmpty() && itemObjectName != objectName) {
            auto *frameWidget = ui->shortcutTreeWidget->itemWidget(item, 1);

            if (frameWidget != nullptr) {
                const auto keySequenceWidgets = frameWidget->findChildren<QKeySequenceWidget *>();

                if (keySequenceWidgets.count() > 0) {
                    QKeySequence keySequence = keySequenceWidgets.at(0)->keySequence();

                    // show an information if the shortcut was already used elsewhere
                    if (keySequence == eventKeySequence) {
                        if (Utils::Gui::information(
                                this, tr("Shortcut already assigned"),
                                tr("The shortcut <strong>%1</strong> is already "
                                   "assigned to <strong>%2</strong>! Do you want to "
                                   "jump to the shortcut?")
                                    .arg(eventKeySequence.toString(), item->text(0)),
                                QStringLiteral("settings-shortcut-already-assigned"),
                                QMessageBox::Yes | QMessageBox::Cancel,
                                QMessageBox::Yes) == QMessageBox::Yes) {
                            ui->shortcutTreeWidget->scrollToItem(item);
                            ui->shortcutTreeWidget->clearSelection();
                            item->setSelected(true);
                        }

                        return true;
                    }
                }
            }
        }

        for (int i = 0; i < item->childCount(); i++) {
            if (checkItem(item->child(i))) {
                return true;
            }
        }

        return false;
    };

    for (int i = 0; i < ui->shortcutTreeWidget->topLevelItemCount(); i++) {
        if (checkItem(ui->shortcutTreeWidget->topLevelItem(i))) {
            return;
        }
    }
}

/**
 * Finds a QKeySequenceWidget in the shortcutTreeWidget by the objectName
 * of the assigned menu action
 */
QKeySequenceWidget *SettingsDialog::findKeySequenceWidget(const QString &objectName) {
    QKeySequenceWidget *result = nullptr;

    std::function<void(QTreeWidgetItem *)> findItem = [&](QTreeWidgetItem *item) {
        if (result != nullptr) {
            return;
        }

        if (item->data(1, Qt::UserRole).toString() == objectName) {
            auto *frameWidget = ui->shortcutTreeWidget->itemWidget(item, 1);

            if (frameWidget != nullptr) {
                const auto keySequenceWidgets = frameWidget->findChildren<QKeySequenceWidget *>();

                if (keySequenceWidgets.count() > 0) {
                    result = keySequenceWidgets.at(0);
                    return;
                }
            }
        }

        for (int i = 0; i < item->childCount(); i++) {
            findItem(item->child(i));
        }
    };

    for (int i = 0; i < ui->shortcutTreeWidget->topLevelItemCount(); i++) {
        findItem(ui->shortcutTreeWidget->topLevelItem(i));
    }

    return result;
}

/**
 * Finds the global QKeySequenceWidget in the shortcutTreeWidget by the
 * objectName of the assigned menu action (column 2)
 */
QKeySequenceWidget *SettingsDialog::findGlobalKeySequenceWidget(const QString &objectName) {
    QKeySequenceWidget *result = nullptr;

    std::function<void(QTreeWidgetItem *)> findItem = [&](QTreeWidgetItem *item) {
        if (result != nullptr) {
            return;
        }

        if (item->data(1, Qt::UserRole).toString() == objectName) {
            result =
                dynamic_cast<QKeySequenceWidget *>(ui->shortcutTreeWidget->itemWidget(item, 2));

            if (result != nullptr) {
                return;
            }
        }

        for (int i = 0; i < item->childCount(); i++) {
            findItem(item->child(i));
        }
    };

    for (int i = 0; i < ui->shortcutTreeWidget->topLevelItemCount(); i++) {
        findItem(ui->shortcutTreeWidget->topLevelItem(i));
    }

    return result;
}

/**
 * Stores the local and global keyboard shortcut settings
 */
void SettingsDialog::storeShortcutSettings() {
    SettingsService settings;
    MainWindow *mainWindow = MainWindow::instance();

    if (mainWindow == nullptr) {
        return;
    }

    // Store shortcuts by iterating through the actual menu actions (the same
    // way initShortcuts reads them) and looking up the corresponding widgets
    // in the tree. This avoids any issues with tree widget item traversal.
    const QList<QMenu *> menus = mainWindow->menuList();

    for (QMenu *menu : menus) {
        for (QAction *action : menu->actions()) {
            const QString actionObjectName = action->objectName();

            if (actionObjectName.isEmpty()) {
                continue;
            }

            // Find the key sequence widget for this action in the tree
            auto *keyWidget = findKeySequenceWidget(actionObjectName);

            if (keyWidget != nullptr) {
                QKeySequence keySequence = keyWidget->keySequence();
                QKeySequence defaultKeySequence = keyWidget->defaultKeySequence();
                const QString settingsKey =
                    QStringLiteral("Shortcuts/MainWindow-") + actionObjectName;

                // remove or store the setting for the shortcut if it's not default
                if (keySequence == defaultKeySequence) {
                    settings.remove(settingsKey);
                } else {
                    settings.setValue(settingsKey, keySequence.toString());
                }
            }

            // Find the global shortcut widget for this action in the tree
            auto *globalWidget = findGlobalKeySequenceWidget(actionObjectName);

            if (globalWidget != nullptr) {
                QKeySequence keySequence = globalWidget->keySequence();
                const QString settingsKey =
                    QStringLiteral("GlobalShortcuts/MainWindow-") + actionObjectName;

                if (keySequence.isEmpty()) {
                    settings.remove(settingsKey);
                } else {
                    settings.setValue(settingsKey, keySequence.toString());
                }
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
 * Forwards the connection test callback to the CloudSettingsWidget
 */
void SettingsDialog::connectTestCallback(bool appIsValid, QString appVersion, QString serverVersion,
                                         QString notesPathExistsText,
                                         QString connectionErrorMessage) {
    ui->cloudSettingsWidget->connectTestCallback(
        appIsValid, std::move(appVersion), std::move(serverVersion), std::move(notesPathExistsText),
        std::move(connectionErrorMessage));
}

/**
 * Forwards the OK label data to the CloudSettingsWidget
 */
void SettingsDialog::setOKLabelData(int number, const QString &text, OKLabelStatus status) {
    ui->cloudSettingsWidget->setOKLabelData(number, text, status);
}

void SettingsDialog::refreshTodoCalendarList(const QList<CalDAVCalendarData> &items,
                                             bool forceReadCheckedState) {
    ui->todoSettingsWidget->refreshTodoCalendarList(items, ui->cloudSettingsWidget->serverUrl(),
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

/**
 * Delegates the remote path list callback to the NoteFolderSettingsWidget.
 * Called by CloudService::loadDirectory().
 */
void SettingsDialog::setNoteFolderRemotePathList(QStringList pathList) {
    ui->noteFolderSettingsWidget->setNoteFolderRemotePathList(pathList);
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

    // Delegate language search line edit icon update to the interface settings widget
    ui->interfaceSettingsWidget->updateSearchIcons();

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
            if (Utils::Gui::hasTreeWidgetHeaderLayout(ui->shortcutTreeWidget)) {
                Utils::Gui::restoreTreeWidgetHeaderLayout(ui->shortcutTreeWidget);
            } else {
                ui->shortcutTreeWidget->resizeColumnToContents(0);
                ui->shortcutTreeWidget->resizeColumnToContents(1);
                ui->shortcutTreeWidget->resizeColumnToContents(2);
            }
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
            ui->interfaceSettingsWidget->initialize();
        } break;
        case SettingsPages::GeneralPage: {
            ui->generalSettingsWidget->initialize();

#ifdef Q_OS_MAC
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
        case SettingsPages::CloudPage: {
            ui->cloudSettingsWidget->initialize();
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
        case SettingsPages::MarkdownLspPage: {
            ui->markdownLspSettingsWidget->initialize();
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
    } else if (index == CloudPage) {
        ui->cloudSettingsWidget->resetOKLabelData();
    } else if (index == AiPage) {
        ui->aiSettingsWidget->buildAiScriptingTreeWidget();
    }

    // turn off the tasks page if no cloud settings are available
    //    QTreeWidgetItem *todoItem =
    //    findSettingsTreeWidgetItemByPage(TodoPage); if (todoItem != nullptr)
    //    {
    //        if (CloudService::hasCloudSettings()) {
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

    ui->cloudSettingsWidget->cancelConnectionTest();

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
