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
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextLength>
#include <QTimer>
#include <QTreeWidgetItem>
#include <QUuid>
#include <QWidgetAction>
#include <QtConcurrent>
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
#include "widgets/htmlpreviewwidget.h"
#include "widgets/noterelationscene.h"
#include "widgets/qownnotesmarkdowntextedit.h"

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
    connect(ui->noteEditTabWidget->tabBar(), &QWidget::customContextMenuRequested, this,
            &MainWindow::showNoteEditTabWidgetContextMenu);

    // Set the two shortcuts for the "increase note text size" action
    const QList<QKeySequence> shortcuts = {QKeySequence(Qt::CTRL | Qt::Key_Plus),
                                           QKeySequence(Qt::CTRL | Qt::Key_Equal)};
    ui->action_Increase_note_text_size->setShortcuts(shortcuts);

    initTreeWidgets();

    initNotePreviewAndTextEdits();

    setWindowIcon(getSystemTrayIcon());

    // initialize the workspace combo box
    initWorkspaceComboBox();

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
    _leaveFullScreenModeButton = nullptr;
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
    reloadOpenAiControls();

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
    updateWorkspaceLists();

    createSystemTrayIcon();

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
            restoreCurrentWorkspace();
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
    restoreDistractionFreeMode();

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
    }

    // set the action group for the width selector of the distraction free mode
    auto *dfmEditorWidthActionGroup = new QActionGroup(this);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthNarrow);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthMedium);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthWide);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthFull);
    dfmEditorWidthActionGroup->addAction(ui->actionEditorWidthCustom);
    dfmEditorWidthActionGroup->setExclusive(true);

    connect(dfmEditorWidthActionGroup, &QActionGroup::triggered, this,
            &MainWindow::dfmEditorWidthActionTriggered);

    setAcceptDrops(true);

    // act on position clicks in the navigation widget
    connect(ui->navigationWidget, &NavigationWidget::positionClicked, this,
            &MainWindow::onNavigationWidgetPositionClicked);
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
    if (!ui->noteViewFrame->layout()) ui->noteViewFrame->setLayout(new QVBoxLayout);
    ui->noteViewFrame->layout()->addWidget(_notePreviewWidget);

    // QTextBrowser previewer is hidden when we use qlitehtml
    ui->noteTextView->setVisible(false);

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
    if (!delayed) {
        connect(&noteDirectoryWatcher, &QFileSystemWatcher::directoryChanged, this,
                &MainWindow::notesDirectoryWasModified, Qt::UniqueConnection);
        connect(&noteDirectoryWatcher, &QFileSystemWatcher::fileChanged, this,
                &MainWindow::notesWereModified, Qt::UniqueConnection);
    } else {
        // In some cases, there are delayed signals coming in which we don't want to handle
        // so reconnect with delay
        QTimer::singleShot(300, this, [this] {
            connect(&noteDirectoryWatcher, &QFileSystemWatcher::directoryChanged, this,
                    &MainWindow::notesDirectoryWasModified, Qt::UniqueConnection);
            connect(&noteDirectoryWatcher, &QFileSystemWatcher::fileChanged, this,
                    &MainWindow::notesWereModified, Qt::UniqueConnection);
        });
    }
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
    // deleting old global shortcut assignments
    foreach (QHotkey *hotKey, _globalShortcuts) {
        delete hotKey;
    }

    _globalShortcuts.clear();
    SettingsService settings;
    settings.beginGroup(QStringLiteral("GlobalShortcuts"));

    foreach (const QString &key, settings.allKeys()) {
        if (!key.contains(QStringLiteral("MainWindow"))) {
            continue;
        }

        QString actionName = key;
        actionName.remove(QStringLiteral("MainWindow-"));
        QAction *action = findAction(actionName);

        if (action == nullptr) {
            qDebug() << "Failed to find action with name: " << actionName;
            continue;
        }

        QString shortcut = settings.value(key).toString();

        auto hotKey = new QHotkey(QKeySequence(shortcut), true, this);
        _globalShortcuts.append(hotKey);
        connect(hotKey, &QHotkey::activated, this, [this, action]() {
            qDebug() << "Global shortcut action triggered: " << action->objectName();

            // Don't call showWindow() for the "Show/Hide application" action
            // because it will call it itself
            if (action->objectName() != "actionShow_Hide_application") {
                // bring application window to the front
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

    if (!isInDistractionFreeMode() && !forceQuit && !_closeEventWasFired) {
        storeCurrentWorkspace();
    }

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
void MainWindow::initWorkspaceComboBox() {
    _workspaceComboBox = new QComboBox(this);
    connect(_workspaceComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::onWorkspaceComboBoxCurrentIndexChanged);
    _workspaceComboBox->setToolTip(tr("Workspaces"));
    _workspaceComboBox->setObjectName(QStringLiteral("workspaceComboBox"));
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
        _noteEditDockWidget = new QDockWidget(tr("Note edit"), this);
        _noteEditDockWidget->setObjectName(QStringLiteral("noteEditDockWidget"));
        _noteEditDockWidget->setWidget(ui->noteEditTabWidget);
        _noteEditDockTitleBarWidget = _noteEditDockWidget->titleBarWidget();
        sizePolicy = _noteEditDockWidget->sizePolicy();
        sizePolicy.setHorizontalStretch(5);
        _noteEditDockWidget->setSizePolicy(sizePolicy);
        addDockWidget(Qt::RightDockWidgetArea, _noteEditDockWidget, Qt::Horizontal);
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

    // macOS and Windows will look better without this
#ifdef Q_OS_LINUX
    if (_noteEditIsCentralWidget) {
        ui->noteTextEdit->setFrameShape(QFrame::StyledPanel);
        ui->encryptedNoteTextEdit->setFrameShape(QFrame::StyledPanel);
    }
#endif

    // restore the current workspace
    restoreCurrentWorkspace();

    // lock the dock widgets
    on_actionUnlock_panels_toggled(false);

    // update the workspace menu and combobox entries
    updateWorkspaceLists();

    // initialize the panel menu
    initPanelMenu();
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
    const bool wasBuilt = buildNotesIndex(0, forceBuild);

    if (wasBuilt || forceLoad) {
        loadNoteDirectoryList();

        // Expire trashed items
        // It is safer to do that here than in MainWindow::frequentPeriodicChecker(), to avoid
        // sync errors after resuming from suspend
        TrashItem::expireItems();
    }

    if (wasBuilt && reloadTabs) {
        // restore the note tabs
        Utils::Gui::reloadNoteTabs(ui->noteEditTabWidget);
    }
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

    _aiModelGroup = new QActionGroup(ui->menuAI_model);

    _aiToolbar = new QToolBar(tr("AI toolbar"), this);
    _aiToolbar->setObjectName(QStringLiteral("aiToolbar"));
    addToolBar(_aiToolbar);

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
    connect(_aiToolbar, &QToolBar::actionTriggered, this, &MainWindow::trackAction);
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
    widgetAction->setDefaultWidget(_workspaceComboBox);
    widgetAction->setObjectName(QStringLiteral("actionWorkspaceComboBox"));
    widgetAction->setText(tr("Workspace selector"));
    _windowToolbar->addAction(widgetAction);
    _windowToolbar->addAction(ui->actionStore_as_new_workspace);
    _windowToolbar->addAction(ui->actionRemove_current_workspace);
    _windowToolbar->addAction(ui->actionRename_current_workspace);
    _windowToolbar->addAction(ui->actionSwitch_to_previous_workspace);
    _windowToolbar->addAction(ui->actionUnlock_panels);

    _windowToolbar->addSeparator();
    _windowToolbar->addAction(ui->actionToggle_distraction_free_mode);
    _windowToolbar->addAction(ui->action_Increase_note_text_size);
    _windowToolbar->addAction(ui->action_Decrease_note_text_size);
    _windowToolbar->addAction(ui->action_Reset_note_text_size);
}

/**
 * Updates the workspace menu and combobox entries
 */
void MainWindow::updateWorkspaceLists(bool rebuild) {
    SettingsService settings;
    const QStringList workspaces = getWorkspaceUuidList();
    const QString currentUuid = currentWorkspaceUuid();

    if (rebuild) {
        // we need to create a new combo box so the width gets updated in the
        // window toolbar
        initWorkspaceComboBox();

        ui->menuWorkspaces->clear();

        _workspaceNameUuidMap.clear();
    }

    const QSignalBlocker blocker(_workspaceComboBox);
    Q_UNUSED(blocker)

    int currentIndex = 0;

    for (int i = 0; i < workspaces.count(); i++) {
        const QString &uuid = workspaces.at(i);

        if (uuid == currentUuid) {
            currentIndex = i;
        }

        // check if we want to skip the rebuilding part
        if (!rebuild) {
            continue;
        }

        const QString name =
            settings.value(QStringLiteral("workspace-") + uuid + QStringLiteral("/name"))
                .toString();
        const QString objectName = QStringLiteral("restoreWorkspace-") + uuid;

        _workspaceNameUuidMap.insert(name, uuid);

        _workspaceComboBox->addItem(name, uuid);

        auto *action = new QAction(name, ui->menuWorkspaces);
        connect(action, &QAction::triggered, this, [this, uuid]() { setCurrentWorkspace(uuid); });

        // set an object name for creating shortcuts
        action->setObjectName(objectName);

        // try to load a key sequence from the settings
        QKeySequence shortcut = QKeySequence(
            settings.value(QStringLiteral("Shortcuts/MainWindow-") + objectName).toString());
        action->setShortcut(shortcut);

        //        if (uuid == currentUuid) {
        //            QFont font = action->font();
        //            font.setBold(true);
        //            action->setFont(font);
        //        }

        ui->menuWorkspaces->addAction(action);
    }

    _workspaceComboBox->setCurrentIndex(currentIndex);

    if (rebuild) {
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
    const auto dockWidgets = findChildren<QDockWidget *>();
    for (QDockWidget *dockWidget : dockWidgets) {
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
    SettingsService settings;
    return settings.value(QStringLiteral("DistractionFreeMode/isEnabled")).toBool();
}

/**
 * Toggles the distraction free mode
 */
void MainWindow::toggleDistractionFreeMode() {
    // Leave the one-column mode if active
    if (ui->actionUse_one_column_mode->isChecked()) {
        ui->actionUse_one_column_mode->toggle();
    }

    SettingsService settings;
    bool isInDistractionFreeMode = MainWindow::isInDistractionFreeMode();

    qDebug() << __func__ << " - 'isInDistractionFreeMode': " << isInDistractionFreeMode;

    // Store the window settings before we go into distraction-free mode
    if (!isInDistractionFreeMode) {
        storeSettings();
    }

    isInDistractionFreeMode = !isInDistractionFreeMode;

    // Remember that we were using the distraction-free mode
    settings.setValue(QStringLiteral("DistractionFreeMode/isEnabled"), isInDistractionFreeMode);

    setDistractionFreeMode(isInDistractionFreeMode);

    // Enter or leave fullscreen mode if we are in or left distraction-free mode
    if ((isInDistractionFreeMode && !isFullScreen()) ||
        (!isInDistractionFreeMode && isFullScreen())) {
        on_actionToggle_fullscreen_triggered();
    }
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

    // move the note view scrollbar when the note edit scrollbar was moved
    connect(ui->noteTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
            SLOT(noteTextSliderValueChanged(int)));
    connect(ui->encryptedNoteTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
            SLOT(noteTextSliderValueChanged(int)));

    // move the note edit scrollbar when the note view scrollbar was moved
#ifdef USE_QLITEHTML
    connect(_notePreviewWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
            SLOT(noteViewSliderValueChanged(int)));
#else
    connect(ui->noteTextView->verticalScrollBar(), SIGNAL(valueChanged(int)), this,
            SLOT(noteViewSliderValueChanged(int)));
#endif

    // hide the combo box if it looses focus if it should not be viewed
    connect(ui->noteFolderComboBox, &ComboBox::focusOut, this,
            &MainWindow::hideNoteFolderComboBoxIfNeeded);

    Utils::Gui::fixDarkModeIcons(this);
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
    SettingsService settings;

    if (enabled) {
        //
        // enter the distraction free mode
        //

        // turn off line numbers because they would look broken in dfm
        ui->noteTextEdit->setLineNumberEnabled(false);
        ui->encryptedNoteTextEdit->setLineNumberEnabled(false);

        // store the current workspace in case we changed something
        storeCurrentWorkspace();

        const bool menuBarWasVisible =
            settings.value(QStringLiteral("showMenuBar"), !ui->menuBar->isHidden()).toBool();

        // set the menu bar visible so we get the correct height
        if (!menuBarWasVisible) {
            ui->menuBar->setVisible(true);
        }

        // remember states, geometry and sizes
        settings.setValue(QStringLiteral("DistractionFreeMode/windowState"), saveState());
        settings.setValue(QStringLiteral("DistractionFreeMode/menuBarGeometry"),
                          ui->menuBar->saveGeometry());
        settings.setValue(QStringLiteral("DistractionFreeMode/menuBarHeight"),
                          ui->menuBar->height());
        settings.setValue(QStringLiteral("DistractionFreeMode/menuBarVisible"), menuBarWasVisible);

        // we must not hide the menu bar or else the shortcuts
        // will not work any more
        ui->menuBar->setFixedHeight(0);

        // hide the toolbars
        const QList<QToolBar *> toolbars = findChildren<QToolBar *>();
        for (QToolBar *toolbar : toolbars) {
            toolbar->hide();
        }

        if (!_noteEditIsCentralWidget) {
            // show the note edit dock widget
            _noteEditDockWidget->show();
        }

        // hide all dock widgets but the note edit dock widget
        const QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();
        for (QDockWidget *dockWidget : dockWidgets) {
            if (dockWidget->objectName() == QStringLiteral("noteEditDockWidget")) {
                continue;
            }
            dockWidget->hide();
        }

        // hide the status bar
        //        ui->statusBar->hide();

        _leaveDistractionFreeModeButton = new QPushButton(tr("leave"));
        _leaveDistractionFreeModeButton->setFlat(true);
        _leaveDistractionFreeModeButton->setToolTip(tr("Leave distraction free mode"));
        _leaveDistractionFreeModeButton->setStyleSheet(
            QStringLiteral("QPushButton {padding: 0 5px}"));

        _leaveDistractionFreeModeButton->setIcon(QIcon::fromTheme(
            QStringLiteral("zoom-original"),
            QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/zoom-original.svg"))));

        connect(_leaveDistractionFreeModeButton, &QPushButton::clicked, this,
                &MainWindow::toggleDistractionFreeMode);

        statusBar()->addPermanentWidget(_leaveDistractionFreeModeButton);

        ui->noteEditTabWidget->tabBar()->hide();
    } else {
        //
        // leave the distraction free mode
        //

        statusBar()->removeWidget(_leaveDistractionFreeModeButton);
        disconnect(_leaveDistractionFreeModeButton, nullptr, nullptr, nullptr);

        // restore states and sizes
        restoreState(
            settings.value(QStringLiteral("DistractionFreeMode/windowState")).toByteArray());
        ui->menuBar->setVisible(
            settings.value(QStringLiteral("DistractionFreeMode/menuBarVisible")).toBool());
        ui->menuBar->restoreGeometry(
            settings.value(QStringLiteral("DistractionFreeMode/menuBarGeometry")).toByteArray());
        ui->menuBar->setFixedHeight(
            settings.value(QStringLiteral("DistractionFreeMode/menuBarHeight")).toInt());

        if (ui->noteEditTabWidget->count() > 1) {
            ui->noteEditTabWidget->tabBar()->show();
        }

        bool showLineNumbersInEditor =
            settings.value(QStringLiteral("Editor/showLineNumbers")).toBool();

        // turn line numbers on again if they were enabled
        if (showLineNumbersInEditor) {
            ui->noteTextEdit->setLineNumberEnabled(true);
            ui->encryptedNoteTextEdit->setLineNumberEnabled(true);
        }
    }

    ui->noteTextEdit->setPaperMargins();
    ui->encryptedNoteTextEdit->setPaperMargins();
    activeNoteTextEdit()->setFocus();
}

/**
 * Sets the distraction free mode if it is currently other than we want it to be
 */
void MainWindow::changeDistractionFreeMode(const bool enabled) {
    if (isInDistractionFreeMode() != enabled) {
        setDistractionFreeMode(enabled);
    }
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

    // reload notes if notes folder was changed
    if (oldPath != folderName) {
        const QSignalBlocker blocker2(this->ui->searchLineEdit);
        {
            Q_UNUSED(blocker2)
            ui->searchLineEdit->clear();
        }

        // store everything before changing folder
        storeUpdatedNotesToDisk();

        // commit the changes in the current note folder to git
        gitCommitCurrentNoteFolder();

        noteFolder.setAsCurrent();

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

        buildNotesIndexAndLoadNoteDirectoryList(false, false, false);

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

        // commit the changes in the selected note folder to git
        gitCommitCurrentNoteFolder();
    }

    generateSystemTrayContextMenu();
    updateWindowTitle();
    _lastNoteId = 0;

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

void MainWindow::createSystemTrayIcon() {
    trayIcon = new QSystemTrayIcon(this);

    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::systemTrayIconClicked);

    if (showSystemTray) {
        trayIcon->setIcon(getSystemTrayIcon());
        trayIcon->show();
    }
}

/**
 * Returns a proper system tray icon
 *
 * @return
 */
QIcon MainWindow::getSystemTrayIcon() {
    const SettingsService settings;
    const bool darkModeIcon = settings.value(QStringLiteral("darkModeTrayIcon"), false).toBool();
    const QString file = darkModeIcon ? QStringLiteral(":/images/icon-dark.png")
                                      : QStringLiteral(":/images/icon.png");
    return QIcon(file);
}

/**
 * Creates the items in the note tree widget from the note and note sub
 * folder tables
 */
void MainWindow::loadNoteDirectoryList() {
    qDebug() << __func__;

    // Clean up favorite notes list (remove entries for deleted notes)
    Note::cleanupFavoriteNotes();

    const QSignalBlocker blocker(ui->noteTextEdit);
    Q_UNUSED(blocker)

    const QSignalBlocker blocker2(ui->noteTreeWidget);
    Q_UNUSED(blocker2)

    const bool isCurrentNoteTreeEnabled = NoteFolder::isCurrentNoteTreeEnabled();
    ui->noteTreeWidget->clear();
    //    ui->noteTreeWidget->setRootIsDecorated(isCurrentNoteTreeEnabled);
    int itemCount;

    if (isCurrentNoteTreeEnabled) {
        auto *noteFolderItem = new QTreeWidgetItem();
        noteFolderItem->setText(0, tr("Note folder"));
        noteFolderItem->setData(0, Qt::UserRole, 0);
        noteFolderItem->setData(0, Qt::UserRole + 1, FolderType);
        noteFolderItem->setIcon(0, Utils::Gui::folderIcon());
        noteFolderItem->setForeground(1, QColor(Qt::gray));
        ui->noteTreeWidget->addTopLevelItem(noteFolderItem);

        ui->noteSubFolderTreeWidget->buildTreeForParentItem(noteFolderItem);
        noteFolderItem->setExpanded(true);

        itemCount = Note::countAll();
    } else {
        // load all notes and add them to the note list widget
        const QVector<Note> noteList = Note::fetchAll();
        for (const Note &note : noteList) {
            addNoteToNoteTreeWidget(note);
        }

        itemCount = noteList.count();
    }

    MetricsService::instance()->sendEventIfEnabled(
        QStringLiteral("note/list/loaded"), QStringLiteral("note"),
        QStringLiteral("note list loaded"), QString::number(itemCount) + QStringLiteral(" notes"),
        itemCount);

    // sort alphabetically again if necessary
    SettingsService settings;
    if (settings.value(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE).toInt() ==
        SORT_ALPHABETICAL) {
        ui->noteTreeWidget->sortItems(
            0, Utils::Gui::toQtOrder(settings.value(QStringLiteral("notesPanelOrder")).toInt()));
    }

    // setup tagging
    setupTags();

    if (!isCurrentNoteTreeEnabled) {
        // setup note sub folders
        setupNoteSubFolders();
    }

    // generate the tray context menu
    generateSystemTrayContextMenu();

    // clear the text edits if there is no visible note
    if (firstVisibleNoteTreeWidgetItem() == nullptr) {
        unsetCurrentNote();
    } else {
        const auto item = findNoteInNoteTreeWidget(currentNote);

        // in the end we need to set the current item again if we can find it
        if (item != nullptr) {
            ui->noteTreeWidget->setCurrentItem(item);
        }
    }
}

/**
 * Adds a note to the note tree widget
 */
bool MainWindow::addNoteToNoteTreeWidget(const Note &note, QTreeWidgetItem *parent) {
    const QString name = note.getName();

    // skip notes without name
    if (name.isEmpty()) {
        return false;
    }

    const bool isNoteListPreview = Utils::Misc::isNoteListPreview();

    // add a note item to the tree
    auto *noteItem = new QTreeWidgetItem();
    Utils::Gui::setTreeWidgetItemToolTipForNote(noteItem, note);
    noteItem->setText(0, name);
    noteItem->setData(0, Qt::UserRole, note.getId());
    noteItem->setData(0, Qt::UserRole + 1, NoteType);

    // Store favorite status for sorting (UserRole + 2)
    const bool isFavorite = note.isFavorite();
    noteItem->setData(0, Qt::UserRole + 2, isFavorite);

    // Set the icon based on favorite status
    if (isFavorite) {
        noteItem->setIcon(0, Utils::Gui::favoriteNoteIcon());
    } else {
        noteItem->setIcon(0, Utils::Gui::noteIcon());
    }

    const Tag tag = Tag::fetchOneOfNoteWithColor(note);
    if (tag.isFetched()) {
        // set the color of the note tree widget item
        Utils::Gui::handleTreeWidgetItemTagColor(noteItem, tag);
    }

    const bool isEditable = Note::allowDifferentFileName();
    if (isEditable) {
        noteItem->setFlags(noteItem->flags() | Qt::ItemIsEditable);
    }

    const QSignalBlocker blocker(ui->noteTreeWidget);
    Q_UNUSED(blocker)

    if (parent == nullptr) {
        // Insert notes with favorites at the top
        // Find the insertion position based on favorite status
        if (isFavorite) {
            // For favorite notes, find the last position of favorites
            int insertPos = 0;
            const int count = ui->noteTreeWidget->topLevelItemCount();
            for (int i = 0; i < count; ++i) {
                QTreeWidgetItem *existingItem = ui->noteTreeWidget->topLevelItem(i);
                if (existingItem->data(0, Qt::UserRole + 1) == NoteType &&
                    existingItem->data(0, Qt::UserRole + 2).toBool()) {
                    insertPos = i + 1;
                } else {
                    break;
                }
            }
            ui->noteTreeWidget->insertTopLevelItem(insertPos, noteItem);
        } else {
            // Non-favorite notes go after all favorites
            ui->noteTreeWidget->addTopLevelItem(noteItem);
        }
    } else {
        parent->addChild(noteItem);
    }

    if (isNoteListPreview) {
        updateNoteTreeWidgetItem(note, noteItem);
    }

    //    SettingsService settings;
    //    if (settings.value("notesPanelSort", SORT_BY_LAST_CHANGE).toInt() ==
    //    SORT_ALPHABETICAL) {
    //        ui->noteTreeWidget->addTopLevelItem(noteItem);
    //    } else {
    //        ui->noteTreeWidget->insertTopLevelItem(0, noteItem);
    //    }

    return true;
}

void MainWindow::updateNoteTreeWidgetItem(const Note &note, QTreeWidgetItem *noteItem) {
    if (noteItem == nullptr) {
        noteItem = findNoteInNoteTreeWidget(note);
    }

    QWidget *widget = ui->noteTreeWidget->itemWidget(noteItem, 0);
    auto *noteTreeWidgetItem = dynamic_cast<NoteTreeWidgetItem *>(widget);

    // check if we already set a NoteTreeWidgetItem in the past
    if (noteTreeWidgetItem != nullptr) {
        noteTreeWidgetItem->updateUserInterface(note);
    } else {
        noteTreeWidgetItem = new NoteTreeWidgetItem(note, ui->noteTreeWidget);
    }

    // TODO: set background color
    //    noteTreeWidgetItem->setBackground(noteItem->background(0).color());
    // TODO: handle note renaming
    // TODO: handle updating when note gets changed
    // TODO: handle updating in handleTreeWidgetItemTagColor

    // this takes too long, it takes ages to do this on 1000 notes
    ui->noteTreeWidget->setItemWidget(noteItem, 0, noteTreeWidgetItem);
}

/**
 * @brief makes the current note the first item in the note list without
 * reloading the whole list
 */
void MainWindow::makeCurrentNoteFirstInNoteList() {
    QTreeWidgetItem *item = findNoteInNoteTreeWidget(currentNote);

    if (item != nullptr) {
        const QSignalBlocker blocker(ui->noteTreeWidget);
        Q_UNUSED(blocker)

        ui->noteTreeWidget->takeTopLevelItem(ui->noteTreeWidget->indexOfTopLevelItem(item));

        // Determine insertion position based on favorite status
        const bool isFavorite = currentNote.isFavorite();
        int insertPos = 0;

        if (!isFavorite) {
            // Non-favorite note: insert after all favorites
            const int count = ui->noteTreeWidget->topLevelItemCount();
            for (int i = 0; i < count; ++i) {
                QTreeWidgetItem *existingItem = ui->noteTreeWidget->topLevelItem(i);
                if (existingItem->data(0, Qt::UserRole + 1) == NoteType &&
                    existingItem->data(0, Qt::UserRole + 2).toBool()) {
                    insertPos = i + 1;
                } else {
                    break;
                }
            }
        }
        // For favorite notes, insertPos stays at 0 (first position)

        ui->noteTreeWidget->insertTopLevelItem(insertPos, item);

        // set the item as current item if it is visible
        if (!item->isHidden()) {
            ui->noteTreeWidget->setCurrentItem(item);

            if (Utils::Misc::isNoteListPreview()) {
                // ui->noteTreeWidget->setCurrentItem seems to destroy the
                // NoteTreeWidgetItem
                // TODO: the list symbol is still gone
                updateNoteTreeWidgetItem(currentNote, item);
            }
        }

        //        bool isInActiveNoteSubFolder =
        //                NoteSubFolder::activeNoteSubFolderId() ==
        //                currentNote.getNoteSubFolderId();

        // has problems with
        // NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively()
        //        if (!(isInActiveNoteSubFolder ||
        //        _showNotesFromAllNoteSubFolders)) {
        //            item->setHidden(true);
        //        } else {
        //            ui->noteTreeWidget->setCurrentItem(item);
        //        }
    }
}

/**
 * Finds a note in the note tree widget and returns its item
 *
 * @param note
 * @return
 */
QTreeWidgetItem *MainWindow::findNoteInNoteTreeWidget(const Note &note) {
    const int noteId = note.getId();
    const int count = ui->noteTreeWidget->topLevelItemCount();

    for (int i = 0; i < count; ++i) {
        QTreeWidgetItem *item = ui->noteTreeWidget->topLevelItem(i);

        if (item->data(0, Qt::UserRole + 1) == NoteType &&
            item->data(0, Qt::UserRole).toInt() == noteId) {
            return item;
        }
    }

    return nullptr;
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
    _spellBackendGroup = new QActionGroup(ui->menuSpelling_backend);
    loadSpellingBackends();
#else
    ui->menuSpelling_backend->menuAction()->setVisible(false);
#endif

    // load language dicts names into menu
    // Delay loading, loading dictionary names is slow
#if (QT_VERSION >= QT_VERSION_CHECK(5, 4, 0))
    QTimer::singleShot(10, this, [this] {
#endif
        _languageGroup = new QActionGroup(ui->menuLanguages);
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
        _aiToolbar->setIconSize(size);
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
    } else if (Utils::Misc::isSocketServerEnabled()) {
        if (_webSocketServerService->getPort() != WebSocketServerService::getSettingsPort()) {
            _webSocketServerService->listen();
        }
    } else {
        _webSocketServerService->close();
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
    // workaround when signal block doesn't work correctly
    if (_isNotesWereModifiedDisabled) {
        return;
    }

    // if we should ignore all changes return here
    if (SettingsService().value(QStringLiteral("ignoreAllExternalNoteFolderChanges")).toBool()) {
        return;
    }

    // We are ignoring changes in the .git folder
    if (str.contains(QStringLiteral("/.git/"))) {
        return;
    }

    QFileInfo fi(str);
    Note note = Note::fetchByFileUrl(QUrl::fromLocalFile(str));

    qDebug() << __func__ << " - 'str': " << str;
    qDebug() << __func__ << " - 'note': " << note;
    qDebug() << __func__ << " - 'currentNote': " << currentNote;

    // load note from disk if current note was changed
    if ((note.getFileName() == this->currentNote.getFileName()) &&
        (note.getNoteSubFolderId() == this->currentNote.getNoteSubFolderId())) {
        if (note.fileExists()) {
            // If the modified date of the file is the same as the one
            // from the current note it was a false alarm
            if (fi.lastModified() == this->currentNote.getFileLastModified()) {
                qDebug() << __func__ << " - Modification date didn't change, ignoring";
                return;
            }

            const QString oldNoteText = note.getNoteText();

            // fetch text of note from disk
            note.updateNoteTextFromDisk();
            const QString noteTextOnDisk = Utils::Misc::transformLineFeeds(note.getNoteText());
            const bool isCurrentNoteNotEditedForAWhile =
                this->currentNoteLastEdited.addSecs(60) < QDateTime::currentDateTime();
            // If the current note wasn't edited for a while, we want that it is possible
            // to get updated even with small changes, so we are setting a threshold of 0.
            // If it was recently edited, we use a threshold of 8 for the similarity check
            // to avoid false positives, but we'll still show the dialog if changes are detected.
            const int threshold = isCurrentNoteNotEditedForAWhile ? 0 : 8;

            // Check if the old note text is the same or similar as the one on disk
            if (Utils::Misc::isSimilar(oldNoteText, noteTextOnDisk, threshold)) {
                qDebug() << __func__ << " - Old and new text are same or similar, ignoring";
                return;
            }

            const QString noteTextOnDiskHash = QString(
                QCryptographicHash::hash(noteTextOnDisk.toLocal8Bit(), QCryptographicHash::Sha1)
                    .toHex());

            // skip dialog if text of note file on disk and current note are
            // equal
            if (noteTextOnDiskHash == _currentNoteTextHash) {
                qDebug() << __func__
                         << " - Note text and _currentNoteTextHash are the same, ignoring";
                return;
            }

            // fetch current text
            const QString noteTextEditText = this->ui->noteTextEdit->toPlainText();

            // skip dialog if text of note file on disk text from note text
            // edit are equal or similar, but only if the note wasn't edited recently
            // If it was recently edited, we want to show the dialog even for small changes
            if (isCurrentNoteNotEditedForAWhile &&
                Utils::Misc::isSimilar(noteTextEditText, noteTextOnDisk, threshold)) {
                qDebug() << __func__ << " - Note text and text on disk are too similar, ignoring";
                return;
            }

            showStatusBarMessage(tr("Current note was modified externally"), QStringLiteral("🔄"),
                                 5000);

            // if we don't want to get notifications at all
            // external modifications check if we really need one
            if (!this->notifyAllExternalModifications) {
                // reloading the current note text straight away
                // if we didn't change it for a minute
                if (!this->currentNote.getHasDirtyData() && isCurrentNoteNotEditedForAWhile) {
                    updateNoteTextFromDisk(std::move(note));
                    return;
                }
            }

            const int result = openNoteDiffDialog(note);
            switch (result) {
                // overwrite file with local changes
                case NoteDiffDialog::Overwrite: {
                    // disconnect the watcher before saving on disk
                    FileWatchDisabler disable(this);

                    showStatusBarMessage(
                        tr("Overwriting external changes of: %1").arg(currentNote.getFileName()),
                        QStringLiteral("💾"), 3000);

                    // the note text has to be stored newly because the
                    // external change is already in the note table entry
                    currentNote.storeNewText(ui->noteTextEdit->toPlainText());
                    currentNote.storeNoteTextFileToDisk();
                } break;

                // reload note file from disk
                case NoteDiffDialog::Reload:
                    showStatusBarMessage(
                        tr("Loading external changes from: %1").arg(currentNote.getFileName()),
                        QStringLiteral("🔄"), 3000);
                    updateNoteTextFromDisk(note);
                    break;

                    //                case NoteDiffDialog::Cancel:
                    //                case NoteDiffDialog::Ignore:
                default:
                    // do nothing
                    break;
            }
        } else if (_noteExternallyRemovedCheckEnabled && (currentNote.getNoteSubFolderId() == 0)) {
            // only allow the check if current note was removed externally in
            // the root note folder, because it gets triggered every time
            // a note gets renamed in subfolders

            qDebug() << "Current note was removed externally!";

            if (Utils::Gui::questionNoSkipOverride(
                    this, tr("Note was removed externally!"),
                    tr("Current note was removed outside of this application!\n"
                       "Restore current note?"),
                    QStringLiteral("restore-note")) == QMessageBox::Yes) {
                const QSignalBlocker blocker(this->noteDirectoryWatcher);
                Q_UNUSED(blocker)

                QString text = this->ui->noteTextEdit->toPlainText();
                note.storeNewText(std::move(text));

                // store note to disk again
                const bool noteWasStored = note.storeNoteTextFileToDisk();
                showStatusBarMessage(noteWasStored ? tr("Stored current note to disk")
                                                   : tr("Current note could not be stored to disk"),
                                     noteWasStored ? QStringLiteral("💾") : QStringLiteral("❌"),
                                     3000);

                // rebuild and reload the notes directory list
                buildNotesIndexAndLoadNoteDirectoryList();

                // fetch note new (because all the IDs have changed
                // after the buildNotesIndex()
                note.refetch();

                // restore old selected row (but don't update the note text)
                setCurrentNote(note, false);
            } else {
                // rebuild and reload the notes directory list
                buildNotesIndexAndLoadNoteDirectoryList();

                resetCurrentNote(true);
            }
        }
    } else {
        qDebug() << "other note was changed: " << str;

        showStatusBarMessage(tr("Note was modified externally: %1").arg(str), QStringLiteral("🔄"),
                             5000);

        // rebuild and reload the notes directory list
        buildNotesIndexAndLoadNoteDirectoryList();
        setCurrentNote(std::move(this->currentNote), false);
    }
}

void MainWindow::notesDirectoryWasModified(const QString &str) {
    // workaround when signal block doesn't work correctly
    if (_isNotesDirectoryWasModifiedDisabled) {
        return;
    }

    // if we should ignore all changes return here
    if (SettingsService().value(QStringLiteral("ignoreAllExternalNoteFolderChanges")).toBool()) {
        return;
    }

    // We are ignoring changes in the .git folder
    if (str.contains(QStringLiteral("/.git/"))) {
        return;
    }

    qDebug() << "notesDirectoryWasModified: " << str;
    showStatusBarMessage(tr("Notes directory was modified externally"), QStringLiteral("🔄"), 5000);

    // rebuild and reload the notes directory list
    buildNotesIndexAndLoadNoteDirectoryList();

    // check if the current note was modified
    // this fixes not detected external note changes of the current note if the
    // event for the change in the current note comes after the event that the
    // note folder was modified
    QString noteFileName = currentNote.getFileName();
    if (!noteFileName.isEmpty()) {
        // Use the full path, like a filesystem watcher would, instead of just the file-name
        notesWereModified(currentNote.fullNoteFilePath());
    }

    // also update the text of the text edit if current note has changed
    bool updateNoteText = !this->currentNote.exists();
    qDebug() << "updateNoteText: " << updateNoteText;

    // restore old selected row (but don't update the note text)
    setCurrentNote(std::move(this->currentNote), updateNoteText);
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

void MainWindow::storeUpdatedNotesToDisk() {
    // disconnect the watcher before saving on disk
    FileWatchDisabler disable(this);

    const QString oldNoteName = currentNote.getName();

    // For some reason this->noteDirectoryWatcher gets an event from this.
    // I didn't find another solution than to wait yet.
    // All flushing and syncing didn't help.
    bool currentNoteChanged = false;
    bool noteWasRenamed = false;
    bool currentNoteTextChanged = false;

    // currentNote will be set by this method if the filename has changed
    const int count = Note::storeDirtyNotesToDisk(currentNote, &currentNoteChanged, &noteWasRenamed,
                                                  &currentNoteTextChanged);

    if (count > 0) {
        _noteViewNeedsUpdate = true;

        MetricsService::instance()->sendEventIfEnabled(
            QStringLiteral("note/notes/stored"), QStringLiteral("note"),
            QStringLiteral("notes stored"), QString::number(count) + QStringLiteral(" notes"),
            count);

        qDebug() << __func__ << " - 'count': " << count;

        showStatusBarMessage(tr("Stored %n note(s) to disk", "", count), QStringLiteral("💾"),
                             3000);

        if (currentNoteChanged) {
            // strip trailing spaces of the current note (if enabled)
            if (SettingsService().value(QStringLiteral("Editor/removeTrailingSpaces")).toBool()) {
                const bool wasStripped =
                    currentNote.stripTrailingSpaces(activeNoteTextEdit()->textCursor().position());

                if (wasStripped) {
                    qDebug() << __func__ << " - 'wasStripped'";

                    // updating the current note text is disabled because it
                    // moves the cursor to the top
                    //                    const QSignalBlocker blocker2(activeNoteTextEdit());
                    //                    Q_UNUSED(blocker2)
                    //                    setNoteTextFromNote(&currentNote);
                }
            }

            if (currentNoteTextChanged) {
                // reload the current note if we had to change it during a note rename
                reloadCurrentNoteByNoteId(true);
            }

            // just to make sure everything is up-to-date
            currentNote.refetch();

            // create a hash of the text of the current note to be able if it
            // was modified outside of QOwnNotes
            updateCurrentNoteTextHash();

            if (oldNoteName != currentNote.getName()) {
                // just to make sure the window title is set correctly
                updateWindowTitle();

                // update current tab name
                updateCurrentTabData(currentNote);
            }
        }

        if (noteWasRenamed) {
            // reload the directory list if note name has changed
            loadNoteDirectoryList();
        }

        updateNoteGraphicsView();
    }
}

/**
 * Shows alerts for calendar items with an alarm date in the current minute
 * Also checks for expired note crypto keys
 */
void MainWindow::frequentPeriodicChecker() {
    CalendarItem::alertTodoReminders();
    Note::expireCryptoKeys();

    if (QDateTime::currentDateTime().addSecs(-1200) >= _lastHeartbeat) {
        _lastHeartbeat = QDateTime::currentDateTime();
        MetricsService::instance()->sendHeartbeat();
    }

    SettingsService settings;
    QDateTime lastUpdateCheck = settings.value(QStringLiteral("LastUpdateCheck")).toDateTime();
    if (!lastUpdateCheck.isValid()) {
        // set the LastUpdateCheck if it wasn't set
        settings.setValue(QStringLiteral("LastUpdateCheck"), QDateTime::currentDateTime());
    } else if (lastUpdateCheck.addSecs(3600) <= QDateTime::currentDateTime()) {
        // check for updates every 1h
        updateService->checkForUpdates(UpdateService::Periodic);
    }
}

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
    ui->statusBar->addPermanentWidget(_openAiActivitySpinner);

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
    _openAiActivitySpinner = new WaitingSpinnerWidget(0, false, false);
    _openAiActivitySpinner->setNumberOfLines(12);
    _openAiActivitySpinner->setLineLength(5);
    _openAiActivitySpinner->setLineWidth(2);
    _openAiActivitySpinner->setInnerRadius(3);
    _openAiActivitySpinner->setRevolutionsPerSecond(1);
    _openAiActivitySpinner->setToolTip(tr("Waiting for answer from AI"));

    const bool darkMode = SettingsService().value(QStringLiteral("darkMode")).toBool();
    _openAiActivitySpinner->setColor(darkMode ? Qt::white : Qt::black);
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
    QString notePath = Utils::Misc::removeIfEndsWith(this->notesPath, QDir::separator());
    NoteSubFolder noteSubFolder;
    bool hasNoteSubFolder = false;
    bool wasModified = false;

    if (noteSubFolderId == 0) {
        qDebug() << __func__ << " - 'noteSubFolderId': " << noteSubFolderId;

        // make sure we destroy nothing
        storeUpdatedNotesToDisk();

        // init the lists to check for removed items
        _buildNotesIndexBeforeNoteIdList = Note::fetchAllIds();
        _buildNotesIndexBeforeNoteSubFolderIdList = NoteSubFolder::fetchAllIds();
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

    // only show certain files
    auto filters = Note::noteFileExtensionList(QStringLiteral("*."));

    // show the newest entry first
    QStringList files = notesDir.entryList(filters, QDir::Files, QDir::Time);
    qDebug() << __func__ << " - 'files': " << files;

    Note::applyIgnoredNotesSetting(files);
    //    qDebug() << __func__ << " - 'files': " << files;

    bool createDemoNotes = (files.count() == 0) && !hasNoteSubFolder;

    if (createDemoNotes) {
        SettingsService settings;
        // check if we already have created the demo notes once
        createDemoNotes = !settings.value(QStringLiteral("demoNotesCreated")).toBool();

        if (createDemoNotes) {
            // we don't want to create the demo notes again
            settings.setValue(QStringLiteral("demoNotesCreated"), true);
        }
    }

    // add some notes if there aren't any, and we haven't already created them once
    if (createDemoNotes) {
        qDebug() << "No notes! We will add some...";
        const QStringList filenames =
            QStringList({"Markdown Cheatsheet.md", "Welcome to QOwnNotes.md"});

        // copy note files to the notes path
        for (const auto &filename : filenames) {
            const QString destinationFile = this->notesPath + QDir::separator() + filename;
            QFile sourceFile(QStringLiteral(":/demonotes/") + filename);
            sourceFile.copy(destinationFile);
            // set read/write permissions for the owner and user
            QFile::setPermissions(destinationFile, QFile::ReadOwner | QFile::WriteOwner |
                                                       QFile::ReadUser | QFile::WriteUser);
        }

        // copy the shortcuts file and handle its file permissions
        //        destinationFile = this->notesPath + QDir::separator() +
        //              "Important Shortcuts.txt";
        //        QFile::copy( ":/shortcuts", destinationFile );
        //        QFile::setPermissions( destinationFile, QFile::ReadOwner |
        //                  QFile::WriteOwner | QFile::ReadUser |
        //                  QFile::WriteUser );

        // fetch all files again
        files = notesDir.entryList(filters, QDir::Files, QDir::Time);

        // jump to the welcome note in the note selector in 500ms
        QTimer::singleShot(500, this, SLOT(jumpToWelcomeNote()));
    }

    // get the current crypto key to set it again
    // after all notes were read again
    const qint64 cryptoKey = currentNote.getCryptoKey();
    const QString cryptoPassword = currentNote.getCryptoPassword();

    if (!hasNoteSubFolder && forceRebuild) {
        // first delete all notes and note sub folders in the database if a
        // rebuild was forced
        Note::deleteAll();
        NoteSubFolder::deleteAll();
    }

    const bool withNoteNameHook = ScriptingService::instance()->handleNoteNameHookExists();
    const int numFiles = files.count();
    QProgressDialog progress(tr("Loading notes…"), tr("Abort"), 0, numFiles, this);
    progress.setWindowModality(Qt::WindowModal);
    int currentCount = 0;

    _buildNotesIndexAfterNoteIdList.reserve(files.size());
    const int maxNoteFileSize = Utils::Misc::getMaximumNoteFileSize();
    // create all notes from the files
    for (QString fileName : Utils::asConst(files)) {
        if (progress.wasCanceled()) {
            break;
        }

        if (hasNoteSubFolder) {
            fileName.prepend(noteSubFolder.relativePath() + QDir::separator());
        }

        // fetching the content of the file
        QFile file(Note::getFullFilePathForFile(fileName));

        if (file.size() > maxNoteFileSize) {
            qDebug() << "Note file '" << fileName << "' is too large: " << file.size() << " > "
                     << maxNoteFileSize;
            continue;
        }

        // update or create a note from the file
        const Note note = Note::updateOrCreateFromFile(file, noteSubFolder, withNoteNameHook);

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
        // QCoreApplication::flush() is obsolete since Qt 5.9
        //            QCoreApplication::flush();

        // this still causes double entries on OS X and maybe Windows
#ifdef Q_OS_LINUX
        QCoreApplication::sendPostedEvents();
#endif
        progress.setValue(++currentCount);
    }

    progress.setValue(numFiles);

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
    const bool showSubfolders = NoteFolder::isCurrentHasSubfolders();
    if (showSubfolders) {
        const QStringList folders = notesDir.entryList(QDir::Dirs | QDir::Hidden, QDir::Time);

        for (const QString &folder : folders) {
            if (NoteSubFolder::willFolderBeIgnored(folder)) {
                continue;
            }

            // fetch or create the parent note sub folder
            NoteSubFolder parentNoteSubFolder =
                NoteSubFolder::fetchByNameAndParentId(folder, noteSubFolderId);
            if (!parentNoteSubFolder.isFetched()) {
                parentNoteSubFolder.setName(folder);
                parentNoteSubFolder.setParentId(noteSubFolderId);
                parentNoteSubFolder.store();

                wasModified = true;
            }

            if (parentNoteSubFolder.isFetched()) {
                // add the note id to in the end check if notes need to
                // be removed
                _buildNotesIndexAfterNoteSubFolderIdList << parentNoteSubFolder.getId();

                // build the notes index for the note subfolder
                const bool result = buildNotesIndex(parentNoteSubFolder.getId());
                if (result) {
                    wasModified = true;
                }

                // update the UI
                // this causes to show sub note folders twice in the
                // ui->noteSubFolderTreeWidget if a
                // not selected note is modified externally
                //                    QCoreApplication::processEvents();

                // we try these two instead to update the UI
                // QCoreApplication::flush() is obsolete since Qt 5.9
                //                    QCoreApplication::flush();

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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        const QList<int> removedNoteIdList =
            QSet<int>(_buildNotesIndexBeforeNoteIdList.begin(),
                      _buildNotesIndexBeforeNoteIdList.end())
                .subtract(QSet<int>(_buildNotesIndexAfterNoteIdList.begin(),
                                    _buildNotesIndexAfterNoteIdList.end()))
                .values();
#else
        const QList<int> removedNoteIdList = _buildNotesIndexBeforeNoteIdList.toList()
                                                 .toSet()
                                                 .subtract(_buildNotesIndexAfterNoteIdList.toSet())
                                                 .toList();
#endif

        // remove all missing notes
        for (const int noteId : removedNoteIdList) {
            Note note = Note::fetch(noteId);
            if (note.isFetched()) {
                note.remove();
                wasModified = true;
            }
        }

        // check for removed note subfolders
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        QList<int> removedNoteSubFolderIdList =
            QSet<int>(_buildNotesIndexBeforeNoteSubFolderIdList.begin(),
                      _buildNotesIndexBeforeNoteSubFolderIdList.end())
                .subtract(QSet<int>(_buildNotesIndexAfterNoteSubFolderIdList.begin(),
                                    _buildNotesIndexAfterNoteSubFolderIdList.end()))
                .values();
#else
        const QList<int> removedNoteSubFolderIdList =
            _buildNotesIndexBeforeNoteSubFolderIdList.toList()
                .toSet()
                .subtract(_buildNotesIndexAfterNoteSubFolderIdList.toSet())
                .toList();
#endif

        // remove all missing note subfolders
        for (const int _noteSubFolderId : removedNoteSubFolderIdList) {
            NoteSubFolder _noteSubFolder = NoteSubFolder::fetch(_noteSubFolderId);
            if (_noteSubFolder.isFetched()) {
                _noteSubFolder.remove();
                wasModified = true;
            }
        }

        // setup the note folder database
        DatabaseService::createNoteFolderConnection();
        DatabaseService::setupNoteFolderTables();

        // update the note directory watcher
        updateNoteDirectoryWatcher();

        // update the information about shared notes
        OwnCloudService *ownCloud = OwnCloudService::instance();
        ownCloud->fetchShares();
    }

    if (noteSubFolderId == 0) {
        removeConflictedNotesDatabaseCopies();

        // Log checksum calculation statistics for the entire note folder
        qint64 totalTime = 0;
        int callCount = 0;
        Note::getChecksumStats(totalTime, callCount);
        qDebug() << "====================================================";
        qDebug() << "Note folder loading complete - Checksum statistics:";
        qDebug() << "  Total calculateChecksum() calls:" << callCount;
        qDebug() << "  Total time spent:" << (totalTime / 1000000.0) << "ms";
        if (callCount > 0) {
            qDebug() << "  Average time per call:" << (totalTime / callCount / 1000.0)
                     << "microseconds";
        }
        qDebug() << "====================================================";
    }

    return wasModified;
}

/**
 * Asks to remove conflicted copies of the notes.sqlite database
 */
void MainWindow::removeConflictedNotesDatabaseCopies() {
    const QStringList filter{"notes (*conflicted copy *).sqlite"};
    QDirIterator it(NoteFolder::currentLocalPath(), filter,
                    QDir::AllEntries | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    auto files = QStringList();
    const QSignalBlocker blocker(this->noteDirectoryWatcher);
    Q_UNUSED(blocker)

    FileWatchDisabler disable(this);

    while (it.hasNext()) {
        const QString &file = it.next();
        qDebug() << "Found conflicting note folder database: " << file;

        // check if conflicted database copy is the same as the current note
        // folder database
        if (Utils::Misc::isSameFile(file, DatabaseService::getNoteFolderDatabasePath())) {
            showStatusBarMessage(
                QFile::remove(file)
                    ? tr("Removed duplicate conflicted database: %1").arg(file)
                    : tr("Could not remove duplicate conflicted database: %1").arg(file),
                QStringLiteral("🗄️"), 4000);
        } else if (DatabaseService::mergeNoteFolderDatabase(file)) {
            showStatusBarMessage(
                QFile::remove(file)
                    ? tr("Removed merged conflicted database: %1").arg(file)
                    : tr("Could not remove merged conflicted database: %1").arg(file),
                QStringLiteral("🗄️"), 4000);
        } else {
            files << file;
        }
    }

    int count = files.count();

    if (count == 0) {
        return;
    }

    if (Utils::Gui::question(
            this, tr("Delete conflicted database copies"),
            Utils::Misc::replaceOwnCloudText(
                tr("Proceed with automatic deletion of <strong>%n</strong>"
                   " conflicted database copies that may block your ownCloud"
                   " sync process?",
                   "", count)) +
                QStringLiteral("<br /><br />") + files.join(QStringLiteral("<br />")),
            QStringLiteral("delete-conflicted-database-files")) != QMessageBox::Yes) {
        return;
    }

    count = 0;

    // remove the database files
    for (const QString &file : Utils::asConst(files)) {
        if (QFile::remove(file)) {
            ++count;
        }
    }

    showStatusBarMessage(tr("Removed %n conflicted database copies", "", count),
                         QStringLiteral("🗄️"));
}

void MainWindow::addDirectoryToDirectoryWatcher(const QString &path) {
    QDir dir(path);
    QFileInfoList entries =
        dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden);

    for (const QFileInfo &entryInfo : entries) {
        QString entryPath = entryInfo.filePath();
        noteDirectoryWatcher.addPath(entryPath);

        if (entryInfo.isDir()) {
            addDirectoryToDirectoryWatcher(entryPath);    // Recursively add subdirectories
        }
    }
}

/**
 * Updates the note directory watcher
 */
void MainWindow::updateNoteDirectoryWatcher() {
    // clear all paths from the directory watcher
    clearNoteDirectoryWatcher();

    const bool hasSubfolders = NoteFolder::isCurrentHasSubfolders();
    //    if (showSubfolders) {
    //        return;
    //    }

    const QString notePath = Utils::Misc::removeIfEndsWith(this->notesPath, QDir::separator());
    if (QDir(notePath).exists()) {
        // watch the notes directory for changes
        noteDirectoryWatcher.addPath(notePath);
    }

    // Add the .git folder to the watcher if it exists
    const QString gitPath = notePath + QDir::separator() + QStringLiteral(".git");
    if (QDir(gitPath).exists()) {
        addDirectoryToDirectoryWatcher(gitPath);
    }

    if (hasSubfolders) {
        const QVector<NoteSubFolder> noteSubFolderList = NoteSubFolder::fetchAll();
        for (const NoteSubFolder &noteSubFolder : noteSubFolderList) {
            const QString path = notePath + QDir::separator() + noteSubFolder.relativePath();

            QDir folderDir(path);

            if (folderDir.exists()) {
                // watch the note sub folder path for changes
                noteDirectoryWatcher.addPath(path);
            }
        }
    }

    int count = 0;
    const QVector<Note> noteList = Note::fetchAll();
    for (const Note &note : noteList) {
#ifdef Q_OS_LINUX
        // only add the last first 200 notes to the file watcher to
        // prevent that nothing is watched at all because of too many
        // open files
        if (count > 200) {
            break;
        }
#endif
        const QString path = note.fullNoteFilePath();
        const QFile file(path);

        if (file.exists()) {
            // watch the note for changes
            noteDirectoryWatcher.addPath(path);

            ++count;
        }
    }

    //    qDebug() << __func__ << " - 'noteDirectoryWatcher.files()': " <<
    //    noteDirectoryWatcher.files();
    //
    //    qDebug() << __func__ << " - 'noteDirectoryWatcher.directories()': " <<
    //    noteDirectoryWatcher.directories();
}

/**
 * Clears all paths from the directory watcher
 */
void MainWindow::clearNoteDirectoryWatcher() {
    const QStringList fileList = noteDirectoryWatcher.directories() + noteDirectoryWatcher.files();
    if (fileList.count() > 0) {
        noteDirectoryWatcher.removePaths(fileList);
    }
}

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
    if (ui->navigationWidget->isVisible()) {
        ui->navigationWidget->selectItemForCursorPosition(position);
    }
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

    this->_lastNoteId = this->currentNote.getId();
    this->currentNote = note;

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
}

void MainWindow::updateNoteGraphicsView() {
    if (_noteRelationScene && _noteGraphicsViewDockWidget->isVisible()) {
        _noteRelationScene->drawForNote(currentNote);
    }
}

void MainWindow::updateCurrentTabData(const Note &note) const {
    Utils::Gui::updateTabWidgetTabData(ui->noteEditTabWidget, ui->noteEditTabWidget->currentIndex(),
                                       note);
}

void MainWindow::closeOrphanedTabs() const {
    const int maxIndex = ui->noteEditTabWidget->count() - 1;

    for (int i = maxIndex; i >= 0; i--) {
        const int noteId = Utils::Gui::getTabWidgetNoteId(ui->noteEditTabWidget, i);

        if (!Note::noteIdExists(noteId)) {
            removeNoteTab(i);
        }
    }
}

bool MainWindow::jumpToTab(const Note &note) const {
    const int noteId = note.getId();
    const int tabIndexOfNote = getNoteTabIndex(noteId);

    if (tabIndexOfNote == -1) {
        return false;
    }

    ui->noteEditTabWidget->setCurrentIndex(tabIndexOfNote);
    QWidget *widget = ui->noteEditTabWidget->currentWidget();

    if (widget->layout() == nullptr) {
        widget->setLayout(ui->noteEditTabWidgetLayout);
        closeOrphanedTabs();
    }

    return true;
}

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
void MainWindow::removeCurrentNote() {
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    if (Utils::Gui::question(
            this, tr("Remove current note"),
            tr("Remove current note: <strong>%1</strong>?").arg(this->currentNote.getName()),
            QStringLiteral("remove-note")) == QMessageBox::Yes) {
        const QSignalBlocker blocker2(ui->noteTextEdit);
        Q_UNUSED(blocker2)

#ifdef USE_QLITEHTML
        const QSignalBlocker blocker3(_notePreviewWidget);
#else
        const QSignalBlocker blocker3(ui->noteTextView);
#endif
        Q_UNUSED(blocker3)

        const QSignalBlocker blocker4(ui->encryptedNoteTextEdit);
        Q_UNUSED(blocker4)

        const QSignalBlocker blocker5(noteDirectoryWatcher);
        Q_UNUSED(blocker5)

        // we try to fix problems with note subfolders
        directoryWatcherWorkaround(true);

        {
            const QSignalBlocker blocker1(ui->noteTreeWidget);
            Q_UNUSED(blocker1)

            // search and remove note from the note tree widget
            removeNoteFromNoteTreeWidget(currentNote);

            // delete note in database and on file system
            currentNote.remove(true);

            unsetCurrentNote();
        }

        // set a new current note
        resetCurrentNote(false);

        // we try to fix problems with note subfolders
        // we need to wait some time to turn the watcher on again because
        // something is happening after this method that reloads the
        // note folder
        directoryWatcherWorkaround(false);
    }
}

/**
 * Searches and removes note from the note tree widget
 */
void MainWindow::removeNoteFromNoteTreeWidget(Note &note) const {
    auto *item = Utils::Gui::getTreeWidgetItemWithUserData(ui->noteTreeWidget, note.getId());

    if (item != nullptr) {
        delete (item);
    }
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
        if (!isInDistractionFreeMode()) {
            storeCurrentWorkspace();
        }

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
    QTreeWidgetItemIterator it(ui->noteTreeWidget, QTreeWidgetItemIterator::NotHidden);

    return *it;
}

/**
 * Highlights all occurrences of str in the note text edit and does a "in note
 * search"
 */
void MainWindow::searchInNoteTextEdit(QString str) {
    QList<QTextEdit::ExtraSelection> extraSelections;
    QList<QTextEdit::ExtraSelection> extraSelections2;
    QList<QTextEdit::ExtraSelection> extraSelections3;

    if (str.size() >= 2) {
        // do an in-note search
        doSearchInNote(str);
        ui->noteTextEdit->moveCursor(QTextCursor::Start);
#ifndef USE_QLITEHTML
        ui->noteTextView->moveCursor(QTextCursor::Start);
#endif
        ui->encryptedNoteTextEdit->moveCursor(QTextCursor::Start);
        const QColor color = QColor(0, 180, 0, 100);

        // build the string list of the search string
        const QString queryStr = str.replace(QLatin1String("|"), QLatin1String("\\|"));
        const QStringList queryStrings = Note::buildQueryStringList(queryStr, true);

        if (queryStrings.count() > 0) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
            const QRegularExpression regExp(
                QLatin1Char('(') + queryStrings.join(QLatin1String("|")) + QLatin1Char(')'),
                QRegularExpression::CaseInsensitiveOption);
#else
            const QRegExp regExp(
                QLatin1String("(") + queryStrings.join(QLatin1String("|")) + QLatin1String(")"),
                Qt::CaseInsensitive);
#endif
            while (ui->noteTextEdit->find(regExp)) {
                QTextEdit::ExtraSelection extra = QTextEdit::ExtraSelection();
                extra.format.setBackground(color);

                extra.cursor = ui->noteTextEdit->textCursor();
                extraSelections.append(extra);
            }

            // TODO:
#ifdef USE_QLITEHTML
            _notePreviewWidget->findText(str, QTextDocument::FindFlag::FindWholeWords, true);
#else
            while (ui->noteTextView->find(regExp)) {
                QTextEdit::ExtraSelection extra = QTextEdit::ExtraSelection();
                extra.format.setBackground(color);

                extra.cursor = ui->noteTextView->textCursor();
                extraSelections2.append(extra);
            }
#endif
            while (ui->encryptedNoteTextEdit->find(regExp)) {
                QTextEdit::ExtraSelection extra = QTextEdit::ExtraSelection();
                extra.format.setBackground(color);

                extra.cursor = ui->encryptedNoteTextEdit->textCursor();
                extraSelections3.append(extra);
            }
        }
    }

    ui->noteTextEdit->setExtraSelections(extraSelections);
#ifndef USE_QLITEHTML
    ui->noteTextView->setExtraSelections(extraSelections2);
#endif
    ui->encryptedNoteTextEdit->setExtraSelections(extraSelections3);
}

/**
 * highlights all occurrences of the search line text in the note text edit
 */
void MainWindow::searchForSearchLineTextInNoteTextEdit() {
    QString searchString = ui->searchLineEdit->text();

    if (searchString.isEmpty()) {
        activeNoteTextEdit()->searchWidget()->close();
    } else {
        searchInNoteTextEdit(std::move(searchString));
    }
}

/**
 * Asks for the password if the note is encrypted and can't be decrypted
 */
void MainWindow::askForEncryptedNotePasswordIfNeeded(const QString &additionalText) {
    currentNote.refetch();

    // check if the note is encrypted and can't be decrypted
    if (currentNote.hasEncryptedNoteText() && !currentNote.canDecryptNoteText()) {
        QString labelText =
            tr("Please enter the <strong>password</strong> "
               "of this encrypted note.");

        if (!additionalText.isEmpty()) {
            labelText += QStringLiteral(" ") + additionalText;
        }

        auto *dialog = new PasswordDialog(this, labelText);
        const int dialogResult = dialog->exec();

        // if user pressed ok take the password
        if (dialogResult == QDialog::Accepted) {
            const QString password = dialog->password();
            if (!password.isEmpty()) {
                // set the password so it can be decrypted
                // for the Markdown view
                currentNote.setCryptoPassword(password);
                currentNote.store();
            }

            // warn if password is incorrect
            if (!currentNote.canDecryptNoteText()) {
                QMessageBox::warning(this, tr("Note can't be decrypted!"),
                                     tr("It seems that your password is not valid!"));
            }
        }

        delete (dialog);
    }
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
        qobject_cast<QOwnNotesMarkdownHighlighter *>(ui->noteTextEdit->highlighter())
            ->updateCurrentNote(note);
        ui->noteTextEdit->setText(note->getNoteText());
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
void MainWindow::startNavigationParser() {
    if (ui->navigationWidget->isVisible()) {
        ui->navigationWidget->parse(activeNoteTextEdit()->document(),
                                    activeNoteTextEdit()->textCursor().position());
    } else if (ui->backlinkWidget->isVisible()) {
        ui->backlinkWidget->findBacklinks(currentNote);
    }
}

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
    const QString extension = Note::defaultNoteFileExtension();
    auto *f =
        new QFile(this->notesPath + QDir::separator() + name + QStringLiteral(".") + extension);
    const bool useNameAsHeadline = options.testFlag(CreateNewNoteOption::UseNameAsHeadline);

    // change the name and headline if note exists
    if (f->exists()) {
        QDateTime currentDate = QDateTime::currentDateTime();
        name.append(
            QStringLiteral(" ") +
            currentDate.toString(Qt::ISODate).replace(QStringLiteral(":"), QStringLiteral(".")));

        if (!useNameAsHeadline) {
            QString preText = Note::createNoteHeader(name);
            text.prepend(preText);
        }
    }

    // create a new note
    ui->searchLineEdit->setText(name);

    jumpToNoteOrCreateNew(options.testFlag(CreateNewNoteOption::DisableLoadNoteDirectoryList));

    // check if to append the text or replace the text of the note
    if (useNameAsHeadline) {
        QTextCursor c = ui->noteTextEdit->textCursor();
        // make sure the cursor is really at the end to be able to
        // insert the text on the correct position
        c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        c.insertText(QStringLiteral("\n\n") + text);
        ui->noteTextEdit->setTextCursor(c);
    } else {
        ui->noteTextEdit->setText(text);
    }

    // move the cursor to the end of the note
    if (options.testFlag(CreateNewNoteOption::CursorAtEnd)) {
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
void MainWindow::restoreTrashedNoteOnServer(const QString &fileName, int timestamp) {
    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->restoreTrashedNoteOnServer(fileName, timestamp);
}

/**
 * @brief Removes selected notes after a confirmation
 */
void MainWindow::removeSelectedNotes() {
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    const auto selItems = ui->noteTreeWidget->selectedItems();
    const int selectedItemsCount = selItems.count();

    if (selectedItemsCount == 0) {
        return;
    }

    // Separate notes and folders from selected items
    QVector<QTreeWidgetItem *> noteItems;
    QVector<QTreeWidgetItem *> folderItems;
    QStringList noteSubFolderPathList;
    QVector<NoteSubFolder> noteSubFolderList;

    for (QTreeWidgetItem *item : selItems) {
        const int itemType = item->data(0, Qt::UserRole + 1).toInt();
        if (itemType == NoteType) {
            noteItems.append(item);
        } else if (itemType == FolderType) {
            folderItems.append(item);
            const int id = item->data(0, Qt::UserRole).toInt();
            const NoteSubFolder noteSubFolder = NoteSubFolder::fetch(id);
            if (noteSubFolder.isFetched()) {
                noteSubFolderList.append(noteSubFolder);
                noteSubFolderPathList.append(noteSubFolder.fullPath());
            }
        }
    }

    const int noteCount = noteItems.count();
    const int folderCount = noteSubFolderList.count();

    if (noteCount == 0 && folderCount == 0) {
        return;
    }

    // Build confirmation message based on what's selected
    QString title;
    QString message;
    QString dialogName;

    if (noteCount > 0 && folderCount > 0) {
        // Both notes and folders selected
        title = tr("Remove selected notes and folders");
        message = Utils::Misc::replaceOwnCloudText(
            tr("Remove <strong>%n</strong> selected note(s) and "
               "<strong>%1</strong> folder(s)?"
               "<ul><li>%2</li></ul>"
               "All files and folders in these folders will be removed as well!\n\n"
               "If the trash is enabled on your ownCloud server you should be able to restore "
               "the notes from there.",
               "", noteCount)
                .arg(folderCount)
                .arg(noteSubFolderPathList.join(QStringLiteral("</li><li>"))));
        dialogName = QStringLiteral("remove-notes-and-folders");
    } else if (folderCount > 0) {
        // Only folders selected
        title = tr("Remove selected folders");
        message = tr("Remove <strong>%n</strong> selected folder(s)?"
                     "<ul><li>%1</li></ul>"
                     "All files and folders in these folders will be removed as well!",
                     "", folderCount)
                      .arg(noteSubFolderPathList.join(QStringLiteral("</li><li>")));
        dialogName = QStringLiteral("remove-folders");
    } else {
        // Only notes selected
        title = tr("Remove selected notes");
        message = Utils::Misc::replaceOwnCloudText(
            tr("Remove <strong>%n</strong> selected note(s)?\n\n"
               "If the trash is enabled on your ownCloud server you should be able to restore "
               "them from there.",
               "", noteCount));
        dialogName = QStringLiteral("remove-notes");
    }

    if (Utils::Gui::question(this, title, message, dialogName) == QMessageBox::Yes) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        const QSignalBlocker blocker2(activeNoteTextEdit());
        Q_UNUSED(blocker2)

#ifndef USE_QLITEHTML
        const QSignalBlocker blocker3(ui->noteTextView);
#else
        const QSignalBlocker blocker3(_notePreviewWidget);
#endif
        Q_UNUSED(blocker3)

        const QSignalBlocker blocker4(ui->encryptedNoteTextEdit);
        Q_UNUSED(blocker4)

        // we try to fix problems with note subfolders
        directoryWatcherWorkaround(true);

        {
            const QSignalBlocker blocker1(ui->noteTreeWidget);
            Q_UNUSED(blocker1)

            // Remove notes
            for (QTreeWidgetItem *item : noteItems) {
                const int id = item->data(0, Qt::UserRole).toInt();
                Note note = Note::fetch(id);

                // search and remove note from the note tree widget
                removeNoteFromNoteTreeWidget(note);

                note.remove(true);
                qDebug() << "Removed note " << note.getName();
            }

            // Remove folders
            for (const auto &noteSubFolder : Utils::asConst(noteSubFolderList)) {
                // remove the directory recursively from the file system
                if (noteSubFolder.removeFromFileSystem()) {
                    showStatusBarMessage(
                        tr("Removed note subfolder: %1").arg(noteSubFolder.fullPath()),
                        QStringLiteral("📁"));
                }
            }

            // clear the text edit so it stays clear after removing notes
            // (either directly or via folder deletion)
            activeNoteTextEdit()->clear();
        }

        // set a new current note (needed whether notes or folders were deleted,
        // as folders may contain the currently displayed note)
        resetCurrentNote(false);

        // we try to fix problems with note subfolders
        // we need to wait some time to turn the watcher on again because
        // something is happening after this method that reloads the note folder
        directoryWatcherWorkaround(false);

        // Reload note folder if folders were deleted
        if (folderCount > 0) {
            reloadNoteFolderAction()->trigger();
        }
    }

    loadNoteDirectoryList();
}

/**
 * Removes selected tags after a confirmation
 */
void MainWindow::removeSelectedTags() {
    const int selectedItemsCount = ui->tagTreeWidget->selectedItems().size();

    if (selectedItemsCount == 0) {
        return;
    }

    if (Utils::Gui::question(this, tr("Remove selected tags"),
                             tr("Remove <strong>%n</strong> selected tag(s)? No notes will "
                                "be removed in this process.",
                                "", selectedItemsCount),
                             QStringLiteral("remove-tags")) == QMessageBox::Yes) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        const QSignalBlocker blocker1(ui->tagTreeWidget);
        Q_UNUSED(blocker1)

        // workaround when signal blocking doesn't work correctly
        directoryWatcherWorkaround(true, true);

        const auto selItems = ui->tagTreeWidget->selectedItems();
        for (QTreeWidgetItem *item : selItems) {
            const int tagId = item->data(0, Qt::UserRole).toInt();
            const Tag tag = Tag::fetch(tagId);

            // take care that the tag is removed from all notes
            handleScriptingNotesTagRemoving(tag, true);

            // remove tag after handled by scripts so it still can be accessed by them
            tag.remove();
            qDebug() << "Removed tag " << tag.getName();
        }

        if (ScriptingService::instance()->noteTaggingHookExists()) {
            storeUpdatedNotesToDisk();
        }

        // disable workaround
        directoryWatcherWorkaround(false, true);

        reloadCurrentNoteTags();
        reloadTagTree();
    }
}

/**
 * @brief Select all notes
 */
void MainWindow::selectAllNotes() { ui->noteTreeWidget->selectAll(); }

/**
 * @brief Moves selected notes after a confirmation
 * @param destinationFolder
 */
void MainWindow::moveSelectedNotesToFolder(const QString &destinationFolder) {
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    const int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();

    if (Utils::Gui::question(
            this, tr("Move selected notes"),
            tr("Move %n selected note(s) to <strong>%2</strong>?", "", selectedItemsCount)
                .arg(destinationFolder),
            QStringLiteral("move-notes")) == QMessageBox::Yes) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        const auto selectedItems = ui->noteTreeWidget->selectedItems();
        for (QTreeWidgetItem *item : selectedItems) {
            if (item->data(0, Qt::UserRole + 1) != NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            Note note = Note::fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            // remove note path form directory watcher
            this->noteDirectoryWatcher.removePath(note.fullNoteFilePath());

            if (note.getId() == currentNote.getId()) {
                // unset the current note
                unsetCurrentNote();
            }

            // move note
            const bool result = note.moveToPath(destinationFolder);
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
 * Returns a list of all selected notes
 *
 * @return
 */
QVector<Note> MainWindow::selectedNotes() {
    QVector<Note> selectedNotes;

    const auto selectedItems = ui->noteTreeWidget->selectedItems();
    for (QTreeWidgetItem *item : selectedItems) {
        if (item->data(0, Qt::UserRole + 1) != NoteType) {
            continue;
        }

        const int noteId = item->data(0, Qt::UserRole).toInt();
        const Note note = Note::fetch(noteId);

        if (note.isFetched()) {
            selectedNotes << note;
        }
    }

    return selectedNotes;
}

/**
 * Un-sets the current note
 */
void MainWindow::unsetCurrentNote() {
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

/**
 * @brief Copies selected notes after a confirmation
 * @param destinationFolder
 */
void MainWindow::copySelectedNotesToFolder(const QString &destinationFolder,
                                           const QString &noteFolderPath) {
    int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();

    if (Utils::Gui::question(
            this, tr("Copy selected notes"),
            tr("Copy %n selected note(s) to <strong>%2</strong>?", "", selectedItemsCount)
                .arg(destinationFolder),
            QStringLiteral("copy-notes")) == QMessageBox::Yes) {
        int copyCount = 0;
        const auto selectedItems = ui->noteTreeWidget->selectedItems();
        for (QTreeWidgetItem *item : selectedItems) {
            if (item->data(0, Qt::UserRole + 1) != NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            Note note = Note::fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            // copy note
            const bool result = note.copyToPath(destinationFolder, noteFolderPath);
            if (result) {
                copyCount++;
                qDebug() << "Note was copied:" << note.getName();
            } else {
                qWarning() << "Could not copy note:" << note.getName();
            }
        }

        Utils::Gui::information(this, tr("Done"),
                                tr("%n note(s) were copied to <strong>%2</strong>.", "", copyCount)
                                    .arg(destinationFolder),
                                QStringLiteral("notes-copied"));
    }
}

/**
 * Tags selected notes
 */
void MainWindow::tagSelectedNotes(const Tag &tag) {
    const int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();

    if (Utils::Gui::question(
            this, tr("Tag selected notes"),
            tr("Tag %n selected note(s) with <strong>%2</strong>?", "", selectedItemsCount)
                .arg(tag.getName()),
            QStringLiteral("tag-notes")) == QMessageBox::Yes) {
        int tagCount = 0;
        const bool useScriptingEngine = ScriptingService::instance()->noteTaggingHookExists();

        // workaround when signal block doesn't work correctly
        directoryWatcherWorkaround(true, true);

        const auto selectedItems = ui->noteTreeWidget->selectedItems();
        for (QTreeWidgetItem *item : selectedItems) {
            if (item->data(0, Qt::UserRole + 1) != NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            const Note note = Note::fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            const QSignalBlocker blocker(noteDirectoryWatcher);
            Q_UNUSED(blocker)

            if (useScriptingEngine) {
                // add the tag to the note text if defined via
                // scripting engine
                handleScriptingNoteTagging(note, tag, false, false);
            }

            // tag note
            const bool result = tag.linkToNote(note);

            if (result) {
                tagCount++;
                qDebug() << "Note was tagged:" << note.getName();

                // handle the coloring of the note in the note tree widget
                handleNoteTreeTagColoringForNote(note);
            } else {
                qWarning() << "Could not tag note:" << note.getName();
            }
        }

        if (useScriptingEngine) {
            const QSignalBlocker blocker(this->noteDirectoryWatcher);
            Q_UNUSED(blocker)

            storeUpdatedNotesToDisk();
        }

        reloadCurrentNoteTags();
        reloadTagTree();

        showStatusBarMessage(
            tr("%n note(s) were tagged with \"%2\"", "", tagCount).arg(tag.getName()),
            QStringLiteral("🏷️"), 5000);

        // turn off the workaround again
        directoryWatcherWorkaround(false, true);
    }
}

/**
 * Removes a tag from the selected notes
 */
void MainWindow::removeTagFromSelectedNotes(const Tag &tag) {
    const int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();

    if (Utils::Gui::question(
            this, tr("Remove tag from selected notes"),
            tr("Remove tag <strong>%1</strong> from %n selected note(s)?", "", selectedItemsCount)
                .arg(tag.getName()),
            QStringLiteral("remove-tag-from-notes")) == QMessageBox::Yes) {
        int tagCount = 0;
        const bool useScriptingEngine = ScriptingService::instance()->noteTaggingHookExists();

        // workaround when signal blocking doesn't work correctly
        directoryWatcherWorkaround(true, true);

        const auto selectedItems = ui->noteTreeWidget->selectedItems();
        for (auto *item : selectedItems) {
            if (item->data(0, Qt::UserRole + 1) != NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            const Note note = Note::fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            const QSignalBlocker blocker(noteDirectoryWatcher);
            Q_UNUSED(blocker)

            if (useScriptingEngine) {
                // take care that the tag is removed from the note
                handleScriptingNoteTagging(note, tag, true, false);
            }

            // tag note
            const bool result = tag.removeLinkToNote(note);

            if (result) {
                tagCount++;
                qDebug() << "Tag was removed from note:" << note.getName();

                // handle the coloring of the note in the note tree widget
                handleNoteTreeTagColoringForNote(note);
            } else {
                qWarning() << "Could not remove tag from note:" << note.getName();
            }
        }

        if (useScriptingEngine) {
            const QSignalBlocker blocker(noteDirectoryWatcher);
            Q_UNUSED(blocker)

            storeUpdatedNotesToDisk();
        }

        reloadCurrentNoteTags();
        reloadTagTree();
        filterNotesByTag();

        Utils::Gui::information(
            this, tr("Done"),
            tr("Tag <strong>%1</strong> was removed from %n note(s)", "", tagCount)
                .arg(tag.getName()),
            QStringLiteral("tag-removed-from-notes"));

        // turn off the workaround again
        directoryWatcherWorkaround(false, true);
    }
}

/**
 * Activates or deactivates a workaround for the ill behaving directory watcher
 *
 * @param isNotesDirectoryWasModifiedDisabled
 * @param alsoHandleNotesWereModified
 */
void MainWindow::directoryWatcherWorkaround(bool isNotesDirectoryWasModifiedDisabled,
                                            bool alsoHandleNotesWereModified) {
    if (!isNotesDirectoryWasModifiedDisabled) {
        Utils::Misc::waitMsecs(200);
    }

    _isNotesDirectoryWasModifiedDisabled = isNotesDirectoryWasModifiedDisabled;

    if (alsoHandleNotesWereModified) {
        _isNotesWereModifiedDisabled = isNotesDirectoryWasModifiedDisabled;
    }
}

/**
 * Handle the coloring of the note in the note tree widget
 *
 * @param note
 */
void MainWindow::handleNoteTreeTagColoringForNote(const Note &note) {
    const Tag colorTag = Tag::fetchOneOfNoteWithColor(note);
    QTreeWidgetItem *noteItem = findNoteInNoteTreeWidget(note);
    Utils::Gui::handleTreeWidgetItemTagColor(noteItem, colorTag);
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
    Utils::Misc::loadPrinterSettings(printer, QStringLiteral("Printer/NotePrinting"));

    QPrintDialog dialog(printer, this);
    dialog.setWindowTitle(tr("Print note"));
    const int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return false;
    }

    Utils::Misc::storePrinterSettings(printer, QStringLiteral("Printer/NotePrinting"));
    return true;
}

/**
 * @brief Prints the content of a text document
 * @param textEdit
 */
void MainWindow::printTextDocument(QTextDocument *textDocument) {
    QPrinter printer;
    if (preparePrintNotePrinter(&printer)) {
        textDocument->print(&printer);
    }
}

/**
 * @brief Prepares the printer dialog to exports the content of a text edit
 *        widget as PDF
 * @param printer
 */
bool MainWindow::prepareExportNoteAsPDFPrinter(QPrinter *printer) {
#ifdef Q_OS_LINUX
    Utils::Misc::loadPrinterSettings(printer, QStringLiteral("Printer/NotePDFExport"));

    // Ensure color mode is always set to Color for PDF export
    printer->setColorMode(QPrinter::Color);

    // under Linux we use the QPageSetupDialog to change layout
    // settings of the PDF export
    QPageSetupDialog pageSetupDialog(printer, this);

    if (pageSetupDialog.exec() != QDialog::Accepted) {
        return false;
    }

    // Ensure color mode is still set to Color before storing settings
    printer->setColorMode(QPrinter::Color);

    Utils::Misc::storePrinterSettings(printer, QStringLiteral("Printer/NotePDFExport"));
#else
    // under OS X and Windows the QPageSetupDialog dialog doesn't work,
    // we will use a workaround to select page sizes and the orientation

    // Ensure color mode is always set to Color for PDF export
    printer->setColorMode(QPrinter::Color);

    SettingsService settings;

    // select the page size
    QStringList pageSizeStrings;
    pageSizeStrings << QStringLiteral("A0") << QStringLiteral("A1") << QStringLiteral("A2")
                    << QStringLiteral("A3") << QStringLiteral("A4") << QStringLiteral("A5")
                    << QStringLiteral("A6") << QStringLiteral("A7") << QStringLiteral("A8")
                    << QStringLiteral("A9") << tr("Letter");
    QList<QPageSize::PageSizeId> pageSizes;
    pageSizes << QPageSize::A0 << QPageSize::A1 << QPageSize::A2 << QPageSize::A3 << QPageSize::A4
              << QPageSize::A5 << QPageSize::A6 << QPageSize::A7 << QPageSize::A8 << QPageSize::A9
              << QPageSize::Letter;

    bool ok;
    QString pageSizeString = QInputDialog::getItem(
        this, tr("Page size"), tr("Page size:"), pageSizeStrings,
        settings.value(QStringLiteral("Printer/NotePDFExportPageSize"), 4).toInt(), false, &ok);

    if (!ok || pageSizeString.isEmpty()) {
        return false;
    }

    int pageSizeIndex = pageSizeStrings.indexOf(pageSizeString);
    if (pageSizeIndex == -1) {
        return false;
    }

    QPageSize pageSize(pageSizes.at(pageSizeIndex));
    settings.setValue(QStringLiteral("Printer/NotePDFExportPageSize"), pageSizeIndex);
    printer->setPageSize(pageSize);

    // select the orientation
    QStringList orientationStrings;
    orientationStrings << tr("Portrait") << tr("Landscape");
    QList<QPageLayout::Orientation> orientations;
    orientations << QPageLayout::Portrait << QPageLayout::Landscape;

    QString orientationString = QInputDialog::getItem(
        this, tr("Orientation"), tr("Orientation:"), orientationStrings,
        settings.value(QStringLiteral("Printer/NotePDFExportOrientation"), 0).toInt(), false, &ok);

    if (!ok || orientationString.isEmpty()) {
        return false;
    }

    int orientationIndex = orientationStrings.indexOf(orientationString);
    if (orientationIndex == -1) {
        return false;
    }

    printer->setPageOrientation(orientations.at(orientationIndex));
    settings.setValue(QStringLiteral("Printer/NotePDFExportOrientation"), orientationIndex);
#endif

    FileDialog dialog(QStringLiteral("NotePDFExport"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("PDF files") + QStringLiteral(" (*.pdf)"));
    dialog.setWindowTitle(tr("Export current note as PDF"));
    dialog.selectFile(currentNote.getName() + QStringLiteral(".pdf"));
    int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return false;
    }

    QString fileName = dialog.selectedFile();

    if (fileName.isEmpty()) {
        return false;
    }

    if (QFileInfo(fileName).suffix().isEmpty()) {
        fileName.append(QLatin1String(".pdf"));
    }

    // Ensure color mode is set to Color right before setting output format
    printer->setColorMode(QPrinter::Color);
    printer->setOutputFormat(QPrinter::PdfFormat);
    printer->setOutputFileName(fileName);
    return true;
}

/**
 * @brief Exports the content of a plain text edit widget as PDF
 * @param textEdit
 */
void MainWindow::exportNoteAsPDF(QPlainTextEdit *textEdit) {
    exportNoteAsPDF(textEdit->document());
}

/**
 * @brief Exports the document as PDF
 * @param doc
 */
void MainWindow::exportNoteAsPDF(QTextDocument *doc) {
    auto *printer = new QPrinter(QPrinter::HighResolution);
    printer->setColorMode(QPrinter::Color);

    if (prepareExportNoteAsPDFPrinter(printer)) {
        doc->print(printer);
        Utils::Misc::openFolderSelect(printer->outputFileName());
    }

    delete printer;
}

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
    Q_UNUSED(arg1)
    filterNotes();
}

/**
 * Does the note filtering
 */
void MainWindow::filterNotes(bool searchForText) {
    ui->noteTreeWidget->scrollToTop();

    // filter the notes by text in the search line edit
    filterNotesBySearchLineEditText(searchForText);

    if (NoteFolder::isCurrentShowSubfolders() && !_showNotesFromAllNoteSubFolders) {
        // filter the notes by note sub folder
        filterNotesByNoteSubFolders();
    }

    // moved condition whether to filter notes by tag at all into
    // filterNotesByTag() -- it can now be used as a slot at startup
    filterNotesByTag();

    if (searchForText) {
        // let's highlight the text from the search line edit
        searchForSearchLineTextInNoteTextEdit();

        // prevent that the last occurrence of the search term is found
        // first, instead the first occurrence should be found first
        ui->noteTextEdit->searchWidget()->doSearchDown();
    }
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
    return _noteEditIsCentralWidget ? true : _noteEditDockWidget->isVisible();
}

/**
 * Does the note filtering by text in the search line edit
 */
void MainWindow::filterNotesBySearchLineEditText(bool searchInNote) {
    const QString searchText = ui->searchLineEdit->text();

    QTreeWidgetItemIterator it(ui->noteTreeWidget);
    ui->noteTreeWidget->setColumnCount(1);

    // search notes when at least 2 characters were entered
    if (searchText.size() >= 2) {
        if (searchInNote) {
            // open search dialog
            doSearchInNote(searchText);
        }

        QVector<int> noteIdList = Note::searchInNotes(
            searchText, _showNotesFromAllNoteSubFolders ||
                            NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively());

        int columnWidth = ui->noteTreeWidget->columnWidth(0);
        ui->noteTreeWidget->setColumnCount(2);
        int maxWidth = 0;
        const QStringList searchTextTerms = Note::buildQueryStringList(searchText);
        const SettingsService settings;
        const bool showMatches = settings.value(QStringLiteral("showMatches"), true).toBool();

        while (*it) {
            QTreeWidgetItem *item = *it;

            // skip note folders (if they are also shown in the note list)
            if (item->data(0, Qt::UserRole + 1) != NoteType) {
                ++it;
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            bool isHidden = noteIdList.indexOf(noteId) < 0;

            // hide all filtered notes
            item->setHidden(isHidden);

            // count occurrences of search terms in notes
            if (!isHidden && showMatches) {
                const Note note = Note::fetch(noteId);
                item->setForeground(1, QColor(Qt::gray));
                int count = 0;

                for (QString word : searchTextTerms) {
                    if (Note::isNameSearch(word)) {
                        word = Note::removeNameSearchPrefix(word);
                    }

                    count += note.countSearchTextInNote(word);
                }

                const QString text = QString::number(count);
                item->setText(1, text);

                const QString &toolTipText =
                    searchTextTerms.count() == 1
                        ? tr("Found <strong>%n</strong> occurrence(s) of "
                             "<strong>%1</strong>",
                             "", count)
                              .arg(searchText)
                        : tr("Found <strong>%n</strong> occurrence(s) of any "
                             "term of <strong>%1</strong>",
                             "", count)
                              .arg(searchText);
                item->setToolTip(1, toolTipText);

                // calculate the size of the search count column
                QFontMetrics fm(item->font(1));

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
                maxWidth = std::max(maxWidth, fm.width(text));
#else
                maxWidth = std::max(maxWidth, fm.horizontalAdvance(text));
#endif
            }

            ++it;
        }

        // resize the column 0, so we can see the search counts
        columnWidth = std::max(10, columnWidth - maxWidth - 5);
        //        ui->noteTreeWidget->resizeColumnToContents(1);
        ui->noteTreeWidget->setColumnWidth(0, columnWidth);
        ui->noteTreeWidget->setColumnWidth(1, maxWidth);
    } else {
        // otherwise show all items
        while (*it) {
            (*it)->setHidden(false);
            ++it;
        }
    }
}

/**
 * Opens the search widget in the current note and searches for all
 * occurrences of the words in the search text
 *
 * @param searchText
 */
void MainWindow::doSearchInNote(QString searchText) {
    const QStringList searchTextTerms = Note::buildQueryStringList(searchText, true, true);

    if (searchTextTerms.count() > 1) {
        QString localSearchTerm =
            QStringLiteral("(") + searchTextTerms.join(QStringLiteral("|")) + QStringLiteral(")");
        activeNoteTextEdit()->doSearch(localSearchTerm,
                                       QPlainTextEditSearchWidget::RegularExpressionMode);
    } else {
        if (Note::isNameSearch(searchText)) {
            searchText = Note::removeNameSearchPrefix(searchText);
        }

        activeNoteTextEdit()->doSearch(searchText.remove(QStringLiteral("\"")));
    }
}

/**
 * Does the note filtering by tags
 */
void MainWindow::filterNotesByTag() {
    if (!isTagsEnabled()) {
        return;    // do nothing
    }

    const int tagId = Tag::activeTagId();
    QVector<int> noteIdList;

    switch (tagId) {
        case Tag::AllNotesId:
            // don't do any additional filtering here
            return;
        case Tag::AllUntaggedNotesId:
            // get all note names that are not tagged
            noteIdList = Note::fetchAllNotTaggedIds();
            break;
        default:
            // check for multiple active;
            const auto selectedItems = ui->tagTreeWidget->selectedItems();
            QVector<int> tagIds;
            Tag activeTag;

            if (selectedItems.count() > 1) {
                tagIds.reserve(selectedItems.count());
                for (auto *i : selectedItems) {
                    const int id = i->data(0, Qt::UserRole).toInt();
                    tagIds << id;
                }
            } else {
                // check if there is an active tag
                activeTag = Tag::activeTag();
                if (!activeTag.isFetched()) {
                    return;
                }
                tagIds << activeTag.getId();
            }

            QVector<int> tagIdList;
            if (Tag::isTaggingShowNotesRecursively()) {
                tagIdList.reserve(tagIds.count());
                for (const int tId : Utils::asConst(tagIds)) {
                    tagIdList << Tag::fetchTagIdsRecursivelyByParentId(tId);
                }
            } else {
                tagIdList = std::move(tagIds);
            }

            qDebug() << __func__ << " - 'tags': " << tagIds;

            const auto selectedFolderItems = ui->noteSubFolderTreeWidget->selectedItems();

            const bool showNotesFromAllNoteSubFolders = _showNotesFromAllNoteSubFolders;
            noteIdList.reserve(tagIdList.count() * 2);
            if (selectedFolderItems.count() > 1) {
                for (const int tagId_ : Utils::asConst(tagIdList)) {
                    for (const QTreeWidgetItem *i : selectedFolderItems) {
                        const int id = i->data(0, Qt::UserRole).toInt();
                        const NoteSubFolder folder = NoteSubFolder::fetch(id);

                        noteIdList << Tag::fetchAllLinkedNoteIdsForFolder(
                            tagId_, folder, showNotesFromAllNoteSubFolders);
                    }
                }
            } else {
                for (const int tagId_ : Utils::asConst(tagIdList)) {
                    noteIdList << Tag::fetchAllLinkedNoteIds(tagId_,
                                                             showNotesFromAllNoteSubFolders);
                }
            }
            break;
    }

    qDebug() << __func__ << " - 'noteIdList': " << noteIdList;

    // omit the already hidden notes
    QTreeWidgetItemIterator it(ui->noteTreeWidget, QTreeWidgetItemIterator::NotHidden);

    // loop through all visible notes
    while (*it) {
        if ((*it)->data(0, Qt::UserRole + 1) != NoteType) {
            ++it;
            continue;
        }

        // hide all notes that are not linked to the active tag
        // note subfolder are not taken into account here (note names are now
        // not unique), but it should be ok because they are filtered by
        // filterNotesByNoteSubFolders
        if (!noteIdList.contains((*it)->data(0, Qt::UserRole).toInt())) {
            (*it)->setHidden(true);
        }

        ++it;
    }
}

/**
 * Does the note filtering by note sub folders
 */
void MainWindow::filterNotesByNoteSubFolders() {
    const auto selectedItems = ui->noteSubFolderTreeWidget->selectedItems();

    // get all the folder ids
    QVector<int> selectedNoteSubFolderIds;
    selectedNoteSubFolderIds.reserve(selectedItems.count());
    if (selectedItems.count() > 1) {
        for (QTreeWidgetItem *i : selectedItems) {
            selectedNoteSubFolderIds << i->data(0, Qt::UserRole).toInt();
        }
    } else {
        selectedNoteSubFolderIds << NoteSubFolder::activeNoteSubFolderId();
    }

    QVector<int> noteSubFolderIds;
    noteSubFolderIds.reserve(selectedNoteSubFolderIds.count());
    // check if the notes should be viewed recursively
    if (NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively()) {
        for (int subFolId : Utils::asConst(selectedNoteSubFolderIds)) {
            noteSubFolderIds << NoteSubFolder::fetchIdsRecursivelyByParentId(subFolId);
        }
    } else {
        noteSubFolderIds << selectedNoteSubFolderIds;
    }

    qDebug() << __func__ << " - 'noteSubFolderIds': " << noteSubFolderIds;

    // get the notes from the subfolders
    QVector<int> noteIdList;
    noteIdList.reserve(noteSubFolderIds.count());
    for (int noteSubFolderId : Utils::asConst(noteSubFolderIds)) {
        // get all notes of a note sub folder
        noteIdList << Note::fetchAllIdsByNoteSubFolderId(noteSubFolderId);
    }

    // omit the already hidden notes
    QTreeWidgetItemIterator it(ui->noteTreeWidget, QTreeWidgetItemIterator::NotHidden);

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
    if (!Utils::Gui::enableDockWidgetQuestion(_noteSearchDockWidget)) {
        return;
    }

    // Search for the selected text if there is any
    const auto selectedText = activeNoteTextEdit()->textCursor().selectedText();
    if (!selectedText.isEmpty()) {
        this->ui->searchLineEdit->setText(selectedText);
    }

    changeDistractionFreeMode(false);
    this->ui->searchLineEdit->setFocus();
    this->ui->searchLineEdit->selectAll();
}

//
// jump to found note or create a new one if not found
//
void MainWindow::on_searchLineEdit_returnPressed() { jumpToNoteOrCreateNew(); }

/**
 * Jumps to found note or create a new one if not found
 */
void MainWindow::jumpToNoteOrCreateNew(bool disableLoadNoteDirectoryList) {
    // ignore if `return` was pressed in the completer
    if (_searchLineEditFromCompleter) {
        _searchLineEditFromCompleter = false;
        return;
    }

    const QString text = ui->searchLineEdit->text().trimmed();

    // prevent creation of broken note text files
    if (text.isEmpty()) {
        return;
    }

    // this doesn't seem to work with note sub folders
    const QSignalBlocker blocker(noteDirectoryWatcher);
    Q_UNUSED(blocker)

    // add the current search text to the saved searches
    storeSavedSearch();

    // clear search line edit so all notes will be viewed again and to prevent
    // a brief appearing of the note search widget when creating a new note
    // with action_New_note
    ui->searchLineEdit->clear();

    // first let us search for the entered text
    Note note = Note::fetchByName(text);

    // if we can't find a note we create a new one
    if (note.getId() == 0) {
        // Allow note editing if it was disabled
        allowNoteEditing();

        // check if a hook wants to set the text
        QString noteText = ScriptingService::instance()->callHandleNewNoteHeadlineHook(text);

        // check if a hook changed the text
        if (noteText.isEmpty()) {
            // fallback to the old text if no hook changed the text
            noteText = Note::createNoteHeader(text);
        } else {
            noteText.append(QLatin1String("\n\n"));
        }

        const NoteSubFolder noteSubFolder = NoteSubFolder::activeNoteSubFolder();
        const QString noteSubFolderPath = noteSubFolder.fullPath();

        note = Note();
        note.setName(text);
        note.setNoteText(noteText);
        note.setNoteSubFolderId(noteSubFolder.getId());
        note.store();

        // workaround when signal block doesn't work correctly
        directoryWatcherWorkaround(true);

        // we even need a 2nd workaround because something triggers that the
        // note folder was modified
        noteDirectoryWatcher.removePath(notesPath);
        noteDirectoryWatcher.removePath(noteSubFolderPath);

        // store the note to disk
        // if a tag is selected add the tag to the just created note
        const Tag tag = Tag::activeTag();
        if (tag.isFetched()) {
            tag.linkToNote(note);
        }

        const bool noteWasStored = note.storeNoteTextFileToDisk();
        showStatusBarMessage(noteWasStored ? tr("Stored current note to disk")
                                           : tr("Current note could not be stored to disk"),
                             noteWasStored ? QStringLiteral("💾") : QStringLiteral("❌"), 3000);

        // Check if a name was set in a script
        // We need to do that in the end or the changed name will leak into the note filename
        const QString hookName = ScriptingService::instance()->callHandleNoteNameHook(&note);

        if (!hookName.isEmpty()) {
            note.setName(hookName);
            note.store();
        }

        {
            const QSignalBlocker blocker2(ui->noteTreeWidget);
            Q_UNUSED(blocker2)

            // adds the note to the note tree widget
            addNoteToNoteTreeWidget(note);
        }

        //        buildNotesIndex();
        if (!disableLoadNoteDirectoryList) {
            loadNoteDirectoryList();
        }

        // fetch note new (because all the IDs have changed after
        // the buildNotesIndex()
        //        note.refetch();

        // add the file to the note directory watcher
        noteDirectoryWatcher.addPath(note.fullNoteFilePath());

        // add the paths from the workaround
        noteDirectoryWatcher.addPath(notesPath);
        noteDirectoryWatcher.addPath(noteSubFolderPath);

        // turn on the method again
        directoryWatcherWorkaround(false);
    }

    // jump to the found or created note
    setCurrentNote(std::move(note));

    // hide the search widget after creating a new note
    activeNoteTextEdit()->hideSearchWidget(true);

    // focus the note text edit and set the cursor correctly
    focusNoteTextEdit();
}

void MainWindow::on_action_Remove_note_triggered() { removeCurrentNote(); }

void MainWindow::on_actionAbout_QOwnNotes_triggered() {
    auto *dialog = new AboutDialog(this);
    dialog->exec();
    delete (dialog);
}

/**
 * Triggered by the shortcut to create a new note with date in the headline
 */
void MainWindow::on_action_New_note_triggered() {
    SettingsService settings;
    const bool newNoteAskHeadline = settings.value(QStringLiteral("newNoteAskHeadline")).toBool();

    // check if we want to ask for a headline
    if (newNoteAskHeadline) {
        bool ok;
        QString headline = QInputDialog::getText(this, tr("New note"), tr("Note headline"),
                                                 QLineEdit::Normal, QString(), &ok);

        if (!ok) {
            return;
        }

        if (!headline.isEmpty()) {
            createNewNote(headline, false);
            return;
        }
    }

    // create a new note
    createNewNote();
}

/**
 * Creates a new note
 *
 * @param noteName
 */
void MainWindow::createNewNote(QString noteName, bool withNameAppend) {
    // turn on note editing if it was disabled
    if (!Utils::Misc::isNoteEditingAllowed()) {
        ui->actionAllow_note_editing->trigger();
    }

    // show the window in case we are using the system tray
    show();

    if (noteName.isEmpty()) {
        noteName = tr("Note", "name for new note");
    }

    if (withNameAppend) {
        QDateTime currentDate = QDateTime::currentDateTime();

        // Format the date and time like "2025-04-18 11h54s09"
        noteName = noteName + QStringLiteral(" ") +
                   currentDate.toString(QStringLiteral("yyyy-MM-dd HH'h'mm's'ss"));
    }

    const QSignalBlocker blocker(ui->searchLineEdit);
    Q_UNUSED(blocker)

    ui->searchLineEdit->setText(noteName);

    // create a new note or jump to the existing
    jumpToNoteOrCreateNew();
}

/*
 * Handles urls in the note preview
 */
void MainWindow::onNotePreviewAnchorClicked(const QUrl &url) {
    qDebug() << __func__ << " - 'url': " << url;

    if (UrlHandler::isUrlSchemeLocal(url)) {
        openLocalUrl(url.toString());
    } else {
        ui->noteTextEdit->openUrl(url.toString());
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
#else
    Q_UNUSED(reason);
#endif
}

/**
 * Shows the window (also brings it to the front and un-minimizes it)
 */
void MainWindow::showWindow() {
    // show the window in case we are using the system tray
    show();

    // bring application window to the front
    activateWindow();    // for Windows
    setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    raise();    // for MacOS

    // Really show the window, by bringing it to focus
    setFocus();

    // parse the current note for the navigation panel in case it wasn't parsed
    // while the mainwindow was hidden (https://github.com/pbek/QOwnNotes/issues/2110)
    startNavigationParser();
}

/**
 * Generates the system tray context menu
 */
void MainWindow::generateSystemTrayContextMenu() {
    // trying to destroy the old context menu as fix for Ubuntu 14.04
    // just clearing an existing menu resulted in empty sub-menus
    //    QMenu *menu = trayIcon->contextMenu();
    //    delete(menu);

    // QMenu(this) is not allowed here or it will not be recognized as child of
    // the tray icon later (see: https://github.com/pbek/QOwnNotes/issues/1239)
    auto *menu = new QMenu();
    menu->setTitle(QStringLiteral("QOwnNotes"));

    // add menu entry to open the app
    QString openActionText = tr("Open QOwnNotes");
#ifdef QT_DEBUG
    openActionText += QStringLiteral(" (Debug)");
#endif
    QAction *openAction = menu->addAction(openActionText);
    openAction->setIcon(getSystemTrayIcon());

    connect(openAction, &QAction::triggered, this, &MainWindow::showWindow);

    menu->addSeparator();
    menu->addAction(ui->actionSend_clipboard);
    menu->addAction(ui->actionSend_clipboard_as_text);
    menu->addSeparator();

    const QList<NoteFolder> noteFolders = NoteFolder::fetchAll();
    const int noteFoldersCount = noteFolders.count();

    if (noteFoldersCount > 1) {
        // didn't resulted in a visible text
        //        QWidgetAction* action = new QWidgetAction(menu);
        //        QLabel* label = new
        //        QLabel(NoteFolder::currentNoteFolder().getName(), menu);
        //        action->setDefaultWidget(label);
        //        menu->addAction(action);

        QMenu *noteFolderMenu = menu->addMenu(tr("Note folders"));

        // populate the note folder menu
        for (const auto &noteFolder : noteFolders) {
            // don't show not existing folders or if path is empty
            if (!noteFolder.localPathExists()) {
                continue;
            }

            // add a menu entry
            QAction *action = noteFolderMenu->addAction(noteFolder.getName());
            action->setToolTip(noteFolder.getLocalPath());
            action->setStatusTip(noteFolder.getLocalPath());

            if (noteFolder.isCurrent()) {
                QFont font = action->font();
                // setting it bold didn't do anything for me
                font.setBold(true);
                action->setFont(font);

                action->setIcon(Utils::Gui::folderIcon());
            }

            const int folderId = noteFolder.getId();
            connect(action, &QAction::triggered, this,
                    [this, folderId]() { changeNoteFolder(folderId); });
        }

        menu->addSeparator();
    }

    // add menu entry to create a new note
    QAction *createNoteAction = menu->addAction(tr("New note"));
    createNoteAction->setIcon(
        QIcon::fromTheme(QStringLiteral("document-new"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/document-new.svg"))));

    connect(createNoteAction, &QAction::triggered, this, &MainWindow::on_action_New_note_triggered);

    int maxNotes = Note::countAll();

    if (maxNotes > 0) {
        if (maxNotes > 9) {
            maxNotes = 9;
        }

        // add a menu for recent notes
        QMenu *noteMenu = menu->addMenu(tr("Recent notes"));

        const auto noteList = Note::fetchAll(maxNotes);

        for (const Note &note : noteList) {
            QAction *action = noteMenu->addAction(note.getName());
            action->setIcon(Utils::Gui::noteIcon());
            int noteId = note.getId();
            connect(action, &QAction::triggered, this,
                    [this, noteId]() { setCurrentNoteFromNoteId(noteId); });
        }
    }

    menu->addSeparator();

    // add menu entry to show the tasks
    QAction *taskAction = menu->addAction(tr("Show todo lists"));
    taskAction->setIcon(QIcon::fromTheme(
        QStringLiteral("view-calendar-tasks"),
        QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/view-calendar-tasks.svg"))));

    connect(taskAction, &QAction::triggered, this, [this]() { openTodoDialog(); });

    QList<CalendarItem> taskList = CalendarItem::fetchAllForSystemTray(10);
    if (taskList.count() > 0) {
        // add a menu for recent tasks
        QMenu *taskMenu = menu->addMenu(tr("Recent tasks"));

        // add menu entries to jump to tasks
        QListIterator<CalendarItem> itr(taskList);
        while (itr.hasNext()) {
            CalendarItem task = itr.next();

            QAction *action = taskMenu->addAction(task.getSummary());
            action->setIcon(QIcon::fromTheme(
                QStringLiteral("view-task"),
                QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/view-task.svg"))));

            connect(action, &QAction::triggered, this,
                    [this, task]() { openTodoDialog(task.getUid()); });
        }
    }

    menu->addSeparator();

    // add menu entry to quit the app
    QAction *quitAction = menu->addAction(tr("Quit"));
    quitAction->setIcon(QIcon::fromTheme(
        QStringLiteral("application-exit"),
        QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/application-exit.svg"))));
    connect(quitAction, &QAction::triggered, this, &MainWindow::on_action_Quit_triggered);

    trayIcon->setContextMenu(menu);
#ifdef QT_DEBUG
    trayIcon->setToolTip(QStringLiteral("QOwnNotes (Debug)"));
#else
    trayIcon->setToolTip(QStringLiteral("QOwnNotes"));
#endif
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
    auto doc = getDocumentForPreviewExport();
    exportNoteAsPDF(doc);
    doc->deleteLater();
}

/**
 * @brief Exports the current note as PDF (text)
 */
void MainWindow::on_action_Export_note_as_PDF_text_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    exportNoteAsPDF(textEdit);
}

QTextDocument *MainWindow::getDocumentForPreviewExport() {
    bool decrypt = ui->noteTextEdit->isHidden();
    QString html =
        currentNote.toMarkdownHtml(NoteFolder::currentLocalPath(), getMaxImageWidth(),
                                   Utils::Misc::useInternalExportStylingForPreview(), decrypt);
    html = Utils::Misc::parseTaskList(html, false);

    // Windows 10 has troubles with the QTextDocument from the QTextBrowser
    // see: https://github.com/pbek/QOwnNotes/issues/2015
    //    auto doc = ui->noteTextView->document()->clone();
    auto doc = new QTextDocument(this);
    doc->setHtml(html);

    return doc;
}

/**
 * @brief Prints the current note (Markdown)
 */
void MainWindow::on_action_Print_note_markdown_triggered() {
    auto doc = getDocumentForPreviewExport();
    printTextDocument(doc);
    doc->deleteLater();
}

/**
 * @brief Prints the current note (text)
 */
void MainWindow::on_action_Print_note_text_triggered() {
    printTextDocument(activeNoteTextEdit()->document());
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
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    textEdit->searchWidget()->activate();
}

/**
 * Asks the user for a password and encrypts the note text with it
 */
void MainWindow::on_action_Encrypt_note_triggered() {
    currentNote.refetch();

    // return if the note text is already encrypted
    if (currentNote.hasEncryptedNoteText()) {
        return;
    }

    // the password dialog can be disabled by scripts
    const bool dialogDisabled = qApp->property("encryptionPasswordDisabled").toBool();

    if (!dialogDisabled) {
        const QString labelText =
            tr("Please enter your <strong>password</strong> to encrypt the note."
               "<br />Keep in mind that you have to <strong>remember</strong> "
               "your password to read the content of the note<br /> and that you "
               "can <strong>only</strong> do that <strong>in QOwnNotes</strong>!");
        auto *dialog = new PasswordDialog(this, labelText, true);
        const int dialogResult = dialog->exec();

        // if the user didn't press ok return
        if (dialogResult != QDialog::Accepted) {
            return;
        }

        // take the password
        const QString password = dialog->password();

        // if password was empty return
        if (password.isEmpty()) {
            return;
        }

        // set the password
        currentNote.setCryptoPassword(password);
        currentNote.store();

        delete (dialog);
    }

    // encrypt the note
    const QString noteText = currentNote.encryptNoteText();
    ui->noteTextEdit->setPlainText(noteText);
    updateNoteTextEditReadOnly();
}

/**
 * Enables or disables the encrypt note buttons
 */
void MainWindow::updateNoteEncryptionUI() {
    currentNote.refetch();
    const bool hasEncryptedNoteText = currentNote.hasEncryptedNoteText();

    ui->action_Encrypt_note->setEnabled(!hasEncryptedNoteText);
    ui->actionEdit_encrypted_note->setEnabled(hasEncryptedNoteText);
    ui->actionDecrypt_note->setEnabled(hasEncryptedNoteText);

    // disable spell checker for encrypted text
    const bool checkSpellingEnabled =
        SettingsService().value(QStringLiteral("checkSpelling"), true).toBool();
    const bool spellCheckerShouldBeActive = !hasEncryptedNoteText && checkSpellingEnabled;

    // check if the spellchecking state is not as it should be
    if (spellCheckerShouldBeActive != ui->noteTextEdit->isSpellCheckingEnabled()) {
        ui->noteTextEdit->setSpellCheckingEnabled(spellCheckerShouldBeActive);
        ui->noteTextEdit->highlighter()->rehighlight();

        // for some reason the encryptedNoteTextEdit is also affected and needs
        // to be set again
        if (hasEncryptedNoteText) {
            ui->encryptedNoteTextEdit->setSpellCheckingEnabled(checkSpellingEnabled);
        }
    }
}

/**
 * Attempt to decrypt note text
 */
void MainWindow::on_actionDecrypt_note_triggered() {
    currentNote.refetch();
    if (!currentNote.hasEncryptedNoteText()) {
        return;
    }

    QMessageBox msgBox(QMessageBox::Warning, tr("Decrypt note and store it as plain text"),
                       tr("Your note will be decrypted and stored as plain text again. "
                          "Keep in mind that the unencrypted note will possibly be "
                          "synced to your server and sensitive text may be exposed!"
                          "<br />Do you want to decrypt your note?"),
                       QMessageBox::NoButton, this);
    msgBox.addButton(tr("&Decrypt"), QMessageBox::AcceptRole);
    QPushButton *cancelButton = msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);
    msgBox.setDefaultButton(cancelButton);
    msgBox.exec();

    if (msgBox.clickedButton() == cancelButton) {
        return;
    }

    askForEncryptedNotePasswordIfNeeded();

    if (currentNote.canDecryptNoteText()) {
        ui->encryptedNoteTextEdit->hide();
        ui->noteTextEdit->setText(currentNote.fetchDecryptedNoteText());
        ui->noteTextEdit->show();
        ui->noteTextEdit->setFocus();
        updateNoteTextEditReadOnly();
    }
}

/**
 * Lets the user edit an encrypted note text in a 2nd text edit
 */
void MainWindow::on_actionEdit_encrypted_note_triggered() { editEncryptedNote(); }

void MainWindow::editEncryptedNoteAsync() {
    QTimer::singleShot(0, this, &MainWindow::editEncryptedNote);
}

void MainWindow::editEncryptedNote() {
    currentNote.refetch();
    if (!currentNote.hasEncryptedNoteText()) {
        return;
    }

    askForEncryptedNotePasswordIfNeeded(tr("<br />You will be able to edit your encrypted note."));

    if (currentNote.canDecryptNoteText()) {
        const QSignalBlocker blocker(ui->encryptedNoteTextEdit);
        Q_UNUSED(blocker)

        ui->noteTextEdit->hide();
        const auto text = currentNote.fetchDecryptedNoteText();
        currentNote.setDecryptedText(text);
        // for some reason this still triggers a "textChanged", so we will do a
        // "currentNote.setDecryptedText" and check if the text really changed
        // in "currentNote.storeNewDecryptedText"
        ui->encryptedNoteTextEdit->setText(text);
        ui->encryptedNoteTextEdit->show();
        ui->encryptedNoteTextEdit->setFocus();
        _noteViewNeedsUpdate = true;
        updateNoteTextEditReadOnly();
    }
}

/**
 * Puts the encrypted text back to the note text edit
 */
void MainWindow::on_encryptedNoteTextEdit_textChanged() {
    // this also triggers when formatting is applied / syntax highlighting
    // changes!
    //    if
    //    (currentNote.storeNewDecryptedText(ui->encryptedNoteTextEdit->toPlainText()))
    //    {
    //        handleNoteTextChanged();
    //    }
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
    FileDialog dialog(QStringLiteral("NoteMarkdownExport"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Markdown files") + " (*.md)");
    dialog.setWindowTitle(tr("Export current note as Markdown file"));
    dialog.selectFile(currentNote.getName() + QStringLiteral(".md"));
    const int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(QStringLiteral(".md"));
            }

            bool withAttachedFiles =
                (currentNote.hasMediaFiles() || currentNote.hasAttachments()) &&
                Utils::Gui::question(
                    this, tr("Export attached files"),
                    tr("Do you also want to export media files and attachments of "
                       "the note? Files may be overwritten in the destination folder!"),
                    QStringLiteral("note-export-attachments")) == QMessageBox::Yes;

            currentNote.exportToPath(fileName, withAttachedFiles);
        }
    }
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

void MainWindow::on_actionToggle_distraction_free_mode_triggered() { toggleDistractionFreeMode(); }

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
    auto header = ui->tagTreeWidget->header();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents);
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
    SettingsService settings;
    settings.setValue(QStringLiteral("DistractionFreeMode/editorWidthMode"),
                      action->whatsThis().toInt());

    ui->noteTextEdit->setPaperMargins();
    ui->encryptedNoteTextEdit->setPaperMargins();
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
    return _workspaceNameUuidMap.value(workspaceName, "");
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
void MainWindow::reloadTagTree() {
    qDebug() << __func__;
    // take care that the tags are synced from the notes to the internal db
    handleScriptingNotesTagUpdating();

    SettingsService settings;

    // remove all broken note tag links
    if (!_brokenTagNoteLinksRemoved) {
        Tag::removeBrokenLinks();
        _brokenTagNoteLinksRemoved = true;
    }

    ui->tagTreeWidget->clear();

    QVector<int> noteSubFolderIds;

    auto noteSubFolderWidgetItems = ui->noteSubFolderTreeWidget->selectedItems();
    // if only one item is selected, then take current Item otherwise we will get
    // the item that was selected previously
    if (noteSubFolderWidgetItems.count() == 1) {
        noteSubFolderWidgetItems[0] = ui->noteSubFolderTreeWidget->currentItem();
    }

    noteSubFolderIds.reserve(noteSubFolderWidgetItems.count());
    // check if the notes should be viewed recursively
    if (NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively()) {
        for (QTreeWidgetItem *i : noteSubFolderWidgetItems) {
            const int id = i->data(0, Qt::UserRole).toInt();
            noteSubFolderIds << NoteSubFolder::fetchIdsRecursivelyByParentId(id);
        }
    } else {
        for (QTreeWidgetItem *i : noteSubFolderWidgetItems) {
            const int id = i->data(0, Qt::UserRole).toInt();
            noteSubFolderIds << id;
        }
    }

    qDebug() << __func__ << " - 'noteSubFolderIds': " << noteSubFolderIds;

    QVector<int> noteIdList;
    int untaggedNoteCount = 0;

    if (NoteFolder::isCurrentShowSubfolders()) {
        // get the notes from the subfolders
        for (int noteSubFolderId : Utils::asConst(noteSubFolderIds)) {
            // get all notes of a note sub folder
            untaggedNoteCount += Note::countAllNotTagged(noteSubFolderId);
            noteIdList << Note::fetchAllIdsByNoteSubFolderId(noteSubFolderId);
        }
    } else {
        untaggedNoteCount = Note::countAllNotTagged(0);
    }

    // create an item to view all notes
    int linkCount = _showNotesFromAllNoteSubFolders || !NoteFolder::isCurrentShowSubfolders()
                        ? Note::countAll()
                        : noteIdList.count();
    QString toolTip = tr("Show all notes (%1)").arg(QString::number(linkCount));

    auto *allItem = new QTreeWidgetItem();
    allItem->setText(0, tr("All notes"));
    allItem->setForeground(1, QColor(Qt::gray));
    allItem->setText(1, QString::number(linkCount));
    allItem->setToolTip(0, toolTip);
    allItem->setToolTip(1, toolTip);
    allItem->setData(0, Qt::UserRole, Tag::AllNotesId);
    allItem->setFlags(allItem->flags() & ~Qt::ItemIsSelectable);
    allItem->setIcon(
        0, QIcon::fromTheme(QStringLiteral("edit-copy"),
                            QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/edit-copy.svg"))));

    // this time, the tags come first
    buildTagTreeForParentItem();
    // and get sorted
    if (settings.value(QStringLiteral("tagsPanelSort")).toInt() == SORT_ALPHABETICAL) {
        ui->tagTreeWidget->sortItems(
            0, Utils::Gui::toQtOrder(settings.value(QStringLiteral("tagsPanelOrder")).toInt()));
    }
    // now add 'All notes' to the top
    ui->tagTreeWidget->insertTopLevelItem(0, allItem);

    // add an item to view untagged notes if there are any
    linkCount = _showNotesFromAllNoteSubFolders ? Note::countAllNotTagged() : untaggedNoteCount;

    if (linkCount > 0) {
        toolTip = tr("show all untagged notes (%1)").arg(QString::number(linkCount));
        auto *untaggedItem = new QTreeWidgetItem();
        untaggedItem->setText(0, tr("Untagged notes"));
        untaggedItem->setForeground(1, QColor(Qt::gray));
        untaggedItem->setText(1, QString::number(linkCount));
        untaggedItem->setToolTip(0, toolTip);
        untaggedItem->setToolTip(1, toolTip);
        untaggedItem->setData(0, Qt::UserRole, Tag::AllUntaggedNotesId);
        untaggedItem->setFlags(untaggedItem->flags() & ~Qt::ItemIsSelectable);
        untaggedItem->setIcon(
            0,
            QIcon::fromTheme(QStringLiteral("edit-copy"),
                             QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/edit-copy.svg"))));
        ui->tagTreeWidget->addTopLevelItem(untaggedItem);
    }

    // Decorate root if there are multiple levels to be able to collapse them,
    // because double-clicking will not collapse the first level, but edit
    // the clicked tag
    ui->tagTreeWidget->setRootIsDecorated(Tag::countAllParentId(0) != Tag::countAll());

    highlightCurrentNoteTagsInTagTree();
}

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
    const int parentId =
        (parent == nullptr || topLevel) ? 0 : parent->data(0, Qt::UserRole).toInt();
    const int activeTagId = Tag::activeTagId();
    SettingsService settings;
    const QStringList expandedList =
        settings
            .value(QStringLiteral("MainWindow/tagTreeWidgetExpandState-") +
                   QString::number(NoteFolder::currentNoteFolderId()))
            .toStringList();
    const int tagPanelSort = settings.value(QStringLiteral("tagsPanelSort")).toInt();
    const int tagPanelOrder = settings.value(QStringLiteral("tagsPanelOrder")).toInt();
    const QVector<TagHeader> tagList = Tag::fetchAllTagHeadersByParentId(parentId);
    for (const TagHeader &tag : tagList) {
        const int tagId = tag._id;
        QTreeWidgetItem *item = addTagToTagTreeWidget(parent, tag);

        // set the active item
        if (activeTagId == tagId) {
            const QSignalBlocker blocker(ui->tagTreeWidget);
            Q_UNUSED(blocker)

            ui->tagTreeWidget->setCurrentItem(item);
        }

        // recursively populate the next level
        buildTagTreeForParentItem(item);

        // set expanded state
        item->setExpanded(expandedList.contains(QString::number(tagId)));

        if (tagPanelSort == SORT_ALPHABETICAL) {
            item->sortChildren(0, Utils::Gui::toQtOrder(tagPanelOrder));
        }
    }

    // update the UI
    // this will crash the app sporadically
    // QCoreApplication::processEvents();
}

/**
 * Ads a tag to the tag tree widget
 */
QTreeWidgetItem *MainWindow::addTagToTagTreeWidget(QTreeWidgetItem *parent, const TagHeader &tag) {
    const int parentId = parent == nullptr ? 0 : parent->data(0, Qt::UserRole).toInt();
    const int tagId = tag._id;
    const QString name = tag._name;
    auto hideCount = SettingsService().value("tagsPanelHideNoteCount", false).toBool();

    int linkCount = 0;
    QVector<int> linkedNoteIds;
    bool isMultipleTags = false;

    if (!hideCount) {
        const QVector<int> tagIdListToCount = Tag::isTaggingShowNotesRecursively()
                                                  ? Tag::fetchTagIdsRecursivelyByParentId(tagId)
                                                  : QVector<int>{tag._id};
        isMultipleTags = tagIdListToCount.count() > 1;
        const auto selectedSubFolderItems = ui->noteSubFolderTreeWidget->selectedItems();
        const bool showNotesFromAllSubFolders = this->_showNotesFromAllNoteSubFolders;
        const bool isShowNotesRecursively =
            NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively();

        if (selectedSubFolderItems.count() > 1) {
            linkedNoteIds.reserve(tagIdListToCount.size());

            for (const int tagIdToCount : tagIdListToCount) {
                for (QTreeWidgetItem *folderItem : selectedSubFolderItems) {
                    int id = folderItem->data(0, Qt::UserRole).toInt();
                    const NoteSubFolder folder = NoteSubFolder::fetch(id);

                    if (!folder.isFetched()) {
                        continue;
                    }

                    if (!isMultipleTags) {
                        linkCount = Tag::countLinkedNoteFileNamesForNoteSubFolder(
                            tagIdToCount, folder, showNotesFromAllSubFolders,
                            isShowNotesRecursively);
                    } else {
                        linkedNoteIds << Tag::fetchAllLinkedNoteIdsForFolder(
                            tagIdToCount, folder, showNotesFromAllSubFolders,
                            isShowNotesRecursively);
                    }
                }
            }
        } else {
            for (const int tagToCount : tagIdListToCount) {
                if (!isMultipleTags) {
                    linkCount = Tag::countLinkedNoteFileNames(
                        tagToCount, showNotesFromAllSubFolders, isShowNotesRecursively);
                } else {
                    linkedNoteIds << Tag::fetchAllLinkedNoteIds(
                        tagToCount, showNotesFromAllSubFolders, isShowNotesRecursively);
                }
            }
        }
    }

    if (isMultipleTags) {
        // remove duplicate note ids
        QVector<int> uniqueLinkedNoteIds;
        for (const int &value : linkedNoteIds) {
            if (!uniqueLinkedNoteIds.contains(value)) {
                uniqueLinkedNoteIds.append(value);
            }
        }

        linkCount = uniqueLinkedNoteIds.count();
    }

    QString toolTip =
        tr("Show all notes tagged with '%1' (%2)").arg(name, QString::number(linkCount));

#ifdef QT_DEBUG
    toolTip += QStringLiteral("<br />id: %1").arg(tag._id);
#endif

    auto *item = new QTreeWidgetItem();
    item->setData(0, Qt::UserRole, tagId);
    item->setText(0, name);
    item->setText(1, linkCount > 0 ? QString::number(linkCount) : QString());
    item->setForeground(1, QColor(Qt::gray));
    item->setIcon(0, Utils::Gui::tagIcon());
    item->setToolTip(0, toolTip);
    item->setToolTip(1, toolTip);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    // set the color of the tag tree widget item
    Utils::Gui::handleTreeWidgetItemTagColor(item, tagId);

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
 * Creates a new tag
 */
void MainWindow::on_tagLineEdit_returnPressed() {
    const QString name = ui->tagLineEdit->text();
    if (name.isEmpty()) {
        return;
    }

    const QSignalBlocker blocker(this->noteDirectoryWatcher);
    Q_UNUSED(blocker)

    Tag tag;
    tag.setName(name);

    if (tag.store()) {
        const QSignalBlocker blocker2(ui->tagLineEdit);
        Q_UNUSED(blocker2)

        // clear the line edit if the tag was stored
        ui->tagLineEdit->clear();
    }

    reloadTagTree();
}

/**
 * Filters tags in the tag tree widget
 */
void MainWindow::on_tagLineEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInTreeWidget(ui->tagTreeWidget, arg1,
                                          Utils::Gui::TreeWidgetSearchFlag::IntCheck);
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
    reloadCurrentNoteTags();
    // filter the notes again
    filterNotes(false);
}

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
void MainWindow::on_newNoteTagButton_clicked() {
    _noteTagDockWidget->setVisible(true);
    ui->newNoteTagLineEdit->setVisible(true);
    ui->newNoteTagLineEdit->setFocus();
    ui->newNoteTagLineEdit->selectAll();
    ui->newNoteTagButton->setVisible(false);

    SettingsService settings;
    // enable the tagging dock widget the first time tagging was used
    if (!settings.value(QStringLiteral("tagWasAddedToNote")).toBool()) {
        _taggingDockWidget->setVisible(true);
        settings.setValue(QStringLiteral("tagWasAddedToNote"), true);
    }

    // add tag name auto-completion
    const QStringList wordList = Tag::fetchAllNames();
    auto *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->newNoteTagLineEdit->setCompleter(completer);
    completer->popup()->installEventFilter(this);
}

/**
 * Links a note to the tag entered after pressing return
 * in the note tag line edit
 */
void MainWindow::on_newNoteTagLineEdit_returnPressed() {
    const QString text = ui->newNoteTagLineEdit->text();
    linkTagNameToCurrentNote(text, true);
}

/**
 * Links a tag to the current note (or all selected notes)
 *
 * @param tagName
 */
void MainWindow::linkTagNameToCurrentNote(const QString &tagName, bool linkToSelectedNotes) {
    if (tagName.isEmpty()) {
        return;
    }

    // workaround when signal block doesn't work correctly
    directoryWatcherWorkaround(true, true);

    // create a new tag if it doesn't exist
    Tag tag = Tag::fetchByName(tagName);
    if (!tag.isFetched()) {
        const QSignalBlocker blocker(noteDirectoryWatcher);
        Q_UNUSED(blocker)

        tag.setName(tagName);
        tag.store();
    }

    // link the current note to the tag
    if (tag.isFetched()) {
        const QSignalBlocker blocker(noteDirectoryWatcher);
        Q_UNUSED(blocker)

        const int selectedNotesCount = getSelectedNotesCount();

        if (linkToSelectedNotes && selectedNotesCount > 1) {
            const auto noteList = selectedNotes();
            for (const Note &note : noteList) {
                if (tag.isLinkedToNote(note)) {
                    continue;
                }

                tag.linkToNote(note);

                // add the tag to the note text if defined via scripting
                // engine
                handleScriptingNoteTagging(note, tag, false, false);
            }
        } else {
            tag.linkToNote(currentNote);

            // add the tag to the note text if defined via scripting engine
            handleScriptingNoteTagging(currentNote, tag, false, false);
        }

        reloadCurrentNoteTags();
        reloadTagTree();
        filterNotes();

        // handle the coloring of the note in the note tree widget
        handleNoteTreeTagColoringForNote(currentNote);
    }

    // turn off the workaround again
    directoryWatcherWorkaround(false, true);
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
    const QString oldNoteText = note.getNoteText();
    const QString &action = doRemove ? QStringLiteral("remove") : QStringLiteral("add");
    QString noteText =
        ScriptingService::instance()->callNoteTaggingHook(note, action, tag.getName()).toString();

    // try noteTaggingByObjectHook if noteTaggingHook didn't do anything
    if (noteText.isEmpty()) {
        noteText =
            ScriptingService::instance()->callNoteTaggingByObjectHook(note, action, tag).toString();

        if (noteText.isEmpty() || (oldNoteText == noteText)) {
            return;
        }
    }

    // return if note could not be stored
    if (!note.storeNewText(std::move(noteText))) {
        return;
    }

    // do some stuff to get the UI updated
    if (triggerPostMethods) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        storeUpdatedNotesToDisk();
        reloadTagTree();
        //        reloadCurrentNoteTags();
    }

    if (note.isSameFile(currentNote)) {
        //            updateNoteTextFromDisk(note);

        currentNote.refetch();
        setNoteTextFromNote(&currentNote);
    }
}

/**
 * Takes care that the tags are synced from the notes to the internal db
 */
void MainWindow::handleScriptingNotesTagUpdating() {
    if (!ScriptingService::instance()->noteTaggingHookExists()) {
        return;
    }

    qDebug() << __func__;

    // workaround when signal blocking doesn't work correctly
    directoryWatcherWorkaround(true, true);

    const QVector<Note> &notes = Note::fetchAll();
    for (const Note &note : notes) {
        QSet<int> tagIdList;
        const QStringList tagNameList = ScriptingService::instance()
                                            ->callNoteTaggingHook(note, QStringLiteral("list"))
                                            .toStringList();

        if (tagNameList.count() == 0) {
            // if callNoteTaggingHook didn't return anything lets try
            // callNoteTaggingByObjectHook
            const auto variantTagIdList =
                ScriptingService::instance()
                    ->callNoteTaggingByObjectHook(note, QStringLiteral("list"))
                    .toList();

            // get a tagId list from the variant list
            for (const QVariant &tagId : variantTagIdList) {
                tagIdList << tagId.toInt();
            }
        } else {
            // get a tagId list from the tag name list
            for (const QString &tagName : tagNameList) {
                Tag tag = Tag::fetchByName(tagName);

                // add missing tags to the tag database
                if (!tag.isFetched()) {
                    tag.setName(tagName);
                    tag.store();
                }

                tagIdList << tag.getId();
            }
        }

        QSet<int> tagIdList2 = Tag::fetchAllIdsByNote(note);

        // we need to create a copy of tagIdList, because subtract would modify tagIdList
        QSet<int> subtraction = tagIdList;
        subtraction.subtract(tagIdList2);

        // add missing tag links to the note
        for (const int tagId : subtraction) {
            Tag tag = Tag::fetch(tagId);
            tag.linkToNote(note);
            qDebug() << " difference1: " << tag;
        }

        const QSet<int> subtraction1 = tagIdList2.subtract(tagIdList);

        // remove tags from the note that are not in the note text
        for (const int tagId : subtraction1) {
            Tag tag = Tag::fetch(tagId);
            tag.removeLinkToNote(note);
            qDebug() << " difference2: " << tag;
        }
    }

    // disable workaround
    directoryWatcherWorkaround(false, true);
}

/**
 * Takes care that a tag is renamed in all notes
 *
 * @param oldTagName
 * @param newTagName
 */
void MainWindow::handleScriptingNotesTagRenaming(const Tag &tag, const QString &newTagName) {
    if (!ScriptingService::instance()->noteTaggingHookExists()) {
        return;
    }

    qDebug() << __func__;

    // workaround when signal blocking doesn't work correctly
    directoryWatcherWorkaround(true, true);

    const QSignalBlocker blocker(this->noteDirectoryWatcher);
    Q_UNUSED(blocker)

    const auto notes = Note::fetchAll();
    for (Note note : notes) {
        const QString oldNoteText = note.getNoteText();
        QString noteText =
            ScriptingService::instance()
                ->callNoteTaggingHook(note, QStringLiteral("rename"), tag.getName(), newTagName)
                .toString();

        // if nothing came back from callNoteTaggingHook let's try
        // callNoteTaggingByObjectHook
        if (noteText.isEmpty()) {
            noteText =
                ScriptingService::instance()
                    ->callNoteTaggingByObjectHook(note, QStringLiteral("rename"), tag, newTagName)
                    .toString();

            if (noteText.isEmpty() || (oldNoteText == noteText)) {
                continue;
            }
        }

        note.storeNewText(std::move(noteText));
    }

    storeUpdatedNotesToDisk();

    // disable workaround
    directoryWatcherWorkaround(false, true);

    reloadTagTree();

    // re-fetch current note to make sure the note text with the tag was updated
    currentNote.refetch();
    setNoteTextFromNote(&currentNote);
}

/**
 * Takes care that a tag is removed from all notes
 *
 * @param tagName
 */
void MainWindow::handleScriptingNotesTagRemoving(const Tag &tag, bool forBulkOperation) {
    if (!ScriptingService::instance()->noteTaggingHookExists()) {
        return;
    }

    qDebug() << __func__;

    if (!forBulkOperation) {
        // workaround when signal blocking doesn't work correctly
        directoryWatcherWorkaround(true, true);
    }

    const QVector<Note> &notes = Note::fetchAll();
    for (const Note &note : notes) {
        handleScriptingNoteTagging(note, tag, true, false);
    }

    if (!forBulkOperation) {
        storeUpdatedNotesToDisk();

        // disable workaround
        directoryWatcherWorkaround(false, true);

        reloadTagTree();
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
 * Reloads the note tag buttons for the current note (or the selected notes)
 */
void MainWindow::reloadCurrentNoteTags() {
    // remove all remove-tag buttons
    QLayoutItem *child;
    while ((child = ui->noteTagButtonFrame->layout()->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    int selectedNotesCount = getSelectedNotesCount();
    bool currentNoteOnly = selectedNotesCount <= 1;
    ui->selectedTagsToolButton->setVisible(!currentNoteOnly);
    ui->newNoteTagButton->setToolTip(currentNoteOnly ? tr("Add a tag to the current note")
                                                     : tr("Add a tag to the selected notes"));
    QVector<Tag> tagList;

    ui->multiSelectActionFrame->setVisible(!currentNoteOnly);
    ui->noteEditorFrame->setVisible(currentNoteOnly);

    if (currentNoteOnly) {
        tagList = Tag::fetchAllOfNote(currentNote);

        // only refresh the preview if we previously selected multiple notes
        // because we used it for showing note information
        if (_lastNoteSelectionWasMultiple) {
            _notePreviewHash.clear();
            regenerateNotePreview();
        }
    } else {
        const QVector<Note> notes = selectedNotes();
        tagList = Tag::fetchAllOfNotes(notes);

        // Count notes and folders separately
        const auto selectedItems = ui->noteTreeWidget->selectedItems();
        int noteCount = 0;
        int folderCount = 0;
        for (const auto *item : selectedItems) {
            const int itemType = item->data(0, Qt::UserRole + 1).toInt();
            if (itemType == NoteType) {
                noteCount++;
            } else if (itemType == FolderType) {
                folderCount++;
            }
        }

        // Build selection text based on what's selected
        QString notesSelectedText;
        if (noteCount > 0 && folderCount > 0) {
            notesSelectedText =
                tr("%n note(s) and %1 folder(s) selected", "", noteCount).arg(folderCount);
        } else if (folderCount > 0) {
            notesSelectedText = tr("%n folder(s) selected", "", folderCount);
        } else {
            notesSelectedText = tr("%n notes selected", "", noteCount);
        }

        ui->selectedTagsToolButton->setText(QString::number(selectedNotesCount));
        ui->selectedTagsToolButton->setToolTip(notesSelectedText);

        ui->notesSelectedLabel->setText(notesSelectedText);

        // overwrite the note preview with a preview of the selected notes
        const QString previewHtml = Note::generateMultipleNotesPreviewText(notes);
#ifdef USE_QLITEHTML
        _notePreviewWidget->setHtml(previewHtml);
#else
        ui->noteTextView->setText(previewHtml);
#endif
    }

    _lastNoteSelectionWasMultiple = !currentNoteOnly;

    // add all new remove-tag buttons
    for (const Tag &tag : Utils::asConst(tagList)) {
        QPushButton *button =
            new QPushButton(Utils::Misc::shorten(tag.getName(), 25), ui->noteTagButtonFrame);
        button->setIcon(QIcon::fromTheme(
            QStringLiteral("tag-delete"),
            QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/xml-attribute-delete.svg"))));
        button->setToolTip(currentNoteOnly
                               ? tr("Remove tag '%1' from the current note").arg(tag.getName())
                               : tr("Remove tag '%1' from the selected notes").arg(tag.getName()));
        button->setObjectName(QStringLiteral("removeNoteTag") + QString::number(tag.getId()));

        QObject::connect(button, &QPushButton::clicked, this, &MainWindow::removeNoteTagClicked);

        ui->noteTagButtonFrame->layout()->addWidget(button);
    }

    //    // find tags not in common of selected notes
    //    if (selectedNotesCount > 1) {
    //        QLabel *noteTagButtonFrame = new QLabel("+3 tags");
    //        ui->noteTagButtonFrame->layout()->addWidget(noteTagButtonFrame);
    //    }

    // add a spacer to prevent the button items to take the full width
    auto *spacer = new QSpacerItem(0, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
    ui->noteTagButtonFrame->layout()->addItem(spacer);

    highlightCurrentNoteTagsInTagTree();
}

/**
 * Highlights the tags of the current note in the tag tree
 */
void MainWindow::highlightCurrentNoteTagsInTagTree() {
    const int selectedNotesCount = getSelectedNotesCount();
    const bool currentNoteOnly = selectedNotesCount <= 1;
    QVector<Tag> tagList;

    if (currentNoteOnly) {
        tagList = Tag::fetchAllOfNote(currentNote);
    } else {
        const QVector<Note> &notes = selectedNotes();
        tagList = Tag::fetchAllOfNotes(notes);
    }

    const QSignalBlocker blocker1(ui->tagTreeWidget);
    Q_UNUSED(blocker1)

    Utils::Gui::resetBoldStateOfAllTreeWidgetItems(ui->tagTreeWidget);

    for (const Tag &tag : Utils::asConst(tagList)) {
        QTreeWidgetItem *item =
            Utils::Gui::getTreeWidgetItemWithUserData(ui->tagTreeWidget, tag.getId());

        if (item != nullptr) {
            // set tag item in tag tree widget to bold if note has tag
            auto font = item->font(0);
            if (!font.bold()) {
                font.setBold(true);
                item->setFont(0, font);
            }
        }
    }
}

/**
 * Removes a note tag link
 */
void MainWindow::removeNoteTagClicked() {
    QString objectName = sender()->objectName();
    if (objectName.startsWith(QLatin1String("removeNoteTag"))) {
        const int tagId = objectName.remove(QLatin1String("removeNoteTag")).toInt();
        const Tag tag = Tag::fetch(tagId);
        if (!tag.isFetched()) {
            return;
        }

        // workaround when signal blocking doesn't work correctly
        directoryWatcherWorkaround(true, true);

        const int selectedNotesCount = getSelectedNotesCount();

        if (selectedNotesCount <= 1) {
            tag.removeLinkToNote(currentNote);

            // remove the tag from the note text if defined via scripting engine
            handleScriptingNoteTagging(currentNote, tag, true);
        } else {
            const auto selectedNotesList = selectedNotes();
            for (const Note &note : selectedNotesList) {
                if (!tag.isLinkedToNote(note)) {
                    continue;
                }

                tag.removeLinkToNote(note);

                // remove the tag from the note text if defined via
                // scripting engine
                handleScriptingNoteTagging(note, tag, true);
            }
        }

        reloadCurrentNoteTags();
        reloadTagTree();
        filterNotesByTag();

        // handle the coloring of the note in the note tree widget
        handleNoteTreeTagColoringForNote(currentNote);

        // disable workaround
        directoryWatcherWorkaround(false, true);
    }
}

int MainWindow::getSelectedNotesCount() const {
    return ui->noteTreeWidget->selectedItems().count();
}

/**
 * Allows the user to add a tag to the current note
 */
void MainWindow::on_action_new_tag_triggered() { on_newNoteTagButton_clicked(); }

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
    Q_UNUSED(column)

    Tag tag = Tag::fetch(item->data(0, Qt::UserRole).toInt());
    if (tag.isFetched()) {
        const QString oldName = tag.getName();
        const QString name = item->text(0);

        // workaround when signal block doesn't work correctly
        directoryWatcherWorkaround(true, true);

        if (!name.isEmpty()) {
            const QSignalBlocker blocker(this->noteDirectoryWatcher);
            Q_UNUSED(blocker)

            // take care that a tag is renamed in all notes
            handleScriptingNotesTagRenaming(tag, name);

            tag.setName(name);
            tag.store();
        }

        // we also have to reload the tag tree if we don't change the tag
        // name to get the old name back
        reloadTagTree();
        reloadCurrentNoteTags();

        // turn off the workaround again
        directoryWatcherWorkaround(false, true);
    }
}

/**
 * Sets a new active tag
 */
void MainWindow::on_tagTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                     QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    if (current == nullptr) {
        return;
    }

    // set the tag id as active
    const int tagId = current->data(0, Qt::UserRole).toInt();
    Tag::setAsActive(tagId);

    const int count = ui->tagTreeWidget->selectedItems().count();
    if (count > 1) return;

    const QSignalBlocker blocker(ui->searchLineEdit);
    Q_UNUSED(blocker)

    ui->searchLineEdit->clear();
    filterNotes();
}

/**
 * Jumps to a tag in the tag tree
 *
 * @param tagId
 * @return
 */
bool MainWindow::jumpToTag(int tagId) {
    QTreeWidgetItem *item = Utils::Gui::getTreeWidgetItemWithUserData(ui->tagTreeWidget, tagId);

    if (item != nullptr) {
        // If the selection isn't cleared then the old subfolder is still selected too
        ui->tagTreeWidget->clearSelection();
        ui->tagTreeWidget->setCurrentItem(item);

        return true;
    }

    return false;
}

/**
 * Triggers filtering when multiple tags are selected
 */
void MainWindow::on_tagTreeWidget_itemSelectionChanged() {
    const int count = ui->tagTreeWidget->selectedItems().count();

    if (count <= 1) {
        if (count == 1) {
            //           on_tagTreeWidget_currentItemChanged(ui->tagTreeWidget->selectedItems().first(),
            //                                                nullptr);
        }
        return;
    }

    const QSignalBlocker blocker(ui->searchLineEdit);
    Q_UNUSED(blocker)

    ui->searchLineEdit->clear();
    filterNotes();
}

/**
 * Creates a context menu for the tag tree widget
 */
void MainWindow::on_tagTreeWidget_customContextMenuRequested(const QPoint pos) {
    // don't open the most of the context menu if no tags are selected
    const bool hasSelected = ui->tagTreeWidget->selectedItems().count() > 0;

    const QPoint globalPos = ui->tagTreeWidget->mapToGlobal(pos);
    QMenu menu;

    QAction *addAction = menu.addAction(tr("&Add tag"));

    // allow these actions only if tags are selected
    QAction *renameAction = nullptr;
    QAction *assignColorAction = nullptr;
    QAction *disableColorAction = nullptr;
    QAction *removeAction = nullptr;
    if (hasSelected) {
        renameAction = menu.addAction(tr("Rename tag"));
        assignColorAction = menu.addAction(tr("Assign color"));
        disableColorAction = menu.addAction(tr("Disable color"));
        removeAction = menu.addAction(tr("&Remove tags"));

        // build the tag moving menu
        QMenu *moveMenu = menu.addMenu(tr("&Move tags to…"));
        buildTagMoveMenuTree(moveMenu);
    }

    QAction *selectedItem = menu.exec(globalPos);

    if (selectedItem == nullptr) {
        return;
    }

    QTreeWidgetItem *item = ui->tagTreeWidget->currentItem();

    if (selectedItem == addAction) {
        // open the "add new tag" dialog
        auto *dialog = new TagAddDialog(this);
        const int dialogResult = dialog->exec();

        // if user pressed ok take the name
        if (dialogResult == QDialog::Accepted) {
            const QString name = dialog->name();
            if (!name.isEmpty()) {
                const int parentId = item->data(0, Qt::UserRole).toInt() < 0
                                         ? 0
                                         : item->data(0, Qt::UserRole).toInt();

                // create a new tag with the name
                Tag tag;
                tag.setParentId(parentId);
                tag.setName(name);
                tag.store();

                if (tag.isFetched()) {
                    reloadTagTree();
                } else {
                    showStatusBarMessage(tr("Tag could not be created!"), QStringLiteral("🏷️"),
                                         3000);
                }
            }
        }

        delete (dialog);
        return;
    }

    if (selectedItem == assignColorAction) {
        // assign and store a color to all selected tags in the tag tree widget
        assignColorToSelectedTagItems();
        return;
    } else if (selectedItem == disableColorAction) {
        // disable the color of all selected tags
        const auto selectedTagItems = ui->tagTreeWidget->selectedItems();
        for (QTreeWidgetItem *tagItem : selectedTagItems) {
            // disable the color of the tag
            disableColorOfTagItem(tagItem);
        }

        // reload the notes in the note tree widget to update the colors
        loadNoteDirectoryList();
        return;
    }

    // don't allow clicking on non-tag items for removing, editing and colors
    if (item->data(0, Qt::UserRole).toInt() <= 0) {
        return;
    }

    if (selectedItem == removeAction) {
        // remove selected tag
        removeSelectedTags();
    } else if (selectedItem == renameAction) {
        ui->tagTreeWidget->editItem(item);
    }
}

/**
 * Assigns and stores a color to a tag from the tag tree widget
 *
 * @param item
 */
void MainWindow::assignColorToTagItem(QTreeWidgetItem *item) {
    const int tagId = item->data(0, Qt::UserRole).toInt();

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
        Utils::Gui::handleTreeWidgetItemTagColor(item, tag);

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
    const auto selectedItems = ui->tagTreeWidget->selectedItems();
    for (QTreeWidgetItem *item : selectedItems) {
        const int tagId = item->data(0, Qt::UserRole).toInt();
        if (tagId > 0) {
            const Tag tag = Tag::fetch(tagId);

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
        const auto selectedItems = ui->tagTreeWidget->selectedItems();
        for (QTreeWidgetItem *item : selectedItems) {
            const int tagId = item->data(0, Qt::UserRole).toInt();
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
            Utils::Gui::handleTreeWidgetItemTagColor(item, tag);
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
    const int tagId = item->data(0, Qt::UserRole).toInt();
    Tag tag = Tag::fetch(tagId);

    if (!tag.isFetched()) {
        return;
    }

    tag.setColor(QColor());
    tag.store();

    // set the color of the tag tree widget item
    Utils::Gui::handleTreeWidgetItemTagColor(item, tag);
}

/**
 * Populates a tag menu tree for moving tags
 */
void MainWindow::buildTagMoveMenuTree(QMenu *parentMenu, int parentTagId) {
    const auto tagList = Tag::fetchAllByParentId(parentTagId, QStringLiteral("t.name ASC"));

    for (const Tag &tag : tagList) {
        const int tagId = tag.getId();
        const QString name = tag.getName();

        const int count = Tag::countAllParentId(tagId);
        if (count > 0) {
            // if there are sub-tag build a new menu level
            auto *tagMenu = parentMenu->addMenu(name);
            buildTagMoveMenuTree(tagMenu, tagId);
        } else {
            // if there are no sub-tags just create a named action
            auto *action = parentMenu->addAction(name);

            connect(action, &QAction::triggered, this,
                    [this, tagId]() { moveSelectedTagsToTagId(tagId); });
        }
    }

    // add an action to move to this tag
    parentMenu->addSeparator();
    QAction *action = parentMenu->addAction(
        parentTagId == 0
            ? tr("Move to the root", "to move a tag to the current tag in the tag context menu")
            : tr("Move to this tag"));
    action->setData(parentTagId);

    connect(action, &QAction::triggered, this,
            [this, parentTagId]() { moveSelectedTagsToTagId(parentTagId); });
}

/**
 * Populates a tag menu tree for bulk note tagging
 */
void MainWindow::buildBulkNoteTagMenuTree(QMenu *parentMenu, int parentTagId) {
    const auto tagList = Tag::fetchAllByParentId(parentTagId, QStringLiteral("t.name ASC"));

    for (const Tag &tag : tagList) {
        const int tagId = tag.getId();
        const QString name = tag.getName();

        const int count = Tag::countAllParentId(tagId);
        if (count > 0) {
            // if there are sub-tag build a new menu level
            QMenu *tagMenu = parentMenu->addMenu(name);
            buildBulkNoteTagMenuTree(tagMenu, tagId);
        } else {
            // if there are no sub-tags just create a named action
            QAction *action = parentMenu->addAction(name);

            connect(action, &QAction::triggered, this,
                    [this, tagId]() { tagSelectedNotesToTagId(tagId); });
        }
    }

    if (parentTagId > 0) {
        // add an action to tag this
        parentMenu->addSeparator();
        QAction *action = parentMenu->addAction(tr("Tag this"));
        action->setData(parentTagId);

        connect(action, &QAction::triggered, this,
                [this, parentTagId]() { tagSelectedNotesToTagId(parentTagId); });
    }
}

/**
 * Moves selected tags to tagId
 */
void MainWindow::moveSelectedTagsToTagId(int tagId) {
    qDebug() << __func__ << " - 'tagId': " << tagId;
    QVector<Tag> tagList;

    // gather tags to move (since we can't be sure the tag tree will not get
    // reloaded when we are actually moving the first tag)
    const auto selectedItems = ui->tagTreeWidget->selectedItems();
    for (QTreeWidgetItem *item : selectedItems) {
        const int id = item->data(0, Qt::UserRole).toInt();
        Tag tag = Tag::fetch(id);
        if (tag.isFetched()) {
            if (tag.hasChild(tagId) || (id == tagId)) {
                showStatusBarMessage(tr("Cannot move tag '%1' to this tag").arg(tag.getName()),
                                     QStringLiteral("🏷️"), 3000);
            } else {
                tagList << tag;
            }
        }
    }

    if (tagList.count() > 0) {
        const bool useScriptingEngine = ScriptingService::instance()->noteTaggingHookExists();

        // workaround when signal block doesn't work correctly
        directoryWatcherWorkaround(true, true);

        // move tags
        for (Tag tag : Utils::asConst(tagList)) {
            if (useScriptingEngine) {
                const QVector<Tag> tagsToHandle = Tag::fetchRecursivelyByParentId(tag.getId());

                // check all tags we need to handle
                for (const Tag &tagToHandle : tagsToHandle) {
                    // remove tag from all notes
                    for (const Note &note : tagToHandle.fetchAllLinkedNotes()) {
                        handleScriptingNoteTagging(note, tagToHandle, true, false);
                    }
                }
            }

            tag.setParentId(tagId);
            tag.store();

            if (useScriptingEngine) {
                const QVector<Tag> tagsToHandle = Tag::fetchRecursivelyByParentId(tag.getId());

                // check all tags we need to handle
                for (const Tag &tagToHandle : tagsToHandle) {
                    // add tag to all notes
                    for (const Note &note : tagToHandle.fetchAllLinkedNotes()) {
                        handleScriptingNoteTagging(note, tagToHandle, false, false);
                    }
                }
            }

            showStatusBarMessage(tr("Moved tag '%1' to new tag").arg(tag.getName()),
                                 QStringLiteral("🏷️"), 3000);
        }

        reloadCurrentNoteTags();
        reloadTagTree();

        // turn off the workaround again
        directoryWatcherWorkaround(false, true);
    }
}

/**
 * Tag selected notes to tagId
 */
void MainWindow::tagSelectedNotesToTagId(int tagId) {
    qDebug() << __func__ << " - 'tagId': " << tagId;
    const Tag tag = Tag::fetch(tagId);

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
    const QVector<NoteSubFolder> noteSubFolderList =
        NoteSubFolder::fetchAllByParentId(parentNoteSubFolderId, QStringLiteral("name ASC"));

    for (const auto &noteSubFolder : noteSubFolderList) {
        const int noteSubFolderId = noteSubFolder.getId();
        const QString name = noteSubFolder.getName();

        const int count = NoteSubFolder::countAllParentId(noteSubFolderId);
        if (count > 0) {
            // if there are sub-noteSubFolder build a new menu level
            QMenu *noteSubFolderMenu = parentMenu->addMenu(name);
            buildBulkNoteSubFolderMenuTree(noteSubFolderMenu, doCopy, noteSubFolderId);
        } else {
            // if there are no sub-noteSubFolders just create a named action
            QAction *action = parentMenu->addAction(name);

            connect(action, &QAction::triggered, this, [this, doCopy, noteSubFolderId]() {
                doCopy ? copySelectedNotesToNoteSubFolderId(noteSubFolderId)
                       : moveSelectedNotesToNoteSubFolderId(noteSubFolderId);
            });
        }
    }

    // add an action to copy or move to this subfolder
    parentMenu->addSeparator();
    const QString text =
        (parentNoteSubFolderId == 0)
            ? (doCopy ? tr("Copy to note folder") : tr("Move to note folder"))
            : (doCopy ? tr("Copy to this subfolder") : tr("Move to this subfolder"));
    QAction *action = parentMenu->addAction(text);
    action->setData(parentNoteSubFolderId);

    connect(action, &QAction::triggered, this, [this, doCopy, parentNoteSubFolderId]() {
        doCopy ? copySelectedNotesToNoteSubFolderId(parentNoteSubFolderId)
               : moveSelectedNotesToNoteSubFolderId(parentNoteSubFolderId);
    });
}

/**
 * Populates a subfolder menu tree for bulk note moving or copying to
 * subfolders of other note folders
 */
void MainWindow::buildBulkNoteFolderSubFolderMenuTree(QMenu *parentMenu, bool doCopy,
                                                      const QString &parentNoteSubFolderPath,
                                                      bool isRoot) {
    QDir dir(parentNoteSubFolderPath);
    QStringList nameFilters{};

    if (isRoot) {
        nameFilters << QStringList({"media", "trash", "attachments"});
    }

    // show newest entry first
    QStringList directoryNames = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

    if (isRoot) {
        const auto names = QStringList({"media", "trash", "attachments"});
        for (const QString &name : names) {
            directoryNames.removeAll(name);
        }
    }

    for (const QString &directoryName : Utils::asConst(directoryNames)) {
        const QString fullPath = parentNoteSubFolderPath + QLatin1Char('/') + directoryName;
        QDir subDir(fullPath);
        const QStringList subDirectoryNames = subDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        if (subDirectoryNames.count() > 0) {
            // if there are sub folders build a new menu level
            QMenu *noteSubFolderMenu = parentMenu->addMenu(directoryName);
            buildBulkNoteFolderSubFolderMenuTree(noteSubFolderMenu, doCopy, fullPath, false);
        } else {
            // if there are no sub folders just create a named action
            QAction *action = parentMenu->addAction(directoryName);
            action->setToolTip(fullPath);
            action->setStatusTip(fullPath);

            connect(action, &QAction::triggered, this, [this, doCopy, fullPath]() {
                doCopy ? copySelectedNotesToFolder(fullPath) : moveSelectedNotesToFolder(fullPath);
            });
        }
    }

    // add an action to copy or move to this subfolder
    parentMenu->addSeparator();
    const QString text =
        (isRoot) ? (doCopy ? tr("Copy to note folder") : tr("Move to note folder"))
                 : (doCopy ? tr("Copy to this subfolder") : tr("Move to this subfolder"));
    auto *action = parentMenu->addAction(text);
    action->setToolTip(parentNoteSubFolderPath);
    action->setStatusTip(parentNoteSubFolderPath);

    connect(action, &QAction::triggered, this, [this, doCopy, parentNoteSubFolderPath]() {
        doCopy ? copySelectedNotesToFolder(parentNoteSubFolderPath)
               : moveSelectedNotesToFolder(parentNoteSubFolderPath);
    });
}

/**
 * Moves selected notes to a note subfolder id
 */
void MainWindow::moveSelectedNotesToNoteSubFolderId(int noteSubFolderId) {
    qDebug() << __func__ << " - 'noteSubFolderId': " << noteSubFolderId;
    const NoteSubFolder noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);

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
    const NoteSubFolder noteSubFolder = NoteSubFolder::fetch(noteSubFolderId);

    // copy selected notes to note subfolder
    if (noteSubFolder.isFetched() || (noteSubFolderId == 0)) {
        copySelectedNotesToNoteSubFolder(noteSubFolder);
    }
}

/**
 * Moves selected notes to a note subfolder
 */
void MainWindow::moveSelectedNotesToNoteSubFolder(const NoteSubFolder &noteSubFolder) {
    const int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();
    const QString text = tr("Move %n selected note(s) to note subfolder "
                            "<strong>%2</strong>?",
                            "", selectedItemsCount)
                             .arg(noteSubFolder.getName());

    if (Utils::Gui::question(this, tr("Move selected notes"), text, QStringLiteral("move-notes")) ==
        QMessageBox::Yes) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        // unset the current note
        //      unsetCurrentNote();

        int noteSubFolderCount = 0;

        // disable the externally removed check, because it might trigger
        _noteExternallyRemovedCheckEnabled = false;

        const auto selectedItems = ui->noteTreeWidget->selectedItems();
        bool forceReload = false;
        for (QTreeWidgetItem *item : selectedItems) {
            if (item->data(0, Qt::UserRole + 1) != NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            Note note = Note::fetch(noteId);
            Note oldNote = note;

            if (!note.isFetched()) {
                continue;
            }

            // don't move note if source and destination paths are the same
            if (noteSubFolder.fullPath() == note.fullNoteFileDirPath()) {
                qWarning() << "Note was not moved because source and "
                              "destination paths were the same:"
                           << note.getName();

                continue;
            }

            // fetch the tags to tag the note after moving it
            const QVector<Tag> tags = Tag::fetchAllOfNote(note);

            if (note.getId() == currentNote.getId()) {
                // unset the current note
                unsetCurrentNote();
            }

            // move note
            const bool result = note.moveToPath(noteSubFolder.fullPath());
            if (result) {
                noteSubFolderCount++;
                qDebug() << "Note was moved:" << note.getName();

                // set the new subfolder so the tags are stored correctly
                note.setNoteSubFolder(noteSubFolder);

                // tag the note again
                for (const Tag &tag : tags) {
                    tag.linkToNote(note);
                }

                // handle the replacing of all note links from other notes
                // because the note was moved
                if (note.handleNoteMoving(oldNote)) {
                    // reload the current note if we had to change it
                    reloadCurrentNoteByNoteId(true);
                    forceReload = true;
                }

                // re-link images
                const bool mediaFileLinksUpdated = note.updateRelativeMediaFileLinks();

                // re-link attachments
                const bool attachmentFileLinksUpdated = note.updateRelativeAttachmentFileLinks();

                if (mediaFileLinksUpdated || attachmentFileLinksUpdated) {
                    note.storeNoteTextFileToDisk();
                }
            } else {
                qWarning() << "Could not move note:" << note.getName();
            }
        }

        // rebuild the index after the move
        if (noteSubFolderCount > 0) {
            // for some reason this only works with a small delay, otherwise
            // not all changes will be recognized
            QTimer::singleShot(150, this, [this, forceReload] {
                // If the outgoing links to other notes were changed, we have to really reload the
                // note folder
                if (forceReload) {
                    buildNotesIndexAndLoadNoteDirectoryList(true, true);
                } else {
                    buildNotesIndexAndLoadNoteDirectoryList();
                }
            });
        }

        showStatusBarMessage(
            tr("%n note(s) were moved to note subfolder \"%2\"", "", noteSubFolderCount)
                .arg(noteSubFolder.getName()),
            QStringLiteral("📁"), 5000);

        // wait some time to enable the check again to prevent troubles on macOS
        QTimer::singleShot(4000, this, SLOT(enableNoteExternallyRemovedCheck()));
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
    const int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();
    const QString text = tr("Copy %n selected note(s) to note subfolder "
                            "<strong>%2</strong>?",
                            "", selectedItemsCount)
                             .arg(noteSubFolder.getName());

    if (Utils::Gui::question(this, tr("Copy selected notes"), text, QStringLiteral("copy-notes")) ==
        QMessageBox::Yes) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        int noteSubFolderCount = 0;
        const auto items = ui->noteTreeWidget->selectedItems();
        for (QTreeWidgetItem *item : items) {
            if (item->data(0, Qt::UserRole + 1) != NoteType) {
                continue;
            }

            const int noteId = item->data(0, Qt::UserRole).toInt();
            Note note = Note::fetch(noteId);

            if (!note.isFetched()) {
                continue;
            }

            // don't copy note if source and destination paths are the same
            if (noteSubFolder.fullPath() == note.fullNoteFileDirPath()) {
                qWarning() << "Note was not copied because source and "
                              "destination paths were the same:"
                           << note.getName();

                continue;
            }

            // fetch the tags to tag the note after copying it
            const QVector<Tag> tags = Tag::fetchAllOfNote(note);

            // copy note
            const bool result = note.copyToPath(noteSubFolder.fullPath());
            if (result) {
                noteSubFolderCount++;
                qDebug() << "Note was copied:" << note.getName();

                // set the new subfolder so the tags are stored correctly
                note.setNoteSubFolder(noteSubFolder);

                // tag the note again
                for (const Tag &tag : tags) {
                    tag.linkToNote(note);
                }

                // re-link images
                const bool mediaFileLinksUpdated = note.updateRelativeMediaFileLinks();

                // re-link attachments
                const bool attachmentFileLinksUpdated = note.updateRelativeAttachmentFileLinks();

                if (mediaFileLinksUpdated || attachmentFileLinksUpdated) {
                    note.storeNoteTextFileToDisk();
                }
            } else {
                qWarning() << "Could not copy note:" << note.getName();
            }
        }

        // rebuild the index after the copy
        if (noteSubFolderCount > 0) {
            // for some reason this only works with a small delay, otherwise
            // not all changes will be recognized
            QTimer::singleShot(150, this, SLOT(buildNotesIndexAndLoadNoteDirectoryList()));
        }

        showStatusBarMessage(
            tr("%n note(s) were copied to note subfolder \"%2\"", "", noteSubFolderCount)
                .arg(noteSubFolder.getName()),
            QStringLiteral("📁"), 5000);
    }
}

/**
 * Opens the widget to replace text in the current note
 */
void MainWindow::on_actionReplace_in_current_note_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    textEdit->searchWidget()->activateReplace();
}

/**
 * Jumps to the position that was clicked in the navigation widget
 */
void MainWindow::onNavigationWidgetPositionClicked(int position) {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();

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
    c.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);

    textEdit->setTextCursor(c);

    // update the preview-slider
    noteTextSliderValueChanged(textEdit->verticalScrollBar()->value(), true);

    // set focus back to the navigation widget, so you can use the
    // keyboard to navigate
    ui->navigationWidget->setFocus();
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

void MainWindow::reloadOpenAiControls() {
    OpenAiService::deleteInstance();
    generateAiBackendComboBox();
    generateAiModelComboBox();
    generateAiModelMainMenu();

    aiModelMainMenuSetCurrentItem();
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
    FileDialog dialog(QStringLiteral("NoteHTMLExport"));
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("HTML files") + " (*.html)");
    dialog.setWindowTitle(tr("Export current note as HTML file"));
    dialog.selectFile(currentNote.getName() + QStringLiteral(".html"));
    const int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QString fileName = dialog.selectedFile();

        if (!fileName.isEmpty()) {
            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(QStringLiteral(".html"));
            }

            QFile file(fileName);

            qDebug() << "exporting html file: " << fileName;

            if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qCritical() << file.errorString();
                return;
            }
            QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            out.setCodec("UTF-8");
#endif
            out << currentNote.toMarkdownHtml(NoteFolder::currentLocalPath(), getMaxImageWidth(),
                                              true, true, true);
            file.flush();
            file.close();
            Utils::Misc::openFolderSelect(fileName);
        }
    }
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
void MainWindow::storeSavedSearch() {
    SettingsService settings;

    if (settings.value(QStringLiteral("disableSavedSearchesAutoCompletion")).toBool()) {
        return;
    }

    const QString text = ui->searchLineEdit->text();
    // Only store searches with less than 30 characters to prevent clogging the settings
    if (!text.isEmpty() && text.length() < 30) {
        int noteFolderId = NoteFolder::currentNoteFolderId();
        QString settingsKey =
            QStringLiteral("savedSearches/noteFolder-") + QString::number(noteFolderId);
        QStringList savedSearches = settings.value(settingsKey).toStringList();

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
    const int noteFolderId = NoteFolder::currentNoteFolderId();
    QStringList savedSearches;
    SettingsService settings;

    if (!settings.value(QStringLiteral("disableSavedSearchesAutoCompletion")).toBool()) {
        QString settingsKey =
            QStringLiteral("savedSearches/noteFolder-") + QString::number(noteFolderId);
        savedSearches = settings.value(settingsKey).toStringList();
    }

    // release the old completer
    auto *completer = ui->searchLineEdit->completer();
    delete completer;

    // add the completer
    completer = new QCompleter(savedSearches, ui->searchLineEdit);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->searchLineEdit->setCompleter(completer);

    // install event filter for the popup
    completer->popup()->installEventFilter(this);
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
    // in case all notes were removed
    if (current == nullptr) {
        return;
    }

    // handle changing of the current item for subfolders
    if (current->data(0, Qt::UserRole + 1).toInt() == FolderType) {
        ui->noteSubFolderTreeWidget->currentItemChanged(current, previous);
        return;
    }
}

void MainWindow::openSelectedNotesInTab() {
    const auto selectedItems = ui->noteTreeWidget->selectedItems();
    for (QTreeWidgetItem *item : selectedItems) {
        if (item->data(0, Qt::UserRole + 1) != NoteType) {
            continue;
        }

        const int noteId = item->data(0, Qt::UserRole).toInt();
        Note note = Note::fetch(noteId);

        if (!note.isFetched()) {
            continue;
        }

        //        setCurrentNote(note);
        //        openCurrentNoteInTab();

        openNoteInTab(note);
    }
}

void MainWindow::openNoteInTab(const Note &note) {
    // simulate a newly opened tab by updating the current tab with the last note
    if (_lastNoteId > 0) {
        auto previousNote = Note::fetch(_lastNoteId);

        // open the previous note in a new tab only if it is not already open in a tab
        if (previousNote.isFetched() && getNoteTabIndex(_lastNoteId) == -1) {
            updateCurrentTabData(previousNote);
        }
    }

    const QString &noteName = note.getName();
    const int noteId = note.getId();
    int tabIndex = getNoteTabIndex(noteId);

    if (tabIndex == -1) {
        auto *widgetPage = new QWidget();
        widgetPage->setLayout(ui->noteEditTabWidgetLayout);
        tabIndex = ui->noteEditTabWidget->addTab(widgetPage, noteName);
    }

    Utils::Gui::updateTabWidgetTabData(ui->noteEditTabWidget, tabIndex, note);

    ui->noteEditTabWidget->setCurrentIndex(tabIndex);

    // remove the tab initially created by the ui file
    if (ui->noteEditTabWidget->widget(0)->property("note-id").isNull()) {
        ui->noteEditTabWidget->removeTab(0);
    }
}

void MainWindow::openCurrentNoteInTab() {
    // simulate a newly opened tab by updating the current tab with the last note
    if (_lastNoteId > 0) {
        auto previousNote = Note::fetch(_lastNoteId);

        // open the previous note in a new tab only if it is not already open in a tab
        if (previousNote.isFetched() && getNoteTabIndex(_lastNoteId) == -1) {
            updateCurrentTabData(previousNote);
        }
    }

    const QString &noteName = currentNote.getName();
    const int noteId = currentNote.getId();
    int tabIndex = getNoteTabIndex(noteId);

    if (tabIndex == -1) {
        auto *widgetPage = new QWidget();
        widgetPage->setLayout(ui->noteEditTabWidgetLayout);
        tabIndex = ui->noteEditTabWidget->addTab(widgetPage, noteName);
    }

    Utils::Gui::updateTabWidgetTabData(ui->noteEditTabWidget, tabIndex, currentNote);

    ui->noteEditTabWidget->setCurrentIndex(tabIndex);

    // remove the tab initially created by the ui file
    if (ui->noteEditTabWidget->widget(0)->property("note-id").isNull()) {
        ui->noteEditTabWidget->removeTab(0);
    }
}

int MainWindow::getNoteTabIndex(int noteId) const {
    return Utils::Gui::getTabWidgetIndexByProperty(ui->noteEditTabWidget, QStringLiteral("note-id"),
                                                   noteId);
}

void MainWindow::on_noteTreeWidget_customContextMenuRequested(const QPoint pos) {
    auto *item = ui->noteTreeWidget->itemAt(pos);

    // if the user clicks at empty space, this is null and if it isn't handled
    // QON crashes
    if (item == nullptr) {
        return;
    }

    const QPoint globalPos = ui->noteTreeWidget->mapToGlobal(pos);
    const auto selectedItems = ui->noteTreeWidget->selectedItems();

    // Check if we have a mixed selection of notes and folders
    bool hasNotes = false;
    bool hasFolders = false;
    for (const auto *selectedItem : selectedItems) {
        const int itemType = selectedItem->data(0, Qt::UserRole + 1).toInt();
        if (itemType == NoteType) {
            hasNotes = true;
        } else if (itemType == FolderType) {
            hasFolders = true;
        }
        if (hasNotes && hasFolders) {
            break;
        }
    }

    // If we have a mixed selection or notes, show the notes context menu
    // (which now supports both notes and folders)
    if (hasNotes || (hasNotes && hasFolders)) {
        openNotesContextMenu(globalPos, hasNotes, hasFolders);
    } else if (hasFolders) {
        // Only folders selected, show folder-specific context menu
        std::unique_ptr<QMenu> menu(NoteSubFolderTree::contextMenu(ui->noteTreeWidget));
        menu->exec(globalPos);
    }
}

void MainWindow::openNotesContextMenu(const QPoint globalPos, bool hasNotes, bool hasFolders) {
    QMenu noteMenu;
    QAction *renameAction = nullptr;

    // Calculate counts for proper labels
    const auto selectedItems = ui->noteTreeWidget->selectedItems();
    int noteCount = 0;
    int folderCount = 0;
    for (const auto *item : selectedItems) {
        const int itemType = item->data(0, Qt::UserRole + 1).toInt();
        if (itemType == NoteType) {
            noteCount++;
        } else if (itemType == FolderType) {
            folderCount++;
        }
    }

    bool multiNoteMenuEntriesOnly = (noteCount + folderCount) > 1;

    if (!multiNoteMenuEntriesOnly) {
        auto *createNoteAction = noteMenu.addAction(tr("New note"));
        connect(createNoteAction, &QAction::triggered, this,
                &MainWindow::on_action_New_note_triggered);

        renameAction = noteMenu.addAction(tr("Rename note"));
        renameAction->setToolTip(
            tr("Allows you to rename the filename of "
               "the note"));
    }

    // Set dynamic remove action text based on selection
    QString removeActionText;
    if (hasNotes && hasFolders) {
        removeActionText = tr("&Remove notes and folders");
    } else if (hasFolders) {
        removeActionText = tr("&Remove folders");
    } else {
        removeActionText = tr("&Remove notes");
    }
    auto *removeAction = noteMenu.addAction(removeActionText);
    noteMenu.addSeparator();

    const QList<NoteFolder> noteFolders = NoteFolder::fetchAll();

    // show copy and move menu entries only if there
    // is at least one other note folder
    QMenu *moveDestinationMenu = nullptr;
    QMenu *copyDestinationMenu = nullptr;
    if (noteFolders.count() > 1) {
        moveDestinationMenu = noteMenu.addMenu(tr("&Move notes to…"));
        copyDestinationMenu = noteMenu.addMenu(tr("&Copy notes to…"));

        for (const NoteFolder &noteFolder : noteFolders) {
            // don't show not existing folders or if path is empty
            if (!noteFolder.localPathExists() || noteFolder.isCurrent()) {
                continue;
            }

            if (noteFolder.isShowSubfolders()) {
                auto *subFolderMoveMenu = moveDestinationMenu->addMenu(noteFolder.getName());
                buildBulkNoteFolderSubFolderMenuTree(subFolderMoveMenu, false,
                                                     noteFolder.getLocalPath());

                auto *subFolderCopyMenu = copyDestinationMenu->addMenu(noteFolder.getName());
                buildBulkNoteFolderSubFolderMenuTree(subFolderCopyMenu, true,
                                                     noteFolder.getLocalPath());
            } else {
                auto *moveAction = moveDestinationMenu->addAction(noteFolder.getName());
                moveAction->setData(noteFolder.getLocalPath());
                moveAction->setToolTip(noteFolder.getLocalPath());
                moveAction->setStatusTip(noteFolder.getLocalPath());

                auto *copyAction = copyDestinationMenu->addAction(noteFolder.getName());
                copyAction->setData(noteFolder.getLocalPath());
                copyAction->setToolTip(noteFolder.getLocalPath());
                copyAction->setStatusTip(noteFolder.getLocalPath());
            }
        }
    }

    QAction *moveToThisSubFolderAction = nullptr;
    const bool showSubFolders = NoteFolder::isCurrentShowSubfolders();
    const bool isEnableNoteTree = Utils::Misc::isEnableNoteTree();
    if (showSubFolders || isEnableNoteTree) {
        if (ui->noteTreeWidget->selectedItems().count() == 1 && !isEnableNoteTree) {
            moveToThisSubFolderAction = noteMenu.addAction(tr("Jump to the note's subfolder"));
        }

        auto *subFolderMoveMenu = noteMenu.addMenu(tr("Move notes to subfolder…"));
        buildBulkNoteSubFolderMenuTree(subFolderMoveMenu, false);

        auto *subFolderCopyMenu = noteMenu.addMenu(tr("Copy notes to subfolder…"));
        buildBulkNoteSubFolderMenuTree(subFolderCopyMenu, true);
    }

    int tagCount = Tag::countAll();

    // show the tagging menu if at least one tag is present
    if (tagCount) {
        auto *tagMenu = noteMenu.addMenu(tr("&Tag selected notes with…"));
        buildBulkNoteTagMenuTree(tagMenu);
    }

    QStringList noteNameList;
    for (QTreeWidgetItem *item : selectedItems) {
        // the note names are not unique anymore but the note subfolder
        // path will be taken into account in
        // Tag::fetchAllWithLinkToNoteNames
        const QString name = item->text(0);
        const Note note = Note::fetchByName(name);
        if (note.isFetched()) {
            noteNameList << note.getName();
        }
    }

    const QVector<Tag> tagRemoveList = Tag::fetchAllWithLinkToNoteNames(noteNameList);

    // show the remove tags menu if at least one tag is present
    QMenu *tagRemoveMenu = nullptr;
    if (tagRemoveList.count() > 0) {
        tagRemoveMenu = noteMenu.addMenu(tr("&Remove tag from selected notes…"));

        for (const Tag &tag : tagRemoveList) {
            auto *action = tagRemoveMenu->addAction(tag.getName());
            action->setData(tag.getId());
            action->setToolTip(tag.getName());
            action->setStatusTip(tag.getName());
        }
    }

    QAction *openInExternalEditorAction = nullptr;
    QAction *openNoteWindowAction = nullptr;
    QAction *openNoteInNextcloudFilesAction = nullptr;
    QAction *openNoteInNextcloudNotesAction = nullptr;
    QAction *showInFileManagerAction = nullptr;
    QAction *showNoteGitLogAction = nullptr;
    QAction *copyNotePathToClipboardAction = nullptr;
    QAction *toggleFavoriteAction = nullptr;

    if (!multiNoteMenuEntriesOnly) {
        noteMenu.addSeparator();
    }

    if ((multiNoteMenuEntriesOnly && selectedItems.count() > 1) || !multiNoteMenuEntriesOnly) {
        auto *openNoteInTabAction = noteMenu.addAction(tr("Open selected notes in tabs"));
        connect(openNoteInTabAction, &QAction::triggered, this,
                &MainWindow::openSelectedNotesInTab);
    }

    if (!multiNoteMenuEntriesOnly) {
        openInExternalEditorAction = noteMenu.addAction(tr("Open note in external editor"));
        openNoteWindowAction = noteMenu.addAction(tr("Open note in different window"));
        if (OwnCloudService::isOwnCloudSupportEnabled()) {
            openNoteInNextcloudFilesAction = noteMenu.addAction(tr("Open note in Nextcloud Files"));
            openNoteInNextcloudNotesAction = noteMenu.addAction(tr("Open note in Nextcloud Notes"));
        };
        showInFileManagerAction = noteMenu.addAction(tr("Show note in file manager"));
        copyNotePathToClipboardAction = noteMenu.addAction(tr("Copy absolute path of note"));

        showNoteGitLogAction = new QAction(this);
        if (Utils::Git::isCurrentNoteFolderUseGit() && Utils::Git::hasLogCommand()) {
            showNoteGitLogAction = noteMenu.addAction(tr("Show note git versions"));
        }

        // Add favorite toggle action for single note
        noteMenu.addSeparator();
        const Note note = getCurrentNote();
        if (note.isFetched()) {
            if (note.isFavorite()) {
                toggleFavoriteAction = noteMenu.addAction(tr("Unmark as favorite"));
            } else {
                toggleFavoriteAction = noteMenu.addAction(tr("Mark as favorite"));
            }
        }
    }

    // add the custom actions to the context menu
    if (!_noteListContextMenuActions.isEmpty()) {
        noteMenu.addSeparator();

        for (QAction *action : Utils::asConst(_noteListContextMenuActions)) {
            noteMenu.addAction(action);
        }
    }

    QAction *selectAllAction = nullptr;
    if (!multiNoteMenuEntriesOnly) {
        noteMenu.addSeparator();
        selectAllAction = noteMenu.addAction(tr("Select &all notes"));
    }

    QAction *selectedItem = noteMenu.exec(globalPos);
    if (selectedItem) {
        if (selectedItem->parent() == moveDestinationMenu) {
            // move notes
            const QString destinationFolder = selectedItem->data().toString();
            moveSelectedNotesToFolder(destinationFolder);
        } else if (selectedItem->parent() == copyDestinationMenu) {
            // copy notes
            const QString destinationFolder = selectedItem->data().toString();
            copySelectedNotesToFolder(destinationFolder);
        } else if (selectedItem->parent() == tagRemoveMenu) {
            // remove tag from notes
            const Tag tag = Tag::fetch(selectedItem->data().toInt());

            if (tag.isFetched()) {
                removeTagFromSelectedNotes(tag);
            }
        } else if (selectedItem == removeAction) {
            // remove notes
            removeSelectedNotes();
        } else if (selectedItem == moveToThisSubFolderAction) {
            const int subFolderId = getCurrentNote().getNoteSubFolderId();
            if (NoteSubFolder::activeNoteSubFolderId() != subFolderId) {
                jumpToNoteSubFolder(subFolderId);
            }
        } else if (selectedItem == selectAllAction) {
            // select all notes
            selectAllNotes();
        } else if (selectedItem == openInExternalEditorAction) {
            // open the current note in an external editor
            on_action_Open_note_in_external_editor_triggered();
        } else if (selectedItem == openNoteWindowAction) {
            // open the current note in a dialog
            on_actionView_note_in_new_window_triggered();
        } else if (selectedItem == openNoteInNextcloudFilesAction) {
            auto fileUrl = currentNote.getNextcloudFileLink();
            qDebug() << __func__ << "fileUrl: " << fileUrl;
            QDesktopServices::openUrl(fileUrl);
        } else if (selectedItem == openNoteInNextcloudNotesAction) {
            auto url = currentNote.getNextcloudNotesLink();
            qDebug() << __func__ << "url: " << url;
            QDesktopServices::openUrl(url);
        } else if (selectedItem == showInFileManagerAction) {
            // show the current note in the file manager
            on_actionShow_note_in_file_manager_triggered();
        } else if (selectedItem == copyNotePathToClipboardAction) {
            on_actionCopy_path_to_note_to_clipboard_triggered();
        } else if (selectedItem == showNoteGitLogAction) {
            // show the git log of the current note
            on_actionShow_note_git_versions_triggered();
        } else if (selectedItem == toggleFavoriteAction) {
            // toggle favorite status of the current note
            currentNote.toggleFavorite();
            // Reload the note list to update the icon and sorting
            loadNoteDirectoryList();
        } else if (selectedItem == renameAction) {
            QTreeWidgetItem *item = ui->noteTreeWidget->currentItem();

            if (Note::allowDifferentFileName()) {
                if (Utils::Misc::isNoteListPreview()) {
                    bool ok{};
                    const QString name =
                        QInputDialog::getText(this, tr("Rename note"), tr("Name:"),
                                              QLineEdit::Normal, currentNote.getName(), &ok);

                    if (ok && !name.isEmpty()) {
                        item->setText(0, name);
                        on_noteTreeWidget_itemChanged(item, 0);
                    }
                } else {
                    ui->noteTreeWidget->editItem(item);
                }
            } else {
                QMessageBox msgBox(QMessageBox::Warning, tr("Note renaming not enabled!"),
                                   tr("If you want to rename your note you have to enable "
                                      "the option to allow the note filename to be "
                                      "different from the headline."),
                                   QMessageBox::NoButton, this);
                QPushButton *settingsButton =
                    msgBox.addButton(tr("Open &settings"), QMessageBox::AcceptRole);
                msgBox.addButton(tr("&Cancel"), QMessageBox::RejectRole);
                msgBox.setDefaultButton(settingsButton);
                msgBox.exec();

                if (msgBox.clickedButton() == settingsButton) {
                    openSettingsDialog(SettingsDialog::NoteFolderPage);
                }
            }
        }
    }
}

/**
 * Renames a note file if the note was renamed in the note tree widget
 */
void MainWindow::on_noteTreeWidget_itemChanged(QTreeWidgetItem *item, int /*column*/) {
    if (item == nullptr) {
        return;
    }

    // handle note subfolder renaming in a note tree
    if (item->data(0, Qt::UserRole + 1) == FolderType) {
        ui->noteSubFolderTreeWidget->renameSubFolder(item);
        return;
    }

    if (!Note::allowDifferentFileName()) {
        return;
    }

    const int noteId = item->data(0, Qt::UserRole).toInt();
    Note note = Note::fetch(noteId);
    if (note.isFetched()) {
        qDebug() << __func__ << " - 'note': " << note;

        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        const Note oldNote = note;
        const QString oldNoteName = note.getName();

        if (note.renameNoteFile(item->text(0))) {
            QString newNoteName = note.getName();

            if (oldNoteName != newNoteName) {
                note.refetch();
                setCurrentNote(note);

                // rename the note file names of note tag links
                Tag::renameNoteFileNamesOfLinks(oldNoteName, newNoteName, note.getNoteSubFolder());

                // handle the replacing of all note urls if a note was renamed
                if (note.handleNoteMoving(oldNote)) {
                    // reload the current note if we had to change it
                    reloadCurrentNoteByNoteId(true);
                }

                // reload the directory list if note name has changed
                //                loadNoteDirectoryList();

                // sort notes if note name has changed
                SettingsService settings;
                if (settings.value(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE).toInt() ==
                    SORT_ALPHABETICAL) {
                    ui->noteTreeWidget->sortItems(
                        0, Utils::Gui::toQtOrder(
                               settings.value(QStringLiteral("notesPanelOrder")).toInt()));
                    ui->noteTreeWidget->scrollToItem(item);
                }

                // update the note list tooltip of the note
                Utils::Gui::setTreeWidgetItemToolTipForNote(item, note);
            }
        }

        const QSignalBlocker blocker2(ui->noteTreeWidget);
        Q_UNUSED(blocker2)

        // set old name back in case the renaming failed or the file name got
        // altered in the renaming process
        item->setText(0, note.getName());

        if (Utils::Misc::isNoteListPreview()) {
            updateNoteTreeWidgetItem(note, item);
        }
    }
}

void MainWindow::onCurrentSubFolderChanged() {
    const QSignalBlocker blocker(ui->searchLineEdit);
    Q_UNUSED(blocker)

    ui->searchLineEdit->clear();

    if (!NoteFolder::isCurrentNoteTreeEnabled()) {
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

void MainWindow::clearTagFilteringColumn() {
    QTreeWidgetItemIterator it(ui->noteTreeWidget);
    while (*it) {
        // if the item wasn't filtered by the searchLineEdit
        if ((*it)->data(4, Qt::UserRole).toBool()) {
            (*it)->setData(4, Qt::UserRole, false);
        }
        // reset the value for searchLineEdit
        ++it;
    }
}

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
    if (Utils::Gui::question(this, tr("Split note"),
                             tr("Split note at current cursor position? "
                                "The text after the cursor will be moved to a new note. "
                                "The new note will be linked to the old note."),
                             QStringLiteral("split-note")) != QMessageBox::Yes) {
        return;
    }

    QString name = currentNote.getName();
    const QVector<Tag> tags = Tag::fetchAllOfNote(currentNote);

    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    // select the text to get into a new note
    c.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    const QString selectedText = c.selectedText();

    // remove the selected text
    c.removeSelectedText();
    textEdit->setTextCursor(c);

    Note previousNote = currentNote;

    // create a new note
    createNewNote(std::move(name));

    // adding a link to new note into the old note
    previousNote.refetch();
    const QString noteLink = previousNote.getNoteUrlForLinkingTo(currentNote);
    QString previousNoteText = previousNote.getNoteText();
    previousNoteText.reserve(3 + noteLink.size() + 1);
    previousNoteText += QStringLiteral("\n\n<") + noteLink + QStringLiteral(">");
    previousNote.storeNewText(std::move(previousNoteText));

    // add the previously removed text
    textEdit = activeNoteTextEdit();
    textEdit->insertPlainText(selectedText);

    // link the tags of the old note to the new note
    for (const Tag &tag : tags) {
        tag.linkToNote(currentNote);
    }
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
    // send an event to jump to "All notes" in the note subfolder tree widget
    selectAllNotesInNoteSubFolderTreeWidget();

    // send an event to jump to "All notes" in the tag tree widget
    selectAllNotesInTagTreeWidget();

    // trigger a "Find note"
    on_action_Find_note_triggered();
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
    const QSignalBlocker blocker(ui->actionUnlock_panels);
    {
        Q_UNUSED(blocker)
        ui->actionUnlock_panels->setChecked(arg1);
    }

    const QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();

    if (!arg1) {
        // remove the title bar widgets of all dock widgets
        for (QDockWidget *dockWidget : dockWidgets) {
            // we don't want to lock floating dock widgets
            if (dockWidget->isFloating()) {
                continue;
            }

            handleDockWidgetLocking(dockWidget);
        }
    } else {
        // add the old title bar widgets to all dock widgets
        _noteSubFolderDockWidget->setTitleBarWidget(_noteSubFolderDockTitleBarWidget);
        _taggingDockWidget->setTitleBarWidget(_taggingDockTitleBarWidget);
        _noteSearchDockWidget->setTitleBarWidget(_noteSearchDockTitleBarWidget);
        _noteFolderDockWidget->setTitleBarWidget(_noteFolderDockTitleBarWidget);
        _noteListDockWidget->setTitleBarWidget(_noteListDockTitleBarWidget);
        _noteNavigationDockWidget->setTitleBarWidget(_noteNavigationDockTitleBarWidget);

        if (!_noteEditIsCentralWidget) {
            _noteEditDockWidget->setTitleBarWidget(_noteEditDockTitleBarWidget);
        }

        _noteTagDockWidget->setTitleBarWidget(_noteTagDockTitleBarWidget);
        _notePreviewDockWidget->setTitleBarWidget(_notePreviewDockTitleBarWidget);
        _logDockWidget->setTitleBarWidget(_logDockTitleBarWidget);
        _scriptingDockWidget->setTitleBarWidget(_scriptingDockTitleBarWidget);
        _noteGraphicsViewDockWidget->setTitleBarWidget(_noteGraphicsViewDockTitleBarWidget);

        for (QDockWidget *dockWidget : dockWidgets) {
            // reset the top margin of the enclosed widget
            dockWidget->widget()->setContentsMargins(0, 0, 0, 0);
        }
    }
}

void MainWindow::handleDockWidgetLocking(QDockWidget *dockWidget) {
    // Remove the title bar widget
    dockWidget->setTitleBarWidget(new QWidget());

#ifndef Q_OS_MAC
    // Set 3px top margin for the enclosed widget
    dockWidget->widget()->setContentsMargins(0, 3, 0, 0);
#endif
}

/**
 * Creates a new workspace with asking for its name
 */
void MainWindow::on_actionStore_as_new_workspace_triggered() {
    const QString name =
        QInputDialog::getText(this, tr("Create new workspace"), tr("Workspace name:")).trimmed();

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

    SettingsService settings;
    const QString currentUuid = currentWorkspaceUuid();
    settings.setValue(QStringLiteral("previousWorkspace"), currentUuid);

    const QString uuid = Utils::Misc::createUuidString();
    QStringList workspaces = getWorkspaceUuidList();
    workspaces.append(uuid);

    settings.setValue(QStringLiteral("workspaces"), workspaces);
    settings.setValue(QStringLiteral("currentWorkspace"), uuid);
    settings.setValue(QStringLiteral("workspace-") + uuid + QStringLiteral("/name"), name);

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
    SettingsService settings;
    return settings.value(QStringLiteral("currentWorkspace")).toString();
}

/**
 * Sets the new current workspace when the workspace combo box index has changed
 */
void MainWindow::onWorkspaceComboBoxCurrentIndexChanged(int index) {
    Q_UNUSED(index)

    const QString uuid = _workspaceComboBox->currentData().toString();

    // set the new workspace
    setCurrentWorkspace(uuid);
}

/**
 * Sets the AI backend when the AI backend combo box index has changed
 */
void MainWindow::onAiBackendComboBoxCurrentIndexChanged(int index) {
    Q_UNUSED(index)

    const QString backendId = _aiBackendComboBox->currentData().toString();

    if (OpenAiService::instance()->setBackendId(backendId)) {
        generateAiModelComboBox();
        aiModelMainMenuSetCurrentItem();
    }
}

/**
 * Puts items into the AI backend combo box
 */
void MainWindow::generateAiBackendComboBox() {
    _aiBackendComboBox->blockSignals(true);
    _aiBackendComboBox->clear();
    auto backendNames = OpenAiService::instance()->getBackendNames();

    for (const auto &key : backendNames.keys()) {
        const QString &name = backendNames.value(key);
        _aiBackendComboBox->addItem(name, key);
    }

    Utils::Gui::setComboBoxIndexByUserData(_aiBackendComboBox,
                                           OpenAiService::instance()->getBackendId());
    _aiBackendComboBox->blockSignals(false);
}

/**
 * Puts items into the AI model main menu
 */
void MainWindow::generateAiModelMainMenu() {
    QMap<QString, QString> backendNames = OpenAiService::instance()->getBackendNames();
    ui->menuAI_model->clear();

    for (const auto &backendId : backendNames.keys()) {
        const QString &backendName = backendNames.value(backendId);

        // Create a submenu for the backend models
        auto *modelSubMenu = new QMenu(backendName, ui->menuAI_model);

        // Retrieve models for the current backend
        QStringList models = OpenAiService::instance()->getModelsForBackend(backendId);

        // Add each model as an action to the submenu
        for (const QString &modelId : models) {
            auto *modelAction = new QAction(modelId, modelSubMenu);
            modelAction->setData(QStringList() << backendId << modelId);
            modelSubMenu->addAction(modelAction);
            modelAction->setActionGroup(_aiModelGroup);
            modelAction->setCheckable(true);
        }

        // Add the submenu to the main menu
        ui->menuAI_model->addMenu(modelSubMenu);
    }
}

void MainWindow::aiModelMainMenuSetCurrentItem() {
    auto currentBackendId = OpenAiService::instance()->getBackendId();
    auto currentModelId = OpenAiService::instance()->getModelId();
    auto action = Utils::Gui::findActionByData(ui->menuAI_model,
                                               QStringList() << currentBackendId << currentModelId);

    if (action) {
        _aiModelGroup->blockSignals(true);
        action->setChecked(true);
        _aiModelGroup->blockSignals(false);
    }
}

/**
 * Sets the AI model when the AI model combo box index has changed
 */
void MainWindow::onAiModelComboBoxCurrentIndexChanged(int index) {
    Q_UNUSED(index)

    const QString modelId = _aiModelComboBox->currentData().toString();

    if (OpenAiService::instance()->setModelId(modelId)) {
        generateAiModelComboBox();
        aiModelMainMenuSetCurrentItem();
    }
}

void MainWindow::onAiModelGroupChanged(QAction *action) {
    const auto data = action->data().toStringList();
    const auto &backendId = data[0];
    const auto &modelId = data[1];

    if (OpenAiService::instance()->setBackendId(backendId)) {
        generateAiBackendComboBox();
    }

    if (OpenAiService::instance()->setModelId(modelId)) {
        generateAiModelComboBox();
    }
}

/**
 * Puts items into the AI model combo box
 */
void MainWindow::generateAiModelComboBox() {
    _aiModelComboBox->blockSignals(true);
    _aiModelComboBox->clear();
    const auto models = OpenAiService::instance()->getModelsForCurrentBackend();

    foreach (QString model, models) {
        _aiModelComboBox->addItem(model, model);
    }

    Utils::Gui::setComboBoxIndexByUserData(_aiModelComboBox,
                                           OpenAiService::instance()->getModelId());
    _aiModelComboBox->blockSignals(false);
}

/**
 * Sets a new current workspace
 */
void MainWindow::setCurrentWorkspace(const QString &uuid) {
    // store the current workspace
    storeCurrentWorkspace();

    SettingsService settings;
    QString currentUuid = currentWorkspaceUuid();
    settings.setValue(QStringLiteral("previousWorkspace"), currentUuid);
    settings.setValue(QStringLiteral("currentWorkspace"), uuid);

    // restore the new workspace
    QTimer::singleShot(0, this, SLOT(restoreCurrentWorkspace()));

    // update the menu and combo box (but don't rebuild it)
    updateWorkspaceLists(false);

    // update the preview in case it was disabled previously
    setNoteTextFromNote(&currentNote, true);

    ScriptingService::instance()->callWorkspaceSwitchedHook(currentUuid, uuid);
}

/**
 * Stores the current workspace
 */
void MainWindow::storeCurrentWorkspace() {
    SettingsService settings;
    QString uuid = currentWorkspaceUuid();

    settings.setValue(QStringLiteral("workspace-") + uuid + QStringLiteral("/windowState"),
                      saveState());
    settings.setValue(
        QStringLiteral("workspace-") + uuid + QStringLiteral("/noteSubFolderDockWidgetVisible"),
        _noteSubFolderDockWidgetVisible);
}

/**
 * Restores the current workspace
 */
void MainWindow::restoreCurrentWorkspace() {
    SettingsService settings;
    QStringList workspaces = getWorkspaceUuidList();
    QWidget *focusWidget = qApp->focusWidget();

    // create a default workspace if there is none yet
    if (workspaces.count() == 0) {
        createNewWorkspace(tr("full", "full workspace"));

        _taggingDockWidget->setVisible(false);
        _noteFolderDockWidget->setVisible(false);
        _noteNavigationDockWidget->setVisible(false);
        _noteTagDockWidget->setVisible(false);
        _notePreviewDockWidget->setVisible(false);
        _noteGraphicsViewDockWidget->setVisible(false);
        createNewWorkspace(tr("minimal", "minimal workspace"));

        // TODO: maybe still create those workspaces initially?
    }

    QString uuid = currentWorkspaceUuid();

    // set the first workspace as current workspace if there is none set
    if (uuid.isEmpty()) {
        workspaces = getWorkspaceUuidList();

        if (workspaces.count() == 0) {
            return;
        }

        uuid = workspaces.at(0);
        settings.setValue(QStringLiteral("currentWorkspace"), uuid);

        // update the menu and combo box
        updateWorkspaceLists();
    }

    restoreState(
        settings.value(QStringLiteral("workspace-") + uuid + QStringLiteral("/windowState"))
            .toByteArray());

    // handle the visibility of the note subfolder panel
    handleNoteSubFolderVisibility();

    // update the panel lists
    updatePanelMenu();

    // check if the user wanted the note subfolder dock widget visible
    _noteSubFolderDockWidgetVisible =
        settings
            .value(QStringLiteral("workspace-") + uuid +
                       QStringLiteral("/noteSubFolderDockWidgetVisible"),
                   true)
            .toBool();

    // set the visibility of the note subfolder dock widget
    handleNoteSubFolderVisibility();

    // if app was newly installed we want to center and resize the window
    if (settings.value(QStringLiteral("initialWorkspace")).toBool()) {
        MetricsService::instance()->sendEventIfEnabled(
            QStringLiteral("app/initial-layout"), QStringLiteral("app"),
            QStringLiteral("initial-layout"),
            settings.value(QStringLiteral("initialLayoutIdentifier")).toString());

        settings.remove(QStringLiteral("initialWorkspace"));
        centerAndResize();
    }

    if (focusWidget != nullptr) {
        // set the focus to the widget that had the focus before
        // the workspace was restored
        focusWidget->setFocus();
    }
}

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
QStringList MainWindow::getWorkspaceUuidList() {
    SettingsService settings;
    return settings.value(QStringLiteral("workspaces")).toStringList();
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
    if (Utils::Gui::question(this, tr("Remove current workspace"),
                             tr("Remove the current workspace?"),
                             QStringLiteral("remove-workspace")) != QMessageBox::Yes) {
        return;
    }

    // reset current workspace
    workspaces.removeAll(uuid);
    const QString newUuid = workspaces.at(0);

    // set the new workspace
    setCurrentWorkspace(newUuid);

    SettingsService settings;
    settings.setValue(QStringLiteral("workspaces"), workspaces);

    // remove all settings in the group
    settings.beginGroup(QStringLiteral("workspace-") + uuid);
    settings.remove(QLatin1String(""));
    settings.endGroup();

    // update the menu and combo box
    updateWorkspaceLists();
}

void MainWindow::on_actionRename_current_workspace_triggered() {
    const QString uuid = currentWorkspaceUuid();

    // if no workspace is set we can't rename it
    if (uuid.isEmpty()) {
        return;
    }

    SettingsService settings;
    QString name =
        settings.value(QStringLiteral("workspace-") + uuid + QStringLiteral("/name")).toString();

    // ask for the new name
    name = QInputDialog::getText(this, tr("Rename workspace"), tr("Workspace name:"),
                                 QLineEdit::Normal, name)
               .trimmed();

    if (name.isEmpty()) {
        return;
    }

    // rename the workspace
    settings.setValue(QStringLiteral("workspace-") + uuid + QStringLiteral("/name"), name);

    // update the menu and combo box
    updateWorkspaceLists();
}

/**
 * Switch to the previous workspace
 */
void MainWindow::on_actionSwitch_to_previous_workspace_triggered() {
    SettingsService settings;
    QString uuid = settings.value(QStringLiteral("previousWorkspace")).toString();

    if (!uuid.isEmpty()) {
        setCurrentWorkspace(uuid);
    }
}

/**
 * Shows all dock widgets
 */
void MainWindow::on_actionShow_all_panels_triggered() {
    const QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();

    for (QDockWidget *dockWidget : dockWidgets) {
        dockWidget->setVisible(true);
    }

    _noteSubFolderDockWidgetVisible = true;

    // handle the visibility of the note subfolder panel
    handleNoteSubFolderVisibility();

    // update the preview in case it was disabled previously
    setNoteTextFromNote(&currentNote, true);

    // filter notes according to selections
    filterNotes();
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
    if (!_noteEditLineNumberLabel->isVisible()) return;
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor cursor = textEdit->textCursor();
    QString selectedText = cursor.selectedText();

    this->noteHistory.updateCursorPositionOfNote(currentNote, textEdit);

    QString text = tr("Ln %1, Col %2", "Line / Column")
                       .arg(QString::number(cursor.block().blockNumber() + 1),
                            QString::number(cursor.positionInBlock() + 1));
    QString toolTip = tr("Line %1, Column %2")
                          .arg(QString::number(cursor.block().blockNumber() + 1),
                               QString::number(cursor.positionInBlock() + 1));

    if (!selectedText.isEmpty()) {
        const QString textAdd = QStringLiteral(" (") +
                                tr("%n selected", "Characters selected", selectedText.size()) +
                                QStringLiteral(")");
        text += textAdd;
        toolTip += textAdd;
    }

    _noteEditLineNumberLabel->setText(text);
    _noteEditLineNumberLabel->setToolTip(toolTip);

    const bool autoSelect =
        SettingsService().value(QStringLiteral("navigationPanelAutoSelect"), true).toBool();
    if (autoSelect) {
        selectNavigationItemAtPosition(textEdit->textCursor().block().position());
    }
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
    on_tagTreeWidget_itemExpanded(item);
}

/**
 * Stores the note tag tree expand state when an tree widget item was expanded
 */
void MainWindow::on_tagTreeWidget_itemExpanded(QTreeWidgetItem *item) {
    Q_UNUSED(item)
    storeTagTreeWidgetExpandState();
}

/**
 * Stores the note tag tree expand state
 */
void MainWindow::storeTagTreeWidgetExpandState() const {
    // get all items
    const auto allItems =
        ui->tagTreeWidget->findItems(QLatin1String(""), Qt::MatchContains | Qt::MatchRecursive);

    QStringList expandedList;
    for (QTreeWidgetItem *item : allItems) {
        if (Utils::Gui::isOneTreeWidgetItemChildVisible(item)) {
            if (item->isExpanded()) {
                expandedList << item->data(0, Qt::UserRole).toString();
            }
        }
    }

    SettingsService settings;
    settings.setValue(QStringLiteral("MainWindow/tagTreeWidgetExpandState-") +
                          QString::number(NoteFolder::currentNoteFolderId()),
                      expandedList);
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
    if (!_noteEditIsCentralWidget) {
        _noteEditDockWidget->show();
    }

    activeNoteTextEdit()->setFocus();
}

/**
 * Double-clicking a tag assigns the tag to the current note
 */
void MainWindow::on_tagTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)
    Tag tag = Tag::fetch(item->data(0, Qt::UserRole).toInt());

    if (tag.isFetched()) {
        // workaround when signal block doesn't work correctly
        directoryWatcherWorkaround(true, true);

        const QSignalBlocker blocker(noteDirectoryWatcher);
        Q_UNUSED(blocker)

        if (tag.isLinkedToNote(currentNote)) {
            tag.removeLinkToNote(currentNote);
            handleScriptingNoteTagging(currentNote, tag, true, false);
        } else {
            tag.linkToNote(currentNote);
            handleScriptingNoteTagging(currentNote, tag, false, false);
        }

        if (!NoteFolder::isCurrentNoteTreeEnabled()) {
            filterNotes();
        }

        reloadCurrentNoteTags();
        reloadTagTree();

        // turn off the workaround again
        directoryWatcherWorkaround(false, true);
    }
}

/**
 * Double-clicking a note calls a hook
 */
void MainWindow::on_noteTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column) {
    Q_UNUSED(item)
    Q_UNUSED(column)

    // call a script hook that a new note was double-clicked
    const bool hookFound =
        ScriptingService::instance()->callHandleNoteDoubleClickedHook(&currentNote);

    if (!hookFound) {
        openCurrentNoteInTab();
    }
}

/**
 * Reloads the current note (and selected notes) tags if there were selected
 * multiple notes
 */
void MainWindow::on_noteTreeWidget_itemSelectionChanged() {
    qDebug() << __func__;
    const auto &selectedItems = ui->noteTreeWidget->selectedItems();
    if (selectedItems.size() == 1) {
        // Don't tread folders as notes
        if (selectedItems[0]->data(0, Qt::UserRole + 1).toInt() == FolderType) {
            return;
        }

        int noteId = selectedItems[0]->data(0, Qt::UserRole).toInt();
        Note note = Note::fetch(noteId);
        bool currentNoteChanged = currentNote.getId() != noteId;
        setCurrentNote(std::move(note), true, false);

        // Let's highlight the text from the search line edit and do an "in-note
        // search" if the current note has changed and there is a search term
        // in the search line edit
        if (currentNoteChanged && !ui->searchLineEdit->text().isEmpty()) {
            searchForSearchLineTextInNoteTextEdit();

            // prevent that the last occurrence of the search term is found
            // first, instead the first occurrence should be found first
            ui->noteTextEdit->searchWidget()->doSearchDown();
            ui->noteTextEdit->searchWidget()->updateSearchExtraSelections();
        }
    }

    // we also need to do this in setCurrentNote because of different timings
    reloadCurrentNoteTags();
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
    QPoint globalPos =
        ui->noteOperationsButton->mapToGlobal(QPoint(0, ui->noteOperationsButton->height()));
    openNotesContextMenu(globalPos, true);
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
    Utils::Gui::searchForTextInTreeWidget(ui->navigationWidget, arg1,
                                          Utils::Gui::TreeWidgetSearchFlag::IntCheck);
    Utils::Gui::searchForTextInTreeWidget(ui->backlinkWidget, arg1,
                                          Utils::Gui::TreeWidgetSearchFlag::IntCheck);
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
    // #1302: we need to init the button in any case if the app was already in
    //        fullscreen mode or "disconnect" will crash the app
    if (_leaveFullScreenModeButton == nullptr) {
        _leaveFullScreenModeButton = new QPushButton(tr("leave"));
    }

    if (isFullScreen()) {
        showNormal();

        // we need a showNormal() first to exist full-screen mode
        if (_isMaximizedBeforeFullScreen) {
            showMaximized();
        } else if (_isMinimizedBeforeFullScreen) {
            showMinimized();
        }

        statusBar()->removeWidget(_leaveFullScreenModeButton);
        disconnect(_leaveFullScreenModeButton, nullptr, nullptr, nullptr);
        delete _leaveFullScreenModeButton;
        _leaveFullScreenModeButton = nullptr;
    } else {
        _isMaximizedBeforeFullScreen = isMaximized();
        _isMinimizedBeforeFullScreen = isMinimized();
        showFullScreen();

        _leaveFullScreenModeButton->setFlat(true);
        _leaveFullScreenModeButton->setToolTip(tr("Leave full-screen mode"));
        _leaveFullScreenModeButton->setStyleSheet(QStringLiteral("QPushButton {padding: 0 5px}"));

        _leaveFullScreenModeButton->setIcon(QIcon::fromTheme(
            QStringLiteral("zoom-original"),
            QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/zoom-original.svg"))));

        connect(_leaveFullScreenModeButton, &QPushButton::clicked, this,
                &MainWindow::on_actionToggle_fullscreen_triggered);

        statusBar()->addPermanentWidget(_leaveFullScreenModeButton);
    }
}

void MainWindow::disableFullScreenMode() {
    if (isFullScreen()) {
        on_actionToggle_fullscreen_triggered();
    }
}

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
    SettingsService settings;
    settings.setValue(QStringLiteral("checkSpelling"), checked);
    ui->noteTextEdit->updateSettings();
    ui->encryptedNoteTextEdit->updateSettings();

    // if spell checking was turned on still turn it off for the current note
    // if encrypted text is shown
    if (checked) {
        updateNoteEncryptionUI();
    }
}

void MainWindow::loadDictionaryNames() {
    SettingsService settings;

    QStringList languages = Sonnet::Speller::availableLanguages();
    QStringList langNames = Sonnet::Speller::availableLanguageNames();

    // if there are no dictionaries installed, disable the spellchecker
    if (languages.isEmpty()) {
        settings.setValue(QStringLiteral("checkSpelling"), false);
        ui->actionCheck_spelling->setEnabled(false);
        ui->menuLanguages->setTitle(QStringLiteral("No dictionaries found"));
        ui->menuLanguages->setEnabled(false);
        ui->noteTextEdit->updateSettings();
        return;
    }

    _languageGroup->setExclusive(true);
    connect(_languageGroup, &QActionGroup::triggered, this, &MainWindow::onLanguageChanged);

    // first add autoDetect
    QAction *autoDetect = ui->menuLanguages->addAction(tr("Automatically detect"));
    autoDetect->setCheckable(true);
    autoDetect->setData(QStringLiteral("auto"));
    autoDetect->setActionGroup(_languageGroup);
    QString prevLang =
        settings.value(QStringLiteral("spellCheckLanguage"), QStringLiteral("auto")).toString();
    // if only one dictionary found, disable auto detect
    if (languages.length() > 1) {
        if (prevLang == QStringLiteral("auto")) {
            autoDetect->setChecked(true);
            autoDetect->trigger();
        }
    } else {
        autoDetect->setChecked(false);
        autoDetect->setEnabled(false);
    }

    // not really possible but just in case
    if (langNames.length() != languages.length()) {
        qWarning() << "Error: langNames.length != languages.length()";
        return;
    }

    QStringList::const_iterator it = langNames.constBegin();
    QStringList::const_iterator itt = languages.constBegin();
    for (; it != langNames.constEnd(); ++it, ++itt) {
        QAction *action = ui->menuLanguages->addAction(*it);
        action->setCheckable(true);
        action->setActionGroup(_languageGroup);
        action->setData(*itt);

        if (*itt == prevLang || languages.length() == 1) {
            action->setChecked(true);
            action->trigger();
        }
    }
}

void MainWindow::onLanguageChanged(QAction *action) {
    QString lang = action->data().toString();
    SettingsService settings;
    settings.setValue(QStringLiteral("spellCheckLanguage"), lang);
    ui->noteTextEdit->updateSettings();
}

void MainWindow::loadSpellingBackends() {
    SettingsService settings;
    QString prevBackend =
        settings.value(QStringLiteral("spellCheckBackend"), QStringLiteral("Hunspell")).toString();

    _spellBackendGroup->setExclusive(true);
    connect(_spellBackendGroup, &QActionGroup::triggered, this, &MainWindow::onSpellBackendChanged);

    QAction *hs = ui->menuSpelling_backend->addAction(QStringLiteral("Hunspell"));
    hs->setCheckable(true);
    hs->setData("Hunspell");
    hs->setActionGroup(_spellBackendGroup);
    QAction *as = ui->menuSpelling_backend->addAction(QStringLiteral("Aspell"));
    as->setCheckable(true);
    as->setActionGroup(_spellBackendGroup);
    as->setData("Aspell");

    if (prevBackend == hs->data()) {
        hs->setChecked(true);
    } else {
        as->setChecked(true);
    }
}

void MainWindow::onSpellBackendChanged(QAction *action) {
    QString backend = action->data().toString();
    SettingsService settings;
    settings.setValue(QStringLiteral("spellCheckBackend"), backend);
    showRestartNotificationIfNeeded(true);
}

void MainWindow::on_actionManage_dictionaries_triggered() {
    auto *dialog = new DictionaryManagerDialog(this);
    dialog->exec();
    delete (dialog);

    // shows a restart application notification
    showRestartNotificationIfNeeded();
}

void MainWindow::on_noteTextEdit_modificationChanged(bool arg1) {
    if (!arg1) {
        return;
    }

    ui->noteTextEdit->document()->setModified(false);
    noteTextEditTextWasUpdated();
}

void MainWindow::on_encryptedNoteTextEdit_modificationChanged(bool arg1) {
    if (!arg1) {
        return;
    }

    ui->encryptedNoteTextEdit->document()->setModified(false);

    if (currentNote.storeNewDecryptedText(ui->encryptedNoteTextEdit->toPlainText())) {
        handleNoteTextChanged();
    }
}

void MainWindow::on_actionEditorWidthCustom_triggered() {
    SettingsService settings;
    bool ok;
    int characters = QInputDialog::getInt(
        this, tr("Custom editor width"), tr("Characters:"),
        settings.value(QStringLiteral("DistractionFreeMode/editorWidthCustom"), 80).toInt(), 20,
        10000, 1, &ok);

    if (ok) {
        settings.setValue(QStringLiteral("DistractionFreeMode/editorWidthCustom"), characters);
    }
}

void MainWindow::on_actionShow_Hide_application_triggered() {
    // isVisible() or isHidden() didn't work properly
    if (isActiveWindow()) {
        hide();
    } else {
        showWindow();
    }
}

void MainWindow::on_noteEditTabWidget_currentChanged(int index) {
    QWidget *widget = ui->noteEditTabWidget->currentWidget();

    if (widget == nullptr) {
        return;
    }

    const int noteId = widget->property("note-id").toInt();

    // close the tab if note doesn't exist anymore
    if (!Note::noteIdExists(noteId)) {
        removeNoteTab(index);
        return;
    }

    // Allow the subfolder of the note to be selected in the subfolder list
    // See: https://github.com/pbek/QOwnNotes/issues/2861
    if (SettingsService()
            .value(QStringLiteral("noteSubfoldersPanelTabsUnsetAllNotesSelection"))
            .toBool()) {
        setShowNotesFromAllNoteSubFolders(false);
    }

    setCurrentNoteFromNoteId(noteId);
    widget->setLayout(ui->noteEditTabWidgetLayout);

    closeOrphanedTabs();
}

void MainWindow::on_noteEditTabWidget_tabCloseRequested(int index) { removeNoteTab(index); }

void MainWindow::on_actionPrevious_note_tab_triggered() {
    int index = ui->noteEditTabWidget->currentIndex() - 1;

    if (index < 0) {
        index = ui->noteEditTabWidget->count() - 1;
    }

    ui->noteEditTabWidget->setCurrentIndex(index);
    focusNoteTextEdit();
}

void MainWindow::on_actionNext_note_tab_triggered() {
    int index = ui->noteEditTabWidget->currentIndex() + 1;

    if (index >= ui->noteEditTabWidget->count()) {
        index = 0;
    }

    ui->noteEditTabWidget->setCurrentIndex(index);
    focusNoteTextEdit();
}

void MainWindow::on_actionClose_current_note_tab_triggered() {
    removeNoteTab(ui->noteEditTabWidget->currentIndex());
}

void MainWindow::on_actionNew_note_in_new_tab_triggered() {
    on_action_New_note_triggered();
    openCurrentNoteInTab();
}

/**
 * Close a note tab on a specific index.
 * @param index The index of the tab to close.
 */
bool MainWindow::removeNoteTab(int index) const {
    const int maxIndex = ui->noteEditTabWidget->count() - 1;

    if (maxIndex <= 0 || index > maxIndex) {
        return false;
    }

    ui->noteEditTabWidget->removeTab(index);
    return true;
}

/**
 * Returns a list of note ids that are opened in tabs
 */
QList<int> MainWindow::getNoteTabNoteIdList() const {
    QList<int> resultList;

    for (int i = 0; i < ui->noteEditTabWidget->count(); i++) {
        auto widget = ui->noteEditTabWidget->widget(i);
        const int noteId = widget->property("note-id").toInt();
        resultList.append(noteId);
    }

    return resultList;
}

void MainWindow::on_noteEditTabWidget_tabBarDoubleClicked(int index) {
    // If the empty area of the tab widget is clicked, open a new note in a new tab
    if (index == -1) {
        createNewNote();
        openCurrentNoteInTab();
        return;
    }

    // Make the note tab "sticky"
    Utils::Gui::setTabWidgetTabSticky(
        ui->noteEditTabWidget, index,
        !Utils::Gui::isTabWidgetTabSticky(ui->noteEditTabWidget, index));
}

void MainWindow::on_actionToggle_note_stickiness_of_current_tab_triggered() {
    on_noteEditTabWidget_tabBarDoubleClicked(ui->noteEditTabWidget->currentIndex());
}

/**
 * If the tab of current note was clicked now the subfolder of the note is
 * activated if that is needed to show the note in the note list
 */
void MainWindow::on_noteEditTabWidget_tabBarClicked(int index) {
    if (ui->noteEditTabWidget->currentIndex() != index) {
        return;
    }

    if (!_showNotesFromAllNoteSubFolders && !currentNote.isInCurrentNoteSubFolder()) {
        jumpToNoteSubFolder(currentNote.getNoteSubFolderId());
    }
}

/**
 * Note tab context menu
 */
void MainWindow::showNoteEditTabWidgetContextMenu(const QPoint &point) {
    if (point.isNull()) {
        return;
    }

    int tabIndex = ui->noteEditTabWidget->tabBar()->tabAt(point);
    auto *menu = new QMenu();

    // Toggle note stickiness
    auto *stickAction = menu->addAction(tr("Toggle note stickiness"));
    connect(stickAction, &QAction::triggered, this,
            [this, tabIndex]() { on_noteEditTabWidget_tabBarDoubleClicked(tabIndex); });

    // Close other note tabs
    auto *closeOtherAction = menu->addAction(tr("Close other note tabs"));
    connect(closeOtherAction, &QAction::triggered, this, [this, tabIndex]() {
        const int maxIndex = ui->noteEditTabWidget->count() - 1;
        const int keepNoteId = Utils::Gui::getTabWidgetNoteId(ui->noteEditTabWidget, tabIndex);

        for (int i = maxIndex; i >= 0; i--) {
            const int noteId = Utils::Gui::getTabWidgetNoteId(ui->noteEditTabWidget, i);

            if (noteId != keepNoteId) {
                removeNoteTab(i);
            }
        }
    });

    // Close note tab
    auto *closeAction = menu->addAction(tr("Close note tab"));
    connect(closeAction, &QAction::triggered, this,
            [this, tabIndex]() { removeNoteTab(tabIndex); });

    menu->exec(ui->noteEditTabWidget->tabBar()->mapToGlobal(point));
}

void MainWindow::on_actionJump_to_navigation_panel_triggered() {
    if (ui->navigationLineEdit->isVisible()) {
        ui->navigationLineEdit->setFocus();
    } else if (ui->navigationWidget->isVisible()) {
        ui->navigationWidget->setFocus();
    } else if (ui->backlinkWidget->isVisible()) {
        ui->backlinkWidget->setFocus();
    }
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

void MainWindow::buildAiToolbarAndActions() {
    _aiToolbar->clear();
    _aiToolbar->addAction(ui->actionEnable_AI);
    ui->actionEnable_AI->blockSignals(true);
    ui->actionEnable_AI->setChecked(OpenAiService::getEnabled());
    ui->actionEnable_AI->blockSignals(false);

    _aiBackendComboBox = new QComboBox(this);
    connect(_aiBackendComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::onAiBackendComboBoxCurrentIndexChanged);
    _aiBackendComboBox->setToolTip(tr("AI backends"));
    _aiBackendComboBox->setObjectName(QStringLiteral("aiBackendComboBox"));
    _aiBackendComboBox->setInsertPolicy(QComboBox::InsertPolicy::InsertAfterCurrent);
    generateAiBackendComboBox();

    _aiModelComboBox = new QComboBox(this);
    connect(_aiModelComboBox,
            static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this,
            &MainWindow::onAiModelComboBoxCurrentIndexChanged);
    _aiModelComboBox->setToolTip(tr("AI models"));
    _aiModelComboBox->setObjectName(QStringLiteral("aiModelComboBox"));
    _aiModelComboBox->setInsertPolicy(QComboBox::InsertPolicy::InsertAfterCurrent);
    generateAiModelComboBox();

    auto *aiBackendWidgetAction = new QWidgetAction(this);
    aiBackendWidgetAction->setDefaultWidget(_aiBackendComboBox);
    aiBackendWidgetAction->setObjectName(QStringLiteral("actionAiBackendComboBox"));
    aiBackendWidgetAction->setText(tr("AI backend selector"));
    _aiToolbar->addAction(aiBackendWidgetAction);

    auto *aiModelWidgetAction = new QWidgetAction(this);
    aiModelWidgetAction->setDefaultWidget(_aiModelComboBox);
    aiModelWidgetAction->setObjectName(QStringLiteral("actionAiModelComboBox"));
    aiModelWidgetAction->setText(tr("AI model selector"));
    _aiToolbar->addAction(aiModelWidgetAction);

    _aiModelGroup->setExclusive(true);
    connect(_aiModelGroup, &QActionGroup::triggered, this, &MainWindow::onAiModelGroupChanged);
    generateAiModelMainMenu();
    aiModelMainMenuSetCurrentItem();
}

void MainWindow::on_actionEnable_AI_toggled(bool arg1) {
    OpenAiService::setEnabled(arg1);
    qDebug() << __func__ << " - 'checked': " << arg1;
}

void MainWindow::on_navigationTabWidget_currentChanged(int index) {
    Q_UNUSED(index)

    startNavigationParser();
}

void MainWindow::enableOpenAiActivitySpinner(bool enable) {
    if (_openAiActivitySpinner == nullptr) {
        return;
    }

    if (enable) {
        _openAiActivitySpinner->start();
    } else {
        _openAiActivitySpinner->stop();
    }
}

/**
 * Reattaches all floating panels in case they can't be reattached manually anymore
 */
void MainWindow::on_actionReattach_panels_triggered() {
    const QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();

    for (QDockWidget *dockWidget : dockWidgets) {
        if (!dockWidget->isFloating()) {
            continue;
        }

        dockWidget->setFloating(false);

        // Remove the title bar if panels are locked
        if (!ui->actionUnlock_panels->isChecked()) {
            handleDockWidgetLocking(dockWidget);
        }
    }
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
