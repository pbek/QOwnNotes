#include "mainwindow.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QSettings>
#include <QTimer>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QActionGroup>
#include <QSystemTrayIcon>
#include <QShortcut>
#include <QPrinter>
#include <QPrintDialog>
#include <QPageSetupDialog>
#include <QMimeData>
#include <QTextBlock>
#include <QClipboard>
#include <QTemporaryFile>
#include <QScrollBar>
#include <QTextDocumentFragment>
#include <QProcess>
#include <QJSEngine>
#include <QInputDialog>
#include <QCompleter>
#include <QTreeWidgetItem>
#include <QCoreApplication>
#include <QUuid>
#include "ui_mainwindow.h"
#include "dialogs/linkdialog.h"
#include "services/owncloudservice.h"
#include "services/databaseservice.h"
#include "dialogs/tododialog.h"
#include "libraries/diff_match_patch/diff_match_patch.h"
#include "dialogs/notediffdialog.h"
#include "build_number.h"
#include "version.h"
#include "release.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/settingsdialog.h"
#include "entities/calendaritem.h"
#include "widgets/qownnotesmarkdowntextedit.h"
#include "dialogs/passworddialog.h"
#include "services/metricsservice.h"
#include <services/cryptoservice.h>
#include <helpers/clientproxy.h>
#include <utils/misc.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <entities/tag.h>
#include <dialogs/tagadddialog.h>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>
#include <QQmlApplicationEngine>
#include <QWidgetAction>
#include <QColorDialog>
#include <services/scriptingservice.h>
#include <dialogs/evernoteimportdialog.h>
#include <widgets/logwidget.h>
#include <dialogs/sharedialog.h>
#include <dialogs/orphanedimagesdialog.h>
#include <helpers/toolbarcontainer.h>
#include <libraries/qttoolbareditor/src/toolbar_editor.hpp>
#include <dialogs/actiondialog.h>


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    // use our custom log handler
    qInstallMessageHandler(LogWidget::logMessageOutput);
    qApp->setProperty("loggingEnabled", true);

#ifdef Q_OS_MAC
    // disable icons in the menu
    QApplication::instance()->setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif

    ui->setupUi(this);

    // initialize the workspace combo box
    initWorkspaceComboBox();

#ifdef Q_OS_MAC
    // disable icons in the menu that weren't handled by
    // Qt::AA_DontShowIconsInMenus
    ui->actionShare_note->setIconVisibleInMenu(false);
#endif

    _noteViewIsRegenerated = false;
    _searchLineEditFromCompleter = false;
    _isNotesDirectoryWasModifiedDisabled = false;
    _isDefaultShortcutInitialized = false;
    _showNotesFromAllNoteSubFolders = false;
    _noteFolderDockWidgetWasVisible = true;
    _noteSubFolderDockWidgetVisible = true;
    this->setWindowTitle(
            "QOwnNotes - version " + QString(VERSION) +
                    " - build " + QString::number(BUILD));

    qApp->setProperty(
            "mainWindow",
            QVariant::fromValue<MainWindow *>(this));

    ClientProxy proxy;
    // refresh the Qt proxy settings
    proxy.setupQtProxyFromSettings();

    QActionGroup *sorting = new QActionGroup(this);
    sorting->addAction(ui->actionAlphabetical);
    sorting->addAction(ui->actionBy_date);
    sorting->setExclusive(true);

    // we only want to see that menu entry if there are note subfolders
    ui->actionFind_notes_in_all_subfolders->setVisible(false);

    // hide the encrypted note text edit by default
    ui->encryptedNoteTextEdit->hide();

    // set the search frames for the note text edits
    ui->noteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame);
    ui->encryptedNoteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame);

    // set the main window for accessing it's public methods
    ui->noteTextEdit->setMainWindow(this);
    ui->encryptedNoteTextEdit->setMainWindow(this);

    // initialize the tag button scroll area
    initTagButtonScrollArea();

    noteHistory = NoteHistory();

    // set our signal mapper
    recentNoteFolderSignalMapper = new QSignalMapper(this);

    // initialize the toolbars
    initToolbars();

#ifdef Q_OS_MAC
    // add some different shortcuts for the note history on the mac
    ui->action_Back_in_note_history->
            setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_Left);
    ui->action_Forward_in_note_history->
            setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_Right);

    // add an other shortcut for the auto-completer
    ui->actionAutocomplete->setShortcut(Qt::META + Qt::Key_Space);

    // add an other shortcut for inserting media
    ui->actionPaste_image->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_V);
#endif

    // adding some alternate shortcuts for changing the current note
    QShortcut *shortcut = new QShortcut(QKeySequence("Ctrl+PgDown"), this);
    QObject::connect(shortcut, SIGNAL(activated()),
                     this, SLOT(on_actionNext_note_triggered()));
    shortcut = new QShortcut(QKeySequence("Ctrl+PgUp"), this);
    QObject::connect(shortcut, SIGNAL(activated()),
                     this, SLOT(on_actionPrevious_Note_triggered()));

    // read the settings (shortcuts have to be defined before that)
    readSettings();

    // initialize the scripting engine
    initScriptingEngine();

    // restore toolbars
    restoreToolbars();

    // check if we want to start the application hidden
    initShowHidden();

    // initialize the dock widgets
    initDockWidgets();

    // set sorting
    ui->actionBy_date->setChecked(!sortAlphabetically);
    ui->actionAlphabetical->setChecked(sortAlphabetically);

    // set the show in system tray checkbox
    ui->actionShow_system_tray->setChecked(showSystemTray);

    createSystemTrayIcon();
    buildNotesIndexAndLoadNoteDirectoryList();

    // setup the update available button
    setupUpdateAvailableButton();

    this->noteDiffDialog = new NoteDiffDialog();

    // look if we need to save something every 10 sec (default)
    this->noteSaveTimer = new QTimer(this);
    QObject::connect(
            this->noteSaveTimer,
            SIGNAL(timeout()),
            this,
            SLOT(storeUpdatedNotesToDisk()));
    this->noteSaveTimer->start(this->noteSaveIntervalTime * 1000);

    // look if we need update the note view every two seconds
    _noteViewUpdateTimer = new QTimer(this);
    QObject::connect(
            _noteViewUpdateTimer,
            SIGNAL(timeout()),
            this,
            SLOT(noteViewUpdateTimerSlot()));
    _noteViewUpdateTimer->start(2000);

    // check if we have a tasks reminder every minute
    this->todoReminderTimer = new QTimer(this);
    QObject::connect(
            this->todoReminderTimer,
            SIGNAL(timeout()),
            this,
            SLOT(frequentPeriodicChecker()));
    this->todoReminderTimer->start(60000);

    QObject::connect(
            &this->noteDirectoryWatcher,
            SIGNAL(directoryChanged(QString)),
            this,
            SLOT(notesDirectoryWasModified(QString)));
    QObject::connect(
            &this->noteDirectoryWatcher,
            SIGNAL(fileChanged(QString)),
            this,
            SLOT(notesWereModified(QString)));
    ui->searchLineEdit->installEventFilter(this);
    ui->noteTreeWidget->installEventFilter(this);
    ui->noteTextEdit->installEventFilter(this);
    ui->noteTextEdit->viewport()->installEventFilter(this);
    ui->encryptedNoteTextEdit->installEventFilter(this);
    ui->encryptedNoteTextEdit->viewport()->installEventFilter(this);
    ui->tagTreeWidget->installEventFilter(this);
    // jump to the first note
    resetCurrentNote();

    // init the saved searches completer
    initSavedSearchesCompleter();

    // ignores note clicks in QMarkdownTextEdit in the note text edit
    ui->noteTextEdit->setIgnoredClickUrlSchemata(
            QStringList() << "note" << "task");
    ui->encryptedNoteTextEdit->setIgnoredClickUrlSchemata(
            QStringList() << "note" << "task");

    // handle note url externally in the note text edit
    QObject::connect(
            ui->noteTextEdit,
            SIGNAL(urlClicked(QString)),
            this,
            SLOT(openLocalUrl(QString)));

    // also handle note url externally in the encrypted note text edit
    QObject::connect(
            ui->encryptedNoteTextEdit,
            SIGNAL(urlClicked(QString)),
            this,
            SLOT(openLocalUrl(QString)));

    // set the tab stop to the width of 4 spaces in the editor
    const int tabStop = 4;
    QFont font = ui->noteTextEdit->font();
    QFontMetrics metrics(font);
    int width = tabStop * metrics.width(' ');
    ui->noteTextEdit->setTabStopWidth(width);
    ui->encryptedNoteTextEdit->setTabStopWidth(width);

    // called now in readSettingsFromSettingsDialog() line 494
    // set the edit mode for the note text edit
    // this->setNoteTextEditMode(true);

    // load the note folder list in the menu
    this->loadNoteFolderListMenu();

    this->updateService = new UpdateService(this);
    this->updateService->checkForUpdates(this, UpdateService::AppStart);

    // we need to restore the current workspace a little later when
    // application window is maximized or in full screen mode
    if (isMaximized() || isFullScreen()) {
        QTimer::singleShot(200, this, SLOT(restoreCurrentWorkspace()));
    }

    // update the current folder tooltip
    updateCurrentFolderTooltip();

    // show the app metrics notification if not already shown
    showAppMetricsNotificationIfNeeded();

    frequentPeriodicChecker();

    // setup the shortcuts for the note bookmarks
    setupNoteBookmarkShortcuts();

    // restore the distraction free mode
    restoreDistractionFreeMode();

    // add action tracking
    connect(ui->menuBar, SIGNAL(triggered(QAction *)),
            this, SLOT(trackAction(QAction *)));

    // set "show toolbar" menu item checked/unchecked
    const QSignalBlocker blocker(ui->actionShow_toolbar);
    {
        Q_UNUSED(blocker);
        ui->actionShow_toolbar->setChecked(isToolbarVisible());
    }

    Q_FOREACH(QToolBar *toolbar, findChildren<QToolBar*>()) {
            connect(toolbar, SIGNAL(visibilityChanged(bool)),
                    this, SLOT(toolbarVisibilityChanged(bool)));
        }

    // set the action group for the width selector of the distraction free mode
    QActionGroup *dfmEditorWidthActionGroup = new QActionGroup(this);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthNarrow);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthMedium);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthWide);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthFull);
    dfmEditorWidthActionGroup->setExclusive(true);

    connect(dfmEditorWidthActionGroup, SIGNAL(triggered(QAction *)),
            this, SLOT(dfmEditorWidthActionTriggered(QAction *)));

    setAcceptDrops(true);
    // we need to disallow this explicitly under Windows
    // so that the MainWindow gets the event
    ui->noteTextEdit->setAcceptDrops(false);

    // do a bit more styling
    initStyling();

    // act on position clicks in the navigation widget
    QObject::connect(ui->navigationWidget, SIGNAL(positionClicked(int)),
                     this, SLOT(onNavigationWidgetPositionClicked(int)));

    // do the navigation parsing after the highlighter was finished
    QObject::connect(ui->noteTextEdit->highlighter(),
                     SIGNAL(parsingFinished()),
                     this,
                     SLOT(startNavigationParser()));
    QObject::connect(ui->encryptedNoteTextEdit->highlighter(),
                     SIGNAL(parsingFinished()),
                     this,
                     SLOT(startNavigationParser()));

    // act on note preview resize
    QObject::connect(ui->noteTextView,
                     SIGNAL(resize(QSize, QSize)),
                     this,
                     SLOT(onNoteTextViewResize(QSize, QSize)));

    // reloads all tasks from the ownCloud server
    reloadTodoLists();

    // create a timer to load all tasks every 10min
    _todoListTimer = new QTimer(this);
    QObject::connect(
            _todoListTimer, SIGNAL(timeout()),
            this, SLOT(reloadTodoLists()));
    _todoListTimer->start(600000);

    // setup the softwrap checkbox
    const QSignalBlocker blocker2(ui->actionUse_softwrap_in_note_editor);
    Q_UNUSED(blocker2);
    QSettings settings;
    ui->actionUse_softwrap_in_note_editor->setChecked(
            settings.value("useSoftWrapInNoteEditor", true).toBool());

    // initialize the editor soft wrapping
    initEditorSoftWrap();

    // check if user has set ownCloud settings
    MetricsService::instance()->sendEventIfEnabled(
            "app/has-owncloud-settings", "app", "has owncloud settings",
            OwnCloudService::hasOwnCloudSettings() ? "yes" : "no");

    // send an event for counting the editor color schemes
    int schemaCount = settings.value("Editor/ColorSchemes").toStringList()
            .count();
    MetricsService::instance()->sendEventIfEnabled(
            "app/editor-color-schema-count",
            "editor",
            "editor color schema count",
            QString::number(schemaCount) + " schemas",
            schemaCount);
}

MainWindow::~MainWindow() {
    if (!isInDistractionFreeMode()) {
        storeCurrentWorkspace();
    }

    storeUpdatedNotesToDisk();

    qApp->setProperty("loggingEnabled", false);

    if (showSystemTray) {
        // if we are using the system tray lets delete the log window so the
        // app can quit
        delete(LogWidget::instance());
    }

    delete ui;
}

/*!
 * Methods
 */


/**
 * Initializes the workspace combo box
 */
void MainWindow::initWorkspaceComboBox() {
    _workspaceComboBox = new QComboBox(this);
    connect(_workspaceComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onWorkspaceComboBoxCurrentIndexChanged(int)));
    _workspaceComboBox->setToolTip(tr("Workspaces"));
    _workspaceComboBox->setObjectName("workspaceComboBox");
    _workspaceSignalMapper = new QSignalMapper(this);
}

/**
 * Initializes the dock widgets
 */
void MainWindow::initDockWidgets() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    setDockOptions(dockOptions() | GroupedDragging);
#endif
    QSizePolicy sizePolicy;

    _noteSubFolderDockWidget = new QDockWidget(tr("Subfolders"), this);
    _noteSubFolderDockWidget->setObjectName("noteSubFolderDockWidget");
    _noteSubFolderDockWidget->setWidget(ui->noteSubFolderFrame);
    _noteSubFolderDockTitleBarWidget =
            _noteSubFolderDockWidget->titleBarWidget();
    sizePolicy = _noteSubFolderDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteSubFolderDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteSubFolderDockWidget,
                  Qt::Horizontal);

    _taggingDockWidget = new QDockWidget(tr("Tags"), this);
    _taggingDockWidget->setObjectName("taggingDockWidget");
    _taggingDockWidget->setWidget(ui->tagFrame);
    _taggingDockTitleBarWidget = _taggingDockWidget->titleBarWidget();
    sizePolicy = _taggingDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _taggingDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _taggingDockWidget, Qt::Vertical);

    _noteSearchDockWidget = new QDockWidget(tr("Note search"), this);
    _noteSearchDockWidget->setObjectName("noteSearchDockWidget");
    _noteSearchDockWidget->setWidget(ui->searchLineEdit);
    _noteSearchDockTitleBarWidget = _noteSearchDockWidget->titleBarWidget();
    sizePolicy = _noteSearchDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteSearchDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteSearchDockWidget, Qt::Vertical);

    _noteFolderDockWidget = new QDockWidget(tr("Note folder"), this);
    _noteFolderDockWidget->setObjectName("noteFolderDockWidget");
    _noteFolderDockWidget->setWidget(ui->noteFolderComboBox);
    _noteFolderDockTitleBarWidget = _noteFolderDockWidget->titleBarWidget();
    sizePolicy = _noteFolderDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteFolderDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteFolderDockWidget, Qt::Vertical);

    _noteListDockWidget = new QDockWidget(tr("Note list"), this);
    _noteListDockWidget->setObjectName("noteListDockWidget");
    _noteListDockWidget->setWidget(ui->notesListFrame);
    _noteListDockTitleBarWidget = _noteListDockWidget->titleBarWidget();
    sizePolicy = _noteListDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteListDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteListDockWidget, Qt::Vertical);

    _noteNavigationDockWidget = new QDockWidget(tr("Navigation"), this);
    _noteNavigationDockWidget->setObjectName("noteNavigationDockWidget");
    _noteNavigationDockWidget->setWidget(ui->navigationFrame);
    _noteNavigationDockTitleBarWidget =
            _noteNavigationDockWidget->titleBarWidget();
    sizePolicy = _noteNavigationDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteNavigationDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteNavigationDockWidget,
                  Qt::Vertical);
    // we want the navigation under the note list
    splitDockWidget(_noteListDockWidget, _noteNavigationDockWidget,
                    Qt::Vertical);

    _noteEditDockWidget = new QDockWidget(tr("Note edit"), this);
    _noteEditDockWidget->setObjectName("noteEditDockWidget");
    _noteEditDockWidget->setWidget(ui->noteEditFrame);
    _noteEditDockTitleBarWidget = _noteEditDockWidget->titleBarWidget();
    sizePolicy = _noteEditDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(5);
    _noteEditDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::RightDockWidgetArea, _noteEditDockWidget, Qt::Horizontal);

    _noteTagDockWidget = new QDockWidget(tr("Note tags"), this);
    _noteTagDockWidget->setObjectName("noteTagDockWidget");
    _noteTagDockWidget->setWidget(ui->noteTagFrame);
    _noteTagDockTitleBarWidget = _noteTagDockWidget->titleBarWidget();
    sizePolicy = _noteTagDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(3);
    _noteTagDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::RightDockWidgetArea, _noteTagDockWidget, Qt::Vertical);

    QSettings settings;
    bool wasInit = settings.value("dockWasInitializedOnce", false).toBool();
    if (!wasInit) {
        // I found no other easy way to set the height on the first start
#ifdef Q_OS_LINUX
        _noteTagDockWidget->setMaximumHeight(120);
#else
        _noteTagDockWidget->setMaximumHeight(50);
#endif

        settings.setValue("dockWasInitializedOnce", true);
    }

    _notePreviewDockWidget = new QDockWidget(tr("Note preview"), this);
    _notePreviewDockWidget->setObjectName("notePreviewDockWidget");
    _notePreviewDockWidget->setWidget(ui->noteViewFrame);
    _notePreviewDockTitleBarWidget = _notePreviewDockWidget->titleBarWidget();
    addDockWidget(Qt::RightDockWidgetArea, _notePreviewDockWidget,
                  Qt::Horizontal);

    _logDockWidget = new QDockWidget(tr("Log"), this);
    _logDockWidget->setObjectName("logDockWidget");
    _logDockWidget->setWidget(LogWidget::instance());
    _logDockTitleBarWidget = _logDockWidget->titleBarWidget();
    addDockWidget(Qt::RightDockWidgetArea, _logDockWidget, Qt::Vertical);
    _logDockWidget->hide();

//    ui->noteEditFrame->setStyleSheet("* { border: none; }");
//    ui->noteTextEdit->setStyleSheet("* { border: none; }");
//    ui->noteEditFrame->layout()->setContentsMargins(0, 0, 0, 0);

    setDockNestingEnabled(true);
    setCentralWidget(Q_NULLPTR);

    // restore the current workspace
    restoreCurrentWorkspace();

    // lock the dock widgets
    on_actionUnlock_panels_toggled(false);

    // update the workspace menu and combobox entries
    updateWorkspaceLists();

    // initialize the panel menu
    initPanelMenu();
}

/**
 * Initializes if we want to start the application hidden
 */
void MainWindow::initShowHidden() {
    QSettings settings;
    bool startHidden = settings.value("StartHidden", false).toBool();
    ui->actionStart_hidden->setChecked(startHidden);
    ui->actionStart_hidden->setEnabled(showSystemTray);
    if (startHidden) {
        QTimer::singleShot(250, this, SLOT(hide()));
    }
}

/**
 * Initializes the tag button scroll area
 *
 * If there are more tags assigned to a note than the width of the edit
 * pane allows there now will be used a scrollbar to scroll through the
 * tags, so that the width of the edit pane can still be small
 */
void MainWindow::initTagButtonScrollArea() {
    _noteTagButtonScrollArea = new QScrollArea(this);
    _noteTagButtonScrollArea->setWidgetResizable(true);
    _noteTagButtonScrollArea->setSizePolicy(QSizePolicy::MinimumExpanding,
                                      QSizePolicy::Minimum);
    _noteTagButtonScrollArea->setAlignment(Qt::AlignLeft);
    _noteTagButtonScrollArea->setWidget(ui->noteTagButtonFrame);

    ui->noteTagButtonFrame->layout()->setContentsMargins(0, 0, 0, 0);
    _noteTagButtonScrollArea->setContentsMargins(0, 0, 0, 0);

#ifdef Q_OS_MAC
    // we need to set a minimum height under OS X or else the scroll area
    // will be far to high
    _noteTagButtonScrollArea->setMinimumHeight(36);
#endif
#ifdef Q_OS_WIN32
    // we need to set a minimum height under Windows or else the scroll area
    // will be far to high
    _noteTagButtonScrollArea->setMinimumHeight(40);
#endif

    ui->noteTagFrame->layout()->addWidget(_noteTagButtonScrollArea);
    ui->noteTagFrame->layout()->addWidget(ui->newNoteTagButton);
    ui->noteTagFrame->layout()->addWidget(ui->newNoteTagLineEdit);
}

/**
 * Returns all menus from the menu bar
 */
QList<QMenu *> MainWindow::menuList() {
    return ui->menuBar->findChildren<QMenu *>();
}

/**
 * Finds an action in all menus of the menu bar
 */
QAction *MainWindow::findAction(QString objectName) {
    QList<QMenu*> menus = menuList();

    // loop through all menus because we were not able to find the action with
    // ui->menuBar->findChild<QAction *>(objectName);
    foreach(QMenu* menu, menus) {
            // loop through all actions of the menu
            foreach(QAction* action, menu->actions()) {
                    if (action->objectName() == objectName) {
                        return action;
                    }
            }
    }

    return Q_NULLPTR;
}

/**
 * Builds the note index and loads the note directory list
 *
 * @param forceBuild
 * @param forceLoad
 */
void MainWindow::buildNotesIndexAndLoadNoteDirectoryList(bool forceBuild,
                                                         bool forceLoad) {
    bool wasBuilt = buildNotesIndex(0, forceBuild);

    if (wasBuilt || forceLoad) {
        loadNoteDirectoryList();
    }
}

/**
 * Returns the global main window instance
 */
MainWindow *MainWindow::instance() {
    return qApp->property("mainWindow").value<MainWindow *>();
}

/**
 * Initializes the editor soft wrapping
 */
void MainWindow::initEditorSoftWrap() {
    QSettings settings;
    QTextEdit::LineWrapMode mode =
            settings.value("useSoftWrapInNoteEditor", true).toBool() ?
            QTextEdit::WidgetWidth : QTextEdit::NoWrap;

    ui->noteTextEdit->setLineWrapMode(mode);
    ui->encryptedNoteTextEdit->setLineWrapMode(mode);
    ui->noteTextView->setLineWrapMode(mode);
}

/**
 * Reloads all tasks from the ownCloud server
 */
void MainWindow::reloadTodoLists() {
    QSettings settings;
    QStringList calendars =
            settings.value("ownCloud/todoCalendarEnabledList").toStringList();
    QString serverUrl =
            settings.value("ownCloud/serverUrl").toString().trimmed();

    if (calendars.count() > 0 && !serverUrl.isEmpty()) {
        OwnCloudService *ownCloud = new OwnCloudService(this);

        QListIterator<QString> itr(calendars);
        while (itr.hasNext()) {
            QString calendar =  itr.next();
            ownCloud->todoGetTodoList(calendar, Q_NULLPTR);
        }

        showStatusBarMessage(
                tr("Your tasks are being loaded from your server"),
                4000);

        // generate the system tray context menu to show modified tasks
        // in 15 sec (because we don't know when all new tasks will be loaded)
        QTimer::singleShot(15000, this, SLOT(generateSystemTrayContextMenu()));
    }
}

/**
 * Initializes the scripting engine
 */
void MainWindow::initScriptingEngine() {
    _customActionSignalMapper = new QSignalMapper(this);
    ui->menuCustom_actions->hide();
    _customActionToolbar->hide();

    // connect the custom action signal mapper
    QObject::connect(_customActionSignalMapper,
                     SIGNAL(mapped(QString)),
                     this,
                     SLOT(onCustomActionInvoked(QString)));

    ScriptingService* scriptingService = ScriptingService::createInstance(this);
    QQmlEngine* engine = scriptingService->engine();
//    engine->setObjectOwnership(ui->noteTextEdit, QQmlEngine::CppOwnership);
    engine->rootContext()->setContextProperty("noteTextEdit", ui->noteTextEdit);
    scriptingService->initComponents();
}

/**
 * Invokes the custom action in the scripting service
 *
 * @param identifier
 */
void MainWindow::onCustomActionInvoked(QString identifier) {
    ScriptingService *scriptingService = ScriptingService::instance();
    if (scriptingService != Q_NULLPTR) {
        scriptingService->onCustomActionInvoked(identifier);
    }
}

/**
 * Initializes the toolbars
 */
void MainWindow::initToolbars() {
    _formattingToolbar = new QToolBar(tr("formatting toolbar"), this);
    _formattingToolbar->addAction(ui->actionFormat_text_bold);
    _formattingToolbar->addAction(ui->actionFormat_text_italic);
    _formattingToolbar->addAction(ui->actionStrike_out_text);
    _formattingToolbar->addAction(ui->actionInset_code_block);
    _formattingToolbar->setObjectName("formattingToolbar");
    addToolBar(_formattingToolbar);

    _insertingToolbar = new QToolBar(tr("inserting toolbar"), this);
    _insertingToolbar->addAction(ui->actionInsert_Link_to_note);
    _insertingToolbar->addAction(ui->actionInsert_image);
    _insertingToolbar->addAction(ui->actionInsert_current_time);
    _insertingToolbar->setObjectName("insertingToolbar");
    addToolBar(_insertingToolbar);

    _encryptionToolbar = new QToolBar(tr("encryption toolbar"), this);
    _encryptionToolbar->addAction(ui->action_Encrypt_note);
    _encryptionToolbar->addAction(ui->actionEdit_encrypted_note);
    _encryptionToolbar->addAction(ui->actionDecrypt_note);
    _encryptionToolbar->setObjectName("encryptionToolbar");
    addToolBar(_encryptionToolbar);

    _windowToolbar = new QToolBar(tr("window toolbar"), this);
    updateWindowToolbar();
    _windowToolbar->setObjectName("windowToolbar");
    addToolBar(_windowToolbar);

    _customActionToolbar = new QToolBar(tr("custom action toolbar"), this);
    _customActionToolbar->setObjectName("customActionsToolbar");
    _customActionToolbar->hide();
    addToolBar(_customActionToolbar);

    _quitToolbar = new QToolBar(tr("quit toolbar"), this);
    _quitToolbar->addAction(ui->action_Quit);
    _quitToolbar->setObjectName("quitToolbar");
    addToolBar(_quitToolbar);
}

/**
 * Populates the window toolbar
 */
void MainWindow::updateWindowToolbar() {
    _windowToolbar->clear();

    QWidgetAction *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(_workspaceComboBox);
    widgetAction->setObjectName("actionWorkspaceComboBox");
    widgetAction->setText(tr("Workspace selector"));
    _windowToolbar->addAction(widgetAction);
    _windowToolbar->addAction(ui->actionStore_as_new_workspace);
    _windowToolbar->addAction(ui->actionRemove_current_workspace);
    _windowToolbar->addAction(ui->actionRename_current_workspace);
    _windowToolbar->addAction(ui->actionSwitch_to_previous_workspace);
    _windowToolbar->addAction(ui->actionUnlock_panels);

    _windowToolbar->addSeparator();
    _windowToolbar->addAction(
            ui->actionToggle_distraction_free_mode);
    _windowToolbar->addAction(ui->action_Increase_note_text_size);
    _windowToolbar->addAction(ui->action_Decrease_note_text_size);
    _windowToolbar->addAction(ui->action_Reset_note_text_size);
}

/**
 * Updates the workspace menu and combobox entries
 */
void MainWindow::updateWorkspaceLists(bool rebuild) {
    QSettings settings;
    QStringList workspaces = getWorkspaceUuidList();
    QString currentUuid = currentWorkspaceUuid();

    if (rebuild) {
        // we need to create a new combo box so the width gets updated in the
        // window toolbar
        initWorkspaceComboBox();

        ui->menuWorkspaces->clear();
    }

    const QSignalBlocker blocker(_workspaceComboBox);
    Q_UNUSED(blocker);

    int currentIndex = 0;

    for (int i = 0; i < workspaces.count(); i++) {
        QString uuid = workspaces.at(i);

        if (uuid == currentUuid) {
            currentIndex = i;
        }

        // check if we want to skip the rebuilding part
        if (!rebuild) {
            continue;
        }

        QString name = settings.value("workspace-" + uuid + "/name").toString();

        _workspaceComboBox->addItem(name, uuid);

        QAction *action = new QAction(name, ui->menuWorkspaces);
        QObject::connect(action, SIGNAL(triggered()),
                         _workspaceSignalMapper, SLOT(map()));

        // add a parameter to the signal mapper
        _workspaceSignalMapper->setMapping(action, uuid);

        // set an object name for creating shortcuts
        action->setObjectName("restoreWorkspace" + QString::number(i));

//        if (uuid == currentUuid) {
//            QFont font = action->font();
//            font.setBold(true);
//            action->setFont(font);
//        }

        ui->menuWorkspaces->addAction(action);
    }

    _workspaceComboBox->setCurrentIndex(currentIndex);

    if (rebuild) {
        // set the new current workspace if a menu entry was triggered
        QObject::connect(_workspaceSignalMapper,
                         SIGNAL(mapped(QString)),
                         this,
                         SLOT(setCurrentWorkspace(QString)));

        // we need to adapt the width of the workspaces combo box
        updateWindowToolbar();
    }

    // enable the remove button if there are at least two workspaces
    ui->actionRemove_current_workspace->setEnabled(workspaces.count() > 1);
}

/**
 * Initializes the panel menu
 */
void MainWindow::initPanelMenu() {
    // update the panel menu if the visibility of a panel was changed
    Q_FOREACH(QDockWidget *dockWidget, findChildren<QDockWidget *>()) {
            QObject::connect(dockWidget, SIGNAL(visibilityChanged(bool)),
                             this, SLOT(updatePanelMenu()));
    }
}

/**
 * Initializes the toolbar menu
 */
void MainWindow::initToolbarMenu() {
    // update the toolbar menu if the visibility of a toolbar was changed
    Q_FOREACH(QToolBar *toolbar, findChildren<QToolBar *>()) {
            // in case the connection was already established
            QObject::disconnect(toolbar, SIGNAL(visibilityChanged(bool)),
                             this, SLOT(updateToolbarMenu()));
            QObject::connect(toolbar, SIGNAL(visibilityChanged(bool)),
                             this, SLOT(updateToolbarMenu()));
    }
}

/**
 * Updates the panel menu entries
 */
void MainWindow::updatePanelMenu() {
    _panelSignalMapper = new QSignalMapper(this);
    ui->menuPanels->clear();

    Q_FOREACH(QDockWidget *dockWidget, findChildren<QDockWidget *>()) {
            QAction *action = new QAction(this);
            action->setText(tr("Show %1 panel").arg(dockWidget->windowTitle()));
            action->setObjectName("togglePanel-" + dockWidget->objectName());
            action->setCheckable(true);
            action->setChecked(!dockWidget->isHidden());

            QObject::connect(action, SIGNAL(triggered()),
                             _panelSignalMapper, SLOT(map()));

            // add a parameter to the signal mapper
            _panelSignalMapper->setMapping(action, dockWidget->objectName());

            ui->menuPanels->addAction(action);
        }

    // toggle the panel if the checkbox was triggered
    QObject::connect(_panelSignalMapper, SIGNAL(mapped(QString)),
                     this, SLOT(togglePanelVisibility(QString)));
}

/**
 * Updates the toolbar menu entries
 */
void MainWindow::updateToolbarMenu() {
    _toolbarSignalMapper = new QSignalMapper(this);
    ui->menuToolbars->clear();

    Q_FOREACH(QToolBar *toolbar, findChildren<QToolBar *>()) {
            QAction *action = new QAction(this);
            action->setText(tr("Show %1").arg(toolbar->windowTitle()));
            action->setObjectName("toggleToolBar-" + toolbar->objectName());
            action->setCheckable(true);
            action->setChecked(!toolbar->isHidden());

            QObject::connect(action, SIGNAL(triggered()),
                             _toolbarSignalMapper, SLOT(map()));

            // add a parameter to the signal mapper
            _toolbarSignalMapper->setMapping(action, toolbar->objectName());

            ui->menuToolbars->addAction(action);
        }

    // toggle the panel if the checkbox was triggered
    QObject::connect(_toolbarSignalMapper, SIGNAL(mapped(QString)),
                     this, SLOT(toggleToolbarVisibility(QString)));
}

/**
 * Toggles the visibility of a panel by object name
 *
 * @param objectName
 */
void MainWindow::togglePanelVisibility(QString objectName) {
    QDockWidget *dockWidget = findChild<QDockWidget *>(objectName);

    if (dockWidget == Q_NULLPTR) {
        return;
    }

    // to prevent crashes if updatePanelMenu removes all actions
    const QSignalBlocker blocker(dockWidget);
    Q_UNUSED(blocker);

    bool newVisibility = dockWidget->isHidden();

    // remember that the user wanted the note subfolder dock widget to be set
    // to visible or invisible
    if (objectName == "noteSubFolderDockWidget") {
        _noteSubFolderDockWidgetVisible = newVisibility;

        // don't allow the note subfolder dock widget to be visible if the
        // note folder has no subfolders activated
        if (newVisibility) {
            newVisibility = NoteFolder::isCurrentShowSubfolders();
        }
    }

    dockWidget->setVisible(newVisibility);
}

/**
 * Toggles the visibility of a toolbar by object name
 *
 * @param objectName
 */
void MainWindow::toggleToolbarVisibility(QString objectName) {
    QToolBar *toolbar = findChild<QToolBar *>(objectName);

    if (toolbar == Q_NULLPTR) {
        return;
    }

    // to prevent crashes if updateToolbarMenu removes all actions
    const QSignalBlocker blocker(toolbar);
    Q_UNUSED(blocker);

    bool newVisibility = toolbar->isHidden();
    toolbar->setVisible(newVisibility);
}

/**
 * Restores the distraction free mode
 */
void MainWindow::restoreDistractionFreeMode() {
    if (isInDistractionFreeMode()) {
        setDistractionFreeMode(true);
    }
}

/**
 * Checks if we are in distraction free mode
 */
bool MainWindow::isInDistractionFreeMode() {
    QSettings settings;
    return settings.value("DistractionFreeMode/isEnabled").toBool();
}

/**
 * Toggles the distraction free mode
 */
void MainWindow::toggleDistractionFreeMode() {
    // leave the one column mode if active
    if (ui->actionUse_one_column_mode->isChecked()) {
        ui->actionUse_one_column_mode->toggle();
    }

    QSettings settings;
    bool isInDistractionFreeMode = this->isInDistractionFreeMode();

    qDebug() << __func__ << " - 'isInDistractionFreeMode': " <<
    isInDistractionFreeMode;

    // store the window settings before we go into distraction free mode
    if (!isInDistractionFreeMode) {
        storeSettings();
    }

    isInDistractionFreeMode = !isInDistractionFreeMode;

    // remember that we were using the distraction free mode
    settings.setValue("DistractionFreeMode/isEnabled",
                      isInDistractionFreeMode);

    setDistractionFreeMode(isInDistractionFreeMode);
}

/**
 * Does some basic styling
 */
void MainWindow::initStyling() {
    QSettings settings;
    bool darkMode = settings.value("darkMode").toBool();
    QString colorName;

    // turn on the dark mode if enabled
    if (darkMode) {
        QFile f(":qdarkstyle/style.qss");
        if (!f.exists()) {
            qWarning("Unable to set stylesheet, file not found!");
        } else {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());
        }

        // QTextEdit background color of qdarkstyle
        colorName = "#201F1F";
    } else {
        QPalette palette;
        QColor color = palette.color(QPalette::Base);
        colorName = color.name();
    }

    QString textEditStyling = QString("QTextEdit {background-color: %1;}")
            .arg(colorName);

    ui->noteTextEdit->setStyleSheet(
            ui->noteTextEdit->styleSheet() + textEditStyling);

    ui->encryptedNoteTextEdit->setStyleSheet(
            ui->encryptedNoteTextEdit->styleSheet() + textEditStyling);

    QString frameStyling = QString("QFrame {background-color: %1;}")
            .arg(colorName);

    ui->noteTagFrame->setStyleSheet(
            ui->noteTagFrame->styleSheet() + frameStyling);

    if (!isInDistractionFreeMode()) {
        ui->noteTextEdit->setPaperMargins(0);
        ui->encryptedNoteTextEdit->setPaperMargins(0);
    }

#ifdef Q_OS_MAC
    // no stylesheets needed for OS X, the margins doesn't work the same there
    ui->tagFrame->setStyleSheet("");
    ui->notesListFrame->setStyleSheet("");
    ui->noteListSubFrame->setStyleSheet("");
    ui->navigationFrame->setStyleSheet("");
    ui->noteEditFrame->setStyleSheet("");
    ui->noteViewFrame->setStyleSheet("");

    // add some margins in OS X to match the styling of the note list
    ui->navigationFrame->setContentsMargins(3, 0, 3, 0);

    // there is no system tray in OS X
    ui->actionShow_system_tray->setText(tr("Show menu bar item"));

    // add a padding for the note tag frame so the `add tag` button doesn't
    // stick to the right corner
    ui->noteTagFrame->setStyleSheet(
            "QFrame {border: none; padding-right: 5px;}");
#endif

    // move the note view scrollbar when the note edit scrollbar was moved
    connect(ui->noteTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(noteTextSliderValueChanged(int)));
    connect(ui->encryptedNoteTextEdit->verticalScrollBar(),
            SIGNAL(valueChanged(int)),
            this, SLOT(noteTextSliderValueChanged(int)));

    // move the note edit scrollbar when the note view scrollbar was moved
    connect(ui->noteTextView->verticalScrollBar(),
            SIGNAL(valueChanged(int)),
            this, SLOT(noteViewSliderValueChanged(int)));

    // hide the combo box if it looses focus if it should not be viewed
    connect(ui->noteFolderComboBox, SIGNAL(focusOut()),
            this, SLOT(hideNoteFolderComboBoxIfNeeded()));
}

/**
 * Moves the note view scrollbar when the note edit scrollbar was moved
 */
void MainWindow::noteTextSliderValueChanged(int value, bool force) {
    // don't react if note text edit doesn't have the focus
    if (!activeNoteTextEdit()->hasFocus() && !force) {
        return;
    }

    QScrollBar *editScrollBar = activeNoteTextEdit()->verticalScrollBar();
    QScrollBar *viewScrollBar = ui->noteTextView->verticalScrollBar();

    float editScrollFactor =
            static_cast<float>(value) / editScrollBar->maximum();
    int viewPosition =
            static_cast<int>(viewScrollBar->maximum() * editScrollFactor);

    // set the scroll position in the note text view
    viewScrollBar->setSliderPosition(viewPosition);
}

/**
 * Moves the note edit scrollbar when the note view scrollbar was moved
 */
void MainWindow::noteViewSliderValueChanged(int value, bool force) {
    // don't react if note text view doesn't have the focus
    if (!ui->noteTextView->hasFocus() && !force) {
        return;
    }

    QScrollBar *editScrollBar = activeNoteTextEdit()->verticalScrollBar();
    QScrollBar *viewScrollBar = ui->noteTextView->verticalScrollBar();

    float editScrollFactor =
            static_cast<float>(value) / viewScrollBar->maximum();

    int editPosition =
            static_cast<int>(editScrollBar->maximum() * editScrollFactor);

    // for some reason we get some int-min value here sometimes
    if (editPosition < 0) {
        return;
    }

    // set the scroll position in the note text edit
    editScrollBar->setSliderPosition(editPosition);
}

/**
 * Enables or disables the distraction free mode
 */
void MainWindow::setDistractionFreeMode(bool enabled) {
    QSettings settings;

    if (enabled) {
        //
        // enter the distraction free mode
        //

        // store the current workspace in case we changed something
        storeCurrentWorkspace();

        // remember states, geometry and sizes
        settings.setValue("DistractionFreeMode/windowState", saveState());
        settings.setValue("DistractionFreeMode/menuBarGeometry",
                          ui->menuBar->saveGeometry());
        settings.setValue("DistractionFreeMode/menuBarHeight",
                          ui->menuBar->height());

        // we must not hide the menu bar or else the shortcuts
        // will not work any more
        ui->menuBar->setFixedHeight(0);

        // hide the toolbars
        QList<QToolBar*> toolbars = findChildren<QToolBar*>();
        Q_FOREACH(QToolBar *toolbar, toolbars) {
                toolbar->hide();
            }

        // show the note edit dock widget
        _noteEditDockWidget->show();

        // hide all dock widgets but the note edit dock widget
        QList<QDockWidget*> dockWidgets = findChildren<QDockWidget*>();
        Q_FOREACH(QDockWidget *dockWidget, dockWidgets) {
                if (dockWidget->objectName() == "noteEditDockWidget") {
                    continue;
                }

                dockWidget->hide();
            }

        // hide the status bar
//        ui->statusBar->hide();

        _leaveDistractionFreeModeButton = new QPushButton(tr("leave"));
        _leaveDistractionFreeModeButton->setFlat(true);
        _leaveDistractionFreeModeButton->setToolTip(
                tr("leave distraction free mode"));
        _leaveDistractionFreeModeButton
                ->setStyleSheet("QPushButton {padding: 0 5px}");

        _leaveDistractionFreeModeButton->setIcon(QIcon::fromTheme(
                "zoom-original",
                QIcon(":icons/breeze-qownnotes/16x16/zoom-original.svg")));

        connect(_leaveDistractionFreeModeButton, SIGNAL(clicked()),
                this, SLOT(toggleDistractionFreeMode()));

        statusBar()->addPermanentWidget(_leaveDistractionFreeModeButton);
    } else {
        //
        // leave the distraction free mode
        //

        statusBar()->removeWidget(_leaveDistractionFreeModeButton);
        disconnect(_leaveDistractionFreeModeButton, 0, 0, 0);

        // restore states and sizes
        restoreState(
                settings.value(
                        "DistractionFreeMode/windowState").toByteArray());
        ui->menuBar->restoreGeometry(
                settings.value(
                        "DistractionFreeMode/menuBarGeometry").toByteArray());
        ui->menuBar->setFixedHeight(
                settings.value("DistractionFreeMode/menuBarHeight").toInt());
    }

    ui->noteTextEdit->setPaperMargins(this->width());
    ui->encryptedNoteTextEdit->setPaperMargins(this->width());
}

/**
 * Sets the distraction free mode if it is currently other than we want it to be
 */
void MainWindow::changeDistractionFreeMode(bool enabled) {
    if (isInDistractionFreeMode() != enabled) {
        setDistractionFreeMode(enabled);
    }
}

/**
 * Shows a status bar message if not in distraction free mode
 */
void MainWindow::showStatusBarMessage(const QString & message, int timeout) {
    if (!isInDistractionFreeMode()) {
        ui->statusBar->showMessage(message, timeout);
    }

    // write to the log widget
    LogWidget::instance()->log(LogWidget::StatusLogType, message);
}

/**
 * Sets the shortcuts for the note bookmarks up
 */
void MainWindow::setupNoteBookmarkShortcuts() {
    this->storeNoteBookmarkSignalMapper = new QSignalMapper(this);
    this->gotoNoteBookmarkSignalMapper = new QSignalMapper(this);

    for (int number = 0; number <= 9; number++) {
        // setup the store shortcut
        QShortcut *storeShortcut = new QShortcut(
                QKeySequence("Ctrl+Shift+" + QString::number(number)),
                this);

        connect(storeShortcut, SIGNAL(activated()),
                storeNoteBookmarkSignalMapper, SLOT(map()));
        storeNoteBookmarkSignalMapper->setMapping(storeShortcut, number);

        // setup the goto shortcut
        QShortcut *gotoShortcut = new QShortcut(
                QKeySequence("Ctrl+" + QString::number(number)),
                this);

        connect(gotoShortcut, SIGNAL(activated()),
                gotoNoteBookmarkSignalMapper, SLOT(map()));
        gotoNoteBookmarkSignalMapper->setMapping(gotoShortcut, number);
    }

    connect(storeNoteBookmarkSignalMapper, SIGNAL(mapped(int)),
            this, SLOT(storeNoteBookmark(int)));

    connect(gotoNoteBookmarkSignalMapper, SIGNAL(mapped(int)),
            this, SLOT(gotoNoteBookmark(int)));
}

/*
 * Loads the menu entries for the note folders
 */
void MainWindow::loadNoteFolderListMenu() {
    // clear menu list
    // we must not do this, because the app might crash if trackAction() is
    // called, because the action was triggered and then removed
//    ui->noteFoldersMenu->clear();

    // find all actions of the recent note folders menu
    QList<QAction*> actions =
            ui->noteFoldersMenu->findChildren<QAction*>();

    // loop through all actions of the recent note folders menu and hide them
    // this is a workaround because the app might crash if trackAction() is
    // called, because the action was triggered and then removed
    int c = 0;
    Q_FOREACH(QAction* action, actions) {
            // start with the 2nd item, the first item is the menu itself
            if (c++ > 0) {
                // hide menu item
                action->setVisible(false);
            }
        }

    QList<NoteFolder> noteFolders = NoteFolder::fetchAll();
    int noteFoldersCount = noteFolders.count();

    const QSignalBlocker blocker(ui->noteFolderComboBox);
    Q_UNUSED(blocker);

    ui->noteFolderComboBox->clear();
    int index = 0;
    int noteFolderComboBoxIndex = 0;

    // populate the note folder list
    if (noteFoldersCount > 0) {
        Q_FOREACH(NoteFolder noteFolder, noteFolders) {
                // don't show not existing folders or if path is empty
                if (!noteFolder.localPathExists()) {
                    continue;
                }

                int folderId = noteFolder.getId();
                const QString &name = noteFolder.getName();

                // add an entry to the combo box
                ui->noteFolderComboBox->addItem(name,
                                                folderId);

                // add a menu entry
                QAction *action =
                        ui->noteFoldersMenu->addAction(name);
                action->setData(folderId);
                action->setToolTip(noteFolder.getLocalPath());
                action->setStatusTip(noteFolder.getLocalPath());
                action->setObjectName(
                        "noteFolder-" + QString::number(folderId));
                action->setIcon(QIcon::fromTheme(
                        "folder",
                        QIcon(":icons/breeze-qownnotes/16x16/folder.svg")));

                if (noteFolder.isCurrent()) {
                    QFont font = action->font();
                    font.setBold(true);
                    action->setFont(font);

                    noteFolderComboBoxIndex = index;
                }

                QObject::connect(
                        action, SIGNAL(triggered()),
                        recentNoteFolderSignalMapper, SLOT(map()));

                // add a parameter to changeNoteFolder with the signal mapper
                recentNoteFolderSignalMapper->setMapping(
                        action, folderId);

                index++;
            }

        QObject::connect(recentNoteFolderSignalMapper,
                         SIGNAL(mapped(int)),
                         this,
                         SLOT(changeNoteFolder(int)));

        // set the current row
        ui->noteFolderComboBox->setCurrentIndex(
                noteFolderComboBoxIndex);
    }
}

/*
 * Set a new note folder
 */
void MainWindow::changeNoteFolder(int noteFolderId, bool forceChange) {
    int currentNoteFolderId = NoteFolder::currentNoteFolderId();

    // store the current note name of the current note folder
    _activeNoteFolderNoteNames[currentNoteFolderId] = currentNote.getName();

    // store the current position in the note of the current note folder
    QTextCursor c = activeNoteTextEdit()->textCursor();
    _activeNoteFolderNotePositions[currentNoteFolderId] = c.position();

    NoteFolder noteFolder = NoteFolder::fetch(noteFolderId);
    if (!noteFolder.isFetched()) {
        return;
    }

    if (noteFolder.isCurrent() && !forceChange) {
        return;
    }

    QString folderName = noteFolder.getLocalPath();
    QString oldPath = this->notesPath;

    // we only want to see that menu entry if there are note subfolders
    ui->actionFind_notes_in_all_subfolders->setVisible(
            noteFolder.isShowSubfolders());

    // reload notes if notes folder was changed
    if (oldPath != folderName) {
        const QSignalBlocker blocker2(this->ui->searchLineEdit);
        {
            Q_UNUSED(blocker2);
            ui->searchLineEdit->clear();
        }

        // store everything before changing folder
        storeUpdatedNotesToDisk();

        noteFolder.setAsCurrent();

        // update the recent note folder list
        storeRecentNoteFolder(this->notesPath, folderName);

        // change notes path
        this->notesPath = folderName;

        // store notesPath setting
        QSettings settings;
        // make the path relative to the portable data path if we are in
        // portable mode
        settings.setValue(
                "notesPath",
                Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                                  folderName));

        // we have to unset the current note otherwise it might show up after
        // switching to an other note folder
        currentNote = Note();

        buildNotesIndexAndLoadNoteDirectoryList();

        const QSignalBlocker blocker(this->ui->noteTextEdit);
        {
            Q_UNUSED(blocker);
            ui->noteTextEdit->clear();
            ui->noteTextEdit->show();
            ui->encryptedNoteTextEdit->hide();
        }

        this->ui->noteTextView->clear();

        // update the current folder tooltip
        updateCurrentFolderTooltip();

        // clear the note history
        this->noteHistory.clear();

        // check if there is a note name set and jump to it
        QString noteName = _activeNoteFolderNoteNames[noteFolderId];
        if (!noteName.isEmpty()) {
            jumpToNoteName(noteName);

            // restore the current position in the note
            QMarkdownTextEdit *textEdit = activeNoteTextEdit();
            QTextCursor c = textEdit->textCursor();
            c.setPosition(_activeNoteFolderNotePositions[noteFolderId]);
            textEdit->setTextCursor(c);
        }
    }
}

/*
 * Adds and removes a folder from the recent note folders
 */
void MainWindow::storeRecentNoteFolder(
        QString addFolderName,
        QString removeFolderName) {
    QSettings settings;
    QStringList recentNoteFolders =
            settings.value("recentNoteFolders").toStringList();

    recentNoteFolders.removeAll(addFolderName);
    recentNoteFolders.removeAll(removeFolderName);

    // remove empty paths
    recentNoteFolders.removeAll("");

    if (addFolderName != removeFolderName) {
        recentNoteFolders.prepend(addFolderName);
    }

    settings.setValue("recentNoteFolders", recentNoteFolders);
    // reload menu
    loadNoteFolderListMenu();
}

int MainWindow::openNoteDiffDialog(Note changedNote) {
    if (this->noteDiffDialog->isVisible()) {
        this->noteDiffDialog->close();
    }

    // if we should ignore all changes return here
    QSettings settings;
    if (settings.value("ignoreAllExternalModifications").toBool()) {
        return NoteDiffDialog::Ignore;
    }

    QString text1 = this->ui->noteTextEdit->toPlainText();

    changedNote.updateNoteTextFromDisk();
    QString text2 = changedNote.getNoteText();

//    qDebug() << __func__ << " - 'text1': " << text1;
//    qDebug() << __func__ << " - 'text2': " << text2;

    diff_match_patch *diff = new diff_match_patch();
    QList<Diff> diffList = diff->diff_main(text1, text2);

    QString html = diff->diff_prettyHtml(diffList);
//    qDebug() << __func__ << " - 'html': " << html;

    this->noteDiffDialog = new NoteDiffDialog(this, html);
    this->noteDiffDialog->exec();

    int result = this->noteDiffDialog->resultActionRole();
    return result;
}

void MainWindow::createSystemTrayIcon() {
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(getSystemTrayIcon());

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(
                    systemTrayIconClicked(QSystemTrayIcon::ActivationReason)));

    if (showSystemTray) {
        trayIcon->show();
    }
}

/**
 * Returns a proper system tray icon
 *
 * @return
 */
QIcon MainWindow::getSystemTrayIcon() {
    QSettings settings;
    QString fileName = ":/images/icon";

    if (settings.value("darkModeTrayIcon").toBool()) {
        fileName += "-dark";
    }

    fileName += ".png";
    return QIcon(fileName);
}

/**
 * Creates the items in the note tree widget from the note and note sub
 * folder tables
 */
void MainWindow::loadNoteDirectoryList() {
    const QSignalBlocker blocker(ui->noteTextEdit);
    Q_UNUSED(blocker);

    const QSignalBlocker blocker2(ui->noteTreeWidget);
    Q_UNUSED(blocker2);

    ui->noteTreeWidget->clear();

    // load all notes and add them to the note list widget
    QList<Note> noteList = Note::fetchAll();
    Q_FOREACH(Note note, noteList) {
            addNoteToNoteTreeWidget(note);
        }

    // clear the text edits if there are no notes
    if (noteList.isEmpty()) {
        ui->noteTextEdit->clear();
        ui->noteTextView->clear();
    }

    int itemCount = noteList.count();
    MetricsService::instance()->sendEventIfEnabled(
            "note/list/loaded",
            "note",
            "note list loaded",
            QString::number(itemCount) + " notes",
            itemCount);

    // sort alphabetically again if necessary
    if (sortAlphabetically) {
        ui->noteTreeWidget->sortItems(0, Qt::AscendingOrder);
    }

    // setup tagging
    setupTags();

    // setup note sub folders
    setupNoteSubFolders();

    // generate the tray context menu
    generateSystemTrayContextMenu();
}

/**
 * Adds a note to the note tree widget
 */
bool MainWindow::addNoteToNoteTreeWidget(Note note) {
    QString name = note.getName();

    // skip notes without name
    if (name.isEmpty()) {
        return false;
    }

    // add a note item to the tree
    QTreeWidgetItem *noteItem = new QTreeWidgetItem();
    setTreeWidgetItemToolTipForNote(noteItem, &note);
    noteItem->setText(0, name);
    noteItem->setData(0, Qt::UserRole, note.getId());
    noteItem->setIcon(0, QIcon::fromTheme(
                    "text-x-generic",
                    QIcon(":icons/breeze-qownnotes/16x16/"
                                  "text-x-generic.svg")));

    Tag tag = Tag::fetchOneOfNoteWithColor(note);
    if (tag.isFetched()) {
        // set the color of the note tree widget item
        handleTreeWidgetItemTagColor(noteItem, tag);
    }

    bool isEditable = Note::allowDifferentFileName();
    if (isEditable) {
        noteItem->setFlags(noteItem->flags() | Qt::ItemIsEditable);
    }

    const QSignalBlocker blocker(ui->noteTreeWidget);
    Q_UNUSED(blocker);

    // strange things happen if we insert with insertTopLevelItem
    ui->noteTreeWidget->addTopLevelItem(noteItem);

//    if (sortAlphabetically) {
//        ui->noteTreeWidget->addTopLevelItem(noteItem);
//    } else {
//        ui->noteTreeWidget->insertTopLevelItem(0, noteItem);
//    }

    return true;
}

/**
 * Adds a note sub folder to the note sub folder tree widget
 */
QTreeWidgetItem *MainWindow::addNoteSubFolderToTreeWidget(
        QTreeWidgetItem *parentItem, NoteSubFolder noteSubFolder) {
    int id = noteSubFolder.getId();
    QString name = noteSubFolder.getName();
    int linkCount = Note::countByNoteSubFolderId(id);
    QString toolTip = tr("show notes in folder '%1' (%2)")
            .arg(name, QString::number(linkCount));

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setData(0, Qt::UserRole, id);
    item->setToolTip(0, toolTip);
    item->setIcon(0, QIcon::fromTheme(
                                "folder",
                                QIcon(":icons/breeze-qownnotes/16x16/"
                                              "folder.svg")));
    item->setTextColor(1, QColor(Qt::gray));
    item->setText(1, QString::number(linkCount));
    item->setToolTip(1, toolTip);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    if (parentItem == NULL) {
        ui->noteSubFolderTreeWidget->addTopLevelItem(item);
    } else {
        parentItem->addChild(item);
    }

    return item;
}

/**
 * Sets the tree widget tooltip for a note
 */
void MainWindow::setTreeWidgetItemToolTipForNote(
        QTreeWidgetItem *item,
        Note *note,
        QDateTime *overrideFileLastModified) {
    if ((item == NULL) || (note == NULL)) {
        return;
    }

    QDateTime modified = note->getFileLastModified();
    QDateTime *fileLastModified = (overrideFileLastModified != NULL) ?
                                 overrideFileLastModified : &modified;

    QString toolTipText = tr("<strong>%1</strong><br />last modified: %2")
            .arg(note->getName(), fileLastModified->toString());

    NoteSubFolder noteSubFolder = note->getNoteSubFolder();
    if (noteSubFolder.isFetched()) {
        toolTipText += tr("<br />path: %1").arg(noteSubFolder.relativePath());
    }

    item->setToolTip(0, toolTipText);
}

/**
 * @brief makes the current note the first item in the note list without reloading the whole list
 */
void MainWindow::makeCurrentNoteFirstInNoteList() {
    QTreeWidgetItem *item = findNoteInNoteTreeWidget(currentNote);

    if (item != Q_NULLPTR) {
        const QSignalBlocker blocker(ui->noteTreeWidget);
        Q_UNUSED(blocker);

        ui->noteTreeWidget->takeTopLevelItem(
                ui->noteTreeWidget->indexOfTopLevelItem(item));
        ui->noteTreeWidget->insertTopLevelItem(0, item);

        bool isInActiveNoteSubFolder =
                NoteSubFolder::activeNoteSubFolderId() ==
                        currentNote.getNoteSubFolderId();

        if (!isInActiveNoteSubFolder) {
            item->setHidden(true);
        } else {
            ui->noteTreeWidget->setCurrentItem(item);
        }
    }
}

/**
 * Finds a note in the note tree widget and returns its item
 *
 * @param note
 * @return
 */
QTreeWidgetItem *MainWindow::findNoteInNoteTreeWidget(Note note) {
    int noteId = note.getId();

    for (int i = 0; i < ui->noteTreeWidget->topLevelItemCount(); i++) {
        QTreeWidgetItem *item = ui->noteTreeWidget->topLevelItem(i);
        if (item->data(0, Qt::UserRole).toInt() == noteId) {
            return item;
        }
    }

    return Q_NULLPTR;
}

void MainWindow::readSettings() {
    NoteFolder::migrateToNoteFolders();

    QSettings settings;
    sortAlphabetically = settings.value(
            "SortingModeAlphabetically", false).toBool();
    showSystemTray = settings.value("ShowSystemTray", false).toBool();
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    ui->menuBar->restoreGeometry(
            settings.value("MainWindow/menuBarGeometry").toByteArray());

    // read all relevant settings, that can be set in the settings dialog
    readSettingsFromSettingsDialog();

    // get the notes path
    // prepend the portable data path if we are in portable mode
    this->notesPath = Utils::Misc::prependPortableDataPathIfNeeded(
            settings.value("notesPath").toString());

    // migration: remove GAnalytics-cid
    if (!settings.value("GAnalytics-cid").toString().isEmpty()) {
        settings.remove("GAnalytics-cid");
    }

    // let us select a folder if we haven't find one in the settings
    if (this->notesPath == "") {
        selectOwnCloudNotesFolder();
    }

    // migration: remove notes path from recent note folders
    if (this->notesPath != "") {
        QStringList recentNoteFolders =
                settings.value("recentNoteFolders").toStringList();
        if (recentNoteFolders.contains(this->notesPath)) {
            recentNoteFolders.removeAll(this->notesPath);
            settings.setValue("recentNoteFolders", recentNoteFolders);
        }
    }

    // set the editor width selector for the distraction free mode
    int editorWidthMode =
            settings.value("DistractionFreeMode/editorWidthMode").toInt();

    switch (editorWidthMode) {
        case QOwnNotesMarkdownTextEdit::Medium:
            ui->actionEditorWidthMedium->setChecked(true);
            break;
        case QOwnNotesMarkdownTextEdit::Wide:
            ui->actionEditorWidthWide->setChecked(true);
            break;
        case QOwnNotesMarkdownTextEdit::Full:
            ui->actionEditorWidthFull->setChecked(true);
            break;
        default:
        case QOwnNotesMarkdownTextEdit::Narrow:
            ui->actionEditorWidthNarrow->setChecked(true);
            break;
    }

    // toggle the show status bar checkbox
    bool showStatusBar = settings.value("showStatusBar", true).toBool();
    on_actionShow_status_bar_triggered(showStatusBar);

    // toggle the show menu bar checkbox
    // use the current menu bar visibility as default (so it will not be
    // shown by default on Unity desktop)
    bool showMenuBar =
            settings.value("showMenuBar", !ui->menuBar->isHidden()).toBool();
    on_actionShow_menu_bar_triggered(showMenuBar);
}

/**
 * Restores the toolbars
 */
void MainWindow::restoreToolbars() {
    QSettings settings;
    QList<ToolbarContainer> toolbarContainers;
    int toolbarCount = settings.beginReadArray("toolbar");

    for (int i = 0; i < toolbarCount; i++) {
        settings.setArrayIndex(i);

        ToolbarContainer toolbarContainer;

        toolbarContainer.name = settings.value("name").toString();
        if (toolbarContainer.name.isEmpty()) {
            qWarning() << tr("Toolbar could not be loaded without name");
            continue;
        }

        toolbarContainer.title = settings.value("title").toString();
        toolbarContainer.actions = settings.value("items").toStringList();

        toolbarContainers.push_back(toolbarContainer);
    }

    settings.endArray();

    if (!toolbarContainers.empty()) {
        // delete the custom toolbars
        foreach(QToolBar* toolbar, findChildren<QToolBar*>() ) {
                if (!toolbar->objectName().startsWith(
                        Toolbar_Editor::customToolbarNamePrefix)) {
                    continue;
                }

                delete toolbar;
            }

        foreach(ToolbarContainer toolbarContainer, toolbarContainers) {
                if (toolbarContainer.toolbarFound()) {
                    toolbarContainer.updateToolbar();
                } else {
                    toolbarContainer.create(this);
                }
            }
    }

    // initialize the toolbar menu
    initToolbarMenu();

    // update the toolbar menu
    updateToolbarMenu();
}

/**
 * @brief Reads all relevant settings, that can be set in the settings dialog
 */
void MainWindow::readSettingsFromSettingsDialog() {
    QSettings settings;

    // disable the automatic update dialog per default for repositories and
    // self-builds
    if (settings.value("disableAutomaticUpdateDialog").toString().isEmpty()) {
        QString release = qApp->property("release").toString();
        bool enabled =
                release.contains("Travis") || release.contains("AppVeyor");
        settings.setValue("disableAutomaticUpdateDialog", !enabled);
    }

    this->notifyAllExternalModifications =
            settings.value("notifyAllExternalModifications").toBool();
    this->noteSaveIntervalTime =
            settings.value("noteSaveIntervalTime", 10).toInt();

    // default value is 10 seconds
    if (this->noteSaveIntervalTime == 0) {
        this->noteSaveIntervalTime = 10;
        settings.setValue("noteSaveIntervalTime", this->noteSaveIntervalTime);
    }

    // set the note text edit font
    ui->noteTextEdit->setStyles();
    ui->encryptedNoteTextEdit->setStyles();

    // load note text view font
    QString fontString = settings.value("MainWindow/noteTextView.font")
            .toString();

    // store the current font if there isn't any set yet
    if (fontString == "") {
        fontString = ui->noteTextView->font().toString();
        settings.setValue("MainWindow/noteTextView.font", fontString);
    }

    // set the note text view font
    QFont font;
    font.fromString(fontString);
    ui->noteTextView->setFont(font);

    // set the main toolbar icon size
    int toolBarIconSize = settings.value(
            "MainWindow/mainToolBar.iconSize").toInt();
    if (toolBarIconSize == 0) {
        toolBarIconSize = ui->mainToolBar->iconSize().height();
        settings.setValue("MainWindow/mainToolBar.iconSize", toolBarIconSize);
    } else {
        QSize size(toolBarIconSize, toolBarIconSize);
        ui->mainToolBar->setIconSize(size);
        _formattingToolbar->setIconSize(size);
        _customActionToolbar->setIconSize(size);
        _insertingToolbar->setIconSize(size);
        _encryptionToolbar->setIconSize(size);
        _windowToolbar->setIconSize(size);
        _quitToolbar->setIconSize(size);
    }

    // change the search notes symbol between dark and light mode
    QString fileName = settings.value("darkModeColors").toBool() ?
                       "search-notes-dark.svg" : "search-notes.svg";
    QString styleSheet = ui->searchLineEdit->styleSheet();
    styleSheet.replace(
            QRegularExpression("background-image: url\\(:.+\\);"),
            QString("background-image: url(:/images/%1);").arg(fileName));
    ui->searchLineEdit->setStyleSheet(styleSheet);

    // initialize the shortcuts for the actions
    initShortcuts();

    // initialize the item height of the tree widgets
    initTreeWidgetItemHeight();

    // we need to initialize the toolbar menu again in case there are new
    // toolbars
    initToolbarMenu();

    // update the toolbar menu
    updateToolbarMenu();
}

/**
 * Initializes the item height of the tree widgets
 */
void MainWindow::initTreeWidgetItemHeight() {
    QSettings settings;
    int height = settings.value("itemHeight").toInt();

    // if the height was 0 set it the the current height of a tree widget item
    if (height == 0) {
        QTreeWidget treeWidget(this);
        QTreeWidgetItem *treeWidgetItem = new QTreeWidgetItem();
        treeWidget.addTopLevelItem(treeWidgetItem);
        height = treeWidget.visualItemRect(treeWidgetItem).height();
        settings.setValue("itemHeight", height);
    }

    updateTreeWidgetItemHeight(ui->tagTreeWidget, height);
    updateTreeWidgetItemHeight(ui->noteTreeWidget, height);
    updateTreeWidgetItemHeight(ui->noteSubFolderTreeWidget, height);
    updateTreeWidgetItemHeight(ui->navigationWidget, height);
}

/**
 * Sets height of the items of a tree widget
 *
 * @param treeWidget
 * @param height
 */
void MainWindow::updateTreeWidgetItemHeight(QTreeWidget *treeWidget,
                                            int height) {
    QString styleText = treeWidget->styleSheet();

    // remove the old height stylesheet
    styleText.remove(QRegularExpression(
            "\nQTreeWidget::item \\{height: \\d+px\\}",
            QRegularExpression::CaseInsensitiveOption));

    // add the new height stylesheet
    styleText += QString("\nQTreeWidget::item {height: %1px}")
            .arg(QString::number(height));

    treeWidget->setStyleSheet(styleText);
}

void MainWindow::updateNoteTextFromDisk(Note note) {
    note.updateNoteTextFromDisk();
    note.store();
    this->currentNote = note;
    updateEncryptNoteButtons();

    {
        const QSignalBlocker blocker(this->ui->noteTextEdit);
        Q_UNUSED(blocker);
        this->setNoteTextFromNote(&note);
    }

    ScriptingService::instance()->onCurrentNoteChanged(&currentNote);
}

void MainWindow::notesWereModified(const QString &str) {
    qDebug() << "notesWereModified: " << str;

    QFileInfo fi(str);
    Note note = Note::fetchByFileName(fi.fileName());

    // load note from disk if current note was changed
    if (note.getFileName() == this->currentNote.getFileName()) {
        if (note.fileExists()) {
            // fetch current text
            QString text1 = this->ui->noteTextEdit->toPlainText();

            // fetch text of note from disk
            note.updateNoteTextFromDisk();
            QString text2 = note.getNoteText();

            // skip dialog if texts are equal
            if (text1 == text2) {
                return;
            }

            qDebug() << "Current note was modified externally!";

            showStatusBarMessage(
                    tr("Current note was modified externally"), 5000);

            // if we don't want to get notifications at all
            // external modifications check if we really need one
            if (!this->notifyAllExternalModifications) {
                bool isCurrentNoteNotEditedForAWhile =
                        this->currentNoteLastEdited.addSecs(60)
                        < QDateTime::currentDateTime();

                // reloading the current note text straight away
                // if we didn't change it for a minute
                if (!this->currentNote.getHasDirtyData()
                    && isCurrentNoteNotEditedForAWhile) {
                    updateNoteTextFromDisk(note);
                    return;
                }
            }

            int result = openNoteDiffDialog(note);
            switch (result) {
                // overwrite file with local changes
                case NoteDiffDialog::Overwrite: {
                    const QSignalBlocker blocker(this->noteDirectoryWatcher);
                    Q_UNUSED(blocker);

                    showStatusBarMessage(
                            tr("Overwriting external changes of: %1").arg(
                                    currentNote.getFileName()), 3000);

                    // the note text has to be stored newly because the
                    // external change is already in the note table entry
                    currentNote.storeNewText(
                            ui->noteTextEdit->toPlainText());
                    currentNote.storeNoteTextFileToDisk();

                    // just to make sure everything is up-to-date
//                        this->currentNote = note;
//                        this->setNoteTextFromNote( &note, true );

                    // wait 100ms before the block on this->noteDirectoryWatcher
                    // is opened, otherwise we get the event
                    Utils::Misc::waitMsecs(100);
                }
                    break;

                // reload note file from disk
                case NoteDiffDialog::Reload:
                    showStatusBarMessage(
                            tr("Loading external changes from: %1").arg(
                                    currentNote.getFileName()), 3000);
                    updateNoteTextFromDisk(note);
                    break;

//                case NoteDiffDialog::Cancel:
//                case NoteDiffDialog::Ignore:
                default:
                    // do nothing
                    break;
            }
        } else if (currentNote.getNoteSubFolderId() == 0) {
            // only allow the check if current note was removed externally in
            // the root note folder, because it gets triggered every time
            // a note gets renamed in subfolders

            qDebug() << "Current note was removed externally!";

            switch (QMessageBox::information(
                    this, tr("Note was removed externally!"),
                    tr("Current note was removed outside of this application!\n"
                            "Restore current note?"),
                     tr("&Restore"), tr("&Cancel"), QString::null,
                                             0, 1)) {
                case 0: {
                    const QSignalBlocker blocker(this->noteDirectoryWatcher);
                    Q_UNUSED(blocker);

                    QString text = this->ui->noteTextEdit->toPlainText();
                    note.storeNewText(text);

                    // store note to disk again
                    note.storeNoteTextFileToDisk();
                    showStatusBarMessage(
                            tr("Stored current note to disk"), 3000);

                    // rebuild and reload the notes directory list
                    buildNotesIndexAndLoadNoteDirectoryList();

                    // fetch note new (because all the IDs have changed
                    // after the buildNotesIndex()
                    note.refetch();

                    // restore old selected row (but don't update the note text)
                    setCurrentNote(note, false);
                }
                    break;
                case 1:
                default:
                    break;
            }
        }
    } else {
        qDebug() << "other note was changed: " << str;

        showStatusBarMessage(
                tr("Note was modified externally: %1").arg(str), 5000);

        // rebuild and reload the notes directory list
        buildNotesIndexAndLoadNoteDirectoryList();
        setCurrentNote(this->currentNote, false);
    }
}

void MainWindow::notesDirectoryWasModified(const QString &str) {
    // workaround when signal block doesn't work correctly
    if (_isNotesDirectoryWasModifiedDisabled) {
        return;
    }

    // if we should ignore all changes return here
    QSettings settings;
    if (settings.value("ignoreAllExternalNoteFolderChanges").toBool()) {
        return;
    }

    qDebug() << "notesDirectoryWasModified: " << str;
    showStatusBarMessage(tr("Notes directory was modified externally"), 5000);

    // rebuild and reload the notes directory list
    buildNotesIndexAndLoadNoteDirectoryList();

#ifdef Q_OS_LINUX
    // check if the current note was modified
    // this is a small remedy for the problem of not detected external note
    // changes
    QString noteFileName = currentNote.getFileName();
    if (!noteFileName.isEmpty()) {
        notesWereModified(currentNote.getFileName());
    }
#endif

    // also update the text of the text edit if current note has changed
    bool updateNoteText = !this->currentNote.exists();
    qDebug() << "updateNoteText: " << updateNoteText;

    // restore old selected row (but don't update the note text)
    setCurrentNote(this->currentNote, updateNoteText);
}

/**
 * Checks if the note view needs an update because the text has changed
 */
void MainWindow::noteViewUpdateTimerSlot() {
    if (_noteViewNeedsUpdate) {
        if (isMarkdownViewEnabled()) {
            setNoteTextFromNote(&currentNote, true);
        }
        _noteViewNeedsUpdate = false;
    }
}

void MainWindow::storeUpdatedNotesToDisk() {
    const QSignalBlocker blocker(noteDirectoryWatcher);
    Q_UNUSED(blocker);

    QString oldNoteName = currentNote.getName();

    // For some reason this->noteDirectoryWatcher gets an event from this.
    // I didn't find an other solution than to wait yet.
    // All flushing and syncing didn't help.
    bool currentNoteChanged = false;
    bool noteWasRenamed = false;

    // currentNote will be set by this method if the filename has changed
    int count = Note::storeDirtyNotesToDisk(currentNote,
                                            &currentNoteChanged,
                                            &noteWasRenamed);

    if (count > 0) {
        _noteViewNeedsUpdate = true;

        MetricsService::instance()
                ->sendEventIfEnabled(
                        "note/notes/stored",
                        "note",
                        "notes stored",
                        QString::number(count) + " notes",
                        count);

        qDebug() << __func__ << " - 'count': " << count;

        showStatusBarMessage(
                tr("Stored %n note(s) to disk", "", count),
                3000);

        // wait 100ms before the block on this->noteDirectoryWatcher
        // is opened, otherwise we get the event
        Utils::Misc::waitMsecs(100);

        if (currentNoteChanged) {
            // just to make sure everything is up-to-date
            currentNote.refetch();

            if (oldNoteName != currentNote.getName()) {
                // just to make sure the window title is set correctly
                updateWindowTitle();
            }
        }

        if (noteWasRenamed) {
            // reload the directory list if note name has changed
            loadNoteDirectoryList();
        }
    }
}

/**
 * Shows alerts for calendar items with an alarm date in the current minute
 * Also checks for expired note crypto keys
 */
void MainWindow::frequentPeriodicChecker() {
    CalendarItem::alertTodoReminders();
    Note::expireCryptoKeys();
    MetricsService::instance()->sendHeartbeat();

    QSettings settings;
    QDateTime lastUpdateCheck = settings.value("LastUpdateCheck").toDateTime();
    if (!lastUpdateCheck.isValid()) {
        // set the LastUpdateCheck if it wasn't set
        settings.setValue("LastUpdateCheck", QDateTime::currentDateTime());
    } else if (lastUpdateCheck.addSecs(3600) <= QDateTime::currentDateTime()) {
        // check for updates every 1h
        updateService->checkForUpdates(this, UpdateService::Periodic);
    }
}

/**
 * Does the setup for the update available button
 */
void MainWindow::setupUpdateAvailableButton() {
    _updateAvailableButton = new QPushButton(this);
    _updateAvailableButton->setFlat(true);
    _updateAvailableButton->setToolTip(
            tr("click here to see what has changed and to be able to "
                       "download the latest version"));
    _updateAvailableButton->hide();
    _updateAvailableButton->setStyleSheet("QPushButton {padding: 0 5px}");

    QObject::connect(
            _updateAvailableButton,
            SIGNAL(pressed()),
            this,
            SLOT(on_actionCheck_for_updates_triggered()));

    ui->statusBar->addPermanentWidget(_updateAvailableButton);
}

void MainWindow::showUpdateAvailableButton(QString version) {
    _updateAvailableButton->setText(
            tr("new version %1 available").arg(version));
    _updateAvailableButton->show();
}

void MainWindow::hideUpdateAvailableButton() {
    _updateAvailableButton->hide();
}

/**
 * Builds the index of notes and note sub folders
 */
bool MainWindow::buildNotesIndex(int noteSubFolderId, bool forceRebuild) {
    QString notePath = Utils::Misc::removeIfEndsWith(
            this->notesPath, QDir::separator());
    NoteSubFolder noteSubFolder;
    bool hasNoteSubFolder = false;
    bool wasModified = false;

//    qDebug() << __func__ << " - 'noteSubFolderId': " << noteSubFolderId;

    if (noteSubFolderId == 0) {
        // make sure we destroy nothing
        storeUpdatedNotesToDisk();

        // init the lists to check for removed items
        _buildNotesIndexBeforeNoteIdList = Note::fetchAllIds();
        _buildNotesIndexBeforeNoteSubFolderIdList =
                NoteSubFolder::fetchAllIds();
        _buildNotesIndexAfterNoteIdList.clear();
        _buildNotesIndexAfterNoteSubFolderIdList.clear();
    } else {
        noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);
        hasNoteSubFolder = noteSubFolder.isFetched();

        if (!hasNoteSubFolder) {
            return false;
        }

        notePath += QDir::separator() + noteSubFolder.relativePath();
    }

//    qDebug() << __func__ << " - 'notePath': " << notePath;

    QDir notesDir(notePath);

    // only show markdown and text files
    QStringList filters;
    filters << "*.txt" << "*.md";

    // append the custom extensions
    filters.append(Note::customNoteFileExtensionList("*."));

    // show newest entry first
    QStringList files = notesDir.entryList(filters, QDir::Files, QDir::Time);
//    qDebug() << __func__ << " - 'files': " << files;

    bool createDemoNotes = (files.count() == 0) && !hasNoteSubFolder;

    if (createDemoNotes) {
        QSettings settings;
        // check if we already have created the demo notes once
        createDemoNotes = !settings.value("demoNotesCreated").toBool();

        if (createDemoNotes) {
            // we don't want to create the demo notes again
            settings.setValue("demoNotesCreated", true);
        }
    }

    // add some notes if there aren't any and
    // we haven't already created them once
    if (createDemoNotes) {
        qDebug() << "No notes! We will add some...";
        QStringList filenames = QStringList() <<
                "Markdown Showcase.md" <<
                "Markdown Cheatsheet.md" <<
                "Welcome to QOwnNotes.md";
        QString filename;
        QString destinationFile;

        // copy note files to the notes path
        for (int i = 0; i < filenames.size(); ++i) {
            filename = filenames.at(i);
            destinationFile = this->notesPath + QDir::separator() + filename;
            QFile sourceFile(":/demonotes/" + filename);
            sourceFile.copy(destinationFile);
            // set read/write permissions for the owner and user
            QFile::setPermissions(destinationFile,
                                  QFile::ReadOwner | QFile::WriteOwner |
                                          QFile::ReadUser | QFile::WriteUser);
        }

        // copy the shortcuts file and handle its file permissions
//        destinationFile = this->notesPath + QDir::separator() +
//              "Important Shortcuts.txt";
//        QFile::copy( ":/shortcuts", destinationFile );
//        QFile::setPermissions( destinationFile, QFile::ReadOwner |
//                  QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser );

        // fetch all files again
        files = notesDir.entryList(filters, QDir::Files, QDir::Time);

        // jump to the welcome note in the note selector in 500ms
        QTimer::singleShot(500, this, SLOT(jumpToWelcomeNote()));
    }

    // get the current crypto key to set it again
    // after all notes were read again
    qint64 cryptoKey = currentNote.getCryptoKey();
    QString cryptoPassword = currentNote.getCryptoPassword();

    if (!hasNoteSubFolder && forceRebuild) {
        // first delete all notes and note sub folders in the database if a
        // rebuild was forced
        Note::deleteAll();
        NoteSubFolder::deleteAll();
    }

    // create all notes from the files
    Q_FOREACH(QString fileName, files) {
            if (hasNoteSubFolder) {
                fileName.prepend(noteSubFolder.relativePath() +
                                         QDir::separator());
            }

            // fetching the content of the file
            QFile file(Note::getFullNoteFilePathForFile(fileName));

            // update or create a note from the file
            Note note = Note::updateOrCreateFromFile(file, noteSubFolder);

            // add the note id to in the end check if notes need to be removed
            _buildNotesIndexAfterNoteIdList << note.getId();

            if (!_buildNotesIndexBeforeNoteIdList.contains(note.getId())) {
                wasModified = true;
            }

            // update the UI
            // this causes to show notes twice in the ui->noteTreeWidget if a
            // not selected note is modified externally
            // https://github.com/pbek/QOwnNotes/issues/242
            // using a blocker on noteTreeWidget or just processing every 10th
            // time doesn't work neither
//            QCoreApplication::processEvents();

            // we try these two instead to update the UI
            QCoreApplication::flush();

            // this still causes double entries on OS X and maybe Windows
#ifdef Q_OS_LINUX
            QCoreApplication::sendPostedEvents();
#endif
        }

    // update the UI and get user input after all the notes were loaded
    // this still can cause duplicate note subfolders to be viewed
//    QCoreApplication::processEvents();

    // re-fetch current note (because all the IDs have changed after the
    // buildNotesIndex()
    currentNote.refetch();

    if (cryptoKey != 0) {
        // reset the old crypto key for the current note
        currentNote.setCryptoKey(cryptoKey);
        currentNote.setCryptoPassword(cryptoPassword);
        currentNote.store();
    }

    // build the note sub folders
    bool showSubfolders = NoteFolder::isCurrentShowSubfolders();
    if (showSubfolders) {
        QStringList folders = notesDir.entryList(
                QStringList("*"), QDir::Dirs, QDir::Time);

        // ignore some folders
        QStringList ignoreFolderList;
        ignoreFolderList << "." << ".." << "media";

        Q_FOREACH(QString folder, folders) {
                if (ignoreFolderList.contains(folder)) {
                    continue;
                }

                // fetch or create the parent note sub folder
                NoteSubFolder parentNoteSubFolder =
                        NoteSubFolder::fetchByNameAndParentId(folder,
                                                              noteSubFolderId);
                if (!parentNoteSubFolder.isFetched()) {
                    parentNoteSubFolder.setName(folder);
                    parentNoteSubFolder.setParentId(noteSubFolderId);
                    parentNoteSubFolder.store();

                    wasModified = true;
                }

                if (parentNoteSubFolder.isFetched()) {
                    // add the note id to in the end check if notes need to
                    // be removed
                    _buildNotesIndexAfterNoteSubFolderIdList
                            << parentNoteSubFolder.getId();

                    // build the notes index for the note subfolder
                    bool result = buildNotesIndex(parentNoteSubFolder.getId());
                    if (result) {
                        wasModified = true;
                    }

                    // update the UI
                    // this causes to show sub note folders twice in the
                    // ui->noteSubFolderTreeWidget if a
                    // not selected note is modified externally
//                    QCoreApplication::processEvents();

                    // we try these two instead to update the UI
                    QCoreApplication::flush();

                    // this still causes double entries on OS X and maybe
                    // Windows
#ifdef Q_OS_LINUX
                    QCoreApplication::sendPostedEvents();
#endif
                }
            }
    }

    if (!hasNoteSubFolder) {
        // check for removed notes
        QList<int> removedNoteIdList = _buildNotesIndexBeforeNoteIdList.toSet()
                .subtract(_buildNotesIndexAfterNoteIdList.toSet()).toList();

        // remove all missing notes
        Q_FOREACH(int noteId, removedNoteIdList) {
                Note note = Note::fetch(noteId);
                if (note.isFetched()) {
                    note.remove();
                    wasModified = true;
                }
            }

        // check for removed note subfolders
        QList<int> removedNoteSubFolderIdList =
                _buildNotesIndexBeforeNoteSubFolderIdList.toSet()
                .subtract(_buildNotesIndexAfterNoteSubFolderIdList.toSet())
                        .toList();

        // remove all missing note subfolders
        Q_FOREACH(int noteSubFolderId, removedNoteSubFolderIdList) {
                NoteSubFolder noteSubFolder = NoteSubFolder::fetch(
                        noteSubFolderId);
                if (noteSubFolder.isFetched()) {
                    noteSubFolder.remove();
                    wasModified = true;
                }
            }

        // setup the note folder database
        DatabaseService::createNoteFolderConnection();
        DatabaseService::setupNoteFolderTables();

        // update the note directory watcher
        updateNoteDirectoryWatcher();

        // update the information about shared notes
        OwnCloudService *ownCloud = new OwnCloudService(this);
        ownCloud->fetchShares();
    }

    return wasModified;
}

/**
 * Updates the note directory watcher
 */
void MainWindow::updateNoteDirectoryWatcher() {
    // clear all paths from the directory watcher
    QStringList fileList = noteDirectoryWatcher.directories() +
                           noteDirectoryWatcher.files();
    if (fileList.count() > 0) {
        noteDirectoryWatcher.removePaths(fileList);
    }

    bool showSubfolders = NoteFolder::isCurrentShowSubfolders();
//    if (showSubfolders) {
//        return;
//    }

    QString notePath = Utils::Misc::removeIfEndsWith(
            this->notesPath, QDir::separator());

    QDir notesDir(notePath);

    if (notesDir.exists()) {
        // watch the notes directory for changes
        noteDirectoryWatcher.addPath(notePath);
    }

    if (showSubfolders) {
        QList<NoteSubFolder> noteSubFolderList = NoteSubFolder::fetchAll();
        Q_FOREACH(NoteSubFolder noteSubFolder, noteSubFolderList) {
                QString path = notePath + QDir::separator() +
                        noteSubFolder.relativePath();

                QDir folderDir(path);

                if (folderDir.exists()) {
                    // watch the note sub folder path for changes
                    noteDirectoryWatcher.addPath(path);
                }
            }
    }

    int count = 0;
    QList<Note> noteList = Note::fetchAll();
    Q_FOREACH(Note note, noteList) {
#ifdef Q_OS_LINUX
            // only add the last first 200 notes to the file watcher to
            // prevent that nothing is watched at all because of too many
            // open files
            if (count > 200) {
                break;
            }
#endif
            QString path = note.fullNoteFilePath();
            QFile file(path);

            if (file.exists()) {
                // watch the note for changes
                noteDirectoryWatcher.addPath(path);

                count++;
            }
        }

//    qDebug() << __func__ << " - 'noteDirectoryWatcher.files()': " <<
//    noteDirectoryWatcher.files();
//
//    qDebug() << __func__ << " - 'noteDirectoryWatcher.directories()': " <<
//    noteDirectoryWatcher.directories();
}

/**
 * Jumps to the welcome note in the note selector
 */
void MainWindow::jumpToWelcomeNote() {
    jumpToNoteName("Welcome to QOwnNotes");
}

/**
 * Jumps to a note in the note selector
 */
void MainWindow::jumpToNoteName(QString name) {
    // search for the note
    QList<QTreeWidgetItem *> items = ui->noteTreeWidget->findItems(
            name, Qt::MatchExactly, 0);
    if (items.count() > 0) {
        // set the welcome note as current note
        ui->noteTreeWidget->setCurrentItem(items.at(0));
    }
}

QString MainWindow::selectOwnCloudNotesFolder() {
    QString path = this->notesPath;

    if (path == "") {
        path = Utils::Misc::defaultNotesPath();
    }

    // TODO(pbek): We sometimes seem to get a "QCoreApplication::postEvent:
    // Unexpected null receiver" here.
    QString dir = QFileDialog::getExistingDirectory(
            this,
            tr("Please select the folder where your notes will get stored to"),
            path,
            QFileDialog::ShowDirsOnly);

    QDir d = QDir(dir);

    if (d.exists() && (dir != "")) {
        // let's remove trailing slashes
        dir = d.path();

        // update the recent note folder list
        storeRecentNoteFolder(this->notesPath, dir);

        this->notesPath = dir;
        QSettings settings;
        // make the path relative to the portable data path if we are in
        // portable mode
        settings.setValue(
                "notesPath",
                Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                                  dir));

        // update the current folder tooltip
        updateCurrentFolderTooltip();
    } else {
        if (this->notesPath == "") {
            switch (QMessageBox::information(
                   this, tr("No folder was selected"),
                    tr("You have to select your ownCloud notes "
                            "folder to make this software work!"),
                    tr("&Retry"), tr("&Exit"), QString::null,
                    0, 1)) {
                case 0:
                    selectOwnCloudNotesFolder();
                    break;
                case 1:
                default:
                    // No other way to quit the application worked
                    // in the constructor
                    QTimer::singleShot(0, this, SLOT(quitApp()));
                    QTimer::singleShot(100, this, SLOT(quitApp()));
                    break;
            }
        }
    }

    return this->notesPath;
}

/**
 * Sets the current note from a note id
 */
void MainWindow::setCurrentNoteFromNoteId(int noteId) {
    // make sure the main window is visible
    show();

    Note note = Note::fetch(noteId);
    if (note.isFetched()) {
        setCurrentNote(note);
    }
}

void MainWindow::setCurrentNote(Note note,
                                bool updateNoteText,
                                bool updateSelectedNote,
                                bool addNoteToHistory) {
    MetricsService::instance()->sendVisitIfEnabled("note/current-note/changed");

    enableShowVersionsButton();
    enableShowTrashButton();

    // update cursor position of previous note
    if (this->currentNote.exists()) {
        QTextCursor c = ui->noteTextEdit->textCursor();
        this->noteHistory.updateCursorPositionOfNote(
                this->currentNote, c.position());
    }

    // add new note to history
    if (addNoteToHistory && note.exists()) {
        this->noteHistory.add(note);
    }

    this->currentNote = note;
    QString name = note.getName();
    updateWindowTitle();

    // set the note text edit to readonly if note file is not writable
    QFileInfo *f = new QFileInfo(note.fullNoteFilePath());
    ui->noteTextEdit->setReadOnly(!f->isWritable());
    ui->encryptedNoteTextEdit->setReadOnly(!f->isWritable());

    // find and set the current item
    if (updateSelectedNote) {
        QList<QTreeWidgetItem *> items = ui->noteTreeWidget->findItems(
                name, Qt::MatchExactly);
        if (items.count() > 0) {
            const QSignalBlocker blocker(ui->noteTreeWidget);
            Q_UNUSED(blocker);

            ui->noteTreeWidget->setCurrentItem(items[0]);
        }
    }

    // update the text of the text edit
    if (updateNoteText) {
        const QSignalBlocker blocker(ui->noteTextEdit);
        Q_UNUSED(blocker);

        this->setNoteTextFromNote(&note);

        // hide the encrypted note text edit by default and show the regular one
        ui->encryptedNoteTextEdit->hide();
        ui->noteTextEdit->show();
    }

    updateEncryptNoteButtons();
    reloadCurrentNoteTags();

    ScriptingService::instance()->onCurrentNoteChanged(&currentNote);

    // update the share button
    updateShareButton();

//    putenv(QString("QOWNNOTES_CURRENT_NOTE_PATH=" + currentNote
//            .fullNoteFilePath()).toLatin1().data());
//    setenv("QOWNNOTES_CURRENT_NOTE_PATH",
//           currentNote.fullNoteFilePath().toLatin1().data(),
//           1);
}

/**
 * Updates the share button
 */
void MainWindow::updateShareButton() {
    const QSignalBlocker blocker(ui->actionShare_note);
    Q_UNUSED(blocker);
    ui->actionShare_note->setChecked(currentNote.isShared());
}

/**
 * Updates the windows title for the current note
 */
void MainWindow::updateWindowTitle() {
    setWindowTitle(currentNote.getName() + " - QOwnNotes " + QString(VERSION));
}

void MainWindow::focusNoteTextEdit() {
    // move the cursor to the 4nd line
    QTextCursor tmpCursor = ui->noteTextEdit->textCursor();
    tmpCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    tmpCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    tmpCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    tmpCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
    ui->noteTextEdit->setTextCursor(tmpCursor);

    // focus note text edit
    ui->noteTextEdit->setFocus();
}

void MainWindow::removeCurrentNote() {
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    switch (QMessageBox::information(this, tr("Remove current note"),
                     tr("Remove current note: <strong>%1</strong>?")
                             .arg(this->currentNote.getName()),
                     tr("&Remove"), tr("&Cancel"), QString::null,
                     0, 1)) {
        case 0: {
            const QSignalBlocker blocker2(ui->noteTextEdit);
            Q_UNUSED(blocker2);

            const QSignalBlocker blocker3(ui->noteTextView);
            Q_UNUSED(blocker3);

            const QSignalBlocker blocker4(ui->encryptedNoteTextEdit);
            Q_UNUSED(blocker4);

            const QSignalBlocker blocker5(noteDirectoryWatcher);
            Q_UNUSED(blocker5);

            // we try to fix problems with note subfolders
            _isNotesDirectoryWasModifiedDisabled = true;

            {
                const QSignalBlocker blocker1(ui->noteTreeWidget);
                Q_UNUSED(blocker1);

                // delete note in database and on file system
                currentNote.remove(true);

                ui->noteTextEdit->clear();
                ui->noteTextView->clear();
                ui->encryptedNoteTextEdit->clear();

                loadNoteDirectoryList();
            }

            // set a new first note
            resetCurrentNote();

            // we try to fix problems with note subfolders
            // we need to wait some time to turn the watcher on again because
            // something is happening after this method that reloads the
            // note folder
            Utils::Misc::waitMsecs(200);
            _isNotesDirectoryWasModifiedDisabled = false;

            break;
        }
        case 1:
        default:
            break;
    }
}

/**
 * Resets the current note to the first note
 */
void MainWindow::resetCurrentNote() {
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Home,
                                     Qt::NoModifier);
    QApplication::postEvent(ui->noteTreeWidget, event);
}

void MainWindow::storeSettings() {
    QSettings settings;

    // don't store the window settings in distraction free mode
    if (!isInDistractionFreeMode()) {
        settings.setValue("MainWindow/geometry", saveGeometry());
        settings.setValue("MainWindow/menuBarGeometry",
                          ui->menuBar->saveGeometry());
    }

    settings.setValue("SortingModeAlphabetically", sortAlphabetically);
    settings.setValue("ShowSystemTray", showSystemTray);
}


/*!
 * Internal events
 */

void MainWindow::closeEvent(QCloseEvent *event) {
    bool forceQuit = qApp->property("clearAppDataAndExit").toBool();

    // make sure no settings get written after after we got the
    // clearAppDataAndExit call
    if (!forceQuit) {
        storeSettings();
    }

    if (showSystemTray && !forceQuit) {
        hide();
        event->ignore();
    } else {
        MetricsService::instance()->sendVisitIfEnabled("app/end", "app end");
        qApp->setProperty("loggingEnabled", false);

        // if we don't use the system tray we delete the log widow so the app
        // can quit
        delete(LogWidget::instance());
        QMainWindow::closeEvent(event);
    }
}

//
// Event filters on the MainWindow
//
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (obj == ui->searchLineEdit->completer()->popup()) {
            if (keyEvent->key() == Qt::Key_Return) {
                // set a variable to ignore that first "Return" in the
                // return-handler
                _searchLineEditFromCompleter = true;
                return false;
            }
        } else if (obj == ui->searchLineEdit) {
            bool downSelectNote = false;

            // fallback to the default completion
            ui->searchLineEdit->completer()->setCompletionMode(
                    QCompleter::PopupCompletion);

            if (keyEvent->key() == Qt::Key_Down) {
                if (ui->searchLineEdit->completer()->completionCount() > 0) {
                    // the search text is empty we want to show all saved
                    // searches if "Down" was pressed
                    if (ui->searchLineEdit->text().isEmpty()) {
                        ui->searchLineEdit->completer()->setCompletionMode(
                                QCompleter::UnfilteredPopupCompletion);
                    }

                    // open the completer
                    ui->searchLineEdit->completer()->complete();
                    return false;
                } else {
                    // if nothing was found in the completer we want to jump
                    // to the note list
                    downSelectNote = true;
                }
            }

            // set focus to the notes list if Key_Right or Key_Tab were
            // pressed in the search line edit
            if ((keyEvent->key() == Qt::Key_Right) ||
                (keyEvent->key() == Qt::Key_Tab) ||
                downSelectNote) {
                // add the current search text to the saved searches
                storeSavedSearch();

                // choose an other selected item if current item is invisible
                QTreeWidgetItem *item = ui->noteTreeWidget->currentItem();
                if ((item != Q_NULLPTR) && item->isHidden()) {
                    QTreeWidgetItem *firstVisibleItem =
                            firstVisibleNoteTreeWidgetItem();
                    if (firstVisibleItem != Q_NULLPTR) {
                        ui->noteTreeWidget->setCurrentItem(firstVisibleItem);
                    }
                }

                // give the keyboard focus to the note tree widget
                ui->noteTreeWidget->setFocus();
                return true;
            }
            return false;
        } else if (obj == activeNoteTextEdit()) {
            // check if we want to leave the distraction free mode and the
            // search widget is not visible (because we want to close that
            // first)
            if ((keyEvent->key() == Qt::Key_Escape)
                && isInDistractionFreeMode()
                && !activeNoteTextEdit()->searchWidget()->isVisible()) {
                toggleDistractionFreeMode();
                return false;
            }
            return false;
        } else if (obj == ui->noteTreeWidget) {
            // set focus to the note text edit if Key_Return or Key_Tab were
            // pressed in the notes list
            if ((keyEvent->key() == Qt::Key_Return) ||
                    (keyEvent->key() == Qt::Key_Tab)) {

                // focusNoteTextEdit() might cause a crash in
                // on_noteTreeWidget_itemChanged when
                // Note::handleNoteRenaming is called, so we don't allow to
                // call focusNoteTextEdit()
                if (!Note::allowDifferentFileName()) {
                    focusNoteTextEdit();
                }
                return true;
            } else if ((keyEvent->key() == Qt::Key_Delete) ||
                       (keyEvent->key() == Qt::Key_Backspace)) {
                removeSelectedNotes();
                return true;
            }
            return false;
        } else if (obj == ui->tagTreeWidget) {
            if ((keyEvent->key() == Qt::Key_Delete) ||
                (keyEvent->key() == Qt::Key_Backspace)) {
                removeSelectedTags();
                return true;
            }
            return false;
        }
    }
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

        if ((mouseEvent->button() == Qt::BackButton)) {
            // move back in the note history
            on_action_Back_in_note_history_triggered();
        } else if ((mouseEvent->button() == Qt::ForwardButton)) {
            // move forward in the note history
            on_action_Forward_in_note_history_triggered();
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

/**
 * Finds the first visible tree widget row
 */
QTreeWidgetItem * MainWindow::firstVisibleNoteTreeWidgetItem() {
    QTreeWidgetItemIterator it(ui->noteTreeWidget,
                               QTreeWidgetItemIterator::NotHidden);

    return *it;
//
//    while (it) {
//        return *it;
//    }
}

/**
 * highlights all occurrences of str in the note text edit
 */
void MainWindow::searchInNoteTextEdit(QString str) {
    QList<QTextEdit::ExtraSelection> extraSelections;
    QList<QTextEdit::ExtraSelection> extraSelections2;
    QList<QTextEdit::ExtraSelection> extraSelections3;

    if (str.count() >= 2) {
        ui->noteTextEdit->moveCursor(QTextCursor::Start);
        ui->noteTextView->moveCursor(QTextCursor::Start);
        ui->encryptedNoteTextEdit->moveCursor(QTextCursor::Start);
        QColor color = QColor(0, 180, 0, 100);

        // build the string list of the search string
        QString queryStr = str.replace("|", "\\|");
        QStringList queryStrings = Note::buildQueryStringList(queryStr);

        QRegExp regExp("(" + queryStrings.join("|") + ")", Qt::CaseInsensitive);

        while (ui->noteTextEdit->find(regExp)) {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground(color);

            extra.cursor = ui->noteTextEdit->textCursor();
            extraSelections.append(extra);
        }

        while (ui->noteTextView->find(regExp)) {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground(color);

            extra.cursor = ui->noteTextView->textCursor();
            extraSelections2.append(extra);
        }

        while (ui->encryptedNoteTextEdit->find(regExp)) {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground(color);

            extra.cursor = ui->encryptedNoteTextEdit->textCursor();
            extraSelections3.append(extra);
        }
    }

    ui->noteTextEdit->setExtraSelections(extraSelections);
    ui->noteTextView->setExtraSelections(extraSelections2);
    ui->encryptedNoteTextEdit->setExtraSelections(extraSelections3);
}

/**
 * highlights all occurrences of tje search line text in the note text edit
 */
void MainWindow::searchForSearchLineTextInNoteTextEdit() {
    QString searchString = ui->searchLineEdit->text();
    searchInNoteTextEdit(searchString);
}

/**
 * Asks for the password if the note is encrypted and can't be decrypted
 */
void MainWindow::askForEncryptedNotePasswordIfNeeded(QString additionalText) {
    currentNote.refetch();

    // check if the note is encrypted and can't be decrypted
    if (currentNote.hasEncryptedNoteText() &&
        !currentNote.canDecryptNoteText()) {
        QString labelText =
                tr("Please enter the <strong>password</strong> "
                        "of this encrypted note.");

        if (!additionalText.isEmpty()) {
            labelText += " " + additionalText;
        }

        PasswordDialog* dialog = new PasswordDialog(this, labelText);
        int dialogResult = dialog->exec();

        // if user pressed ok take the password
        if (dialogResult == QDialog::Accepted) {
            QString password = dialog->password();
            if (password != "") {
                // set the password so it can be decrypted
                // for the markdown view
                currentNote.setCryptoPassword(password);
                currentNote.store();
            }

            // warn if password is incorrect
            if (!currentNote.canDecryptNoteText()) {
                QMessageBox::warning(
                        this,
                        tr("Note can't be decrypted!"),
                        tr("It seems that your password is not valid!"));
            }
        }
    }
}

/**
 * Gets the maximum image width
 */
int MainWindow::getMaxImageWidth()
{
    QMargins margins = ui->noteTextView->contentsMargins();
    int maxImageWidth = ui->noteTextView->viewport()->width() - margins.left()
                        - margins.right() - 15;

    if (maxImageWidth < 0) {
        maxImageWidth = 16;
    }

    return maxImageWidth;
}

/**
 * Sets the note text according to a note
 */
void MainWindow::setNoteTextFromNote(Note *note, bool updateNoteTextViewOnly) {
    if (!updateNoteTextViewOnly) {
        ui->noteTextEdit->setText(note->getNoteText());
    }

    bool decrypt = ui->noteTextEdit->isHidden();
    ui->noteTextView->setHtml(
            note->toMarkdownHtml(NoteFolder::currentLocalPath(),
                                 getMaxImageWidth(), false, decrypt));

    // update the slider when editing notes
    noteTextSliderValueChanged(
            activeNoteTextEdit()->verticalScrollBar()->value(), true);
}

/**
 * Starts the parsing for the navigation widget
 */
void MainWindow::startNavigationParser() {
    ui->navigationWidget->parse(activeNoteTextEdit()->document());
}

/**
 * Sets the text of the current note.
 * This is a public callback function for the version dialog.
 *
 * @brief MainWindow::setCurrentNoteText
 * @param text
 */
void MainWindow::setCurrentNoteText(QString text) {
    currentNote.setNoteText(text);
    setNoteTextFromNote(&currentNote, false);
}

/**
 * Creates a new note (to restore a trashed note)
 * This is a public callback function for the trash dialog.
 *
 * @brief MainWindow::createNewNote
 * @param name
 * @param text
 * @param cursorAtEnd
 */
void MainWindow::createNewNote(QString name, QString text, bool cursorAtEnd) {
    QString extension = Note::defaultNoteFileExtension();
    QFile *f = new QFile(this->notesPath + QDir::separator() + name + "."
                         + extension);

    // change the name and headline if note exists
    if (f->exists()) {
        QDateTime currentDate = QDateTime::currentDateTime();
        name.append(" " + currentDate.toString(Qt::ISODate).replace(":", "."));

        QString preText = Note::createNoteHeader(name);
        text.prepend(preText);
    }

    ui->searchLineEdit->setText(name);
    on_searchLineEdit_returnPressed();
    ui->noteTextEdit->setText(text);

    // move the cursor to the end of the note
    if (cursorAtEnd) {
        QTextCursor c = ui->noteTextEdit->textCursor();
        c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        ui->noteTextEdit->setTextCursor(c);
    }
}

/**
 * @brief Restores a trashed note on the server.
 * @param name
 * @param text
 *
 * This is a public callback function for the trash dialog.
 */
void MainWindow::restoreTrashedNoteOnServer(QString fileName, int timestamp) {
    OwnCloudService *ownCloud = new OwnCloudService(this);
    ownCloud->restoreTrashedNoteOnServer(fileName, timestamp, this);
}

/**
 * @brief Removes selected notes after a confirmation
 */
void MainWindow::removeSelectedNotes() {
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    int selectedItemsCount = ui->noteTreeWidget->selectedItems().count();

    if (selectedItemsCount == 0) {
        return;
    }

    if (QMessageBox::information(
            this,
            tr("Remove selected notes"),
            tr("Remove <strong>%n</strong> selected note(s)?\n\n"
               "If the trash is enabled on your "
                    "ownCloud server you should be able to restore "
                    "them from there.",
               "", selectedItemsCount),
             tr("&Remove"), tr("&Cancel"), QString::null,
             0, 1) == 0) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker);

        const QSignalBlocker blocker2(activeNoteTextEdit());
        Q_UNUSED(blocker2);

        const QSignalBlocker blocker3(ui->noteTextView);
        Q_UNUSED(blocker3);

        const QSignalBlocker blocker4(ui->encryptedNoteTextEdit);
        Q_UNUSED(blocker4);

        // we try to fix problems with note subfolders
        _isNotesDirectoryWasModifiedDisabled = true;

        {
            const QSignalBlocker blocker1(ui->noteTreeWidget);
            Q_UNUSED(blocker1);

            Q_FOREACH(QTreeWidgetItem *item,
                      ui->noteTreeWidget->selectedItems()) {
                    int id = item->data(0, Qt::UserRole).toInt();
                    Note note = Note::fetch(id);
                    note.remove(true);
                    qDebug() << "Removed note " << note.getName();
                }

            // clear the text edit so it stays clear after removing the
            // last note
            activeNoteTextEdit()->clear();

            loadNoteDirectoryList();
        }

        // set a new first note
        resetCurrentNote();

        // we try to fix problems with note subfolders
        // we need to wait some time to turn the watcher on again because
        // something is happening after this method that reloads the note folder
        Utils::Misc::waitMsecs(200);
        _isNotesDirectoryWasModifiedDisabled = false;
    }
}

/**
 * Removes selected note subfolders after a confirmation
 */
void MainWindow::removeSelectedNoteSubFolders() {
    int selectedItemsCount =
            ui->noteSubFolderTreeWidget->selectedItems().size();

    if (selectedItemsCount == 0) {
        return;
    }

    // gather the folders that are about to be deleted
    QStringList noteSubFolderPathList;
    QList<NoteSubFolder> noteSubFolderList;
    Q_FOREACH(QTreeWidgetItem *item,
              ui->noteSubFolderTreeWidget->selectedItems()) {
            int id = item->data(0, Qt::UserRole).toInt();
            NoteSubFolder noteSubFolder = NoteSubFolder::fetch(id);
            if (noteSubFolder.isFetched()) {
                noteSubFolderList << noteSubFolder;
                noteSubFolderPathList << noteSubFolder.fullPath();
            }
    }

    if (noteSubFolderList.count() == 0) {
        return;
    }

    if (QMessageBox::information(
            this,
            tr("Remove selected folders"),
            tr("Remove <strong>%n</strong> selected folder(s)?"
                   "<ul><li>%1</li></ul>"
                   "All files and folders in these folders will be removed as"
                       " well!",
               "", selectedItemsCount).arg(noteSubFolderPathList.join(
                    "</li><li>")),
             tr("&Remove"), tr("&Cancel"), QString::null,
             0, 1) == 0) {
        // delete the note subfolders
        Q_FOREACH(NoteSubFolder noteSubFolder, noteSubFolderList) {
                // remove the directory recursively from the file system
                if (noteSubFolder.removeFromFileSystem()) {
                    showStatusBarMessage(tr("Removed note subfolder: %1").arg(
                            noteSubFolder.fullPath()));
                }
        }
    }
}

/**
 * Removes selected tags after a confirmation
 */
void MainWindow::removeSelectedTags() {
    int selectedItemsCount = ui->tagTreeWidget->selectedItems().size();

    if (selectedItemsCount == 0) {
        return;
    }

    if (QMessageBox::information(
            this,
            tr("Remove selected tags"),
            tr("Remove <strong>%n</strong> selected tag(s)? No notes will "
                       "be removed in this process.",
               "", selectedItemsCount),
             tr("&Remove"), tr("&Cancel"), QString::null,
             0, 1) == 0) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker);

        const QSignalBlocker blocker1(ui->tagTreeWidget);
        Q_UNUSED(blocker1);

        Q_FOREACH(QTreeWidgetItem *item, ui->tagTreeWidget->selectedItems()) {
            int tagId = item->data(0, Qt::UserRole).toInt();
            Tag tag = Tag::fetch(tagId);
            tag.remove();
            qDebug() << "Removed tag " << tag.getName();
        }

        reloadCurrentNoteTags();
        reloadTagTree();
    }
}

/**
 * @brief Select all notes
 */
void MainWindow::selectAllNotes() {
    ui->noteTreeWidget->selectAll();
}

/**
 * @brief Moves selected notes after a confirmation
 * @param destinationFolder
 */
void MainWindow::moveSelectedNotesToFolder(QString destinationFolder) {
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();

    if (QMessageBox::information(
            this,
            tr("Move selected notes"),
            tr("Move %n selected note(s) to <strong>%2</strong>?", "",
               selectedItemsCount).arg(destinationFolder),
            tr("&Move"), tr("&Cancel"), QString::null,
            0, 1) == 0) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker);

        Q_FOREACH(QTreeWidgetItem *item, ui->noteTreeWidget->selectedItems()) {
                int noteId = item->data(0, Qt::UserRole).toInt();
                Note note = Note::fetch(noteId);

                if (!note.isFetched()) {
                    continue;
                }

                // remove note path form directory watcher
                this->noteDirectoryWatcher.removePath(note.fullNoteFilePath());

                if (note.getId() == currentNote.getId()) {
                    // reset the current note
                    this->currentNote = Note();

                    // clear the note text edit
                    const QSignalBlocker blocker2(ui->noteTextEdit);
                    Q_UNUSED(blocker2);
                    ui->noteTextEdit->clear();

                    // clear the encrypted note text edit
                    const QSignalBlocker blocker3(ui->encryptedNoteTextEdit);
                    Q_UNUSED(blocker3);
                    ui->encryptedNoteTextEdit->clear();
                }

                // move note
                bool result = note.move(destinationFolder);
                if (result) {
                    qDebug() << "Note was moved:" << note.getName();
                } else {
                    qWarning() << "Could not move note:" << note.getName();
                }
            }

        loadNoteDirectoryList();
    }
}

/**
 * @brief Copies selected notes after a confirmation
 * @param destinationFolder
 */
void MainWindow::copySelectedNotesToFolder(QString destinationFolder) {
    int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();

    if (QMessageBox::information(
            this,
            tr("Copy selected notes"),
            tr("Copy %n selected note(s) to <strong>%2</strong>?", "",
               selectedItemsCount).arg(destinationFolder),
            tr("&Copy"), tr("&Cancel"), QString::null, 0, 1) == 0) {
        int copyCount = 0;
        Q_FOREACH(QTreeWidgetItem *item, ui->noteTreeWidget->selectedItems()) {
                int noteId = item->data(0, Qt::UserRole).toInt();
                Note note = Note::fetch(noteId);

                if (!note.isFetched()) {
                    continue;
                }

                // copy note
                bool result = note.copy(destinationFolder);
                if (result) {
                    copyCount++;
                    qDebug() << "Note was copied:" << note.getName();
                } else {
                    qWarning() << "Could not copy note:" << note.getName();
                }
            }

        QMessageBox::information(
                this, tr("Done"),
                tr("%n note(s) were copied to <strong>%2</strong>.", "",
                   copyCount).arg(destinationFolder));
    }
}

/**
 * Tags selected notes
 */
void MainWindow::tagSelectedNotes(Tag tag) {
    int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();

    if (QMessageBox::information(
            this,
            tr("Tag selected notes"),
            tr("Tag %n selected note(s) with <strong>%2</strong>?", "",
               selectedItemsCount).arg(tag.getName()),
            tr("&Tag"), tr("&Cancel"), QString::null, 0, 1) == 0) {
        int tagCount = 0;
        Q_FOREACH(QTreeWidgetItem *item, ui->noteTreeWidget->selectedItems()) {
                int noteId = item->data(0, Qt::UserRole).toInt();
                Note note = Note::fetch(noteId);

                if (!note.isFetched()) {
                    continue;
                }

                // tag note
                bool result = tag.linkToNote(note);
                if (result) {
                    tagCount++;
                    qDebug() << "Note was tagged:" << note.getName();

                    // handle the coloring of the note in the note tree widget
                    handleNoteTreeTagColoringForNote(note);
                } else {
                    qWarning() << "Could not tag note:" << note.getName();
                }
            }

        reloadCurrentNoteTags();
        reloadTagTree();

        showStatusBarMessage(
                tr("%n note(s) were tagged with \"%2\"", "",
                   tagCount).arg(tag.getName()), 5000);
    }
}

/**
 * Removes a tag from the selected notes
 */
void MainWindow::removeTagFromSelectedNotes(Tag tag) {
    int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();

    if (QMessageBox::information(
            this,
            tr("Remove tag from selected notes"),
            tr("Remove tag <strong>%1</strong> from %n selected note(s)?", "",
               selectedItemsCount).arg(tag.getName()),
            tr("&Remove"), tr("&Cancel"), QString::null, 0, 1) == 0) {
        int tagCount = 0;
        Q_FOREACH(QTreeWidgetItem *item, ui->noteTreeWidget->selectedItems()) {
                int noteId = item->data(0, Qt::UserRole).toInt();
                Note note = Note::fetch(noteId);

                if (!note.isFetched()) {
                    continue;
                }

                // tag note
                bool result = tag.removeLinkToNote(note);
                if (result) {
                    tagCount++;
                    qDebug() << "Tag was removed from note:" << note.getName();

                    // handle the coloring of the note in the note tree widget
                    handleNoteTreeTagColoringForNote(note);
                } else {
                    qWarning() << "Could not remove tag from note:"
                    << note.getName();
                }
            }

        reloadCurrentNoteTags();
        reloadTagTree();

        QMessageBox::information(
                this, tr("Done"),
                tr("Tag <strong>%1</strong> was removed from %n note(s)", "",
                   tagCount).arg(tag.getName()));
    }
}

/**
 * Handle the coloring of the note in the note tree widget
 *
 * @param note
 */
void MainWindow::handleNoteTreeTagColoringForNote(const Note &note) {
    Tag colorTag = Tag::fetchOneOfNoteWithColor(note);
    QTreeWidgetItem *noteItem = findNoteInNoteTreeWidget(note);
    handleTreeWidgetItemTagColor(noteItem, colorTag);
}

/**
 * @brief Updates the current folder tooltip
 */
void MainWindow::updateCurrentFolderTooltip() {
    ui->actionSet_ownCloud_Folder
            ->setStatusTip(tr("Current notes folder: ") + this->notesPath);
    ui->actionSet_ownCloud_Folder
            ->setToolTip(tr("Set the notes folder. Current notes folder: ") +
                                 this->notesPath);
}

/**
 * Opens the settings dialog
 */
void MainWindow::openSettingsDialog(int page) {
    // open the settings dialog
    SettingsDialog *dialog = new SettingsDialog(page, this);
    dialog->exec();

    // make sure no settings get written after after we got the
    // clearAppDataAndExit call
    if (qApp->property("clearAppDataAndExit").toBool()) {
        return;
    }

    // read all relevant settings, that can be set in the settings dialog,
    // even if the dialog was canceled
    readSettingsFromSettingsDialog();

    // reset the note save timer
    this->noteSaveTimer->stop();
    this->noteSaveTimer->start(this->noteSaveIntervalTime * 1000);

    // set the current note folder again in case its path was changed
    NoteFolder noteFolder = NoteFolder::currentNoteFolder();

    if (noteFolder.isFetched()) {
        changeNoteFolder(noteFolder.getId(), true);
    }

    // reload note folders in case we changed them in the settings
    loadNoteFolderListMenu();

    // load the note list again in case the setting on the note name has changed
    loadNoteDirectoryList();
}

/**
 * @brief Returns the active note text edit
 */
QMarkdownTextEdit* MainWindow::activeNoteTextEdit() {
    return ui->noteTextEdit->isHidden() ?
                                  ui->encryptedNoteTextEdit : ui->noteTextEdit;
}

/**
 * @brief Handles the linking of text
 */
void MainWindow::handleTextNoteLinking() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    LinkDialog *dialog = new LinkDialog(tr("Link to an url or note"), this);
    dialog->exec();
    if (dialog->result() == QDialog::Accepted) {
        QString url = dialog->getURL();
        QString noteName = dialog->getSelectedNoteName();
        QString noteNameForLink = Note::generateTextForLink(noteName);

        if ((noteName != "") || (url != "")) {
            QString selectedText =
                    textEdit->textCursor().selectedText();
            QString newText;

            // if user has entered an url
            if (url != "") {
                if (selectedText != "") {
                    newText = "[" + selectedText + "](" + url + ")";
                } else {
                    // if possible fetch the title of the webpage
                    QString title = dialog->getTitleForUrl(QUrl(url));

                    // if we got back a tile let's use it in the link
                    if (title != "") {
                        newText = "[" + title + "](" + url + ")";
                    } else {
                        newText = "<" + url + ">";
                    }
                }
            } else {
                // if user has selected a note
                if (selectedText != "") {
                    newText = "[" + selectedText + "]"
                           "(note://" + noteNameForLink + ")";
                } else {
                    newText = "<note://" + noteNameForLink + ">";
                }
            }
            textEdit->textCursor().insertText(newText);
        }
    }
}

/**
 * Downloads an url and stores it to a file
 */
bool MainWindow::downloadUrlToFile(QUrl url, QFile *file) {
    if (!file->open(QIODevice::WriteOnly)) {
        return false;
    }

    if (!file->isWritable()) {
        return false;
    }

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QEventLoop loop;
    QTimer timer;

    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), &loop, SLOT(quit()));
    connect(manager, SIGNAL(finished(QNetworkReply *)), &loop, SLOT(quit()));

    // 10 sec timeout for the request
    timer.start(10000);

    QNetworkRequest networkRequest = QNetworkRequest(url);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    networkRequest.setAttribute(QNetworkRequest::FollowRedirectsAttribute,
                                true);
#endif

    QNetworkReply *reply = manager->get(networkRequest);
    loop.exec();

    // if we didn't get a timeout let's write the file
    if (timer.isActive()) {
        // get the text from the network reply
        QByteArray data = reply->readAll();
        if (data.size() > 0) {
            file->write(data);
            return true;
        }
    }

    // timer elapsed, no reply from network request or empty data
    return false;
}


/**
 * @brief Sets the current note from a CurrentNoteHistoryItem
 * @param item
 */
void MainWindow::setCurrentNoteFromHistoryItem(NoteHistoryItem item) {
    qDebug() << item;
    qDebug() << item.getNote();

    setCurrentNote(item.getNote(), true, true, false);
    QTextCursor c = ui->noteTextEdit->textCursor();
    c.setPosition(item.getCursorPosition());
    ui->noteTextEdit->setTextCursor(c);
}

/**
 * @brief Prints the content of a text edit widget
 * @param textEdit
 */
void MainWindow::printNote(QTextEdit *textEdit) {
    QPrinter printer;

    QPrintDialog dialog(&printer, this);
    dialog.setWindowTitle(tr("Print note"));

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    textEdit->document()->print(&printer);
}

/**
 * @brief Exports the content of a text edit widget as PDF
 * @param textEdit
 */
void MainWindow::exportNoteAsPDF(QTextEdit *textEdit) {
    QPrinter printer(QPrinter::HighResolution);

#ifdef Q_OS_LINUX
    // under Linux we use the the QPageSetupDialog to change layout
    // settings of the PDF export
    QPageSetupDialog pageSetupDialog(&printer, this);

    if (pageSetupDialog.exec() != QDialog::Accepted) {
        return;
    }
#else
    // under OS X and Windows the QPageSetupDialog dialog doesn't work,
    // we will use a workaround to select page sizes and the orientation

    // select the page size
    QStringList pageSizeStrings;
    pageSizeStrings << "A0" << "A1" << "A2" << "A3" << "A4" << "A5"
        << "A6" << "A7" << "A8" << "A9" << tr("Letter");
    QList<QPageSize::PageSizeId> pageSizes;
    pageSizes << QPageSize::A0 << QPageSize::A1 << QPageSize::A2 <<
            QPageSize::A3 << QPageSize::A4 << QPageSize::A5 <<
            QPageSize::A6 << QPageSize::A7 << QPageSize::A8 <<
            QPageSize::A9 << QPageSize::Letter;

    bool ok;
    QString pageSizeString = QInputDialog::getItem(
            this, tr("Page size"), tr("Page size:"),
            pageSizeStrings, 4, false, &ok);

    if (!ok || pageSizeString.isEmpty()) {
        return;
    }

    int pageSizeIndex = pageSizeStrings.indexOf(pageSizeString);
    if (pageSizeIndex == -1) {
        return;
    }

    QPageSize pageSize(pageSizes.at(pageSizeIndex));
    printer.setPageSize(pageSize);

    // select the orientation
    QStringList orientationStrings;
    orientationStrings << tr("Portrait") << tr("Landscape");
    QList<QPrinter::Orientation> orientations;
    orientations << QPrinter::Portrait << QPrinter::Landscape;

    QString orientationString = QInputDialog::getItem(
            this, tr("Orientation"), tr("Orientation:"),
            orientationStrings, 0, false, &ok);

    if (!ok || orientationString.isEmpty()) {
        return;
    }

    int orientationIndex =
            orientationStrings.indexOf(orientationString);
    if (orientationIndex == -1) {
        return;
    }

    printer.setOrientation(orientations.at(orientationIndex));
#endif

    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("PDF files (*.pdf)"));
    dialog.setWindowTitle(tr("Export current note as PDF"));
    dialog.selectFile(currentNote.getName() + ".pdf");
    int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return;
    }

    QStringList fileNames = dialog.selectedFiles();

    if (fileNames.count() == 0) {
        return;
    }

    QString fileName = fileNames.at(0);

    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName.append(".pdf");
    }

    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    textEdit->document()->print(&printer);
}

/**
 * Shows the app metrics notification if not already shown
 */
void MainWindow::showAppMetricsNotificationIfNeeded() {
    QSettings settings;
    bool showDialog = !settings.value("appMetrics/notificationShown").toBool();

    if (showDialog) {
        settings.setValue("appMetrics/notificationShown", true);

        if (QMessageBox::information(
                this,
                "QOwnNotes",
                tr("QOwnNotes will track anonymous usage data, that helps to "
                        "decide what parts of QOwnNotes to improve next "
                        "and to find and fix bugs. You can disable that "
                        "behaviour in the settings."),
                tr("&Ok"),
                tr("Open &settings"),
                QString::null, 0, 1) == 1) {
            openSettingsDialog(SettingsDialog::NetworkPage);
        }
    }
}

/**
 * Opens the task list dialog
 */
void MainWindow::openTodoDialog(QString taskUid) {
    QSettings settings;
    QStringList todoCalendarEnabledUrlList =
            settings.value("ownCloud/todoCalendarEnabledUrlList")
                    .toStringList();

    // check if we have got any task list enabled
    if (todoCalendarEnabledUrlList.count() == 0) {
        if (QMessageBox::warning(
                0, tr("No selected todo lists!"),
                tr("You have not selected any todo lists.<br />"
                           "Please check your <strong>Todo</strong> "
                           "configuration in the settings!"),
                tr("Open &settings"),
                tr("&Cancel"),
                QString::null, 0, 1) == 0) {
            openSettingsDialog(SettingsDialog::TodoPage);
        }

        return;
    }

    TodoDialog *dialog = new TodoDialog(this, taskUid, this);
    dialog->show();

    // generate the system tray context menu to show modified tasks
    generateSystemTrayContextMenu();
}




// *****************************************************************************
// *
// *
// * Slot implementations
// *
// *
// *****************************************************************************

void MainWindow::on_noteTextEdit_textChanged() {
    Note note = this->currentNote;
    note.updateNoteTextFromDisk();
    QString noteTextFromDisk = note.getNoteText();

    QString text = this->ui->noteTextEdit->toPlainText();

    if (text != noteTextFromDisk) {
        this->currentNote.storeNewText(text);
        this->currentNote.refetch();
        this->currentNoteLastEdited = QDateTime::currentDateTime();
        _noteViewNeedsUpdate = true;

        updateEncryptNoteButtons();

        if (!sortAlphabetically) {
            makeCurrentNoteFirstInNoteList();
        }

        const QSignalBlocker blocker(ui->noteTreeWidget);
        Q_UNUSED(blocker);

        // update the note list tooltip of the note
        setTreeWidgetItemToolTipForNote(ui->noteTreeWidget->currentItem(),
                                        &currentNote,
                                        &currentNoteLastEdited);
    }
}

void MainWindow::on_action_Quit_triggered() {
    storeSettings();
    QApplication::quit();
}

void MainWindow::quitApp() {
    QApplication::quit();
}

void MainWindow::on_actionSet_ownCloud_Folder_triggered() {
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    openSettingsDialog(SettingsDialog::NoteFolderPage);
}

void MainWindow::on_searchLineEdit_textChanged(const QString &arg1) {
    Q_UNUSED(arg1);
    filterNotes();
}

/**
 * Does the note filtering
 */
void MainWindow::filterNotes(bool searchForText) {
    // filter the notes by text in the search line edit
    filterNotesBySearchLineEditText();

    if (NoteFolder::isCurrentShowSubfolders() &&
            !_showNotesFromAllNoteSubFolders) {
        // filter the notes by note sub folder
        filterNotesByNoteSubFolders();
    }

    if (isTagsEnabled()) {
        // filter the notes by tag
        filterNotesByTag();
    }

    if (searchForText) {
        // let's highlight the text from the search line edit
        searchForSearchLineTextInNoteTextEdit();
    }
}

/**
 * Checks if tagging is enabled
 */
bool MainWindow::isTagsEnabled() {
    return _taggingDockWidget->isVisible();
}

/**
 * Checks if the markdown view is enabled
 */
bool MainWindow::isMarkdownViewEnabled() {
    QSettings settings;
    return settings.value("markdownViewEnabled", true).toBool();
}

/**
 * Checks if the note edit pane is enabled
 */
bool MainWindow::isNoteEditPaneEnabled() {
    return _noteEditDockWidget->isVisible();
}

/**
 * Does the note filtering by text in the search line edit
 */
void MainWindow::filterNotesBySearchLineEditText() {
    QString arg1 = ui->searchLineEdit->text();

    QTreeWidgetItemIterator it(ui->noteTreeWidget);

    // search notes when at least 2 characters were entered
    if (arg1.count() >= 2) {
        QList<int> noteIdList = Note::searchInNotes(
                arg1, _showNotesFromAllNoteSubFolders);

        while (*it) {
            // hide all filtered notes
            (*it)->setHidden(noteIdList.indexOf(
                    (*it)->data(0, Qt::UserRole).toInt()) < 0);
            ++it;
        }
    } else {
        // otherwise show all items
        while (*it) {
            (*it)->setHidden(false);
            ++it;
        }
    }
}

/**
 * Does the note filtering by tags
 */
void MainWindow::filterNotesByTag() {
    int tagId = Tag::activeTagId();
    QStringList fileNameList;

    switch (tagId) {
        case Tag::AllNotesId:
            // don't do any additional filtering here
            return;
        break;
        case Tag::AllUntaggedNotesId:
            // get all note names that are not tagged
            fileNameList = Note::fetchAllNotTaggedNames();
            break;
        default:
            // check if there is an active tag
            Tag tag = Tag::activeTag();

            if (!tag.isFetched()) {
                return;
            }

            // fetch all linked note names
            fileNameList = tag.fetchAllLinkedNoteFileNames();
            break;
    }

    qDebug() << __func__ << " - 'fileNameList': " << fileNameList;

    // omit the already hidden notes
    QTreeWidgetItemIterator it(ui->noteTreeWidget,
                               QTreeWidgetItemIterator::NotHidden);

    // loop through all visible notes
    while (*it) {
        // hide all notes that are not linked to the active tag
        // note subfolder are not taken into account here (note names are now
        // not unique), but it should be ok because they are filtered by
        // filterNotesByNoteSubFolders
        if (!fileNameList.contains((*it)->text(0))) {
            (*it)->setHidden(true);
        }

        ++it;
    }
}

/**
 * Does the note filtering by note sub folders
 */
void MainWindow::filterNotesByNoteSubFolders() {
    int subNoteFolderId = NoteSubFolder::activeNoteSubFolderId();

    // get all notes of a note sub folder
    QList<Note> noteList = Note::fetchAllByNoteSubFolderId(subNoteFolderId);
    QList<int> noteIdList = Note::noteIdListFromNoteList(noteList);

    // omit the already hidden notes
    QTreeWidgetItemIterator it(ui->noteTreeWidget,
                               QTreeWidgetItemIterator::NotHidden);

    // loop through all visible notes
    while (*it) {
        // hide all notes that are not in the note sub folder
        if (!noteIdList.contains((*it)->data(0, Qt::UserRole).toInt())) {
            (*it)->setHidden(true);
        }

        ++it;
    }
}

//
// set focus on search line edit if Ctrl + Shift + F was pressed
//
void MainWindow::on_action_Find_note_triggered() {
    changeDistractionFreeMode(false);
    this->ui->searchLineEdit->setFocus();
    this->ui->searchLineEdit->selectAll();
}

//
// jump to found note or create a new one if not found
//
void MainWindow::on_searchLineEdit_returnPressed() {
    jumpToNoteOrCreateNew();
}

/**
 * Jumps to found note or create a new one if not found
 */
void MainWindow::jumpToNoteOrCreateNew() {
    // ignore if `return` was pressed in the completer
    if (_searchLineEditFromCompleter) {
        _searchLineEditFromCompleter = false;
        return;
    }

    // this doesn't seem to work with note sub folders
    const QSignalBlocker blocker(noteDirectoryWatcher);
    Q_UNUSED(blocker);

    // add the current search text to the saved searches
    storeSavedSearch();

    QString text = ui->searchLineEdit->text();
    text = text.trimmed();

    // first let us search for the entered text
    Note note = Note::fetchByName(text);

    // if we can't find a note we create a new one
    if (note.getId() == 0) {
        // check if a hook wants to set the text
        QString noteText = ScriptingService::instance()->
            callHandleNewNoteHeadlineHook(text);

        // check if a hook changed the text
        if (noteText.isEmpty()) {
            // fallback to the old text if no hook changed the text
            noteText = Note::createNoteHeader(text);
        } else {
            noteText.append("\n\n");
        }

        NoteSubFolder noteSubFolder = NoteSubFolder::activeNoteSubFolder();
        QString noteSubFolderPath = noteSubFolder.fullPath();

        note = Note();
        note.setName(text);
        note.setNoteText(noteText);
        note.setNoteSubFolderId(noteSubFolder.getId());
        note.store();

        // workaround when signal block doesn't work correctly
        _isNotesDirectoryWasModifiedDisabled = true;

        // we even need a 2nd workaround because something triggers that the
        // note folder was modified
        noteDirectoryWatcher.removePath(notesPath);
        noteDirectoryWatcher.removePath(noteSubFolderPath);

        // store the note to disk
        // if a tag is selected add the tag to the just created note
        Tag tag = Tag::activeTag();
        if (tag.isFetched()) {
            tag.linkToNote(note);
        }

        note.storeNoteTextFileToDisk();
        showStatusBarMessage(
                tr("Stored current note to disk"), 3000);

        {
            const QSignalBlocker blocker2(ui->noteTreeWidget);
            Q_UNUSED(blocker2);

            // adds the note to the note tree widget
            addNoteToNoteTreeWidget(note);
        }

//        buildNotesIndex();
        loadNoteDirectoryList();

        // fetch note new (because all the IDs have changed after
        // the buildNotesIndex()
//        note.refetch();

        // clear search line edit so all notes will be viewed again
        ui->searchLineEdit->clear();

        // add the file to the note directory watcher
        noteDirectoryWatcher.addPath(note.fullNoteFilePath());

        // add the paths from the workaround
        noteDirectoryWatcher.addPath(notesPath);
        noteDirectoryWatcher.addPath(noteSubFolderPath);

        // turn on the method again
        _isNotesDirectoryWasModifiedDisabled = false;
    }

    // jump to the found or created note
    setCurrentNote(note);

    // focus the note text edit and set the cursor correctly
    focusNoteTextEdit();
}

void MainWindow::on_action_Remove_note_triggered() {
    removeCurrentNote();
}

void MainWindow::on_actionAbout_QOwnNotes_triggered() {
    AboutDialog *dialog = new AboutDialog(this);
    dialog->exec();
}

//
// Triggered by the shortcut to create new note with date in the headline
//
void MainWindow::on_action_New_note_triggered() {
    // create a new note
    createNewNote();
}

/**
 * Creates a new note
 *
 * @param noteName
 */
void MainWindow::createNewNote(QString noteName) {
    // show the window in case we are using the system tray
    show();

    QDateTime currentDate = QDateTime::currentDateTime();

    if (noteName.isEmpty()) {
        noteName = "Note";
    }

    // replacing ":" with "_" for Windows systems
    QString text = noteName + " " + currentDate.toString(Qt::ISODate)
                                     .replace(":", ".");

    this->ui->searchLineEdit->setText(text);

    // create a new note
    jumpToNoteOrCreateNew();
}

/*
 * Handles urls in the noteTextView
 *
 * examples:
 * - <note://MyNote> opens the note "MyNote"
 * - <note://my-note-with-spaces-in-the-name> opens the note "My Note with spaces in the name"
 * - <http://www.qownnotes.org> opens the web page
 * - <file:///path/to/my/file/QOwnNotes.pdf> opens the file "/path/to/my/file/QOwnNotes.pdf" if the operating system supports that handler
 */
void MainWindow::on_noteTextView_anchorClicked(const QUrl &url) {
    qDebug() << __func__ << " - 'url': " << url;
    QString scheme = url.scheme();

    if ((scheme == "note" || scheme == "task")) {
        openLocalUrl(url.toString());
    } else {
        ui->noteTextEdit->openUrl(url.toString());
    }
}

/*
 * Handles note urls
 *
 * examples:
 * - <note://MyNote> opens the note "MyNote"
 * - <note://my-note-with-spaces-in-the-name> opens the note "My Note with spaces in the name"
 */
void MainWindow::openLocalUrl(QString urlString) {
    // convert relative file urls to absolute urls and open them
    if (urlString.startsWith("file://..")) {
        QString windowsSlash = "";

#ifdef Q_OS_WIN32
    // we need an other slash for Windows
    windowsSlash = "/";
#endif

        urlString.replace(
                "file://..",
                "file://" + windowsSlash + NoteFolder::currentLocalPath() +
                        "/..");

        QDesktopServices::openUrl(QUrl(urlString));
        return;
    }

    QUrl url = QUrl(urlString);
    QString scheme = url.scheme();

    if (scheme == "note") {
        // add a ".com" to the filename to simulate a valid domain
        QString fileName = url.host() + ".com";;

        // convert the ACE to IDN (internationalized domain names) to support
        // links to notes with unicode characters in their names
        // then remove the ".com" again
        fileName = Utils::Misc::removeIfEndsWith(
                QUrl::fromAce(fileName.toLatin1()), ".com");

        // if it seem we have unicode characters in our filename let us use
        // wildcards for each number, because full width numbers get somehow
        // translated to normal numbers by the QTextEdit
        if (fileName != url.host()) {
            fileName.replace("1", "[1１]")
                    .replace("2", "[2２]")
                    .replace("3", "[3３]")
                    .replace("4", "[4４]")
                    .replace("5", "[5５]")
                    .replace("6", "[6６]")
                    .replace("7", "[7７]")
                    .replace("8", "[8８]")
                    .replace("9", "[9９]")
                    .replace("0", "[0０]");
        }

        // this makes it possible to search for file names containing spaces
        // instead of spaces a "-" has to be used in the note link
        // example: note://my-note-with-spaces-in-the-name
        fileName.replace("-", "?").replace("_", "?");

        // we need to search for the case sensitive filename,
        // we only get it lowercase by QUrl
        QDir currentDir = QDir(NoteSubFolder::activeNoteSubFolder().fullPath());

        QStringList files;
        QStringList fileSearchList =
                QStringList() << fileName + ".txt" << fileName + ".md";

        // append the files with custom extension
        fileSearchList.append(
                Note::customNoteFileExtensionList(fileName + "."));

        // search for files with that name
        files = currentDir.entryList(fileSearchList,
                                     QDir::Files | QDir::NoSymLinks);

        // did we find files?
        if (files.length() > 0) {
            // take the first found file
            fileName = files.first();

            // try to fetch note
            Note note = Note::fetchByFileName(fileName);

            // does this note really exist?
            if (note.isFetched()) {
                // set current note
                setCurrentNote(note);
            }
        }
    } else if (scheme == "task") {
        openTodoDialog(url.host());
    }
}

/*
 * Manually check for updates
 */
void MainWindow::on_actionCheck_for_updates_triggered() {
    this->updateService->checkForUpdates(this, UpdateService::Manual);
}

/*
 * Open the issue page
 */
void MainWindow::on_actionReport_problems_or_ideas_triggered() {
    QDesktopServices::openUrl(QUrl("https://github.com/pbek/QOwnNotes/issues"));
}

void MainWindow::on_actionAlphabetical_triggered(bool checked) {
    if (checked) {
        sortAlphabetically = true;
        ui->noteTreeWidget->sortItems(0, Qt::AscendingOrder);
    }
}

void MainWindow::on_actionBy_date_triggered(bool checked) {
    if (checked) {
        sortAlphabetically = false;
        loadNoteDirectoryList();
    }
}

void MainWindow::systemTrayIconClicked(
        QSystemTrayIcon::ActivationReason reason) {
    // don't show or hide the app on OS X with a simple click because also the
    // context menu will be triggered
#ifndef Q_OS_MAC
    if (reason == QSystemTrayIcon::Trigger) {
        if (isVisible() && !isMinimized()) {
            this->hide();
        } else {
            showWindow();
        }
    }
#endif
}

/**
 * Shows the window (also brings it to the front and un-minimizes it)
 */
void MainWindow::showWindow() {
    show();
    activateWindow();
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    raise();
}

/**
 * Generates the system tray context menu
 */
void MainWindow::generateSystemTrayContextMenu() {
    // trying to destroy the old context menu as fix for Ubuntu 14.04
    // just clearing an existing menu resulted in empty sub-menus
//    QMenu *menu = trayIcon->contextMenu();
//    delete(menu);

    QMenu *menu = new QMenu();
    menu->setTitle("QOwnNotes");

    // add menu entry to open the app
    QAction *openAction = menu->addAction(tr("Open QOwnNotes"));
    openAction->setIcon(getSystemTrayIcon());

    connect(openAction, SIGNAL(triggered()),
            this, SLOT(showWindow()));

    menu->addSeparator();

    // add menu entry to create a new note
    QAction *createNoteAction = menu->addAction(tr("New note"));
    createNoteAction->setIcon(QIcon::fromTheme(
            "document-new",
            QIcon(":icons/breeze-qownnotes/16x16/document-new.svg")));

    connect(createNoteAction, SIGNAL(triggered()),
            this, SLOT(on_action_New_note_triggered()));

    int maxNotes = Note::countAll();

    if (maxNotes > 0) {
        if (maxNotes > 9) {
            maxNotes = 9;
        }

        // add a menu for recent notes
        QMenu *noteMenu = menu->addMenu(tr("Recent notes"));

        QSignalMapper *noteSignalMapper = new QSignalMapper(this);
        QList<Note> noteList = Note::fetchAll(maxNotes);

        Q_FOREACH(Note note, noteList) {
                QAction *action = noteMenu->addAction(note.getName());
                action->setIcon(QIcon::fromTheme(
                        "text-x-generic",
                        QIcon(":icons/breeze-qownnotes/16x16/"
                                      "text-x-generic.svg")));

                QObject::connect(
                        action, SIGNAL(triggered()),
                        noteSignalMapper, SLOT(map()));

                noteSignalMapper->setMapping(
                        action, note.getId());
            }

        // connect the signal mapper
        QObject::connect(noteSignalMapper,
                         SIGNAL(mapped(int)),
                         this,
                         SLOT(setCurrentNoteFromNoteId(int)));
    }

    menu->addSeparator();

    // add menu entry to show the tasks
    QAction *taskAction = menu->addAction(tr("Show todo lists"));
    taskAction->setIcon(QIcon::fromTheme(
            "view-calendar-tasks",
            QIcon(":icons/breeze-qownnotes/16x16/view-calendar-tasks.svg")));

    connect(taskAction, SIGNAL(triggered()),
            this, SLOT(openTodoDialog()));

    QList<CalendarItem> taskList = CalendarItem::fetchAllForSystemTray(10);
    if (taskList.count() > 0) {
        // add a menu for recent tasks
        QMenu *taskMenu = menu->addMenu(tr("Recent tasks"));

        QSignalMapper *taskSignalMapper = new QSignalMapper(this);

        // add menu entries to jump to tasks
        QListIterator<CalendarItem> itr(taskList);
        while (itr.hasNext()) {
            CalendarItem task = itr.next();

            QAction *action = taskMenu->addAction(task.getSummary());
            action->setIcon(QIcon::fromTheme(
                    "view-task",
                    QIcon(":icons/breeze-qownnotes/16x16/view-task.svg")));

            QObject::connect(
                    action, SIGNAL(triggered()),
                    taskSignalMapper, SLOT(map()));

            taskSignalMapper->setMapping(
                    action, task.getUid());
        }

        // connect the signal mapper
        QObject::connect(taskSignalMapper,
                         SIGNAL(mapped(QString)),
                         this,
                         SLOT(openTodoDialog(QString)));
    }

    menu->addSeparator();

    // add menu entry to quit the app
    QAction *quitAction = menu->addAction(tr("Quit"));
    quitAction->setIcon(QIcon::fromTheme(
            "application-exit",
            QIcon(":icons/breeze-qownnotes/16x16/application-exit.svg")));
    connect(quitAction, SIGNAL(triggered()),
            this, SLOT(on_action_Quit_triggered()));

    trayIcon->setContextMenu(menu);
}

void MainWindow::on_actionShow_system_tray_triggered(bool checked) {
    showSystemTray = checked;
    ui->actionStart_hidden->setEnabled(checked);

    if (checked) {
        trayIcon->show();
    } else {
        trayIcon->hide();

        // turn off "Start hidden"
        on_actionStart_hidden_triggered(false);
    }
}

void MainWindow::on_action_Settings_triggered() {
    // open the settings dialog
    openSettingsDialog();
}

void MainWindow::on_actionShow_versions_triggered() {
    // check if we have selected a note
    if (!currentNote.exists()) {
        return;
    }

    ui->actionShow_versions->setDisabled(true);
    showStatusBarMessage(
            tr("Note versions are currently loaded from your ownCloud server"),
            20000);

    OwnCloudService *ownCloud = new OwnCloudService(this);
    ownCloud->loadVersions(this->currentNote.relativeNoteFilePath("/"), this);
}

void MainWindow::enableShowVersionsButton() {
    ui->actionShow_versions->setDisabled(false);
}

void MainWindow::on_actionShow_trash_triggered() {
    ui->actionShow_trash->setDisabled(true);
    showStatusBarMessage(
            tr("Trashed notes are currently loaded from your ownCloud server"),
            20000);

    OwnCloudService *ownCloud = new OwnCloudService(this);
    ownCloud->loadTrash(this);
}

void MainWindow::enableShowTrashButton() {
    ui->actionShow_trash->setDisabled(false);
}

void MainWindow::on_actionSelect_all_notes_triggered() {
    selectAllNotes();
}

/**
 * @brief create the additional menu entries for the note text edit field
 * @param pos
 */
void MainWindow::on_noteTextEdit_customContextMenuRequested(const QPoint &pos) {
    QPoint globalPos = ui->noteTextEdit->mapToGlobal(pos);
    QMenu *menu = ui->noteTextEdit->createStandardContextMenu();

    menu->addSeparator();

    QString linkTextActionName =
            ui->noteTextEdit->textCursor().selectedText() != "" ?
                tr("&Link selected text") : tr("Insert &link");
    QAction *linkTextAction = menu->addAction(linkTextActionName);
    linkTextAction->setShortcut(ui->actionInsert_Link_to_note->shortcut());

    QAction *pasteMediaAction = menu->addAction(tr("Paste HTML or media"));
    pasteMediaAction->setShortcut(ui->actionPaste_image->shortcut());

    QAction *selectedItem = menu->exec(globalPos);
    if (selectedItem) {
        if (selectedItem == linkTextAction) {
            // handle the linking of text with a note
            handleTextNoteLinking();
        } else if (selectedItem == pasteMediaAction) {
            // paste HTML or media into the note
            pasteMediaIntoNote();
        }
    }
}

void MainWindow::on_actionInsert_Link_to_note_triggered() {
    // handle the linking of text with a note
    handleTextNoteLinking();
}

void MainWindow::on_action_DuplicateText_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    textEdit->duplicateText();
}

void MainWindow::on_action_Back_in_note_history_triggered() {
    if (this->noteHistory.back()) {
        ui->searchLineEdit->clear();
        setCurrentNoteFromHistoryItem(
                this->noteHistory.getCurrentHistoryItem());
    }
}

void MainWindow::on_action_Forward_in_note_history_triggered() {
    if (this->noteHistory.forward()) {
        ui->searchLineEdit->clear();
        setCurrentNoteFromHistoryItem(
                this->noteHistory.getCurrentHistoryItem());
    }
}

void MainWindow::on_action_Shortcuts_triggered() {
    QDesktopServices::openUrl(
            QUrl("http://www.qownnotes.org/shortcuts/QOwnNotes"));
}

void MainWindow::on_action_Knowledge_base_triggered() {
    QDesktopServices::openUrl(QUrl("http://www.qownnotes.org/Knowledge-base"));
}

/**
 * @brief Inserts the current date in ISO 8601 format
 */
void MainWindow::on_actionInsert_current_time_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    QDateTime dateTime = QDateTime::currentDateTime();

    // insert the current date in ISO 8601 format
    c.insertText(dateTime.toString(Qt::SystemLocaleShortDate));
}

void MainWindow::on_actionOpen_List_triggered() {
    openTodoDialog();
}

/**
 * @brief Exports the current note as PDF (markdown)
 */
void MainWindow::on_action_Export_note_as_PDF_markdown_triggered() {
    exportNoteAsPDF(ui->noteTextView);
}

/**
 * @brief Exports the current note as PDF (text)
 */
void MainWindow::on_action_Export_note_as_PDF_text_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    exportNoteAsPDF(textEdit);
}

/**
 * @brief Prints the current note (markdown)
 */
void MainWindow::on_action_Print_note_markdown_triggered() {
    printNote(ui->noteTextView);
}

/**
 * @brief Prints the current note (text)
 */
void MainWindow::on_action_Print_note_text_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    printNote(textEdit);
}

/**
 * @brief Inserts a chosen image at the current cursor position in the note text edit
 */
void MainWindow::on_actionInsert_image_triggered() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("Image files (*.jpg *.png *.gif)"));
    dialog.setWindowTitle(tr("Select image to insert"));
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.count() > 0) {
            QString fileName = fileNames.at(0);

            QFile file(fileName);

            // insert the image
            insertMedia(&file);
        }
    }
}

/**
 * Inserts a media file into a note
 */
bool MainWindow::insertMedia(QFile *file) {
    QString text = Note::getInsertMediaMarkdown(file);
    if (!text.isEmpty()) {
        ScriptingService* scriptingService = ScriptingService::instance();
        // attempts to ask a script for an other markdown text
        text = scriptingService->callInsertMediaHook(file, text);
        qDebug() << __func__ << " - 'text': " << text;

        QMarkdownTextEdit* textEdit = activeNoteTextEdit();
        QTextCursor c = textEdit->textCursor();

        // if we try to insert media in the first line of the note (aka.
        // note name) move the cursor to the last line
        if (currentNoteLineNumber() == 1) {
            c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
            textEdit->setTextCursor(c);
        }

        // insert the image link
        c.insertText(text);

        return true;
    }

    return false;
}

/**
 * Returns the cursor's line number in the current note
 */
int MainWindow::currentNoteLineNumber()
{
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor cursor = textEdit->textCursor();

    QTextDocument *doc = textEdit->document();
    QTextBlock blk = doc->findBlock(cursor.position());
    QTextBlock blk2 = doc->begin();

    int i = 1;
    while ( blk != blk2 ) {
        blk2 = blk2.next();
        i++;
    }

    return i;
}

/**
 * @brief Opens a browser with the changelog page
 */
void MainWindow::on_actionShow_changelog_triggered() {
    QDesktopServices::openUrl(
            QUrl("http://www.qownnotes.org/changelog/QOwnNotes"));
}

void MainWindow::on_action_Find_text_in_note_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    textEdit->searchWidget()->activate();
}

/**
 * Asks the user for a password and encrypts the note text with it
 */
void MainWindow::on_action_Encrypt_note_triggered()
{
    currentNote.refetch();

    // return if there the note text is already encrypted
    if (currentNote.hasEncryptedNoteText()) {
        return;
    }

    // the password dialog can be disabled by scripts
    bool dialogDisabled = qApp->property("encryptionPasswordDisabled").toBool();

    if (!dialogDisabled) {
        QString labelText = tr(
            "Please enter your <strong>password</strong> to encrypt the note."
            "<br />Keep in mind that you have to <strong>remember</strong> "
            "your password to read the content of the note<br /> and that you "
            "can <strong>only</strong> do that <strong>in QOwnNotes</strong>!");
        PasswordDialog* dialog = new PasswordDialog(this, labelText, true);
        int dialogResult = dialog->exec();

        // if the user didn't pressed ok return
        if (dialogResult != QDialog::Accepted) {
            return;
        }

        // take the password
        QString password = dialog->password();

        // if password was empty return
        if (password.isEmpty()) {
            return;
        }

        // set the password
        currentNote.setCryptoPassword(password);
        currentNote.store();
    }

    // encrypt the note
    QString noteText = currentNote.encryptNoteText();
    ui->noteTextEdit->setPlainText(noteText);
}

/**
 * Enables or disables the encrypt note buttons
 */
void MainWindow::updateEncryptNoteButtons() {
    currentNote.refetch();
    bool hasEncryptedNoteText = currentNote.hasEncryptedNoteText();

    ui->action_Encrypt_note->setEnabled(!hasEncryptedNoteText);
    ui->actionEdit_encrypted_note->setEnabled(hasEncryptedNoteText);
    ui->actionDecrypt_note->setEnabled(hasEncryptedNoteText);
}

/**
 * Attempt to decrypt note text
 */
void MainWindow::on_actionDecrypt_note_triggered() {
    currentNote.refetch();
    if (!currentNote.hasEncryptedNoteText()) {
        return;
    }

    if (QMessageBox::warning(
            this, tr("Decrypt note and store it as plain text"),
            tr("Your note will be decrypted and stored as plain text gain. Keep "
                    "in mind that the unencrypted note will possibly be synced "
                    "to your server and sensitive text may be exposed!<br />"
                    "Do you want to decrypt your note?"),
            tr("&Decrypt"), tr("&Cancel"), QString::null,
            0, 1) == 1) {
        return;
    }

    askForEncryptedNotePasswordIfNeeded();

    if (currentNote.canDecryptNoteText()) {
        ui->encryptedNoteTextEdit->hide();
        ui->noteTextEdit->setText(currentNote.getDecryptedNoteText());
        ui->noteTextEdit->show();
        ui->noteTextEdit->setFocus();
    }
}

/**
 * Lets the user edit an encrypted note text in a 2nd text edit
 */
void MainWindow::on_actionEdit_encrypted_note_triggered() {
    currentNote.refetch();
    if (!currentNote.hasEncryptedNoteText()) {
        return;
    }

    askForEncryptedNotePasswordIfNeeded(
            tr("<br />You will be able to edit your encrypted note."));

    if (currentNote.canDecryptNoteText()) {
        const QSignalBlocker blocker(ui->encryptedNoteTextEdit);
        Q_UNUSED(blocker);

        ui->noteTextEdit->hide();
        ui->encryptedNoteTextEdit->setText(currentNote.getDecryptedNoteText());
        ui->encryptedNoteTextEdit->show();
        ui->encryptedNoteTextEdit->setFocus();
        _noteViewNeedsUpdate = true;
    }
}

/**
 * Puts the encrypted text back to the note text edit
 */
void MainWindow::on_encryptedNoteTextEdit_textChanged() {
    currentNote.storeNewDecryptedText(ui->encryptedNoteTextEdit->toPlainText());
}

/**
 * Opens the current note in an external editor
 */
void MainWindow::on_action_Open_note_in_external_editor_triggered() {
    QSettings settings;
    QString externalEditorPath =
            settings.value("externalEditorPath").toString();

    // use the default editor if no other editor was set
    if (externalEditorPath.isEmpty()) {
        QUrl url = currentNote.fullNoteFileUrl();
        qDebug() << __func__ << " - 'url': " << url;

        // open note file in default application for the type of file
        QDesktopServices::openUrl(url);
    } else {
        QString path = currentNote.fullNoteFilePath();

        qDebug() << __func__ << " - 'externalEditorPath': " <<
        externalEditorPath;
        qDebug() << __func__ << " - 'path': " << path;

        // open note file in external editor
        Utils::Misc::startDetachedProcess(externalEditorPath,
                                          QStringList() << path);
    }
}

/**
 * Exports the current note as markdown file
 */
void MainWindow::on_action_Export_note_as_markdown_triggered() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("Markdown files (*.md)"));
    dialog.setWindowTitle(tr("Export current note as Markdown file"));
    dialog.selectFile(currentNote.getName() + ".md");
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.count() > 0) {
            QString fileName = fileNames.at(0);

            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(".md");
            }

            QFile file(fileName);

            qDebug() << "exporting note file: " << fileName;

            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qCritical() << file.errorString();
                return;
            }
            QTextStream out(&file);
            out.setCodec("UTF-8");
            out << ui->noteTextEdit->toPlainText();
            file.flush();
            file.close();
        }
    }
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    MetricsService::instance()->sendVisitIfEnabled("dialog/" + objectName());
}

void MainWindow::on_actionGet_invloved_triggered() {
    QDesktopServices::openUrl(
            QUrl("http://www.qownnotes.org/Knowledge-base/"
                         "How-can-I-get-involved-with-QOwnNotes"));
}

/**
 * Sets a note bookmark on bookmark slot 0..9
 */
void MainWindow::storeNoteBookmark(int slot) {
    // return if note text edit doesn't have the focus
    if (!ui->noteTextEdit->hasFocus()) {
        return;
    }

    QTextCursor c = ui->noteTextEdit->textCursor();
    NoteHistoryItem item = NoteHistoryItem(&currentNote, c.position());
    noteBookmarks[slot] = item;

    showStatusBarMessage(
            tr("Bookmarked note position at slot %1").arg(
                    QString::number(slot)), 3000);
}

/**
 * Loads and jumps to a note bookmark from bookmark slot 0..9
 */
void MainWindow::gotoNoteBookmark(int slot) {
    NoteHistoryItem item = noteBookmarks[slot];

    // check if the note (still) exists
    if (item.getNote().exists()) {
        ui->noteTextEdit->setFocus();
        setCurrentNoteFromHistoryItem(item);

        showStatusBarMessage(
                tr("Jumped to bookmark position at slot %1").arg(
                        QString::number(slot)), 3000);
    }
}

/**
 * Inserts a code block at the current cursor position
 */
void MainWindow::on_actionInset_code_block_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    QString selectedText = c.selection().toPlainText();

    if (selectedText.isEmpty()) {
        c.insertText("``");
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
        textEdit->setTextCursor(c);
    } else {
        bool addNewline = false;

        // if the selected text has multiple lines add a multi-line code block
        if (selectedText.contains("\n")) {
            // add an other newline if there is no newline at the end of the
            // selected text
            QString endNewline = selectedText.endsWith("\n") ? "" : "\n";

            selectedText = "``\n" + selectedText + endNewline + "``";
            addNewline = true;
        }

        c.insertText("`" + selectedText + "`");

        if (addNewline) {
            c.insertText("\n");
        }
    }
}

void MainWindow::on_actionNext_note_triggered() {
    gotoNextNote();
}

/**
 * Jumps to the next visible note
 */
void MainWindow::gotoNextNote() {
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down,
                                    Qt::NoModifier);
    QApplication::postEvent(ui->noteTreeWidget, event);
}

void MainWindow::on_actionPrevious_Note_triggered() {
    gotoPreviousNote();
}

/**
 * Jumps to the previous visible note
 */
void MainWindow::gotoPreviousNote()
{
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Up,
                                     Qt::NoModifier);
    QApplication::postEvent(ui->noteTreeWidget, event);
}

void MainWindow::on_actionToggle_distraction_free_mode_triggered()
{
    toggleDistractionFreeMode();
}

/**
 * Tracks an action
 */
void MainWindow::trackAction(QAction *action) {
    if (action == Q_NULLPTR) {
        return;
    }

    MetricsService::instance()->sendVisitIfEnabled(
            "action/" + action->objectName());
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    ui->noteTextEdit->setPaperMargins(event->size().width());
    ui->encryptedNoteTextEdit->setPaperMargins(event->size().width());
    ui->tagTreeWidget->resizeColumnToContents(0);
    ui->tagTreeWidget->resizeColumnToContents(1);
    ui->noteSubFolderTreeWidget->resizeColumnToContents(0);
    ui->noteSubFolderTreeWidget->resizeColumnToContents(1);
}

/**
 * Toggles the visibility of the toolbars
 */
void MainWindow::on_actionShow_toolbar_triggered(bool checked) {
    Q_FOREACH(QToolBar *toolbar, findChildren<QToolBar*>()) {
            toolbar->setVisible(checked);
        }
}

/**
 * Toggles the checked state of the "show toolbar" checkbox in the main menu
 */
void MainWindow::toolbarVisibilityChanged(bool visible) {
    Q_UNUSED(visible);

    const QSignalBlocker blocker(ui->actionShow_toolbar);
    {
        Q_UNUSED(blocker);
        ui->actionShow_toolbar->setChecked(isToolbarVisible());
    }
}

/**
 * Checks if at least one toolbar is visible
 */
bool MainWindow::isToolbarVisible() {
    Q_FOREACH(QToolBar *toolbar, findChildren<QToolBar*>()) {
            if (toolbar->isVisible()) {
                return true;
            }
        }

    return false;
}

void MainWindow::dfmEditorWidthActionTriggered(QAction *action) {
    QSettings settings;
    settings.setValue("DistractionFreeMode/editorWidthMode",
                      action->whatsThis().toInt());

    ui->noteTextEdit->setPaperMargins(this->width());
    ui->encryptedNoteTextEdit->setPaperMargins(this->width());
}

/**
 * Allows files to be dropped to QOwnNotes
 */
void MainWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

/**
 * Handles the copying of notes to the current notes folder
 */
void MainWindow::dropEvent(QDropEvent *e) {
    handleInsertingFromMimeData(e->mimeData());
}

/**
 * Handles the inserting of media files and notes from a mime data, for example
 * produced by a drop event or a paste action
 */
void MainWindow::handleInsertingFromMimeData(const QMimeData *mimeData) {
    // check if a QML wants to set the inserted text
    if (mimeData->hasText() || mimeData->hasHtml()) {
        ScriptingService* scriptingService = ScriptingService::instance();
        QString text = scriptingService->callInsertingFromMimeDataHook(
                mimeData);

        if (!text.isEmpty()) {
            QMarkdownTextEdit* textEdit = activeNoteTextEdit();
            QTextCursor c = textEdit->textCursor();

            // insert text from QML
            c.insertText(text);

            return;
        }
    }

    if (mimeData->hasHtml()) {
        insertHtml(mimeData->html());
    } else if (mimeData->hasUrls()) {
        int successCount = 0;
        int failureCount = 0;
        int skipCount = 0;

        foreach(const QUrl &url, mimeData->urls()) {
                QString path(url.toLocalFile());
                QFileInfo fileInfo(path);
                qDebug() << __func__ << " - 'path': " << path;

                if (fileInfo.isReadable()) {
                    QFile *file = new QFile(path);

                    // only allow markdown and text files to be copied as note
                    if (isValidNoteFile(file)) {
                        // copy file to notes path
                        bool success = file->copy(
                                notesPath + QDir::separator() +
                                fileInfo.fileName());

                        if (success) {
                            successCount++;
                        } else {
                            failureCount++;
                        }
                    // only allow image files to be inserted as image
                    } else if (isValidMediaFile(file)) {
                        showStatusBarMessage(tr("Inserting image"));

                        // insert the image
                        insertMedia(file);

                        showStatusBarMessage(tr("Done inserting image"), 3000);
                    } else {
                        skipCount++;
                    }
                } else {
                    skipCount++;
                }
            }

        QString message;
        if (successCount > 0) {
            message += tr("Copied %n note(s) to %1", "", successCount)
                    .arg(notesPath);
        }

        if (failureCount > 0) {
            if (!message.isEmpty()) {
                message += ", ";
            }

            message += tr(
                    "Failed to copy %n note(s) (most likely already existing)",
                    "", failureCount);
        }

        if (skipCount > 0) {
            if (!message.isEmpty()) {
                message += ", ";
            }

            message += tr(
                    "Skipped copying of %n note(s) "
                            "(no markdown or text file or not readable)",
                    "", skipCount);
        }

        if (!message.isEmpty()) {
            showStatusBarMessage(message, 5000);
        }
    } else if (mimeData->hasImage()) {
        // get the image from mime data
        QImage image = mimeData->imageData().value<QImage>();

        if (!image.isNull()) {
            showStatusBarMessage(tr("Saving temporary image"));

            QTemporaryFile tempFile(
                    QDir::tempPath() + QDir::separator() +
                    "qownnotes-media-XXXXXX.png");

            if (tempFile.open()) {
                // save temporary png image
                image.save(tempFile.fileName(), "PNG");

                // insert media into note
                QFile *file = new QFile(tempFile.fileName());

                showStatusBarMessage(tr("Inserting image"));
                insertMedia(file);

                showStatusBarMessage(tr("Done inserting image"), 3000);
            } else {
                showStatusBarMessage(
                        tr("Temporary file can't be opened"), 3000);
            }
        }
    }
}

/**
 * Inserts html as markdown in the current note
 * Images are also downloaded
 */
void MainWindow::insertHtml(QString html) {
    qDebug() << __func__ << " - 'html': " << html;

    // convert html tags to markdown
    html = Utils::Misc::htmlToMarkdown(html);

    // match image tags
    QRegularExpression re("<img.+?src=\"(.+?)\".*?>",
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(html);

    // find, download locally and replace all images
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString imageTag = match.captured(0);
        QUrl imageUrl = QUrl(match.captured(1));

        qDebug() << __func__ << " - 'imageUrl': " << imageUrl;

        if (!imageUrl.isValid()) {
            continue;
        }

        showStatusBarMessage(tr("Downloading %1").arg(imageUrl.toString()));

        // try to get the suffix from the url
        QString suffix =
                imageUrl.toString().split(".", QString::SkipEmptyParts).last();
        if (suffix.isEmpty()) {
            suffix = "image";
        }

        // remove strings like "?b=16068071000" from the suffix
        suffix.remove(QRegularExpression("\\?.+$"));

        QTemporaryFile *tempFile = new QTemporaryFile(
                QDir::tempPath() + QDir::separator() + "media-XXXXXX." +
                        suffix);

        if (tempFile->open()) {
            // download the image to the temporary file
            if (downloadUrlToFile(imageUrl, tempFile)) {
                // copy image to media folder and generate markdown code for
                // the image
                QString markdownCode = Note::getInsertMediaMarkdown(tempFile);
                if (!markdownCode.isEmpty()) {
                    // replace image tag with markdown code
                    html.replace(imageTag, markdownCode);
                }
            }
        }
    }

    showStatusBarMessage(tr("Done downloading images"));

    // remove all html tags
    html.remove(QRegularExpression("<.+?>"));

    // remove the last character, that is broken
    html = html.left(html.size() - 1);

    qDebug() << __func__ << " - 'html': " << html;

    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    c.insertText(html);
}

/**
 * Evaluates if file is a note file
 */
bool MainWindow::isValidMediaFile(QFile *file) {
    QStringList mediaExtensions = QStringList() << "jpg" << "png" << "gif";
    QFileInfo fileInfo(file->fileName());
    QString extension = fileInfo.suffix();
    return mediaExtensions.contains(extension, Qt::CaseInsensitive);
}

/**
 * Evaluates if file is a media file
 */
bool MainWindow::isValidNoteFile(QFile *file) {
    QStringList mediaExtensions = QStringList() << "txt" << "md";

    // append the custom extensions
    mediaExtensions.append(Note::customNoteFileExtensionList());

    QFileInfo fileInfo(file->fileName());
    QString extension = fileInfo.suffix();
    return mediaExtensions.contains(extension, Qt::CaseInsensitive);
}

void MainWindow::on_actionPaste_image_triggered()
{
    pasteMediaIntoNote();
}

/**
 * Handles the pasting of media into notes
 */
void MainWindow::pasteMediaIntoNote() {
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData * mimeData = clipboard->mimeData(QClipboard::Clipboard);
    handleInsertingFromMimeData(mimeData);
}

void MainWindow::on_actionShow_note_in_file_manager_triggered() {
    Utils::Misc::openFolderSelect(currentNote.fullNoteFilePath());
}

/**
 * Inserts a bold block at the current cursor position
 */
void MainWindow::on_actionFormat_text_bold_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    QString selectedText = textEdit->textCursor().selectedText();

    if (selectedText.isEmpty()) {
        c.insertText("****");
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 2);
        textEdit->setTextCursor(c);
    } else {
        c.insertText("**" + selectedText + "**");
    }
}

/**
 * Inserts an italic block at the current cursor position
 */
void MainWindow::on_actionFormat_text_italic_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    QString selectedText = textEdit->textCursor().selectedText();

    if (selectedText.isEmpty()) {
        c.insertText("**");
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
        textEdit->setTextCursor(c);
    } else {
        c.insertText("*" + selectedText + "*");
    }
}

/**
 * Increases the note text font size by one
 */
void MainWindow::on_action_Increase_note_text_size_triggered() {
    int fontSize = ui->noteTextEdit
            ->modifyFontSize(QOwnNotesMarkdownTextEdit::Increase);
    ui->encryptedNoteTextEdit->setStyles();
    ui->encryptedNoteTextEdit->highlighter()->parse();
    showStatusBarMessage(
            tr("Increased font size to %1 pt").arg(fontSize), 3000);
}

/**
 * Decreases the note text font size by one
 */
void MainWindow::on_action_Decrease_note_text_size_triggered() {
    int fontSize = ui->noteTextEdit
            ->modifyFontSize(QOwnNotesMarkdownTextEdit::Decrease);
    ui->encryptedNoteTextEdit->setStyles();
    ui->encryptedNoteTextEdit->highlighter()->parse();
    showStatusBarMessage(
            tr("Decreased font size to %1 pt").arg(fontSize), 3000);
}

/**
 * Resets the note text font size
 */
void MainWindow::on_action_Reset_note_text_size_triggered() {
    int fontSize = ui->noteTextEdit
            ->modifyFontSize(QOwnNotesMarkdownTextEdit::Reset);
    ui->encryptedNoteTextEdit->setStyles();
    ui->encryptedNoteTextEdit->highlighter()->parse();
    showStatusBarMessage(tr("Reset font size to %1 pt").arg(fontSize), 3000);

    QTextCursor cursor(ui->noteTextEdit->document()->findBlockByNumber(3));

    QTextCursor c = ui->noteTextEdit->textCursor();
    c.setPosition(cursor.position());
    ui->noteTextEdit->setTextCursor(c);
}

/**
 * Sets the note folder from the recent note folder combobox
 */
void MainWindow::on_noteFolderComboBox_currentIndexChanged(int index) {
    int noteFolderId = ui->noteFolderComboBox->itemData(index).toInt();
    NoteFolder noteFolder = NoteFolder::fetch(noteFolderId);
    if (noteFolder.isFetched()) {
        changeNoteFolder(noteFolderId);
    }

    // hide the note folder combobox if it should not be visible
    hideNoteFolderComboBoxIfNeeded();
}

/**
 * Hides the note folder combobox if it should not be visible
 */
void MainWindow::hideNoteFolderComboBoxIfNeeded() {
    if (!_noteFolderDockWidgetWasVisible) {
        _noteFolderDockWidget->hide();
        _noteFolderDockWidgetWasVisible = true;
    }
}

/**
 * Reloads the tag tree
 */
void MainWindow::reloadTagTree() {
    qDebug() << __func__;

    ui->tagTreeWidget->clear();

    // add an item to view all notes
    int linkCount = Note::countAll();
    QString toolTip = tr("show all notes (%1)").arg(QString::number(linkCount));

    QTreeWidgetItem *allItem = new QTreeWidgetItem();
    allItem->setText(0, tr("All notes"));
    allItem->setTextColor(1, QColor(Qt::gray));
    allItem->setText(1, QString::number(linkCount));
    allItem->setToolTip(0, toolTip);
    allItem->setToolTip(1, toolTip);
    allItem->setData(0, Qt::UserRole, Tag::AllNotesId);
    allItem->setFlags(allItem->flags() & ~Qt::ItemIsSelectable);
    allItem->setIcon(0, QIcon::fromTheme(
            "edit-copy",
            QIcon(":icons/breeze-qownnotes/16x16/edit-copy.svg")));
    ui->tagTreeWidget->addTopLevelItem(allItem);

    // add an empty item
//    QTreeWidgetItem *emptyItem = new QTreeWidgetItem();
//    emptyItem->setData(0, Qt::UserRole, 0);
//    emptyItem->setFlags(allItem->flags() & ~Qt::ItemIsSelectable);
//    ui->tagTreeWidget->addTopLevelItem(emptyItem);

    // add all tags recursively as items
    buildTagTreeForParentItem();

    // add an item to view untagged notes if there are any
    linkCount = Note::countAllNotTagged();

    if (linkCount > 0) {
        toolTip = tr("show all untagged notes (%1)")
                .arg(QString::number(linkCount));
        QTreeWidgetItem *untaggedItem = new QTreeWidgetItem();
        untaggedItem->setText(0, tr("Untagged notes"));
        untaggedItem->setTextColor(1, QColor(Qt::gray));
        untaggedItem->setText(1, QString::number(linkCount));
        untaggedItem->setToolTip(0, toolTip);
        untaggedItem->setToolTip(1, toolTip);
        untaggedItem->setData(0, Qt::UserRole, Tag::AllUntaggedNotesId);
        untaggedItem->setFlags(untaggedItem->flags() & ~Qt::ItemIsSelectable);
        untaggedItem->setIcon(0, QIcon::fromTheme(
                "edit-copy",
                QIcon(":icons/breeze-qownnotes/16x16/edit-copy.svg")));
        ui->tagTreeWidget->addTopLevelItem(untaggedItem);
    }

    // decorate root of there are multiple levels to be able to expand them
    ui->tagTreeWidget->setRootIsDecorated(
            Tag::countAllParentId(0) != Tag::countAll());

    ui->tagTreeWidget->resizeColumnToContents(0);
    ui->tagTreeWidget->resizeColumnToContents(1);
    ui->tagTreeWidget->expandAll();
}

/**
 * Reloads the note sub folder tree
 */
void MainWindow::reloadNoteSubFolderTree() {
    ui->noteSubFolderTreeWidget->clear();
    int activeNoteSubFolderId = NoteSubFolder::activeNoteSubFolderId();

    // add the "all notes" item
    int linkCount = Note::countAll();
    QString toolTip = tr("show notes from all note subfolders (%1)")
            .arg(QString::number(linkCount));

    QTreeWidgetItem *allItem = new QTreeWidgetItem();
    allItem->setText(0, tr("All notes"));
    allItem->setData(0, Qt::UserRole, -1);
    allItem->setToolTip(0, toolTip);
    allItem->setIcon(0, QIcon::fromTheme(
            "edit-copy",
            QIcon(":icons/breeze-qownnotes/16x16/edit-copy.svg")));
    allItem->setTextColor(1, QColor(Qt::gray));
    allItem->setText(1, QString::number(linkCount));
    allItem->setToolTip(1, toolTip);
    allItem->setFlags(allItem->flags() & ~Qt::ItemIsSelectable);
    ui->noteSubFolderTreeWidget->addTopLevelItem(allItem);

    // add the "note folder" item
    linkCount = Note::countByNoteSubFolderId(0);
    toolTip = tr("show notes in note root folder (%1)")
            .arg(QString::number(linkCount));

    QTreeWidgetItem *item = new QTreeWidgetItem();
    item->setText(0, tr("Note folder"));
    item->setData(0, Qt::UserRole, 0);
    item->setToolTip(0, toolTip);
    item->setIcon(0, QIcon::fromTheme(
            "folder",
            QIcon(":icons/breeze-qownnotes/16x16/folder.svg")));
    item->setTextColor(1, QColor(Qt::gray));
    item->setText(1, QString::number(linkCount));
    item->setToolTip(1, toolTip);
    ui->noteSubFolderTreeWidget->addTopLevelItem(item);

    // set the active item
    if (activeNoteSubFolderId == 0) {
        const QSignalBlocker blocker(ui->noteSubFolderTreeWidget);
        Q_UNUSED(blocker);

        ui->noteSubFolderTreeWidget->setCurrentItem(item);
    }

    // add all note sub folders recursively as items
    buildNoteSubFolderTreeForParentItem();

    ui->noteSubFolderTreeWidget->resizeColumnToContents(0);
    ui->noteSubFolderTreeWidget->resizeColumnToContents(1);
}

/**
 * Populates the note sub folder tree recursively with its items
 */
void MainWindow::buildNoteSubFolderTreeForParentItem(QTreeWidgetItem *parent) {
    int parentId = parent == NULL ? 0 : parent->data(0, Qt::UserRole).toInt();
    int activeNoteSubFolderId = NoteSubFolder::activeNoteSubFolderId();

    QList<NoteSubFolder> noteSubFolderList =
            NoteSubFolder::fetchAllByParentId(parentId);

    // build the next level of note sub folders
    Q_FOREACH(NoteSubFolder noteSubFolder, noteSubFolderList) {
            QTreeWidgetItem *item =
                    addNoteSubFolderToTreeWidget(parent,
                                                 noteSubFolder);
            // set the active item
            if (activeNoteSubFolderId == noteSubFolder.getId()) {
                const QSignalBlocker blocker(ui->noteSubFolderTreeWidget);
                Q_UNUSED(blocker);

                ui->noteSubFolderTreeWidget->setCurrentItem(item);
            }

            buildNoteSubFolderTreeForParentItem(item);

            // set the expanded state
            bool isExpanded = noteSubFolder.treeWidgetExpandState();
            item->setExpanded(isExpanded);
        }
}

/**
 * Populates the tag tree recursively with its tags
 */
void MainWindow::buildTagTreeForParentItem(QTreeWidgetItem *parent) {
    int parentId = parent == NULL ? 0 : parent->data(0, Qt::UserRole).toInt();
    int activeTagId = Tag::activeTagId();

    QList<Tag> tagList = Tag::fetchAllByParentId(parentId);
    Q_FOREACH(Tag tag, tagList) {
            QTreeWidgetItem *item = addTagToTagTreeWidget(parent, tag);

            // set the active item
            if (activeTagId == tag.getId()) {
                const QSignalBlocker blocker(ui->tagTreeWidget);
                Q_UNUSED(blocker);

                ui->tagTreeWidget->setCurrentItem(item);
            }

            // recursively populate the next level
            buildTagTreeForParentItem(item);
        }

    // update the UI
    // this will crash the app sporadically
    //QCoreApplication::processEvents();
}

/**
 * Ads a tag to the tag tree widget
 */
QTreeWidgetItem *MainWindow::addTagToTagTreeWidget(
        QTreeWidgetItem *parent, Tag tag) {
    int parentId = parent == NULL ? 0 : parent->data(0, Qt::UserRole).toInt();

    if (parentId < 0) {
        parentId = 0;
    }

    int tagId = tag.getId();

    QTreeWidgetItem *item = new QTreeWidgetItem();
    QString name = tag.getName();
    int linkCount = tag.countLinkedNoteFileNames();
    QString toolTip = tr("show all notes tagged with '%1' (%2)")
                    .arg(name, QString::number(linkCount));
    item->setData(0, Qt::UserRole, tagId);
    item->setText(0, name);
    item->setText(1, linkCount > 0 ? QString::number(linkCount) : "");
    item->setTextColor(1, QColor(Qt::gray));
    item->setIcon(0, QIcon::fromTheme(
                    "tag", QIcon(":icons/breeze-qownnotes/16x16/tag.svg")));
    item->setToolTip(0, toolTip);
    item->setToolTip(1, toolTip);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    // set the color of the tag tree widget item
    handleTreeWidgetItemTagColor(item, tag);

    if (parentId == 0) {
        // add the item at top level if there was no parent item
        ui->tagTreeWidget->addTopLevelItem(item);
    } else {
        // add the item as child of the parent
        parent->addChild(item);
    }

    return item;
}

/**
 * Reads the color from a tag and sets the background color of a tree widget
 * item
 *
 * @param item
 * @param tag
 */
void MainWindow::handleTreeWidgetItemTagColor(QTreeWidgetItem *item,
                                              Tag &tag) const {
    if (item == Q_NULLPTR) {
        return;
    }

    int columnCount = item->columnCount();

    if (columnCount == 0) {
        return;
    }

    // get the color from the tag
    QColor color = tag.getColor();

    // if no color was set reset it by using a transparent white
    if (!color.isValid()) {
        color = QColor(255, 255, 255, 0);
    }

    QBrush brush = QBrush(color);

    // the tree widget events have to be blocked because when called in
    // assignColorToTagItem() the 2nd setBackground() crashes the app,
    // because it seems the tag tree will be reloaded
    const QSignalBlocker blocker(item->treeWidget());
    Q_UNUSED(blocker);

    // set the color for all columns
    for (int column = 0; column < columnCount; column++) {
        item->setBackground(column, brush);
    }
}

/**
 * Creates a new tag
 */
void MainWindow::on_tagLineEdit_returnPressed() {
    QString name = ui->tagLineEdit->text();
    if (name.isEmpty()) {
        return;
    }

    const QSignalBlocker blocker(this->noteDirectoryWatcher);
    Q_UNUSED(blocker);

    Tag tag;
    tag.setName(name);

    if (tag.store()) {
        const QSignalBlocker blocker2(ui->tagLineEdit);
        Q_UNUSED(blocker2);

        // clear the line edit if the tag was stored
        ui->tagLineEdit->clear();
    }

    reloadTagTree();
}

/**
 * Filters tags in the tag tree widget
 */
void MainWindow::on_tagLineEdit_textChanged(const QString &arg1)
{
    searchForTextInTreeWidget(ui->tagTreeWidget, arg1);
}

/**
 * Checks if there is at least one child that is visible
 */
bool MainWindow::isOneTreeWidgetItemChildVisible(QTreeWidgetItem *item) {
    for (int i = 0; i < item->childCount(); i++) {
        QTreeWidgetItem *child = item->child(i);
        if (!child->isHidden() || isOneTreeWidgetItemChildVisible(child)) {
            return true;
        }
    }

    return false;
}

/**
 * Shows or hides everything for the note tags
 */
void MainWindow::setupTags() {
    ui->newNoteTagLineEdit->setVisible(false);
    ui->newNoteTagButton->setVisible(true);

#ifdef Q_OS_MAC
    // try to compensate for the different button top margins in OS X
    ui->noteTagFrame->layout()->setContentsMargins(0, 0, 0, 0);
    ui->noteTagButtonFrame->layout()->setContentsMargins(0, 8, 0, 0);
#else
    // we want the tag frame as small as possible
    ui->noteTagFrame->layout()->setContentsMargins(8, 0, 8, 0);
#endif

    reloadTagTree();
    ui->tagTreeWidget->expandAll();
    reloadCurrentNoteTags();

    // filter the notes again
    filterNotes(false);
}

/**
 * Shows or hides everything for the note sub folders
 */
void MainWindow::setupNoteSubFolders() {
    bool showSubfolders = NoteFolder::isCurrentShowSubfolders();

    // handle the visibility of the note subfolder panel
    handleNoteSubFolderVisibility();

    // we only want to see that menu entry if there are note subfolders
    ui->actionFind_notes_in_all_subfolders->setVisible(showSubfolders);

    if (showSubfolders) {
        reloadNoteSubFolderTree();
    }

    // filter the notes again
    filterNotes(false);
}

/**
 * Hides the note tag add button and shows the text edit
 */
void MainWindow::on_newNoteTagButton_clicked() {
    ui->newNoteTagLineEdit->setVisible(true);
    ui->newNoteTagLineEdit->setFocus();
    ui->newNoteTagLineEdit->selectAll();
    ui->newNoteTagButton->setVisible(false);

    // add tag name auto-completion
    QStringList wordList = Tag::fetchAllNames();
    QCompleter *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->newNoteTagLineEdit->setCompleter(completer);
}

/**
 * Links a note to the tag entered after pressing return
 * in the note tag line edit
 */
void MainWindow::on_newNoteTagLineEdit_returnPressed() {
    QString text = ui->newNoteTagLineEdit->text();
    linkTagNameToCurrentNote(text);
}

/**
 * Links a tag to the current note
 *
 * @param tagName
 */
void MainWindow::linkTagNameToCurrentNote(QString tagName) {
    if (tagName.isEmpty()) {
        return;
    }

    // create a new tag if it doesn't exist
    Tag tag = Tag::fetchByName(tagName);
    if (!tag.isFetched()) {
        const QSignalBlocker blocker(noteDirectoryWatcher);
        Q_UNUSED(blocker);

        tag.setName(tagName);
        tag.store();
    }

    // link the current note to the tag
    if (tag.isFetched()) {
        const QSignalBlocker blocker(noteDirectoryWatcher);
        Q_UNUSED(blocker);

        tag.linkToNote(currentNote);
        reloadCurrentNoteTags();
        reloadTagTree();

        // handle the coloring of the note in the note tree widget
        handleNoteTreeTagColoringForNote(currentNote);
    }
}

/**
 * Hides the note tag line edit after editing
 */
void MainWindow::on_newNoteTagLineEdit_editingFinished() {
    ui->newNoteTagLineEdit->setVisible(false);
    ui->newNoteTagButton->setVisible(true);
}

/**
 * Reloads the note tag buttons for the current note
 */
void MainWindow::reloadCurrentNoteTags() {
    // remove all remove-tag buttons
    QLayoutItem *child;
    while ((child = ui->noteTagButtonFrame->layout()->takeAt(0)) != 0) {
        delete child->widget();
        delete child;
    }

    // add all new remove-tag buttons
    QList<Tag> tagList = Tag::fetchAllOfNote(currentNote);
    Q_FOREACH(Tag tag, tagList) {
            QPushButton* button = new QPushButton(
                    Utils::Misc::shorten(tag.getName(), 25),
                    ui->noteTagButtonFrame);
            button->setIcon(QIcon::fromTheme(
                    "tag-delete",
                    QIcon(":icons/breeze-qownnotes/16x16/"
                                  "xml-attribute-delete.svg")));
            button->setToolTip(
                    tr("remove tag '%1' from note").arg(tag.getName()));
            button->setObjectName(
                    "removeNoteTag" + QString::number(tag.getId()));

            QObject::connect(button, SIGNAL(clicked()),
                             this, SLOT(removeNoteTagClicked()));

            ui->noteTagButtonFrame->layout()->addWidget(button);
        }

    // add a spacer to prevent the button items to take the full width
    QSpacerItem *spacer = new QSpacerItem(0, 20,
                                          QSizePolicy::MinimumExpanding,
                                          QSizePolicy::Ignored);
    ui->noteTagButtonFrame->layout()->addItem(spacer);
}

/**
 * Removes a note tag link
 */
void MainWindow::removeNoteTagClicked() {
    QString objectName = sender()->objectName();
    if (objectName.startsWith("removeNoteTag")) {
        int tagId = objectName.remove("removeNoteTag").toInt();
        Tag tag = Tag::fetch(tagId);
        if (!tag.isFetched()) {
            return;
        }

        const QSignalBlocker blocker(noteDirectoryWatcher);
        Q_UNUSED(blocker);

        tag.removeLinkToNote(currentNote);
        reloadCurrentNoteTags();
        reloadTagTree();

        // handle the coloring of the note in the note tree widget
        handleNoteTreeTagColoringForNote(currentNote);
    }
}

/**
 * Allows the user to add a tag to the current note
 */
void MainWindow::on_action_new_tag_triggered() {
    on_newNoteTagButton_clicked();
}

/**
 * Reloads the current note folder
 */
void MainWindow::on_action_Reload_note_folder_triggered() {
    // force build and load
    buildNotesIndexAndLoadNoteDirectoryList(true, true);
    currentNote.refetch();
    setNoteTextFromNote(&currentNote);
}

/**
 * Stores the tag after it was edited
 */
void MainWindow::on_tagTreeWidget_itemChanged(
        QTreeWidgetItem *item, int column) {
    Q_UNUSED(column);

    Tag tag = Tag::fetch(item->data(0, Qt::UserRole).toInt());
    if (tag.isFetched()) {
        QString name = item->text(0);

        if (!name.isEmpty()) {
            const QSignalBlocker blocker(this->noteDirectoryWatcher);
            Q_UNUSED(blocker);

            tag.setName(name);
            tag.store();
        }

        // we also have to reload the tag tree if we don't change the tag
        // name to get the old name back
        reloadTagTree();
    }
}

/**
 * Sets a new active tag
 */
void MainWindow::on_tagTreeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    if (current == NULL) {
        return;
    }

    // set the tag id as active
    int tagId = current->data(0, Qt::UserRole).toInt();
    Tag::setAsActive(tagId);

    const QSignalBlocker blocker(ui->searchLineEdit);
    Q_UNUSED(blocker);

    ui->searchLineEdit->clear();

    filterNotes();
}

/**
 * Creates a context menu for the tag tree widget
 */
void MainWindow::on_tagTreeWidget_customContextMenuRequested(
        const QPoint &pos) {
    // don't open the most of the context menu if no tags are selected
    bool hasSelected = ui->tagTreeWidget->selectedItems().count() > 0;

    QPoint globalPos = ui->tagTreeWidget->mapToGlobal(pos);
    QMenu menu;

    QAction *addAction = menu.addAction(tr("&Add tag"));
    QAction *editAction;
    QAction *assignColorAction;
    QAction *disableColorAction;
    QAction *removeAction;

    // allow these actions only if tags are selected
    if (hasSelected) {
        editAction = menu.addAction(tr("&Edit tag"));
        assignColorAction = menu.addAction(tr("Assign color"));
        disableColorAction = menu.addAction(tr("Disable color"));
        removeAction = menu.addAction(tr("&Remove tags"));

        // build the tag moving menu
        QMenu *moveMenu = menu.addMenu(tr("&Move tags to..."));
        buildTagMoveMenuTree(moveMenu);
    }

    QAction *selectedItem = menu.exec(globalPos);

    if (selectedItem == NULL) {
        return;
    }

    QTreeWidgetItem *item = ui->tagTreeWidget->currentItem();

    if (selectedItem == addAction) {
        // open the "add new tag" dialog
        TagAddDialog *dialog = new TagAddDialog(this);
        int dialogResult = dialog->exec();

        // if user pressed ok take the name
        if (dialogResult == QDialog::Accepted) {
            QString name = dialog->name();
            if (!name.isEmpty()) {
                int parentId = item->data(0, Qt::UserRole).toInt();
                if (parentId < 0) {
                    parentId = 0;
                }

                // create a new tag with the name
                Tag tag;
                tag.setParentId(parentId);
                tag.setName(name);
                tag.store();

                if (tag.isFetched()) {
                    reloadTagTree();
                } else {
                    showStatusBarMessage(tr("Tag could not be created!"), 3000);
                }
            }
        }

        return;
    }

    if (selectedItem == assignColorAction) {
        // assign and store a color to all selected tags in the tag tree widget
        assignColorToSelectedTagItems();
        return;
    } else if (selectedItem ==
            disableColorAction) {
        // disable the color of all selected tags
        Q_FOREACH(QTreeWidgetItem *tagItem,
                  ui->tagTreeWidget->selectedItems()) {
                // disable the color of the tag
                disableColorOfTagItem(tagItem);
            }

        // reload the notes in the note tree widget to update the colors
        loadNoteDirectoryList();
        return;
    }

    // don't allow clicking on non-tag items for removing, editing and colors
    if (item->data(0, Qt::UserRole) <= 0) {
        return;
    }

    if (selectedItem == removeAction) {
        // remove selected tag
        removeSelectedTags();
    } else if (selectedItem == editAction) {
        ui->tagTreeWidget->editItem(item);
    }
}

/**
 * Assigns and stores a color to a tag from the tag tree widget
 *
 * @param item
 */
void MainWindow::assignColorToTagItem(QTreeWidgetItem *item) {
    int tagId = item->data(0, Qt::UserRole).toInt();

    if (tagId <= 0) {
        return;
    }

    Tag tag = Tag::fetch(tagId);

    if (!tag.isFetched()) {
        return;
    }

    QColor color = tag.getColor();
    color = QColorDialog::getColor(color.isValid() ? color : QColor(Qt::white));

    if (color.isValid()) {
        tag.setColor(color);
        tag.store();

        // set the color of the tag tree widget item
        handleTreeWidgetItemTagColor(item, tag);

        // reload the notes in the note tree widget to update the colors
        loadNoteDirectoryList();
    }
}

/**
 * Assigns and stores a color to all selected tags from the tag tree widget
 */
void MainWindow::assignColorToSelectedTagItems() {
    QColor color;
    bool hasTags = false;

    // get the color of a selected tag
    Q_FOREACH(QTreeWidgetItem *item,
              ui->tagTreeWidget->selectedItems()) {
            int tagId = item->data(0, Qt::UserRole).toInt();
            if (tagId > 0) {
                Tag tag = Tag::fetch(tagId);

                if (!tag.isFetched()) {
                    continue;
                }

                color = tag.getColor();
                hasTags = true;
                break;
            }
        }

    if (!hasTags) {
        return;
    }

    color = QColorDialog::getColor(color.isValid() ? color : QColor(Qt::white));

    // store the color to all selected tags
    if (color.isValid()) {
        Q_FOREACH(QTreeWidgetItem *item,
                  ui->tagTreeWidget->selectedItems()) {
                int tagId = item->data(0, Qt::UserRole).toInt();
                if (tagId <= 0) {
                    continue;
                }

                Tag tag = Tag::fetch(tagId);

                if (!tag.isFetched()) {
                    continue;
                }

                tag.setColor(color);
                tag.store();

                // set the color of the tag tree widget item
                handleTreeWidgetItemTagColor(item, tag);
            }
    }

    // reload the notes in the note tree widget to update the colors
    loadNoteDirectoryList();
}

/**
 * Disables a color of a tag from the tag tree widget
 *
 * @param item
 */
void MainWindow::disableColorOfTagItem(QTreeWidgetItem *item) {
    int tagId = item->data(0, Qt::UserRole).toInt();
    Tag tag = Tag::fetch(tagId);

    if (!tag.isFetched()) {
        return;
    }

    tag.setColor(QColor());
    tag.store();

    // set the color of the tag tree widget item
    handleTreeWidgetItemTagColor(item, tag);
}

/**
 * Populates a tag menu tree for moving tags
 */
void MainWindow::buildTagMoveMenuTree(QMenu *parentMenu,
                                      int parentTagId) {
    QList<Tag> tagList = Tag::fetchAllByParentId(parentTagId);
    QSignalMapper *tagMovingSignalMapper = new QSignalMapper(this);

    Q_FOREACH(Tag tag, tagList) {
            int tagId = tag.getId();
            QString name = tag.getName();

            int count = Tag::countAllParentId(tagId);
            if (count > 0) {
                // if there are sub-tag build a new menu level
                QMenu *tagMenu = parentMenu->addMenu(name);
                buildTagMoveMenuTree(tagMenu, tagId);
            } else {
                // if there are no sub-tags just create a named action
                QAction *action = parentMenu->addAction(name);

                QObject::connect(
                        action, SIGNAL(triggered()),
                        tagMovingSignalMapper, SLOT(map()));

                tagMovingSignalMapper->setMapping(
                        action, tagId);
            }
        }

    // add an action to move to this tag
    parentMenu->addSeparator();
    QAction *action = parentMenu->addAction(
            parentTagId == 0 ? tr("Move to the root") : tr("Move to this tag"));
    action->setData(parentTagId);

    QObject::connect(
            action, SIGNAL(triggered()),
            tagMovingSignalMapper, SLOT(map()));

    tagMovingSignalMapper->setMapping(
            action, parentTagId);

    // connect the signal mapper
    QObject::connect(tagMovingSignalMapper,
                     SIGNAL(mapped(int)),
                     this,
                     SLOT(moveSelectedTagsToTagId(int)));
}

/**
 * Populates a tag menu tree for bulk note tagging
 */
void MainWindow::buildBulkNoteTagMenuTree(QMenu *parentMenu,
                                          int parentTagId) {
    QList<Tag> tagList = Tag::fetchAllByParentId(parentTagId);
    QSignalMapper *signalMapper = new QSignalMapper(this);

    Q_FOREACH(Tag tag, tagList) {
            int tagId = tag.getId();
            QString name = tag.getName();

            int count = Tag::countAllParentId(tagId);
            if (count > 0) {
                // if there are sub-tag build a new menu level
                QMenu *tagMenu = parentMenu->addMenu(name);
                buildBulkNoteTagMenuTree(tagMenu, tagId);
            } else {
                // if there are no sub-tags just create a named action
                QAction *action = parentMenu->addAction(name);

                QObject::connect(
                        action, SIGNAL(triggered()),
                        signalMapper, SLOT(map()));

                signalMapper->setMapping(
                        action, tagId);
            }
        }

    if (parentTagId > 0) {
        // add an action to tag this
        parentMenu->addSeparator();
        QAction *action = parentMenu->addAction(tr("Tag this"));
        action->setData(parentTagId);

        QObject::connect(
                action, SIGNAL(triggered()),
                signalMapper, SLOT(map()));

        signalMapper->setMapping(
                action, parentTagId);
    }

    // connect the signal mapper
    QObject::connect(signalMapper,
                     SIGNAL(mapped(int)),
                     this,
                     SLOT(tagSelectedNotesToTagId(int)));
}

/**
 * Moves selected tags to tagId
 */
void MainWindow::moveSelectedTagsToTagId(int tagId) {
    qDebug() << __func__ << " - 'tagId': " << tagId;

    Q_FOREACH(QTreeWidgetItem *item, ui->tagTreeWidget->selectedItems()) {
            int id = item->data(0, Qt::UserRole).toInt();
            Tag tag = Tag::fetch(id);
            if (tag.isFetched()) {
                if (tag.hasChild(tagId) || (id == tagId)) {
                    showStatusBarMessage(
                            tr("Cannot move tag '%1' to this tag")
                                    .arg(tag.getName()),
                            3000);
                } else {
                    tag.setParentId(tagId);
                    tag.store();

                    reloadCurrentNoteTags();
                    reloadTagTree();

                    showStatusBarMessage(
                            tr("Moved tag '%1' to new tag").arg(tag.getName()),
                            3000);
                }
            }
        }
}

/**
 * Tag selected notes to tagId
 */
void MainWindow::tagSelectedNotesToTagId(int tagId) {
    qDebug() << __func__ << " - 'tagId': " << tagId;
    Tag tag = Tag::fetch(tagId);

    // tag notes
    if (tag.isFetched()) {
        tagSelectedNotes(tag);
    }
}

/**
 * Populates a subfolder menu tree for bulk note moving or copying
 */
void MainWindow::buildBulkNoteSubFolderMenuTree(QMenu *parentMenu, bool doCopy,
                                                int parentNoteSubFolderId) {
    QList<NoteSubFolder> noteSubFolderList = NoteSubFolder::fetchAllByParentId(
            parentNoteSubFolderId);
    QSignalMapper *signalMapper = new QSignalMapper(this);

    Q_FOREACH(NoteSubFolder noteSubFolder, noteSubFolderList) {
            int noteSubFolderId = noteSubFolder.getId();
            QString name = noteSubFolder.getName();

            int count = NoteSubFolder::countAllParentId(noteSubFolderId);
            if (count > 0) {
                // if there are sub-noteSubFolder build a new menu level
                QMenu *noteSubFolderMenu = parentMenu->addMenu(name);
                buildBulkNoteSubFolderMenuTree(noteSubFolderMenu, doCopy,
                                               noteSubFolderId);
            } else {
                // if there are no sub-noteSubFolders just create a named action
                QAction *action = parentMenu->addAction(name);

                QObject::connect(
                        action, SIGNAL(triggered()),
                        signalMapper, SLOT(map()));

                signalMapper->setMapping(
                        action, noteSubFolderId);
            }
        }

    // add an action to copy or move to this subfolder
    parentMenu->addSeparator();
    QString text = (parentNoteSubFolderId == 0) ?
                   (doCopy ? tr("Copy to note folder") :
                             tr("Move to note folder")) :
                   (doCopy ? tr("Copy to this subfolder") :
                             tr("Move to this subfolder"));
    QAction *action = parentMenu->addAction(text);
    action->setData(parentNoteSubFolderId);

    QObject::connect(
            action, SIGNAL(triggered()),
            signalMapper, SLOT(map()));

    signalMapper->setMapping(
            action, parentNoteSubFolderId);

    // connect the signal mapper
    QObject::connect(signalMapper,
                     SIGNAL(mapped(int)),
                     this,
                     doCopy ?
                     SLOT(copySelectedNotesToNoteSubFolderId(int)) :
                     SLOT(moveSelectedNotesToNoteSubFolderId(int)));
}

/**
 * Moves selected notes to a note subfolder id
 */
void MainWindow::moveSelectedNotesToNoteSubFolderId(int noteSubFolderId) {
    qDebug() << __func__ << " - 'noteSubFolderId': " << noteSubFolderId;
    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);

    // move selected notes to note subfolder
    if (noteSubFolder.isFetched() || (noteSubFolderId == 0)) {
        moveSelectedNotesToNoteSubFolder(noteSubFolder);
    }
}

/**
 * Copies selected notes to a note subfolder id
 */
void MainWindow::copySelectedNotesToNoteSubFolderId(int noteSubFolderId) {
    qDebug() << __func__ << " - 'noteSubFolderId': " << noteSubFolderId;
    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);

    // copy selected notes to note subfolder
    if (noteSubFolder.isFetched() || (noteSubFolderId == 0)) {
        copySelectedNotesToNoteSubFolder(noteSubFolder);
    }
}

/**
 * Moves selected notes to a note subfolder
 */
void MainWindow::moveSelectedNotesToNoteSubFolder(NoteSubFolder noteSubFolder) {
    int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();
    QString text = tr("Move %n selected note(s) to note subfolder "
                              "<strong>%2</strong>?", "",
                      selectedItemsCount).arg(noteSubFolder.getName());

    if (selectedNotesHaveTags()) {
        text += " " + tr("Tagging information of these notes will be lost at "
                                 "the destination.");
    }

    if (QMessageBox::information(
            this,
            tr("Move selected notes"),
            text,
            tr("Move"), tr("Cancel"), QString::null, 0, 1) == 0) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker);

        int noteSubFolderCount = 0;
        Q_FOREACH(QTreeWidgetItem *item, ui->noteTreeWidget->selectedItems()) {
                int noteId = item->data(0, Qt::UserRole).toInt();
                Note note = Note::fetch(noteId);

                if (!note.isFetched()) {
                    continue;
                }

                // move note
                bool result = note.move(noteSubFolder.fullPath());
                if (result) {
                    noteSubFolderCount++;
                    qDebug() << "Note was moved:" << note.getName();
                } else {
                    qWarning() << "Could not move note:" << note.getName();
                }
            }

        // rebuild the index after the move
        if (noteSubFolderCount > 0) {
            // for some reason this only works with a small delay, otherwise
            // not all changes will be recognized
            QTimer::singleShot(150, this,
                               SLOT(buildNotesIndexAndLoadNoteDirectoryList()));
        }

        showStatusBarMessage(
                tr("%n note(s) were moved to note subfolder \"%2\"", "",
                   noteSubFolderCount).arg(noteSubFolder.getName()), 5000);
    }
}

/**
 * Copies selected notes to a note subfolder
 */
void MainWindow::copySelectedNotesToNoteSubFolder(NoteSubFolder noteSubFolder) {
    int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();
    QString text = tr("Copy %n selected note(s) to note subfolder "
                       "<strong>%2</strong>?", "",
                      selectedItemsCount).arg(noteSubFolder.getName());

    if (selectedNotesHaveTags()) {
        text += " " + tr("Tagging information of these notes will be lost at "
                                 "the destination.");
    }

    if (QMessageBox::information(
            this,
            tr("Copy selected notes"),
            text,
            tr("Copy"), tr("Cancel"), QString::null, 0, 1) == 0) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker);

        int noteSubFolderCount = 0;
        Q_FOREACH(QTreeWidgetItem *item, ui->noteTreeWidget->selectedItems()) {
                int noteId = item->data(0, Qt::UserRole).toInt();
                Note note = Note::fetch(noteId);

                if (!note.isFetched()) {
                    continue;
                }

                // copy note
                bool result = note.copy(noteSubFolder.fullPath());
                if (result) {
                    noteSubFolderCount++;
                    qDebug() << "Note was copied:" << note.getName();
                } else {
                    qWarning() << "Could not copy note:" << note.getName();
                }
            }

        // rebuild the index after the copy
        if (noteSubFolderCount > 0) {
            // for some reason this only works with a small delay, otherwise
            // not all changes will be recognized
            QTimer::singleShot(150, this,
                               SLOT(buildNotesIndexAndLoadNoteDirectoryList()));
        }

        showStatusBarMessage(
                tr("%n note(s) were copied to note subfolder \"%2\"", "",
                   noteSubFolderCount).arg(noteSubFolder.getName()), 5000);
    }
}

/**
 * Retruns true if one of the selected notes has a linked tag
 *
 * @return
 */
bool MainWindow::selectedNotesHaveTags() {
    Q_FOREACH(QTreeWidgetItem *item, ui->noteTreeWidget->selectedItems()) {
            int noteId = item->data(0, Qt::UserRole).toInt();
            Note note = Note::fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            if (Tag::countAllOfNote(note) > 0) {
                return true;
            }
        }

    return false;
}

/**
 * Opens the widget to replace text in the current note
 */
void MainWindow::on_actionReplace_in_current_note_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    textEdit->searchWidget()->activateReplace();
}

/**
 * Jumps to the position that was clicked in the navigation widget
 */
void MainWindow::onNavigationWidgetPositionClicked(int position) {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();

    // set the focus first so the preview also scrolls to the headline
    textEdit->setFocus();

    QTextCursor c = textEdit->textCursor();

    // if the current position of the cursor is smaller than the position
    // where we want to jump to set the cursor to the end of the note to make
    // sure it scrolls up, not down
    // everything is visible that way
    if (c.position() < position) {
        c.movePosition(QTextCursor::End);
        textEdit->setTextCursor(c);
    }

    c.setPosition(position);

    // select the text of the headline
    c.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

    textEdit->setTextCursor(c);
}

/**
 * Start a note preview regeneration to resize too large images
 */
void MainWindow::onNoteTextViewResize(QSize size, QSize oldSize) {
    Q_UNUSED(size);
    Q_UNUSED(oldSize);

    // just regenerate the note once a second for performance reasons
    if (!_noteViewIsRegenerated) {
        _noteViewIsRegenerated = true;
        QTimer::singleShot(1000, this, SLOT(regenerateNotePreview()));
    }
}

/**
 * Regenerate the note preview by converting the markdown to html again
 */
void MainWindow::regenerateNotePreview() {
    setNoteTextFromNote(&currentNote, true);
    _noteViewIsRegenerated = false;
}

/**
 * Tries to open a link at the current cursor position or solve an equation
 */
void MainWindow::on_actionAutocomplete_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();

    // try to open a link at the cursor position
    if (textEdit->openLinkAtCursorPosition()) {
        showStatusBarMessage(
                tr("An url was opened at the current cursor position"), 5000);
        return;
    }

    QMenu menu;

    double resultValue;
    if (solveEquationInNoteTextEdit(resultValue)) {
        QString text = QString::number(resultValue);
        QAction *action = menu.addAction("= " + text);
        action->setData(text);
        action->setWhatsThis("equation");
    }

    QStringList resultList;
    if (noteTextEditAutoComplete(resultList)) {
        Q_FOREACH(QString text, resultList) {
                QAction *action = menu.addAction(text);
                action->setData(text);
                action->setWhatsThis("autocomplete");
            }
    }

    QPoint globalPos = textEdit->mapToGlobal(
            textEdit->cursorRect().bottomRight());

    if (menu.actions().count() > 0) {
        QAction *selectedItem = menu.exec(globalPos);
        if (selectedItem) {
            QString text = selectedItem->data().toString();
            QString type = selectedItem->whatsThis();

            if (text.isEmpty()) {
                return;
            }

            if (type == "autocomplete") {
                // overwrite the currently written word
                QTextCursor c = textEdit->textCursor();
                c.movePosition(QTextCursor::StartOfWord,
                               QTextCursor::KeepAnchor);
                c.insertText(text + " ");
            } else {
                textEdit->insertPlainText(text);
            }
        }
    }
}

/**
 * Tries to find an equation in the current line and solves it
 *
 * @param returnValue
 * @return
 */
bool MainWindow::solveEquationInNoteTextEdit(double &returnValue) {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    // get the text from the current cursor to the start of the line
    c.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    QString text = c.selectedText();
    qDebug() << __func__ << " - 'text': " << text;

    QString equation = text;

    // replace "," with "." to allow "," as coma
    equation.replace(",", ".");

    // match all characters and basic operations like +, -, * and /
    QRegularExpressionMatch match =
            QRegularExpression("([\\d\\.,+\\-*\\/\\(\\)\\s]+)\\s*=")
                    .match(equation);

    if (!match.hasMatch()) {
        return false;
    }

    showStatusBarMessage(tr("No equation was found in front of the cursor"),
                         5000);

    equation = match.captured(1);
    qDebug() << __func__ << " - 'equation': " << equation;

    QJSEngine engine;
    // evaluate our equation
    QJSValue result = engine.evaluate(equation);
    double resultValue = result.toNumber();
    qDebug() << __func__ << " - 'resultValue': " << resultValue;

    // compensate for subtraction errors with 0
    if ((resultValue < 0.0001) && (resultValue > 0)) {
        resultValue = 0;
    }

    showStatusBarMessage(tr("Result for equation: %1 = %2")
                                 .arg(equation,
                                      QString::number(resultValue)),
                         10000);

    // check if cursor is after the "="
    match = QRegularExpression("=\\s*$").match(text);
    if (!match.hasMatch()) {
        return false;
    }

    returnValue = resultValue;
    return true;
}

/**
 * Tries to find words that start with the current word in the note text edit
 *
 * @param resultList
 * @return
 */
bool MainWindow::noteTextEditAutoComplete(QStringList &resultList) {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    // get the text from the current cursor to the start of the line
    c.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);
    QString text = c.selectedText();
    qDebug() << __func__ << " - 'text': " << text;

    if (text.isEmpty()) {
        return false;
    }

    QString noteText = textEdit->toPlainText();

    // find all items that match our current word
    resultList = noteText.split(
            QRegularExpression("[^\\w\\d]",
                               QRegularExpression::UseUnicodePropertiesOption),
            QString::SkipEmptyParts)
            .filter(QRegularExpression(
                    "^" + QRegularExpression::escape(text),
                    QRegularExpression::CaseInsensitiveOption));

    // we only want each word once
    resultList.removeDuplicates();

    // remove the text we already entered
    resultList.removeOne(text);

    if (resultList.count() == 0) {
        return false;
    }

    qDebug() << __func__ << " - 'resultList': " << resultList;

    return true;
}

/**
 * Shows the note folder selection popup
 */
void MainWindow::on_actionSelect_note_folder_triggered() {
    _noteFolderDockWidgetWasVisible = _noteFolderDockWidget->isVisible();
    _noteFolderDockWidget->show();
    ui->noteFolderComboBox->showPopup();
}

/**
 * Reloads the scripting engine
 */
void MainWindow::on_actionReload_scripting_engine_triggered() {
    ScriptingService::instance()->reloadEngine();
    showStatusBarMessage(tr("The scripting engine was reloaded"), 3000);
}

/**
 * Things to do before the scripting engine will be reloaded
 * Will be invoked by the ScriptingService
 */
void MainWindow::preReloadScriptingEngine() {
    // clear and hide the custom actions
    ui->menuCustom_actions->clear();
    ui->menuCustom_actions->hide();
    _customActionToolbar->clear();
    _customActionToolbar->hide();
}

void MainWindow::on_actionShow_log_triggered() {
    _logDockWidget->show();
}

/**
 * Exports the note preview as HTML
 */
void MainWindow::on_actionExport_preview_HTML_triggered() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("HTML files (*.html)"));
    dialog.setWindowTitle(tr("Export current note as HTML file"));
    dialog.selectFile(currentNote.getName() + ".html");
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.count() > 0) {
            QString fileName = fileNames.at(0);

            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(".html");
            }

            QFile file(fileName);

            qDebug() << "exporting html file: " << fileName;

            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qCritical() << file.errorString();
                return;
            }
            QTextStream out(&file);
            out.setCodec("UTF-8");
            out << currentNote.toMarkdownHtml(
                    NoteFolder::currentLocalPath(), getMaxImageWidth(), true);
            file.flush();
            file.close();
        }
    }
}

/**
 * Opens the IRC web chat page
 */
void MainWindow::on_actionOpen_IRC_Channel_triggered() {
    QDesktopServices::openUrl(
            QUrl("https://kiwiirc.com/client/irc.freenode.net/#qownnotes"));
}

/**
 * Opens the Gitter room in a webpage
 */
void MainWindow::on_actionGitter_triggered() {
    QDesktopServices::openUrl(QUrl("https://gitter.im/qownnotes/qownnotes"));
}

/**
 * Adds the current search text to the saved searches
 */
void MainWindow::storeSavedSearch() {
    QString text = ui->searchLineEdit->text();
    if (!text.isEmpty()) {
        int noteFolderId = NoteFolder::currentNoteFolderId();
        QSettings settings;
        QString settingsKey = "savedSearches/noteFolder-"
                              + QString::number(noteFolderId);
        QStringList savedSearches = settings.value(settingsKey)
                .toStringList();

        // add the text to the saved searches
        savedSearches.prepend(text);

        // remove duplicate entries, `text` will remain at the top
        savedSearches.removeDuplicates();

        // only keep 100 searches
        while (savedSearches.count() > 100) {
            savedSearches.removeLast();
        }

        settings.setValue(settingsKey, savedSearches);

        // init the saved searches completer
        initSavedSearchesCompleter();
    }
}

/**
 * Initializes the saved searches completer
 */
void MainWindow::initSavedSearchesCompleter() {
    int noteFolderId = NoteFolder::currentNoteFolderId();
    QSettings settings;
    QString settingsKey = "savedSearches/noteFolder-"
                          + QString::number(noteFolderId);
    QStringList savedSearches = settings.value(settingsKey).toStringList();

    // add the completer
    QCompleter *completer = new QCompleter(savedSearches, ui->searchLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->searchLineEdit->setCompleter(completer);

    // install event filter for the popup
    completer->popup()->installEventFilter(this);
}

/**
 * Inserts the note file name as headline
 */
void MainWindow::on_actionInsert_headline_from_note_filename_triggered()
{
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    c.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);

    QString fileName = currentNote.fileBaseName(true);
    QString text = Note::createNoteHeader(fileName);
    c.insertText(text);
}

/**
 * Toggles the editor soft wrapping
 */
void MainWindow::on_actionUse_softwrap_in_note_editor_toggled(bool arg1)
{
    QSettings settings;
    settings.setValue("useSoftWrapInNoteEditor", arg1);

    // initialize the editor soft wrapping
    initEditorSoftWrap();
}

void MainWindow::on_actionShow_status_bar_triggered(bool checked) {
    ui->statusBar->setVisible(checked);

    const QSignalBlocker blocker(ui->actionShow_status_bar);
    {
        Q_UNUSED(blocker);
        ui->actionShow_status_bar->setChecked(checked);
    }

    QSettings settings;
    settings.setValue("showStatusBar", checked);
}

void MainWindow::on_noteTreeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);

    // in case all notes were removed
    if (current == NULL) {
        return;
    }

    int noteId = current->data(0, Qt::UserRole).toInt();
    Note note = Note::fetch(noteId);
    qDebug() << "currentItemChanged " << note;

    setCurrentNote(note, true, false);

    // parse the current note for markdown highlighting
    ui->noteTextEdit->highlighter()->parse();

    // let's highlight the text from the search line edit
    searchForSearchLineTextInNoteTextEdit();

    // also do a "in note search" if the widget is visible
    if (ui->noteTextEdit->searchWidget()->isVisible()) {
        ui->noteTextEdit->searchWidget()->doSearchDown();
    }
}

void MainWindow::on_noteTreeWidget_customContextMenuRequested(
        const QPoint &pos) {
    QPoint globalPos = ui->noteTreeWidget->mapToGlobal(pos);
    QMenu noteMenu;
    QMenu *moveDestinationMenu = new QMenu();
    QMenu *copyDestinationMenu = new QMenu();
    QMenu *tagRemoveMenu = new QMenu();

    QList<NoteFolder> noteFolders = NoteFolder::fetchAll();

    // show copy and move menu entries only if there
    // is at least one other note folder
    if (noteFolders.count() > 1) {
        moveDestinationMenu = noteMenu.addMenu(tr("&Move notes to..."));
        copyDestinationMenu = noteMenu.addMenu(tr("&Copy notes to..."));

        Q_FOREACH(NoteFolder noteFolder, noteFolders) {
                // don't show not existing folders or if path is empty
                if (!noteFolder.localPathExists()) {
                    continue;
                }

                if (noteFolder.isCurrent()) {
                    continue;
                }

                QAction *moveAction = moveDestinationMenu->addAction(
                        noteFolder.getName());
                moveAction->setData(noteFolder.getLocalPath());
                moveAction->setToolTip(noteFolder.getLocalPath());
                moveAction->setStatusTip(noteFolder.getLocalPath());

                QAction *copyAction = copyDestinationMenu->addAction(
                        noteFolder.getName());
                copyAction->setData(noteFolder.getLocalPath());
                copyAction->setToolTip(noteFolder.getLocalPath());
                copyAction->setStatusTip(noteFolder.getLocalPath());
            }
    }

    bool showSubFolders = NoteFolder::isCurrentShowSubfolders();
    if (showSubFolders) {
        QMenu *subFolderMoveMenu = noteMenu.addMenu(
                tr("Move notes to subfolder..."));
        buildBulkNoteSubFolderMenuTree(subFolderMoveMenu, false);

        QMenu *subFolderCopyMenu = noteMenu.addMenu(
                tr("Copy notes to subfolder..."));
        buildBulkNoteSubFolderMenuTree(subFolderCopyMenu, true);
    }

    QList<Tag> tagList = Tag::fetchAll();

    // show the tagging menu if at least one tag is present
    if (tagList.count() > 0) {
        QMenu *tagMenu = noteMenu.addMenu(tr("&Tag selected notes with..."));
        buildBulkNoteTagMenuTree(tagMenu);
    }

    QStringList noteNameList;
    Q_FOREACH(QTreeWidgetItem *item, ui->noteTreeWidget->selectedItems()) {
            // the note names are not unique any more but the note subfolder
            // path will be taken into account in
            // Tag::fetchAllWithLinkToNoteNames
            QString name = item->text(0);
            Note note = Note::fetchByName(name);
            if (note.isFetched()) {
                noteNameList << note.getName();
            }
        }

    QList<Tag> tagRemoveList = Tag::fetchAllWithLinkToNoteNames(
            noteNameList);

    // show the remove tags menu if at least one tag is present
    if (tagRemoveList.count() > 0) {
        tagRemoveMenu = noteMenu.addMenu(
                tr("&Remove tag from selected notes..."));

        Q_FOREACH(Tag tag, tagRemoveList) {
                QAction *action = tagRemoveMenu->addAction(
                        tag.getName());
                action->setData(tag.getId());
                action->setToolTip(tag.getName());
                action->setStatusTip(tag.getName());
            }
    }

    QAction *removeAction = noteMenu.addAction(tr("&Remove notes"));
    noteMenu.addSeparator();
    QAction *openInExternalEditorAction = noteMenu.addAction(
            tr("Open note in external editor"));
    QAction *showInFileManagerAction = noteMenu.addAction(
            tr("Show note in file manager"));
    noteMenu.addSeparator();
    QAction *selectAllAction = noteMenu.addAction(tr("Select &all notes"));

    QAction *selectedItem = noteMenu.exec(globalPos);
    if (selectedItem) {
        if (selectedItem->parent() == moveDestinationMenu) {
            // move notes
            QString destinationFolder = selectedItem->data().toString();
            moveSelectedNotesToFolder(destinationFolder);
        } else if (selectedItem->parent() == copyDestinationMenu) {
            // copy notes
            QString destinationFolder = selectedItem->data().toString();
            copySelectedNotesToFolder(destinationFolder);
        } else if (selectedItem->parent() == tagRemoveMenu) {
            // remove tag from notes
            Tag tag = Tag::fetch(selectedItem->data().toInt());

            if (tag.isFetched()) {
                removeTagFromSelectedNotes(tag);
            }
        } else if (selectedItem == removeAction) {
            // remove notes
            removeSelectedNotes();
        } else if (selectedItem == selectAllAction) {
            // select all notes
            selectAllNotes();
        } else if (selectedItem == openInExternalEditorAction) {
            // opens the current note in an external editor
            on_action_Open_note_in_external_editor_triggered();
        } else if (selectedItem == showInFileManagerAction) {
            // show the current note in the file manager
            on_actionShow_note_in_file_manager_triggered();
        }
    }
}

/**
 * Renames a note file if the note was renamed in the note tree widget
 */
void MainWindow::on_noteTreeWidget_itemChanged(QTreeWidgetItem *item,
                                               int column) {
    Q_UNUSED(column);
    if (item == NULL || !Note::allowDifferentFileName()) {
        return;
    }

    int noteId = item->data(0, Qt::UserRole).toInt();
    Note note = Note::fetch(noteId);
    if (note.isFetched()) {
        qDebug() << __func__ << " - 'note': " << note;

        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker);

        QString oldNoteName = note.getName();

        if (note.renameNoteFile(item->text(0))) {
            QString newNoteName = note.getName();

            if (oldNoteName != newNoteName) {
                note.refetch();
                setCurrentNote(note);

                // rename the note file names of note tag links
                Tag::renameNoteFileNamesOfLinks(oldNoteName, newNoteName);

                // handle the replacing of all note urls if a note was renamed
                Note::handleNoteRenaming(oldNoteName, newNoteName);

                // reload the directory list if note name has changed
//                loadNoteDirectoryList();
            }
        }

        const QSignalBlocker blocker2(ui->noteTreeWidget);
        Q_UNUSED(blocker2);

        // set old name back in case the renaming failed or the file name got
        // altered in the renaming process
        item->setText(0, note.getName());
    }
}

void MainWindow::on_noteSubFolderTreeWidget_currentItemChanged(
        QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous);

    if (current == NULL) {
        return;
    }

    // set the note sub folder id as active
    int noteSubFolderId = current->data(0, Qt::UserRole).toInt();
    _showNotesFromAllNoteSubFolders = noteSubFolderId == -1;

    NoteSubFolder::setAsActive(_showNotesFromAllNoteSubFolders ?
                               0 : noteSubFolderId);

    const QSignalBlocker blocker(ui->searchLineEdit);
    Q_UNUSED(blocker);

    ui->searchLineEdit->clear();

    filterNotes();
}

/**
 * Searches for note sub folders in the note sub folder tree widget
 */
void MainWindow::on_noteSubFolderLineEdit_textChanged(const QString &arg1) {
    if (arg1.isEmpty()) {
        // reload the note subfolder tree, so that the expand/collapse state
        // of the items gets restored
        reloadNoteSubFolderTree();
    } else {
        // block the events because we don't want the expand/collapse state
        // of the items get stored while searching
        const QSignalBlocker blocker(ui->noteSubFolderTreeWidget);
        Q_UNUSED(blocker);

        // search for the text
        searchForTextInTreeWidget(ui->noteSubFolderTreeWidget, arg1);
    }
}

/**
 * Searches for text in items of a tree widget
 */
void MainWindow::searchForTextInTreeWidget(QTreeWidget *treeWidget,
                                           QString text) {
    // get all items
    QList<QTreeWidgetItem*> allItems = treeWidget->
            findItems("", Qt::MatchContains | Qt::MatchRecursive);

    // search text if at least one character was entered
    if (text.count() >= 1) {
        // search for items
        QList<QTreeWidgetItem*> foundItems = treeWidget->
                findItems(text, Qt::MatchContains | Qt::MatchRecursive);

        // hide all not found items
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                int id = item->data(0, Qt::UserRole).toInt();
                item->setHidden(!foundItems.contains(item) && (id > 0));
            }

        // show items again that have visible children so that they are
        // really shown
        Q_FOREACH(QTreeWidgetItem *item, allItems) {
                if (isOneTreeWidgetItemChildVisible(item)) {
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
 * Saves the expand status of the an item
 */
void MainWindow::on_noteSubFolderTreeWidget_itemExpanded(
        QTreeWidgetItem *item) {
    int noteSubFolderId = item->data(0, Qt::UserRole).toInt();
    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);
    if (noteSubFolder.isFetched()) {
        noteSubFolder.saveTreeWidgetExpandState(
                item->isExpanded());
    }

    // resize columns so long folder names get displayed
    ui->noteSubFolderTreeWidget->resizeColumnToContents(0);
    ui->noteSubFolderTreeWidget->resizeColumnToContents(1);
}

void MainWindow::on_noteSubFolderTreeWidget_itemCollapsed(
        QTreeWidgetItem *item) {
    on_noteSubFolderTreeWidget_itemExpanded(item);
}

/**
 * Shows the context menu for the note subfolder tree
 */
void MainWindow::on_noteSubFolderTreeWidget_customContextMenuRequested(
        const QPoint &pos) {
    // don't open the context menu if no subfolders are selected
    if (ui->noteSubFolderTreeWidget->selectedItems().count() == 0) {
        return;
    }

    QPoint globalPos = ui->noteSubFolderTreeWidget->mapToGlobal(pos);
    QMenu menu;

    QAction *newNoteAction = menu.addAction(tr("New note"));
    QAction *newAction = menu.addAction(tr("New subfolder"));
    QAction *removeAction = menu.addAction(tr("Remove selected folders"));
    QAction *showInFileManagerAction = menu.addAction(
            tr("Show folder in file manager"));

    QAction *selectedItem = menu.exec(globalPos);
    if (selectedItem) {
        if (selectedItem == newNoteAction) {
            // create a new note
            on_action_New_note_triggered();
        } else if (selectedItem == newAction) {
            // create a new folder
            createNewNoteSubFolder();
        } else if (selectedItem == removeAction) {
            // remove folders
            removeSelectedNoteSubFolders();
        } else if (selectedItem == showInFileManagerAction) {
            NoteSubFolder noteSubFolder =
                    NoteFolder::currentNoteFolder().getActiveNoteSubFolder();

            // show the current folder in the file manager
            Utils::Misc::openPath(noteSubFolder.fullPath());
        }
    }
}

/**
 * Creates a new note subfolder in the current subfolder
 */
bool MainWindow::createNewNoteSubFolder(QString folderName) {
    bool ok = true;

    if (folderName.isEmpty()) {
        folderName = QInputDialog::getText(
                this, tr("Create a new folder"), tr("Folder name:"),
                QLineEdit::Normal, "", &ok);
    }

    if (!ok || folderName.isEmpty()) {
        return false;
    }

    NoteSubFolder noteSubFolder =
            NoteFolder::currentNoteFolder().getActiveNoteSubFolder();
    QString path = noteSubFolder.fullPath() + QDir::separator() + folderName;
    QDir directory;
    bool success = directory.mkpath(path);

    // rebuild the index of the note subfolders
    buildNotesIndex();

    // reload note subfolders
    setupNoteSubFolders();

    return success;
}

/**
 * Creates a new subfolder with a name already entered
 */
void MainWindow::on_noteSubFolderLineEdit_returnPressed() {
    if (createNewNoteSubFolder(ui->noteSubFolderLineEdit->text())) {
        // clear the line edit on success
        ui->noteSubFolderLineEdit->clear();
    }
}

/**
 * Renames a note subfolder
 */
void MainWindow::on_noteSubFolderTreeWidget_itemChanged(
        QTreeWidgetItem *item, int column) {
    Q_UNUSED(column);

    NoteSubFolder noteSubFolder = NoteSubFolder::fetch(
            item->data(0, Qt::UserRole).toInt());
    if (noteSubFolder.isFetched()) {
        QString name = item->text(0);

        // rename the note subfolder in the file system
        noteSubFolder.rename(name);

        // reload tags, note subfolder and notes
        on_action_Reload_note_folder_triggered();
    }
}

void MainWindow::on_actionShare_note_triggered() {
    const QSignalBlocker blocker(ui->actionShare_note);
    Q_UNUSED(blocker);
    ui->actionShare_note->setChecked(currentNote.isShared());

    ShareDialog *dialog = new ShareDialog(currentNote, this);
    dialog->exec();

    currentNote.refetch();

    // update the share button
    updateShareButton();
}

/**
 * Toggles the case of the selected text
 */
void MainWindow::on_actionToggle_text_case_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    QString selectedText = textEdit->textCursor().selectedText();

    if (selectedText.isEmpty()) {
        return;
    }

    // insert the text in uppercase or lowercase
    c.insertText(selectedText == selectedText.toLower() ?
                 selectedText.toUpper() :
                 selectedText.toLower());

    // select the text again to maybe do an other operation on it
    c.movePosition(
            QTextCursor::Left, QTextCursor::KeepAnchor, selectedText.count());
    textEdit->setTextCursor(c);
}

/**
 * Opens the Markdown Cheatsheet webpage
 */
void MainWindow::on_actionMarkdown_cheatsheet_triggered() {
    QDesktopServices::openUrl(
            QUrl("https://github.com/pbek/QOwnNotes/blob/develop/src/demonotes"
                         "/Markdown%20Cheatsheet.md"));
}

/**
 * Strikes out the selected text
 */
void MainWindow::on_actionStrike_out_text_triggered() {
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    QString selectedText = textEdit->textCursor().selectedText();

    if (selectedText.isEmpty()) {
        c.insertText("~~~~");
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 2);
        textEdit->setTextCursor(c);
    } else {
        c.insertText("~~" + selectedText + "~~");
    }
}

/**
 * Initializes the shortcuts for the actions
 *
 * @param setDefaultShortcut
 */
void MainWindow::initShortcuts() {
    QList<QMenu*> menus = menuList();
    QSettings settings;

    // we also have to clear the shortcuts directly, just removing the
    // objects didn't remove the shortcut
    foreach(QShortcut* menuShortcut, _menuShortcuts) {
            menuShortcut->setKey(QKeySequence());
        }

    // remove all menu shortcuts to create new ones
    _menuShortcuts.clear();

#ifndef Q_OS_MAC
    bool menuBarIsVisible = !ui->menuBar->isHidden();
    qDebug() << __func__ << " - 'menuBarIsVisible': " << menuBarIsVisible;
#endif

    // loop through all menus
    foreach(QMenu* menu, menus) {
            // loop through all actions of the menu
            foreach(QAction* action, menu->actions()) {
                    // we don't need empty objects
                    if (action->objectName().isEmpty()) {
                        continue;
                    }

                    QString oldShortcut = action->shortcut().toString();

                    // try to load a key sequence from the settings
                    QKeySequence shortcut = QKeySequence(settings.value(
                                    "Shortcuts/MainWindow-" +
                                            action->objectName()).toString());

                    // do we can this method the first time?
                    if (!_isDefaultShortcutInitialized) {
                        // set the default shortcut
                        action->setData(oldShortcut);

                        // if there was a shortcut set use the new shortcut
                        if (!shortcut.isEmpty()) {
                            action->setShortcut(shortcut);
                        }
                    } else {
                        // set to the default shortcut if no shortcut was found,
                        // otherwise store the new shortcut
                        action->setShortcut(shortcut.isEmpty()
                                            ? action->data().toString()
                                            : shortcut);
                    }

#ifndef Q_OS_MAC
                    // if the menu bar is not visible (like for the Unity
                    // desktop) create a workaround with a QShortcut so the
                    // shortcuts are still working
                    // we don't do that under OS X, it causes all shortcuts
                    // to not be viewed
                    if (!menuBarIsVisible) {
                        shortcut = action->shortcut();
                        action->setShortcut(QKeySequence());

                        QShortcut *shortcutItem = new QShortcut(shortcut, this);
                        connect(shortcutItem, SIGNAL(activated()),
                                action, SLOT(trigger()));
                        _menuShortcuts.append(shortcutItem);
                    }
#endif
            }
    }

    if (!_isDefaultShortcutInitialized) {
        _isDefaultShortcutInitialized = true;
    }
}

/**
 * Shows or hides the main menu bar
 *
 * @param checked
 */
void MainWindow::on_actionShow_menu_bar_triggered(bool checked) {
    ui->menuBar->setVisible(checked);

    const QSignalBlocker blocker(ui->actionShow_menu_bar);
    {
        Q_UNUSED(blocker);
        ui->actionShow_menu_bar->setChecked(checked);
    }

    QSettings settings;
    settings.setValue("showMenuBar", checked);

    // show the action in the toolbar if the main menu isn't shown
    if (checked) {
        _windowToolbar->removeAction(ui->actionShow_menu_bar);
    } else {
        _windowToolbar->addAction(ui->actionShow_menu_bar);
    }

    // init the shortcuts again to create or remove the menu bar shortcut
    // workaround
    initShortcuts();
}

/**
 * Splits the current note into two notes at the current cursor position
 */
void MainWindow::on_actionSplit_note_at_cursor_position_triggered() {
    QString name = currentNote.getName();
    QList<Tag> tags = Tag::fetchAllOfNote(currentNote);

    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    // select the text to get into a new note
    c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    QString selectedText = c.selectedText();

    // remove the selected text
    c.removeSelectedText();
    textEdit->setTextCursor(c);

    Note previousNote = currentNote;

    // create a new note
    createNewNote(name);

    // adding a link to new note into the old note
    previousNote.refetch();
    QString noteNameForLink = Note::generateTextForLink(currentNote.getName());
    QString previousNoteText = previousNote.getNoteText();
    previousNoteText += "\n\n<note://" + noteNameForLink + ">";
    previousNote.storeNewText(previousNoteText);

    // add the previously removed text
    textEdit = activeNoteTextEdit();
    textEdit->insertPlainText(selectedText);

    // link the tags of the old note to the new note
    Q_FOREACH(Tag tag, tags) {
            tag.linkToNote(currentNote);
        }
}

/**
 * Adds a custom action as menu item and button
 */
void MainWindow::addCustomAction(QString identifier, QString menuText,
                                 QString buttonText, QString icon) {
//    ui->menuCustom_actions->show();
    QAction *action = ui->menuCustom_actions->addAction(menuText);
    action->setObjectName("customAction_" + identifier);
    action->setData(identifier);

    // try to add an icon
    if (!icon.isEmpty()) {
        QFile file(icon);
        // if no icon file was found set it as freedesktop theme icon
        QIcon i = file.exists() ? QIcon(icon) : QIcon::fromTheme(icon);
        action->setIcon(i);
    }

    // set a button text if not empty
    if (!buttonText.isEmpty()) {
        action->setIconText(buttonText);
    }

    // add a button to the custom action toolbar
    if (!buttonText.isEmpty() || !icon.isEmpty()) {
        _customActionToolbar->show();
        _customActionToolbar->addAction(action);
    }

    // connect to the custom action signal mapper
    QObject::connect(action, SIGNAL(triggered()),
                     _customActionSignalMapper, SLOT(map()));
    _customActionSignalMapper->setMapping(action, identifier);
}

/**
 * Opens the donation page
 */
void MainWindow::on_actionDonate_triggered() {
    QDesktopServices::openUrl(
            QUrl("http://www.qownnotes.org/donate"));
}

/**
 * Jumps to "All notes" in the note subfolder tree widget and triggers
 * a "Find note"
 */
void MainWindow::on_actionFind_notes_in_all_subfolders_triggered() {
    // send an event to jump to "All notes" in the note subfolder tree widget
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Home,
                                     Qt::NoModifier);
    QApplication::postEvent(ui->noteSubFolderTreeWidget, event);

    // trigger a "Find note"
    on_action_Find_note_triggered();
}

/**
 * Toggle if we want to start the app hidden
 */
void MainWindow::on_actionStart_hidden_triggered(bool checked) {
    QSettings settings;
    settings.setValue("StartHidden", checked);

    const QSignalBlocker blocker(ui->actionStart_hidden);
    {
        Q_UNUSED(blocker);
        ui->actionStart_hidden->setChecked(checked);
    }
}

void MainWindow::on_actionImport_notes_from_Evernote_triggered() {
    EvernoteImportDialog* dialog = new EvernoteImportDialog(this);
    dialog->exec();

    if (dialog->getImportCount() > 0) {
        // reload the note folder after importing new notes
        buildNotesIndexAndLoadNoteDirectoryList(true);
    }
}

/**
 * Shows a dialog to delete orphaned images
 */
void MainWindow::on_actionDelete_orphaned_images_triggered() {
    OrphanedImagesDialog* dialog = new OrphanedImagesDialog(this);
    dialog->exec();
}

/**
 * Writes text to the note text edit (for ScriptingService)
 *
 * @param text
 */
void MainWindow::writeToNoteTextEdit(QString text) {
    QTextEdit *textEdit = activeNoteTextEdit();
    textEdit->insertPlainText(text);
}

QString MainWindow::selectedNoteTextEditText() {
    QTextEdit *textEdit = activeNoteTextEdit();
    return textEdit->textCursor().selectedText();
}

/**
 * Locks and unlocks the dock widgets
 *
 * @param arg1
 */
void MainWindow::on_actionUnlock_panels_toggled(bool arg1) {
    const QSignalBlocker blocker(ui->actionUnlock_panels);
    {
        Q_UNUSED(blocker);
        ui->actionUnlock_panels->setChecked(arg1);
    }

    QList<QDockWidget*> dockWidgets = findChildren<QDockWidget*>();

    if (!arg1) {
        // remove the title bar widgets of all dock widgets
        Q_FOREACH(QDockWidget *dockWidget, dockWidgets) {
                // we don't want to lock floating dock widgets
                if (dockWidget->isFloating()) {
                    continue;
                }

                // remove the title bar widget
                dockWidget->setTitleBarWidget(new QWidget());

#ifndef Q_OS_MAC
                // set 3px top margin for the enclosed widget
                dockWidget->widget()->setContentsMargins(0, 3, 0, 0);
#endif
            }
    } else {
        // add the old title bar widgets to all dock widgets
        _noteSubFolderDockWidget->setTitleBarWidget(
                _noteSubFolderDockTitleBarWidget);
        _taggingDockWidget->setTitleBarWidget(_taggingDockTitleBarWidget);
        _noteSearchDockWidget->setTitleBarWidget(_noteSearchDockTitleBarWidget);
        _noteFolderDockWidget->setTitleBarWidget(_noteFolderDockTitleBarWidget);
        _noteListDockWidget->setTitleBarWidget(_noteListDockTitleBarWidget);
        _noteNavigationDockWidget->setTitleBarWidget(
                _noteNavigationDockTitleBarWidget);
        _noteEditDockWidget->setTitleBarWidget(_noteEditDockTitleBarWidget);
        _noteTagDockWidget->setTitleBarWidget(_noteTagDockTitleBarWidget);
        _notePreviewDockWidget->setTitleBarWidget(
                _notePreviewDockTitleBarWidget);
        _logDockWidget->setTitleBarWidget(_logDockTitleBarWidget);

        Q_FOREACH(QDockWidget *dockWidget, dockWidgets) {
                // reset the top margin of the enclosed widget
                dockWidget->widget()->setContentsMargins(0, 0, 0, 0);
            }
    }
}

/**
 * Creates a new workspace with asking for its name
 */
void MainWindow::on_actionStore_as_new_workspace_triggered() {
    QString name = QInputDialog::getText(
            this, tr("Create new workspace"), tr("Workspace name:")).trimmed();

    if (name.isEmpty()) {
        return;
    }

    // store the current workspace
    storeCurrentWorkspace();

    // create the new workspace
    createNewWorkspace(name);
}

/**
 * Creates a new workspace with name
 *
 * @param name
 * @return
 */
bool MainWindow::createNewWorkspace(QString name) {
    name = name.trimmed();

    if (name.isEmpty()) {
        return false;
    }

    QSettings settings;
    QString currentUuid = currentWorkspaceUuid();
    settings.setValue("previousWorkspace", currentUuid);

    QString uuid = QUuid::createUuid().toString();
    uuid.replace("{", "").replace("}", "");

    QStringList workspaces = getWorkspaceUuidList();
    workspaces.append(uuid);

    settings.setValue("workspaces", workspaces);
    settings.setValue("currentWorkspace", uuid);
    settings.setValue("workspace-" + uuid + "/name", name);

    // store the new current workspace
    storeCurrentWorkspace();

    // update the menu and combo box
    updateWorkspaceLists();

    return true;
}

/**
 * Returns the uuid of the current workspace
 *
 * @return
 */
QString MainWindow::currentWorkspaceUuid() {
    QSettings settings;
    return settings.value("currentWorkspace").toString();
}

/**
 * Sets the new current workspace when the workspace combo box index has changed
 */
void MainWindow::onWorkspaceComboBoxCurrentIndexChanged(int index) {
    Q_UNUSED(index);

    QString uuid = _workspaceComboBox->currentData().toString();

    // set the new workspace
    setCurrentWorkspace(uuid);
}

/**
 * Sets a new current workspace
 */
void MainWindow::setCurrentWorkspace(QString uuid) {
    QWidget *focusWidget = qApp->focusWidget();

    // store the current workspace
    storeCurrentWorkspace();

    QSettings settings;
    QString currentUuid = currentWorkspaceUuid();
    settings.setValue("previousWorkspace", currentUuid);
    settings.setValue("currentWorkspace", uuid);

    // restore the new workspace
    restoreCurrentWorkspace();

    // update the menu and combo box (but don't rebuild it)
    updateWorkspaceLists(false);

    if (focusWidget != Q_NULLPTR) {
        // set the focus to the widget that had the focus before
        // the workspace was restored
        focusWidget->setFocus();
    }
}

/**
 * Stores the current workspace
 */
void MainWindow::storeCurrentWorkspace() {
    QSettings settings;
    QString uuid = currentWorkspaceUuid();

    settings.setValue("workspace-" + uuid + "/windowState", saveState());
    settings.setValue("workspace-" + uuid + "/noteSubFolderDockWidgetVisible",
                      _noteSubFolderDockWidgetVisible);
}

/**
 * Restores the current workspace
 */
void MainWindow::restoreCurrentWorkspace() {
    QSettings settings;
    QStringList workspaces = getWorkspaceUuidList();

    // create a default workspace if there is none yet
    if (workspaces.count() == 0) {
        createNewWorkspace(tr("default", "default workspace"));
    }

    QString uuid = currentWorkspaceUuid();

    // set the first workspace as current workspace if there is none set
    if (uuid.isEmpty()) {
        workspaces = getWorkspaceUuidList();

        if (workspaces.count() == 0) {
            return;
        }

        uuid = workspaces.at(0);
        settings.setValue("currentWorkspace", uuid);

        // update the menu and combo box
        updateWorkspaceLists();
    }

    restoreState(settings.value(
            "workspace-" + uuid + "/windowState").toByteArray());

    // handle the visibility of the note subfolder panel
    handleNoteSubFolderVisibility();

    // update the panel lists
    updatePanelMenu();

    // check if the user wanted the note subfolder dock widget visible
    _noteSubFolderDockWidgetVisible = settings.value(
            "workspace-" + uuid + "/noteSubFolderDockWidgetVisible", true)
            .toBool();

    // set the visibility of the note subfolder dock widget
    handleNoteSubFolderVisibility();
}

/**
 * Handles the visibility of the note subfolder panel
 */
void MainWindow::handleNoteSubFolderVisibility() const {
    // turn the subfolder dock widget on or off according to whether the
    // subfolders are enabled or not
    bool showSubfolders = NoteFolder::isCurrentShowSubfolders();
    _noteSubFolderDockWidget->setVisible(
            showSubfolders && _noteSubFolderDockWidgetVisible);
}

/**
 * Returns the list of workspace uuids
 * @return
 */
QStringList MainWindow::getWorkspaceUuidList() {
    QSettings settings;
    return settings.value("workspaces").toStringList();
}

/**
 * Removes the current workspace
 */
void MainWindow::on_actionRemove_current_workspace_triggered() {
    QStringList workspaces = getWorkspaceUuidList();

    // there have to be at least one workspace
    if (workspaces.count() < 2) {
        return;
    }

    QString uuid = currentWorkspaceUuid();

    // if no workspace is set we can't remove it
    if (uuid.isEmpty()) {
        return;
    }

    // ask for permission
    if (QMessageBox::information(
            this,
            tr("Remove current workspace"),
            tr("Remove the current workspace?"),
            tr("&Remove"), tr("&Cancel"), QString::null,
            0, 1) != 0) {
        return;
    }

    // reset current workspace
    workspaces.removeAll(uuid);
    QString newUuid = workspaces.at(0);

    // set the new workspace
    setCurrentWorkspace(newUuid);

    QSettings settings;
    settings.setValue("workspaces", workspaces);

    // remove all settings in the group
    settings.beginGroup("workspace-" + uuid);
    settings.remove("");
    settings.endGroup();

    // update the menu and combo box
    updateWorkspaceLists();
}

void MainWindow::on_actionRename_current_workspace_triggered() {
    QString uuid = currentWorkspaceUuid();

    // if no workspace is set we can't rename it
    if (uuid.isEmpty()) {
        return;
    }

    QSettings settings;
    QString name = settings.value("workspace-" + uuid + "/name").toString();

    // ask for the new name
    name = QInputDialog::getText(
            this, tr("Rename workspace"), tr("Workspace name:"),
            QLineEdit::Normal, name).trimmed();

    if (name.isEmpty()) {
        return;
    }

    // rename the workspace
    settings.setValue("workspace-" + uuid + "/name", name);

    // update the menu and combo box
    updateWorkspaceLists();
}

/**
 * Switch to the previous workspace
 */
void MainWindow::on_actionSwitch_to_previous_workspace_triggered() {
    QSettings settings;
    QString uuid = settings.value("previousWorkspace").toString();

    if (!uuid.isEmpty()) {
        setCurrentWorkspace(uuid);
    }
}

/**
 * Shows all dock widgets
 */
void MainWindow::on_actionShow_all_panels_triggered() {
    QList<QDockWidget*> dockWidgets = findChildren<QDockWidget*>();

    Q_FOREACH(QDockWidget *dockWidget, dockWidgets) {
            dockWidget->setVisible(true);
        }

    _noteSubFolderDockWidgetVisible = true;

    // handle the visibility of the note subfolder panel
    handleNoteSubFolderVisibility();
}

void MainWindow::on_actionFind_action_triggered() {
    ActionDialog* dialog = new ActionDialog(ui->menuBar, this);
    dialog->show();
}
