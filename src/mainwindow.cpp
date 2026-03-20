/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "mainwindow.h"

#include <dialogs/actiondialog.h>
#include <dialogs/attachmentdialog.h>
#include <dialogs/dictionarymanagerdialog.h>
#include <dialogs/evernoteimportdialog.h>
#include <dialogs/filedialog.h>
#include <dialogs/imagedialog.h>
#include <dialogs/joplinimportdialog.h>
#include <dialogs/localtrashdialog.h>
#include <dialogs/nextclouddeckdialog.h>
#include <dialogs/notedialog.h>
#include <dialogs/scriptrepositorydialog.h>
#include <dialogs/sharedialog.h>
#include <dialogs/tabledialog.h>
#include <dialogs/tagadddialog.h>
#include <diff_match_patch.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <entities/tag.h>
#include <entities/trashitem.h>
#include <helpers/clientproxy.h>
#include <helpers/fakevimproxy.h>
#include <helpers/flowlayout.h>
#include <helpers/toolbarcontainer.h>
#include <libraries/qtwaitingspinner/waitingspinnerwidget.h>
#include <services/cryptoservice.h>
#include <services/scriptingservice.h>
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
#include <services/xdgglobalshortcutmanager.h>
#endif
#include <utils/git.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include <utils/schema.h>
#include <widgets/logwidget.h>
#include <widgets/notefilepathlabel.h>
#include <widgets/notesubfoldertree.h>
#include <widgets/notetreewidgetitem.h>

#include <QAbstractEventDispatcher>
#include <QActionGroup>
#include <QClipboard>
#include <QColorDialog>
#include <QCompleter>
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QDirIterator>
#include <QDockWidget>
#include <QFile>
#include <QGraphicsView>
#include <QGuiApplication>
#include <QInputDialog>
#include <QJSEngine>
#include <QJSValueIterator>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QMimeData>
#include <QPageSetupDialog>
#include <QPointer>
#include <QPrintDialog>
#include <QPrinter>
#include <QProgressDialog>
#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QScreen>
#include <QScrollBar>
#include <QShortcut>
#include <QSystemTrayIcon>
#include <QTemporaryFile>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QTextLength>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QUuid>
#include <QWidgetAction>
#include <QtConcurrent>
#include <QtSql/QSqlDatabase>
#include <libraries/qttoolbareditor/src/toolbar_editor.hpp>
#include <memory>
#include <utility>

#include "build_number.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/commandbar.h"
#include "dialogs/issueassistantdialog.h"
#include "dialogs/linkdialog.h"
#include "dialogs/notediffdialog.h"
#include "dialogs/passworddialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/storedattachmentsdialog.h"
#include "dialogs/storedimagesdialog.h"
#include "dialogs/tododialog.h"
#include "dialogs/versiondialog.h"
#include "entities/calendaritem.h"
#include "helpers/qownnotesmarkdownhighlighter.h"
#include "libraries/fakevim/fakevim/fakevimactions.h"
#include "libraries/fakevim/fakevim/fakevimhandler.h"
#include "libraries/sonnet/src/core/speller.h"
#include "release.h"
#include "services/databaseservice.h"
#include "services/metricsservice.h"
#include "services/nextclouddeckservice.h"
#include "services/openaiservice.h"
#include "services/owncloudservice.h"
#include "services/settingsservice.h"
#include "services/updateservice.h"
#include "services/webappclientservice.h"
#include "services/websocketserverservice.h"
#include "ui_mainwindow.h"
#include "utils/urlhandler.h"
#include "version.h"
#include "widgets/filenavigationwidget.h"
#include "widgets/htmlpreviewwidget.h"
#include "widgets/noterelationscene.h"
#include "widgets/qownnotesmarkdowntextedit.h"

// Manager includes
#include "managers/aitoolbarmanager.h"
#include "managers/distractionfreemanager.h"
#include "managers/exportprintmanager.h"
#include "managers/mediainsertionmanager.h"
#include "managers/navigationmanager.h"
#include "managers/noteencryptionmanager.h"
#include "managers/noteindexmanager.h"
#include "managers/noteoperationsmanager.h"
#include "managers/notetabmanager.h"
#include "managers/notetreemanager.h"
#include "managers/searchfiltermanager.h"
#include "managers/spellcheckmanager.h"
#include "managers/systemtraymanager.h"
#include "managers/tagmanager.h"
#include "managers/workspacemanager.h"

static MainWindow *s_self = nullptr;

struct FileWatchDisabler {
    FileWatchDisabler(MainWindow *mw) : _mainWindow(mw) {
        Q_ASSERT(mw);
        QObject::disconnect(&mw->noteDirectoryWatcher, nullptr, nullptr, nullptr);
    }

    ~FileWatchDisabler() {
        Q_ASSERT(_mainWindow);
        _mainWindow->connectFileWatcher(/*delayed = */ true);
    }

   private:
    MainWindow *const _mainWindow = nullptr;
};

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    // static reference to us
    s_self = this;
    ui = new Ui::MainWindow;

    ui->setupUi(this);

    _logWidget = new LogWidget(this);
    connect(this, &MainWindow::log, _logWidget, &LogWidget::log);

    // use our custom log handler
    qInstallMessageHandler(LogWidget::logMessageOutput);

    // Instantiate manager classes
    _spellCheckManager = new SpellCheckManager(this, ui, this);
    _exportPrintManager = new ExportPrintManager(this, ui, this);
    _noteEncryptionManager = new NoteEncryptionManager(this, ui, this);
    _distractionFreeManager = new DistractionFreeManager(this, ui, this);
    _noteTabManager = new NoteTabManager(this, ui, this);
    _noteTreeManager = new NoteTreeManager(this, ui, this);
    _tagManager = new TagManager(this, ui, this);
    _noteOperationsManager = new NoteOperationsManager(this, ui, this);
    _noteIndexManager = new NoteIndexManager(this, ui, this);
    _searchFilterManager = new SearchFilterManager(this, ui, this);
    _mediaInsertionManager = new MediaInsertionManager(this, ui, this);
    _navigationManager = new NavigationManager(this, ui, this);
    _workspaceManager = new WorkspaceManager(this, ui, this);
    SettingsService settings;

    // Disable note editing if the user has set the start in read-only mode
    if (settings.value(QStringLiteral("startInReadOnlyMode")).toBool()) {
        settings.setValue(QStringLiteral("allowNoteEditing"), false);
    }

    _noteEditIsCentralWidget =
        settings.value(QStringLiteral("noteEditIsCentralWidget"), true).toBool();

    ui->noteEditTabWidget->setTabBarAutoHide(true);
    ui->noteEditTabWidget->setTabsClosable(
        !settings.value(QStringLiteral("hideTabCloseButton")).toBool());
    ui->noteEditTabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->noteEditTabWidget->tabBar(), &QWidget::customContextMenuRequested, _noteTabManager,
            &NoteTabManager::showNoteEditTabWidgetContextMenu);

    // Set the two shortcuts for the "increase note text size" action
    const QList<QKeySequence> shortcuts = {QKeySequence(Qt::CTRL | Qt::Key_Plus),
                                           QKeySequence(Qt::CTRL | Qt::Key_Equal)};
    ui->action_Increase_note_text_size->setShortcuts(shortcuts);

    initTreeWidgets();

    initNotePreviewAndTextEdits();

    setWindowIcon(getSystemTrayIcon());

    // initialize the workspace combo box
    _workspaceManager->initWorkspaceComboBox();

#ifdef Q_OS_MAC
    // set another shortcut for delete line under macOS
    ui->actionDelete_line->setShortcut(QKeySequence(QStringLiteral("Ctrl+Backspace")));
    ui->actionDelete_word->setShortcut(QKeySequence(QStringLiteral("Alt+Backspace")));

    // set another shortcut for the full-screen mode
    ui->actionToggle_fullscreen->setShortcut(QKeySequence(QStringLiteral("Ctrl+F11")));
#endif

    _noteViewIsRegenerated = false;
    _searchLineEditFromCompleter = false;
    _isNotesDirectoryWasModifiedDisabled = false;
    _isNotesWereModifiedDisabled = false;
    _isDefaultShortcutInitialized = false;
    _showNotesFromAllNoteSubFolders = showNotesFromAllNoteSubFolders();
    _noteFolderDockWidgetWasVisible = true;
    _noteSubFolderDockWidgetVisible = true;
    _noteExternallyRemovedCheckEnabled = true;
    _readOnlyButton = new QPushButton(this);
    _lastNoteSelectionWasMultiple = false;
    _webSocketServerService = nullptr;
    _closeEventWasFired = false;
    _useNoteFolderButtons = settings.value("useNoteFolderButtons").toBool();

    this->setWindowTitle(QStringLiteral("QOwnNotes - version ") + QStringLiteral(VERSION) +
                         QStringLiteral(" - build ") + QString::number(BUILD));

    ClientProxy proxy;
    // refresh the Qt proxy settings
    proxy.setupQtProxyFromSettings();

    auto *sorting = new QActionGroup(this);
    sorting->addAction(ui->actionAlphabetical);
    sorting->addAction(ui->actionBy_date);

    auto *sortingOrder = new QActionGroup(this);
    sortingOrder->addAction(ui->actionAscending);
    sortingOrder->addAction(ui->actionDescending);
    sortingOrder->setExclusive(true);

    // hide the encrypted note text edit by default
    ui->encryptedNoteTextEdit->hide();
    ui->multiSelectActionFrame->hide();

    // initialize the tag button scroll area
    initTagButtonScrollArea();

    // initialize the toolbars
    initToolbars();

    // Initialize the Redo action as disabled (no action to redo yet)
    ui->actionRedo_action->setEnabled(false);

    if (!settings.value(QStringLiteral("guiFirstRunInit")).toBool()) {
        // hide the custom action toolbar initially
        _customActionToolbar->hide();
        settings.setValue(QStringLiteral("guiFirstRunInit"), true);
    }

#ifdef Q_OS_MAC
    // add some different shortcuts for the note history on the mac
    ui->action_Back_in_note_history->setShortcut(Qt::CTRL | Qt::ALT | Qt::Key_Left);
    ui->action_Forward_in_note_history->setShortcut(Qt::CTRL | Qt::ALT | Qt::Key_Right);

    // add another shortcut for the auto-completer
    ui->actionAutocomplete->setShortcut(Qt::META | Qt::Key_Space);

    // add another shortcut for inserting media
    ui->actionPaste_image->setShortcut(Qt::CTRL | Qt::ALT | Qt::Key_V);
#endif

    _autoReadOnlyModeTimer = new QTimer(this);
    _autoReadOnlyModeTimer->setSingleShot(true);
    connect(_autoReadOnlyModeTimer, &QTimer::timeout, this, &MainWindow::autoReadOnlyModeTimerSlot);

    // setup the update available button
    setupStatusBarWidgets();

    // read the settings (shortcuts have to be defined before that)
    readSettings();

    // do a bit more styling
    initStyling();

    // initialize the dock widgets
    initDockWidgets();

    // Delete the OpenAiService instance again, so we can add backends after the scripts are loaded
    // by initScriptingEngine()
    OpenAiService::deleteInstance();

    // initialize the scripting engine
    // initDockWidgets() has to be called before that so the scripting dock
    // widget is already in place
    initScriptingEngine();

    // Reload the OpenAI controls after the scripting engine is initialized,
    // so that scripts can add custom backends
    _aiToolbarManager->reloadOpenAiControls();

    // we need to init global shortcuts after the scriptengine is initialized
    // in case there are global shortcuts for custom actions
    // https://github.com/pbek/QOwnNotes/issues/2530
    initGlobalKeyboardShortcuts();

    // restore toolbars
    // initDockWidgets() has to be called first so panel checkboxes can be
    // used in toolbars
    restoreToolbars();

    // update the workspace menu and combobox entries again after
    // restoreToolbars() to fill the workspace combo box again
    _workspaceManager->updateWorkspaceLists();

    // Instantiate the system tray manager
    _systemTrayManager = new SystemTrayManager(this, ui, showSystemTray, this);
    _systemTrayManager->createSystemTrayIcon();

    buildNotesIndexAndLoadNoteDirectoryList(false, false, false);

    this->noteDiffDialog = new NoteDiffDialog();

    // look if we need to save something every 10 sec (default)
    this->noteSaveTimer = new QTimer(this);
    connect(this->noteSaveTimer, &QTimer::timeout, this, &MainWindow::storeUpdatedNotesToDisk);

    this->noteSaveTimer->start(this->noteSaveIntervalTime * 1000);

    // look if we need update the note view every two seconds
    _noteViewUpdateTimer = new QTimer(this);
    _noteViewUpdateTimer->setSingleShot(true);
    connect(_noteViewUpdateTimer, &QTimer::timeout, this, &MainWindow::noteViewUpdateTimerSlot);

    _noteViewUpdateTimer->start(2000);

    // commit changes from the current note folder to git every 30 sec
    gitCommitCurrentNoteFolder();
    _gitCommitTimer = new QTimer(this);
    connect(_gitCommitTimer, &QTimer::timeout, this, &MainWindow::gitCommitCurrentNoteFolder);
    _gitCommitTimer->start(_gitCommitInterval * 1000);

    // set last heartbeat in the past, so it gets called the first time
    _lastHeartbeat = QDateTime::currentDateTime().addDays(-1);

    // do some stuff periodically
    this->_frequentPeriodicTimer = new QTimer(this);
    connect(this->_frequentPeriodicTimer, &QTimer::timeout, this,
            &MainWindow::frequentPeriodicChecker);
    this->_frequentPeriodicTimer->start(60000);

    connectFileWatcher();

    ui->searchLineEdit->installEventFilter(this);
    ui->noteTreeWidget->installEventFilter(this);

    ui->tagTreeWidget->installEventFilter(this);
    ui->newNoteTagLineEdit->installEventFilter(this);
    ui->selectedTagsToolButton->installEventFilter(this);
    ui->noteEditTabWidget->tabBar()->installEventFilter(this);

    // init the saved searches completer
    initSavedSearchesCompleter();

    // called now in readSettingsFromSettingsDialog() line 494
    // set the edit mode for the note text edit
    // this->setNoteTextEditMode(true);

    // load the note folder list in the menu
    this->loadNoteFolderListMenu();

    // Update panels sort and order
    // Best don't do that with QTimer::singleShot
    // See: https://github.com/pbek/QOwnNotes/issues/2309
    // See: https://github.com/pbek/QOwnNotes/issues/2319
    updatePanelsSortOrder();

    this->updateService = new UpdateService(this);
    this->updateService->checkForUpdates(UpdateService::AppStart);

    // expire trashed items
    TrashItem::expireItems();

    // we need to restore the current workspace a little later when
    // application window is maximized or in full-screen mode
    if (isMaximized() || isFullScreen()) {
        // if it is in distraction mode we restore it immediately
        // otherwise it can result in mixed state
        if (isInDistractionFreeMode()) {
            _workspaceManager->restoreCurrentWorkspace();
        } else {
            QTimer::singleShot(500, this, SLOT(restoreCurrentWorkspace()));
        }
    }

    // update the current folder tooltip
    updateCurrentFolderTooltip();

    // show the app metrics notification if not already shown
    showAppMetricsNotificationIfNeeded();

    frequentPeriodicChecker();

    // set up the shortcuts for the note bookmarks
    setupNoteBookmarkShortcuts();

    // restore the distraction free mode
    _distractionFreeManager->restoreDistractionFreeMode();

    // add action tracking
    connect(ui->menuBar, &QMenuBar::triggered, this, &MainWindow::trackAction);

    // set "show toolbar" menu item checked/unchecked
    const QSignalBlocker blocker(ui->actionShow_toolbar);
    {
        Q_UNUSED(blocker)
        ui->actionShow_toolbar->setChecked(isToolbarVisible());
    }

    const auto toolbars = findChildren<QToolBar *>();
    for (QToolBar *toolbar : toolbars) {
        connect(toolbar, &QToolBar::visibilityChanged, this, &MainWindow::toolbarVisibilityChanged);

        // Store the current workspace when a toolbar is moved (docked/undocked)
        connect(toolbar, &QToolBar::topLevelChanged, this, [this]() { storeCurrentWorkspace(); });
    }

    // set the action group for the width selector of the distraction free mode
    auto *dfmEditorWidthActionGroup = new QActionGroup(this);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthNarrow);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthMedium);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthWide);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthFull);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthCustom);
    dfmEditorWidthActionGroup->setExclusive(true);

    connect(dfmEditorWidthActionGroup, &QActionGroup::triggered, _distractionFreeManager,
            &DistractionFreeManager::dfmEditorWidthActionTriggered);

    setAcceptDrops(true);

    // act on position clicks in the navigation widget
    connect(ui->navigationWidget, &NavigationWidget::positionClicked, this,
            &MainWindow::onNavigationWidgetPositionClicked);
    // act on heading renames in the navigation widget
    connect(ui->navigationWidget, &NavigationWidget::headingRenamed, this,
            &MainWindow::onNavigationWidgetHeadingRenamed);
    connect(ui->fileNavigationWidget, &FileNavigationWidget::positionClicked, this,
            &MainWindow::onFileNavigationWidgetPositionClicked);
    connect(ui->backlinkWidget, &BacklinkWidget::noteClicked, this,
            &MainWindow::onBacklinkWidgetNoteClicked);

    // reloads all tasks from the ownCloud server
    reloadTodoLists();

    // create a timer to load all tasks every 10min
    _todoListTimer = new QTimer(this);
    connect(_todoListTimer, &QTimer::timeout, this, &MainWindow::reloadTodoLists);
    _todoListTimer->start(600000);

    // set up the soft-wrap checkbox
    const QSignalBlocker blocker2(ui->actionUse_softwrap_in_note_editor);
    Q_UNUSED(blocker2)
    ui->actionUse_softwrap_in_note_editor->setChecked(
        settings.value(QStringLiteral("useSoftWrapInNoteEditor"), true).toBool());

    // initialize the editor soft wrapping
    initEditorSoftWrap();

    // check if user has set ownCloud settings
    MetricsService::instance()->sendEventIfEnabled(
        QStringLiteral("app/has-owncloud-settings"), QStringLiteral("app"),
        QStringLiteral("has owncloud settings"),
        OwnCloudService::hasOwnCloudSettings() ? QStringLiteral("yes") : QStringLiteral("no"));

    // send an event for counting the editor color schemes
    const int schemaCount =
        settings.value(QStringLiteral("Editor/ColorSchemes")).toStringList().count();
    MetricsService::instance()->sendEventIfEnabled(
        QStringLiteral("app/editor-color-schema-count"), QStringLiteral("editor"),
        QStringLiteral("editor color schema count"),
        QString::number(schemaCount) + QStringLiteral(" schemas"), schemaCount);

    _actionDialog = nullptr;
    _commandBar = nullptr;
    _todoDialog = nullptr;
    _storedImagesDialog = nullptr;
    _storedAttachmentsDialog = nullptr;
    _issueAssistantDialog = nullptr;

    // restore the note tabs
    Utils::Gui::restoreNoteTabs(ui->noteEditTabWidget, ui->noteEditTabWidgetLayout);

    if (isInDistractionFreeMode()) {
        ui->noteEditTabWidget->tabBar()->hide();
    }

    // restore the note history of the current note folder
    noteHistory.restoreForCurrentNoteFolder();

    if (settings.value(QStringLiteral("restoreLastNoteAtStartup"), true).toBool()) {
        // try to restore the last note before the app was quit
        // if that fails jump to the first note
        // we do that with a timer, because otherwise the scrollbar will not be
        // restored correctly, because the maximum position of the scrollbar is
        // 0
        QTimer::singleShot(250, this, SLOT(restoreActiveNoteHistoryItem()));
    }

    // wait some time for the tagTree to get visible, if selected, and apply
    // last selected tag search
    QTimer::singleShot(250, this, SLOT(filterNotesByTag()));

    // attempt to check the api app version
    startAppVersionTest();

    // attempt to quit the application when a logout is initiated
    connect(qApp, &QApplication::commitDataRequest, this, &MainWindow::on_action_Quit_triggered);

    // Register the LogWidget::LogType type so showStatusBarMessage there doesn't throw a warning,
    // like this: `QMetaMethod::invoke: Unable to handle unregistered datatype 'LogWidget::LogType'`
    qRegisterMetaType<LogWidget::LogType>("LogWidget::LogType");

    automaticScriptUpdateCheck();

    // trigger cli parameter menu action if there was any set
    triggerStartupMenuAction();

    resizeTagTreeWidgetColumnToContents();

    // check if we want to start the application hidden
    initShowHidden();

    // Update the ignored click url regexps for the note text edits
    ui->encryptedNoteTextEdit->updateIgnoredClickUrlRegexps();
    ui->noteTextEdit->updateIgnoredClickUrlRegexps();
}

void MainWindow::initTreeWidgets() {
    connect(ui->noteSubFolderTreeWidget, &NoteSubFolderTree::multipleSubfoldersSelected, this,
            &MainWindow::onMultipleSubfoldersSelected);
    connect(ui->noteSubFolderTreeWidget, &NoteSubFolderTree::currentSubFolderChanged, this,
            &MainWindow::onCurrentSubFolderChanged);
    if (NoteFolder::isCurrentNoteTreeEnabled()) {
        connect(ui->noteTreeWidget, &QTreeWidget::itemExpanded, ui->noteSubFolderTreeWidget,
                &NoteSubFolderTree::onItemExpanded);
        connect(ui->noteTreeWidget, &QTreeWidget::itemCollapsed, ui->noteSubFolderTreeWidget,
                &NoteSubFolderTree::onItemExpanded);
    }
}

void MainWindow::initNotePreviewAndTextEdits() {
    SettingsService settings;

    // set the search frames for the note text edits
    const bool darkMode = settings.value(QStringLiteral("darkMode")).toBool();
    ui->noteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame, darkMode);
    ui->encryptedNoteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame, darkMode);
    ui->noteTextEdit->setCurrentNoteReference(QString());

    // setup vim mode
    if (settings.value(QStringLiteral("Editor/vimMode")).toBool()) {
        initFakeVim(ui->noteTextEdit);
        initFakeVim(ui->encryptedNoteTextEdit);
    }

    // do the navigation parsing after the highlighter was finished
    connect(ui->noteTextEdit->highlighter(), &QOwnNotesMarkdownHighlighter::highlightingFinished,
            this, &MainWindow::startNavigationParser);

    connect(ui->encryptedNoteTextEdit->highlighter(),
            &QOwnNotesMarkdownHighlighter::highlightingFinished, this,
            &MainWindow::startNavigationParser);

    // track cursor position changes for the line number label
    connect(ui->noteTextEdit, &QOwnNotesMarkdownTextEdit::cursorPositionChanged, this,
            &MainWindow::noteEditCursorPositionChanged);

    // track cursor position changes for the line number label
    connect(ui->encryptedNoteTextEdit, &QOwnNotesMarkdownTextEdit::cursorPositionChanged, this,
            &MainWindow::noteEditCursorPositionChanged);

    // TODO: Remove and handle this in widgets directly
    ui->noteTextEdit->installEventFilter(this);
    ui->noteTextEdit->viewport()->installEventFilter(this);
    ui->encryptedNoteTextEdit->installEventFilter(this);
    ui->encryptedNoteTextEdit->viewport()->installEventFilter(this);

    // Allow to update the preview if the zoom level of the note text edits change
    // Using regenerateNotePreview(), refreshNotePreview(true) or ui->noteTextView->repaint() didn't
    // work
    connect(ui->noteTextEdit, &QOwnNotesMarkdownTextEdit::zoomIn, this,
            &MainWindow::forceRegenerateNotePreview);
    connect(ui->noteTextEdit, &QOwnNotesMarkdownTextEdit::zoomOut, this,
            &MainWindow::forceRegenerateNotePreview);
    connect(ui->encryptedNoteTextEdit, &QOwnNotesMarkdownTextEdit::zoomIn, this,
            &MainWindow::forceRegenerateNotePreview);
    connect(ui->encryptedNoteTextEdit, &QOwnNotesMarkdownTextEdit::zoomOut, this,
            &MainWindow::forceRegenerateNotePreview);

#ifdef USE_QLITEHTML
    _notePreviewWidget = new HtmlPreviewWidget(this);
    auto *noteViewLayout = ui->noteViewFrame->layout();
    if (!noteViewLayout) {
        noteViewLayout = new QVBoxLayout(ui->noteViewFrame);
        ui->noteViewFrame->setLayout(noteViewLayout);
    }

    noteViewLayout->setContentsMargins(0, 0, 0, 0);
    noteViewLayout->setSpacing(0);

    auto *noteViewGridLayout = qobject_cast<QGridLayout *>(noteViewLayout);
    if (noteViewGridLayout != nullptr) {
        noteViewGridLayout->addWidget(_notePreviewWidget, 1, 1);
        noteViewGridLayout->setRowStretch(1, 1);
        noteViewGridLayout->setColumnStretch(1, 1);
    } else {
        noteViewLayout->addWidget(_notePreviewWidget);
    }

    // QTextBrowser previewer is hidden when we use qlitehtml
    ui->noteTextView->setVisible(false);
    ui->noteTextViewSearchFrame->setVisible(false);

    // TODO: remove this, and handle stuff in the widget directly
    _notePreviewWidget->installEventFilter(this);
    _notePreviewWidget->viewport()->installEventFilter(this);

    connect(_notePreviewWidget, &HtmlPreviewWidget::anchorClicked, this,
            &MainWindow::onNotePreviewAnchorClicked);

#else
    ui->noteTextView->installEventFilter(this);
    ui->noteTextView->viewport()->installEventFilter(this);

    connect(ui->noteTextView, &NotePreviewWidget::resize, this, &MainWindow::onNoteTextViewResize);

    // TODO centralize dark mode handling
    ui->noteTextView->initSearchFrame(ui->noteTextViewSearchFrame, darkMode);

    connect(ui->noteTextView, &QTextBrowser::anchorClicked, this,
            &MainWindow::onNotePreviewAnchorClicked);
#endif
}

void MainWindow::connectFileWatcher(bool delayed) {
    _noteIndexManager->connectFileWatcher(delayed);
}

/**
 * Triggers the cli parameter menu action if there was any set
 */
void MainWindow::triggerStartupMenuAction() {
    const auto actionName = qApp->property("startupAction").toString();

    if (actionName.isEmpty()) {
        return;
    }

    QAction *action = findAction(actionName);

    // use a single-shot timer to prevent crashes when called by SingleApplication::receivedMessage
    QTimer::singleShot(0, this, [this, actionName, action]() {
        if (action != nullptr) {
            qDebug() << "Running menu action: " << actionName;
            action->trigger();
        } else {
            Utils::Gui::warning(this, tr("Action not found!"),
                                tr("Could not find menu action <code>%1</code>! "
                                   "Did you spell it correctly?")
                                    .arg(actionName),
                                "menu-action-not-found");
        }
    });
}

/**
 * Initializes the global shortcuts
 */
void MainWindow::initGlobalKeyboardShortcuts() {
    // Deleting old global shortcut assignments
    foreach (QHotkey *hotKey, _globalShortcuts) {
        delete hotKey;
    }

    _globalShortcuts.clear();
    SettingsService settings;
    settings.beginGroup(QStringLiteral("GlobalShortcuts"));

    const auto allKeys = settings.allKeys();

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    const bool isWayland = QGuiApplication::platformName() == QStringLiteral("wayland");

    if (isWayland) {
        initWaylandGlobalShortcuts(settings, allKeys);
        return;
    }
#endif

    if (allKeys.isEmpty()) {
        return;
    }

    initX11GlobalShortcuts(settings, allKeys);
}

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
/**
 * Initializes global shortcuts via XDG Desktop Portal on Wayland
 */
void MainWindow::initWaylandGlobalShortcuts(SettingsService &settings, const QStringList &allKeys) {
    // Clean up existing Wayland shortcut manager
    if (_xdgShortcutManager) {
        _xdgShortcutManager->closeSession();
        _xdgShortcutManager->deleteLater();
        _xdgShortcutManager = nullptr;
    }

    if (allKeys.isEmpty()) {
        return;
    }

    if (!XdgGlobalShortcutManager::isAvailable()) {
        qWarning() << "XDG GlobalShortcuts portal is not available. "
                      "Global shortcuts will not work on this Wayland session.";
        return;
    }

    _xdgShortcutManager = new XdgGlobalShortcutManager(this);

    QMap<QString, XdgGlobalShortcutManager::ShortcutInfo> shortcuts;

    for (const QString &key : allKeys) {
        if (!key.contains(QStringLiteral("MainWindow"))) {
            continue;
        }

        QString actionName = key;
        actionName.remove(QStringLiteral("MainWindow-"));
        QAction *action = findAction(actionName);

        if (action == nullptr) {
            qDebug() << "Failed to find action with name:" << actionName;
            continue;
        }

        QString shortcutStr = settings.value(key).toString();
        if (shortcutStr.isEmpty()) {
            continue;
        }

        XdgGlobalShortcutManager::ShortcutInfo info;
        info.keySequence = QKeySequence(shortcutStr);
        info.description = action->text().remove(QLatin1Char('&'));

        shortcuts.insert(actionName, info);
    }

    if (shortcuts.isEmpty()) {
        _xdgShortcutManager->deleteLater();
        _xdgShortcutManager = nullptr;
        return;
    }

    _xdgShortcutManager->setShortcuts(shortcuts);

    // Connect the activation signal to trigger the corresponding action
    connect(_xdgShortcutManager, &XdgGlobalShortcutManager::shortcutActivated, this,
            [this](const QString &shortcutId) {
                QAction *action = findAction(shortcutId);
                if (action == nullptr) {
                    qDebug() << "XDG GlobalShortcuts: failed to find action:" << shortcutId;
                    return;
                }

                qDebug() << "XDG global shortcut action triggered:" << action->objectName();

                // Don't call showWindow() for the "Show/Hide application" action
                // because it will call it itself
                if (action->objectName() != QStringLiteral("actionShow_Hide_application")) {
                    showWindow();
                }

                action->trigger();
            });

    // Start the session creation and shortcut binding flow
    _xdgShortcutManager->createSession();
}
#endif

/**
 * Initializes global shortcuts via QHotkey (X11/Windows/macOS)
 */
void MainWindow::initX11GlobalShortcuts(SettingsService &settings, const QStringList &allKeys) {
    for (const QString &key : allKeys) {
        if (!key.contains(QStringLiteral("MainWindow"))) {
            continue;
        }

        QString actionName = key;
        actionName.remove(QStringLiteral("MainWindow-"));
        QAction *action = findAction(actionName);

        if (action == nullptr) {
            qDebug() << "Failed to find action with name:" << actionName;
            continue;
        }

        QString shortcut = settings.value(key).toString();

        auto hotKey = new QHotkey(QKeySequence(shortcut), true, this);
        _globalShortcuts.append(hotKey);
        connect(hotKey, &QHotkey::activated, this, [this, action]() {
            qDebug() << "Global shortcut action triggered:" << action->objectName();

            // Don't call showWindow() for the "Show/Hide application" action
            // because it will call it itself
            if (action->objectName() != QStringLiteral("actionShow_Hide_application")) {
                // Bring application window to the front
                showWindow();
            }

            action->trigger();
        });
    }
}

void MainWindow::initWebSocketServerService() {
    _webSocketServerService = new WebSocketServerService();
}

void MainWindow::initWebAppClientService() { _webAppClientService = new WebAppClientService(); }

void MainWindow::initFakeVim(QOwnNotesMarkdownTextEdit *noteTextEdit) {
    auto handler = new FakeVim::Internal::FakeVimHandler(noteTextEdit, this);
    new FakeVimProxy(noteTextEdit, handler);
}

/**
 * Attempts to check the api app version
 */
void MainWindow::startAppVersionTest() {
    if (!OwnCloudService::hasOwnCloudSettings() &&
        CloudConnection::currentCloudConnection().getAppQOwnNotesAPIEnabled()) {
        return;
    }

    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->startAppVersionTest();
}

/**
 * Restores the active note history item
 */
bool MainWindow::restoreActiveNoteHistoryItem() {
    SettingsService settings;
    QVariant var = settings.value(QStringLiteral("ActiveNoteHistoryItem"));
    //    qDebug() << __func__ << " - 'var': " << var;

    // check if the NoteHistoryItem could be de-serialized
    if (var.isValid()) {
        NoteHistoryItem noteHistoryItem = var.value<NoteHistoryItem>();
        //        qDebug() << __func__ << " - 'noteHistoryItem': " <<
        //        noteHistoryItem;

        if (jumpToNoteHistoryItem(noteHistoryItem)) {
            noteHistoryItem.restoreTextEditPosition(ui->noteTextEdit);
            reloadCurrentNoteTags();
            return true;
        }
    }

    // if restoring the last note failed jump to the first note
    resetCurrentNote();

    reloadCurrentNoteTags();

    return false;
}

MainWindow::~MainWindow() {
    _noteRelationScene->stopDrawing();
    disableFullScreenMode();

    const bool forceQuit = qApp->property("clearAppDataAndExit").toBool();

    // make sure no settings get written after we got the
    // clearAppDataAndExit call
    if (!forceQuit) {
        storeSettings();
    }

    // Checks will be done in the method
    storeCurrentWorkspace();

    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("app/end"),
                                                   QStringLiteral("app end"));

    storeUpdatedNotesToDisk();

    // commit the changes in the current note folder to git
    // (#1269, may cause an interruption of the shutdown process)
    //    gitCommitCurrentNoteFolder();

    delete ui;

    s_self = nullptr;
}

/*!
 * Methods
 */

/**
 * Initializes the workspace combo box
 */
void MainWindow::initWorkspaceComboBox() { _workspaceManager->initWorkspaceComboBox(); }

/**
 * Initializes the dock widgets
 */
void MainWindow::initDockWidgets() {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    setDockOptions(dockOptions() | GroupedDragging);
#endif
    QSizePolicy sizePolicy;

    _noteSubFolderDockWidget = new QDockWidget(tr("Subfolders"), this);
    _noteSubFolderDockWidget->setObjectName(QStringLiteral("noteSubFolderDockWidget"));
    _noteSubFolderDockWidget->setWidget(ui->noteSubFolderFrame);
    _noteSubFolderDockTitleBarWidget = _noteSubFolderDockWidget->titleBarWidget();
    sizePolicy = _noteSubFolderDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteSubFolderDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteSubFolderDockWidget, Qt::Horizontal);

    _taggingDockWidget = new QDockWidget(tr("Tags"), this);
    _taggingDockWidget->setObjectName(QStringLiteral("taggingDockWidget"));
    _taggingDockWidget->setWidget(ui->tagFrame);
    _taggingDockTitleBarWidget = _taggingDockWidget->titleBarWidget();
    sizePolicy = _taggingDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _taggingDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _taggingDockWidget, Qt::Vertical);

    _noteSearchDockWidget = new QDockWidget(tr("Note search"), this);
    _noteSearchDockWidget->setObjectName(QStringLiteral("noteSearchDockWidget"));
    _noteSearchDockWidget->setWidget(ui->searchLineEdit);
    _noteSearchDockTitleBarWidget = _noteSearchDockWidget->titleBarWidget();
    sizePolicy = _noteSearchDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteSearchDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteSearchDockWidget, Qt::Vertical);

    _noteFolderDockWidget = new QDockWidget(tr("Note folder"), this);
    _noteFolderDockWidget->setObjectName(QStringLiteral("noteFolderDockWidget"));

    if (_useNoteFolderButtons) {
        _noteFolderDockWidgetFrame = new QFrame(_noteFolderDockWidget);
        _noteFolderDockWidgetFrame->setLayout(new FlowLayout());
        _noteFolderDockWidget->setWidget(_noteFolderDockWidgetFrame);
        ui->noteFolderComboBox->setHidden(true);
    } else {
        _noteFolderDockWidget->setWidget(ui->noteFolderComboBox);
    }

    _noteFolderDockTitleBarWidget = _noteFolderDockWidget->titleBarWidget();
    sizePolicy = _noteFolderDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteFolderDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteFolderDockWidget, Qt::Vertical);

    _noteListDockWidget = new QDockWidget(tr("Note list"), this);
    _noteListDockWidget->setObjectName(QStringLiteral("noteListDockWidget"));
    _noteListDockWidget->setWidget(ui->notesListFrame);
    _noteListDockTitleBarWidget = _noteListDockWidget->titleBarWidget();
    sizePolicy = _noteListDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteListDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteListDockWidget, Qt::Vertical);

    _noteNavigationDockWidget = new QDockWidget(tr("Navigation"), this);
    _noteNavigationDockWidget->setObjectName(QStringLiteral("noteNavigationDockWidget"));
    _noteNavigationDockWidget->setWidget(ui->navigationFrame);
    _noteNavigationDockTitleBarWidget = _noteNavigationDockWidget->titleBarWidget();
    sizePolicy = _noteNavigationDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteNavigationDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteNavigationDockWidget, Qt::Vertical);
    // we want the navigation under the note list
    splitDockWidget(_noteListDockWidget, _noteNavigationDockWidget, Qt::Vertical);
    // We want to start with the Navigation tab
    ui->navigationTabWidget->setCurrentIndex(0);

    if (!_noteEditIsCentralWidget) {
        createNoteEditDockWidget();
    }

    _noteTagDockWidget = new QDockWidget(tr("Note tags"), this);
    _noteTagDockWidget->setObjectName(QStringLiteral("noteTagDockWidget"));
    _noteTagDockWidget->setWidget(ui->noteTagFrame);
    _noteTagDockTitleBarWidget = _noteTagDockWidget->titleBarWidget();
    sizePolicy = _noteTagDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(5);
    _noteTagDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(_noteEditIsCentralWidget ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea,
                  _noteTagDockWidget, Qt::Vertical);

    _notePreviewDockWidget = new QDockWidget(tr("Note preview"), this);
    _notePreviewDockWidget->setObjectName(QStringLiteral("notePreviewDockWidget"));
    _notePreviewDockWidget->setWidget(ui->noteViewFrame);
    _notePreviewDockTitleBarWidget = _notePreviewDockWidget->titleBarWidget();
    addDockWidget(Qt::RightDockWidgetArea, _notePreviewDockWidget, Qt::Horizontal);

    _noteGraphicsViewDockWidget = new QDockWidget(tr("Note relations"), this);
    _noteGraphicsViewDockWidget->setObjectName(QStringLiteral("noteGraphicsViewDockWidget"));
    _noteGraphicsViewDockWidget->setWidget(ui->noteGraphicsView);
    _noteGraphicsViewDockTitleBarWidget = _noteGraphicsViewDockWidget->titleBarWidget();
    addDockWidget(Qt::RightDockWidgetArea, _noteGraphicsViewDockWidget, Qt::Horizontal);
    _noteGraphicsViewDockWidget->hide();
    // Prevent that widget can't be seen when enabled
    _noteGraphicsViewDockWidget->setMinimumHeight(20);
    setupNoteRelationScene();

    _logDockWidget = new QDockWidget(tr("Log"), this);
    _logDockWidget->setObjectName(QStringLiteral("logDockWidget"));
    _logDockWidget->setWidget(_logWidget);
    _logDockTitleBarWidget = _logDockWidget->titleBarWidget();
    addDockWidget(Qt::RightDockWidgetArea, _logDockWidget, Qt::Vertical);
    _logDockWidget->hide();
    // prevent that log widgets can't be seen when enabled
    _logDockWidget->setMinimumHeight(20);

    _scriptingDockWidget = new QDockWidget(tr("Scripting"), this);
    _scriptingDockWidget->setObjectName(QStringLiteral("scriptingDockWidget"));
    _scriptingDockWidget->setWidget(ui->scriptingScrollArea);
    _scriptingDockTitleBarWidget = _scriptingDockWidget->titleBarWidget();
    addDockWidget(Qt::RightDockWidgetArea, _scriptingDockWidget, Qt::Vertical);
    _scriptingDockWidget->hide();
    // we only needed that label to set a layout in QtCreator
    delete ui->scriptingDemoLabel;

    SettingsService settings;

    // forcing some dock widget sizes on the first application start
    if (!settings.value(QStringLiteral("dockWasInitializedOnce")).toBool()) {
        // setting a small height for the note tag panel
        _noteTagDockWidget->setMaximumHeight(40);

        // giving the left panels with the note list a fifth of the screen
        _noteListDockWidget->setMaximumWidth(width() / 5);

        // giving the preview pane a third of the screen, the rest goes to the
        // note edit pane
        _notePreviewDockWidget->setMaximumWidth(width() / 3);

        settings.setValue(QStringLiteral("dockWasInitializedOnce"), true);

        // releasing the forced maximum sizes
        QTimer::singleShot(250, this, SLOT(releaseDockWidgetSizes()));
    }

    //    ui->noteEditTabWidget->setStyleSheet("* { border: none; }");
    //    ui->noteTextEdit->setStyleSheet("* { border: none; }");
    //    ui->noteEditTabWidget->layout()->setContentsMargins(0, 0, 0, 0);

    setDockNestingEnabled(true);
    setCentralWidget(_noteEditIsCentralWidget ? ui->noteEditTabWidget : nullptr);
    updateNoteEditFrameShape();

    // restore the current workspace
    restoreCurrentWorkspace();

    // lock the dock widgets
    on_actionUnlock_panels_toggled(false);

    // update the workspace menu and combobox entries
    updateWorkspaceLists();

    // initialize the panel menu
    initPanelMenu();
}

void MainWindow::createNoteEditDockWidget() {
    if (_noteEditDockWidget != nullptr) {
        return;
    }

    auto *noteEditDockWidget = new QDockWidget(tr("Note edit"), this);
    noteEditDockWidget->setObjectName(QStringLiteral("noteEditDockWidget"));
    noteEditDockWidget->setWidget(ui->noteEditTabWidget);

    QSizePolicy sizePolicy = noteEditDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(5);
    noteEditDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::RightDockWidgetArea, noteEditDockWidget, Qt::Horizontal);

    _noteEditDockWidget = noteEditDockWidget;
    _noteEditDockTitleBarWidget = _noteEditDockWidget->titleBarWidget();
}

void MainWindow::setNoteEditCentralWidgetEnabled(bool enabled) {
    if (_noteEditIsCentralWidget == enabled) {
        return;
    }

    QWidget *focusWidget = qApp->focusWidget();
    const bool panelsUnlocked = ui->actionUnlock_panels->isChecked();
    _noteEditIsCentralWidget = enabled;

    if (_noteEditIsCentralWidget) {
        if (_noteEditDockWidget != nullptr) {
            _noteEditDockWidget->hide();
            ui->noteEditTabWidget->hide();
            ui->noteEditTabWidget->setParent(this);
            removeDockWidget(_noteEditDockWidget);
            delete _noteEditDockWidget;
            _noteEditDockWidget = nullptr;
            _noteEditDockTitleBarWidget = nullptr;
        }

        setCentralWidget(ui->noteEditTabWidget);
    } else {
        if (centralWidget() == ui->noteEditTabWidget) {
            takeCentralWidget();
        }

        createNoteEditDockWidget();
    }

    if ((_noteTagDockWidget != nullptr) && !_noteTagDockWidget->isFloating()) {
        removeDockWidget(_noteTagDockWidget);
        addDockWidget(_noteEditIsCentralWidget ? Qt::LeftDockWidgetArea : Qt::RightDockWidgetArea,
                      _noteTagDockWidget, Qt::Vertical);
    }

    updateNoteEditFrameShape();
    setDockNestingEnabled(true);
    initPanelMenu();
    on_actionUnlock_panels_toggled(panelsUnlocked);
    updatePanelMenu();

    if (focusWidget != nullptr) {
        focusWidget->setFocus();
    }
}

void MainWindow::updateNoteEditFrameShape() const {
#ifdef Q_OS_LINUX
    const auto shape = _noteEditIsCentralWidget ? QFrame::StyledPanel : QFrame::NoFrame;
    ui->noteTextEdit->setFrameShape(shape);
    ui->encryptedNoteTextEdit->setFrameShape(shape);
#endif
}

void MainWindow::setupNoteRelationScene() {
    _noteRelationScene = new NoteRelationScene();
    ui->noteGraphicsView->setScene(_noteRelationScene);
    ui->noteGraphicsView->setRenderHint(QPainter::Antialiasing);
}

/**
 * Releasing the forced maximum sizes on some dock widgets
 */
void MainWindow::releaseDockWidgetSizes() {
    _noteListDockWidget->setMaximumWidth(10000);
    _notePreviewDockWidget->setMaximumWidth(10000);
    _noteTagDockWidget->setMaximumHeight(10000);
}

/**
 * Initializes if we want to start the application hidden
 */
void MainWindow::initShowHidden() {
    SettingsService settings;
    const bool startHidden = settings.value(QStringLiteral("StartHidden"), false).toBool();

    if (startHidden) {
        QTimer::singleShot(0, this, SLOT(hide()));
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
    _noteTagButtonScrollArea->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
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
    ui->selectedTagsToolButton->setVisible(false);
}

/**
 * Returns all menus from the menu bar
 */
QList<QMenu *> MainWindow::menuList() { return ui->menuBar->findChildren<QMenu *>(); }

/**
 * Finds an action in all menus of the menu bar
 */
QAction *MainWindow::findAction(const QString &objectName) {
    const QList<QMenu *> menus = menuList();

    // loop through all menus because we were not able to find the action with
    // ui->menuBar->findChild<QAction *>(objectName);
    for (QMenu *menu : menus) {
        // loop through all actions of the menu
        const auto menuActions = menu->actions();
        for (QAction *action : menuActions) {
            if (action->objectName() == objectName) {
                return action;
            }
        }
    }

    return nullptr;
}

/**
 * Builds the note index and loads the note directory list
 *
 * @param forceBuild
 * @param forceLoad
 * @param reloadTabs
 */
void MainWindow::buildNotesIndexAndLoadNoteDirectoryList(bool forceBuild, bool forceLoad,
                                                         bool reloadTabs) {
    _noteIndexManager->buildNotesIndexAndLoadNoteDirectoryList(forceBuild, forceLoad, reloadTabs);
}

/**
 * Returns the global main window instance
 */
MainWindow *MainWindow::instance() { return s_self; }

/**
 * Initializes the editor soft wrapping
 */
void MainWindow::initEditorSoftWrap() {
    SettingsService settings;
    const bool useSoftWrapInNoteEditor =
        settings.value(QStringLiteral("useSoftWrapInNoteEditor"), true).toBool();

    QTextEdit::LineWrapMode mode =
        useSoftWrapInNoteEditor ? QTextEdit::WidgetWidth : QTextEdit::NoWrap;
    QPlainTextEdit::LineWrapMode pMode =
        useSoftWrapInNoteEditor ? QPlainTextEdit::WidgetWidth : QPlainTextEdit::NoWrap;
    Q_UNUSED(mode)

    ui->noteTextEdit->setLineWrapMode(pMode);
    ui->encryptedNoteTextEdit->setLineWrapMode(pMode);

#ifndef USE_QLITEHTML
    ui->noteTextView->setLineWrapMode(mode);
#endif
}

/**
 * Reloads all tasks from the ownCloud server
 */
void MainWindow::reloadTodoLists() {
    if (!OwnCloudService::isTodoCalendarSupportEnabled()) {
        return;
    }

    SettingsService settings;
    QStringList calendars =
        settings.value(QStringLiteral("ownCloud/todoCalendarEnabledList")).toStringList();
    QString serverUrl = CloudConnection::currentCloudConnection().getServerUrl();

    if (calendars.count() > 0 && !serverUrl.isEmpty()) {
        OwnCloudService *ownCloud = OwnCloudService::instance();

        QListIterator<QString> itr(calendars);
        while (itr.hasNext()) {
            QString calendar = itr.next();
            ownCloud->todoGetTodoList(calendar, nullptr);
        }

        showStatusBarMessage(tr("Your tasks are being loaded from your server"),
                             QStringLiteral("☑️"), 4000);

        // generate the system tray context menu to show modified tasks
        // in 15 sec (because we don't know when all new tasks will be loaded)
        QTimer::singleShot(15000, this, SLOT(generateSystemTrayContextMenu()));
    }
}

/**
 * Initializes the scripting engine
 */
void MainWindow::initScriptingEngine() {
    qDebug() << __func__ << " - 'initScriptingEngine'";

    ui->menuCustom_actions->hide();
    //    _customActionToolbar->hide();

    ScriptingService *scriptingService = ScriptingService::createInstance(this);
    QQmlEngine *engine = scriptingService->engine();
    //    engine->setObjectOwnership(ui->noteTextEdit,
    //    QQmlEngine::CppOwnership);
    engine->rootContext()->setContextProperty(QStringLiteral("noteTextEdit"), ui->noteTextEdit);

    scriptingService->initComponents();
}

/**
 * Invokes the custom action in the scripting service
 *
 * @param identifier
 */
void MainWindow::onCustomActionInvoked(const QString &identifier) {
    ScriptingService *scriptingService = ScriptingService::instance();
    if (scriptingService != nullptr) {
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
    _formattingToolbar->addAction(ui->actionInsert_code_block);
    _formattingToolbar->addAction(ui->actionInsert_block_quote);
    _formattingToolbar->setObjectName(QStringLiteral("formattingToolbar"));
    addToolBar(_formattingToolbar);

    _insertingToolbar = new QToolBar(tr("inserting toolbar"), this);
    _insertingToolbar->addAction(ui->actionInsert_text_link);
    _insertingToolbar->addAction(ui->actionInsert_image);
    _insertingToolbar->addAction(ui->actionInsert_current_time);
    _insertingToolbar->setObjectName(QStringLiteral("insertingToolbar"));
    addToolBar(_insertingToolbar);

    _encryptionToolbar = new QToolBar(tr("encryption toolbar"), this);
    _encryptionToolbar->addAction(ui->action_Encrypt_note);
    _encryptionToolbar->addAction(ui->actionEdit_encrypted_note);
    _encryptionToolbar->addAction(ui->actionDecrypt_note);
    _encryptionToolbar->setObjectName(QStringLiteral("encryptionToolbar"));
    addToolBar(_encryptionToolbar);

    auto *aiToolbar = new QToolBar(tr("AI toolbar"), this);
    aiToolbar->setObjectName(QStringLiteral("aiToolbar"));
    addToolBar(aiToolbar);

    // Instantiate the AI toolbar manager with the toolbar
    _aiToolbarManager = new AiToolbarManager(this, ui, aiToolbar, this);

    _windowToolbar = new QToolBar(tr("window toolbar"), this);
    updateWindowToolbar();
    _windowToolbar->setObjectName(QStringLiteral("windowToolbar"));
    addToolBar(_windowToolbar);

    _customActionToolbar = new QToolBar(tr("custom action toolbar"), this);
    _customActionToolbar->setObjectName(QStringLiteral("customActionsToolbar"));
    //    _customActionToolbar->hide();
    addToolBar(_customActionToolbar);

    _quitToolbar = new QToolBar(tr("quit toolbar"), this);
    _quitToolbar->addAction(ui->action_Quit);
    _quitToolbar->setObjectName(QStringLiteral("quitToolbar"));
    addToolBar(_quitToolbar);

    // Connect all toolbars to track action triggers
    connect(_formattingToolbar, &QToolBar::actionTriggered, this, &MainWindow::trackAction);
    connect(_insertingToolbar, &QToolBar::actionTriggered, this, &MainWindow::trackAction);
    connect(_encryptionToolbar, &QToolBar::actionTriggered, this, &MainWindow::trackAction);
    connect(aiToolbar, &QToolBar::actionTriggered, this, &MainWindow::trackAction);
    connect(_windowToolbar, &QToolBar::actionTriggered, this, &MainWindow::trackAction);
    connect(_customActionToolbar, &QToolBar::actionTriggered, this, &MainWindow::trackAction);
    connect(_quitToolbar, &QToolBar::actionTriggered, this, &MainWindow::trackAction);
}

/**
 * Populates the window toolbar
 */
void MainWindow::updateWindowToolbar() {
    _windowToolbar->clear();

    auto *widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(_workspaceManager->workspaceComboBox());
    widgetAction->setObjectName(QStringLiteral("actionWorkspaceComboBox"));
    widgetAction->setText(tr("Workspace selector"));
    _windowToolbar->addAction(widgetAction);
    _windowToolbar->addAction(ui->actionStore_as_new_workspace);
    _windowToolbar->addAction(ui->actionRemove_current_workspace);
    _windowToolbar->addAction(ui->actionRename_current_workspace);
    _windowToolbar->addAction(ui->actionSwitch_to_previous_workspace);
    _windowToolbar->addAction(ui->actionUnlock_panels);

    _windowToolbar->addSeparator();
    _windowToolbar->addAction(ui->actionFold_all_headings);
    _windowToolbar->addAction(ui->actionUnfold_all_headings);
    _windowToolbar->addAction(ui->actionToggle_distraction_free_mode);
    _windowToolbar->addAction(ui->action_Increase_note_text_size);
    _windowToolbar->addAction(ui->action_Decrease_note_text_size);
    _windowToolbar->addAction(ui->action_Reset_note_text_size);
}

/**
 * Updates the workspace menu and combobox entries
 */
void MainWindow::updateWorkspaceLists(bool rebuild) {
    _workspaceManager->updateWorkspaceLists(rebuild);
}

/**
 * Initializes the panel menu
 */
void MainWindow::initPanelMenu() {
    // update the panel menu if the visibility of a panel was changed
    const auto dockWidgets = findChildren<QDockWidget *>();
    for (QDockWidget *dockWidget : dockWidgets) {
        QObject::disconnect(dockWidget, SIGNAL(visibilityChanged(bool)), this,
                            SLOT(updatePanelMenu()));

        // seems to crash the application on exit
        //        connect(dockWidget, &QDockWidget::visibilityChanged, this,
        //        [this](){
        //            updatePanelMenu();
        //        });

        // this connect works without crash, it doesn't seem to trigger on exit
        QObject::connect(dockWidget, SIGNAL(visibilityChanged(bool)), this,
                         SLOT(updatePanelMenu()));

        // we are disabling the dock widget context menu to prevent enabling
        // of the note subfolder toolbar if subfolders are disabled
        dockWidget->setContextMenuPolicy(Qt::PreventContextMenu);
    }
}

/**
 * Initializes the toolbar menu
 */
void MainWindow::initToolbarMenu() {
    // update the toolbar menu if the visibility of a toolbar was changed
    const auto toolbars = findChildren<QToolBar *>();
    for (QToolBar *toolbar : toolbars) {
        // in case the connection was already established
        QObject::disconnect(toolbar, &QToolBar::visibilityChanged, this,
                            &MainWindow::updateToolbarMenu);
        QObject::connect(toolbar, &QToolBar::visibilityChanged, this,
                         &MainWindow::updateToolbarMenu);

        // we are disabling the toolbar context menu to prevent enabling of the
        // note subfolder toolbar if subfolders are disabled
        toolbar->setContextMenuPolicy(Qt::PreventContextMenu);
    }
}

/**
 * Updates the panel menu entries
 */
void MainWindow::updatePanelMenu() {
    qDebug() << __func__ << " - 'updatePanelMenu'";

    ui->menuPanels->clear();
    SettingsService settings;

    const auto dockWidgets = findChildren<QDockWidget *>();
    for (QDockWidget *dockWidget : dockWidgets) {
        auto *action = new QAction(this);
        const QString objectName = QStringLiteral("togglePanel-") + dockWidget->objectName();

        action->setText(tr("Show %1 panel").arg(dockWidget->windowTitle()));
        action->setObjectName(objectName);
        action->setCheckable(true);
        action->setChecked(!dockWidget->isHidden());

        // disable the noteSubFolderDockWidget menu entry if subfolders are
        // not enabled
        if (dockWidget->objectName() == QStringLiteral("noteSubFolderDockWidget") &&
            !NoteFolder::isCurrentShowSubfolders()) {
            action->setEnabled(false);
        }

        // try to load a key sequence from the settings
        QKeySequence shortcut = QKeySequence(
            settings.value(QStringLiteral("Shortcuts/MainWindow-") + objectName).toString());
        action->setShortcut(shortcut);

        // toggle the panel if the checkbox was triggered
        connect(action, &QAction::triggered, this, [this, dockWidget]() {
            togglePanelVisibility(dockWidget->objectName());
            updateJumpToActionsAvailability();
        });

        ui->menuPanels->addAction(action);
    }

    updateJumpToActionsAvailability();

    // update the preview in case it was disabled previously
    if (_notePreviewDockWidget->isVisible()) {
        setNoteTextFromNote(&currentNote, true);
    }
}

/**
 * Updates the toolbar menu entries
 */
void MainWindow::updateToolbarMenu() {
    ui->menuToolbars->clear();

    const auto toolbars = findChildren<QToolBar *>();
    for (QToolBar *toolbar : toolbars) {
        auto *action = new QAction(this);
        action->setText(tr("Show %1").arg(toolbar->windowTitle()));
        action->setObjectName(QStringLiteral("toggleToolBar-") + toolbar->objectName());
        action->setCheckable(true);
        action->setChecked(!toolbar->isHidden());

        // toggle the panel if the checkbox was triggered
        connect(action, &QAction::triggered, this,
                [this, toolbar]() { toggleToolbarVisibility(toolbar->objectName()); });

        ui->menuToolbars->addAction(action);
    }
}

/**
 * Toggles the visibility of a panel by object name
 *
 * @param objectName
 */
void MainWindow::togglePanelVisibility(const QString &objectName) {
    auto *dockWidget = findChild<QDockWidget *>(objectName);

    if (dockWidget == nullptr) {
        return;
    }

    // to prevent crashes if updatePanelMenu removes all actions
    const QSignalBlocker blocker(dockWidget);
    Q_UNUSED(blocker)

    bool newVisibility = dockWidget->isHidden();

    // remember that the user wanted the note subfolder dock widget to be set
    // to visible or invisible
    if (objectName == QStringLiteral("noteSubFolderDockWidget")) {
        _noteSubFolderDockWidgetVisible = newVisibility;

        // don't allow the note subfolder dock widget to be visible if the
        // note folder has no subfolders activated or if the note tree feature is enabled
        if (newVisibility) {
            newVisibility =
                NoteFolder::isCurrentShowSubfolders() && !Utils::Misc::isEnableNoteTree();
        }
    }

    dockWidget->setVisible(newVisibility);

    // filter notes again according to new widget state
    filterNotes();

    // Ensure we have the latest note preview
    if (dockWidget == _notePreviewDockWidget && dockWidget->isVisible()) {
        refreshNotePreview(true);
    }

    if (objectName == QStringLiteral("noteGraphicsViewDockWidget")) {
        updateNoteGraphicsView();
    }

    // Store the workspace to persist the panel visibility change
    storeCurrentWorkspace();
}

/**
 * Toggles the visibility of a toolbar by object name
 *
 * @param objectName
 */
void MainWindow::toggleToolbarVisibility(const QString &objectName) {
    auto *toolbar = findChild<QToolBar *>(objectName);

    if (toolbar == nullptr) {
        return;
    }

    // to prevent crashes if updateToolbarMenu removes all actions
    const QSignalBlocker blocker(toolbar);
    Q_UNUSED(blocker)

    const bool newVisibility = toolbar->isHidden();
    toolbar->setVisible(newVisibility);

    // Store the workspace to persist the toolbar visibility change
    storeCurrentWorkspace();
}

/**
 * Restores the distraction free mode
 */
void MainWindow::restoreDistractionFreeMode() {
    _distractionFreeManager->restoreDistractionFreeMode();
}

/**
 * Checks if we are in distraction free mode
 */
bool MainWindow::isInDistractionFreeMode() {
    return DistractionFreeManager::isInDistractionFreeMode();
}

/**
 * Toggles the distraction free mode
 */
void MainWindow::toggleDistractionFreeMode() {
    _distractionFreeManager->toggleDistractionFreeMode();
}

/**
 * Does some basic styling
 */
void MainWindow::initStyling() {
    SettingsService settings;
    const bool darkMode = settings.value(QStringLiteral("darkMode")).toBool();
    QString appStyleSheet;
    QString noteTagFrameColorName;

    // turn on the dark mode if enabled
    if (darkMode) {
        QFile f(QStringLiteral(":qdarkstyle/style.qss"));
        if (!f.exists()) {
            qWarning("Unable to set stylesheet, file not found!");
        } else if (!f.open(QFile::ReadOnly | QFile::Text)) {
            qWarning() << "Failed to open file:" << f.fileName();
        } else {
            QTextStream ts(&f);
            appStyleSheet = ts.readAll();
            f.close();
        }

        // QTextEdit background color of qdarkstyle
        noteTagFrameColorName = QStringLiteral("#201F1F");
    } else {
        QPalette palette;
        const QColor &color = palette.color(QPalette::Base);
        noteTagFrameColorName = color.name();
    }

    // get the color name of the background color of the default text
    // highlighting item
    const QString fgColorName =
        Utils::Schema::schemaSettings
            ->getForegroundColor(MarkdownHighlighter::HighlighterState::NoState)
            .name();
    const QString bgColorName =
        Utils::Schema::schemaSettings
            ->getBackgroundColor(MarkdownHighlighter::HighlighterState::NoState)
            .name();

    // set the foreground and background color for the note text edits
    appStyleSheet += QStringLiteral("QMarkdownTextEdit{color:%1;background-color:%2;}")
                         .arg(fgColorName, bgColorName);

    // set the background color for the note tag frame and its children QFrames
    appStyleSheet += QStringLiteral(
                         "QFrame#noteTagFrame, QFrame#noteTagFrame QFrame "
                         "{background-color: %1;}")
                         .arg(noteTagFrameColorName);

    qApp->setStyleSheet(appStyleSheet);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QTimer::singleShot(1, this, [] {
#endif
        Utils::Gui::updateInterfaceFontSize();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    });
#endif

    if (!isInDistractionFreeMode()) {
        ui->noteTextEdit->setPaperMargins(0);
        ui->encryptedNoteTextEdit->setPaperMargins(0);
    }

#ifdef Q_OS_MAC
    // no stylesheets needed for OS X, the margins doesn't work the same there
    ui->tagFrame->setStyleSheet(QString());
    ui->notesListFrame->setStyleSheet(QString());
    ui->noteListSubFrame->setStyleSheet(QString());
    ui->navigationFrame->setStyleSheet(QString());
    ui->noteEditTabWidget->setStyleSheet(QString());
    ui->navigationTabWidget->setStyleSheet(QString());
    ui->noteViewFrame->setStyleSheet(QString());

    // add some margins in OS X to match the styling of the note list
    ui->navigationFrame->setContentsMargins(3, 0, 3, 0);

    // add a padding for the note tag frame so the `add tag` button doesn't
    // stick to the right corner
    ui->noteTagFrame->setStyleSheet("QFrame {border: none; padding-right: 5px;}");
#endif

    // Disconnect first to avoid duplicate connections when initStyling() is
    // called multiple times (e.g. during live dark mode preview in settings)
    disconnect(ui->noteTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
               SLOT(noteTextSliderValueChanged(int)));
    disconnect(ui->encryptedNoteTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
               SLOT(noteTextSliderValueChanged(int)));

    // move the note view scrollbar when the note edit scrollbar was moved
    connect(ui->noteTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
            SLOT(noteTextSliderValueChanged(int)));
    connect(ui->encryptedNoteTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
            SLOT(noteTextSliderValueChanged(int)));

    // move the note edit scrollbar when the note view scrollbar was moved
#ifdef USE_QLITEHTML
    disconnect(_notePreviewWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
               SLOT(noteViewSliderValueChanged(int)));
    connect(_notePreviewWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
            SLOT(noteViewSliderValueChanged(int)));
#else
    disconnect(ui->noteTextView->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
               SLOT(noteViewSliderValueChanged(int)));
    connect(ui->noteTextView->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
            SLOT(noteViewSliderValueChanged(int)));
#endif

    // hide the combo box if it looses focus if it should not be viewed
    disconnect(ui->noteFolderComboBox, &ComboBox::focusOut, this,
               &MainWindow::hideNoteFolderComboBoxIfNeeded);
    connect(ui->noteFolderComboBox, &ComboBox::focusOut, this,
            &MainWindow::hideNoteFolderComboBoxIfNeeded);

    Utils::Gui::fixDarkModeIcons(this);
}

void MainWindow::applyDarkModeSettings() {
    SettingsService settings;
    const bool systemIconTheme = settings.value(QStringLiteral("systemIconTheme")).toBool();

    if (!systemIconTheme) {
        const bool internalIconTheme = settings.value(QStringLiteral("internalIconTheme")).toBool();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
        if (!internalIconTheme && QIcon::themeName().isEmpty()) {
            QIcon::setThemeName(QIcon::fallbackThemeName());
        }
#endif

        const bool darkModeIconTheme = Utils::Misc::isDarkModeIconTheme();
        const bool useInternalIconTheme =
            internalIconTheme || QIcon::themeName().isEmpty() ||
            (QIcon::themeName() == QLatin1String("breeze-qownnotes")) ||
            (QIcon::themeName() == QLatin1String("breeze-dark-qownnotes"));

        if (darkModeIconTheme || useInternalIconTheme) {
            QIcon::setThemeName(darkModeIconTheme ? QStringLiteral("breeze-dark-qownnotes")
                                                  : QStringLiteral("breeze-qownnotes"));
        }
    }

    initStyling();
    setWindowIcon(getSystemTrayIcon());

    if (trayIcon != nullptr) {
        trayIcon->setIcon(getSystemTrayIcon());
    }

    readSettingsFromSettingsDialog();
    Q_EMIT settingsChanged();
#ifdef USE_QLITEHTML
    // Update the QLiteHtml viewport background color and search widget dark
    // mode to match the new theme before regenerating the note preview
    _notePreviewWidget->updateBackground();
#endif
    forceRegenerateNotePreview();
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
#ifdef USE_QLITEHTML
    QScrollBar *viewScrollBar = _notePreviewWidget->verticalScrollBar();
#else
    QScrollBar *viewScrollBar = ui->noteTextView->verticalScrollBar();
#endif

    const float editScrollFactor = static_cast<float>(value) / editScrollBar->maximum();
    const int viewPosition = static_cast<int>(viewScrollBar->maximum() * editScrollFactor);

    // set the scroll position in the note text view
    viewScrollBar->setSliderPosition(viewPosition);
}

/**
 * Moves the note edit scrollbar when the note view scrollbar was moved
 */
void MainWindow::noteViewSliderValueChanged(int value, bool force) {
    // don't react if note text view doesn't have the focus

#ifdef USE_QLITEHTML
    if (!_notePreviewWidget->hasFocus() && !force) {
#else
    if (!ui->noteTextView->hasFocus() && !force) {
#endif
        return;
    }

    QScrollBar *editScrollBar = activeNoteTextEdit()->verticalScrollBar();
#ifdef USE_QLITEHTML
    QScrollBar *viewScrollBar = _notePreviewWidget->verticalScrollBar();
#else
    QScrollBar *viewScrollBar = ui->noteTextView->verticalScrollBar();
#endif

    const float editScrollFactor = static_cast<float>(value) / viewScrollBar->maximum();

    const int editPosition = static_cast<int>(editScrollBar->maximum() * editScrollFactor);

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
void MainWindow::setDistractionFreeMode(const bool enabled) {
    _distractionFreeManager->setDistractionFreeMode(enabled);
}

/**
 * Sets the distraction free mode if it is currently other than we want it to be
 */
void MainWindow::changeDistractionFreeMode(const bool enabled) {
    _distractionFreeManager->changeDistractionFreeMode(enabled);
}

/**
 * Shows a status bar message if not in distraction free mode
 */
void MainWindow::showStatusBarMessage(const QString &message, const QString &symbol, int timeout) {
    // Add a symbol to the message
    const QString text = symbol.isEmpty() ? message : QStringLiteral("%1 %2").arg(symbol, message);

    // If timeout is smaller than 100, it is in seconds
    if (timeout < 100) {
        timeout = timeout * 1000;
    }

    if (!isInDistractionFreeMode()) {
        ui->statusBar->showMessage(text, timeout);
    }

    // Write to the log widget
    emit log(LogWidget::StatusLogType, text);
}

void MainWindow::showStatusBarMessage(const QString &message, int timeout) {
    showStatusBarMessage(message, QString(), timeout);
}

/**
 * Sets the shortcuts for the note bookmarks up
 */
void MainWindow::setupNoteBookmarkShortcuts() {
    for (int number = 1; number <= 9; number++) {
        // setup the store shortcut
        auto *storeShortcut = new QShortcut(
            QKeySequence(QStringLiteral("Ctrl+Shift+") + QString::number(number)), this);

        connect(storeShortcut, &QShortcut::activated, this,
                [this, number]() { storeNoteBookmark(number); });

        // setup the goto shortcut
        auto *gotoShortcut =
            new QShortcut(QKeySequence(QStringLiteral("Ctrl+") + QString::number(number)), this);

        connect(gotoShortcut, &QShortcut::activated, this,
                [this, number]() { gotoNoteBookmark(number); });
    }
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
    const QList<QAction *> actions = ui->noteFoldersMenu->findChildren<QAction *>();

    // loop through all actions of the recent note folders menu and hide them
    // this is a workaround because the app might crash if trackAction() is
    // called, because the action was triggered and then removed
    int c = 0;
    for (QAction *action : actions) {
        // start with the 2nd item, the first item is the menu itself
        if (c++ > 0) {
            // hide menu item
            action->setVisible(false);
        }
    }

    const auto noteFolders = NoteFolder::fetchAll();
    const auto noteFoldersCount = noteFolders.count();

    const QSignalBlocker blocker(ui->noteFolderComboBox);
    Q_UNUSED(blocker)

    ui->noteFolderComboBox->clear();
    int index = 0;
    int noteFolderComboBoxIndex = 0;

    // clear all note folder buttons
    if (_useNoteFolderButtons) {
        QLayoutItem *child;

        while ((child = _noteFolderDockWidgetFrame->layout()->takeAt(0)) != nullptr) {
            _noteFolderDockWidgetFrame->layout()->removeWidget(child->widget());
            delete child->widget();
            delete child;
        }
    }

    // populate the note folder list
    if (noteFoldersCount > 0) {
        QFont font;

        for (const NoteFolder &noteFolder : noteFolders) {
            // don't show not existing folders or if path is empty
            if (!noteFolder.localPathExists()) {
                continue;
            }

            const int folderId = noteFolder.getId();
            const QString name = noteFolder.getName();
            const bool isCurrentNoteFolder = noteFolder.isCurrent();

            // add an entry to the combo box
            ui->noteFolderComboBox->addItem(name, folderId);

            // add a menu entry
            QAction *action = ui->noteFoldersMenu->addAction(name);
            action->setData(folderId);
            action->setToolTip(noteFolder.getLocalPath());
            action->setStatusTip(noteFolder.getLocalPath());
            action->setObjectName(QStringLiteral("noteFolder-") + QString::number(folderId));
            action->setIcon(Utils::Gui::folderIcon());

            if (isCurrentNoteFolder) {
                font = action->font();
                font.setBold(true);
                action->setFont(font);

                noteFolderComboBoxIndex = index;
            }

            connect(action, &QAction::triggered, this,
                    [this, folderId]() { changeNoteFolder(folderId); });

            // add note folder button if enabled
            if (_useNoteFolderButtons) {
                auto *button = new QPushButton(name);
                button->setToolTip(tr("Switch to note folder"));

                if (isCurrentNoteFolder) {
                    button->setFont(font);
                }

                _noteFolderDockWidgetFrame->layout()->addWidget(button);

                connect(button, &QPushButton::pressed, this,
                        [this, folderId]() { changeNoteFolder(folderId); });
            }

            ++index;
        }

        // set the current row
        ui->noteFolderComboBox->setCurrentIndex(noteFolderComboBoxIndex);
    }
}

/*
 * Set a new note folder
 */
bool MainWindow::changeNoteFolder(const int noteFolderId, const bool forceChange) {
    const int currentNoteFolderId = NoteFolder::currentNoteFolderId();

    // store the current position in the note of the current note folder
    _activeNoteFolderNotePositions[currentNoteFolderId] =
        NoteHistoryItem(&currentNote, ui->noteTextEdit);

    // store the note history of the old note folder
    noteHistory.storeForCurrentNoteFolder();

    Utils::Gui::storeNoteTabs(ui->noteEditTabWidget);

    // recheck for broken tag note links
    resetBrokenTagNotesLinkFlag();

    const NoteFolder noteFolder = NoteFolder::fetch(noteFolderId);
    if (!noteFolder.isFetched()) {
        return false;
    }

    if (noteFolder.isCurrent() && !forceChange) {
        return false;
    }

    QString folderName = noteFolder.getLocalPath();
    const QString oldPath = this->notesPath;

    // Switch the active note folder first so all subsequent refresh steps
    // (including combo/menu rebuilding) use the correct current folder.
    noteFolder.setAsCurrent();

    const bool notesPathChanged = (oldPath != folderName);

    // reload notes if notes folder was changed
    if (notesPathChanged) {
        const QSignalBlocker blocker2(this->ui->searchLineEdit);
        {
            Q_UNUSED(blocker2)
            ui->searchLineEdit->clear();
        }

        // store everything before changing folder
        storeUpdatedNotesToDisk();

        // commit the changes in the current note folder to git
        gitCommitCurrentNoteFolder();

        // update the recent note folder list
        storeRecentNoteFolder(this->notesPath, folderName);

        // change notes path
        this->notesPath = folderName;

        // store notesPath setting
        SettingsService settings;
        // make the path relative to the portable data path if we are in
        // portable mode
        settings.setValue(
            QStringLiteral("notesPath"),
            Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(std::move(folderName)));

        // we have to unset the current note otherwise it might show up after
        // switching to another note folder
        unsetCurrentNote();
    } else {
        // Keep selector and Note -> Note folders menu in sync when switching
        // between folders that share the same path.
        loadNoteFolderListMenu();
    }

    buildNotesIndexAndLoadNoteDirectoryList(false, !notesPathChanged, false);

    // update the current folder tooltip
    updateCurrentFolderTooltip();

    // restore the note tabs
    Utils::Gui::restoreNoteTabs(ui->noteEditTabWidget, ui->noteEditTabWidgetLayout);

    // restore the note history of the new note folder
    noteHistory.restoreForCurrentNoteFolder();

    // check if there is a note name set and jump to it
    QString noteName = _activeNoteFolderNotePositions[noteFolderId].getNoteName();

    // if there was no NoteHistory found try the note history
    if (noteName.isEmpty()) {
        _activeNoteFolderNotePositions[noteFolderId] = noteHistory.getCurrentHistoryItem();
        noteName = _activeNoteFolderNotePositions[noteFolderId].getNoteName();
    }

    if (!noteName.isEmpty()) {
        jumpToNoteName(noteName);

        // restore the current position in the note
        _activeNoteFolderNotePositions[noteFolderId].restoreTextEditPosition(ui->noteTextEdit);
    }

    if (notesPathChanged) {
        // commit the changes in the selected note folder to git
        gitCommitCurrentNoteFolder();
    }

    generateSystemTrayContextMenu();
    updateWindowTitle();
    _noteTabManager->_lastNoteId = 0;

    // Update the ignored click url regexps for the note text edits
    ui->encryptedNoteTextEdit->updateIgnoredClickUrlRegexps();
    ui->noteTextEdit->updateIgnoredClickUrlRegexps();

    return true;
}

/*
 * Adds and removes a folder from the recent note folders
 */
void MainWindow::storeRecentNoteFolder(const QString &addFolderName,
                                       const QString &removeFolderName) {
    SettingsService settings;
    QStringList recentNoteFolders =
        settings.value(QStringLiteral("recentNoteFolders")).toStringList();

    recentNoteFolders.removeAll(addFolderName);
    recentNoteFolders.removeAll(removeFolderName);

    // remove empty paths
    recentNoteFolders.removeAll(QLatin1String(""));

    if (addFolderName != removeFolderName) {
        recentNoteFolders.prepend(addFolderName);
    }

    settings.setValue(QStringLiteral("recentNoteFolders"), recentNoteFolders);
    // reload menu
    loadNoteFolderListMenu();
}

int MainWindow::openNoteDiffDialog(Note changedNote) {
    if (this->noteDiffDialog->isVisible()) {
        this->noteDiffDialog->close();
    }

    qDebug() << __func__ << " - 'changedNote': " << changedNote;

    SettingsService settings;

    // check if we should ignore all changes
    if (settings.value(QStringLiteral("ignoreAllExternalModifications")).toBool()) {
        return NoteDiffDialog::Ignore;
    }

    // check if we should accept all changes
    if (settings.value(QStringLiteral("acceptAllExternalModifications")).toBool()) {
        return NoteDiffDialog::Reload;
    }

    const QString text1 = this->ui->noteTextEdit->toPlainText();

    changedNote.updateNoteTextFromDisk();
    const QString text2 = changedNote.getNoteText();

    //    qDebug() << __func__ << " - 'text1': " << text1;
    //    qDebug() << __func__ << " - 'text2': " << text2;

    auto *diff = new diff_match_patch();
    const QList<Diff> diffList = diff->diff_main(text1, text2);

    const QString html = diff->diff_prettyHtml(diffList);
    //    qDebug() << __func__ << " - 'html': " << html;

    this->noteDiffDialog = new NoteDiffDialog(this, html);
    this->noteDiffDialog->exec();

    int result = this->noteDiffDialog->resultActionRole();
    return result;
}

void MainWindow::createSystemTrayIcon() { _systemTrayManager->createSystemTrayIcon(); }

/**
 * Returns a proper system tray icon
 *
 * @return
 */
QIcon MainWindow::getSystemTrayIcon() { return SystemTrayManager::getSystemTrayIcon(); }

/**
 * Creates the items in the note tree widget from the note and note sub
 * folder tables
 */
void MainWindow::loadNoteDirectoryList() { _noteIndexManager->loadNoteDirectoryList(); }

/**
 * Adds a note to the note tree widget
 */
bool MainWindow::addNoteToNoteTreeWidget(const Note &note, QTreeWidgetItem *parent) {
    return _noteTreeManager->addNoteToNoteTreeWidget(note, parent);
}

void MainWindow::updateNoteTreeWidgetItem(const Note &note, QTreeWidgetItem *noteItem) {
    _noteTreeManager->updateNoteTreeWidgetItem(note, noteItem);
}

/**
 * @brief makes the current note the first item in the note list without
 * reloading the whole list
 */
void MainWindow::makeCurrentNoteFirstInNoteList() {
    _noteTreeManager->makeCurrentNoteFirstInNoteList();
}

/**
 * Finds a note in the note tree widget and returns its item
 *
 * @param note
 * @return
 */
QTreeWidgetItem *MainWindow::findNoteInNoteTreeWidget(const Note &note) {
    return _noteTreeManager->findNoteInNoteTreeWidget(note);
}

void MainWindow::readSettings() {
    NoteFolder::migrateToNoteFolders();

    SettingsService settings;
    showSystemTray = settings.value(QStringLiteral("ShowSystemTray"), false).toBool();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QTimer::singleShot(1, this, [this] {
#endif
        restoreGeometry(
            SettingsService().value(QStringLiteral("MainWindow/geometry")).toByteArray());
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    });
#endif

    ui->menuBar->restoreGeometry(
        settings.value(QStringLiteral("MainWindow/menuBarGeometry")).toByteArray());

    // read all relevant settings, that can be set in the settings dialog
    readSettingsFromSettingsDialog(true);

    // get the notes path
    // prepend the portable data path if we are in portable mode
    this->notesPath = Utils::Misc::prependPortableDataPathIfNeeded(
        settings.value(QStringLiteral("notesPath")).toString());

    qDebug() << __func__ << " - 'this->notesPath': " << this->notesPath;

    // migration: remove GAnalytics-cid
    if (!settings.value(QStringLiteral("GAnalytics-cid")).toString().isEmpty()) {
        settings.remove(QStringLiteral("GAnalytics-cid"));
    }

    // let us select a folder if we haven't found one in the settings
    if (this->notesPath.isEmpty()) {
        selectOwnCloudNotesFolder();
    }

    // migration: remove notes path from recent note folders
    if (!this->notesPath.isEmpty()) {
        QStringList recentNoteFolders =
            settings.value(QStringLiteral("recentNoteFolders")).toStringList();
        if (recentNoteFolders.contains(this->notesPath)) {
            recentNoteFolders.removeAll(this->notesPath);
            settings.setValue(QStringLiteral("recentNoteFolders"), recentNoteFolders);
        }
    }

    // set the editor width selector for the distraction free mode
    const int editorWidthMode =
        settings.value(QStringLiteral("DistractionFreeMode/editorWidthMode")).toInt();

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
        case QOwnNotesMarkdownTextEdit::Custom:
            ui->actionEditorWidthCustom->setChecked(true);
            break;
        default:
        case QOwnNotesMarkdownTextEdit::Narrow:
            ui->actionEditorWidthNarrow->setChecked(true);
            break;
    }

    // toggle the show status bar checkbox
    const bool showStatusBar = settings.value(QStringLiteral("showStatusBar"), true).toBool();
    on_actionShow_status_bar_triggered(showStatusBar);

    // toggle the show menu bar checkbox
    // use the current menu bar visibility as default (so it will not be
    // shown by default on Unity desktop)
    const bool showMenuBar =
        settings.value(QStringLiteral("showMenuBar"), !ui->menuBar->isHidden()).toBool();
    on_actionShow_menu_bar_triggered(showMenuBar);

    {
        const QSignalBlocker blocker(ui->actionAllow_note_editing);
        Q_UNUSED(blocker)

        bool isAllowNoteEditing = Utils::Misc::isNoteEditingAllowed();
        ui->actionAllow_note_editing->setChecked(isAllowNoteEditing);
        // we want to trigger the method regardless if the button was toggled
        // or not
        on_actionAllow_note_editing_triggered(isAllowNoteEditing);
    }

    // we want to trigger the event afterward so the settings of the note edits
    // are updated
    const bool centerCursor = settings.value(QStringLiteral("Editor/centerCursor")).toBool();
    ui->actionTypewriter_mode->setChecked(centerCursor);

    // restore old spell check settings
    ui->actionCheck_spelling->setChecked(
        settings.value(QStringLiteral("checkSpelling"), true).toBool());

    // load backends
#ifdef ASPELL_ENABLED
    loadSpellingBackends();
#else
    ui->menuSpelling_backend->menuAction()->setVisible(false);
#endif

    // load language dicts names into menu
    // Delay loading, loading dictionary names is slow
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QTimer::singleShot(10, this, [this] {
#endif
        loadDictionaryNames();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    });
#endif

    // Enable/disable auto read-only mode
    if (!startAutoReadOnlyModeIfEnabled()) {
        _autoReadOnlyModeTimer->stop();
    }

    loadNoteBookmarks();
    buildAiToolbarAndActions();
}

bool MainWindow::startAutoReadOnlyModeIfEnabled() {
    SettingsService settings;

    if (settings.value(QStringLiteral("autoReadOnlyMode")).toBool()) {
        _autoReadOnlyModeTimer->start(
            settings.value(QStringLiteral("autoReadOnlyModeTimeout")).toInt() * 1000);

        return true;
    }

    return false;
}

/**
 * Restores the toolbars
 */
void MainWindow::restoreToolbars() {
    SettingsService settings;
    QList<ToolbarContainer> toolbarContainers;
    const int toolbarCount = settings.beginReadArray(QStringLiteral("toolbar"));

    for (int i = 0; i < toolbarCount; i++) {
        settings.setArrayIndex(i);

        ToolbarContainer toolbarContainer;

        toolbarContainer.name = settings.value(QStringLiteral("name")).toString();
        if (toolbarContainer.name.isEmpty()) {
            qWarning() << tr("Toolbar could not be loaded without name");
            continue;
        }

        toolbarContainer.title = settings.value(QStringLiteral("title")).toString();
        toolbarContainer.actions = settings.value(QStringLiteral("items")).toStringList();

        toolbarContainers.push_back(toolbarContainer);
    }

    settings.endArray();

    if (!toolbarContainers.empty()) {
        // delete the custom toolbars
        const auto toolbars = findChildren<QToolBar *>();
        for (QToolBar *toolbar : toolbars) {
            if (!toolbar->objectName().startsWith(Toolbar_Editor::customToolbarNamePrefix)) {
                continue;
            }

            delete toolbar;
        }

        for (ToolbarContainer toolbarContainer : Utils::asConst(toolbarContainers)) {
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

    // initialize web app websocket connection
    QTimer::singleShot(250, this, SLOT(initWebAppClientService()));
}

/**
 * @brief Reads all relevant settings, that can be set in the settings dialog
 */
void MainWindow::readSettingsFromSettingsDialog(const bool isAppLaunch) {
    SettingsService settings;
    const bool noteEditIsCentralWidget =
        settings.value(QStringLiteral("noteEditIsCentralWidget"), true).toBool();

    if (_noteEditIsCentralWidget != noteEditIsCentralWidget) {
        setNoteEditCentralWidgetEnabled(noteEditIsCentralWidget);
        storeCurrentWorkspace();
    }

    this->notifyAllExternalModifications =
        settings.value(QStringLiteral("notifyAllExternalModifications")).toBool();
    this->noteSaveIntervalTime = settings.value(QStringLiteral("noteSaveIntervalTime"), 10).toInt();

    // default value is 10 seconds
    if (this->noteSaveIntervalTime == 0) {
        this->noteSaveIntervalTime = 10;
        settings.setValue(QStringLiteral("noteSaveIntervalTime"), this->noteSaveIntervalTime);
    }

    // set the git commit interval time
    _gitCommitInterval = settings.value(QStringLiteral("gitCommitInterval"), 30).toInt();

    // load note text view font
    QString fontString = Utils::Misc::previewFontString();

    // store the current font if there isn't any set yet
    if (fontString.isEmpty()) {
#ifdef USE_QLITEHTML
        fontString = _notePreviewWidget->defaultFont().toString();
#else
        fontString = ui->noteTextView->font().toString();
#endif
        settings.setValue(QStringLiteral("MainWindow/noteTextView.font"), fontString);
    }

    // set the note text view font
    QFont font;
    font.fromString(fontString);
#ifdef USE_QLITEHTML
    _notePreviewWidget->setDefaultFont(font);
#else
    ui->noteTextView->setFont(font);
#endif

    // set the main toolbar icon size
    int toolBarIconSize = settings.value(QStringLiteral("MainWindow/mainToolBar.iconSize")).toInt();
    if (toolBarIconSize == 0) {
        toolBarIconSize = ui->mainToolBar->iconSize().height();
        settings.setValue(QStringLiteral("MainWindow/mainToolBar.iconSize"), toolBarIconSize);
    } else {
        QSize size(toolBarIconSize, toolBarIconSize);
        ui->mainToolBar->setIconSize(size);
        _formattingToolbar->setIconSize(size);
        _customActionToolbar->setIconSize(size);
        _insertingToolbar->setIconSize(size);
        _encryptionToolbar->setIconSize(size);
        _aiToolbarManager->aiToolbar()->setIconSize(size);
        _windowToolbar->setIconSize(size);
        _quitToolbar->setIconSize(size);
    }

    // change the search notes symbol between dark and light mode
    QString fileName = settings.value(QStringLiteral("darkModeColors")).toBool()
                           ? QStringLiteral("search-notes-dark.svg")
                           : QStringLiteral("search-notes.svg");
    QString styleSheet = ui->searchLineEdit->styleSheet();
    static const QRegularExpression re(QStringLiteral("background-image: url\\(:.+\\);"));
    styleSheet.replace(re, QStringLiteral("background-image: url(:/images/%1);").arg(fileName));
    ui->noteSubFolderLineEdit->setStyleSheet(styleSheet);
    ui->tagLineEdit->setStyleSheet(styleSheet);
    ui->searchLineEdit->setStyleSheet(styleSheet);
    ui->navigationLineEdit->setStyleSheet(styleSheet);

    // initialize the shortcuts for the actions
    initShortcuts();

    // initialize the item height of the tree widgets
    initTreeWidgetItemHeight();

    // we need to initialize the toolbar menu again in case there are new
    // toolbars
    initToolbarMenu();

    // update the toolbar menu
    updateToolbarMenu();

    // init the saved searches completer
    initSavedSearchesCompleter();

    // show or hide the note git version menu entry
#ifdef USE_LIBGIT2
    ui->actionShow_note_git_versions->setVisible(true);
    ui->actionShow_note_git_versions_external->setVisible(Utils::Git::hasLogCommand());
#else
    ui->actionShow_note_git_versions->setVisible(Utils::Git::hasLogCommand());
    ui->actionShow_note_git_versions_external->setVisible(false);
#endif

    // show or hide 'Find or create ...' search in Note Subfolders & Tags Panels
    ui->noteSubFolderLineEdit->setHidden(
        settings.value(QStringLiteral("noteSubfoldersPanelHideSearch")).toBool());
    ui->tagLineEdit->setHidden(settings.value(QStringLiteral("tagsPanelHideSearch")).toBool());
    ui->navigationLineEdit->setHidden(
        settings.value(QStringLiteral("navigationPanelHideSearch")).toBool());

    // set the cursor width of the note text-edits
    int cursorWidth = settings.value(QStringLiteral("cursorWidth"), 1).toInt();
    ui->noteTextEdit->setCursorWidth(cursorWidth);
    ui->encryptedNoteTextEdit->setCursorWidth(cursorWidth);

    // turn line numbers on if enabled
    bool showLineNumbersInEditor =
        settings.value(QStringLiteral("Editor/showLineNumbers")).toBool();
    ui->noteTextEdit->setLineNumberEnabled(showLineNumbersInEditor);
    ui->encryptedNoteTextEdit->setLineNumberEnabled(showLineNumbersInEditor);
    const bool headingFoldingEnabled =
        settings.value(QStringLiteral("Editor/headingFolding"), false).toBool();
    ui->actionFold_all_headings->setVisible(headingFoldingEnabled);
    ui->actionUnfold_all_headings->setVisible(headingFoldingEnabled);

    if (showLineNumbersInEditor) {
        bool darkMode = settings.value(QStringLiteral("darkMode")).toBool();
        ui->noteTextEdit->setLineNumbersCurrentLineColor(
            QColor(darkMode ? QStringLiteral("#eef067") : QStringLiteral("##141414")));
    }

    ui->noteTextEdit->setPaperMargins();
    ui->encryptedNoteTextEdit->setPaperMargins();
    _noteFilePathLabel->setVisible(
        settings.value(QStringLiteral("showStatusBarNotePath"), true).toBool());
    _noteFilePathLabel->updateText();

    if (_webSocketServerService == nullptr) {
        QTimer::singleShot(250, this, SLOT(initWebSocketServerService()));
    } else {
        _webSocketServerService->refreshServers();
    }

    if (settings.value(QStringLiteral("Editor/disableCursorBlinking")).toBool()) {
        qApp->setCursorFlashTime(0);
    }

    // reset cloud service instance
    OwnCloudService::instance(true);

    if (!isAppLaunch) {
        // the notes need to be reloaded and subfolder panel needs to be populated
        // if subfolders were activated for a note folder in the settings
        if (NoteFolder::isCurrentShowSubfolders()) {
            buildNotesIndexAndLoadNoteDirectoryList();
        }

        // Read AI API key, in case the keys changed
        OpenAiService::instance()->setApiKeyForCurrentBackend();
    }

    initGlobalKeyboardShortcuts();

    const bool isWebAppSupportEnabled = Utils::Misc::isWebAppSupportEnabled();
    ui->actionSend_clipboard->setEnabled(isWebAppSupportEnabled);
    ui->actionSend_clipboard_as_text->setEnabled(isWebAppSupportEnabled);
}

/**
 * Initializes the item height of the tree widgets
 */
void MainWindow::initTreeWidgetItemHeight() {
    SettingsService settings;
    int height = settings.value(QStringLiteral("itemHeight")).toInt();

    // if the height was 0 set it the current height of a tree widget item
    if (height == 0) {
        QTreeWidget treeWidget(this);
        auto *treeWidgetItem = new QTreeWidgetItem();
        treeWidget.addTopLevelItem(treeWidgetItem);
        height = treeWidget.visualItemRect(treeWidgetItem).height();
        settings.setValue(QStringLiteral("itemHeight"), height);
    }

    updateTreeWidgetItemHeight(ui->tagTreeWidget, height);
    updateTreeWidgetItemHeight(ui->noteTreeWidget, height);
    updateTreeWidgetItemHeight(ui->noteSubFolderTreeWidget, height);
    updateTreeWidgetItemHeight(ui->navigationWidget, height);
    updateTreeWidgetItemHeight(ui->backlinkWidget, height);
}

/**
 * Sets height of the items of a tree widget
 *
 * @param treeWidget
 * @param height
 */
void MainWindow::updateTreeWidgetItemHeight(QTreeWidget *treeWidget, int height) {
    QString styleText = treeWidget->styleSheet();

    // remove the old height stylesheet
    static const QRegularExpression re(QStringLiteral("\nQTreeWidget::item \\{height: \\d+px\\}"),
                                       QRegularExpression::CaseInsensitiveOption);
    styleText.remove(re);

    // add the new height stylesheet
    styleText += QStringLiteral("\nQTreeWidget::item {height: %1px}").arg(QString::number(height));

    treeWidget->setStyleSheet(styleText);
}

void MainWindow::updateNoteTextFromDisk(Note note) {
    note.updateNoteTextFromDisk();
    note.store();
    this->currentNote = note;
    updateNoteEncryptionUI();

    {
        const QSignalBlocker blocker(this->ui->noteTextEdit);
        Q_UNUSED(blocker)
        this->setNoteTextFromNote(&note, false, false, true);
    }

    ScriptingService::instance()->onCurrentNoteChanged(&currentNote);
}

void MainWindow::notesWereModified(const QString &str) {
    _noteIndexManager->notesWereModified(str);
}

void MainWindow::notesDirectoryWasModified(const QString &str) {
    _noteIndexManager->notesDirectoryWasModified(str);
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
    _noteViewUpdateTimer->start(2000);
}

void MainWindow::autoReadOnlyModeTimerSlot() {
    // Disable note editing if it was enabled
    if (ui->actionAllow_note_editing->isChecked()) {
        ui->actionAllow_note_editing->trigger();
    }

    // Start timer again
    startAutoReadOnlyModeIfEnabled();
}

void MainWindow::storeUpdatedNotesToDisk() { _noteIndexManager->storeUpdatedNotesToDisk(); }

/**
 * Shows alerts for calendar items with an alarm date in the current minute
 * Also checks for expired note crypto keys
 */
void MainWindow::frequentPeriodicChecker() { _noteIndexManager->frequentPeriodicChecker(); }

/**
 * Does the setup the status bar widgets
 */
void MainWindow::setupStatusBarWidgets() {
    /*
     * setup of file path label
     */
    _noteFilePathLabel = new NoteFilePathLabel(this);
    ui->statusBar->addWidget(_noteFilePathLabel);

    initializeOpenAiActivitySpinner();
    ui->statusBar->addPermanentWidget(_aiToolbarManager->openAiActivitySpinner());

    /*
     * setup of readonly button
     */
    _readOnlyButton->setText(tr("Read-only"));
    _readOnlyButton->setToolTip(
        tr("Note editing is disabled, click to "
           "enable"));
    _readOnlyButton->setStyleSheet(QStringLiteral("QPushButton {padding: 0 5px}"));
    _readOnlyButton->setFlat(true);
    _readOnlyButton->setHidden(Utils::Misc::isNoteEditingAllowed());

    connect(_readOnlyButton, &QPushButton::pressed, this, &MainWindow::allowNoteEditing);

    ui->statusBar->addPermanentWidget(_readOnlyButton);

    /*
     * setup of line number label
     */
    _noteEditLineNumberLabel = new QLabel(this);
    _noteEditLineNumberLabel->setText(QStringLiteral("0:0"));
    _noteEditLineNumberLabel->setToolTip(tr("Line numbers"));

    ui->statusBar->addPermanentWidget(_noteEditLineNumberLabel);

    /*
     * setup of update available button
     */
    _updateAvailableButton = new QPushButton(this);
    _updateAvailableButton->setFlat(true);
    _updateAvailableButton->setToolTip(
        tr("Click here to see what has changed and to be able to "
           "update to the latest version"));
    _updateAvailableButton->hide();
    _updateAvailableButton->setStyleSheet(QStringLiteral("QPushButton {padding: 0 5px}"));

    connect(_updateAvailableButton, &QPushButton::pressed, this,
            &MainWindow::on_actionCheck_for_updates_triggered);

    ui->statusBar->addPermanentWidget(_updateAvailableButton);
}

void MainWindow::initializeOpenAiActivitySpinner() {
    _aiToolbarManager->initializeOpenAiActivitySpinner();
}

void MainWindow::showUpdateAvailableButton(const QString &version) {
    _updateAvailableButton->setText(tr("new version %1 available").arg(version));
    _updateAvailableButton->show();
}

void MainWindow::hideUpdateAvailableButton() { _updateAvailableButton->hide(); }

/**
 * Builds the index of notes and note sub folders
 */
bool MainWindow::buildNotesIndex(int noteSubFolderId, bool forceRebuild) {
    return _noteIndexManager->buildNotesIndex(noteSubFolderId, forceRebuild);
}

/**
 * Asks to remove conflicted copies of the notes.sqlite database
 */
void MainWindow::removeConflictedNotesDatabaseCopies() {
    _noteIndexManager->removeConflictedNotesDatabaseCopies();
}

bool MainWindow::noteDirectoryWatcherAddPath(const QString &path) {
    return _noteIndexManager->noteDirectoryWatcherAddPath(path);
}

void MainWindow::addDirectoryToDirectoryWatcher(const QString &path) {
    _noteIndexManager->addDirectoryToDirectoryWatcher(path);
}

/**
 * Updates the note directory watcher
 */
void MainWindow::updateNoteDirectoryWatcher() { _noteIndexManager->updateNoteDirectoryWatcher(); }

/**
 * Clears all paths from the directory watcher
 */
void MainWindow::clearNoteDirectoryWatcher() { _noteIndexManager->clearNoteDirectoryWatcher(); }

/**
 * Jumps to the welcome note in the note selector
 */
void MainWindow::jumpToWelcomeNote() { jumpToNoteName(QStringLiteral("Welcome to QOwnNotes")); }

/**
 * Jumps to a note in the note selector
 */
bool MainWindow::jumpToNoteName(const QString &name) {
    // search for the note
    QList<QTreeWidgetItem *> items =
        ui->noteTreeWidget->findItems(name, Qt::MatchExactly | Qt::MatchRecursive, 0);

    if (items.count() > 0) {
        ui->noteTreeWidget->setCurrentItem(items.at(0));
        return true;
    }

    return false;
}

/**
 * Jumps to a note in the note selector by NoteHistoryItem
 */
bool MainWindow::jumpToNoteHistoryItem(const NoteHistoryItem &historyItem) {
    // search for the note
    const QList<QTreeWidgetItem *> items = ui->noteTreeWidget->findItems(
        historyItem.getNoteName(), Qt::MatchExactly | Qt::MatchRecursive, 0);
    const bool isCurrentNoteTreeEnabled = NoteFolder::isCurrentNoteTreeEnabled();

    for (QTreeWidgetItem *item : items) {
        if (isCurrentNoteTreeEnabled) {
            QString pathData = historyItem.getNoteSubFolderPathData();
            auto noteSubFolder = NoteSubFolder::fetchByPathData(std::move(pathData));
            int parentId = item->parent()->data(0, Qt::UserRole).toInt();

            if (parentId != noteSubFolder.getId()) {
                continue;
            }
        }

        ui->noteTreeWidget->setCurrentItem(item);
        return true;
    }

    return false;
}

/**
 * Jumps to a note subfolder in the note subfolder tree
 *
 * @param noteSubFolderId
 * @return
 */
bool MainWindow::jumpToNoteSubFolder(int noteSubFolderId) {
    QTreeWidgetItem *item =
        Utils::Gui::getTreeWidgetItemWithUserData(ui->noteSubFolderTreeWidget, noteSubFolderId);

    if (item != nullptr) {
        // If the selection isn't cleared then the old subfolder is still selected too
        ui->noteSubFolderTreeWidget->clearSelection();
        ui->noteSubFolderTreeWidget->setCurrentItem(item);

        return true;
    }

    return false;
}

void MainWindow::selectNavigationItemAtPosition(int position) {
    _navigationManager->selectNavigationItemAtPosition(position);
}

void MainWindow::setOptionalNavigationTabVisible(QWidget *tab, const QString &title,
                                                 int preferredIndex, bool visible) {
    _navigationManager->setOptionalNavigationTabVisible(tab, title, preferredIndex, visible);
}

void MainWindow::updateFileNavigationTab() { _navigationManager->updateFileNavigationTab(); }

void MainWindow::updateBacklinkNavigationTab() {
    _navigationManager->updateBacklinkNavigationTab();
}

QString MainWindow::selectOwnCloudNotesFolder() {
    QString path = this->notesPath;

    if (path.isEmpty()) {
        path = Utils::Misc::defaultNotesPath();
    }

    // TODO(pbek): We sometimes seem to get a "QCoreApplication::postEvent:
    // Unexpected null receiver" here.
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Please select the folder where your notes will get stored"), path,
        QFileDialog::ShowDirsOnly);

    QDir d = QDir(dir);

    if (d.exists() && (!dir.isEmpty())) {
        // let's remove trailing slashes
        dir = d.path();

        // update the recent note folder list
        storeRecentNoteFolder(this->notesPath, dir);

        this->notesPath = dir;
        SettingsService settings;
        // make the path relative to the portable data path if we are in
        // portable mode
        settings.setValue(QStringLiteral("notesPath"),
                          Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(std::move(dir)));

        // update the current folder tooltip
        updateCurrentFolderTooltip();
    } else {
        if (this->notesPath.isEmpty()) {
            if (QMessageBox::question(
                    this, tr("No folder was selected"),
                    Utils::Misc::replaceOwnCloudText(tr("You have to select your ownCloud notes "
                                                        "folder to make this software work!")),
                    QMessageBox::Retry | QMessageBox::Close,
                    QMessageBox::Retry) == QMessageBox::Retry) {
                selectOwnCloudNotesFolder();
            } else {
                // No other way to quit the application worked
                // in the constructor
                // Waqar144: this doesn't seem very wise...
                QTimer::singleShot(0, this, SLOT(quitApp()));
                QTimer::singleShot(100, this, SLOT(quitApp()));
            }
        }
    }

    return this->notesPath;
}

/**
 * Sets the current note from a note id
 */
void MainWindow::setCurrentNoteFromNoteId(const int noteId) {
    // make sure the main window is visible
    show();

    Note note = Note::fetch(noteId);
    if (note.isFetched()) {
        setCurrentNote(std::move(note));
    }
}

/**
 * Reloads the current note by id
 * This is useful when the path or filename of the current note changed
 */
void MainWindow::reloadCurrentNoteByNoteId(bool updateNoteText) {
    // get current cursor position
    auto cursor = activeNoteTextEdit()->textCursor();
    const int pos = cursor.position();

    // update the current note
    currentNote = Note::fetch(currentNote.getId());
    setCurrentNote(std::move(currentNote), updateNoteText);

    // restore old cursor position
    cursor.setPosition(pos);
    activeNoteTextEdit()->setTextCursor(cursor);
}

void MainWindow::setCurrentNote(Note note, bool updateNoteText, bool updateSelectedNote,
                                bool addNoteToHistory) {
    qDebug() << __func__ << " - 'note': " << note << " - 'updateNoteText': " << updateNoteText
             << " - 'updateSelectedNote': " << updateSelectedNote;

    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("note/current-note/changed"));

    // if note subfolder was different than the current we will
    // switch to that note subfolder
    // TODO: don't switch if "All notes" was selected!
    if (!_showNotesFromAllNoteSubFolders && !note.isInCurrentNoteSubFolder() &&
        !NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively()) {
        qDebug() << "Switching note subfolder";
        jumpToNoteSubFolder(note.getNoteSubFolderId());
    }

    enableShowVersionsButton();
    enableShowTrashButton();

    // update cursor position of previous note
    const int noteId = note.getId();
    if (currentNote.exists() && (currentNote.getId() != noteId)) {
        this->noteHistory.updateCursorPositionOfNote(this->currentNote, ui->noteTextEdit);
    }

    _noteTabManager->_lastNoteId = this->currentNote.getId();
    this->currentNote = note;
    const QString currentNoteReference = noteFoldingReference(this->currentNote);
    ui->noteTextEdit->setCurrentNoteReference(currentNoteReference);

    // for places we can't get the current note id, like the Markdown
    // highlighter
    qApp->setProperty("currentNoteId", noteId);

    const QString name = note.getName();
    updateWindowTitle();

    // update current tab
    if (!jumpToTab(note) && Utils::Gui::isTabWidgetTabSticky(
                                ui->noteEditTabWidget, ui->noteEditTabWidget->currentIndex())) {
        openCurrentNoteInTab();
    }

    updateCurrentTabData(note);

    // find and set the current item
    if (updateSelectedNote) {
        QList<QTreeWidgetItem *> items = ui->noteTreeWidget->findItems(name, Qt::MatchExactly);
        if (items.count() > 0) {
            const QSignalBlocker blocker(ui->noteTreeWidget);
            Q_UNUSED(blocker)

            // to avoid that multiple notes will be selected
            ui->noteTreeWidget->clearSelection();

            ui->noteTreeWidget->setCurrentItem(items[0]);
        }
    }

    // update the text of the text edit
    if (updateNoteText) {
        const QSignalBlocker blocker(ui->noteTextEdit);
        Q_UNUSED(blocker)

        this->setNoteTextFromNote(&note, false, false, true);

        // hide the encrypted note text edit by default and show the regular one
        ui->encryptedNoteTextEdit->hide();
        ui->noteTextEdit->show();
    }

    if (updateNoteText && note.exists()) {
        ui->noteTextEdit->setMarkdownLspDocumentPath(note.fullNoteFilePath(),
                                                     ui->noteTextEdit->toPlainText());
    } else if (updateNoteText) {
        ui->noteTextEdit->closeMarkdownLspDocument();
    }

    updateNoteEncryptionUI();
    // we also need to do this in on_noteTreeWidget_itemSelectionChanged
    // because of different timings
    reloadCurrentNoteTags();
    updateNoteTextEditReadOnly();

    ScriptingService::instance()->onCurrentNoteChanged(&currentNote);

    // update the share button
    updateShareButton();

    // call a script hook that a new note was opened
    ScriptingService::instance()->callHandleNoteOpenedHook(&currentNote);

    // update file path label
    _noteFilePathLabel->updateText();

    //    putenv(QStringLiteral("QOWNNOTES_CURRENT_NOTE_PATH=%1").arg(currentNote)
    //            .fullNoteFilePath()).toLatin1().data());
    //    setenv("QOWNNOTES_CURRENT_NOTE_PATH",
    //           currentNote.fullNoteFilePath().toLatin1().data(),
    //           1);

    // add new note to history
    if (addNoteToHistory && note.exists()) {
        this->noteHistory.add(note, ui->noteTextEdit);
    }

    noteEditCursorPositionChanged();

    // create a hash of the text of the current note to be able if it was
    // modified outside of QOwnNotes
    updateCurrentNoteTextHash();

    // clear external image cache
    Note::externalImageHash()->clear();

    ui->actionToggle_distraction_free_mode->setEnabled(true);

    updateNoteGraphicsView();

    // Update bookmark display for the current note
    updateNoteBookmarkDisplay();
}

void MainWindow::updateNoteGraphicsView() {
    if (_noteRelationScene && _noteGraphicsViewDockWidget->isVisible()) {
        _noteRelationScene->drawForNote(currentNote);
    }
}

void MainWindow::updateCurrentTabData(const Note &note) const {
    _noteTabManager->updateCurrentTabData(note);
}

void MainWindow::closeOrphanedTabs() const { _noteTabManager->closeOrphanedTabs(); }

bool MainWindow::jumpToTab(const Note &note) const { return _noteTabManager->jumpToTab(note); }

/**
 * Creates a hash of the text of the current note to be able to tell if it was
 * modified outside of QOwnNotes
 */
void MainWindow::updateCurrentNoteTextHash() {
    _currentNoteTextHash = QString(
        QCryptographicHash::hash(currentNote.getNoteText().toLocal8Bit(), QCryptographicHash::Sha1)
            .toHex());
}

void MainWindow::updateActionUiEnabled() {
    const bool allowEditing = !ui->noteTextEdit->isReadOnly();

    setMenuEnabled(ui->menuEditNote, allowEditing);
    setMenuEnabled(ui->menuInsert, allowEditing);
    setMenuEnabled(ui->menuFormat, allowEditing);
    ui->actionPaste_image->setEnabled(allowEditing);
    ui->actionReplace_in_current_note->setEnabled(allowEditing);
    ui->actionAutocomplete->setEnabled(allowEditing);
    ui->actionSplit_note_at_cursor_position->setEnabled(allowEditing);
}

/**
 * Sets the note text edit to readonly if the note does not exist or the
 * note file is not writable or the note is encrypted
 */
void MainWindow::updateNoteTextEditReadOnly() {
    setNoteTextEditReadOnly(!(currentNote.exists() && currentNote.fileWriteable() &&
                              Utils::Misc::isNoteEditingAllowed()));

    if (ui->noteTextEdit->isVisible() && currentNote.hasEncryptedNoteText()) {
        ui->noteTextEdit->setReadOnly(true);
    }

    // Also update the other UI elements
    updateActionUiEnabled();

    ui->noteTextEdit->setTextInteractionFlags(ui->noteTextEdit->textInteractionFlags() |
                                              Qt::TextSelectableByKeyboard);
}

/**
 * Sets the readonly state of the note text edits
 *
 * @param readonly
 */
void MainWindow::setNoteTextEditReadOnly(bool readonly) const {
    ui->noteTextEdit->setReadOnly(readonly);
    ui->encryptedNoteTextEdit->setReadOnly(readonly);

    ui->noteTextEdit->setTextInteractionFlags(ui->noteTextEdit->textInteractionFlags() |
                                              Qt::TextSelectableByKeyboard);
    ui->encryptedNoteTextEdit->setTextInteractionFlags(
        ui->encryptedNoteTextEdit->textInteractionFlags() | Qt::TextSelectableByKeyboard);
}

/**
 * Updates the share button
 */
void MainWindow::updateShareButton() {
    const QSignalBlocker blocker(ui->actionShare_note);
    Q_UNUSED(blocker)
    ui->actionShare_note->setChecked(currentNote.isShared());
}

/**
 * Updates the window title
 */
void MainWindow::updateWindowTitle() {
    const QString &session = qApp->property("session").toString();
    QString title = currentNote.exists() ? currentNote.getName() : QStringLiteral("#");

    if (NoteFolder::countAll() > 0) {
        title += QStringLiteral(" - %1").arg(NoteFolder::currentNoteFolder().getName());
    }

    if (!session.isEmpty()) {
        title += QStringLiteral(" - %1").arg(session);
    }

    title += QStringLiteral(" - QOwnNotes");

    if (Utils::Misc::isInPortableMode()) {
        title += QStringLiteral(" Portable");
    }

    title += QStringLiteral(" - %3").arg(QStringLiteral(VERSION));

#ifdef QT_DEBUG
    title.prepend(QStringLiteral("[Debug] "));
#endif

    setWindowTitle(title);
}

/**
 * Focuses the note text edit and sets the cursor
 */
void MainWindow::focusNoteTextEdit() {
    QTextCursor tmpCursor = ui->noteTextEdit->textCursor();

    // move the cursor to the 4th line if the cursor was at the beginning
    if (tmpCursor.position() == 0) {
        tmpCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
        tmpCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
        tmpCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
        tmpCursor.movePosition(QTextCursor::Down, QTextCursor::MoveAnchor);
        ui->noteTextEdit->setTextCursor(tmpCursor);
    }

    // focus note text edit
    ui->noteTextEdit->setFocus();
}

/**
 * Removes the current note
 */
void MainWindow::removeCurrentNote() { _noteOperationsManager->removeCurrentNote(); }

/**
 * Searches and removes note from the note tree widget
 */
void MainWindow::removeNoteFromNoteTreeWidget(Note &note) const {
    _noteTreeManager->removeNoteFromNoteTreeWidget(note);
}

/**
 * Resets the current note to the first note
 */
void MainWindow::resetCurrentNote(bool goToTop) {
    auto *event =
        new QKeyEvent(QEvent::KeyPress, goToTop ? Qt::Key_Home : Qt::Key_Down, Qt::NoModifier);
    QApplication::postEvent(ui->noteTreeWidget, event);
    _noteFilePathLabel->setText(QLatin1String());
}

/**
 * Stores the settings
 */
void MainWindow::storeSettings() {
    SettingsService settings;

    // don't store the window settings in distraction free mode
    if (!isInDistractionFreeMode()) {
        settings.setValue(QStringLiteral("MainWindow/geometry"), saveGeometry());
        settings.setValue(QStringLiteral("MainWindow/menuBarGeometry"),
                          ui->menuBar->saveGeometry());
    }

    // store a NoteHistoryItem to open the note again after the app started
    const NoteHistoryItem noteHistoryItem(&currentNote, ui->noteTextEdit);
    qDebug() << __func__ << " - 'noteHistoryItem': " << noteHistoryItem;
    settings.setValue(QStringLiteral("ActiveNoteHistoryItem"),
                      QVariant::fromValue(noteHistoryItem));

    // store the note history of the current note folder
    noteHistory.storeForCurrentNoteFolder();

    Utils::Gui::storeNoteTabs(ui->noteEditTabWidget);
}

/*!
 * Internal events
 */

void MainWindow::closeEvent(QCloseEvent *event) {
    _closeEventWasFired = true;
    const bool forceQuit = qApp->property("clearAppDataAndExit").toBool();
    const bool isJustHide = showSystemTray;

#ifdef Q_OS_MAC
    // #1113, unfortunately the closeEvent is also fired when the application
    // will be quit in the dock menu
//    isJustHide = true;
#endif

    // #1496, don't ignore close event when the app is hidden to tray
    // this can occur when the OS issues close events on shutdown
    if (isJustHide && !forceQuit && !isHidden()) {
#ifdef Q_OS_MAC
        showMinimized();
#else
        hide();
#endif
        event->ignore();
    } else {
        // we need to do this in the close event (and _not_ in the destructor),
        // because in the destructor the layout will be destroyed in dark mode
        // when the window was closed
        // https://github.com/pbek/QOwnNotes/issues/1015
        // Checks will be done in the method
        storeCurrentWorkspace();

        QMainWindow::closeEvent(event);
    }
}

//
// Event filters on the MainWindow
//
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);

        if (obj == ui->searchLineEdit->completer()->popup()) {
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
                // set a variable to ignore that first "Return" in the
                // return-handler
                _searchLineEditFromCompleter = true;
                return QMainWindow::eventFilter(obj, event);
            }
        } else if ((obj == ui->newNoteTagLineEdit) ||
                   ((ui->newNoteTagLineEdit->completer() != nullptr) &&
                    (obj == ui->newNoteTagLineEdit->completer()->popup()))) {
            // if tab is pressed while adding a tag the tag that starts with
            // the current text will be added
            if (keyEvent->key() == Qt::Key_Tab) {
                // fetch the tag that is starting with the current text
                Tag tag = Tag::fetchByName(ui->newNoteTagLineEdit->text(), true);
                if (tag.isFetched()) {
                    linkTagNameToCurrentNote(tag.getName(), true);
                    on_newNoteTagLineEdit_editingFinished();
                }

                return QMainWindow::eventFilter(obj, event);
            }
        } else if (obj == ui->searchLineEdit) {
            bool downSelectNote = false;

            // fallback to the default completion
            ui->searchLineEdit->completer()->setCompletionMode(QCompleter::PopupCompletion);

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
                    return QMainWindow::eventFilter(obj, event);
                } else {
                    // if nothing was found in the completer we want to jump
                    // to the note list
                    downSelectNote = true;
                }
            }

            // set focus to the notes list if Key_Tab was pressed in the search line edit
            if (keyEvent->key() == Qt::Key_Tab || downSelectNote) {
                // add the current search text to the saved searches
                storeSavedSearch();

                // choose another selected item if current item is invisible
                QTreeWidgetItem *item = ui->noteTreeWidget->currentItem();
                if ((item != nullptr) && item->isHidden()) {
                    QTreeWidgetItem *firstVisibleItem = firstVisibleNoteTreeWidgetItem();
                    if (firstVisibleItem != nullptr) {
                        ui->noteTreeWidget->setCurrentItem(firstVisibleItem);
                    }
                }

                // give the keyboard focus to the note tree widget
                ui->noteTreeWidget->setFocus();
                return true;
            }
            return QMainWindow::eventFilter(obj, event);
        } else if (obj == activeNoteTextEdit()) {
            // check if we want to leave the distraction free mode and the
            // search widget is not visible (because we want to close that
            // first)
            if ((keyEvent->key() == Qt::Key_Escape) && isInDistractionFreeMode() &&
                !activeNoteTextEdit()->searchWidget()->isVisible()) {
                toggleDistractionFreeMode();

                return true;
            }

            return QMainWindow::eventFilter(obj, event);
        } else if (obj == ui->noteTreeWidget) {
            // set focus to the note text edit if Key_Return or Key_Tab were
            // pressed in the notes list
            if ((keyEvent->key() == Qt::Key_Return) || (keyEvent->key() == Qt::Key_Tab)) {
                // focusNoteTextEdit() might cause a crash in
                // on_noteTreeWidget_itemChanged if Note::allowDifferentFileName()
                // is true when Note::handleNoteRenaming is called, the
                // QTimer::singleShot helps with that
                QTimer::singleShot(150, this, SLOT(focusNoteTextEdit()));

                return true;
            } else if ((keyEvent->key() == Qt::Key_Delete) ||
                       (keyEvent->key() == Qt::Key_Backspace)) {
                removeSelectedNotes();
                return true;
            } else if (keyEvent->key() == Qt::Key_Escape) {
                ui->searchLineEdit->clear();
                return true;
            }
            return QMainWindow::eventFilter(obj, event);
        } else if (obj == ui->tagTreeWidget) {
            if ((keyEvent->key() == Qt::Key_Delete) || (keyEvent->key() == Qt::Key_Backspace)) {
                removeSelectedTags();
                return true;
            }
            return QMainWindow::eventFilter(obj, event);
        }
    } else if (event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);

        if ((mouseEvent->button() == Qt::BackButton)) {
            // There was an issue with the back button triggering infinitely when
            // the selected notes were opened in a new tab, the singleShot helped
            QTimer::singleShot(0, this, [this]() {
                // move back in the note history
                on_action_Back_in_note_history_triggered();
            });
        } else if ((mouseEvent->button() == Qt::ForwardButton)) {
            // move forward in the note history
            on_action_Forward_in_note_history_triggered();
        }
    } else if (event->type() == QEvent::MouseButtonPress && obj == ui->selectedTagsToolButton) {
        // we don't want to make the button clickable
        return true;
    } else if (obj == ui->noteEditTabWidget->tabBar() &&
               event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        int tabIndex = ui->noteEditTabWidget->tabBar()->tabAt(mouseEvent->pos());

        // Close the note tab on the middle-click
        if (mouseEvent->button() == Qt::MiddleButton && tabIndex != -1) {
            ui->noteEditTabWidget->removeTab(tabIndex);
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

/**
 * Finds the first visible tree widget item
 */
QTreeWidgetItem *MainWindow::firstVisibleNoteTreeWidgetItem() {
    return _noteTreeManager->firstVisibleNoteTreeWidgetItem();
}

/**
 * Highlights all occurrences of str in the note text edit and does a "in note
 * search"
 */
void MainWindow::searchInNoteTextEdit(QString str) {
    _searchFilterManager->searchInNoteTextEdit(str);
}

/**
 * highlights all occurrences of the search line text in the note text edit
 */
void MainWindow::searchForSearchLineTextInNoteTextEdit() {
    _searchFilterManager->searchForSearchLineTextInNoteTextEdit();
}

/**
 * Asks for the password if the note is encrypted and can't be decrypted
 */
void MainWindow::askForEncryptedNotePasswordIfNeeded(const QString &additionalText) {
    _noteEncryptionManager->askForEncryptedNotePasswordIfNeeded(additionalText);
}

/**
 * Gets the maximum image width
 */
int MainWindow::getMaxImageWidth() const {
    // TODO: Make sure this works or not
#ifndef USE_QLITEHTML
    const QMargins margins = ui->noteTextView->contentsMargins();
    int maxImageWidth = ui->noteTextView->viewport()->width() - margins.left() -
#else
    const QMargins margins = _notePreviewWidget->contentsMargins();
    int maxImageWidth = _notePreviewWidget->viewport()->width() - margins.left() -
#endif
                        margins.right() - 15;

    if (maxImageWidth < 0) {
        maxImageWidth = 16;
    }

    return maxImageWidth;
}

/**
 * Sets the note text according to a note
 */
void MainWindow::setNoteTextFromNote(Note *note, bool updateNoteTextViewOnly,
                                     bool ignorePreviewVisibility,
                                     bool allowRestoreCursorPosition) {
    if (note == nullptr) {
        return;
    }

    auto historyItem = noteHistory.getLastItemOfNote(currentNote);

    if (!updateNoteTextViewOnly) {
        const QString noteReference = noteFoldingReference(*note);
        qobject_cast<QOwnNotesMarkdownHighlighter *>(ui->noteTextEdit->highlighter())
            ->updateCurrentNote(note);
        ui->noteTextEdit->closeMarkdownLspDocument();
        ui->noteTextEdit->setCurrentNoteReference(noteReference);
        ui->noteTextEdit->setText(note->getNoteText());
        if (note->exists()) {
            ui->noteTextEdit->setMarkdownLspDocumentPath(note->fullNoteFilePath(),
                                                         ui->noteTextEdit->toPlainText());
        }
    }

    if (allowRestoreCursorPosition && Utils::Misc::isRestoreCursorPosition()) {
        historyItem.restoreTextEditPosition(ui->noteTextEdit);
        ui->noteTextEdit->highlightCurrentLine();
    }

    // update the preview text edit if the dock widget is visible
    if (_notePreviewDockWidget->isVisible() || ignorePreviewVisibility) {
        const bool decrypt = ui->noteTextEdit->isHidden();

        const QString html = note->toMarkdownHtml(NoteFolder::currentLocalPath(),
                                                  getMaxImageWidth(), false, decrypt);

        // create a hash of the html (because
        const QString hash =
            QString(QCryptographicHash::hash(html.toLocal8Bit(), QCryptographicHash::Sha1).toHex());

        // update the note preview if the text has changed
        // we use our hash because ui->noteTextView->toHtml() may return
        // a different text than before
        if (_notePreviewHash != hash) {
#ifdef USE_QLITEHTML
            _notePreviewWidget->setHtml(html);
#else
            ui->noteTextView->setHtml(html);
#endif
            _notePreviewHash = hash;
        }
    }

    // update the slider when editing notes
    noteTextSliderValueChanged(activeNoteTextEdit()->verticalScrollBar()->value(), true);
}

/**
 * Starts the parsing for the navigation widget
 */
void MainWindow::startNavigationParser() { _navigationManager->startNavigationParser(); }

/**
 * Sets the text of the current note.
 * This is a public callback function for the version dialog.
 *
 * @brief MainWindow::setCurrentNoteText
 * @param text
 */
void MainWindow::setCurrentNoteText(const QString &text) {
    bool canDecryptNoteText = currentNote.canDecryptNoteText();
    allowNoteEditing();

    // If the note can be decrypted, we need to show the encrypted note text edit and set the text
    // there
    if (canDecryptNoteText) {
        ui->encryptedNoteTextEdit->setText(text);
        ui->encryptedNoteTextEdit->show();
        ui->noteTextEdit->hide();
    } else {
        currentNote.setNoteText(text);
        setNoteTextFromNote(&currentNote, false);
    }
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
void MainWindow::createNewNote(QString name, QString text, CreateNewNoteOptions options) {
    _noteOperationsManager->createNewNote(std::move(name), std::move(text), options);
}

/**
 * @brief Restores a trashed note on the server.
 * @param name
 * @param text
 *
 * This is a public callback function for the trash dialog.
 */
void MainWindow::restoreTrashedNoteOnServer(const QString &fileName, int timestamp) {
    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->restoreTrashedNoteOnServer(fileName, timestamp);
}

/**
 * @brief Removes selected notes after a confirmation
 */
void MainWindow::removeSelectedNotes() { _noteOperationsManager->removeSelectedNotes(); }

/**
 * Removes selected tags after a confirmation
 */
void MainWindow::removeSelectedTags() { _tagManager->removeSelectedTags(); }

/**
 * @brief Select all notes
 */
void MainWindow::selectAllNotes() { ui->noteTreeWidget->selectAll(); }

/**
 * @brief Moves selected notes after a confirmation
 * @param destinationFolder
 */
void MainWindow::moveSelectedNotesToFolder(const QString &destinationFolder) {
    _noteOperationsManager->moveSelectedNotesToFolder(destinationFolder);
}

/**
 * Returns a list of all selected notes
 *
 * @return
 */
QVector<Note> MainWindow::selectedNotes() { return _noteTreeManager->selectedNotes(); }

/**
 * Un-sets the current note
 */
void MainWindow::unsetCurrentNote() {
    ui->noteTextEdit->setCurrentNoteReference(QString());

    // reset the current note
    currentNote = Note();

    // clear the note preview
#ifndef USE_QLITEHTML
    const QSignalBlocker blocker(ui->noteTextView);
    ui->noteTextView->clear();
#else
    const QSignalBlocker blocker(_notePreviewWidget);
    _notePreviewWidget->setHtml(QString());
#endif
    Q_UNUSED(blocker)

    // clear the note text edit
    const QSignalBlocker blocker2(ui->noteTextEdit);
    Q_UNUSED(blocker2)
    ui->noteTextEdit->clear();
    ui->noteTextEdit->show();

    // clear the encrypted note text edit
    const QSignalBlocker blocker3(ui->encryptedNoteTextEdit);
    Q_UNUSED(blocker3)
    ui->encryptedNoteTextEdit->hide();
    ui->encryptedNoteTextEdit->clear();

    ui->actionToggle_distraction_free_mode->setEnabled(false);

    // set the note text edits to readonly
    setNoteTextEditReadOnly(true);
}

QString MainWindow::noteFoldingReference(const Note &note) const {
    if (!note.exists()) {
        return QString();
    }

    return QStringLiteral("%1:%2")
        .arg(NoteFolder::currentNoteFolderId())
        .arg(note.relativeNoteFilePath(QStringLiteral("/")));
}

/**
 * @brief Copies selected notes after a confirmation
 * @param destinationFolder
 */
void MainWindow::copySelectedNotesToFolder(const QString &destinationFolder,
                                           const QString &noteFolderPath) {
    _noteOperationsManager->copySelectedNotesToFolder(destinationFolder, noteFolderPath);
}

/**
 * Tags selected notes
 */
void MainWindow::tagSelectedNotes(const Tag &tag) { _tagManager->tagSelectedNotes(tag); }

/**
 * Removes a tag from the selected notes
 */
void MainWindow::removeTagFromSelectedNotes(const Tag &tag) {
    _tagManager->removeTagFromSelectedNotes(tag);
}

/**
 * Activates or deactivates a workaround for the ill behaving directory watcher
 *
 * @param isNotesDirectoryWasModifiedDisabled
 * @param alsoHandleNotesWereModified
 */
void MainWindow::directoryWatcherWorkaround(bool isNotesDirectoryWasModifiedDisabled,
                                            bool alsoHandleNotesWereModified) {
    _noteIndexManager->directoryWatcherWorkaround(isNotesDirectoryWasModifiedDisabled,
                                                  alsoHandleNotesWereModified);
}

/**
 * Handle the coloring of the note in the note tree widget
 *
 * @param note
 */
void MainWindow::handleNoteTreeTagColoringForNote(const Note &note) {
    _tagManager->handleNoteTreeTagColoringForNote(note);
}

/**
 * @brief Updates the current folder tooltip
 */
void MainWindow::updateCurrentFolderTooltip() {
    ui->actionSet_ownCloud_Folder->setStatusTip(tr("Current notes folder: ") + this->notesPath);
    ui->actionSet_ownCloud_Folder->setToolTip(tr("Set the notes folder. Current notes folder: ") +
                                              this->notesPath);
}

/**
 * Opens the settings dialog
 */
void MainWindow::openSettingsDialog(int page, bool openScriptRepository) {
    QPointer<SettingsDialog> settingsDialog = new SettingsDialog(page, this);

    if (openScriptRepository) {
        QTimer::singleShot(10, settingsDialog, SLOT(searchScriptInRepository()));
    }

    // open the settings dialog
    int ret = settingsDialog->exec();

    delete settingsDialog;

    if (ret != QDialog::Accepted) {
        return;
    }

    // let the world know that settings have change
    // anything that uses settings should use this
    // signal to adapt dynamically
    Q_EMIT settingsChanged();

    // TODO: Remove the need for restart
    // shows a restart application notification if needed
    if (showRestartNotificationIfNeeded()) {
        return;
    }

    // make sure no settings get written after we got the clearAppDataAndExit call
    if (qApp->property("clearAppDataAndExit").toBool()) {
        return;
    }

    // disable spell checking if current note is encrypted
    updateNoteEncryptionUI();

    // read all relevant settings, that can be set in the settings dialog,
    // even if the dialog was canceled
    readSettingsFromSettingsDialog();

    // update the panels sort and order
    updatePanelsSortOrder();

    // reset the note save timer
    this->noteSaveTimer->stop();
    this->noteSaveTimer->start(this->noteSaveIntervalTime * 1000);

    // reset the git commit timer
    _gitCommitTimer->stop();
    _gitCommitTimer->start(_gitCommitInterval * 1000);

    // set the current note folder again in case its path was changed
    const NoteFolder noteFolder = NoteFolder::currentNoteFolder();

    if (noteFolder.isFetched()) {
        changeNoteFolder(noteFolder.getId(), true);
    }

    // reload note folders in case we changed them in the settings
    loadNoteFolderListMenu();

    // load the note list again in case the setting on the note name has changed
    loadNoteDirectoryList();

    // force that the preview is regenerated
    forceRegenerateNotePreview();
}

void MainWindow::forceRegenerateNotePreview() {
    _notePreviewHash.clear();
    currentNote.resetNoteTextHtmlConversionHash();
    regenerateNotePreview();
}

/**
 * Shows a restart application notification if needed
 *
 * @return true if the applications is restarting
 */
bool MainWindow::showRestartNotificationIfNeeded(bool force) {
    const bool needsRestart = qApp->property("needsRestart").toBool() || force;

    if (!needsRestart) {
        return false;
    }

    qApp->setProperty("needsRestart", false);

    if (QMessageBox::question(this, tr("Restart application"),
                              tr("You may need to restart the application to let the "
                                 "changes take effect.") +
                                  Utils::Misc::appendSingleAppInstanceTextIfNeeded(),
                              QMessageBox::Yes | QMessageBox::Cancel,
                              QMessageBox::Yes) == QMessageBox::Yes) {
        storeSettings();
        Utils::Misc::restartApplication();

        return true;
    }

    return false;
}

/**
 * @brief Returns the active note text edit
 */
QOwnNotesMarkdownTextEdit *MainWindow::activeNoteTextEdit() {
    return ui->noteTextEdit->isHidden() ? ui->encryptedNoteTextEdit : ui->noteTextEdit;
}

/**
 * @brief Handles the linking of text
 */
void MainWindow::handleTextNoteLinking(int page) {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    auto *dialog = new LinkDialog(page, QString(), this);

    QString selectedText = textEdit->textCursor().selectedText();
    if (!selectedText.isEmpty()) {
        dialog->setLinkName(selectedText);
    }

    dialog->exec();

    if (dialog->result() == QDialog::Accepted) {
        const QString url = dialog->getURL();
        const QString linkName = dialog->getLinkName();
        const QString linkDescription = dialog->getLinkDescription();
        // remove "]" characters, because they will break Markdown links
        QString noteName = dialog->getSelectedNoteName().remove("]");

        if ((!noteName.isEmpty()) || (!url.isEmpty())) {
            QString newText;
            QString chosenLinkName =
                linkName.isEmpty() ? textEdit->textCursor().selectedText() : linkName;
            // remove "]" characters, because they will break Markdown links
            chosenLinkName.remove("]");

            // if user has entered a URL
            if (!url.isEmpty()) {
                newText = !chosenLinkName.isEmpty()
                              ? QStringLiteral("[") + chosenLinkName + QStringLiteral("](") + url +
                                    QStringLiteral(")")
                              : QStringLiteral("<") + url + QStringLiteral(">");
            } else {
                const QString noteUrl =
                    currentNote.getNoteUrlForLinkingTo(dialog->getSelectedNote());

                const QString heading = dialog->getSelectedHeading();
                const QString headingText =
                    heading.isEmpty() ? QStringLiteral()
                                      : QStringLiteral("#") + QUrl::toPercentEncoding(heading);

                // if user has selected a note
                if (!chosenLinkName.isEmpty()) {
                    noteName = chosenLinkName;
                } else if (!heading.isEmpty()) {
                    // if a note and a heading were selected add heading text to link title
                    noteName += QStringLiteral(" - ") + heading;
                }

                newText = QStringLiteral("[") + noteName + QStringLiteral("](") + noteUrl +
                          headingText + QStringLiteral(")");
            }

            if (!linkDescription.isEmpty()) {
                newText += QStringLiteral(" ") + linkDescription;
            }

            textEdit->textCursor().insertText(newText);
        }
    }

    delete (dialog);
}

/**
 * @brief Sets the current note from a CurrentNoteHistoryItem
 * @param item
 */
void MainWindow::setCurrentNoteFromHistoryItem(const NoteHistoryItem &item) {
    qDebug() << item;
    qDebug() << item.getNote();

    setCurrentNote(item.getNote(), true, true, false);
    item.restoreTextEditPosition(ui->noteTextEdit);
}

/**
 * @brief Prepares the printer to print the content of a text edit widget
 * @param textEdit
 */
bool MainWindow::preparePrintNotePrinter(QPrinter *printer) {
    return _exportPrintManager->preparePrintNotePrinter(printer);
}

/**
 * @brief Prints the content of a text document
 * @param textEdit
 */
void MainWindow::printTextDocument(QTextDocument *textDocument) {
    _exportPrintManager->printTextDocument(textDocument);
}

/**
 * @brief Prepares the printer dialog to exports the content of a text edit
 *        widget as PDF
 * @param printer
 */
bool MainWindow::prepareExportNoteAsPDFPrinter(QPrinter *printer) {
    return _exportPrintManager->prepareExportNoteAsPDFPrinter(printer);
}

/**
 * @brief Exports the content of a plain text edit widget as PDF
 * @param textEdit
 */
void MainWindow::exportNoteAsPDF(QPlainTextEdit *textEdit) {
    _exportPrintManager->exportNoteAsPDF(textEdit);
}

/**
 * @brief Exports the document as PDF
 * @param doc
 */
void MainWindow::exportNoteAsPDF(QTextDocument *doc) { _exportPrintManager->exportNoteAsPDF(doc); }

/**
 * Shows the app metrics notification if not already shown
 */
void MainWindow::showAppMetricsNotificationIfNeeded() {
    SettingsService settings;
    bool showDialog = !settings.value(QStringLiteral("appMetrics/notificationShown")).toBool();

    if (showDialog) {
        settings.setValue(QStringLiteral("appMetrics/notificationShown"), true);

        QMessageBox msgBox(QMessageBox::Information, QStringLiteral("QOwnNotes"),
                           tr("QOwnNotes will track anonymous usage data, that helps to "
                              "decide what parts of QOwnNotes to improve next "
                              "and to find and fix bugs. You can disable that "
                              "behaviour in the settings."),
                           QMessageBox::NoButton, this);
        msgBox.addButton(tr("&Ok"), QMessageBox::AcceptRole);
        QPushButton *settingsButton =
            msgBox.addButton(tr("Open &settings"), QMessageBox::ActionRole);
        msgBox.setDefaultButton(settingsButton);
        msgBox.exec();

        if (msgBox.clickedButton() == settingsButton) {
            openSettingsDialog(SettingsDialog::NetworkPage);
        }
    }
}

/**
 * Opens the task list dialog
 */
void MainWindow::openTodoDialog(const QString &taskUid) {
    if (!OwnCloudService::isTodoCalendarSupportEnabled()) {
        QMessageBox msgBox(QMessageBox::Warning, tr("Todo lists disabled!"),
                           tr("You have disabled the todo lists.<br />"
                              "Please check your <strong>Todo</strong> "
                              "configuration in the settings!"),
                           QMessageBox::NoButton, nullptr);
        QPushButton *settingsButton =
            msgBox.addButton(tr("Open &settings"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);
        msgBox.setDefaultButton(settingsButton);
        msgBox.exec();

        if (msgBox.clickedButton() == settingsButton) {
            openSettingsDialog(SettingsDialog::TodoPage);
        }

        return;
    }

    // show main window to prevent crashes on Windows
    show();

    SettingsService settings;
    QStringList todoCalendarEnabledUrlList =
        settings.value(QStringLiteral("ownCloud/todoCalendarEnabledUrlList")).toStringList();

    // check if we have got any task list enabled
    if (todoCalendarEnabledUrlList.count() == 0) {
        QMessageBox msgBox(QMessageBox::Warning, tr("No selected todo lists!"),
                           tr("You have not selected any todo lists.<br />"
                              "Please check your <strong>Todo</strong> "
                              "configuration in the settings!"),
                           QMessageBox::NoButton, nullptr);
        QPushButton *settingsButton =
            msgBox.addButton(tr("Open &settings"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);
        msgBox.setDefaultButton(settingsButton);
        msgBox.exec();

        if (msgBox.clickedButton() == settingsButton) {
            openSettingsDialog(SettingsDialog::TodoPage);
        }

        return;
    }

    if (_todoDialog == nullptr) {
        _todoDialog = new TodoDialog(taskUid, this);
    } else {
        _todoDialog->refreshUi();
        _todoDialog->jumpToTask(taskUid);
    }

    _todoDialog->show();
    _todoDialog->activateWindow();
    _todoDialog->raise();

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

/**
 * Triggers if the text in the note text edit was modified
 */
void MainWindow::on_noteTextEdit_textChanged() {
    // this also triggers when formatting is applied / syntax highlighting
    // changes!
    //    noteTextEditTextWasUpdated();

    // Override the _noteViewUpdateTimer with a debounce time
    const int debounceTime = Utils::Misc::getPreviewRefreshDebounceTime();
    if (debounceTime > 0) {
        _noteViewUpdateTimer->start(debounceTime);
    }

    startAutoReadOnlyModeIfEnabled();
}

void MainWindow::noteTextEditTextWasUpdated() {
    Note note = this->currentNote;
    note.updateNoteTextFromDisk();

    // we are transforming line feeds, because in some instances Windows
    // managed to sneak some "special" line feeds in
    const QString noteTextFromDisk = Utils::Misc::transformLineFeeds(note.getNoteText());
    QString text = Utils::Misc::transformLineFeeds(ui->noteTextEdit->toPlainText());

    // store the note to the database if the note text differs from the one
    // on the disk or the note was already modified but not stored to disk
    if ((text != noteTextFromDisk || currentNote.getHasDirtyData())) {
        this->currentNote.storeNewText(std::move(text));
        this->currentNote.refetch();
        this->currentNoteLastEdited = QDateTime::currentDateTime();
        _noteViewNeedsUpdate = true;

        ScriptingService::instance()->onCurrentNoteChanged(&currentNote);

        updateNoteEncryptionUI();
        handleNoteTextChanged();
    }
}

void MainWindow::handleNoteTextChanged() {
    SettingsService settings;
    if (settings.value(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE).toInt() ==
        SORT_BY_LAST_CHANGE) {
        makeCurrentNoteFirstInNoteList();
    } else if (Utils::Misc::isNoteListPreview()) {
        updateNoteTreeWidgetItem(currentNote);
    }

    const QSignalBlocker blocker(ui->noteTreeWidget);
    Q_UNUSED(blocker)

    // update the note list tooltip of the note
    Utils::Gui::setTreeWidgetItemToolTipForNote(ui->noteTreeWidget->currentItem(), currentNote,
                                                &currentNoteLastEdited);
}

void MainWindow::on_action_Quit_triggered() {
    storeSettings();
    QApplication::quit();
}

void MainWindow::quitApp() { QApplication::quit(); }

void MainWindow::on_actionSet_ownCloud_Folder_triggered() {
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    openSettingsDialog(SettingsDialog::NoteFolderPage);
}

void MainWindow::on_searchLineEdit_textChanged(const QString &arg1) {
    _searchFilterManager->on_searchLineEdit_textChanged(arg1);
}

/**
 * Does the note filtering
 */
void MainWindow::filterNotes(bool searchForText) {
    _searchFilterManager->filterNotes(searchForText);
}

/**
 * Checks if tagging is enabled
 */
bool MainWindow::isTagsEnabled() { return _taggingDockWidget->isVisible(); }

/**
 * Checks if the NoteDiffDialog is currently open
 */
bool MainWindow::isNoteDiffDialogOpen() {
    return noteDiffDialog != nullptr && noteDiffDialog->isVisible();
}

/**
 * Checks if the Markdown view is enabled
 */
bool MainWindow::isMarkdownViewEnabled() {
    SettingsService settings;
    return settings.value(QStringLiteral("markdownViewEnabled"), true).toBool();
}

/**
 * Checks if the note edit pane is enabled
 */
bool MainWindow::isNoteEditPaneEnabled() {
    return _noteEditIsCentralWidget ||
           ((_noteEditDockWidget != nullptr) && _noteEditDockWidget->isVisible());
}

/**
 * Does the note filtering by text in the search line edit
 */
void MainWindow::filterNotesBySearchLineEditText(bool searchInNote) {
    _searchFilterManager->filterNotesBySearchLineEditText(searchInNote);
}

/**
 * Opens the search widget in the current note and searches for all
 * occurrences of the words in the search text
 *
 * @param searchText
 */
void MainWindow::doSearchInNote(QString searchText) {
    _searchFilterManager->doSearchInNote(searchText);
}

/**
 * Does the note filtering by tags
 */
void MainWindow::filterNotesByTag() { _searchFilterManager->filterNotesByTag(); }

/**
 * Does the note filtering by note sub folders
 */
void MainWindow::filterNotesByNoteSubFolders() {
    _searchFilterManager->filterNotesByNoteSubFolders();
}

//
// set focus on search line edit if Ctrl + Shift + F was pressed
//
void MainWindow::on_action_Find_note_triggered() {
    _searchFilterManager->on_action_Find_note_triggered();
}

//
// jump to found note or create a new one if not found
//
void MainWindow::on_searchLineEdit_returnPressed() {
    _searchFilterManager->on_searchLineEdit_returnPressed();
}

/**
 * Jumps to found note or create a new one if not found
 */
void MainWindow::jumpToNoteOrCreateNew(bool disableLoadNoteDirectoryList) {
    _searchFilterManager->jumpToNoteOrCreateNew(disableLoadNoteDirectoryList);
}

void MainWindow::on_action_Remove_note_triggered() {
    _noteOperationsManager->on_action_Remove_note_triggered();
}

void MainWindow::on_actionAbout_QOwnNotes_triggered() {
    auto *dialog = new AboutDialog(this);
    dialog->exec();
    delete (dialog);
}

/**
 * Triggered by the shortcut to create a new note with date in the headline
 */
void MainWindow::on_action_New_note_triggered() {
    _noteOperationsManager->on_action_New_note_triggered();
}

/**
 * Creates a new note
 *
 * @param noteName
 */
void MainWindow::createNewNote(QString noteName, bool withNameAppend) {
    _noteOperationsManager->createNewNote(std::move(noteName), withNameAppend);
}

/*
 * Handles urls in the note preview
 */
void MainWindow::onNotePreviewAnchorClicked(const QUrl &url) {
    qDebug() << __func__ << " - 'url': " << url;

    // Check if Ctrl key is pressed to open in new tab
    bool openInNewTab = QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier);

    const bool isLocalScheme = UrlHandler::isUrlSchemeLocal(url);
    const bool isDeckCardUrl = NextcloudDeckService::isCardUrl(url.toString());

    if (isLocalScheme || isDeckCardUrl) {
        // Update _lastNoteId when opening in a new tab to ensure the current note
        // is correctly placed in the old tab, not some previous note
        if (openInNewTab && currentNote.exists()) {
            _noteTabManager->_lastNoteId = currentNote.getId();
        }

        UrlHandler().openUrl(url.toString(), openInNewTab);
    } else {
        ui->noteTextEdit->openUrl(url.toString(), false);
    }
}

/*
 * Handles note urls
 */
void MainWindow::openLocalUrl(QString urlString) { UrlHandler().openUrl(std::move(urlString)); }

/*
 * Manually check for updates
 */
void MainWindow::on_actionCheck_for_updates_triggered() {
    this->updateService->checkForUpdates(UpdateService::Manual);
}

/*
 * Open the issue page
 */
void MainWindow::on_actionReport_problems_or_ideas_triggered() {
    //    QDesktopServices::openUrl(QUrl("https://github.com/pbek/QOwnNotes/issues"));

    openIssueAssistantDialog();
}

void MainWindow::openIssueAssistantDialog() {
    if (_issueAssistantDialog == nullptr) {
        _issueAssistantDialog = new IssueAssistantDialog(this);
    } else {
    }

    _issueAssistantDialog->show();
    _issueAssistantDialog->activateWindow();
    _issueAssistantDialog->raise();
}

void MainWindow::on_actionAlphabetical_triggered(bool checked) {
    if (checked) {
        SettingsService settings;
        settings.setValue(QStringLiteral("notesPanelSort"), SORT_ALPHABETICAL);
        loadNoteDirectoryList();
    }

    // update the visibility of the note sort order selector
    updateNoteSortOrderSelectorVisibility(checked);
}

void MainWindow::on_actionBy_date_triggered(bool checked) {
    if (checked) {
        SettingsService settings;
        settings.setValue(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE);
        loadNoteDirectoryList();
    }

    // update the visibility of the note sort order selector
    updateNoteSortOrderSelectorVisibility(!checked);
}

void MainWindow::systemTrayIconClicked(QSystemTrayIcon::ActivationReason reason) {
    _systemTrayManager->systemTrayIconClicked(reason);
}

/**
 * Shows the window (also brings it to the front and un-minimizes it)
 */
void MainWindow::showWindow() { _systemTrayManager->showWindow(); }

/**
 * Generates the system tray context menu
 */
void MainWindow::generateSystemTrayContextMenu() {
    _systemTrayManager->generateSystemTrayContextMenu();
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
        Utils::Misc::replaceOwnCloudText(tr("Note versions are currently loaded from your ownCloud "
                                            "server")),
        QStringLiteral("🛜"), 20000);

    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->loadVersions(this->currentNote.relativeNoteFilePath(QStringLiteral("/")));
}

void MainWindow::enableShowVersionsButton() { ui->actionShow_versions->setDisabled(false); }

void MainWindow::on_actionShow_trash_triggered() {
    ui->actionShow_trash->setDisabled(true);
    showStatusBarMessage(
        Utils::Misc::replaceOwnCloudText(tr("Trashed notes are currently loaded from your ownCloud"
                                            " server")),
        QStringLiteral("🗑"), 20000);

    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->loadTrash();
}

void MainWindow::enableShowTrashButton() { ui->actionShow_trash->setDisabled(false); }

void MainWindow::on_actionSelect_all_notes_triggered() { selectAllNotes(); }

void MainWindow::on_actionInsert_text_link_triggered() {
    // handle the linking of text
    handleTextNoteLinking(LinkDialog::TextLinkPage);
}

void MainWindow::on_actionInsert_note_link_triggered() {
    // handle the linking of a note
    handleTextNoteLinking(LinkDialog::NoteLinkPage);
}

void MainWindow::on_action_DuplicateText_triggered() { activeNoteTextEdit()->duplicateText(); }

void MainWindow::on_action_Back_in_note_history_triggered() {
    if (this->noteHistory.back()) {
        ui->searchLineEdit->clear();
        setCurrentNoteFromHistoryItem(this->noteHistory.getCurrentHistoryItem());
    }
}

void MainWindow::on_action_Forward_in_note_history_triggered() {
    if (this->noteHistory.forward()) {
        ui->searchLineEdit->clear();
        setCurrentNoteFromHistoryItem(this->noteHistory.getCurrentHistoryItem());
    }
}

void MainWindow::on_action_Previous_heading_triggered() {
    if (!ui->navigationWidget->isVisible()) {
        return;
    }

    auto *textEdit = activeNoteTextEdit();
    int currentPosition = textEdit->textCursor().position();
    int headingPosition = ui->navigationWidget->getPreviousHeadingPosition(currentPosition);

    if (headingPosition >= 0) {
        QTextCursor cursor = textEdit->textCursor();
        cursor.setPosition(headingPosition);
        textEdit->setTextCursor(cursor);
        textEdit->ensureCursorVisible();
    }
}

void MainWindow::on_action_Next_heading_triggered() {
    if (!ui->navigationWidget->isVisible()) {
        return;
    }

    auto *textEdit = activeNoteTextEdit();
    int currentPosition = textEdit->textCursor().position();
    int headingPosition = ui->navigationWidget->getNextHeadingPosition(currentPosition);

    if (headingPosition >= 0) {
        QTextCursor cursor = textEdit->textCursor();
        cursor.setPosition(headingPosition);
        textEdit->setTextCursor(cursor);
        textEdit->ensureCursorVisible();
    }
}

void MainWindow::on_actionFold_all_headings_triggered() { activeNoteTextEdit()->foldAllHeadings(); }

void MainWindow::on_actionUnfold_all_headings_triggered() {
    activeNoteTextEdit()->unfoldAllHeadings();
}

void MainWindow::on_action_Shortcuts_triggered() {
    QDesktopServices::openUrl(
        QUrl(QStringLiteral("https://www.qownnotes.org/getting-started/shortcuts.html")));
}

void MainWindow::on_action_Knowledge_base_triggered() {
    QDesktopServices::openUrl(
        QUrl(QStringLiteral("https://www.qownnotes.org/getting-started/concept.html")));
}

/**
 * Inserts the current date
 */
void MainWindow::on_actionInsert_current_time_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    const QDateTime dateTime = QDateTime::currentDateTime();
    SettingsService settings;
    const QString format = settings.value(QStringLiteral("insertTimeFormat")).toString();

    QLocale locale = QLocale::system();
    QString text;
    if (format.isEmpty()) {
        text = locale.toString(dateTime.date(), QLocale::FormatType::ShortFormat);
    } else {
        text = locale.toString(dateTime, format);
    }

    // insert the current date
    c.insertText(text);
}

void MainWindow::on_actionShow_Todo_List_triggered() { openTodoDialog(); }

/**
 * @brief Exports the current note as PDF (Markdown)
 */
void MainWindow::on_action_Export_note_as_PDF_markdown_triggered() {
    _exportPrintManager->on_action_Export_note_as_PDF_markdown_triggered();
}

/**
 * @brief Exports the current note as PDF (text)
 */
void MainWindow::on_action_Export_note_as_PDF_text_triggered() {
    _exportPrintManager->on_action_Export_note_as_PDF_text_triggered();
}

QTextDocument *MainWindow::getDocumentForPreviewExport() {
    return _exportPrintManager->getDocumentForPreviewExport();
}

/**
 * @brief Prints the current note (Markdown)
 */
void MainWindow::on_action_Print_note_markdown_triggered() {
    _exportPrintManager->on_action_Print_note_markdown_triggered();
}

/**
 * @brief Prints the current note (text)
 */
void MainWindow::on_action_Print_note_text_triggered() {
    _exportPrintManager->on_action_Print_note_text_triggered();
}

/**
 * @brief Inserts a chosen image at the current cursor position in the note text
 * edit
 */
void MainWindow::on_actionInsert_image_triggered() {
    auto *dialog = new ImageDialog(this);
    const int ret = dialog->exec();

    if (ret == QDialog::Accepted) {
        QString title = dialog->getImageTitle();

        if (dialog->isDisableCopying()) {
            QString pathOrUrl = dialog->getFilePathOrUrl();
            auto url = QUrl(pathOrUrl);

            if (!url.isValid()) {
                return;
            }

            if (url.scheme() == QStringLiteral("file")) {
                pathOrUrl = url.toLocalFile();
            }

            if (!url.scheme().startsWith(QStringLiteral("http"))) {
                pathOrUrl = currentNote.relativeFilePath(pathOrUrl);
            }

#ifdef Q_OS_WIN32
            // make sure a local path on a different drive really works
            if (Utils::Misc::fileExists(pathOrUrl)) {
                pathOrUrl = QUrl::toPercentEncoding(pathOrUrl).prepend("file:///");
            }
#endif

            // title must not be empty
            if (title.isEmpty()) {
                title = QStringLiteral("img");
            }

            insertNoteText(QStringLiteral("![") + title + QStringLiteral("](") + pathOrUrl +
                           QStringLiteral(")"));
        } else {
            QFile *file = dialog->getImageFile();

            if (file->size() > 0) {
                insertMedia(file, title);
            }
        }
    }

    delete (dialog);
}

/**
 * Inserts a media file into the current note
 */
bool MainWindow::insertMedia(QFile *file, QString title) {
    QString text = currentNote.getInsertMediaMarkdown(file, true, false, std::move(title));

    if (!text.isEmpty()) {
        ScriptingService *scriptingService = ScriptingService::instance();
        // attempts to ask a script for another Markdown text
        text = scriptingService->callInsertMediaHook(file, text);
        qDebug() << __func__ << " - 'text': " << text;

        insertNoteText(text);

        return true;
    }

    return false;
}

void MainWindow::insertNoteText(const QString &text) {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    // if we try to insert media in the first line of the note (aka.
    // note name) move the cursor to the last line
    if (c.block() == textEdit->document()->firstBlock()) {
        c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        textEdit->setTextCursor(c);
    }

    // insert the image link
    c.insertText(text);
}

/**
 * Inserts text as a file attachment into the current note
 */
bool MainWindow::insertTextAsAttachment(const QString &text) {
    if (text.isEmpty()) {
        return false;
    }

    // Select the file extension
    QStringList fileExtensions;
    fileExtensions << QStringLiteral("txt") << QStringLiteral("json") << QStringLiteral("xml")
                   << QStringLiteral("log") << QStringLiteral("csv") << QStringLiteral("md")
                   << QStringLiteral("cpp") << QStringLiteral("nix") << QStringLiteral("html")
                   << QStringLiteral("yaml") << QStringLiteral("sql") << QStringLiteral("py")
                   << QStringLiteral("js") << QStringLiteral("ini");

    QString extension = Utils::Misc::detectFileFormat(text);
    int index = fileExtensions.indexOf(extension);

    bool ok;
    QString fileExtension =
        QInputDialog::getItem(this, tr("File extension"), tr("Extension of file attachment:"),
                              fileExtensions, index, true, &ok);

    if (!ok) {
        return false;
    }

    QString fileBaseName = QInputDialog::getText(
        this, tr("File base name"),
        tr("Base name of file attachment:") + QStringLiteral("<br><i>") +
            tr("Leave empty for automatic name", "Leave the base name empty for automatic name") +
            QStringLiteral("</i>"),
        QLineEdit::Normal, QString(), &ok);

    if (!ok) {
        return false;
    }

    QString templateName = QDir::tempPath() + QDir::separator() + QStringLiteral("text-XXXXXX");

    if (!fileExtension.isEmpty()) {
        templateName += QStringLiteral(".") + fileExtension;
    }

    // Create a temporary file for the attachment
    auto *tempFile = new QTemporaryFile(templateName);

    if (!tempFile->open()) {
        showStatusBarMessage(tr("Temporary file can't be opened"), QStringLiteral("❌"), 3000);

        return false;
    }

    // write file data to the temporary file
    tempFile->write(text.toUtf8());
    tempFile->flush();
    tempFile->close();

    // We need a reference to tempFile, or else it will be gone before inserted
    auto *file = new QFile(tempFile->fileName());

    // Determine the filename for the attachment
    QString fileName;
    if (fileBaseName.isEmpty()) {
        auto currentDate = QDateTime::currentDateTime();
        fileName = QStringLiteral("file-") +
                   currentDate.toString(QStringLiteral("yyyy-MM-dd-HH'h'mm's'ss"));
    } else {
        fileName = fileBaseName;
    }

    // If there is a file extension, append it to the filename
    if (!fileExtension.isEmpty()) {
        fileName += QStringLiteral(".") + fileExtension;
    }

    bool result = insertAttachment(file, fileName, fileName);

    if (result) {
        showStatusBarMessage(tr("Inserted text as text attachment file"), QStringLiteral("📄"),
                             3000);
    }

    // For some reason the temp file on disk will not be removed automatically
    // without deleting the pointer manually
    delete tempFile;

    return result;
}

/**
 * Inserts a file attachment into the current note
 */
bool MainWindow::insertAttachment(QFile *file, const QString &title, const QString &fileName) {
    QString text = currentNote.getInsertAttachmentMarkdown(file, title, false, fileName);

    if (!text.isEmpty()) {
        ScriptingService *scriptingService = ScriptingService::instance();
        // attempts to ask a script for another Markdown text
        text = scriptingService->callInsertAttachmentHook(file, text);
        qDebug() << __func__ << " - 'text': " << text;

        QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
        QTextCursor c = textEdit->textCursor();

        // if we try to insert the attachment in the first line of the note
        // (aka. note name) move the cursor to the last line
        if (c.block() == textEdit->document()->firstBlock()) {
            c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
            textEdit->setTextCursor(c);
        }

        // add a space if we are not at the start of a line or if there is no
        // space in front of the current cursor position
        c.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
        if (!c.atBlockStart() && c.selectedText() != QStringLiteral(" ")) {
            text = QStringLiteral(" ") + text;
        }

        // insert the attachment link
        c = textEdit->textCursor();
        c.insertText(text);

        return true;
    }

    return false;
}

/**
 * @brief Opens a browser with the changelog page
 */
void MainWindow::on_actionShow_changelog_triggered() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://www.qownnotes.org/changelog.html")));
}

void MainWindow::on_action_Find_text_in_note_triggered() {
    _searchFilterManager->on_action_Find_text_in_note_triggered();
}

/**
 * Asks the user for a password and encrypts the note text with it
 */
void MainWindow::on_action_Encrypt_note_triggered() {
    _noteEncryptionManager->on_action_Encrypt_note_triggered();
}

/**
 * Enables or disables the encrypt note buttons
 */
void MainWindow::updateNoteEncryptionUI() { _noteEncryptionManager->updateNoteEncryptionUI(); }

/**
 * Attempt to decrypt note text
 */
void MainWindow::on_actionDecrypt_note_triggered() {
    _noteEncryptionManager->on_actionDecrypt_note_triggered();
}

/**
 * Lets the user edit an encrypted note text in a 2nd text edit
 */
void MainWindow::on_actionEdit_encrypted_note_triggered() {
    _noteEncryptionManager->on_actionEdit_encrypted_note_triggered();
}

void MainWindow::editEncryptedNoteAsync() { _noteEncryptionManager->editEncryptedNoteAsync(); }

void MainWindow::editEncryptedNote() { _noteEncryptionManager->editEncryptedNote(); }

/**
 * Puts the encrypted text back to the note text edit
 */
void MainWindow::on_encryptedNoteTextEdit_textChanged() {
    _noteEncryptionManager->on_encryptedNoteTextEdit_textChanged();
}

/**
 * Opens the current note in an external editor
 */
void MainWindow::on_action_Open_note_in_external_editor_triggered() {
    SettingsService settings;
    const QString externalEditorPath =
        settings.value(QStringLiteral("externalEditorPath")).toString();

    // use the default editor if no other editor was set
    if (externalEditorPath.isEmpty()) {
        const QUrl url = currentNote.fullNoteFileUrl();
        qDebug() << __func__ << " - 'url': " << url;

        // open note file in default application for the type of file
        QDesktopServices::openUrl(url);
    } else {
        const QString path = currentNote.fullNoteFilePath();

        qDebug() << __func__ << " - 'externalEditorPath': " << externalEditorPath;
        qDebug() << __func__ << " - 'path': " << path;

        // open note file in external editor
        Utils::Misc::openFilesWithApplication(externalEditorPath, QStringList({path}));
    }
}

/**
 * Exports the current note as Markdown file
 */
void MainWindow::on_action_Export_note_as_markdown_triggered() {
    _noteEncryptionManager->on_action_Export_note_as_markdown_triggered();
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("dialog/") + objectName());
}

void MainWindow::on_actionGet_invloved_triggered() {
    QDesktopServices::openUrl(QUrl("https://www.qownnotes.org/contributing/get-involved.html"));
}

/**
 * Sets a note bookmark on bookmark slot 0..9
 */
void MainWindow::storeNoteBookmark(int slot) {
    // return if note text edit doesn't have the focus
    if (!ui->noteTextEdit->hasFocus()) {
        return;
    }

    NoteHistoryItem item = NoteHistoryItem(&currentNote, ui->noteTextEdit);
    noteBookmarks[slot] = item;

    SettingsService().setValue(QStringLiteral("NoteBookmark%1").arg(slot),
                               QVariant::fromValue(item));

    showStatusBarMessage(tr("Bookmarked note position at slot %1").arg(QString::number(slot)),
                         QStringLiteral("🔖"), 3000);

    // Update the bookmark display in the line number area
    updateNoteBookmarkDisplay();
}

/**
 * Updates the bookmark markers display in the line number area
 */
void MainWindow::updateNoteBookmarkDisplay() {
    // Build a hash map of bookmark slot -> line number for the current note
    QHash<int, int> bookmarkLines;

    for (auto it = noteBookmarks.constBegin(); it != noteBookmarks.constEnd(); ++it) {
        const NoteHistoryItem &item = it.value();

        // Only show bookmarks for the current note
        if (item.getNoteName() == currentNote.getName() &&
            item.getNoteSubFolderPathData() == currentNote.noteSubFolderPathData()) {
            // Convert cursor position to line number
            QTextCursor cursor(ui->noteTextEdit->document());
            cursor.setPosition(item.getCursorPosition());
            int lineNumber = cursor.blockNumber() + 1;    // Line numbers are 1-based

            bookmarkLines[it.key()] = lineNumber;
        }
    }

    ui->noteTextEdit->setBookmarkLines(bookmarkLines);
}

void MainWindow::loadNoteBookmarks() {
    SettingsService settings;

    for (int i = 0; i <= 9; i++) {
        QVariant bookmark = settings.value(QStringLiteral("NoteBookmark%1").arg(i));

        if (bookmark.isValid()) {
            noteBookmarks[i] = bookmark.value<NoteHistoryItem>();
        }
    }
}

/**
 * Loads and jumps to a note bookmark from bookmark slot 0..9
 */
void MainWindow::gotoNoteBookmark(int slot) {
    NoteHistoryItem item = noteBookmarks[slot];

    // check if the note (still) exists
    if (item.getNote().isFetched()) {
        ui->noteTextEdit->setFocus();
        setCurrentNoteFromHistoryItem(item);

        showStatusBarMessage(
            tr("Jumped to bookmark position at slot %1").arg(QString::number(slot)),
            QStringLiteral("🔖"), 3000);
    }
}

/**
 * Inserts a code block at the current cursor position
 */
void MainWindow::on_actionInsert_code_block_triggered() { activeNoteTextEdit()->insertCodeBlock(); }

void MainWindow::on_actionNext_note_triggered() { gotoNextNote(); }

/**
 * Jumps to the next visible note
 */
void MainWindow::gotoNextNote() {
    auto *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    QApplication::postEvent(ui->noteTreeWidget, event);
}

/**
 * Activate the context menu in the currently focused widget
 */
void MainWindow::activateContextMenu() {
    auto *event = new QContextMenuEvent(QContextMenuEvent::Keyboard, QPoint(), QPoint());
    QApplication::postEvent(focusWidget(), event);
}

void MainWindow::on_actionPrevious_Note_triggered() { gotoPreviousNote(); }

/**
 * Jumps to the previous visible note
 */
void MainWindow::gotoPreviousNote() {
    auto *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    QApplication::postEvent(ui->noteTreeWidget, event);
}

void MainWindow::on_actionToggle_distraction_free_mode_triggered() {
    _distractionFreeManager->on_actionToggle_distraction_free_mode_triggered();
}

/**
 * Tracks an action
 */
void MainWindow::trackAction(QAction *action) {
    if (action == nullptr) {
        return;
    }

    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("action/") +
                                                   action->objectName());

    // Remember the last triggered action (excluding the Redo action itself and "Find action")
    if (action != ui->actionRedo_action && action != ui->actionFind_action) {
        _lastTriggeredAction = action;

        // Update the Redo action text to show which action will be redone
        QString actionText = action->text().remove(QChar('&'));    // Remove mnemonic
        ui->actionRedo_action->setText(tr("Redo action: %1").arg(actionText));
        ui->actionRedo_action->setEnabled(true);
    }
}

void MainWindow::resizeTagTreeWidgetColumnToContents() const {
    _tagManager->resizeTagTreeWidgetColumnToContents();
}

/**
 * Toggles the visibility of the toolbars
 */
void MainWindow::on_actionShow_toolbar_triggered(bool checked) {
    const auto toolbars = findChildren<QToolBar *>();
    for (QToolBar *toolbar : toolbars) {
        toolbar->setVisible(checked);
    }
}

/**
 * Toggles the checked state of the "show toolbar" checkbox in the main menu
 */
void MainWindow::toolbarVisibilityChanged(bool visible) {
    Q_UNUSED(visible)

    const QSignalBlocker blocker(ui->actionShow_toolbar);
    {
        Q_UNUSED(blocker)
        ui->actionShow_toolbar->setChecked(isToolbarVisible());
    }
}

/**
 * Checks if at least one toolbar is visible
 */
bool MainWindow::isToolbarVisible() {
    const auto toolbars = findChildren<QToolBar *>();
    for (QToolBar *toolbar : toolbars) {
        if (toolbar->isVisible()) {
            return true;
        }
    }

    return false;
}

void MainWindow::dfmEditorWidthActionTriggered(QAction *action) {
    _distractionFreeManager->dfmEditorWidthActionTriggered(action);
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
void MainWindow::dropEvent(QDropEvent *e) { handleInsertingFromMimeData(e->mimeData()); }

/**
 * Handles the inserting of media files and notes from a mime data, for example
 * produced by a drop event or a paste action
 */
void MainWindow::handleInsertingFromMimeData(const QMimeData *mimeData) {
    //    qDebug() << __func__ << " - 'mimeData->hasText()': " << mimeData->hasText();
    //    qDebug() << __func__ << " - 'mimeData->hasHtml()': " << mimeData->hasHtml();
    //    qDebug() << __func__ << " - 'mimeData->hasImage()': " << mimeData->hasImage();
    //    qDebug() << __func__ << " - 'mimeData->hasUrls()': " << mimeData->hasUrls();

    // prevent pasting if note editor is in read-only mode
    if (ui->noteTextEdit->isReadOnly()) {
        return;
    }

    // check if a QML wants to set the inserted text
    if (mimeData->hasText() || mimeData->hasHtml()) {
        ScriptingService *scriptingService = ScriptingService::instance();
        QString text = scriptingService->callInsertingFromMimeDataHook(mimeData);

        if (!text.isEmpty()) {
            QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
            QTextCursor c = textEdit->textCursor();

            // insert text from QML
            c.insertText(text);

            return;
        }
    }

    if (mimeData->hasUrls()) {
        int successCount = 0;
        int failureCount = 0;
        int skipCount = 0;

        const auto urls = mimeData->urls();
        for (const QUrl &url : urls) {
            const QString path(url.toLocalFile());
            const QFileInfo fileInfo(path);
            qDebug() << __func__ << " - 'path': " << path;

            if (fileInfo.isReadable()) {
                auto *file = new QFile(path);

                // only allow Markdown and text files to be copied as note
                if (isValidNoteFile(file)) {
                    const NoteSubFolder noteSubFolder = NoteSubFolder::activeNoteSubFolder();
                    const QString noteSubFolderPath = noteSubFolder.fullPath();

                    // copy file to notes path
                    const bool success =
                        file->copy(noteSubFolderPath + QDir::separator() + fileInfo.fileName());

                    if (success) {
                        successCount++;
                    } else {
                        failureCount++;
                    }
                    // only allow image files to be inserted as image
                } else if (isValidMediaFile(file)) {
                    showStatusBarMessage(tr("Inserting image"), QStringLiteral("🖼️"), 0);

                    // insert the image
                    insertMedia(file);

                    showStatusBarMessage(tr("Done inserting image"), QStringLiteral("🖼️"), 3000);
                } else {
                    showStatusBarMessage(tr("Inserting attachment"), QStringLiteral("🖼️"), 0);

                    // inserting the attachment
                    insertAttachment(file);

                    showStatusBarMessage(tr("Done inserting attachment"), QStringLiteral("📥️"),
                                         3000);
                }

                delete file;
            } else {
                skipCount++;
            }
        }

        QString message;
        if (successCount > 0) {
            message += tr("Copied %n note(s) to %1", "", successCount).arg(notesPath);
            on_action_Reload_note_folder_triggered();
        }

        if (failureCount > 0) {
            if (!message.isEmpty()) {
                message += QStringLiteral(", ");
            }

            message +=
                tr("Failed to copy %n note(s) (most likely already existing)", "", failureCount);
        }

        if (skipCount > 0) {
            if (!message.isEmpty()) {
                message += QStringLiteral(", ");
            }

            message +=
                tr("Skipped copying of %n note(s) "
                   "(no Markdown or text file or not readable)",
                   "", skipCount);
        }

        if (!message.isEmpty()) {
            showStatusBarMessage(message, QStringLiteral("⤵️️"), 5000);
        }
    } else if (mimeData->hasImage()) {
        // get the image from mime data
        QImage image = mimeData->imageData().value<QImage>();

        if (!image.isNull()) {
            showStatusBarMessage(tr("Saving temporary image"), QStringLiteral("🖼️"), 0);

            QTemporaryFile tempFile(QDir::tempPath() + QDir::separator() +
                                    QStringLiteral("qownnotes-media-XXXXXX.png"));

            if (tempFile.open()) {
                // save temporary png image
                image.save(tempFile.fileName(), "PNG");

                // insert media into note
                auto *file = new QFile(tempFile.fileName());

                showStatusBarMessage(tr("Inserting image"), QStringLiteral("🖼️"), 0);
                insertMedia(file);
                delete file;

                showStatusBarMessage(tr("Done inserting image"), QStringLiteral("🖼️"), 3000);
            } else {
                showStatusBarMessage(tr("Temporary file can't be opened"), QStringLiteral("❌️"),
                                     3000);
            }
        }
    } else if (mimeData->hasText() || mimeData->hasHtml()) {
        const auto textEdit = activeNoteTextEdit();
        auto rect = textEdit->cursorRect(textEdit->textCursor());
        //    const auto pos = textEdit->cursor().pos();
        const QPoint globalPos = textEdit->mapToGlobal(rect.bottomRight());
        QMenu menu;

        // We need to fetch the text and html from the mime data here, because the mimeData object
        // may not be available anymore after the menu was closed and accessing it may cause a crash
        const auto text = mimeData->text();
        const auto html = mimeData->html();

        QAction *htmlAction = menu.addAction(tr("Paste &HTML as Markdown"));
        QAction *textAttachmentAction = menu.addAction(tr("Paste as &text file attachment"));
        QAction *selectedItem = menu.exec(globalPos);

        if (selectedItem == htmlAction) {
            insertHtmlAsMarkdownIntoCurrentNote(html);
        } else if (selectedItem == textAttachmentAction) {
            // Insert text as an attachment file
            insertTextAsAttachment(text);
        }
    }
}

/**
 * Inserts html as Markdown in the current note
 * Images are also downloaded
 */
void MainWindow::insertHtmlAsMarkdownIntoCurrentNote(QString html) {
    // convert html tags to Markdown
    html = Utils::Misc::htmlToMarkdown(std::move(html));

    // match image tags
    static const QRegularExpression re(QStringLiteral("<img.+?src=[\"'](.+?)[\"'].*?>"),
                                       QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(html);

    // find, download locally and replace all images
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        const QString imageTag = match.captured(0);
        const QString imageUrlText = match.captured(1).trimmed();
        // try to import a media file into the current note
        QString markdownCode = currentNote.importMediaFromDataUrl(imageUrlText);

        // if no inline-image was detected try to download the url
        if (markdownCode.isEmpty()) {
            const QUrl imageUrl = QUrl(imageUrlText);

            qDebug() << __func__ << " - 'imageUrl': " << imageUrl;

            if (!imageUrl.isValid()) {
                continue;
            }

            showStatusBarMessage(tr("Downloading %1").arg(imageUrl.toString()), QStringLiteral("⬇️️"),
                                 0);

            // download the image and get the media Markdown code for it
            markdownCode = currentNote.downloadUrlToMedia(imageUrl);
        }

        if (!markdownCode.isEmpty()) {
            // replace the image tag with Markdown code
            html.replace(imageTag, markdownCode);
        }
    }

    showStatusBarMessage(tr("Downloading images finished"), QStringLiteral("🖼️"), 3000);

    // remove all html tags
    static const QRegularExpression tagRE(QStringLiteral("<.+?>"));
    html.remove(tagRE);

    // unescape some html special characters
    html = Utils::Misc::unescapeHtml(std::move(html)).trimmed();

    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    c.insertText(html);
}

void MainWindow::resetBrokenTagNotesLinkFlag() {
    if (_brokenTagNoteLinksRemoved) _brokenTagNoteLinksRemoved = false;
}

QString MainWindow::getWorkspaceUuid(const QString &workspaceName) {
    return _workspaceManager->getWorkspaceUuid(workspaceName);
}

/**
 * Evaluates if file is a media file
 */
bool MainWindow::isValidMediaFile(QFile *file) {
    const QStringList mediaExtensions = QStringList({"jpg", "png", "gif", "webp"});
    const QFileInfo fileInfo(file->fileName());
    const QString extension = fileInfo.suffix();
    return mediaExtensions.contains(extension, Qt::CaseInsensitive);
}

/**
 * Evaluates if file is a note file
 */
bool MainWindow::isValidNoteFile(QFile *file) {
    auto mediaExtensions = Note::noteFileExtensionList();
    const QFileInfo fileInfo(file->fileName());
    const QString extension = fileInfo.suffix();
    return mediaExtensions.contains(extension, Qt::CaseInsensitive);
}

void MainWindow::on_actionPaste_image_triggered() { pasteMediaIntoNote(); }

/**
 * Handles the pasting of media into notes
 */
void MainWindow::pasteMediaIntoNote() {
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData(QClipboard::Clipboard);
    handleInsertingFromMimeData(mimeData);
}

void MainWindow::on_actionShow_note_in_file_manager_triggered() {
    Utils::Misc::openFolderSelect(currentNote.fullNoteFilePath());
}

/**
 * Attempts to undo the formatting on a selected string
 *
 * @param formatter
 * @return
 */
bool MainWindow::undoFormatting(const QString &formatter) {
    auto *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    const QString selectedText = c.selectedText();
    const int formatterLength = formatter.length();
    const int selectionStart = c.selectionStart();
    const int selectionEnd = c.selectionEnd();

    c.setPosition(selectionStart - formatterLength);
    c.setPosition(selectionEnd + formatterLength, QTextCursor::KeepAnchor);
    const QString selectedTextWithFormatter = c.selectedText();

    // if the formatter characters were found we remove them
    if (selectedTextWithFormatter.startsWith(formatter) &&
        selectedTextWithFormatter.endsWith(formatter)) {
        c.insertText(selectedText);
        return true;
    }

    return false;
}

/**
 * Applies a formatter to a selected string
 *
 * @param formatter
 */
void MainWindow::applyFormatter(const QString &formatter) {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    QString selectedText = c.selectedText();

    // Check if selected text ends with a line break (ParagraphSeparator = U+2029)
    bool endsWithLineBreak = false;
    if (!selectedText.isEmpty() && selectedText.endsWith(QChar::ParagraphSeparator)) {
        endsWithLineBreak = true;
        selectedText.chop(1);    // remove the trailing paragraph separator
    }

    // First try to undo an existing formatting
    if (undoFormatting(formatter)) {
        return;
    }

    if (selectedText.isEmpty()) {
        c.insertText(formatter.repeated(2));
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, formatter.length());
        textEdit->setTextCursor(c);
    } else {
        QRegularExpressionMatch match =
            QRegularExpression(QStringLiteral(R"(^(\s*)(.+?)(\s*)$)")).match(selectedText);
        if (match.hasMatch()) {
            QString formattedText =
                match.captured(1) + formatter + match.captured(2) + formatter + match.captured(3);
            if (endsWithLineBreak) {
                formattedText += QChar::ParagraphSeparator;
            }
            c.insertText(formattedText);
        }
    }
}

/**
 * Inserts a bold block at the current cursor position
 */
void MainWindow::on_actionFormat_text_bold_triggered() { applyFormatter(QStringLiteral("**")); }

/**
 * Inserts a underline block at the current cursor position
 */
void MainWindow::on_actionFormat_text_underline_triggered() {
    applyFormatter(QStringLiteral("__"));
}

/**
 * Inserts an italic block at the current cursor position
 */
void MainWindow::on_actionFormat_text_italic_triggered() { applyFormatter(QStringLiteral("*")); }

/**
 * Increases the note text font size by one
 */
void MainWindow::on_action_Increase_note_text_size_triggered() {
    Q_EMIT activeNoteTextEdit()->zoomIn();
}

/**
 * Decreases the note text font size by one
 */
void MainWindow::on_action_Decrease_note_text_size_triggered() {
    Q_EMIT activeNoteTextEdit()->zoomOut();
}

/**
 * Resets the note text font size
 */
void MainWindow::on_action_Reset_note_text_size_triggered() {
    const int fontSize = ui->noteTextEdit->modifyFontSize(QOwnNotesMarkdownTextEdit::Reset);
    ui->encryptedNoteTextEdit->setStyles();
    showStatusBarMessage(tr("Reset font size to %1 pt",
                            "Will be shown after "
                            "the font size is reset by 'Reset note text size'")
                             .arg(fontSize),
                         QStringLiteral("🔤️"), 3000);

    // Allow to update the preview if the zoom level of the note text edits change
    forceRegenerateNotePreview();
}

/**
 * Sets the note folder from the recent note folder combobox
 */
void MainWindow::on_noteFolderComboBox_currentIndexChanged(int index) {
    const int noteFolderId = ui->noteFolderComboBox->itemData(index).toInt();
    const NoteFolder noteFolder = NoteFolder::fetch(noteFolderId);

    if (noteFolder.isFetched()) {
        if (!noteFolder.localPathExists()) {
            QMessageBox::warning(
                this, tr("Note folder does not exist!"),
                tr("Note folder <b>%1</b> with path <b>%2</b> does not exist anymore!")
                    .arg(noteFolder.getName(), noteFolder.getLocalPath()));
            Utils::Gui::setComboBoxIndexByUserData(ui->noteFolderComboBox,
                                                   NoteFolder::currentNoteFolderId());

            return;
        }
        changeNoteFolder(noteFolderId);
        resetBrokenTagNotesLinkFlag();
    }

    // hide the noteSubFolderDockWidget menu entry if subfolders are
    // not enabled
    QAction *action = findAction(QStringLiteral("togglePanel-noteSubFolderDockWidget"));

    if (action != nullptr) {
        action->setVisible(NoteFolder::isCurrentShowSubfolders());
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
void MainWindow::reloadTagTree() { _tagManager->reloadTagTree(); }

/**
 * Reloads the note sub folder tree
 */
void MainWindow::reloadNoteSubFolderTree() {
    qDebug() << __func__;
    ui->noteSubFolderTreeWidget->reload();
}

/**
 * Populates the tag tree recursively with its tags
 */
void MainWindow::buildTagTreeForParentItem(QTreeWidgetItem *parent, bool topLevel) {
    _tagManager->buildTagTreeForParentItem(parent, topLevel);
}

/**
 * Ads a tag to the tag tree widget
 */
QTreeWidgetItem *MainWindow::addTagToTagTreeWidget(QTreeWidgetItem *parent, const TagHeader &tag) {
    return _tagManager->addTagToTagTreeWidget(parent, tag);
}

/**
 * Creates a new tag
 */
void MainWindow::on_tagLineEdit_returnPressed() { _tagManager->on_tagLineEdit_returnPressed(); }

/**
 * Filters tags in the tag tree widget
 */
void MainWindow::on_tagLineEdit_textChanged(const QString &arg1) {
    _tagManager->on_tagLineEdit_textChanged(arg1);
}

/**
 * Shows or hides everything for the note tags
 */
void MainWindow::setupTags() { _tagManager->setupTags(); }

/**
 * Shows or hides everything for the note sub folders
 */
void MainWindow::setupNoteSubFolders() {
    const bool showSubfolders = NoteFolder::isCurrentShowSubfolders();

    // handle the visibility of the note subfolder panel
    handleNoteSubFolderVisibility();

    if (showSubfolders) {
        reloadNoteSubFolderTree();
    }

    // filter the notes again
    filterNotes(false);
}

/**
 * Hides the note tag add button and shows the text edit
 */
void MainWindow::on_newNoteTagButton_clicked() { _tagManager->on_newNoteTagButton_clicked(); }

/**
 * Links a note to the tag entered after pressing return
 * in the note tag line edit
 */
void MainWindow::on_newNoteTagLineEdit_returnPressed() {
    _tagManager->on_newNoteTagLineEdit_returnPressed();
}

/**
 * Links a tag to the current note (or all selected notes)
 *
 * @param tagName
 */
void MainWindow::linkTagNameToCurrentNote(const QString &tagName, bool linkToSelectedNotes) {
    _tagManager->linkTagNameToCurrentNote(tagName, linkToSelectedNotes);
}

void MainWindow::changeEvent(QEvent *event) {
    if (event->type() == QEvent::WindowStateChange) {
        QString windowStateString;
        switch (windowState()) {
            case Qt::WindowMinimized:
                windowStateString = "minimized";
                break;
            case Qt::WindowMaximized:
                windowStateString = "maximized";
                break;
            case Qt::WindowFullScreen:
                windowStateString = "fullscreen";
                break;
            case Qt::WindowActive:
                windowStateString = "active";
                break;
            default:
                windowStateString = "nostate";
                break;
        }

        ScriptingService::instance()->callWindowStateChangeHook(windowStateString);
    }

    QMainWindow::changeEvent(event);
}

/**
 * Adds or removes a tag from the note text if defined via scripting engine
 *
 * @param note
 * @param tagName
 * @param doRemove
 * @param triggerPostMethods
 */
void MainWindow::handleScriptingNoteTagging(Note note, const Tag &tag, bool doRemove,
                                            bool triggerPostMethods) {
    _tagManager->handleScriptingNoteTagging(std::move(note), tag, doRemove, triggerPostMethods);
}

/**
 * Takes care that the tags are synced from the notes to the internal db
 */
void MainWindow::handleScriptingNotesTagUpdating() {
    _tagManager->handleScriptingNotesTagUpdating();
}

/**
 * Takes care that a tag is renamed in all notes
 *
 * @param oldTagName
 * @param newTagName
 */
void MainWindow::handleScriptingNotesTagRenaming(const Tag &tag, const QString &newTagName) {
    _tagManager->handleScriptingNotesTagRenaming(tag, newTagName);
}

/**
 * Takes care that a tag is removed from all notes
 *
 * @param tagName
 */
void MainWindow::handleScriptingNotesTagRemoving(const Tag &tag, bool forBulkOperation) {
    _tagManager->handleScriptingNotesTagRemoving(tag, forBulkOperation);
}

/**
 * Hides the note tag line edit after editing
 */
void MainWindow::on_newNoteTagLineEdit_editingFinished() {
    _tagManager->on_newNoteTagLineEdit_editingFinished();
}

/**
 * Reloads the note tag buttons for the current note (or the selected notes)
 */
void MainWindow::reloadCurrentNoteTags() { _tagManager->reloadCurrentNoteTags(); }

/**
 * Highlights the tags of the current note in the tag tree
 */
void MainWindow::highlightCurrentNoteTagsInTagTree() {
    _tagManager->highlightCurrentNoteTagsInTagTree();
}

/**
 * Removes a note tag link
 */
void MainWindow::removeNoteTagClicked() { _tagManager->removeNoteTagClicked(); }

int MainWindow::getSelectedNotesCount() const { return _noteTreeManager->getSelectedNotesCount(); }

/**
 * Allows the user to add a tag to the current note
 */
void MainWindow::on_action_new_tag_triggered() { _tagManager->on_action_new_tag_triggered(); }

/**
 * Reloads the current note folder
 */
void MainWindow::on_action_Reload_note_folder_triggered() {
    // force build and load
    buildNotesIndexAndLoadNoteDirectoryList(true, true);
    currentNote.refetch();
    setNoteTextFromNote(&currentNote, false, false, true);
}

/**
 * Stores the tag after it was edited
 */
void MainWindow::on_tagTreeWidget_itemChanged(QTreeWidgetItem *item, int column) {
    _tagManager->on_tagTreeWidget_itemChanged(item, column);
}

/**
 * Sets a new active tag
 */
void MainWindow::on_tagTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                     QTreeWidgetItem *previous) {
    _tagManager->on_tagTreeWidget_currentItemChanged(current, previous);
}

/**
 * Jumps to a tag in the tag tree
 *
 * @param tagId
 * @return
 */
bool MainWindow::jumpToTag(int tagId) { return _tagManager->jumpToTag(tagId); }

/**
 * Triggers filtering when multiple tags are selected
 */
void MainWindow::on_tagTreeWidget_itemSelectionChanged() {
    _tagManager->on_tagTreeWidget_itemSelectionChanged();
}

/**
 * Creates a context menu for the tag tree widget
 */
void MainWindow::on_tagTreeWidget_customContextMenuRequested(const QPoint pos) {
    _tagManager->on_tagTreeWidget_customContextMenuRequested(pos);
}

/**
 * Assigns and stores a color to a tag from the tag tree widget
 *
 * @param item
 */
void MainWindow::assignColorToTagItem(QTreeWidgetItem *item) {
    _tagManager->assignColorToTagItem(item);
}

/**
 * Assigns and stores a color to all selected tags from the tag tree widget
 */
void MainWindow::assignColorToSelectedTagItems() { _tagManager->assignColorToSelectedTagItems(); }

/**
 * Disables a color of a tag from the tag tree widget
 *
 * @param item
 */
void MainWindow::disableColorOfTagItem(QTreeWidgetItem *item) {
    _tagManager->disableColorOfTagItem(item);
}

/**
 * Populates a tag menu tree for moving tags
 */
void MainWindow::buildTagMoveMenuTree(QMenu *parentMenu, int parentTagId) {
    _tagManager->buildTagMoveMenuTree(parentMenu, parentTagId);
}

/**
 * Populates a tag menu tree for bulk note tagging
 */
void MainWindow::buildBulkNoteTagMenuTree(QMenu *parentMenu, int parentTagId) {
    _tagManager->buildBulkNoteTagMenuTree(parentMenu, parentTagId);
}

/**
 * Moves selected tags to tagId
 */
void MainWindow::moveSelectedTagsToTagId(int tagId) { _tagManager->moveSelectedTagsToTagId(tagId); }

/**
 * Tag selected notes to tagId
 */
void MainWindow::tagSelectedNotesToTagId(int tagId) { _tagManager->tagSelectedNotesToTagId(tagId); }

/**
 * Populates a subfolder menu tree for bulk note moving or copying
 */
void MainWindow::buildBulkNoteSubFolderMenuTree(QMenu *parentMenu, bool doCopy,
                                                int parentNoteSubFolderId) {
    _noteOperationsManager->buildBulkNoteSubFolderMenuTree(parentMenu, doCopy,
                                                           parentNoteSubFolderId);
}

/**
 * Populates a subfolder menu tree for bulk note moving or copying to
 * subfolders of other note folders
 */
void MainWindow::buildBulkNoteFolderSubFolderMenuTree(QMenu *parentMenu, bool doCopy,
                                                      const QString &parentNoteSubFolderPath,
                                                      bool isRoot) {
    _noteOperationsManager->buildBulkNoteFolderSubFolderMenuTree(parentMenu, doCopy,
                                                                 parentNoteSubFolderPath, isRoot);
}

/**
 * Moves selected notes to a note subfolder id
 */
void MainWindow::moveSelectedNotesToNoteSubFolderId(int noteSubFolderId) {
    _noteOperationsManager->moveSelectedNotesToNoteSubFolderId(noteSubFolderId);
}

/**
 * Copies selected notes to a note subfolder id
 */
void MainWindow::copySelectedNotesToNoteSubFolderId(int noteSubFolderId) {
    _noteOperationsManager->copySelectedNotesToNoteSubFolderId(noteSubFolderId);
}

/**
 * Moves selected notes to a note subfolder
 */
void MainWindow::moveSelectedNotesToNoteSubFolder(const NoteSubFolder &noteSubFolder) {
    _noteOperationsManager->moveSelectedNotesToNoteSubFolder(noteSubFolder);
}

/**
 * Sets the visibility of a dock widget by its object name
 */
void MainWindow::setDockWidgetVisible(const QString &objectName, bool visible) {
    auto *dockWidget = findChild<QDockWidget *>(objectName);
    if (dockWidget != nullptr) {
        dockWidget->setVisible(visible);
    }
}

/**
 * Restores the original title bar widgets on all dock widgets
 */
void MainWindow::restoreDockWidgetTitleBars() {
    _taggingDockWidget->setTitleBarWidget(_taggingDockTitleBarWidget);
    _noteSubFolderDockWidget->setTitleBarWidget(_noteSubFolderDockTitleBarWidget);
    _noteSearchDockWidget->setTitleBarWidget(_noteSearchDockTitleBarWidget);
    _noteFolderDockWidget->setTitleBarWidget(_noteFolderDockTitleBarWidget);
    _noteListDockWidget->setTitleBarWidget(_noteListDockTitleBarWidget);
    _noteNavigationDockWidget->setTitleBarWidget(_noteNavigationDockTitleBarWidget);
    _noteTagDockWidget->setTitleBarWidget(_noteTagDockTitleBarWidget);
    _notePreviewDockWidget->setTitleBarWidget(_notePreviewDockTitleBarWidget);
    _noteGraphicsViewDockWidget->setTitleBarWidget(_noteGraphicsViewDockTitleBarWidget);
    _logDockWidget->setTitleBarWidget(_logDockTitleBarWidget);
    _scriptingDockWidget->setTitleBarWidget(_scriptingDockTitleBarWidget);

    if (_noteEditDockWidget != nullptr && _noteEditDockTitleBarWidget != nullptr) {
        _noteEditDockWidget->setTitleBarWidget(_noteEditDockTitleBarWidget);
    }
}

/**
 * Enables the note externally removed check
 */
void MainWindow::enableNoteExternallyRemovedCheck() { _noteExternallyRemovedCheckEnabled = true; }

/**
 * Copies selected notes to a note subfolder
 */
void MainWindow::copySelectedNotesToNoteSubFolder(const NoteSubFolder &noteSubFolder) {
    _noteOperationsManager->copySelectedNotesToNoteSubFolder(noteSubFolder);
}

/**
 * Opens the widget to replace text in the current note
 */
void MainWindow::on_actionReplace_in_current_note_triggered() {
    _searchFilterManager->on_actionReplace_in_current_note_triggered();
}

/**
 * Jumps to the position that was clicked in the navigation widget
 */
void MainWindow::onNavigationWidgetPositionClicked(int position) {
    _navigationManager->onNavigationWidgetPositionClicked(position);
}

void MainWindow::onFileNavigationWidgetPositionClicked(int position) {
    _navigationManager->onFileNavigationWidgetPositionClicked(position);
}

/**
 * Handles renaming of headings from the navigation widget
 * Updates the heading text in the note
 */
void MainWindow::onNavigationWidgetHeadingRenamed(int position, const QString &oldText,
                                                  const QString &newText) {
    _navigationManager->onNavigationWidgetHeadingRenamed(position, oldText, newText);
}

/**
 * Updates backlinks after a heading has been renamed
 * Scans all notes that link to the current note and updates heading references
 * Also checks the current note itself for TOC links (e.g., [text](#old-heading))
 */
void MainWindow::updateBacklinksAfterHeadingRename(const QString &oldHeading,
                                                   const QString &newHeading) {
    _navigationManager->updateBacklinksAfterHeadingRename(oldHeading, newHeading);
}

/**
 * Jumps to the note that was clicked in the backlink widget
 */
void MainWindow::onBacklinkWidgetNoteClicked(int noteId, QString markdown) {
    setCurrentNoteFromNoteId(noteId);
    activeNoteTextEdit()->doSearch(markdown, QPlainTextEditSearchWidget::PlainTextMode);
}

/**
 * Starts a note preview regeneration to resize too large images
 */
void MainWindow::onNoteTextViewResize(QSize size, QSize oldSize) {
    Q_UNUSED(size)
    Q_UNUSED(oldSize)

    // just regenerate the note once a second for performance reasons
    if (!_noteViewIsRegenerated) {
        _noteViewIsRegenerated = true;
        QTimer::singleShot(1000, this, SLOT(regenerateNotePreview()));
    }
}

/**
 * Regenerates the note preview by converting the markdown to html again
 */
void MainWindow::regenerateNotePreview() {
    setNoteTextFromNote(&currentNote, true);
    _noteViewIsRegenerated = false;
}

/**
 * Tries to open a link at the current cursor position or solve an equation
 */
void MainWindow::on_actionAutocomplete_triggered() {
    activeNoteTextEdit()->onAutoCompleteRequested();
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
    showStatusBarMessage(tr("The scripting engine was reloaded"), QStringLiteral("🔧"), 3000);
    forceRegenerateNotePreview();
}

void MainWindow::reloadOpenAiControls() { _aiToolbarManager->reloadOpenAiControls(); }

/**
 * Things to do before the scripting engine will be reloaded
 * Will be invoked by the ScriptingService
 */
void MainWindow::preReloadScriptingEngine() {
    // clear and hide the custom actions
    ui->menuCustom_actions->clear();
    ui->menuCustom_actions->hide();
    _customActionToolbar->clear();
    //    _customActionToolbar->hide();
    _noteTextEditContextMenuActions.clear();
    _noteListContextMenuActions.clear();

    // hide the scripting dock widget and remove all registered labels
    _scriptingDockWidget->hide();
    const auto labels = ui->scriptingScrollArea->findChildren<QLabel *>();
    for (QLabel *label : labels) {
        delete label;
    }
}

void MainWindow::on_actionShow_log_triggered() { _logDockWidget->show(); }

/**
 * Exports the note preview as HTML
 */
void MainWindow::on_actionExport_preview_HTML_triggered() {
    _exportPrintManager->on_actionExport_preview_HTML_triggered();
}

/**
 * Opens the IRC web chat page
 */
void MainWindow::on_actionOpen_IRC_Channel_triggered() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://web.libera.chat/#qownnotes")));
}

/**
 * Opens the Gitter room in a webpage
 */
void MainWindow::on_actionGitter_triggered() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://gitter.im/qownnotes/qownnotes")));
}

/**
 * Adds the current search text to the saved searches
 */
void MainWindow::storeSavedSearch() { _searchFilterManager->storeSavedSearch(); }

/**
 * Initializes the saved searches completer
 */
void MainWindow::initSavedSearchesCompleter() {
    _searchFilterManager->initSavedSearchesCompleter();
}

/**
 * Inserts the note file name as headline
 */
void MainWindow::on_actionInsert_headline_from_note_filename_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    c.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);

    const QString fileName = currentNote.fileBaseName(true);
    const QString text = Note::createNoteHeader(fileName);
    c.insertText(text);
}

/**
 * Toggles the editor soft wrapping
 */
void MainWindow::on_actionUse_softwrap_in_note_editor_toggled(bool arg1) {
    SettingsService settings;
    settings.setValue(QStringLiteral("useSoftWrapInNoteEditor"), arg1);

    // initialize the editor soft wrapping
    initEditorSoftWrap();
}

void MainWindow::on_actionShow_status_bar_triggered(bool checked) {
    ui->statusBar->setVisible(checked);

    const QSignalBlocker blocker(ui->actionShow_status_bar);
    {
        Q_UNUSED(blocker)
        ui->actionShow_status_bar->setChecked(checked);
    }

    SettingsService settings;
    settings.setValue(QStringLiteral("showStatusBar"), checked);
}

void MainWindow::on_noteTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                      QTreeWidgetItem *previous) {
    _noteTreeManager->on_noteTreeWidget_currentItemChanged(current, previous);
}

void MainWindow::openSelectedNotesInTab() { _noteTabManager->openSelectedNotesInTab(); }

void MainWindow::openNoteInTab(const Note &note, bool forceNewTab) {
    _noteTabManager->openNoteInTab(note, forceNewTab);
}

void MainWindow::openCurrentNoteInTab() { _noteTabManager->openCurrentNoteInTab(); }

int MainWindow::getNoteTabIndex(int noteId) const {
    return _noteTabManager->getNoteTabIndex(noteId);
}

void MainWindow::on_noteTreeWidget_customContextMenuRequested(const QPoint pos) {
    _noteTreeManager->on_noteTreeWidget_customContextMenuRequested(pos);
}

void MainWindow::openNotesContextMenu(const QPoint globalPos, bool hasNotes, bool hasFolders) {
    _noteTreeManager->openNotesContextMenu(globalPos, hasNotes, hasFolders);
}

/**
 * Renames a note file if the note was renamed in the note tree widget
 */
void MainWindow::on_noteTreeWidget_itemChanged(QTreeWidgetItem *item, int /*column*/) {
    _noteTreeManager->on_noteTreeWidget_itemChanged(item, 0);
}

void MainWindow::onCurrentSubFolderChanged() {
    if (NoteFolder::isCurrentNoteTreeEnabled()) {
        filterNotesBySearchLineEditText(false);
    } else {
        filterNotes();
    }

    if (isTagsEnabled()) {
        reloadTagTree();
    }
}

void MainWindow::onMultipleSubfoldersSelected() {
    filterNotes();

    if (isTagsEnabled()) {
        reloadTagTree();
    }
}

void MainWindow::clearTagFilteringColumn() { _tagManager->clearTagFilteringColumn(); }

/**
 * Returns true if notes from all note sub folders should be shown
 * @return
 */
bool MainWindow::showNotesFromAllNoteSubFolders() const {
    SettingsService settings;
    return settings
        .value(QStringLiteral("MainWindow/showNotesFromAllNoteSubFolders"),
               _showNotesFromAllNoteSubFolders)
        .toBool();
}

void MainWindow::setShowNotesFromAllNoteSubFolders(bool show) {
    _showNotesFromAllNoteSubFolders = show;
    SettingsService settings;
    settings.setValue(QStringLiteral("MainWindow/showNotesFromAllNoteSubFolders"),
                      _showNotesFromAllNoteSubFolders);
}

NoteSubFolderTree *MainWindow::noteSubFolderTree() { return ui->noteSubFolderTreeWidget; }

QOwnNotesMarkdownTextEdit *MainWindow::noteTextEdit() { return ui->noteTextEdit; }

void MainWindow::refreshNotePreview(bool force) {
    if (force) {
        _noteViewNeedsUpdate = true;
    }

    _noteViewUpdateTimer->start(1);
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
        Q_UNUSED(blocker)

        // search for the text
        Utils::Gui::searchForTextInTreeWidget(ui->noteSubFolderTreeWidget, arg1,
                                              Utils::Gui::TreeWidgetSearchFlag::IntCheck);
    }
}

/**
 * Creates a new note subfolder in the current subfolder
 */
bool MainWindow::createNewNoteSubFolder(QString folderName) {
    bool ok = true;

    if (folderName.isEmpty()) {
        folderName = QInputDialog::getText(this, tr("Create a new folder"), tr("Folder name:"),
                                           QLineEdit::Normal, QString(), &ok);
    }

    if (!ok || folderName.isEmpty() || NoteSubFolder::willFolderBeIgnored(folderName, true)) {
        return false;
    }

    NoteSubFolder noteSubFolder = NoteFolder::currentNoteFolder().getActiveNoteSubFolder();
    const QString path = noteSubFolder.fullPath() + QDir::separator() + folderName;
    QDir directory;
    const bool success = directory.mkpath(path);

    // rebuild the index of the note subfolders
    buildNotesIndex();

    if (NoteFolder::isCurrentNoteTreeEnabled()) {
        loadNoteDirectoryList();
    } else {
        // reload note subfolders
        setupNoteSubFolders();
    }

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

void MainWindow::on_actionShare_note_triggered() {
    const QSignalBlocker blocker(ui->actionShare_note);
    Q_UNUSED(blocker)
    ui->actionShare_note->setChecked(currentNote.isShared());

    ShareDialog *dialog = new ShareDialog(currentNote, this);
    dialog->exec();
    OwnCloudService::instance()->unsetShareDialog();
    delete (dialog);

    currentNote.refetch();

    // update the share button
    updateShareButton();
}

/**
 * Toggles the case of the word under the Cursor or the selected text
 */
void MainWindow::on_actionToggle_text_case_triggered() { activeNoteTextEdit()->toggleCase(); }

/**
 * Opens the Markdown Cheatsheet webpage
 */
void MainWindow::on_actionMarkdown_cheatsheet_triggered() {
    QDesktopServices::openUrl(QUrl("https://www.qownnotes.org/getting-started/markdown.html"));
}

/**
 * Strikes out the selected text
 */
void MainWindow::on_actionStrike_out_text_triggered() { applyFormatter(QStringLiteral("~~")); }

/**
 * Initializes the shortcuts for the actions
 *
 * @param setDefaultShortcut
 */
void MainWindow::initShortcuts() {
    const QList<QMenu *> menus = menuList();
    SettingsService settings;

    // we also have to clear the shortcuts directly, just removing the
    // objects didn't remove the shortcut
    for (QShortcut *menuShortcut : Utils::asConst(_menuShortcuts)) {
        menuShortcut->setKey(QKeySequence());
    }

    // remove all menu shortcuts to create new ones
    _menuShortcuts.clear();

#ifndef Q_OS_MAC
    const bool menuBarIsVisible = !ui->menuBar->isHidden();
    qDebug() << __func__ << " - 'menuBarIsVisible': " << menuBarIsVisible;
#endif

    // loop through all menus
    for (QMenu *menu : menus) {
        // loop through all actions of the menu
        const auto actions = menu->actions();
        for (QAction *action : actions) {
            // we don't need empty objects
            if (action->objectName().isEmpty()) {
                continue;
            }

            QString oldShortcut = action->shortcut().toString();

#ifdef Q_OS_MAC
            // #1222, replace Option key by Ctrl key on macOS to prevent
            // blocking of accent characters when writing text
            oldShortcut.replace(QStringLiteral("Alt+"), QStringLiteral("Meta+"));
#endif

            const QString &key = QStringLiteral("Shortcuts/MainWindow-") + action->objectName();
            const bool settingFound = settings.contains(key);

            // try to load a key sequence from the settings
            auto shortcut = QKeySequence(settingFound ? settings.value(key).toString() : "");

            // do we can this method the first time?
            if (!_isDefaultShortcutInitialized) {
                // set the default shortcut
                action->setData(oldShortcut);

                // if there was a shortcut set use the new shortcut
                if (!shortcut.isEmpty()) {
                    action->setShortcut(shortcut);
                }
            } else if (!settingFound) {
                // set to the default shortcut if no shortcut setting was found
                action->setShortcut(action->data().toString());
            } else if (shortcut.isEmpty()) {
                // disable shortcut if setting is empty
                action->setShortcut(QKeySequence());
            } else {
                // else store the new shortcut
                action->setShortcut(shortcut);
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

                auto *shortcutItem = new QShortcut(shortcut, this);
                connect(shortcutItem, &QShortcut::activated, action, &QAction::trigger);
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
        Q_UNUSED(blocker)
        ui->actionShow_menu_bar->setChecked(checked);
    }

    SettingsService settings;
    settings.setValue(QStringLiteral("showMenuBar"), checked);

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
    _noteOperationsManager->on_actionSplit_note_at_cursor_position_triggered();
}

/**
 * Adds a custom action as menu item and button
 */
void MainWindow::addCustomAction(const QString &identifier, const QString &menuText,
                                 const QString &buttonText, const QString &icon,
                                 bool useInNoteEditContextMenu, bool hideButtonInToolbar,
                                 bool useInNoteListContextMenu) {
    //    ui->menuCustom_actions->show();
    QAction *action = ui->menuCustom_actions->addAction(menuText);
    action->setObjectName(QStringLiteral("customAction_") + identifier);
    action->setData(identifier);

    // restore the shortcut of the custom action
    SettingsService settings;
    QKeySequence shortcut = QKeySequence(
        settings.value(QStringLiteral("Shortcuts/MainWindow-customAction_") + identifier)
            .toString());
    if (!shortcut.isEmpty()) {
        action->setShortcut(shortcut);
    }

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
    if (!hideButtonInToolbar && (!buttonText.isEmpty() || !icon.isEmpty())) {
        //        _customActionToolbar->show();
        _customActionToolbar->addAction(action);
    }

    connect(action, &QAction::triggered, this,
            [this, identifier]() { onCustomActionInvoked(identifier); });

    // add the custom action to the note text edit context menu later
    if (useInNoteEditContextMenu) {
        _noteTextEditContextMenuActions.append(action);
    }

    // add the custom action to the note list context menu later
    if (useInNoteListContextMenu) {
        _noteListContextMenuActions.append(action);
    }
}

/**
 * Adds a label to the scripting dock widget
 */
void MainWindow::addScriptingLabel(const QString &identifier, const QString &text) {
    _scriptingDockWidget->show();
    auto *label = new QLabel(text, _scriptingDockWidget);
    // It seems that if set to true QLabel::linkActivated doesn't trigger anymore!
    label->setOpenExternalLinks(false);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::LinksAccessibleByMouse);
    label->setWordWrap(true);
    label->setObjectName(QStringLiteral("scriptingLabel-") + identifier);

    connect(label, &QLabel::linkActivated, this, [this](const QString &link) {
        // Check for internal links and open them
        if (UrlHandler::isUrlSchemeLocal(QUrl(link))) {
            openLocalUrl(link);
        } else {
            QDesktopServices::openUrl(QUrl(link));
            return;
        }
    });

    ui->scriptingScrollAreaLayout->addWidget(label);
}

/**
 * Sets the text of a label in the scripting dock widget
 */
void MainWindow::setScriptingLabelText(const QString &identifier, const QString &text) {
    auto *label = ui->scriptingScrollArea->findChild<QLabel *>(QStringLiteral("scriptingLabel-") +
                                                               identifier);
    if (label != nullptr) {
        label->setText(text);
    }
}

/**
 * Opens the donation page
 */
void MainWindow::on_actionDonate_triggered() {
    QDesktopServices::openUrl(
        QUrl(QStringLiteral("https://www.qownnotes.org/contributing/donate.html")));
}

/**
 * Jumps to "All notes" in the note subfolder and tag tree widget and triggers
 * a "Find note"
 */
void MainWindow::on_actionFind_notes_in_all_subfolders_triggered() {
    _searchFilterManager->on_actionFind_notes_in_all_subfolders_triggered();
}

/**
 * Sends an event to jump to "All notes" in the note subfolder tree widget
 */
void MainWindow::selectAllNotesInNoteSubFolderTreeWidget() const {
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
    QCoreApplication::postEvent(ui->noteSubFolderTreeWidget, event);
}

/**
 * Sends an event to jump to "All notes" in the tag tree widget
 */
void MainWindow::selectAllNotesInTagTreeWidget() const {
    QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
    QCoreApplication::postEvent(ui->tagTreeWidget, event);
}

void MainWindow::on_actionImport_notes_from_Evernote_triggered() {
    const QSignalBlocker blocker(noteDirectoryWatcher);
    Q_UNUSED(blocker)

    auto dialog = new EvernoteImportDialog(this);
    dialog->exec();

    if (dialog->getImportCount() > 0) {
        // reload the note folder after importing new notes
        buildNotesIndexAndLoadNoteDirectoryList(true, true);
    }

    delete (dialog);
}

/**
 * Shows a dialog to manage stored images
 */
void MainWindow::on_actionManage_stored_images_triggered() {
    delete (_storedImagesDialog);
    _storedImagesDialog = new StoredImagesDialog(this);
    _storedImagesDialog->show();
}

/**
 * Writes text to the note text edit
 *
 * @param text
 */
void MainWindow::writeToNoteTextEdit(const QString &text) {
    // Enable note editing if it is disabled
    if (!Utils::Misc::isNoteEditingAllowed()) {
        on_actionAllow_note_editing_triggered(true);
        ui->actionAllow_note_editing->setChecked(true);
    }

    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    textEdit->insertPlainText(text);
}

/**
 * Returns the text that is selected in the note text edit
 *
 * @return
 */
QString MainWindow::selectedNoteTextEditText() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QString selectedText = textEdit->textCursor().selectedText();

    // transform Unicode line endings
    // this newline character seems to be used in multi-line selections
    const QString newLine = QString::fromUtf8(QByteArray::fromHex("e280a9"));
    selectedText.replace(newLine, QStringLiteral("\n"));

    return selectedText.isNull() ? QString() : selectedText;
}

/**
 * Locks and unlocks the dock widgets
 *
 * @param arg1
 */
void MainWindow::on_actionUnlock_panels_toggled(bool arg1) {
    _workspaceManager->on_actionUnlock_panels_toggled(arg1);
}

void MainWindow::handleDockWidgetLocking(QDockWidget *dockWidget) {
    _workspaceManager->handleDockWidgetLocking(dockWidget);
}

/**
 * Creates a new workspace with asking for its name
 */
void MainWindow::on_actionStore_as_new_workspace_triggered() {
    _workspaceManager->on_actionStore_as_new_workspace_triggered();
}

/**
 * Creates a new workspace with name
 *
 * @param name
 * @return
 */
bool MainWindow::createNewWorkspace(QString name) {
    return _workspaceManager->createNewWorkspace(name);
}

/**
 * Returns the uuid of the current workspace
 *
 * @return
 */
QString MainWindow::currentWorkspaceUuid() { return _workspaceManager->currentWorkspaceUuid(); }

/**
 * Sets the new current workspace when the workspace combo box index has changed
 */
void MainWindow::onWorkspaceComboBoxCurrentIndexChanged(int index) {
    _workspaceManager->onWorkspaceComboBoxCurrentIndexChanged(index);
}

/**
 * Sets the AI backend when the AI backend combo box index has changed
 */
void MainWindow::onAiBackendComboBoxCurrentIndexChanged(int index) {
    _aiToolbarManager->onAiBackendComboBoxCurrentIndexChanged(index);
}

/**
 * Puts items into the AI backend combo box
 */
void MainWindow::generateAiBackendComboBox() { _aiToolbarManager->generateAiBackendComboBox(); }

/**
 * Puts items into the AI model main menu
 */
void MainWindow::generateAiModelMainMenu() { _aiToolbarManager->generateAiModelMainMenu(); }

void MainWindow::aiModelMainMenuSetCurrentItem() {
    _aiToolbarManager->aiModelMainMenuSetCurrentItem();
}

/**
 * Sets the AI model when the AI model combo box index has changed
 */
void MainWindow::onAiModelComboBoxCurrentIndexChanged(int index) {
    _aiToolbarManager->onAiModelComboBoxCurrentIndexChanged(index);
}

void MainWindow::onAiModelGroupChanged(QAction *action) {
    _aiToolbarManager->onAiModelGroupChanged(action);
}

/**
 * Puts items into the AI model combo box
 */
void MainWindow::generateAiModelComboBox() { _aiToolbarManager->generateAiModelComboBox(); }

/**
 * Sets a new current workspace
 */
void MainWindow::setCurrentWorkspace(const QString &uuid) {
    _workspaceManager->setCurrentWorkspace(uuid);
}

/**
 * Stores the current workspace
 */
void MainWindow::storeCurrentWorkspace() { _workspaceManager->storeCurrentWorkspace(); }

/**
 * Restores the current workspace
 */
void MainWindow::restoreCurrentWorkspace() { _workspaceManager->restoreCurrentWorkspace(); }

/**
 * Handles the visibility of the note subfolder panel
 */
void MainWindow::handleNoteSubFolderVisibility() const {
    // turn the subfolder dock widget on or off according to whether the
    // subfolders are enabled or not
    bool showSubfolders = NoteFolder::isCurrentShowSubfolders();
    _noteSubFolderDockWidget->setVisible(showSubfolders && _noteSubFolderDockWidgetVisible);
}

/**
 * Returns the list of workspace uuids
 * @return
 */
QStringList MainWindow::getWorkspaceUuidList() { return _workspaceManager->getWorkspaceUuidList(); }

/**
 * Removes the current workspace
 */
void MainWindow::on_actionRemove_current_workspace_triggered() {
    _workspaceManager->on_actionRemove_current_workspace_triggered();
}

void MainWindow::on_actionRename_current_workspace_triggered() {
    _workspaceManager->on_actionRename_current_workspace_triggered();
}

/**
 * Switch to the previous workspace
 */
void MainWindow::on_actionSwitch_to_previous_workspace_triggered() {
    _workspaceManager->on_actionSwitch_to_previous_workspace_triggered();
}

/**
 * Shows all dock widgets
 */
void MainWindow::on_actionShow_all_panels_triggered() {
    _workspaceManager->on_actionShow_all_panels_triggered();
}

static void loadAllActions(QMenu *menu, QVector<QPair<QString, QAction *>> &outActions) {
    if (!menu) {
        return;
    }

    const auto menuActions = menu->actions();
    QVector<QPair<QString, QAction *>> actions;
    actions.reserve(menuActions.size());

    for (auto action : menuActions) {
        if (auto submenu = action->menu()) {
            loadAllActions(submenu, outActions);
        } else {
            if (!action->text().isEmpty() && !action->objectName().isEmpty() &&
                action->isVisible()) {
                outActions.append({menu->title(), action});
            }
        }
    }
}

/**
 * Opens the find action dialog
 */
void MainWindow::on_actionFind_action_triggered() {
#if 0
    if (_actionDialog == nullptr) {
        _actionDialog = new ActionDialog(ui->menuBar, this);
    } else {
        _actionDialog->refreshUi();
    }

    _actionDialog->show();
    _actionDialog->activateWindow();
    _actionDialog->raise();
#endif
    auto menuBar = this->menuBar();
    const auto menus = menuBar->actions();

    QVector<QPair<QString, QAction *>> actions;
    for (auto subMenu : menus) {
        if (auto menu = subMenu->menu()) {
            loadAllActions(menu, actions);
        }
    }

    if (_commandBar != nullptr) {
        delete _commandBar;
    }

    // We need to instantiate the class every time, otherwise no and then it's not drawn correctly
    _commandBar = new CommandBar(this);

    _commandBar->updateBar(actions);
    _commandBar->setFocus();

    // Keep in mind this call is not synchronous anymore for some reason, so we can't delete the
    // instance
    _commandBar->exec();

    //    QPointer<CommandBar> commandBar = new CommandBar(this);
    //
    //    commandBar->updateBar(actions);
    //    commandBar->setFocus();
    //    QAction *action = commandBar->exec();
    //
    //    delete commandBar;
    //
    //    qDebug() << __func__ << " - 'action': " << action;
}

/**
 * Opens the find action dialog
 */
void MainWindow::on_actionRedo_action_triggered() {
    qDebug() << __func__ << "actionRedo_action: " << ui->actionRedo_action->text();

    // Retrigger the last triggered action if it exists
    if (_lastTriggeredAction != nullptr) {
        qDebug() << "Retriggering action:" << _lastTriggeredAction->objectName()
                 << _lastTriggeredAction->text();
        _lastTriggeredAction->trigger();
    } else {
        qDebug() << "No action to redo";
    }
}

/**
 * Opens the table dialog
 */
void MainWindow::on_actionInsert_table_triggered() {
    auto *dialog = new TableDialog(this);
    dialog->exec();
    delete (dialog);
}

void MainWindow::on_actionInsert_block_quote_triggered() {
    activeNoteTextEdit()->insertBlockQuote();
}

/**
 * Searches for the selected text on the web
 */
void MainWindow::on_actionSearch_text_on_the_web_triggered() {
    auto *textEdit = activeNoteTextEdit();
    QString selectedText = textEdit->textCursor().selectedText().trimmed();

    if (selectedText.isEmpty()) {
        return;
    }

    // handling the case in which the saved engine id
    // has been removed

    SettingsService settings;
    typedef Utils::Misc::SearchEngine SearchEngine;
    int selectedSearchEngineId =
        settings.value(QStringLiteral("SearchEngineId"), Utils::Misc::getDefaultSearchEngineId())
            .toInt();
    QHash<int, SearchEngine> SearchEngines = Utils::Misc::getSearchEnginesHashMap();
    SearchEngine selectedEngine = SearchEngines.value(selectedSearchEngineId);
    QString searchEngineUrl = selectedEngine.searchUrl;
    QUrl url(searchEngineUrl + QUrl::toPercentEncoding(selectedText));
    QDesktopServices::openUrl(url);
}

/**
 * Updates the line number label and the selected navigation item, if required
 */
void MainWindow::noteEditCursorPositionChanged() {
    _navigationManager->noteEditCursorPositionChanged();
}

/**
 * Deletes the current line in the active note text edit
 */
void MainWindow::on_actionDelete_line_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();

    // if the note text edit doesn't have the focus delegate the default
    // shortcut to the widget with the focus
    if (!textEdit->hasFocus()) {
        QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::AltModifier);

        // we need a special fallback for QLineEdit because it seems to ignore
        // our event
        if (dynamic_cast<QLineEdit *>(QApplication::focusWidget()) != nullptr) {
            auto *lineEdit = dynamic_cast<QLineEdit *>(QApplication::focusWidget());
            lineEdit->clear();
        } else {
            QApplication::postEvent(QApplication::focusWidget(), event);
        }

        return;
    }

    QTextCursor cursor = textEdit->textCursor();
    cursor.select(QTextCursor::BlockUnderCursor);
    QString selectedText = cursor.selectedText();

    if (selectedText.isEmpty()) {
        cursor.deletePreviousChar();
    } else {
        // remove the text in the current line
        cursor.removeSelectedText();
    }

    cursor.movePosition(QTextCursor::NextBlock);
    textEdit->setTextCursor(cursor);
}

/**
 * Deletes the current word in the active note text edit
 */
void MainWindow::on_actionDelete_word_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();

    // if the note text edit doesn't have the focus delegate the default
    // shortcut to the widget with the focus
    if (!textEdit->hasFocus()) {
        QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::ControlModifier);

        // we need a special fallback for QLineEdit because it seems to ignore
        // our event
        if (dynamic_cast<QLineEdit *>(QApplication::focusWidget()) != nullptr) {
            auto *lineEdit = dynamic_cast<QLineEdit *>(QApplication::focusWidget());
            lineEdit->cursorWordBackward(true);
            lineEdit->del();
        } else {
            QApplication::postEvent(QApplication::focusWidget(), event);
        }

        return;
    }

    QTextCursor cursor = textEdit->textCursor();

    if (cursor.selectedText().isEmpty()) {
        cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::KeepAnchor);
    }

    cursor.removeSelectedText();
}

/**
 * Opens the current note in a dialog
 */
void MainWindow::on_actionView_note_in_new_window_triggered() {
    auto *dialog = new NoteDialog(this);
    dialog->setNote(currentNote);
    dialog->show();
}

/**
 * Manually stores updated notes to disk
 */
void MainWindow::on_actionSave_modified_notes_triggered() {
    // store updated notes to disk
    storeUpdatedNotesToDisk();
}

/**
 * Sets ascending note sort order
 */
void MainWindow::on_actionAscending_triggered() {
    SettingsService settings;
    settings.setValue(QStringLiteral("notesPanelOrder"), ORDER_ASCENDING);
    ui->noteTreeWidget->sortItems(0, Utils::Gui::toQtOrder(ORDER_ASCENDING));
}

/**
 * Sets descending note sort order
 */
void MainWindow::on_actionDescending_triggered() {
    SettingsService settings;
    settings.setValue(QStringLiteral("notesPanelOrder"), ORDER_DESCENDING);
    ui->noteTreeWidget->sortItems(0, Utils::Gui::toQtOrder(ORDER_DESCENDING));
}

/**
 * Updates the visibility of the note sort order selector
 */
void MainWindow::updateNoteSortOrderSelectorVisibility(bool visible) {
    ui->actionAscending->setVisible(visible);
    ui->actionDescending->setVisible(visible);
    //    ui->sortOrderSeparator->setVisible(visible);
}

/**
 * Commits changes from the current note folder to git
 */
void MainWindow::gitCommitCurrentNoteFolder() { Utils::Git::commitCurrentNoteFolder(); }

/**
 * Shows a git log of the current note
 */
void MainWindow::on_actionShow_note_git_versions_triggered() {
    if (!Utils::Git::isCurrentNoteFolderUseGit()) {
        QMessageBox msgBox(QMessageBox::Information, QStringLiteral("Git support"),
                           tr("Git support is not enabled for the current note folder, "
                              "do you want to enable it in the settings?"),
                           QMessageBox::NoButton, this);
        QPushButton *settingsButton =
            msgBox.addButton(tr("Open &settings"), QMessageBox::AcceptRole);
        msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);
        msgBox.setDefaultButton(settingsButton);
        msgBox.exec();

        if (msgBox.clickedButton() == settingsButton) {
            openSettingsDialog(SettingsDialog::NoteFolderPage);
        } else {
            // User doesn't want to enable git support
            return;
        }

        if (!Utils::Git::isCurrentNoteFolderUseGit()) {
            return;
        }
    }

#ifdef USE_LIBGIT2
    // Providing a QJSEngine instance to the Utils::Git::getNoteVersions is important, otherwise
    // the returned versions will not be usable
    QJSEngine engine;

    // Limit the number of versions to load, because loading all versions
    // can take a long time and use a lot of memory
    const int limit = 100;

    showStatusBarMessage(
        tr("Latest %n note versions are currently gathered from your local git repository", "",
           limit),
        QStringLiteral("🗃️"), 20000);

    auto versions = Utils::Git::getNoteVersions(engine, currentNote, limit);
    showStatusBarMessage(tr("Done with gathering note versions from git"), QStringLiteral("🕒"),
                         2000);

    // Init the iterator for checking if there are versions
    QJSValueIterator versionsCheckIterator(versions);

    // QJSValueIterator may report hasNext as true even if there are no valid items,
    // so we check for at least one valid version with a humanReadableTimestamp property.
    bool hasValidVersion = false;
    while (versionsCheckIterator.hasNext()) {
        versionsCheckIterator.next();
        QJSValue property =
            versionsCheckIterator.value().property(QStringLiteral("humanReadableTimestamp"));
        if (!property.isUndefined() && !property.toString().isEmpty()) {
            hasValidVersion = true;
            break;
        }
    }

    if (!hasValidVersion) {
        Utils::Gui::information(this, tr("No versions available"),
                                tr("No versions are available for this note."),
                                QStringLiteral("git-no-versions"));
        return;
    }

    auto *dialog = new VersionDialog(versions);
    dialog->setWindowTitle(
        tr("Latest %n git versions of note: %1", "", limit).arg(currentNote.getFileName()));
    dialog->exec();
#else
    QString relativeFilePath = currentNote.relativeNoteFilePath();
    Utils::Git::showLog(relativeFilePath);
#endif
}

void MainWindow::on_actionShow_note_git_versions_external_triggered() {
    QString relativeFilePath = currentNote.relativeNoteFilePath();
    Utils::Git::showLog(relativeFilePath);
}

/**
 * Stores the note tag tree expand state when an tree widget item was collapsed
 */
void MainWindow::on_tagTreeWidget_itemCollapsed(QTreeWidgetItem *item) {
    _tagManager->on_tagTreeWidget_itemCollapsed(item);
}

/**
 * Stores the note tag tree expand state when an tree widget item was expanded
 */
void MainWindow::on_tagTreeWidget_itemExpanded(QTreeWidgetItem *item) {
    _tagManager->on_tagTreeWidget_itemExpanded(item);
}

/**
 * Stores the note tag tree expand state
 */
void MainWindow::storeTagTreeWidgetExpandState() const {
    _tagManager->storeTagTreeWidgetExpandState();
}

/**
 * Opens the script repository
 */
void MainWindow::on_actionScript_repository_triggered() {
    openSettingsDialog(SettingsDialog::ScriptingPage, true);
}

/**
 * Opens the script settings
 */
void MainWindow::on_actionScript_settings_triggered() {
    openSettingsDialog(SettingsDialog::ScriptingPage);
}

void MainWindow::updatePanelsSortOrder() {
    updateNotesPanelSortOrder();
    reloadNoteSubFolderTree();
    // do not reload it again, it has already been reloaded when
    // updateNotesPanelSortOrder() was called
    // reloadTagTree();
}

void MainWindow::updateNotesPanelSortOrder() {
    SettingsService settings;
    int sort = settings.value(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE).toInt();
    ui->actionAlphabetical->setChecked(sort == SORT_ALPHABETICAL);
    ui->actionBy_date->setChecked(sort == SORT_BY_LAST_CHANGE);

    updateNoteSortOrderSelectorVisibility(sort == SORT_ALPHABETICAL);

    int order = settings.value(QStringLiteral("notesPanelOrder")).toInt();
    ui->actionAscending->setChecked(order == ORDER_ASCENDING);
    ui->actionDescending->setChecked(order == ORDER_DESCENDING);

    loadNoteDirectoryList();
}

/**
 * Inserts a file as attachment
 */
void MainWindow::on_actionInsert_attachment_triggered() {
    auto *dialog = new AttachmentDialog(this);
    dialog->exec();

    if (dialog->result() == QDialog::Accepted) {
        insertAttachment(dialog->getFile(), dialog->getTitle());
    }

    delete (dialog);
}

/**
 * Turns note editing on or off
 *
 * @param checked
 */
void MainWindow::on_actionAllow_note_editing_triggered(bool checked) {
    SettingsService settings;
    settings.setValue(QStringLiteral("allowNoteEditing"), checked);

    updateNoteTextEditReadOnly();
    setMenuEnabled(ui->menuEncryption, checked);
    _readOnlyButton->setHidden(checked);

    ui->actionAllow_note_editing->setText(checked ? tr("Disallow all note editing")
                                                  : tr("Allow all note editing"));
}

void MainWindow::allowNoteEditing() {
    if (!ui->actionAllow_note_editing->isChecked()) {
        ui->actionAllow_note_editing->trigger();
    }
}

/**
 * Checks if read-only mode is enabled and allows to get out of it
 */
bool MainWindow::doNoteEditingCheck() {
    if (Utils::Misc::isNoteEditingAllowed()) {
        return true;
    }

    if (Utils::Gui::questionNoSkipOverride(this, tr("Note editing disabled"),
                                           tr("Note editing is currently disabled, do you "
                                              "want to allow it again?"),
                                           QStringLiteral("readonly-mode-allow")) ==
        QMessageBox::Yes) {
        allowNoteEditing();

        return true;
    }

    return false;
}

void MainWindow::disallowNoteEditing() {
    if (ui->actionAllow_note_editing->isChecked()) {
        ui->actionAllow_note_editing->trigger();
    }
}

/**
 * Enables or disables a menu and all its actions
 *
 * @param menu
 * @param enabled
 */
void MainWindow::setMenuEnabled(QMenu *menu, bool enabled) {
    menu->setEnabled(enabled);

    // loop through all actions of the menu
    const auto actions = menu->actions();
    for (QAction *action : actions) {
        action->setEnabled(enabled);
    }
}

void MainWindow::on_actionCheck_for_script_updates_triggered() {
    ScriptRepositoryDialog::checkForScriptUpdates(this);
}

/**
 * This method checks if scripts need to be updated and open a dialog to ask if
 * the user wants to update them (if the dialog wasn't disabled)
 */
void MainWindow::automaticScriptUpdateCheck() {
    // Sqlite isn't available in a separate thread, so we need to fetch the scripts before that
    auto scripts = Script::fetchAll();

    // We don't need to check for updates if there are no scripts
    if (scripts.isEmpty()) {
        return;
    }

    _scriptUpdateFound = false;
    auto *dialog = new ScriptRepositoryDialog(this, true);

    // Show a message once if no script update were found
    // We need to do that in a slot, because you can't use a timer in a separate thread
    QObject::connect(dialog, &ScriptRepositoryDialog::noUpdateFound, this, [this, dialog]() {
        showStatusBarMessage(tr("No script updates were found"), QStringLiteral("🔧"), 3000);
        delete (dialog);
    });

    // Show a dialog once if a script update was found
    QObject::connect(dialog, &ScriptRepositoryDialog::updateFound, this, [this, dialog]() {
        // We only want to run this once
        if (_scriptUpdateFound) {
            return;
        }

        _scriptUpdateFound = true;
        showStatusBarMessage(tr("A script update was found!"), QStringLiteral("🔧"), 4000);
        delete (dialog);

        if (Utils::Gui::question(this, tr("Script updates"),
                                 tr("Updates to your scripts were found in the script "
                                    "repository! Do you want to update them?"),
                                 "auto-script-update") == QMessageBox::Yes) {
            on_actionCheck_for_script_updates_triggered();
        }
    });

    // Search for script updates in the background after the "updateFound" signal was connected
    // We must not do a `delete (dialog)` in the new thread, that was causing a crash on some
    // systems, see https://github.com/pbek/QOwnNotes/issues/2937
    QFuture<void> future = QtConcurrent::run([this, dialog, scripts]() {
        qDebug() << "start searchForUpdates";
        dialog->searchForUpdatesForScripts(scripts);
        qDebug() << "done searchForUpdates";

        if (_scriptUpdateFound) {
            _scriptUpdateFound = false;
        }
    });
}

void MainWindow::updateJumpToActionsAvailability() {
    ui->actionJump_to_note_list_panel->setEnabled(ui->notesListFrame->isVisible());
    ui->actionJump_to_note_subfolder_panel->setEnabled(ui->noteSubFolderFrame->isVisible());
    ui->actionJump_to_tags_panel->setEnabled(ui->tagFrame->isVisible());

    ui->actionMove_up_in_subfolder_list->setEnabled(ui->noteSubFolderFrame->isVisible());
    ui->actionMove_down_in_subfolder_list->setEnabled(ui->noteSubFolderFrame->isVisible());
    ui->actionMove_up_in_tag_list->setEnabled(ui->tagFrame->isVisible());
    ui->actionMove_down_in_tag_list->setEnabled(ui->tagFrame->isVisible());
}

void MainWindow::on_actionShow_local_trash_triggered() {
    auto *dialog = new LocalTrashDialog(this);
    dialog->exec();
}

void MainWindow::on_actionJump_to_note_text_edit_triggered() {
    if (!_noteEditIsCentralWidget && _noteEditDockWidget != nullptr) {
        _noteEditDockWidget->show();
    }

    activeNoteTextEdit()->setFocus();
}

/**
 * Double-clicking a tag assigns the tag to the current note
 */
void MainWindow::on_tagTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    _tagManager->on_tagTreeWidget_itemDoubleClicked(item, column);
}

/**
 * Double-clicking a note calls a hook
 */
void MainWindow::on_noteTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    _noteTreeManager->on_noteTreeWidget_itemDoubleClicked(item, column);
}

/**
 * Reloads the current note (and selected notes) tags if there were selected
 * multiple notes
 */
void MainWindow::on_noteTreeWidget_itemSelectionChanged() {
    _noteTreeManager->on_noteTreeWidget_itemSelectionChanged();
}

/**
 * Shows a dialog to delete stored attachments
 */
void MainWindow::on_actionManage_stored_attachments_triggered() {
    delete (_storedAttachmentsDialog);
    _storedAttachmentsDialog = new StoredAttachmentsDialog(this);
    _storedAttachmentsDialog->show();
}

void MainWindow::on_noteOperationsButton_clicked() {
    _noteTreeManager->on_noteOperationsButton_clicked();
}

/**
 * Returns the text of the log widget
 */
QString MainWindow::getLogText() { return _logWidget->getLogText(); }

/**
 * Enables all logging and shows the log panel
 */
void MainWindow::turnOnDebugLogging() {
    SettingsService settings;
    settings.setValue(QStringLiteral("LogWidget/debugLog"), true);
    settings.setValue(QStringLiteral("LogWidget/infoLog"), true);
    settings.setValue(QStringLiteral("LogWidget/warningLog"), true);
    settings.setValue(QStringLiteral("LogWidget/criticalLog"), true);
    settings.setValue(QStringLiteral("LogWidget/fatalLog"), true);
    settings.setValue(QStringLiteral("LogWidget/statusLog"), true);
    settings.setValue(QStringLiteral("LogWidget/scriptingLog"), true);

    _logDockWidget->show();
}

void MainWindow::on_actionImport_notes_from_text_files_triggered() {
    FileDialog dialog(QStringLiteral("ImportTextFiles"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setWindowTitle(tr("Select text files to import"));
    int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return;
    }

    QStringList fileNames = dialog.selectedFiles();
    const int fileCount = fileNames.size();

    if (fileCount == 0) {
        return;
    }

    QProgressDialog progressDialog(QString(), tr("Cancel"), 0, fileCount, this);
    progressDialog.setWindowModality(Qt::WindowModal);

    const QSignalBlocker blocker(noteDirectoryWatcher);
    Q_UNUSED(blocker)

    for (int i = 0; i < fileCount; i++) {
        if (progressDialog.wasCanceled()) {
            break;
        }

        const QString &fileName = fileNames.at(i);

        QFile file(fileName);
        QFileInfo fileInfo(file);
        progressDialog.setLabelText(tr("Importing: %1").arg(fileInfo.fileName()));

        if (!file.open(QFile::ReadOnly | QFile::Text)) {
            qWarning() << "Failed to open file:" << file.fileName();
            continue;
        }

        QTextStream ts(&file);
        QString text = ts.readAll().trimmed();

        QRegularExpressionMatch match =
            QRegularExpression(QStringLiteral(R"(^.+\n=+)"), QRegularExpression::MultilineOption)
                .match(text);

        CreateNewNoteOptions options = CreateNewNoteOption::None;

        // add a headline if none was found
        if (!match.hasMatch()) {
            options = CreateNewNoteOption::UseNameAsHeadline;
        }

        options |= CreateNewNoteOption::DisableLoadNoteDirectoryList;

        createNewNote(fileInfo.baseName(), text, options);
        progressDialog.setValue(i);
    }

    progressDialog.setValue(fileCount);
    loadNoteDirectoryList();
}

/**
 * Opens the Telegram group
 */
void MainWindow::on_actionTelegram_triggered() {
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://t.me/QOwnNotes")));
}

/**
 * Copies the headline of the current note
 */
void MainWindow::on_actionCopy_headline_triggered() {
    QString noteText = currentNote.getNoteText();

    // try regular headlines
    QRegularExpressionMatch match =
        QRegularExpression(QStringLiteral(R"(^(.+)\n=+)"), QRegularExpression::MultilineOption)
            .match(noteText);

    QString headline;
    if (match.hasMatch()) {
        headline = match.captured(1);
    } else {
        // try alternative headlines
        match =
            QRegularExpression(QStringLiteral(R"(^#+ (.+)$)"), QRegularExpression::MultilineOption)
                .match(noteText);

        if (match.hasMatch()) {
            headline = match.captured(1);
        }
    }

    if (!headline.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(headline);
        showStatusBarMessage(tr("Note headline '%1' was copied to the clipboard").arg(headline),
                             QStringLiteral("📋"), 3000);
    }
}

void MainWindow::on_action_FormatTable_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    Utils::Gui::autoFormatTableAtCursor(textEdit);
}

/**
 * Centers and resized the main window
 */
void MainWindow::centerAndResize() {
    // get the dimension available on this screen
    QSize availableSize = QGuiApplication::primaryScreen()->availableGeometry().size();
    int width = availableSize.width();
    int height = availableSize.height();
    qDebug() << "Available dimensions " << width << "x" << height;
    width *= 0.9;     // 90% of the screen size
    height *= 0.9;    // 90% of the screen size
    qDebug() << "Computed dimensions " << width << "x" << height;
    QSize newSize(width, height);

    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, newSize,
                                    QGuiApplication::primaryScreen()->availableGeometry()));
}

/**
 * Filters navigation entries in the navigation tree widget
 */
void MainWindow::on_navigationLineEdit_textChanged(const QString &arg1) {
    _navigationManager->on_navigationLineEdit_textChanged(arg1);
}

const Note &MainWindow::getCurrentNote() { return currentNote; }

void MainWindow::on_actionJump_to_note_list_panel_triggered() { ui->noteTreeWidget->setFocus(); }

void MainWindow::on_actionJump_to_tags_panel_triggered() { ui->tagTreeWidget->setFocus(); }

void MainWindow::on_actionJump_to_note_subfolder_panel_triggered() {
    ui->noteSubFolderTreeWidget->setFocus();
}

void MainWindow::on_actionActivate_context_menu_triggered() { activateContextMenu(); }

void MainWindow::on_actionImport_bookmarks_from_server_triggered() {
    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->fetchBookmarks();
}

void MainWindow::on_actionElementMatrix_triggered() {
    QDesktopServices::openUrl(
        QUrl(QStringLiteral("https://app.element.io/#/room/"
                            "#qownnotes:matrix.org")));
}

void MainWindow::on_actionToggle_fullscreen_triggered() {
    _distractionFreeManager->on_actionToggle_fullscreen_triggered();
}

void MainWindow::disableFullScreenMode() { _distractionFreeManager->disableFullScreenMode(); }

void MainWindow::on_actionTypewriter_mode_toggled(bool arg1) {
    SettingsService settings;
    settings.setValue(QStringLiteral("Editor/centerCursor"), arg1);
    ui->noteTextEdit->updateSettings();
    ui->encryptedNoteTextEdit->updateSettings();

    if (arg1) {
        // center the cursor immediately if typewriter mode is turned on
        activeNoteTextEdit()->centerTheCursor();
    }
}

void MainWindow::on_actionCheck_spelling_toggled(bool checked) {
    _spellCheckManager->on_actionCheck_spelling_toggled(checked);
}

void MainWindow::loadDictionaryNames() { _spellCheckManager->loadDictionaryNames(); }

void MainWindow::onLanguageChanged(QAction *action) {
    _spellCheckManager->onLanguageChanged(action);
}

void MainWindow::loadSpellingBackends() { _spellCheckManager->loadSpellingBackends(); }

void MainWindow::onSpellBackendChanged(QAction *action) {
    _spellCheckManager->onSpellBackendChanged(action);
}

void MainWindow::on_actionManage_dictionaries_triggered() {
    _spellCheckManager->on_actionManage_dictionaries_triggered();
}

void MainWindow::on_noteTextEdit_modificationChanged(bool arg1) {
    if (!arg1) {
        return;
    }

    ui->noteTextEdit->document()->setModified(false);
    noteTextEditTextWasUpdated();
}

void MainWindow::on_encryptedNoteTextEdit_modificationChanged(bool arg1) {
    _noteEncryptionManager->on_encryptedNoteTextEdit_modificationChanged(arg1);
}

void MainWindow::on_actionEditorWidthCustom_triggered() {
    _distractionFreeManager->on_actionEditorWidthCustom_triggered();
}

void MainWindow::on_actionShow_Hide_application_triggered() {
    _systemTrayManager->on_actionShow_Hide_application_triggered();
}

void MainWindow::on_noteEditTabWidget_currentChanged(int index) {
    _noteTabManager->on_noteEditTabWidget_currentChanged(index);
}

void MainWindow::on_noteEditTabWidget_tabCloseRequested(int index) {
    _noteTabManager->on_noteEditTabWidget_tabCloseRequested(index);
}

void MainWindow::on_actionPrevious_note_tab_triggered() {
    _noteTabManager->on_actionPrevious_note_tab_triggered();
}

void MainWindow::on_actionNext_note_tab_triggered() {
    _noteTabManager->on_actionNext_note_tab_triggered();
}

void MainWindow::on_actionClose_current_note_tab_triggered() {
    _noteTabManager->on_actionClose_current_note_tab_triggered();
}

void MainWindow::on_actionNew_note_in_new_tab_triggered() {
    _noteTabManager->on_actionNew_note_in_new_tab_triggered();
}

/**
 * Close a note tab on a specific index.
 * @param index The index of the tab to close.
 */
bool MainWindow::removeNoteTab(int index) const { return _noteTabManager->removeNoteTab(index); }

/**
 * Returns a list of note ids that are opened in tabs
 */
QList<int> MainWindow::getNoteTabNoteIdList() const {
    return _noteTabManager->getNoteTabNoteIdList();
}

void MainWindow::on_noteEditTabWidget_tabBarDoubleClicked(int index) {
    _noteTabManager->on_noteEditTabWidget_tabBarDoubleClicked(index);
}

void MainWindow::on_actionToggle_note_stickiness_of_current_tab_triggered() {
    _noteTabManager->on_actionToggle_note_stickiness_of_current_tab_triggered();
}

/**
 * If the tab of current note was clicked now the subfolder of the note is
 * activated if that is needed to show the note in the note list
 */
void MainWindow::on_noteEditTabWidget_tabBarClicked(int index) {
    _noteTabManager->on_noteEditTabWidget_tabBarClicked(index);
}

/**
 * Note tab context menu
 */
void MainWindow::showNoteEditTabWidgetContextMenu(const QPoint &point) {
    _noteTabManager->showNoteEditTabWidgetContextMenu(point);
}

void MainWindow::on_actionJump_to_navigation_panel_triggered() {
    _navigationManager->on_actionJump_to_navigation_panel_triggered();
}

/**
 * Imports a DataUrl as file into QOwnNotes and inserts it into the current note
 * This currently only supports images
 * @param dataUrl
 * @return
 */
bool MainWindow::insertDataUrlAsFileIntoCurrentNote(const QString &dataUrl) {
    QString markdownCode = currentNote.importMediaFromDataUrl(dataUrl);

    if (markdownCode.isEmpty()) {
        return false;
    }

    insertNoteText(markdownCode);

    return true;
}

void MainWindow::on_actionImport_notes_from_Joplin_triggered() {
    const QSignalBlocker blocker(noteDirectoryWatcher);
    Q_UNUSED(blocker)

    auto dialog = new JoplinImportDialog(this);
    dialog->exec();

    if (dialog->getImportCount() > 0) {
        // reload the note folder after importing new notes
        buildNotesIndexAndLoadNoteDirectoryList(true, true);
    }

    delete (dialog);
}

QAction *MainWindow::reloadNoteFolderAction() { return ui->action_Reload_note_folder; }

QAction *MainWindow::newNoteAction() { return ui->action_New_note; }

QAction *MainWindow::insertTextLinkAction() { return ui->actionInsert_text_link; }

QAction *MainWindow::searchTextOnWebAction() { return ui->actionSearch_text_on_the_web; }

QAction *MainWindow::pasteImageAction() { return ui->actionPaste_image; }

QAction *MainWindow::autocompleteAction() { return ui->actionAutocomplete; }

QAction *MainWindow::splitNoteAtPosAction() { return ui->actionSplit_note_at_cursor_position; }

QAction *MainWindow::findNoteAction() { return ui->action_Find_note; }

QList<QAction *> MainWindow::customTextEditActions() { return _noteTextEditContextMenuActions; }

void MainWindow::on_actionToggle_Always_on_top_triggered() {
    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags.testFlag(Qt::WindowStaysOnTopHint) ? flags & (~Qt::WindowStaysOnTopHint)
                                                            : flags | Qt::WindowStaysOnTopHint);
    show();
}

void MainWindow::on_action_Load_Todo_Items_triggered() { reloadTodoLists(); }

bool MainWindow::nextCloudDeckCheck() {
    NextcloudDeckService nextcloudDeckService(this);

    if (!nextcloudDeckService.isEnabled()) {
        if (QMessageBox::question(
                nullptr, QObject::tr("Nextcloud Deck support disabled!"),
                QObject::tr(
                    "Nextcloud Deck support is not enabled or the settings are invalid.<br />"
                    "Please check your <strong>Nextcloud</strong> configuration in the settings!"),
                QMessageBox::Open | QMessageBox::Cancel, QMessageBox::Open) == QMessageBox::Open) {
            openSettingsDialog(SettingsDialog::OwnCloudPage);
        }

        return false;
    }

    return true;
}

void MainWindow::on_actionInsert_Nextcloud_Deck_card_triggered() { openNextcloudDeckDialog(); }

void MainWindow::openNextcloudDeckDialog(int cardId) {
    if (!nextCloudDeckCheck()) {
        return;
    }

    auto *dialog = new NextcloudDeckDialog(this);
    connect(dialog, &NextcloudDeckDialog::searchInNotes, this,
            [this](const QString &searchText) { ui->searchLineEdit->setText(searchText); });

    qDebug() << __func__ << "cardId: " << cardId;

    if (cardId > 0) {
        dialog->setCardId(cardId);
    } else {
        QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
        QString selectedText = textEdit->textCursor().selectedText();

        if (!selectedText.isEmpty()) {
            dialog->setTitle(selectedText);
        }
    }

    dialog->exec();
}

void MainWindow::on_actionCopy_path_to_note_to_clipboard_triggered() {
    const QString path = currentNote.fullNoteFilePath();

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(path);
    showStatusBarMessage(tr("Note path '%1' was copied to the clipboard").arg(path),
                         QStringLiteral("📋"), 3000);
}

void MainWindow::on_actionCopy_note_filename_to_clipboard_triggered() {
    const QString fileName = currentNote.getFileName();

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(fileName);
    showStatusBarMessage(tr("Note filename '%1' was copied to the clipboard").arg(fileName),
                         QStringLiteral("📋"), 3000);
}

void MainWindow::on_actionMove_up_in_subfolder_list_triggered() {
    auto *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    QApplication::postEvent(ui->noteSubFolderTreeWidget, event);
}

void MainWindow::on_actionMove_down_in_subfolder_list_triggered() {
    auto *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    QApplication::postEvent(ui->noteSubFolderTreeWidget, event);
}

void MainWindow::on_actionMove_up_in_tag_list_triggered() {
    auto *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier);
    QApplication::postEvent(ui->tagTreeWidget, event);
}

void MainWindow::on_actionMove_down_in_tag_list_triggered() {
    auto *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier);
    QApplication::postEvent(ui->tagTreeWidget, event);
}

void MainWindow::buildAiToolbarAndActions() { _aiToolbarManager->buildAiToolbarAndActions(); }

void MainWindow::on_actionEnable_AI_toggled(bool arg1) {
    _aiToolbarManager->on_actionEnable_AI_toggled(arg1);
}

void MainWindow::on_navigationTabWidget_currentChanged(int index) {
    _navigationManager->on_navigationTabWidget_currentChanged(index);
}

void MainWindow::enableOpenAiActivitySpinner(bool enable) {
    _aiToolbarManager->enableOpenAiActivitySpinner(enable);
}

/**
 * Reattaches all floating panels in case they can't be reattached manually anymore
 */
void MainWindow::on_actionReattach_panels_triggered() {
    _workspaceManager->on_actionReattach_panels_triggered();
}

void MainWindow::on_actionManage_Nextcloud_Deck_cards_triggered() {
    on_actionInsert_Nextcloud_Deck_card_triggered();
}

void MainWindow::on_actionSend_clipboard_triggered() {
    // We need to show the window because on some systems the clipboard
    // can't be accessed if the app is not focused
    showWindow();

    // We need a delay because otherwise the clipboard can't be properly accessed on some systems
    QTimer::singleShot(1000, this, [this] {
        if (_webAppClientService->sendClipboard()) {
            showStatusBarMessage(tr("Clipboard sent successfully"), QStringLiteral("✅"), 3000);
        } else {
            showStatusBarMessage(tr("Failed to send clipboard"), QStringLiteral("⚠️"), 5000);
        }
    });
}

void MainWindow::on_actionSend_clipboard_as_text_triggered() {
    // We need to show the window because on some systems the clipboard
    // can't be accessed if the app is not focused
    showWindow();

    // We need a delay because otherwise the clipboard can't be properly accessed on some systems
    QTimer::singleShot(1000, this, [this] {
        if (_webAppClientService->sendClipboardAsText()) {
            showStatusBarMessage(tr("Clipboard text sent successfully"), QStringLiteral("✅"),
                                 3000);
        } else {
            showStatusBarMessage(tr("Failed to send clipboard text"), QStringLiteral("⚠️"), 5000);
        }
    });
}
