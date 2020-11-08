/*
 * Copyright (c) 2014-2020 Patrizio Bekerle -- <patrizio@bekerle.com>
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
#include <dialogs/localtrashdialog.h>
#include <dialogs/notedialog.h>
#include <dialogs/scriptrepositorydialog.h>
#include <dialogs/sharedialog.h>
#include <dialogs/tabledialog.h>
#include <dialogs/tagadddialog.h>
#include <entities/notefolder.h>
#include <entities/notesubfolder.h>
#include <entities/tag.h>
#include <entities/trashitem.h>
#include <helpers/clientproxy.h>
#include <helpers/fakevimproxy.h>
#include <helpers/toolbarcontainer.h>
#include <helpers/flowlayout.h>
#include <services/cryptoservice.h>
#include <services/scriptingservice.h>
#include <utils/git.h>
#include <utils/gui.h>
#include <utils/misc.h>
#include <utils/schema.h>
#include <widgets/logwidget.h>
#include <widgets/notetreewidgetitem.h>
#include <QAbstractEventDispatcher>
#include <QActionGroup>
#include <QClipboard>
#include <QColorDialog>
#include <QCompleter>
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QDirIterator>
#include <QDockWidget>
#include <QFile>
#include <QInputDialog>
#include <QJSEngine>
#include <QKeyEvent>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QMimeData>
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QPrinter>
#include <QProcess>
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
#include <QSettings>
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
#include <libraries/qttoolbareditor/src/toolbar_editor.hpp>
#include <utility>

#include "build_number.h"
#include "dialogs/aboutdialog.h"
#include "dialogs/issueassistantdialog.h"
#include "dialogs/linkdialog.h"
#include "dialogs/notediffdialog.h"
#include "dialogs/orphanedattachmentsdialog.h"
#include "dialogs/orphanedimagesdialog.h"
#include "dialogs/passworddialog.h"
#include "dialogs/settingsdialog.h"
#include "dialogs/tododialog.h"
#include "entities/calendaritem.h"
#include "helpers/qownnotesmarkdownhighlighter.h"
#include <diff_match_patch.h>
#include "libraries/fakevim/fakevim/fakevimhandler.h"
#include "libraries/fakevim/fakevim/fakevimactions.h"
#include "libraries/sonnet/src/core/speller.h"
#include "release.h"
#include "services/databaseservice.h"
#include "services/metricsservice.h"
#include "services/owncloudservice.h"
#include "services/updateservice.h"
#include "services/websocketserverservice.h"
#include "ui_mainwindow.h"
#include "version.h"
#include "widgets/qownnotesmarkdowntextedit.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    // handle logging as signal/slot to even more prevent crashes when
    // writing to the log-widget while the app is shutting down
    connect(this, &MainWindow::log, LogWidget::instance(), &LogWidget::log);

    // use our custom log handler
    qInstallMessageHandler(LogWidget::logMessageOutput);
    qApp->setProperty("loggingEnabled", true);

#ifdef Q_OS_MAC
    // disable icons in the menu
    QApplication::instance()->setAttribute(Qt::AA_DontShowIconsInMenus, true);
#endif

    QSettings settings;
    _noteEditIsCentralWidget =
        settings.value(QStringLiteral("noteEditIsCentralWidget"), true)
            .toBool();

    ui->setupUi(this);

#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    ui->noteEditTabWidget->setTabBarAutoHide(true);
#endif
    ui->noteEditTabWidget->tabBar()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->noteEditTabWidget->tabBar(), &QWidget::customContextMenuRequested,
            this, &MainWindow::showNoteEditTabWidgetContextMenu);

    // setup vim mode
    if (settings.value(QStringLiteral("Editor/vimMode")).toBool()) {
        initFakeVim(ui->noteTextEdit);
        initFakeVim(ui->encryptedNoteTextEdit);
    }

    setWindowIcon(getSystemTrayIcon());

    // initialize the workspace combo box
    initWorkspaceComboBox();

#ifdef Q_OS_MAC
    // disable icons in the menu that weren't handled by
    // Qt::AA_DontShowIconsInMenus
    ui->actionShare_note->setIconVisibleInMenu(false);

    // set another shortcut for delete line under macOS
    ui->actionDelete_line->setShortcut(
        QKeySequence(QStringLiteral("Ctrl+Backspace")));
    ui->actionDelete_word->setShortcut(
        QKeySequence(QStringLiteral("Alt+Backspace")));

    // set another shortcut for the full-screen mode
    ui->actionToggle_fullscreen->setShortcut(
        QKeySequence(QStringLiteral("Ctrl+F11")));
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
    _settingsDialog = Q_NULLPTR;
    _lastNoteSelectionWasMultiple = false;
    _webSocketServerService = Q_NULLPTR;
    _closeEventWasFired = false;
    _leaveFullScreenModeButton = nullptr;
    _useNoteFolderButtons = settings.value("useNoteFolderButtons").toBool();

    this->setWindowTitle(QStringLiteral("QOwnNotes - version ") +
                         QStringLiteral(VERSION) + QStringLiteral(" - build ") +
                         QString::number(BUILD));

    qApp->setProperty("mainWindow", QVariant::fromValue<MainWindow *>(this));

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

    // set the search frames for the note text edits
    bool darkMode = settings.value(QStringLiteral("darkMode")).toBool();
    ui->noteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame, darkMode);
    ui->encryptedNoteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame,
                                               darkMode);
    ui->noteTextView->initSearchFrame(ui->noteTextViewSearchFrame, darkMode);

    // set the main window for accessing it's public methods
    ui->noteTextEdit->setMainWindow(this);
    ui->encryptedNoteTextEdit->setMainWindow(this);

    // initialize the tag button scroll area
    initTagButtonScrollArea();

    noteHistory = NoteHistory();

    // initialize the toolbars
    initToolbars();

    if (!settings.value(QStringLiteral("guiFirstRunInit")).toBool()) {
        // hide the custom action toolbar initially
        _customActionToolbar->hide();
        settings.setValue(QStringLiteral("guiFirstRunInit"), true);
    }

#ifdef Q_OS_MAC
    // add some different shortcuts for the note history on the mac
    ui->action_Back_in_note_history->setShortcut(Qt::CTRL + Qt::ALT +
                                                 Qt::Key_Left);
    ui->action_Forward_in_note_history->setShortcut(Qt::CTRL + Qt::ALT +
                                                    Qt::Key_Right);

    // add another shortcut for the auto-completer
    ui->actionAutocomplete->setShortcut(Qt::META + Qt::Key_Space);

    // add another shortcut for inserting media
    ui->actionPaste_image->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_V);
#endif

    // adding some alternate shortcuts for changing the current note
    auto *shortcut =
        new QShortcut(QKeySequence(QStringLiteral("Ctrl+PgDown")), this);
    connect(shortcut, &QShortcut::activated, this,
            &MainWindow::on_actionNext_note_triggered);

    shortcut = new QShortcut(QKeySequence(QStringLiteral("Ctrl+PgUp")), this);
    connect(shortcut, &QShortcut::activated, this,
            &MainWindow::on_actionPrevious_Note_triggered);

    // read the settings (shortcuts have to be defined before that)
    readSettings();

    // do a bit more styling
    initStyling();

    // initialize the dock widgets
    initDockWidgets();

    // initialize the scripting engine
    // initDockWidgets() has to be called before that so the scripting dock
    // widget is already in place
    initScriptingEngine();

    // restore toolbars
    // initDockWidgets() has to be called first so panel checkboxes can be
    // used in toolbars
    restoreToolbars();

    // update the workspace menu and combobox entries again after
    // restoreToolbars() to fill the workspace combo box again
    updateWorkspaceLists();

    // check if we want to start the application hidden
    initShowHidden();

    createSystemTrayIcon();
    buildNotesIndexAndLoadNoteDirectoryList(false, false, false);

    // setup the update available button
    setupStatusBarWidgets();

    this->noteDiffDialog = new NoteDiffDialog();

    // look if we need to save something every 10 sec (default)
    this->noteSaveTimer = new QTimer(this);
    connect(this->noteSaveTimer, &QTimer::timeout, this,
            &MainWindow::storeUpdatedNotesToDisk);

    this->noteSaveTimer->start(this->noteSaveIntervalTime * 1000);

    // look if we need update the note view every two seconds
    _noteViewUpdateTimer = new QTimer(this);
    _noteViewUpdateTimer->setSingleShot(true);
    connect(_noteViewUpdateTimer, &QTimer::timeout, this,
            &MainWindow::noteViewUpdateTimerSlot);

    _noteViewUpdateTimer->start(2000);

    // commit changes from the current note folder to git every 30 sec
    gitCommitCurrentNoteFolder();
    _gitCommitTimer = new QTimer(this);
    connect(_gitCommitTimer, &QTimer::timeout, this,
            &MainWindow::gitCommitCurrentNoteFolder);

    _gitCommitTimer->start(_gitCommitInterval * 1000);

    // check if we have a tasks reminder every minute
    this->todoReminderTimer = new QTimer(this);
    connect(this->todoReminderTimer, &QTimer::timeout, this,
            &MainWindow::frequentPeriodicChecker);

    this->todoReminderTimer->start(60000);

    QObject::connect(&this->noteDirectoryWatcher,
                     SIGNAL(directoryChanged(QString)), this,
                     SLOT(notesDirectoryWasModified(QString)));
    QObject::connect(&this->noteDirectoryWatcher, SIGNAL(fileChanged(QString)),
                     this, SLOT(notesWereModified(QString)));

    ui->searchLineEdit->installEventFilter(this);
    ui->noteTreeWidget->installEventFilter(this);
    ui->noteTextView->installEventFilter(this);
    ui->noteTextView->viewport()->installEventFilter(this);
    ui->noteTextEdit->installEventFilter(this);
    ui->noteTextEdit->viewport()->installEventFilter(this);
    ui->encryptedNoteTextEdit->installEventFilter(this);
    ui->encryptedNoteTextEdit->viewport()->installEventFilter(this);
    ui->tagTreeWidget->installEventFilter(this);
    ui->newNoteTagLineEdit->installEventFilter(this);
    ui->selectedTagsToolButton->installEventFilter(this);

    // init the saved searches completer
    initSavedSearchesCompleter();

    // ignores note clicks in QMarkdownTextEdit in the note text edit
    ui->noteTextEdit->setIgnoredClickUrlSchemata(QStringList({"note", "task"}));
    ui->encryptedNoteTextEdit->setIgnoredClickUrlSchemata(
        QStringList({"note", "task"}));

    // handle note url externally in the note text edit
    connect(ui->noteTextEdit, &QOwnNotesMarkdownTextEdit::urlClicked, this,
            &MainWindow::openLocalUrl);
    // also handle note url externally in the encrypted note text edit
    connect(ui->encryptedNoteTextEdit, &QOwnNotesMarkdownTextEdit::urlClicked,
            this, &MainWindow::openLocalUrl);

    // handle note edit zooming
    connect(ui->noteTextEdit, &QOwnNotesMarkdownTextEdit::zoomIn, this,
            &MainWindow::on_action_Increase_note_text_size_triggered);
    connect(ui->noteTextEdit, &QOwnNotesMarkdownTextEdit::zoomOut, this,
            &MainWindow::on_action_Decrease_note_text_size_triggered);
    connect(ui->encryptedNoteTextEdit, &QOwnNotesMarkdownTextEdit::zoomIn, this,
            &MainWindow::on_action_Increase_note_text_size_triggered);
    connect(ui->encryptedNoteTextEdit, &QOwnNotesMarkdownTextEdit::zoomOut, this,
            &MainWindow::on_action_Decrease_note_text_size_triggered);

    // handle note text edit resize events
    connect(ui->noteTextEdit, &QOwnNotesMarkdownTextEdit::resize, this,
            &MainWindow::noteTextEditResize);
    connect(ui->encryptedNoteTextEdit, &QOwnNotesMarkdownTextEdit::resize, this,
            &MainWindow::encryptedNoteTextEditResize);

    // set the tab stop to the width of 4 spaces in the editor
    const int tabStop = 4;
    QFont font = ui->noteTextEdit->font();
    QFontMetrics metrics(font);

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    int width = tabStop * metrics.width(' ');
    ui->noteTextEdit->setTabStopWidth(width);
    ui->encryptedNoteTextEdit->setTabStopWidth(width);
#else
    int width = tabStop * metrics.horizontalAdvance(' ');
    ui->noteTextEdit->setTabStopDistance(width);
    ui->encryptedNoteTextEdit->setTabStopDistance(width);
#endif

    // called now in readSettingsFromSettingsDialog() line 494
    // set the edit mode for the note text edit
    // this->setNoteTextEditMode(true);

    // load the note folder list in the menu
    this->loadNoteFolderListMenu();

    // update panels sort and order
    updatePanelsSortOrder();

    this->updateService = new UpdateService(this);
    this->updateService->checkForUpdates(this, UpdateService::AppStart);

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

    // setup the shortcuts for the note bookmarks
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
        connect(toolbar, &QToolBar::visibilityChanged, this,
                &MainWindow::toolbarVisibilityChanged);
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

    // do the navigation parsing after the highlighter was finished
    connect(ui->noteTextEdit->highlighter(),
            &QOwnNotesMarkdownHighlighter::highlightingFinished, this,
            &MainWindow::startNavigationParser);

    connect(ui->encryptedNoteTextEdit->highlighter(),
            &QOwnNotesMarkdownHighlighter::highlightingFinished, this,
            &MainWindow::startNavigationParser);

    // act on note preview resize
    connect(ui->noteTextView, &NotePreviewWidget::resize, this,
            &MainWindow::onNoteTextViewResize);

    // reloads all tasks from the ownCloud server
    reloadTodoLists();

    // create a timer to load all tasks every 10min
    _todoListTimer = new QTimer(this);
    connect(_todoListTimer, &QTimer::timeout, this,
            &MainWindow::reloadTodoLists);
    _todoListTimer->start(600000);

    // setup the soft-wrap checkbox
    const QSignalBlocker blocker2(ui->actionUse_softwrap_in_note_editor);
    Q_UNUSED(blocker2)
    ui->actionUse_softwrap_in_note_editor->setChecked(
        settings.value(QStringLiteral("useSoftWrapInNoteEditor"), true)
            .toBool());

    // initialize the editor soft wrapping
    initEditorSoftWrap();

    // check if user has set ownCloud settings
    MetricsService::instance()->sendEventIfEnabled(
        QStringLiteral("app/has-owncloud-settings"), QStringLiteral("app"),
        QStringLiteral("has owncloud settings"),
        OwnCloudService::hasOwnCloudSettings() ? QStringLiteral("yes")
                                               : QStringLiteral("no"));

    // send an event for counting the editor color schemes
    const int schemaCount =
        settings.value(QStringLiteral("Editor/ColorSchemes"))
            .toStringList()
            .count();
    MetricsService::instance()->sendEventIfEnabled(
        QStringLiteral("app/editor-color-schema-count"),
        QStringLiteral("editor"), QStringLiteral("editor color schema count"),
        QString::number(schemaCount) + QStringLiteral(" schemas"), schemaCount);

    _actionDialog = Q_NULLPTR;
    _todoDialog = Q_NULLPTR;
    _orphanedImagesDialog = Q_NULLPTR;
    _orphanedAttachmentsDialog = Q_NULLPTR;
    _issueAssistantDialog = Q_NULLPTR;

    // track cursor position changes for the line number label
    connect(ui->noteTextEdit, &QOwnNotesMarkdownTextEdit::cursorPositionChanged,
            this, &MainWindow::noteEditCursorPositionChanged);

    // track cursor position changes for the line number label
    connect(ui->encryptedNoteTextEdit,
            &QOwnNotesMarkdownTextEdit::cursorPositionChanged, this,
            &MainWindow::noteEditCursorPositionChanged);

    // restore the note tabs
    Utils::Gui::restoreNoteTabs(ui->noteEditTabWidget,
                                ui->noteEditTabWidgetLayout);

    if (isInDistractionFreeMode()) {
        ui->noteEditTabWidget->tabBar()->hide();
    }

    // restore the note history of the current note folder
    noteHistory.restoreForCurrentNoteFolder();

    if (settings.value(QStringLiteral("restoreLastNoteAtStartup"), true)
            .toBool()) {
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
    connect(qApp, &QApplication::commitDataRequest, this,
            &MainWindow::on_action_Quit_triggered);

    automaticScriptUpdateCheck();
}

/**
 * Initializes the global shortcuts
 */
void MainWindow::initGlobalKeyboardShortcuts() {
    // deleting old global shortcut assignments
    foreach(QHotkey *hotKey, _globalShortcuts) {
        delete hotKey;
    }

    _globalShortcuts.clear();
    QSettings settings;
    settings.beginGroup(QStringLiteral("GlobalShortcuts"));

    foreach(const QString &key, settings.allKeys()) {
        if (!key.contains(QStringLiteral("MainWindow"))) {
            continue;
        }

        QString actionName = key;
        actionName.remove(QStringLiteral("MainWindow-"));
        QAction *action = findAction(actionName);
        QString shortcut = settings.value(key).toString();

        auto hotKey = new QHotkey(QKeySequence(shortcut), true, this);
        _globalShortcuts.append(hotKey);
        connect(hotKey, &QHotkey::activated, this, [this, action]() {
            qDebug() << "Global shortcut action triggered: " << action->objectName();

            // bring application window to the front
            showWindow();

            action->trigger();
        });
    }
}

void MainWindow::initWebSocketServerService() {
    _webSocketServerService = new WebSocketServerService();
}

void MainWindow::initFakeVim(QOwnNotesMarkdownTextEdit *noteTextEdit) {
    auto handler = new FakeVim::Internal::FakeVimHandler(noteTextEdit, this);
    handler->installEventFilter();
    handler->setupWidget();

    auto proxy = new FakeVimProxy(noteTextEdit, this, handler);

    QSettings settings;
    bool setExpandTab = !settings.value(QStringLiteral("Editor/useTabIndent")).toBool();
    FakeVim::Internal::theFakeVimSettings()->item("et")->setValue(setExpandTab);
    auto width = settings.value("Editor/indentSize", 4).toInt();
    FakeVim::Internal::theFakeVimSettings()->item("ts")->setValue(width);

    QObject::connect(handler,
                     &FakeVim::Internal::FakeVimHandler::commandBufferChanged,
                     proxy, &FakeVimProxy::changeStatusMessage);
    QObject::connect(
        handler, &FakeVim::Internal::FakeVimHandler::extraInformationChanged,
        proxy, &FakeVimProxy::changeExtraInformation);
    QObject::connect(handler,
                     &FakeVim::Internal::FakeVimHandler::statusDataChanged,
                     proxy, &FakeVimProxy::changeStatusData);
    QObject::connect(handler,
                     &FakeVim::Internal::FakeVimHandler::highlightMatches,
                     proxy, &FakeVimProxy::highlightMatches);
    QObject::connect(
        handler, &FakeVim::Internal::FakeVimHandler::handleExCommandRequested,
        proxy, &FakeVimProxy::handleExCommand);
    QObject::connect(
        handler, &FakeVim::Internal::FakeVimHandler::requestSetBlockSelection,
        proxy, &FakeVimProxy::requestSetBlockSelection);
    QObject::connect(
        handler,
        &FakeVim::Internal::FakeVimHandler::requestDisableBlockSelection, proxy,
        &FakeVimProxy::requestDisableBlockSelection);
    QObject::connect(
        handler, &FakeVim::Internal::FakeVimHandler::requestHasBlockSelection,
        proxy, &FakeVimProxy::requestHasBlockSelection);

    QObject::connect(handler, &FakeVim::Internal::FakeVimHandler::indentRegion,
                     proxy, &FakeVimProxy::indentRegion);
    QObject::connect(
        handler, &FakeVim::Internal::FakeVimHandler::checkForElectricCharacter,
        proxy, &FakeVimProxy::checkForElectricCharacter);

    QObject::connect(
        proxy, &FakeVimProxy::handleInput, handler,
        [handler](const QString &text) { handler->handleInput(text); });
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
    QSettings settings;
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

    qApp->setProperty("loggingEnabled", false);

    if (showSystemTray) {
        // if we are using the system tray lets delete the log window so the
        // app can quit
        delete (LogWidget::instance());
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
    connect(
        _workspaceComboBox,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this, &MainWindow::onWorkspaceComboBoxCurrentIndexChanged);
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
    _noteSubFolderDockWidget->setObjectName(
        QStringLiteral("noteSubFolderDockWidget"));
    _noteSubFolderDockWidget->setWidget(ui->noteSubFolderFrame);
    _noteSubFolderDockTitleBarWidget =
        _noteSubFolderDockWidget->titleBarWidget();
    sizePolicy = _noteSubFolderDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteSubFolderDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteSubFolderDockWidget,
                  Qt::Horizontal);

    _taggingDockWidget = new QDockWidget(tr("Tags"), this);
    _taggingDockWidget->setObjectName(QStringLiteral("taggingDockWidget"));
    _taggingDockWidget->setWidget(ui->tagFrame);
    _taggingDockTitleBarWidget = _taggingDockWidget->titleBarWidget();
    sizePolicy = _taggingDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _taggingDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _taggingDockWidget, Qt::Vertical);

    _noteSearchDockWidget = new QDockWidget(tr("Note search"), this);
    _noteSearchDockWidget->setObjectName(
        QStringLiteral("noteSearchDockWidget"));
    _noteSearchDockWidget->setWidget(ui->searchLineEdit);
    _noteSearchDockTitleBarWidget = _noteSearchDockWidget->titleBarWidget();
    sizePolicy = _noteSearchDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(2);
    _noteSearchDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(Qt::LeftDockWidgetArea, _noteSearchDockWidget, Qt::Vertical);

    _noteFolderDockWidget = new QDockWidget(tr("Note folder"), this);
    _noteFolderDockWidget->setObjectName(
        QStringLiteral("noteFolderDockWidget"));

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
    _noteNavigationDockWidget->setObjectName(
        QStringLiteral("noteNavigationDockWidget"));
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

    if (!_noteEditIsCentralWidget) {
        _noteEditDockWidget = new QDockWidget(tr("Note edit"), this);
        _noteEditDockWidget->setObjectName(
            QStringLiteral("noteEditDockWidget"));
        _noteEditDockWidget->setWidget(ui->noteEditTabWidget);
        _noteEditDockTitleBarWidget = _noteEditDockWidget->titleBarWidget();
        sizePolicy = _noteEditDockWidget->sizePolicy();
        sizePolicy.setHorizontalStretch(5);
        _noteEditDockWidget->setSizePolicy(sizePolicy);
        addDockWidget(Qt::RightDockWidgetArea, _noteEditDockWidget,
                      Qt::Horizontal);
    }

    _noteTagDockWidget = new QDockWidget(tr("Note tags"), this);
    _noteTagDockWidget->setObjectName(QStringLiteral("noteTagDockWidget"));
    _noteTagDockWidget->setWidget(ui->noteTagFrame);
    _noteTagDockTitleBarWidget = _noteTagDockWidget->titleBarWidget();
    sizePolicy = _noteTagDockWidget->sizePolicy();
    sizePolicy.setHorizontalStretch(5);
    _noteTagDockWidget->setSizePolicy(sizePolicy);
    addDockWidget(_noteEditIsCentralWidget ? Qt::LeftDockWidgetArea
                                           : Qt::RightDockWidgetArea,
                  _noteTagDockWidget, Qt::Vertical);

    _notePreviewDockWidget = new QDockWidget(tr("Note preview"), this);
    _notePreviewDockWidget->setObjectName(
        QStringLiteral("notePreviewDockWidget"));
    _notePreviewDockWidget->setWidget(ui->noteViewFrame);
    _notePreviewDockTitleBarWidget = _notePreviewDockWidget->titleBarWidget();
    addDockWidget(Qt::RightDockWidgetArea, _notePreviewDockWidget,
                  Qt::Horizontal);

    _logDockWidget = new QDockWidget(tr("Log"), this);
    _logDockWidget->setObjectName(QStringLiteral("logDockWidget"));
    _logDockWidget->setWidget(LogWidget::instance());
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

    QSettings settings;

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
    setCentralWidget(_noteEditIsCentralWidget ? ui->noteEditTabWidget : Q_NULLPTR);

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
    QSettings settings;
    const bool startHidden =
        settings.value(QStringLiteral("StartHidden"), false).toBool();
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
                                            QSizePolicy::Ignored);
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
QList<QMenu *> MainWindow::menuList() {
    return ui->menuBar->findChildren<QMenu *>();
}

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

    return Q_NULLPTR;
}

/**
 * Builds the note index and loads the note directory list
 *
 * @param forceBuild
 * @param forceLoad
 * @param reloadTabs
 */
void MainWindow::buildNotesIndexAndLoadNoteDirectoryList(bool forceBuild,
                                                         bool forceLoad,
                                                         bool reloadTabs) {
    const bool wasBuilt = buildNotesIndex(0, forceBuild);

    if (wasBuilt || forceLoad) {
        loadNoteDirectoryList();
    }

    if (wasBuilt && reloadTabs) {
        // restore the note tabs
        Utils::Gui::reloadNoteTabs(ui->noteEditTabWidget);
    }
}

/**
 * Returns the global main window instance
 */
MainWindow *MainWindow::instance() {
    return qApp ? qApp->property("mainWindow").value<MainWindow *>() : nullptr;
}

/**
 * Initializes the editor soft wrapping
 */
void MainWindow::initEditorSoftWrap() {
    QSettings settings;
    const bool useSoftWrapInNoteEditor =
        settings.value(QStringLiteral("useSoftWrapInNoteEditor"), true)
            .toBool();

    QTextEdit::LineWrapMode mode =
        useSoftWrapInNoteEditor ? QTextEdit::WidgetWidth : QTextEdit::NoWrap;
    QPlainTextEdit::LineWrapMode pMode = useSoftWrapInNoteEditor
                                             ? QPlainTextEdit::WidgetWidth
                                             : QPlainTextEdit::NoWrap;

    ui->noteTextEdit->setLineWrapMode(pMode);
    ui->encryptedNoteTextEdit->setLineWrapMode(pMode);
    ui->noteTextView->setLineWrapMode(mode);
}

/**
 * Reloads all tasks from the ownCloud server
 */
void MainWindow::reloadTodoLists() {
    if (!OwnCloudService::isTodoCalendarSupportEnabled()) {
        return;
    }

    QSettings settings;
    QStringList calendars =
        settings.value(QStringLiteral("ownCloud/todoCalendarEnabledList"))
            .toStringList();
    QString serverUrl =
        CloudConnection::currentCloudConnection().getServerUrl();

    if (calendars.count() > 0 && !serverUrl.isEmpty()) {
        OwnCloudService *ownCloud = OwnCloudService::instance();

        QListIterator<QString> itr(calendars);
        while (itr.hasNext()) {
            QString calendar = itr.next();
            ownCloud->todoGetTodoList(calendar, Q_NULLPTR);
        }

        showStatusBarMessage(tr("Your tasks are being loaded from your server"),
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
    ui->menuCustom_actions->hide();
    //    _customActionToolbar->hide();

    ScriptingService *scriptingService = ScriptingService::createInstance(this);
    QQmlEngine *engine = scriptingService->engine();
    //    engine->setObjectOwnership(ui->noteTextEdit,
    //    QQmlEngine::CppOwnership);
    engine->rootContext()->setContextProperty(QStringLiteral("noteTextEdit"),
                                              ui->noteTextEdit);
    scriptingService->initComponents();
}

/**
 * Invokes the custom action in the scripting service
 *
 * @param identifier
 */
void MainWindow::onCustomActionInvoked(const QString &identifier) {
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
    _formattingToolbar->addAction(ui->actionInsert_code_block);
    _formattingToolbar->addAction(ui->actionInsert_block_quote);
    _formattingToolbar->setObjectName(QStringLiteral("formattingToolbar"));
    addToolBar(_formattingToolbar);

    _insertingToolbar = new QToolBar(tr("inserting toolbar"), this);
    _insertingToolbar->addAction(ui->actionInsert_Link_to_note);
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
    QSettings settings;
    const QStringList workspaces = getWorkspaceUuidList();
    const QString currentUuid = currentWorkspaceUuid();

    if (rebuild) {
        // we need to create a new combo box so the width gets updated in the
        // window toolbar
        initWorkspaceComboBox();

        ui->menuWorkspaces->clear();
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

        const QString name = settings
                                 .value(QStringLiteral("workspace-") + uuid +
                                        QStringLiteral("/name"))
                                 .toString();
        const QString objectName = QStringLiteral("restoreWorkspace-") + uuid;

        _workspaceComboBox->addItem(name, uuid);

        auto *action = new QAction(name, ui->menuWorkspaces);
        connect(action, &QAction::triggered, this,
                [this, uuid]() { setCurrentWorkspace(uuid); });

        // set an object name for creating shortcuts
        action->setObjectName(objectName);

        // try to load a key sequence from the settings
        QKeySequence shortcut = QKeySequence(
            settings.value(QStringLiteral("Shortcuts/MainWindow-") + objectName)
                .toString());
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
    QSettings settings;

    const auto dockWidgets = findChildren<QDockWidget *>();
    for (QDockWidget *dockWidget : dockWidgets) {
        auto *action = new QAction(this);
        const QString objectName =
            QStringLiteral("togglePanel-") + dockWidget->objectName();

        action->setText(tr("Show %1 panel").arg(dockWidget->windowTitle()));
        action->setObjectName(objectName);
        action->setCheckable(true);
        action->setChecked(!dockWidget->isHidden());

        // disable the noteSubFolderDockWidget menu entry if subfolders are
        // not enabled
        if (dockWidget->objectName() ==
                QStringLiteral("noteSubFolderDockWidget") &&
            !NoteFolder::isCurrentShowSubfolders()) {
            action->setEnabled(false);
        }

        // try to load a key sequence from the settings
        QKeySequence shortcut = QKeySequence(
            settings.value(QStringLiteral("Shortcuts/MainWindow-") + objectName)
                .toString());
        action->setShortcut(shortcut);

        // toggle the panel if the checkbox was triggered
        connect(action, &QAction::triggered, this, [this, dockWidget]() {
            togglePanelVisibility(dockWidget->objectName());
        });

        ui->menuPanels->addAction(action);
    }

    // update the preview in case it was disable previously
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
        action->setObjectName(QStringLiteral("toggleToolBar-") +
                              toolbar->objectName());
        action->setCheckable(true);
        action->setChecked(!toolbar->isHidden());

        // toggle the panel if the checkbox was triggered
        connect(action, &QAction::triggered, this, [this, toolbar]() {
            toggleToolbarVisibility(toolbar->objectName());
        });

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

    if (dockWidget == Q_NULLPTR) {
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
        // note folder has no subfolders activated
        if (newVisibility) {
            newVisibility = NoteFolder::isCurrentNoteTreeEnabled();
        }
    }

    dockWidget->setVisible(newVisibility);

    // filter notes again according to new widget state
    filterNotes();
}

/**
 * Toggles the visibility of a toolbar by object name
 *
 * @param objectName
 */
void MainWindow::toggleToolbarVisibility(const QString &objectName) {
    auto *toolbar = findChild<QToolBar *>(objectName);

    if (toolbar == Q_NULLPTR) {
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
    QSettings settings;
    return settings.value(QStringLiteral("DistractionFreeMode/isEnabled"))
        .toBool();
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

    qDebug() << __func__
             << " - 'isInDistractionFreeMode': " << isInDistractionFreeMode;

    // store the window settings before we go into distraction free mode
    if (!isInDistractionFreeMode) {
        storeSettings();
    }

    isInDistractionFreeMode = !isInDistractionFreeMode;

    // remember that we were using the distraction free mode
    settings.setValue(QStringLiteral("DistractionFreeMode/isEnabled"),
                      isInDistractionFreeMode);

    setDistractionFreeMode(isInDistractionFreeMode);
}

/**
 * Does some basic styling
 */
void MainWindow::initStyling() {
    QSettings settings;
    const bool darkMode = settings.value(QStringLiteral("darkMode")).toBool();
    QString appStyleSheet;
    QString noteTagFrameColorName;

    // turn on the dark mode if enabled
    if (darkMode) {
        QFile f(QStringLiteral(":qdarkstyle/style.qss"));
        if (!f.exists()) {
            qWarning("Unable to set stylesheet, file not found!");
        } else {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            appStyleSheet = ts.readAll();
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
    appStyleSheet +=
        QStringLiteral("QMarkdownTextEdit{color:%1;background-color:%2;}")
            .arg(fgColorName, bgColorName);

    // set the background color for the note tag frame and its children QFrames
    appStyleSheet += QStringLiteral(
                         "QFrame#noteTagFrame, QFrame#noteTagFrame QFrame "
                         "{background-color: %1;}")
                         .arg(noteTagFrameColorName);

    qApp->setStyleSheet(appStyleSheet);
    Utils::Gui::updateInterfaceFontSize();

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
    ui->noteViewFrame->setStyleSheet(QString());

    // add some margins in OS X to match the styling of the note list
    ui->navigationFrame->setContentsMargins(3, 0, 3, 0);

    // add a padding for the note tag frame so the `add tag` button doesn't
    // stick to the right corner
    ui->noteTagFrame->setStyleSheet(
        "QFrame {border: none; padding-right: 5px;}");
#endif

    // move the note view scrollbar when the note edit scrollbar was moved
    connect(ui->noteTextEdit->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(noteTextSliderValueChanged(int)));
    connect(ui->encryptedNoteTextEdit->verticalScrollBar(),
            SIGNAL(valueChanged(int)), this,
            SLOT(noteTextSliderValueChanged(int)));

    // move the note edit scrollbar when the note view scrollbar was moved
    connect(ui->noteTextView->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(noteViewSliderValueChanged(int)));

    // hide the combo box if it looses focus if it should not be viewed
    connect(ui->noteFolderComboBox, &ComboBox::focusOut, this,
            &MainWindow::hideNoteFolderComboBoxIfNeeded);
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

    const float editScrollFactor =
        static_cast<float>(value) / editScrollBar->maximum();
    const int viewPosition =
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

    const float editScrollFactor =
        static_cast<float>(value) / viewScrollBar->maximum();

    const int editPosition =
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
void MainWindow::setDistractionFreeMode(const bool enabled) {
    QSettings settings;

    if (enabled) {
        //
        // enter the distraction free mode
        //

        // store the current workspace in case we changed something
        storeCurrentWorkspace();

        const bool menuBarWasVisible =
            settings
                .value(QStringLiteral("showMenuBar"), !ui->menuBar->isHidden())
                .toBool();

        // set the menu bar visible so we get the correct height
        if (!menuBarWasVisible) {
            ui->menuBar->setVisible(true);
        }

        // remember states, geometry and sizes
        settings.setValue(QStringLiteral("DistractionFreeMode/windowState"),
                          saveState());
        settings.setValue(QStringLiteral("DistractionFreeMode/menuBarGeometry"),
                          ui->menuBar->saveGeometry());
        settings.setValue(QStringLiteral("DistractionFreeMode/menuBarHeight"),
                          ui->menuBar->height());
        settings.setValue(QStringLiteral("DistractionFreeMode/menuBarVisible"),
                          menuBarWasVisible);

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
            if (dockWidget->objectName() ==
                QStringLiteral("noteEditDockWidget")) {
                continue;
            }
            dockWidget->hide();
        }

        // hide the status bar
        //        ui->statusBar->hide();

        _leaveDistractionFreeModeButton = new QPushButton(tr("leave"));
        _leaveDistractionFreeModeButton->setFlat(true);
        _leaveDistractionFreeModeButton->setToolTip(
            tr("Leave distraction free mode"));
        _leaveDistractionFreeModeButton->setStyleSheet(
            QStringLiteral("QPushButton {padding: 0 5px}"));

        _leaveDistractionFreeModeButton->setIcon(QIcon::fromTheme(
            QStringLiteral("zoom-original"),
            QIcon(QStringLiteral(
                ":icons/breeze-qownnotes/16x16/zoom-original.svg"))));

        connect(_leaveDistractionFreeModeButton, &QPushButton::clicked, this,
                &MainWindow::toggleDistractionFreeMode);

        statusBar()->addPermanentWidget(_leaveDistractionFreeModeButton);

        ui->noteEditTabWidget->tabBar()->hide();
    } else {
        //
        // leave the distraction free mode
        //

        statusBar()->removeWidget(_leaveDistractionFreeModeButton);
        disconnect(_leaveDistractionFreeModeButton, Q_NULLPTR, Q_NULLPTR,
                   Q_NULLPTR);

        // restore states and sizes
        restoreState(
            settings.value(QStringLiteral("DistractionFreeMode/windowState"))
                .toByteArray());
        ui->menuBar->setVisible(
            settings.value(QStringLiteral("DistractionFreeMode/menuBarVisible"))
                .toBool());
        ui->menuBar->restoreGeometry(
            settings
                .value(QStringLiteral("DistractionFreeMode/menuBarGeometry"))
                .toByteArray());
        ui->menuBar->setFixedHeight(
            settings.value(QStringLiteral("DistractionFreeMode/menuBarHeight"))
                .toInt());

        if (ui->noteEditTabWidget->count() > 1) {
            ui->noteEditTabWidget->tabBar()->show();
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
void MainWindow::showStatusBarMessage(const QString &message,
                                      const int timeout) {
    if (!isInDistractionFreeMode()) {
        ui->statusBar->showMessage(message, timeout);
    }

    // write to the log widget
    emit log(LogWidget::StatusLogType, message);
}

/**
 * Sets the shortcuts for the note bookmarks up
 */
void MainWindow::setupNoteBookmarkShortcuts() {
    for (int number = 1; number <= 9; number++) {
        // setup the store shortcut
        auto *storeShortcut =
            new QShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+") +
                                       QString::number(number)),
                          this);

        connect(storeShortcut, &QShortcut::activated, this,
                [this, number]() { storeNoteBookmark(number); });

        // setup the goto shortcut
        auto *gotoShortcut = new QShortcut(
            QKeySequence(QStringLiteral("Ctrl+") + QString::number(number)),
            this);

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
    const QList<QAction *> actions =
        ui->noteFoldersMenu->findChildren<QAction *>();

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

        while ((child = _noteFolderDockWidgetFrame->layout()->takeAt(0)) !=
               nullptr) {
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
            action->setObjectName(QStringLiteral("noteFolder-") +
                                  QString::number(folderId));
            action->setIcon(_folderIcon);

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
void MainWindow::changeNoteFolder(const int noteFolderId,
                                  const bool forceChange) {
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
        return;
    }

    if (noteFolder.isCurrent() && !forceChange) {
        return;
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
        QSettings settings;
        // make the path relative to the portable data path if we are in
        // portable mode
        settings.setValue(
            QStringLiteral("notesPath"),
            Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                std::move(folderName)));

        // we have to unset the current note otherwise it might show up after
        // switching to another note folder
        unsetCurrentNote();

        buildNotesIndexAndLoadNoteDirectoryList(false, false, false);

        // update the current folder tooltip
        updateCurrentFolderTooltip();

        // restore the note tabs
        Utils::Gui::restoreNoteTabs(ui->noteEditTabWidget,
                                    ui->noteEditTabWidgetLayout);

        // restore the note history of the new note folder
        noteHistory.restoreForCurrentNoteFolder();

        // check if there is a note name set and jump to it
        QString noteName = _activeNoteFolderNotePositions[noteFolderId]
                               .getNoteName();

        // if there was no NoteHistory found try the note history
        if (noteName.isEmpty()) {
            _activeNoteFolderNotePositions[noteFolderId] =
                noteHistory.getCurrentHistoryItem();
            noteName = _activeNoteFolderNotePositions[noteFolderId].getNoteName();
        }

        if (!noteName.isEmpty()) {
            jumpToNoteName(noteName);

            // restore the current position in the note
            _activeNoteFolderNotePositions[noteFolderId]
                .restoreTextEditPosition(ui->noteTextEdit);
        }

        // commit the changes in the selected note folder to git
        gitCommitCurrentNoteFolder();
    }

    generateSystemTrayContextMenu();
    updateWindowTitle();
    _lastNoteId = 0;
}

/*
 * Adds and removes a folder from the recent note folders
 */
void MainWindow::storeRecentNoteFolder(const QString &addFolderName,
                                       const QString &removeFolderName) {
    QSettings settings;
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

    QSettings settings;

    // check if we should ignore all changes
    if (settings.value(QStringLiteral("ignoreAllExternalModifications"))
            .toBool()) {
        return NoteDiffDialog::Ignore;
    }

    // check if we should accept all changes
    if (settings.value(QStringLiteral("acceptAllExternalModifications"))
            .toBool()) {
        return NoteDiffDialog::Reload;
    }

    const QString text1 = this->ui->noteTextEdit->toPlainText();

    changedNote.updateNoteTextFromDisk();
    const QString text2 = changedNote.getNoteText();

    //    qDebug() << __func__ << " - 'text1': " << text1;
    //    qDebug() << __func__ << " - 'text2': " << text2;

    diff_match_patch *diff = new diff_match_patch();
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
    trayIcon->setIcon(getSystemTrayIcon());

    connect(trayIcon, &QSystemTrayIcon::activated, this,
            &MainWindow::systemTrayIconClicked);

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
    const QSettings settings;
    const bool darkModeIcon =
        settings.value(QStringLiteral("darkModeTrayIcon"), false).toBool();
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

    const QSignalBlocker blocker(ui->noteTextEdit);
    Q_UNUSED(blocker)

    const QSignalBlocker blocker2(ui->noteTreeWidget);
    Q_UNUSED(blocker2)

    const bool isCurrentNoteTreeEnabled =
        NoteFolder::isCurrentNoteTreeEnabled();
    ui->noteTreeWidget->clear();
    //    ui->noteTreeWidget->setRootIsDecorated(isCurrentNoteTreeEnabled);
    int itemCount;

    if (isCurrentNoteTreeEnabled) {
        auto *noteFolderItem = new QTreeWidgetItem();
        noteFolderItem->setText(0, tr("Note folder"));
        noteFolderItem->setData(0, Qt::UserRole, 0);
        noteFolderItem->setData(0, Qt::UserRole + 1, FolderType);
        noteFolderItem->setIcon(0, _folderIcon);
        noteFolderItem->setForeground(1, QColor(Qt::gray));
        ui->noteTreeWidget->addTopLevelItem(noteFolderItem);

        buildNoteSubFolderTreeForParentItem(noteFolderItem);
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
        QStringLiteral("note list loaded"),
        QString::number(itemCount) + QStringLiteral(" notes"), itemCount);

    // sort alphabetically again if necessary
    QSettings settings;
    if (settings.value(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE)
            .toInt() == SORT_ALPHABETICAL) {
        ui->noteTreeWidget->sortItems(
            0, toQtOrder(
                   settings.value(QStringLiteral("notesPanelOrder")).toInt()));
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
    if (firstVisibleNoteTreeWidgetItem() == Q_NULLPTR) {
        unsetCurrentNote();
    } else {
        const auto item = findNoteInNoteTreeWidget(currentNote);

        // in the end we need to set the current item again if we can find it
        if (item != Q_NULLPTR) {
            ui->noteTreeWidget->setCurrentItem(item);
        }
    }
}

/**
 * Adds a note to the note tree widget
 */
bool MainWindow::addNoteToNoteTreeWidget(const Note &note,
                                         QTreeWidgetItem *parent) {
    const QString name = note.getName();

    // skip notes without name
    if (name.isEmpty()) {
        return false;
    }

    const bool isNoteListPreview = Utils::Misc::isNoteListPreview();

    // add a note item to the tree
    auto *noteItem = new QTreeWidgetItem();
    setTreeWidgetItemToolTipForNote(noteItem, note);
    noteItem->setText(0, name);
    noteItem->setData(0, Qt::UserRole, note.getId());
    noteItem->setData(0, Qt::UserRole + 1, NoteType);
    noteItem->setIcon(0, _noteIcon);

    const Tag tag = Tag::fetchOneOfNoteWithColor(note);
    if (tag.isFetched()) {
        // set the color of the note tree widget item
        handleTreeWidgetItemTagColor(noteItem, tag);
    }

    const bool isEditable = Note::allowDifferentFileName();
    if (isEditable) {
        noteItem->setFlags(noteItem->flags() | Qt::ItemIsEditable);
    }

    const QSignalBlocker blocker(ui->noteTreeWidget);
    Q_UNUSED(blocker)

    if (parent == nullptr) {
        // strange things happen if we insert with insertTopLevelItem
        ui->noteTreeWidget->addTopLevelItem(noteItem);
    } else {
        parent->addChild(noteItem);
    }

    if (isNoteListPreview) {
        updateNoteTreeWidgetItem(note, noteItem);
    }

    //    QSettings settings;
    //    if (settings.value("notesPanelSort", SORT_BY_LAST_CHANGE).toInt() ==
    //    SORT_ALPHABETICAL) {
    //        ui->noteTreeWidget->addTopLevelItem(noteItem);
    //    } else {
    //        ui->noteTreeWidget->insertTopLevelItem(0, noteItem);
    //    }

    return true;
}

void MainWindow::updateNoteTreeWidgetItem(const Note &note,
                                          QTreeWidgetItem *noteItem) {
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
 * Adds a note sub folder to the note sub folder tree widget
 */
QTreeWidgetItem *MainWindow::addNoteSubFolderToTreeWidget(
    QTreeWidgetItem *parentItem, const NoteSubFolder &noteSubFolder) {
    const int id = noteSubFolder.getId();
    const QString name = noteSubFolder.getName();
    QSettings settings;
    const int linkCount = Note::countByNoteSubFolderId(
        id,
        settings
            .value(QStringLiteral("noteSubfoldersPanelShowNotesRecursively"))
            .toBool());
    QString toolTip = tr("show notes in folder '%1' (%2)")
                          .arg(name, QString::number(linkCount));

    auto *item = new QTreeWidgetItem();
    item->setText(0, name);
    item->setData(0, Qt::UserRole, id);
    item->setData(0, Qt::UserRole + 1, FolderType);
    item->setToolTip(0, toolTip);
    item->setIcon(0, _folderIcon);
    item->setForeground(1, QColor(Qt::gray));
    item->setText(1, QString::number(linkCount));
    item->setToolTip(1, toolTip);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    if (parentItem == nullptr) {
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
    QTreeWidgetItem *item, const Note &note,
    QDateTime *overrideFileLastModified) {
    if (item == nullptr) {
        return;
    }

    QDateTime modified = note.getFileLastModified();
    QDateTime *fileLastModified = (overrideFileLastModified != nullptr)
                                      ? overrideFileLastModified
                                      : &modified;

    QString toolTipText =
        tr("<strong>%1</strong><br />last modified: %2")
            .arg(note.getFileName(), fileLastModified->toString());

    NoteSubFolder noteSubFolder = note.getNoteSubFolder();
    if (noteSubFolder.isFetched()) {
        toolTipText += tr("<br />path: %1").arg(noteSubFolder.relativePath());
    }

    item->setToolTip(0, toolTipText);

    // TODO: handle item widget too
}

/**
 * @brief makes the current note the first item in the note list without
 * reloading the whole list
 */
void MainWindow::makeCurrentNoteFirstInNoteList() {
    QTreeWidgetItem *item = findNoteInNoteTreeWidget(currentNote);

    if (item != Q_NULLPTR) {
        const QSignalBlocker blocker(ui->noteTreeWidget);
        Q_UNUSED(blocker)

        ui->noteTreeWidget->takeTopLevelItem(
            ui->noteTreeWidget->indexOfTopLevelItem(item));
        ui->noteTreeWidget->insertTopLevelItem(0, item);

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

    return Q_NULLPTR;
}

void MainWindow::readSettings() {
    NoteFolder::migrateToNoteFolders();

    QSettings settings;
    showSystemTray =
        settings.value(QStringLiteral("ShowSystemTray"), false).toBool();
    restoreGeometry(
        settings.value(QStringLiteral("MainWindow/geometry")).toByteArray());
    ui->menuBar->restoreGeometry(
        settings.value(QStringLiteral("MainWindow/menuBarGeometry"))
            .toByteArray());

    // read all relevant settings, that can be set in the settings dialog
    readSettingsFromSettingsDialog(true);

    // get the notes path
    // prepend the portable data path if we are in portable mode
    this->notesPath = Utils::Misc::prependPortableDataPathIfNeeded(
        settings.value(QStringLiteral("notesPath")).toString());

    // migration: remove GAnalytics-cid
    if (!settings.value(QStringLiteral("GAnalytics-cid"))
             .toString()
             .isEmpty()) {
        settings.remove(QStringLiteral("GAnalytics-cid"));
    }

    // let us select a folder if we haven't find one in the settings
    if (this->notesPath.isEmpty()) {
        selectOwnCloudNotesFolder();
    }

    // migration: remove notes path from recent note folders
    if (!this->notesPath.isEmpty()) {
        QStringList recentNoteFolders =
            settings.value(QStringLiteral("recentNoteFolders")).toStringList();
        if (recentNoteFolders.contains(this->notesPath)) {
            recentNoteFolders.removeAll(this->notesPath);
            settings.setValue(QStringLiteral("recentNoteFolders"),
                              recentNoteFolders);
        }
    }

    // set the editor width selector for the distraction free mode
    const int editorWidthMode =
        settings.value(QStringLiteral("DistractionFreeMode/editorWidthMode"))
            .toInt();

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
    const bool showStatusBar =
        settings.value(QStringLiteral("showStatusBar"), true).toBool();
    on_actionShow_status_bar_triggered(showStatusBar);

    // toggle the show menu bar checkbox
    // use the current menu bar visibility as default (so it will not be
    // shown by default on Unity desktop)
    const bool showMenuBar =
        settings.value(QStringLiteral("showMenuBar"), !ui->menuBar->isHidden())
            .toBool();
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

    // we want to trigger the event afterwards so the settings of the note edits
    // are updated
    const bool centerCursor =
        settings.value(QStringLiteral("Editor/centerCursor")).toBool();
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
    _languageGroup = new QActionGroup(ui->menuLanguages);
    loadDictionaryNames();
}

/**
 * Restores the toolbars
 */
void MainWindow::restoreToolbars() {
    QSettings settings;
    QList<ToolbarContainer> toolbarContainers;
    const int toolbarCount = settings.beginReadArray(QStringLiteral("toolbar"));

    for (int i = 0; i < toolbarCount; i++) {
        settings.setArrayIndex(i);

        ToolbarContainer toolbarContainer;

        toolbarContainer.name =
            settings.value(QStringLiteral("name")).toString();
        if (toolbarContainer.name.isEmpty()) {
            qWarning() << tr("Toolbar could not be loaded without name");
            continue;
        }

        toolbarContainer.title =
            settings.value(QStringLiteral("title")).toString();
        toolbarContainer.actions =
            settings.value(QStringLiteral("items")).toStringList();

        toolbarContainers.push_back(toolbarContainer);
    }

    settings.endArray();

    if (!toolbarContainers.empty()) {
        // delete the custom toolbars
        const auto toolbars = findChildren<QToolBar *>();
        for (QToolBar *toolbar : toolbars) {
            if (!toolbar->objectName().startsWith(
                    Toolbar_Editor::customToolbarNamePrefix)) {
                continue;
            }

            delete toolbar;
        }

        for (ToolbarContainer toolbarContainer :
             Utils::asConst(toolbarContainers)) {
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
void MainWindow::readSettingsFromSettingsDialog(const bool isAppLaunch) {
    QSettings settings;

    this->notifyAllExternalModifications =
        settings.value(QStringLiteral("notifyAllExternalModifications"))
            .toBool();
    this->noteSaveIntervalTime =
        settings.value(QStringLiteral("noteSaveIntervalTime"), 10).toInt();

    // default value is 10 seconds
    if (this->noteSaveIntervalTime == 0) {
        this->noteSaveIntervalTime = 10;
        settings.setValue(QStringLiteral("noteSaveIntervalTime"),
                          this->noteSaveIntervalTime);
    }

    // set the git commit interval time
    _gitCommitInterval =
        settings.value(QStringLiteral("gitCommitInterval"), 30).toInt();

    // load note text view font
    QString fontString = Utils::Misc::previewFontString();

    // store the current font if there isn't any set yet
    if (fontString.isEmpty()) {
        fontString = ui->noteTextView->font().toString();
        settings.setValue(QStringLiteral("MainWindow/noteTextView.font"),
                          fontString);
    }

    // set the note text view font
    QFont font;
    font.fromString(fontString);
    ui->noteTextView->setFont(font);

    // set the main toolbar icon size
    int toolBarIconSize =
        settings.value(QStringLiteral("MainWindow/mainToolBar.iconSize"))
            .toInt();
    if (toolBarIconSize == 0) {
        toolBarIconSize = ui->mainToolBar->iconSize().height();
        settings.setValue(QStringLiteral("MainWindow/mainToolBar.iconSize"),
                          toolBarIconSize);
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
    QString fileName = settings.value(QStringLiteral("darkModeColors")).toBool()
                           ? QStringLiteral("search-notes-dark.svg")
                           : QStringLiteral("search-notes.svg");
    QString styleSheet = ui->searchLineEdit->styleSheet();
    styleSheet.replace(
        QRegularExpression(QStringLiteral("background-image: url\\(:.+\\);")),
        QStringLiteral("background-image: url(:/images/%1);").arg(fileName));
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

    // init the saved searches completer
    initSavedSearchesCompleter();

    // show or hide the note git version menu entry
    ui->actionShow_note_git_versions->setVisible(Utils::Git::hasLogCommand());

    // show or hide 'Find or create ...' search in Note Subfolders & Tags Panels
    ui->noteSubFolderLineEdit->setHidden(
        settings.value(QStringLiteral("noteSubfoldersPanelHideSearch"))
            .toBool());
    ui->tagLineEdit->setHidden(
        settings.value(QStringLiteral("tagsPanelHideSearch")).toBool());
    ui->navigationLineEdit->setHidden(
        settings.value(QStringLiteral("navigationPanelHideSearch")).toBool());

    // set the cursor width of the note text-edits
    int cursorWidth = settings.value(QStringLiteral("cursorWidth"), 1).toInt();
    ui->noteTextEdit->setCursorWidth(cursorWidth);
    ui->encryptedNoteTextEdit->setCursorWidth(cursorWidth);

    ui->noteTextEdit->setPaperMargins();
    ui->encryptedNoteTextEdit->setPaperMargins();

    if (_webSocketServerService == Q_NULLPTR) {
        QTimer::singleShot(250, this, SLOT(initWebSocketServerService()));
    } else if (Utils::Misc::isSocketServerEnabled()) {
        if (_webSocketServerService->getPort() !=
            WebSocketServerService::getSettingsPort()) {
            _webSocketServerService->listen();
        }
    } else {
        _webSocketServerService->close();
    }

    if (settings.value(QStringLiteral("Editor/disableCursorBlinking"))
            .toBool()) {
        qApp->setCursorFlashTime(0);
    }

    // reset cloud service instance
    OwnCloudService::instance(true);

    // the notes need to be reloaded and subfolder panel needs to be populated
    // if subfolders were activated for a note folder in the settings
    if (!isAppLaunch && NoteFolder::isCurrentShowSubfolders()) {
        buildNotesIndexAndLoadNoteDirectoryList();
    }

    initGlobalKeyboardShortcuts();
}

/**
 * Initializes the item height of the tree widgets
 */
void MainWindow::initTreeWidgetItemHeight() {
    QSettings settings;
    int height = settings.value(QStringLiteral("itemHeight")).toInt();

    // if the height was 0 set it the the current height of a tree widget item
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
        QStringLiteral("\nQTreeWidget::item \\{height: \\d+px\\}"),
        QRegularExpression::CaseInsensitiveOption));

    // add the new height stylesheet
    styleText += QStringLiteral("\nQTreeWidget::item {height: %1px}")
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
        Q_UNUSED(blocker)
        this->setNoteTextFromNote(&note);
    }

    ScriptingService::instance()->onCurrentNoteChanged(&currentNote);
}

void MainWindow::notesWereModified(const QString &str) {
    // workaround when signal block doesn't work correctly
    if (_isNotesWereModifiedDisabled) {
        return;
    }

    // if we should ignore all changes return here
    if (QSettings().value(QStringLiteral("ignoreAllExternalNoteFolderChanges"))
        .toBool()) {
        return;
    }

    qDebug() << "notesWereModified: " << str;

    QFileInfo fi(str);
    Note note = Note::fetchByFileName(fi.fileName());

    // load note from disk if current note was changed
    if (note.getFileName() == this->currentNote.getFileName()) {
        if (note.fileExists()) {
            // fetch text of note from disk
            note.updateNoteTextFromDisk();
            const QString noteTextOnDisk =
                Utils::Misc::transformLineFeeds(note.getNoteText());

            const QString noteTextOnDiskHash =
                QString(QCryptographicHash::hash(noteTextOnDisk.toLocal8Bit(),
                                                 QCryptographicHash::Sha1)
                            .toHex());

            // skip dialog if text of note file on disk and current note are
            // equal
            if (noteTextOnDiskHash == _currentNoteTextHash) {
                return;
            }

            // fetch current text
            const QString noteTextEditText =
                this->ui->noteTextEdit->toPlainText();

            // skip dialog if text of note file on disk text from note text
            // edit are equal
            if (noteTextEditText == noteTextOnDisk) {
                return;
            }

            showStatusBarMessage(tr("Current note was modified externally"),
                                 5000);

            // if we don't want to get notifications at all
            // external modifications check if we really need one
            if (!this->notifyAllExternalModifications) {
                bool isCurrentNoteNotEditedForAWhile =
                    this->currentNoteLastEdited.addSecs(60) <
                    QDateTime::currentDateTime();

                // reloading the current note text straight away
                // if we didn't change it for a minute
                if (!this->currentNote.getHasDirtyData() &&
                    isCurrentNoteNotEditedForAWhile) {
                    updateNoteTextFromDisk(std::move(note));
                    return;
                }
            }

            const int result = openNoteDiffDialog(note);
            switch (result) {
                // overwrite file with local changes
                case NoteDiffDialog::Overwrite: {
                    const QSignalBlocker blocker(this->noteDirectoryWatcher);
                    Q_UNUSED(blocker)

                    showStatusBarMessage(
                        tr("Overwriting external changes of: %1")
                            .arg(currentNote.getFileName()),
                        3000);

                    // the note text has to be stored newly because the
                    // external change is already in the note table entry
                    currentNote.storeNewText(ui->noteTextEdit->toPlainText());
                    currentNote.storeNoteTextFileToDisk();

                    // just to make sure everything is up-to-date
                    //                        this->currentNote = note;
                    //                        this->setNoteTextFromNote( &note,
                    //                        true );

                    // wait 100ms before the block on this->noteDirectoryWatcher
                    // is opened, otherwise we get the event
                    Utils::Misc::waitMsecs(100);
                } break;

                // reload note file from disk
                case NoteDiffDialog::Reload:
                    showStatusBarMessage(tr("Loading external changes from: %1")
                                             .arg(currentNote.getFileName()),
                                         3000);
                    updateNoteTextFromDisk(note);
                    break;

                    //                case NoteDiffDialog::Cancel:
                    //                case NoteDiffDialog::Ignore:
                default:
                    // do nothing
                    break;
            }
        } else if (_noteExternallyRemovedCheckEnabled &&
                   (currentNote.getNoteSubFolderId() == 0)) {
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
                showStatusBarMessage(
                    noteWasStored
                        ? tr("Stored current note to disk")
                        : tr("Current note could not be stored to disk"),
                    3000);

                // rebuild and reload the notes directory list
                buildNotesIndexAndLoadNoteDirectoryList();

                // fetch note new (because all the IDs have changed
                // after the buildNotesIndex()
                note.refetch();

                // restore old selected row (but don't update the note text)
                setCurrentNote(note, false);
            }
        }
    } else {
        qDebug() << "other note was changed: " << str;

        showStatusBarMessage(tr("Note was modified externally: %1").arg(str),
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
    if (QSettings().value(QStringLiteral("ignoreAllExternalNoteFolderChanges"))
            .toBool()) {
        return;
    }

    qDebug() << "notesDirectoryWasModified: " << str;
    showStatusBarMessage(tr("Notes directory was modified externally"), 5000);

    // rebuild and reload the notes directory list
    buildNotesIndexAndLoadNoteDirectoryList();

    // check if the current note was modified
    // this fixes not detected external note changes of the current note if the
    // event for the change in the current note comes after the event that the
    // note folder was modified
    QString noteFileName = currentNote.getFileName();
    if (!noteFileName.isEmpty()) {
        notesWereModified(currentNote.getFileName());
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

void MainWindow::storeUpdatedNotesToDisk() {
    const QSignalBlocker blocker(noteDirectoryWatcher);
    Q_UNUSED(blocker)

    const QString oldNoteName = currentNote.getName();

    // For some reason this->noteDirectoryWatcher gets an event from this.
    // I didn't find another solution than to wait yet.
    // All flushing and syncing didn't help.
    bool currentNoteChanged = false;
    bool noteWasRenamed = false;

    // currentNote will be set by this method if the filename has changed
    const int count = Note::storeDirtyNotesToDisk(
        currentNote, &currentNoteChanged, &noteWasRenamed);

    if (count > 0) {
        _noteViewNeedsUpdate = true;

        MetricsService::instance()->sendEventIfEnabled(
            QStringLiteral("note/notes/stored"), QStringLiteral("note"),
            QStringLiteral("notes stored"),
            QString::number(count) + QStringLiteral(" notes"), count);

        qDebug() << __func__ << " - 'count': " << count;

        showStatusBarMessage(tr("Stored %n note(s) to disk", "", count), 3000);

        // wait 100ms before the block on this->noteDirectoryWatcher
        // is opened, otherwise we get the event
        Utils::Misc::waitMsecs(100);

        if (currentNoteChanged) {
            // strip trailing spaces of the current note (if enabled)
            if (QSettings().value(QStringLiteral("Editor/removeTrainingSpaces"))
                    .toBool()) {
                const bool wasStripped = currentNote.stripTrailingSpaces(
                    activeNoteTextEdit()->textCursor().position());

                if (wasStripped) {
                    qDebug() << __func__ << " - 'wasStripped'";

                    // updating the current note text is disabled because it
                    // moves the cursor to the top
//                    const QSignalBlocker blocker2(activeNoteTextEdit());
//                    Q_UNUSED(blocker2)
//                    setNoteTextFromNote(&currentNote);
                }
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
    QDateTime lastUpdateCheck =
        settings.value(QStringLiteral("LastUpdateCheck")).toDateTime();
    if (!lastUpdateCheck.isValid()) {
        // set the LastUpdateCheck if it wasn't set
        settings.setValue(QStringLiteral("LastUpdateCheck"),
                          QDateTime::currentDateTime());
    } else if (lastUpdateCheck.addSecs(3600) <= QDateTime::currentDateTime()) {
        // check for updates every 1h
        updateService->checkForUpdates(this, UpdateService::Periodic);

        // expire trashed items
        TrashItem::expireItems();
    }
}

/**
 * Does the setup the status bar widgets
 */
void MainWindow::setupStatusBarWidgets() {
    /*
     * setup of readonly button
     */
    _readOnlyButton->setText(tr("Read-only"));
    _readOnlyButton->setToolTip(
        tr("Note editing is disabled, click to "
           "enable"));
    _readOnlyButton->setStyleSheet(
        QStringLiteral("QPushButton {padding: 0 5px}"));
    _readOnlyButton->setFlat(true);
    _readOnlyButton->setHidden(Utils::Misc::isNoteEditingAllowed());

    connect(_readOnlyButton, &QPushButton::pressed, this,
            &MainWindow::allowNoteEditing);

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
    _updateAvailableButton->setStyleSheet(
        QStringLiteral("QPushButton {padding: 0 5px}"));

    connect(_updateAvailableButton, &QPushButton::pressed, this,
            &MainWindow::on_actionCheck_for_updates_triggered);

    ui->statusBar->addPermanentWidget(_updateAvailableButton);
}

void MainWindow::showUpdateAvailableButton(const QString &version) {
    _updateAvailableButton->setText(
        tr("new version %1 available").arg(version));
    _updateAvailableButton->show();
}

void MainWindow::hideUpdateAvailableButton() { _updateAvailableButton->hide(); }

/**
 * Builds the index of notes and note sub folders
 */
bool MainWindow::buildNotesIndex(int noteSubFolderId, bool forceRebuild) {
    QString notePath =
        Utils::Misc::removeIfEndsWith(this->notesPath, QDir::separator());
    NoteSubFolder noteSubFolder;
    bool hasNoteSubFolder = false;
    bool wasModified = false;

    if (noteSubFolderId == 0) {
        qDebug() << __func__ << " - 'noteSubFolderId': " << noteSubFolderId;

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
    QStringList filters{"*.txt", "*.md"};

    // append the custom extensions
    filters.append(Note::customNoteFileExtensionList(QStringLiteral("*.")));

    // show newest entry first
    QStringList files = notesDir.entryList(filters, QDir::Files, QDir::Time);
    //    qDebug() << __func__ << " - 'files': " << files;

    bool createDemoNotes = (files.count() == 0) && !hasNoteSubFolder;

    if (createDemoNotes) {
        QSettings settings;
        // check if we already have created the demo notes once
        createDemoNotes =
            !settings.value(QStringLiteral("demoNotesCreated")).toBool();

        if (createDemoNotes) {
            // we don't want to create the demo notes again
            settings.setValue(QStringLiteral("demoNotesCreated"), true);
        }
    }

    // add some notes if there aren't any and
    // we haven't already created them once
    if (createDemoNotes) {
        qDebug() << "No notes! We will add some...";
        const QStringList filenames =
            QStringList({"Markdown Cheatsheet.md", "Welcome to QOwnNotes.md"});

        // copy note files to the notes path
        for (int i = 0; i < filenames.size(); ++i) {
            const QString &filename = filenames.at(i);
            const QString destinationFile =
                this->notesPath + QDir::separator() + filename;
            QFile sourceFile(QStringLiteral(":/demonotes/") + filename);
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

    const bool withNoteNameHook =
        ScriptingService::instance()->handleNoteNameHookExists();
    const int numFiles = files.count();
    QProgressDialog progress(tr("Loading notes…"), tr("Abort"), 0, numFiles,
                             this);
    progress.setWindowModality(Qt::WindowModal);
    int currentCount = 0;

    _buildNotesIndexAfterNoteIdList.reserve(files.size());
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

        // update or create a note from the file
        const Note note =
            Note::updateOrCreateFromFile(file, noteSubFolder, withNoteNameHook);

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
        const QStringList folders =
            notesDir.entryList(QDir::Dirs | QDir::Hidden, QDir::Time);

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
                _buildNotesIndexAfterNoteSubFolderIdList
                    << parentNoteSubFolder.getId();

                // build the notes index for the note subfolder
                const bool result =
                    buildNotesIndex(parentNoteSubFolder.getId());
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
        const QList<int> removedNoteIdList =
            _buildNotesIndexBeforeNoteIdList.toList()
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
                          _buildNotesIndexAfterNoteSubFolderIdList.end())).values();
#else
        const QList<int> removedNoteSubFolderIdList =
            _buildNotesIndexBeforeNoteSubFolderIdList.toList()
                .toSet()
                .subtract(_buildNotesIndexAfterNoteSubFolderIdList.toSet())
                .toList();
#endif

        // remove all missing note subfolders
        for (const int _noteSubFolderId : removedNoteSubFolderIdList) {
            NoteSubFolder _noteSubFolder =
                NoteSubFolder::fetch(_noteSubFolderId);
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

    // we try to fix problems with the blocker
    directoryWatcherWorkaround(true);

    while (it.hasNext()) {
        const QString &file = it.next();
        qDebug() << "Found conflicting note folder database: " << file;

        // check if conflicted database copy is the same as the current note
        // folder database
        if (Utils::Misc::isSameFile(
                file, DatabaseService::getNoteFolderDatabasePath())) {
            showStatusBarMessage(
                QFile::remove(file)
                    ? tr("Removed duplicate conflicted database: %1").arg(file)
                    : tr("Could not remove duplicate conflicted database: %1")
                          .arg(file),
                4000);
        } else if (DatabaseService::mergeNoteFolderDatabase(file)) {
            showStatusBarMessage(
                QFile::remove(file)
                    ? tr("Removed merged conflicted database: %1").arg(file)
                    : tr("Could not remove merged conflicted database: %1")
                          .arg(file),
                4000);
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
                QStringLiteral("<br /><br />") +
                files.join(QStringLiteral("<br />")),
            QStringLiteral("delete-conflicted-database-files")) !=
        QMessageBox::Yes) {
        return;
    }

    count = 0;

    // remove the database files
    for (const QString &file : Utils::asConst(files)) {
        if (QFile::remove(file)) {
            ++count;
        }
    }

    showStatusBarMessage(
        tr("Removed %n conflicted database copies", "", count));

    directoryWatcherWorkaround(false);
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

    const QString notePath =
        Utils::Misc::removeIfEndsWith(this->notesPath, QDir::separator());

    const QDir notesDir(notePath);

    if (notesDir.exists()) {
        // watch the notes directory for changes
        noteDirectoryWatcher.addPath(notePath);
    }

    if (hasSubfolders) {
        const QVector<NoteSubFolder> noteSubFolderList =
            NoteSubFolder::fetchAll();
        for (const NoteSubFolder &noteSubFolder : noteSubFolderList) {
            const QString path =
                notePath + QDir::separator() + noteSubFolder.relativePath();

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
    const QStringList fileList =
        noteDirectoryWatcher.directories() + noteDirectoryWatcher.files();
    if (fileList.count() > 0) {
        noteDirectoryWatcher.removePaths(fileList);
    }
}

/**
 * Jumps to the welcome note in the note selector
 */
void MainWindow::jumpToWelcomeNote() {
    jumpToNoteName(QStringLiteral("Welcome to QOwnNotes"));
}

/**
 * Jumps to a note in the note selector
 */
bool MainWindow::jumpToNoteName(const QString &name) {
    // search for the note
    QList<QTreeWidgetItem *> items = ui->noteTreeWidget->findItems(
        name, Qt::MatchExactly | Qt::MatchRecursive, 0);

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
    const bool isCurrentNoteTreeEnabled =
        NoteFolder::isCurrentNoteTreeEnabled();

    for (QTreeWidgetItem *item : items) {
        if (isCurrentNoteTreeEnabled) {
            QString pathData = historyItem.getNoteSubFolderPathData();
            auto noteSubFolder =
                NoteSubFolder::fetchByPathData(std::move(pathData));
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
    QTreeWidgetItem *item = Utils::Gui::getTreeWidgetItemWithUserData(
        ui->noteSubFolderTreeWidget, noteSubFolderId);

    if (item != nullptr) {
        ui->noteSubFolderTreeWidget->setCurrentItem(item);

        return true;
    }

    return false;
}

QString MainWindow::selectOwnCloudNotesFolder() {
    QString path = this->notesPath;

    if (path.isEmpty()) {
        path = Utils::Misc::defaultNotesPath();
    }

    // TODO(pbek): We sometimes seem to get a "QCoreApplication::postEvent:
    // Unexpected null receiver" here.
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("Please select the folder where your notes will get stored"),
        path, QFileDialog::ShowDirsOnly);

    QDir d = QDir(dir);

    if (d.exists() && (!dir.isEmpty())) {
        // let's remove trailing slashes
        dir = d.path();

        // update the recent note folder list
        storeRecentNoteFolder(this->notesPath, dir);

        this->notesPath = dir;
        QSettings settings;
        // make the path relative to the portable data path if we are in
        // portable mode
        settings.setValue(
            QStringLiteral("notesPath"),
            Utils::Misc::makePathRelativeToPortableDataPathIfNeeded(
                std::move(dir)));

        // update the current folder tooltip
        updateCurrentFolderTooltip();
    } else {
        if (this->notesPath.isEmpty()) {
            switch (QMessageBox::information(
                this, tr("No folder was selected"),
                Utils::Misc::replaceOwnCloudText(
                    tr("You have to select your ownCloud notes "
                       "folder to make this software work!")),
                tr("&Retry"), tr("&Exit"), QString(), 0, 1)) {
                case 0:
                    selectOwnCloudNotesFolder();
                    break;
                case 1:
                default:
                    // No other way to quit the application worked
                    // in the constructor
                    // Waqar144: this doesn't seem very wise...
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
void MainWindow::reloadCurrentNoteByNoteId() {
    // get current cursor position
    auto cursor = activeNoteTextEdit()->textCursor();
    const int pos = cursor.position();

    // update the current note
    currentNote = Note::fetch(currentNote.getId());
    setCurrentNote(std::move(currentNote), false);

    // restore old cursor position
    cursor.setPosition(pos);
    activeNoteTextEdit()->setTextCursor(cursor);
}

void MainWindow::setCurrentNote(Note note, bool updateNoteText,
                                bool updateSelectedNote,
                                bool addNoteToHistory) {
    qDebug() << __func__ << " - 'note': " << note
             << " - 'updateNoteText': " << updateNoteText
             << " - 'updateSelectedNote': " << updateSelectedNote;

    MetricsService::instance()->sendVisitIfEnabled(
        QStringLiteral("note/current-note/changed"));

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
        this->noteHistory.updateCursorPositionOfNote(this->currentNote,
                                                     ui->noteTextEdit);
    }

    this->_lastNoteId = this->currentNote.getId();
    this->currentNote = note;

    // for places we can't get the current note id, like the markdown
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
        QList<QTreeWidgetItem *> items =
            ui->noteTreeWidget->findItems(name, Qt::MatchExactly);
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

        this->setNoteTextFromNote(&note);

        // hide the encrypted note text edit by default and show the regular one
        ui->encryptedNoteTextEdit->hide();
        ui->noteTextEdit->show();
    }

    updateEncryptNoteButtons();
    // we also need to do this in on_noteTreeWidget_itemSelectionChanged
    // because of different timings
    reloadCurrentNoteTags();
    updateNoteTextEditReadOnly();

    ScriptingService::instance()->onCurrentNoteChanged(&currentNote);

    // update the share button
    updateShareButton();

    // call a script hook that a new note was opened
    ScriptingService::instance()->callHandleNoteOpenedHook(&currentNote);

    //    putenv(QString("QOWNNOTES_CURRENT_NOTE_PATH=" + currentNote
    //            .fullNoteFilePath()).toLatin1().data());
    //    setenv("QOWNNOTES_CURRENT_NOTE_PATH",
    //           currentNote.fullNoteFilePath().toLatin1().data(),
    //           1);

    QSettings settings;

#ifdef Q_OS_MAC
    const bool restoreCursorPositionDefault = false;
#else
    const bool restoreCursorPositionDefault = true;
#endif

    const bool restoreCursorPosition =
        settings
            .value(QStringLiteral("restoreCursorPosition"),
                   restoreCursorPositionDefault)
            .toBool();

    // restore the last position in the note text edit
    if (restoreCursorPosition) {
        noteHistory.getLastItemOfNote(note).restoreTextEditPosition(
            ui->noteTextEdit);
    }

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
}

void MainWindow::updateCurrentTabData(const Note &note) const {
    Utils::Gui::updateTabWidgetTabData(ui->noteEditTabWidget,
                                       ui->noteEditTabWidget->currentIndex(),
                                       note);
}

void MainWindow::closeOrphanedTabs() const {
    const int maxIndex = ui->noteEditTabWidget->count() - 1;

    for (int i = maxIndex; i >= 0; i--) {
        const int noteId = Utils::Gui::getTabWidgetNoteId(
            ui->noteEditTabWidget, i);

        if (!Note::noteIdExists(noteId)) {
            removeNoteTab(i);
        }
    }
}

bool MainWindow::jumpToTab(const Note &note) const {
    const int noteId = note.getId();
    const int tabIndexOfNote = Utils::Gui::getTabWidgetIndexByProperty(
        ui->noteEditTabWidget, QStringLiteral("note-id"), noteId);

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
        QCryptographicHash::hash(currentNote.getNoteText().toLocal8Bit(),
                                 QCryptographicHash::Sha1)
            .toHex());
}

/**
 * Sets the note text edit to readonly if the note does not exist or the
 * note file is not writable or the note is encrypted
 */
void MainWindow::updateNoteTextEditReadOnly() {
    setNoteTextEditReadOnly(!(currentNote.exists() &&
                              currentNote.fileWriteable() &&
                              Utils::Misc::isNoteEditingAllowed()));

    if (ui->noteTextEdit->isVisible() && currentNote.hasEncryptedNoteText()) {
        ui->noteTextEdit->setReadOnly(true);
    }

    ui->noteTextEdit->setTextInteractionFlags(
        ui->noteTextEdit->textInteractionFlags() |
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

    ui->noteTextEdit->setTextInteractionFlags(
        ui->noteTextEdit->textInteractionFlags() |
        Qt::TextSelectableByKeyboard);
    ui->encryptedNoteTextEdit->setTextInteractionFlags(
        ui->encryptedNoteTextEdit->textInteractionFlags() |
        Qt::TextSelectableByKeyboard);
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
    QString title = currentNote.exists() ?
        currentNote.getName() : QStringLiteral("#");

    if (NoteFolder::countAll() > 0) {
        title += QStringLiteral(" - %1").arg(
            NoteFolder::currentNoteFolder().getName());
    }

    if (!session.isEmpty()) {
        title += QStringLiteral(" - %1").arg(session);
    }

    title += QStringLiteral(" - QOwnNotes");

    if (Utils::Misc::isInPortableMode()) {
        title += QStringLiteral(" Portable");
    }

    title += QStringLiteral(" - %3").arg(QStringLiteral(VERSION));
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

    if (Utils::Gui::question(this, tr("Remove current note"),
                             tr("Remove current note: <strong>%1</strong>?")
                                 .arg(this->currentNote.getName()),
                             QStringLiteral("remove-note")) ==
        QMessageBox::Yes) {
        const QSignalBlocker blocker2(ui->noteTextEdit);
        Q_UNUSED(blocker2)

        const QSignalBlocker blocker3(ui->noteTextView);
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
    auto *item = Utils::Gui::getTreeWidgetItemWithUserData(ui->noteTreeWidget,
                                                           note.getId());

    if (item != nullptr) {
        delete(item);
    }
}

/**
 * Resets the current note to the first note
 */
void MainWindow::resetCurrentNote(bool goToTop) {
    auto *event =
        new QKeyEvent(QEvent::KeyPress, goToTop ? Qt::Key_Home : Qt::Key_Down,
            Qt::NoModifier);
    QApplication::postEvent(ui->noteTreeWidget, event);
}

/**
 * Stores the settings
 */
void MainWindow::storeSettings() {
    QSettings settings;

    // don't store the window settings in distraction free mode
    if (!isInDistractionFreeMode()) {
        settings.setValue(QStringLiteral("MainWindow/geometry"),
                          saveGeometry());
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
            if (keyEvent->key() == Qt::Key_Return) {
                // set a variable to ignore that first "Return" in the
                // return-handler
                _searchLineEditFromCompleter = true;
                return false;
            }
        } else if ((obj == ui->newNoteTagLineEdit) ||
                   ((ui->newNoteTagLineEdit->completer() != nullptr) &&
                    (obj == ui->newNoteTagLineEdit->completer()->popup()))) {
            // if tab is pressed while adding a tag the tag that starts with
            // the current text will be added
            if (keyEvent->key() == Qt::Key_Tab) {
                // fetch the tag that is starting with the current text
                Tag tag =
                    Tag::fetchByName(ui->newNoteTagLineEdit->text(), true);
                if (tag.isFetched()) {
                    linkTagNameToCurrentNote(tag.getName(), true);
                }

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
                (keyEvent->key() == Qt::Key_Tab) || downSelectNote) {
                // add the current search text to the saved searches
                storeSavedSearch();

                // choose another selected item if current item is invisible
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
            if ((keyEvent->key() == Qt::Key_Escape) &&
                isInDistractionFreeMode() &&
                !activeNoteTextEdit()->searchWidget()->isVisible()) {
                toggleDistractionFreeMode();

                return true;
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
//                if (!Note::allowDifferentFileName()) {
                    focusNoteTextEdit();
//                }
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
    } else if (event->type() == QEvent::MouseButtonRelease) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);

        if ((mouseEvent->button() == Qt::BackButton)) {
            // move back in the note history
            on_action_Back_in_note_history_triggered();
        } else if ((mouseEvent->button() == Qt::ForwardButton)) {
            // move forward in the note history
            on_action_Forward_in_note_history_triggered();
        }
    } else if (event->type() == QEvent::MouseButtonPress &&
               obj == ui->selectedTagsToolButton) {
        // we don't want to make the button clickable
        return true;
    }

    return QMainWindow::eventFilter(obj, event);
}

/**
 * Finds the first visible tree widget item
 */
QTreeWidgetItem *MainWindow::firstVisibleNoteTreeWidgetItem() {
    QTreeWidgetItemIterator it(ui->noteTreeWidget,
                               QTreeWidgetItemIterator::NotHidden);

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

    if (str.count() >= 2) {
        // do a in-note search
        doSearchInNote(str);
        ui->noteTextEdit->moveCursor(QTextCursor::Start);
        ui->noteTextView->moveCursor(QTextCursor::Start);
        ui->encryptedNoteTextEdit->moveCursor(QTextCursor::Start);
        const QColor color = QColor(0, 180, 0, 100);

        // build the string list of the search string
        const QString queryStr =
            str.replace(QLatin1String("|"), QLatin1String("\\|"));
        const QStringList queryStrings =
            Note::buildQueryStringList(queryStr, true);

        if (queryStrings.count() > 0) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
            const QRegularExpression regExp(
                QLatin1Char('(') + queryStrings.join(QLatin1String("|")) +
                    QLatin1Char(')'),
                QRegularExpression::CaseInsensitiveOption);
#else
            const QRegExp regExp(QLatin1String("(") +
                                     queryStrings.join(QLatin1String("|")) +
                                     QLatin1String(")"),
                                 Qt::CaseInsensitive);
#endif
            while (ui->noteTextEdit->find(regExp)) {
                QTextEdit::ExtraSelection extra = QTextEdit::ExtraSelection();
                extra.format.setBackground(color);

                extra.cursor = ui->noteTextEdit->textCursor();
                extraSelections.append(extra);
            }

            while (ui->noteTextView->find(regExp)) {
                QTextEdit::ExtraSelection extra = QTextEdit::ExtraSelection();
                extra.format.setBackground(color);

                extra.cursor = ui->noteTextView->textCursor();
                extraSelections2.append(extra);
            }

            while (ui->encryptedNoteTextEdit->find(regExp)) {
                QTextEdit::ExtraSelection extra = QTextEdit::ExtraSelection();
                extra.format.setBackground(color);

                extra.cursor = ui->encryptedNoteTextEdit->textCursor();
                extraSelections3.append(extra);
            }
        }
    }

    ui->noteTextEdit->setExtraSelections(extraSelections);
    ui->noteTextView->setExtraSelections(extraSelections2);
    ui->encryptedNoteTextEdit->setExtraSelections(extraSelections3);
}

/**
 * highlights all occurrences of the search line text in the note text edit
 */
void MainWindow::searchForSearchLineTextInNoteTextEdit() {
    QString searchString = ui->searchLineEdit->text();
    searchInNoteTextEdit(std::move(searchString));
}

/**
 * Asks for the password if the note is encrypted and can't be decrypted
 */
void MainWindow::askForEncryptedNotePasswordIfNeeded(
    const QString &additionalText) {
    currentNote.refetch();

    // check if the note is encrypted and can't be decrypted
    if (currentNote.hasEncryptedNoteText() &&
        !currentNote.canDecryptNoteText()) {
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
                // for the markdown view
                currentNote.setCryptoPassword(password);
                currentNote.store();
            }

            // warn if password is incorrect
            if (!currentNote.canDecryptNoteText()) {
                QMessageBox::warning(
                    this, tr("Note can't be decrypted!"),
                    tr("It seems that your password is not valid!"));
            }
        }

        delete (dialog);
    }
}

/**
 * Gets the maximum image width
 */
int MainWindow::getMaxImageWidth() {
    const QMargins margins = ui->noteTextView->contentsMargins();
    int maxImageWidth = ui->noteTextView->viewport()->width() - margins.left() -
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
                                     bool ignorePreviewVisibility) {
    if (note == nullptr) {
        return;
    }
    if (!updateNoteTextViewOnly) {
      qobject_cast<QOwnNotesMarkdownHighlighter *>(
          ui->noteTextEdit->highlighter())
          ->updateCurrentNote(note);
      ui->noteTextEdit->setText(note->getNoteText());
    }

    // update the preview text edit if the dock widget is visible
    if (_notePreviewDockWidget->isVisible() || ignorePreviewVisibility) {
        const bool decrypt = ui->noteTextEdit->isHidden();

        const QString html = note->toMarkdownHtml(
            NoteFolder::currentLocalPath(), getMaxImageWidth(), false, decrypt);

        // create a hash of the html (because
        const QString hash =
            QString(QCryptographicHash::hash(html.toLocal8Bit(),
                                             QCryptographicHash::Sha1)
                        .toHex());

        // update the note preview if the text has changed
        // we use our hash because ui->noteTextView->toHtml() may return
        // a different text than before
        if (_notePreviewHash != hash) {
            ui->noteTextView->setHtml(html);
            _notePreviewHash = hash;
        }
    }

    // update the slider when editing notes
    noteTextSliderValueChanged(
        activeNoteTextEdit()->verticalScrollBar()->value(), true);
}

/**
 * Starts the parsing for the navigation widget
 */
void MainWindow::startNavigationParser() {
    if (ui->navigationWidget->isVisible())
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
    currentNote.setNoteText(std::move(text));
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
void MainWindow::createNewNote(QString name, QString text,
                               CreateNewNoteOptions options) {
    const QString extension = Note::defaultNoteFileExtension();
    auto *f = new QFile(this->notesPath + QDir::separator() + name +
                        QStringLiteral(".") + extension);
    const bool useNameAsHeadline =
        options.testFlag(CreateNewNoteOption::UseNameAsHeadline);

    // change the name and headline if note exists
    if (f->exists()) {
        QDateTime currentDate = QDateTime::currentDateTime();
        name.append(QStringLiteral(" ") +
                    currentDate.toString(Qt::ISODate)
                        .replace(QStringLiteral(":"), QStringLiteral(".")));

        if (!useNameAsHeadline) {
            QString preText = Note::createNoteHeader(name);
            text.prepend(preText);
        }
    }

    // create a new note
    ui->searchLineEdit->setText(name);

    jumpToNoteOrCreateNew(
        options.testFlag(CreateNewNoteOption::DisableLoadNoteDirectoryList));

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
void MainWindow::restoreTrashedNoteOnServer(const QString &fileName,
                                            int timestamp) {
    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->restoreTrashedNoteOnServer(fileName, timestamp, this);
}

/**
 * @brief Removes selected notes after a confirmation
 */
void MainWindow::removeSelectedNotes() {
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    const int selectedItemsCount = getSelectedNotesCount();

    if (selectedItemsCount == 0) {
        return;
    }

    if (Utils::Gui::question(
            this, tr("Remove selected notes"),
            Utils::Misc::replaceOwnCloudText(
                tr("Remove <strong>%n</strong> selected note(s)?\n\n"
                   "If the trash is enabled on your "
                   "ownCloud server you should be able to restore "
                   "them from there.",
                   "", selectedItemsCount)),
            QStringLiteral("remove-notes")) == QMessageBox::Yes) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        const QSignalBlocker blocker2(activeNoteTextEdit());
        Q_UNUSED(blocker2)

        const QSignalBlocker blocker3(ui->noteTextView);
        Q_UNUSED(blocker3)

        const QSignalBlocker blocker4(ui->encryptedNoteTextEdit);
        Q_UNUSED(blocker4)

        // we try to fix problems with note subfolders
        directoryWatcherWorkaround(true);

        {
            const QSignalBlocker blocker1(ui->noteTreeWidget);
            Q_UNUSED(blocker1)

            const auto selItems = ui->noteTreeWidget->selectedItems();
            for (QTreeWidgetItem *item : selItems) {
                if (item->data(0, Qt::UserRole + 1) != NoteType) {
                    continue;
                }

                const int id = item->data(0, Qt::UserRole).toInt();
                Note note = Note::fetch(id);

                // search and remove note from the note tree widget
                removeNoteFromNoteTreeWidget(note);

                note.remove(true);
                qDebug() << "Removed note " << note.getName();
            }

            // clear the text edit so it stays clear after removing the
            // last note
            activeNoteTextEdit()->clear();
        }

        // set a new current note
        resetCurrentNote(false);

        // we try to fix problems with note subfolders
        // we need to wait some time to turn the watcher on again because
        // something is happening after this method that reloads the note folder
        directoryWatcherWorkaround(false);
    }

    loadNoteDirectoryList();
}

/**
 * Removes selected note subfolders after a confirmation
 */
void MainWindow::removeSelectedNoteSubFolders(QTreeWidget *treeWidget) {
    const int selectedItemsCount = treeWidget->selectedItems().size();

    if (selectedItemsCount == 0) {
        return;
    }

    // gather the folders that are about to be deleted
    QStringList noteSubFolderPathList;
    QVector<NoteSubFolder> noteSubFolderList;
    const auto selItems = treeWidget->selectedItems();
    for (QTreeWidgetItem *item : selItems) {
        if (item->data(0, Qt::UserRole + 1) != FolderType) {
            continue;
        }

        int id = item->data(0, Qt::UserRole).toInt();
        const NoteSubFolder noteSubFolder = NoteSubFolder::fetch(id);
        if (noteSubFolder.isFetched()) {
            noteSubFolderList << noteSubFolder;
            noteSubFolderPathList << noteSubFolder.fullPath();
        }
    }

    if (noteSubFolderList.count() == 0) {
        return;
    }

    if (Utils::Gui::question(
            this, tr("Remove selected folders"),
            tr("Remove <strong>%n</strong> selected folder(s)?"
               "<ul><li>%1</li></ul>"
               "All files and folders in these folders will be removed as"
               " well!",
               "", selectedItemsCount)
                .arg(noteSubFolderPathList.join(QStringLiteral("</li><li>"))),
            QStringLiteral("remove-folders")) == QMessageBox::Yes) {
        // delete the note subfolders
        for (const auto &noteSubFolder : Utils::asConst(noteSubFolderList)) {
            // remove the directory recursively from the file system
            if (noteSubFolder.removeFromFileSystem()) {
                showStatusBarMessage(tr("Removed note subfolder: %1")
                                         .arg(noteSubFolder.fullPath()));
            }
        }

        on_action_Reload_note_folder_triggered();
    }
}

/**
 * Removes selected tags after a confirmation
 */
void MainWindow::removeSelectedTags() {
    const int selectedItemsCount = ui->tagTreeWidget->selectedItems().size();

    if (selectedItemsCount == 0) {
        return;
    }

    if (Utils::Gui::question(
            this, tr("Remove selected tags"),
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
            tag.remove();
            qDebug() << "Removed tag " << tag.getName();

            // take care that the tag is removed from all notes
            handleScriptingNotesTagRemoving(tag, true);
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
            tr("Move %n selected note(s) to <strong>%2</strong>?", "",
               selectedItemsCount)
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
    const QSignalBlocker blocker(ui->noteTextView);
    Q_UNUSED(blocker)
    ui->noteTextView->clear();

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
            tr("Copy %n selected note(s) to <strong>%2</strong>?", "",
               selectedItemsCount)
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
            const bool result =
                note.copyToPath(destinationFolder, noteFolderPath);
            if (result) {
                copyCount++;
                qDebug() << "Note was copied:" << note.getName();
            } else {
                qWarning() << "Could not copy note:" << note.getName();
            }
        }

        Utils::Gui::information(
            this, tr("Done"),
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
            tr("Tag %n selected note(s) with <strong>%2</strong>?", "",
               selectedItemsCount)
                .arg(tag.getName()),
            QStringLiteral("tag-notes")) == QMessageBox::Yes) {
        int tagCount = 0;
        const bool useScriptingEngine =
            ScriptingService::instance()->noteTaggingHookExists();

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
            tr("%n note(s) were tagged with \"%2\"", "", tagCount)
                .arg(tag.getName()),
            5000);

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
            tr("Remove tag <strong>%1</strong> from %n selected note(s)?", "",
               selectedItemsCount)
                .arg(tag.getName()),
            QStringLiteral("remove-tag-from-notes")) == QMessageBox::Yes) {
        int tagCount = 0;
        const bool useScriptingEngine =
            ScriptingService::instance()->noteTaggingHookExists();

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
                qWarning() << "Could not remove tag from note:"
                           << note.getName();
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
            tr("Tag <strong>%1</strong> was removed from %n note(s)", "",
               tagCount)
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
void MainWindow::directoryWatcherWorkaround(
    bool isNotesDirectoryWasModifiedDisabled,
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
    handleTreeWidgetItemTagColor(noteItem, colorTag);
}

/**
 * @brief Updates the current folder tooltip
 */
void MainWindow::updateCurrentFolderTooltip() {
    ui->actionSet_ownCloud_Folder->setStatusTip(tr("Current notes folder: ") +
                                                this->notesPath);
    ui->actionSet_ownCloud_Folder->setToolTip(
        tr("Set the notes folder. Current notes folder: ") + this->notesPath);
}

/**
 * Opens the settings dialog
 */
void MainWindow::openSettingsDialog(int page, bool openScriptRepository) {
    if (_settingsDialog == Q_NULLPTR) {
        _settingsDialog = new SettingsDialog(page, this);
    } else {
        _settingsDialog->readSettings();
        _settingsDialog->setCurrentPage(page);
    }

    if (openScriptRepository) {
        QTimer::singleShot(150, _settingsDialog,
                           SLOT(searchScriptInRepository()));
    }

    // open the settings dialog
    _settingsDialog->exec();

    // seems to safe a little leaking memory
    // we must not null the dialog, this will crash if the ownCloud check
    // tries to write to the labels and the dialog went away
    //    delete(_settingsDialog);
    //    _settingsDialog = Q_NULLPTR;

    // shows a restart application notification if needed
    if (showRestartNotificationIfNeeded()) {
        return;
    }

    // make sure no settings get written after after we got the
    // clearAppDataAndExit call
    if (qApp->property("clearAppDataAndExit").toBool()) {
        return;
    }

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
bool MainWindow::showRestartNotificationIfNeeded() {
    const bool needsRestart = qApp->property("needsRestart").toBool();

    if (!needsRestart) {
        return false;
    }

    qApp->setProperty("needsRestart", false);
    const QString title = tr("Restart application");
    bool singleApplication = qApp->property("singleApplication").toBool();

    if (singleApplication) {
        QMessageBox::information(
            this, title,
            tr("You may need to restart the application manually to let "
               "the changes take effect."));
    } else {
        if (QMessageBox::information(
                this, title,
                tr("You may need to restart the application to let the changes "
                   "take effect."),
                tr("Restart"), tr("Cancel"), QString(), 0, 1) == 0) {
            storeSettings();
            Utils::Misc::restartApplication();
            return true;
        }
    }

    return false;
}

/**
 * @brief Returns the active note text edit
 */
QOwnNotesMarkdownTextEdit *MainWindow::activeNoteTextEdit() {
    return ui->noteTextEdit->isHidden() ? ui->encryptedNoteTextEdit
                                        : ui->noteTextEdit;
}

/**
 * @brief Handles the linking of text
 */
void MainWindow::handleTextNoteLinking() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    auto *dialog = new LinkDialog(QString(), this);

    QString selectedText = textEdit->textCursor().selectedText();
    if (!selectedText.isEmpty()) {
        dialog->setLinkName(selectedText);
    }

    dialog->exec();

    if (dialog->result() == QDialog::Accepted) {
        const QString url = dialog->getURL();
        const QString linkName = dialog->getLinkName();
        const QString linkDescription = dialog->getLinkDescription();
        // remove ] characters, because they will break markdown links
        QString noteName = dialog->getSelectedNoteName().remove("]");

        if ((!noteName.isEmpty()) || (!url.isEmpty())) {
            QString newText;
            QString chosenLinkName = linkName.isEmpty()
                                         ? textEdit->textCursor().selectedText()
                                         : linkName;
            // remove ] characters, because they will break markdown links
            chosenLinkName.remove("]");

            // if user has entered an url
            if (!url.isEmpty()) {
                newText = !chosenLinkName.isEmpty()
                              ? QStringLiteral("[") + chosenLinkName +
                                    QStringLiteral("](") + url +
                                    QStringLiteral(")")
                              : QStringLiteral("<") + url + QStringLiteral(">");
            } else {
                // if user has selected a note
                if (!chosenLinkName.isEmpty()) {
                    noteName = chosenLinkName;
                }

                const QString noteUrl = currentNote.getNoteUrlForLinkingTo(
                    dialog->getSelectedNote());

                newText = QStringLiteral("[") + noteName +
                          QStringLiteral("](") + noteUrl + QStringLiteral(")");
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
    Utils::Misc::loadPrinterSettings(printer,
                                     QStringLiteral("Printer/NotePrinting"));

    QPrintDialog dialog(printer, this);
    dialog.setWindowTitle(tr("Print note"));
    const int ret = dialog.exec();

    if (ret != QDialog::Accepted) {
        return false;
    }

    Utils::Misc::storePrinterSettings(printer,
                                      QStringLiteral("Printer/NotePrinting"));
    return true;
}

/**
 * @brief Prints the content of a plain text edit widget
 * @param textEdit
 */
void MainWindow::printNote(QPlainTextEdit *textEdit) {
    printTextDocument(textEdit->document());
}

/**
 * @brief Prints the content of a text edit widget
 * @param textEdit
 */
void MainWindow::printNote(QTextEdit *textEdit) {
    printTextDocument(textEdit->document());
}

/**
 * @brief Prints the content of a text document
 * @param textEdit
 */
void MainWindow::printTextDocument(QTextDocument *textDocument) {
    auto *printer = new QPrinter();

    if (preparePrintNotePrinter(printer)) {
        textDocument->print(printer);
    }

    delete printer;
}

/**
 * @brief Prepares the printer dialog to exports the content of a text edit
 *        widget as PDF
 * @param printer
 */
bool MainWindow::prepareExportNoteAsPDFPrinter(QPrinter *printer) {
#ifdef Q_OS_LINUX
    Utils::Misc::loadPrinterSettings(printer,
                                     QStringLiteral("Printer/NotePDFExport"));

    // under Linux we use the the QPageSetupDialog to change layout
    // settings of the PDF export
    QPageSetupDialog pageSetupDialog(printer, this);

    if (pageSetupDialog.exec() != QDialog::Accepted) {
        return false;
    }

    Utils::Misc::storePrinterSettings(printer,
                                      QStringLiteral("Printer/NotePDFExport"));
#else
    // under OS X and Windows the QPageSetupDialog dialog doesn't work,
    // we will use a workaround to select page sizes and the orientation

    QSettings settings;

    // select the page size
    QStringList pageSizeStrings;
    pageSizeStrings << QStringLiteral("A0") << QStringLiteral("A1")
                    << QStringLiteral("A2") << QStringLiteral("A3")
                    << QStringLiteral("A4") << QStringLiteral("A5")
                    << QStringLiteral("A6") << QStringLiteral("A7")
                    << QStringLiteral("A8") << QStringLiteral("A9")
                    << tr("Letter");
    QList<QPageSize::PageSizeId> pageSizes;
    pageSizes << QPageSize::A0 << QPageSize::A1 << QPageSize::A2
              << QPageSize::A3 << QPageSize::A4 << QPageSize::A5
              << QPageSize::A6 << QPageSize::A7 << QPageSize::A8
              << QPageSize::A9 << QPageSize::Letter;

    bool ok;
    QString pageSizeString = QInputDialog::getItem(
        this, tr("Page size"), tr("Page size:"), pageSizeStrings,
        settings.value(QStringLiteral("Printer/NotePDFExportPageSize"), 4)
            .toInt(),
        false, &ok);

    if (!ok || pageSizeString.isEmpty()) {
        return false;
    }

    int pageSizeIndex = pageSizeStrings.indexOf(pageSizeString);
    if (pageSizeIndex == -1) {
        return false;
    }

    QPageSize pageSize(pageSizes.at(pageSizeIndex));
    settings.setValue(QStringLiteral("Printer/NotePDFExportPageSize"),
                      pageSizeIndex);
    printer->setPageSize(pageSize);

    // select the orientation
    QStringList orientationStrings;
    orientationStrings << tr("Portrait") << tr("Landscape");
    QList<QPrinter::Orientation> orientations;
    orientations << QPrinter::Portrait << QPrinter::Landscape;

    QString orientationString = QInputDialog::getItem(
        this, tr("Orientation"), tr("Orientation:"), orientationStrings,
        settings.value(QStringLiteral("Printer/NotePDFExportOrientation"), 0)
            .toInt(),
        false, &ok);

    if (!ok || orientationString.isEmpty()) {
        return false;
    }

    int orientationIndex = orientationStrings.indexOf(orientationString);
    if (orientationIndex == -1) {
        return false;
    }

    printer->setOrientation(orientations.at(orientationIndex));
    settings.setValue(QStringLiteral("Printer/NotePDFExportOrientation"),
                      orientationIndex);
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
 * @brief Exports the content of a text edit widget as PDF
 * @param textEdit
 */
void MainWindow::exportNoteAsPDF(QTextEdit *textEdit) {
    exportNoteAsPDF(textEdit->document());
}

/**
 * @brief Exports the document as PDF
 * @param doc
 */
void MainWindow::exportNoteAsPDF(QTextDocument *doc) {
    auto *printer = new QPrinter(QPrinter::HighResolution);

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
    QSettings settings;
    bool showDialog =
        !settings.value(QStringLiteral("appMetrics/notificationShown"))
             .toBool();

    if (showDialog) {
        settings.setValue(QStringLiteral("appMetrics/notificationShown"), true);

        if (QMessageBox::information(
                this, QStringLiteral("QOwnNotes"),
                tr("QOwnNotes will track anonymous usage data, that helps to "
                   "decide what parts of QOwnNotes to improve next "
                   "and to find and fix bugs. You can disable that "
                   "behaviour in the settings."),
                tr("&Ok"), tr("Open &settings"), QString(), 0, 1) == 1) {
            openSettingsDialog(SettingsDialog::NetworkPage);
        }
    }
}

/**
 * Opens the task list dialog
 */
void MainWindow::openTodoDialog(const QString &taskUid) {
    if (!OwnCloudService::isTodoCalendarSupportEnabled()) {
        if (QMessageBox::warning(nullptr, tr("Todo lists disabled!"),
                                 tr("You have disabled the todo lists.<br />"
                                    "Please check your <strong>Todo</strong> "
                                    "configuration in the settings!"),
                                 tr("Open &settings"), tr("&Cancel"), QString(),
                                 0, 1) == 0) {
            openSettingsDialog(SettingsDialog::TodoPage);
        }

        return;
    }

    // show main window to prevent crashes on Windows
    show();

    QSettings settings;
    QStringList todoCalendarEnabledUrlList =
        settings.value(QStringLiteral("ownCloud/todoCalendarEnabledUrlList"))
            .toStringList();

    // check if we have got any task list enabled
    if (todoCalendarEnabledUrlList.count() == 0) {
        if (QMessageBox::warning(
                nullptr, tr("No selected todo lists!"),
                tr("You have not selected any todo lists.<br />"
                   "Please check your <strong>Todo</strong> "
                   "configuration in the settings!"),
                tr("Open &settings"), tr("&Cancel"), QString(), 0, 1) == 0) {
            openSettingsDialog(SettingsDialog::TodoPage);
        }

        return;
    }

    if (_todoDialog == Q_NULLPTR) {
        _todoDialog = new TodoDialog(this, taskUid, this);
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
}

void MainWindow::noteTextEditTextWasUpdated() {
    Note note = this->currentNote;
    note.updateNoteTextFromDisk();

    // we are transforming line feeds, because in some instances Windows
    // managed to sneak some "special" line feeds in
    const QString noteTextFromDisk =
        Utils::Misc::transformLineFeeds(note.getNoteText());
    QString text =
        Utils::Misc::transformLineFeeds(ui->noteTextEdit->toPlainText());

    // store the note to the database if the note text differs from the one
    // on the disk or the note was already modified but not stored to disk
    if ((text != noteTextFromDisk || currentNote.getHasDirtyData())) {
        this->currentNote.storeNewText(std::move(text));
        this->currentNote.refetch();
        this->currentNoteLastEdited = QDateTime::currentDateTime();
        _noteViewNeedsUpdate = true;

        ScriptingService::instance()->onCurrentNoteChanged(&currentNote);

        updateEncryptNoteButtons();
        handleNoteTextChanged();
    }
}

void MainWindow::handleNoteTextChanged() {
    QSettings settings;
    if (settings.value(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE)
            .toInt() == SORT_BY_LAST_CHANGE) {
        makeCurrentNoteFirstInNoteList();
    } else if (Utils::Misc::isNoteListPreview()) {
        updateNoteTreeWidgetItem(currentNote);
    }

    const QSignalBlocker blocker(ui->noteTreeWidget);
    Q_UNUSED(blocker)

    // update the note list tooltip of the note
    setTreeWidgetItemToolTipForNote(ui->noteTreeWidget->currentItem(),
                                    currentNote, &currentNoteLastEdited);
}

void MainWindow::on_action_Quit_triggered() {
    // this will be done again in the destructor, but we want to make sure
    // nothing is logged to the log widget that might already be destroyed
    qApp->setProperty("loggingEnabled", false);

    storeSettings();
    QApplication::quit();
}

void MainWindow::quitApp() {
    // this will be done again in the destructor, but we want to make sure
    // nothing is logged to the log widget that might already be destroyed
    qApp->setProperty("loggingEnabled", false);

    QApplication::quit();
}

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
    filterNotesBySearchLineEditText();

    if (NoteFolder::isCurrentShowSubfolders() &&
        !_showNotesFromAllNoteSubFolders) {
        // filter the notes by note sub folder
        filterNotesByNoteSubFolders();
    }

    // moved condition whether to filter notes by tag at all into
    // filterNotesByTag() -- it can now be used as a slot at startup
    filterNotesByTag();

    if (searchForText) {
        // let's highlight the text from the search line edit
        searchForSearchLineTextInNoteTextEdit();
    }
}

/**
 * Checks if tagging is enabled
 */
bool MainWindow::isTagsEnabled() { return _taggingDockWidget->isVisible(); }

/**
 * Checks if the markdown view is enabled
 */
bool MainWindow::isMarkdownViewEnabled() {
    QSettings settings;
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
void MainWindow::filterNotesBySearchLineEditText() {
    const QString searchText = ui->searchLineEdit->text();

    QTreeWidgetItemIterator it(ui->noteTreeWidget);
    ui->noteTreeWidget->setColumnCount(1);

    // search notes when at least 2 characters were entered
    if (searchText.count() >= 2) {
        // open search dialog
        doSearchInNote(searchText);

        QVector<int> noteIdList = Note::searchInNotes(
            searchText,
            _showNotesFromAllNoteSubFolders ||
                NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively());

        int columnWidth = ui->noteTreeWidget->columnWidth(0);
        ui->noteTreeWidget->setColumnCount(2);
        int maxWidth = 0;
        const QStringList searchTextTerms =
            Note::buildQueryStringList(searchText);
        const QSettings settings;
        const bool showMatches =
            settings.value(QStringLiteral("showMatches"), true).toBool();

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

        // resize the column 0 so we can see the search counts
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
 * Opens the search widget in the the current note and searches for all
 * occurrences of the words in the search text
 *
 * @param searchText
 */
void MainWindow::doSearchInNote(QString searchText) {
    const QStringList searchTextTerms =
        Note::buildQueryStringList(searchText, true, true);

    if (searchTextTerms.count() > 1) {
        QString localSearchTerm = QStringLiteral("(") +
                                  searchTextTerms.join(QStringLiteral("|")) +
                                  QStringLiteral(")");
        activeNoteTextEdit()->doSearch(
            localSearchTerm, QPlainTextEditSearchWidget::RegularExpressionMode);
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

            const auto selectedFolderItems =
                ui->noteSubFolderTreeWidget->selectedItems();

            const bool showNotesFromAllNoteSubFolders = _showNotesFromAllNoteSubFolders;
            noteIdList.reserve(tagIdList.count() * 2);
            if (selectedFolderItems.count() > 1) {
                for (const int tagId_ : Utils::asConst(tagIdList)) {
                    for (const QTreeWidgetItem *i : selectedFolderItems) {
                        const int id = i->data(0, Qt::UserRole).toInt();
                        const NoteSubFolder folder = NoteSubFolder::fetch(id);

                        noteIdList << Tag::fetchAllLinkedNoteIdsForFolder(
                                          tagId_, folder,
                                          showNotesFromAllNoteSubFolders);
                    }
                }
            } else {
                for (const int tagId_ : Utils::asConst(tagIdList)) {
                    noteIdList << Tag::fetchAllLinkedNoteIds(
                                      tagId_,
                                      showNotesFromAllNoteSubFolders);
                }
            }
            break;
    }

    qDebug() << __func__ << " - 'noteIdList': " << noteIdList;

    // omit the already hidden notes
    QTreeWidgetItemIterator it(ui->noteTreeWidget,
                               QTreeWidgetItemIterator::NotHidden);

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
            noteSubFolderIds
                << NoteSubFolder::fetchIdsRecursivelyByParentId(subFolId);
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

    // this doesn't seem to work with note sub folders
    const QSignalBlocker blocker(noteDirectoryWatcher);
    Q_UNUSED(blocker)

    // add the current search text to the saved searches
    storeSavedSearch();

    const QString text = ui->searchLineEdit->text().trimmed();

    // clear search line edit so all notes will be viewed again and to prevent
    // a brief appearing of the note search widget when creating a new note
    // with action_New_note
    ui->searchLineEdit->clear();

    // first let us search for the entered text
    Note note = Note::fetchByName(text);

    // if we can't find a note we create a new one
    if (note.getId() == 0) {
        // check if a hook wants to set the text
        QString noteText =
            ScriptingService::instance()->callHandleNewNoteHeadlineHook(text);

        // check if a hook changed the text
        if (noteText.isEmpty()) {
            // fallback to the old text if no hook changed the text
            noteText = Note::createNoteHeader(text);
        } else {
            noteText.append(QLatin1String("\n\n"));
        }

        const NoteSubFolder noteSubFolder =
            NoteSubFolder::activeNoteSubFolder();
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
        showStatusBarMessage(
            noteWasStored ? tr("Stored current note to disk")
                          : tr("Current note could not be stored to disk"),
            3000);

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
    QSettings settings;
    const bool newNoteAskHeadline =
        settings.value(QStringLiteral("newNoteAskHeadline")).toBool();

    // check if we want to ask for a headline
    if (newNoteAskHeadline) {
        bool ok;
        QString headline =
            QInputDialog::getText(this, tr("New note"), tr("Note headline"),
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
        noteName = QStringLiteral("Note");
    }

    if (withNameAppend) {
        QDateTime currentDate = QDateTime::currentDateTime();

        // replacing ":" with "_" for Windows systems
        noteName = noteName + QStringLiteral(" ") +
                   currentDate.toString(Qt::ISODate)
                       .replace(QStringLiteral(":"), QStringLiteral("."));
    }

    const QSignalBlocker blocker(ui->searchLineEdit);
    Q_UNUSED(blocker)

    ui->searchLineEdit->setText(noteName);

    // create a new note or jump to the existing
    jumpToNoteOrCreateNew();
}

/*
 * Handles urls in the noteTextView
 *
 * examples:
 * - <note://MyNote> opens the note "MyNote"
 * - <note://my-note-with-spaces-in-the-name> opens the note "My Note with
 * spaces in the name"
 * - <https://www.qownnotes.org> opens the web page
 * - <file:///path/to/my/note/folder/subfolder/My%20note.pdf> opens the note
 * "My note" in the subfolder "subfolder"
 * - <file:///path/to/my/file/QOwnNotes.pdf> opens the file
 * "/path/to/my/file/QOwnNotes.pdf" if the operating system supports that
 * handler
 */
void MainWindow::on_noteTextView_anchorClicked(const QUrl &url) {
    qDebug() << __func__ << " - 'url': " << url;
    const QString scheme = url.scheme();

    if ((scheme == QStringLiteral("note") ||
         scheme == QStringLiteral("noteid") ||
         scheme == QStringLiteral("task") ||
         scheme == QStringLiteral("checkbox")) ||
        (scheme == QStringLiteral("file") &&
         Note::fileUrlIsNoteInCurrentNoteFolder(url))) {
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
 * - <note://my-note-with-spaces-in-the-name> opens the note "My Note with
 * spaces in the name"
 */
void MainWindow::openLocalUrl(QString urlString) {
    if (urlString.isEmpty()) {
        return;
    }

    bool urlWasNotValid = false;

    // if urlString is no valid url we will try to convert it into a note file
    // url
    if (!QOwnNotesMarkdownTextEdit::isValidUrl(urlString)) {
        urlString = currentNote.getFileURLFromFileName(urlString, true);
        urlWasNotValid = true;
    }

    QUrl url = QUrl(urlString);
    const bool isExistingNoteFileUrl = Note::fileUrlIsExistingNoteInCurrentNoteFolder(url);
    const bool isNoteFileUrl = Note::fileUrlIsNoteInCurrentNoteFolder(url);

    // convert relative file urls to absolute urls and open them
    if (urlString.startsWith(QStringLiteral("file://..")) && !isExistingNoteFileUrl) {
        QString windowsSlash = QString();

#ifdef Q_OS_WIN32
        // we need another slash for Windows
        windowsSlash = QStringLiteral("/");
#endif

        urlString.replace(QLatin1String("file://.."),
                          QStringLiteral("file://") + windowsSlash +
                              NoteFolder::currentLocalPath() +
                              QStringLiteral("/.."));

        QDesktopServices::openUrl(QUrl(urlString));
        return;
    }

    // convert legacy attachment urls to absolute urls and open them
    if (urlString.startsWith(QStringLiteral("file://attachments"))) {
        QString windowsSlash = QString();

#ifdef Q_OS_WIN32
        // we need another slash for Windows
        windowsSlash = QStringLiteral("/");
#endif

        urlString.replace(QLatin1String("file://attachments"),
                          QStringLiteral("file://") + windowsSlash +
                              NoteFolder::currentLocalPath() +
                              QStringLiteral("/attachments"));

        QDesktopServices::openUrl(QUrl(urlString));
        return;
    }

    const QString scheme = url.scheme();

    if (scheme == QStringLiteral("noteid")) {    // jump to a note by note id
        QRegularExpressionMatch match =
            QRegularExpression(QStringLiteral(R"(^noteid:\/\/note-(\d+)$)"))
                .match(urlString);

        if (match.hasMatch()) {
            int noteId = match.captured(1).toInt();
            Note note = Note::fetch(noteId);
            if (note.isFetched()) {
                // set current note
                setCurrentNote(std::move(note));
            }
        } else {
            qDebug() << "malformed url: " << urlString;
        }
    } else if (scheme == QStringLiteral("note") ||
               isNoteFileUrl) {    // jump to a note url string
        Note note;

        if (isNoteFileUrl) {
            ui->noteSubFolderTreeWidget->reset();
            note = Note::fetchByFileUrl(url);
        } else {
            // try to fetch a note from the url string
            note = Note::fetchByUrlString(urlString);
        }

        // does this note really exist?
        if (note.isFetched()) {
            // set current note
            setCurrentNote(std::move(note));
        } else {
            QString fileName;
            QUrl filePath;

            if (!isNoteFileUrl) {
                // if the name of the linked note only consists of numbers we cannot
                // use host() to get the filename, it would get converted to an
                // ip-address
                QRegularExpressionMatch match =
                    QRegularExpression(QStringLiteral(R"(^\w+:\/\/(\d+)$)"))
                        .match(urlString);
                fileName =
                    match.hasMatch() ? match.captured(1) : url.host();

                // try to generate a useful title for the note
                fileName = Utils::Misc::toStartCase(
                            fileName.replace(QStringLiteral("_"), QStringLiteral(" ")));
            } else {
                fileName = url.fileName();
                filePath = url.adjusted(QUrl::RemoveFilename);
            }

            // remove file extension
            QFileInfo fileInfo(fileName);
            fileName = fileInfo.baseName();

            QString relativeFilePath =
                    Note::fileUrlInCurrentNoteFolderToRelativePath(filePath);

            if (!relativeFilePath.isEmpty() && !NoteFolder::isCurrentHasSubfolders()) {
                Utils::Gui::warning(
                    this, tr("Note was not found"),
                    tr("Could not find note.<br />Unable to automatically "
                       "create note at location, because subfolders are "
                       "disabled for the current note folder."),
                    "cannot-create-note-not-has-subfolders");
                return;
            }

            QString promptQuestion;

            if (relativeFilePath.isEmpty()) {
                promptQuestion = tr("Note was not found, create new note "
                                    "<strong>%1</strong>?")
                        .arg(fileName);
            } else {
                promptQuestion = tr("Note was not found, create new note "
                                    "<strong>%1</strong> at path <strong>%2</strong> ?")
                        .arg(fileName).arg(relativeFilePath);
            }

            // ask if we want to create a new note if note wasn't found
            if (Utils::Gui::questionNoSkipOverride(this, tr("Note was not found"),
                                                   promptQuestion,
                                                   QStringLiteral("open-url-create-note")) == QMessageBox::Yes) {

                NoteSubFolder noteSubFolder = NoteSubFolder::activeNoteSubFolder();
                bool subFolderCreationFailed(false);

                if (!relativeFilePath.isEmpty()) {
                    for (QString folderName : relativeFilePath.split("/")) {
                        if (folderName.isEmpty()) {
                            break;
                        }

                        NoteSubFolder subFolder = NoteSubFolder::fetchByNameAndParentId(folderName, noteSubFolder.getId());
                        if (!subFolder.exists()) {
                            createNewNoteSubFolder(folderName);
                            noteSubFolder = NoteSubFolder::fetchByNameAndParentId(folderName, noteSubFolder.getId());
                            if (!noteSubFolder.exists()) {
                                qWarning() << "Failed to create subfolder: " << folderName <<
                                              "when attempting to create path: " << relativeFilePath;
                                subFolderCreationFailed = true;
                                break;
                            }
                        } else {
                            noteSubFolder = subFolder;
                        }

                        noteSubFolder.setAsActive();
                    }
                }

                if (!subFolderCreationFailed) {
                    if (!relativeFilePath.isEmpty()) {
                        ui->noteSubFolderTreeWidget->reset();
                        jumpToNoteSubFolder(noteSubFolder.getId());
                    }
                    createNewNote(fileName, false);
                } else {
                    Utils::Gui::warning(
                        this, tr("Failed to create note"),
                        tr("Note creation failed"),
                        "note-create-failed");
                }
                return;
            }
        }
    } else if (scheme == QStringLiteral("task")) {
        return openTodoDialog(url.host());
    } else if (scheme == QStringLiteral("checkbox")) {
        const auto text = ui->noteTextEdit->toPlainText();

        int index = url.host().midRef(1).toInt();
        QRegExp re(R"((^|\n)\s*[-*+]\s\[([xX ]?)\])", Qt::CaseInsensitive);
        int pos = 0;
        while (true) {
            pos = re.indexIn(text, pos);
            if (pos == -1)    // not found
                return;
            auto cursor = ui->noteTextEdit->textCursor();
            cursor.setPosition(pos + re.matchedLength() - 1);
            if (cursor.block().userState() ==
                MarkdownHighlighter::HighlighterState::List) {
                if (index == 0) {
                    auto ch = re.cap(2);
                    if (ch.isEmpty())
                        cursor.insertText(QStringLiteral("x"));
                    else {
                        cursor.movePosition(QTextCursor::PreviousCharacter,
                                            QTextCursor::KeepAnchor);
                        cursor.insertText(ch == QStringLiteral(" ")
                                              ? QStringLiteral("x")
                                              : QStringLiteral(" "));
                    }

                    // refresh instantly
                    _noteViewUpdateTimer->start(1);
                    break;
                }
                --index;
            }
            pos += re.matchedLength();
        }
    } else if (scheme == QStringLiteral("file") && urlWasNotValid) {
        // open urls that previously were not valid
        QDesktopServices::openUrl(QUrl(urlString));
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
    //    QDesktopServices::openUrl(QUrl("https://github.com/pbek/QOwnNotes/issues"));

    openIssueAssistantDialog();
}

void MainWindow::openIssueAssistantDialog() {
    if (_issueAssistantDialog == Q_NULLPTR) {
        _issueAssistantDialog = new IssueAssistantDialog(this);
    } else {
    }

    _issueAssistantDialog->show();
    _issueAssistantDialog->activateWindow();
    _issueAssistantDialog->raise();
}

void MainWindow::on_actionAlphabetical_triggered(bool checked) {
    if (checked) {
        QSettings settings;
        settings.setValue(QStringLiteral("notesPanelSort"), SORT_ALPHABETICAL);
        loadNoteDirectoryList();
    }

    // update the visibility of the note sort order selector
    updateNoteSortOrderSelectorVisibility(checked);
}

void MainWindow::on_actionBy_date_triggered(bool checked) {
    if (checked) {
        QSettings settings;
        settings.setValue(QStringLiteral("notesPanelSort"),
                          SORT_BY_LAST_CHANGE);
        loadNoteDirectoryList();
    }

    // update the visibility of the note sort order selector
    updateNoteSortOrderSelectorVisibility(!checked);
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
    raise();             // for MacOS
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
    QAction *openAction = menu->addAction(tr("Open QOwnNotes"));
    openAction->setIcon(getSystemTrayIcon());

    connect(openAction, &QAction::triggered, this, &MainWindow::showWindow);

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

                action->setIcon(_folderIcon);
            }

            const int folderId = noteFolder.getId();
            connect(action, &QAction::triggered, this,
                    [this, folderId]() { changeNoteFolder(folderId); });
        }

        menu->addSeparator();
    }

    // add menu entry to create a new note
    QAction *createNoteAction = menu->addAction(tr("New note"));
    createNoteAction->setIcon(QIcon::fromTheme(
        QStringLiteral("document-new"),
        QIcon(
            QStringLiteral(":icons/breeze-qownnotes/16x16/document-new.svg"))));

    connect(createNoteAction, &QAction::triggered, this,
            &MainWindow::on_action_New_note_triggered);

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
            action->setIcon(_noteIcon);
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
        QIcon(QStringLiteral(
            ":icons/breeze-qownnotes/16x16/view-calendar-tasks.svg"))));

    connect(taskAction, &QAction::triggered, this,
            [this]() { openTodoDialog(); });

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
                QIcon(QStringLiteral(
                    ":icons/breeze-qownnotes/16x16/view-task.svg"))));

            connect(action, &QAction::triggered, this,
                    [this, task]() { openTodoDialog(task.getUid()); });
        }
    }

    menu->addSeparator();

    // add menu entry to quit the app
    QAction *quitAction = menu->addAction(tr("Quit"));
    quitAction->setIcon(QIcon::fromTheme(
        QStringLiteral("application-exit"),
        QIcon(QStringLiteral(
            ":icons/breeze-qownnotes/16x16/application-exit.svg"))));
    connect(quitAction, &QAction::triggered, this,
            &MainWindow::on_action_Quit_triggered);

    trayIcon->setContextMenu(menu);
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
        Utils::Misc::replaceOwnCloudText(
            tr("Note versions are currently loaded from your ownCloud "
               "server")),
        20000);

    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->loadVersions(
        this->currentNote.relativeNoteFilePath(QStringLiteral("/")), this);
}

void MainWindow::enableShowVersionsButton() {
    ui->actionShow_versions->setDisabled(false);
}

void MainWindow::on_actionShow_trash_triggered() {
    ui->actionShow_trash->setDisabled(true);
    showStatusBarMessage(
        Utils::Misc::replaceOwnCloudText(
            tr("Trashed notes are currently loaded from your ownCloud"
               " server")),
        20000);

    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->loadTrash(this);
}

void MainWindow::enableShowTrashButton() {
    ui->actionShow_trash->setDisabled(false);
}

void MainWindow::on_actionSelect_all_notes_triggered() { selectAllNotes(); }

/**
 * Creates the additional menu entries for the note text edit field
 *
 * @param pos
 */
void MainWindow::on_noteTextEdit_customContextMenuRequested(const QPoint pos) {
    noteTextEditCustomContextMenuRequested(ui->noteTextEdit, pos);
}

/**
 * Creates the additional menu entries for the encrypted note text edit field
 *
 * @param pos
 */
void MainWindow::on_encryptedNoteTextEdit_customContextMenuRequested(
    const QPoint pos) {
    noteTextEditCustomContextMenuRequested(ui->encryptedNoteTextEdit, pos);
}

/**
 * Creates the additional menu entries for a note text edit field
 *
 * @param noteTextEdit
 * @param pos
 */
void MainWindow::noteTextEditCustomContextMenuRequested(
    QOwnNotesMarkdownTextEdit *noteTextEdit, const QPoint pos) {
    const QPoint globalPos = noteTextEdit->mapToGlobal(pos);
    QMenu *menu = noteTextEdit->createStandardContextMenu();
    const bool isAllowNoteEditing = Utils::Misc::isNoteEditingAllowed();
    const bool isTextSelected = isNoteTextSelected();

    const QString linkTextActionName =
        isTextSelected ? tr("&Link selected text") : tr("Insert &link");
    QAction *linkTextAction = menu->addAction(linkTextActionName);
    linkTextAction->setShortcut(ui->actionInsert_Link_to_note->shortcut());
    linkTextAction->setEnabled(isAllowNoteEditing);

    QString blockQuoteTextActionName =
        isTextSelected ? tr("Block &quote selected text",
                            "Action to apply a block quote formatting to the "
                            "selected text")
                       : tr("Insert block &quote");
    QAction *blockQuoteTextAction = menu->addAction(blockQuoteTextActionName);
    blockQuoteTextAction->setShortcut(ui->actionInsert_block_quote->shortcut());
    blockQuoteTextAction->setEnabled(isAllowNoteEditing);

    QAction *searchAction =
        menu->addAction(ui->actionSearch_text_on_the_web->text());
    searchAction->setShortcut(ui->actionSearch_text_on_the_web->shortcut());
    searchAction->setEnabled(isTextSelected);

    QAction *copyCodeBlockAction = menu->addAction(tr("Copy code block"));
    copyCodeBlockAction->setIcon(QIcon::fromTheme(
        QStringLiteral("edit-copy"),
        QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/edit-copy.svg"))));
    const QTextBlock &currentTextBlock =
        noteTextEdit->cursorForPosition(pos).block();
    const int userState = currentTextBlock.userState();
    copyCodeBlockAction->setEnabled(
        MarkdownHighlighter::isCodeBlock(userState));

    menu->addSeparator();

    // add the print menu
    QMenu *printMenu = menu->addMenu(tr("Print"));
    QIcon printIcon = QIcon::fromTheme(
        QStringLiteral("document-print"),
        QIcon(QStringLiteral(
            ":icons/breeze-qownnotes/16x16/document-print.svg")));
    printMenu->setIcon(printIcon);

    // add the print selected text action
    QAction *printTextAction = printMenu->addAction(tr("Print selected text"));
    printTextAction->setEnabled(isTextSelected);
    printTextAction->setIcon(printIcon);

    // add the print selected text (preview) action
    QAction *printHTMLAction =
        printMenu->addAction(tr("Print selected text (preview)"));
    printHTMLAction->setEnabled(isTextSelected);
    printHTMLAction->setIcon(printIcon);

    // add the export menu
    QMenu *exportMenu = menu->addMenu(tr("Export"));
    exportMenu->setIcon(QIcon::fromTheme(
        QStringLiteral("document-export"),
        QIcon(QStringLiteral(
            ":icons/breeze-qownnotes/16x16/document-export.svg"))));

    QIcon pdfIcon = QIcon::fromTheme(
        QStringLiteral("application-pdf"),
        QIcon(QStringLiteral(
            ":icons/breeze-qownnotes/16x16/application-pdf.svg")));

    // add the export selected text action
    QAction *exportTextAction =
        exportMenu->addAction(tr("Export selected text as PDF"));
    exportTextAction->setEnabled(isTextSelected);
    exportTextAction->setIcon(pdfIcon);

    // add the export selected text (preview) action
    QAction *exportHTMLAction =
        exportMenu->addAction(tr("Export selected text as PDF (preview)"));
    exportHTMLAction->setEnabled(isTextSelected);
    exportHTMLAction->setIcon(pdfIcon);

    menu->addSeparator();

    // add some other existing menu entries
    menu->addAction(ui->actionPaste_image);
    menu->addAction(ui->actionAutocomplete);
    menu->addAction(ui->actionSplit_note_at_cursor_position);

    // add the custom actions to the context menu
    if (!_noteTextEditContextMenuActions.isEmpty()) {
        // add the scripts menu
        QIcon scriptIcon = QIcon::fromTheme(
            QStringLiteral("story-editor"),
            QIcon(QStringLiteral(
                ":icons/breeze-qownnotes/16x16/story-editor.svg")));
        menu->addSeparator();
        QMenu *scriptMenu = menu->addMenu(tr("Custom actions"));
        scriptMenu->setIcon(scriptIcon);
        for (QAction *action :
             Utils::asConst(_noteTextEditContextMenuActions)) {
            scriptMenu->addAction(action);
        }
    }

    QAction *selectedItem = menu->exec(globalPos);
    if (selectedItem) {
        if (selectedItem == linkTextAction) {
            // handle the linking of text with a note
            handleTextNoteLinking();
        } else if (selectedItem == blockQuoteTextAction) {
            // handle the block quoting of text
            on_actionInsert_block_quote_triggered();
        } else if (selectedItem == searchAction) {
            // search for the selected text on the web
            on_actionSearch_text_on_the_web_triggered();
        } else if (selectedItem == printTextAction) {
            // print the selected text
            auto *textEdit = new QOwnNotesMarkdownTextEdit(this);
            textEdit->setPlainText(selectedNoteTextEditText());
            printNote(textEdit);
        } else if (selectedItem == printHTMLAction) {
            // print the selected text (preview)
            QString html = currentNote.textToMarkdownHtml(
                selectedNoteTextEditText(), NoteFolder::currentLocalPath(),
                getMaxImageWidth(),
                Utils::Misc::useInternalExportStylingForPreview());
            auto *textEdit = new QTextEdit(this);
            textEdit->setHtml(html);
            printNote(textEdit);
        } else if (selectedItem == exportTextAction) {
            // export the selected text as PDF
            auto *textEdit = new QOwnNotesMarkdownTextEdit(this);
            textEdit->setPlainText(selectedNoteTextEditText());
            exportNoteAsPDF(textEdit);
        } else if (selectedItem == exportHTMLAction) {
            // export the selected text (preview) as PDF
            QString html = currentNote.textToMarkdownHtml(
                selectedNoteTextEditText(), NoteFolder::currentLocalPath(),
                getMaxImageWidth(),
                Utils::Misc::useInternalExportStylingForPreview());
            html = Utils::Misc::parseTaskList(html, false);
            auto *textEdit = new QTextEdit(this);
            textEdit->setHtml(html);
            exportNoteAsPDF(textEdit);
        } else if (selectedItem == copyCodeBlockAction) {
            // copy the text from a copy block around currentTextBlock to the
            // clipboard
            Utils::Gui::copyCodeBlockText(currentTextBlock);
        }
    }
}

/**
 * Checks if text in a note is selected
 *
 * @return
 */
bool MainWindow::isNoteTextSelected() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    const QString selectedText =
        textEdit->textCursor().selectedText().trimmed();
    return !selectedText.isEmpty();
}

void MainWindow::on_actionInsert_Link_to_note_triggered() {
    // handle the linking of text with a note
    handleTextNoteLinking();
}

void MainWindow::on_action_DuplicateText_triggered() {
    activeNoteTextEdit()->duplicateText();
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
    QSettings settings;
    const QString format =
        settings.value(QStringLiteral("insertTimeFormat")).toString();
    const QString text = format.isEmpty()
                             ? dateTime.toString(Qt::SystemLocaleShortDate)
                             : dateTime.toString(format);

    // insert the current date
    c.insertText(text);
}

void MainWindow::on_actionOpen_List_triggered() { openTodoDialog(); }

/**
 * @brief Exports the current note as PDF (markdown)
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
    QString html = currentNote.toMarkdownHtml(
        NoteFolder::currentLocalPath(), getMaxImageWidth(),
        Utils::Misc::useInternalExportStylingForPreview(), decrypt);
    html = Utils::Misc::parseTaskList(html, false);

    auto doc = ui->noteTextView->document()->clone();
    doc->setHtml(html);

    return doc;
}

/**
 * @brief Prints the current note (markdown)
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
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    printNote(textEdit);
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

            insertNoteText(QStringLiteral("![") + title + QStringLiteral("](") +
                           pathOrUrl + QStringLiteral(")"));
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
    QString text =
        currentNote.getInsertMediaMarkdown(file, true, false, std::move(title));

    if (!text.isEmpty()) {
        ScriptingService *scriptingService = ScriptingService::instance();
        // attempts to ask a script for another markdown text
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
    if (currentNoteLineNumber() == 1) {
        c.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
        textEdit->setTextCursor(c);
    }

    // insert the image link
    c.insertText(text);
}

/**
 * Inserts a file attachment into the current note
 */
bool MainWindow::insertAttachment(QFile *file, const QString &title) {
    QString text = currentNote.getInsertAttachmentMarkdown(file, title);

    if (!text.isEmpty()) {
        ScriptingService *scriptingService = ScriptingService::instance();
        // attempts to ask a script for another markdown text
        text = scriptingService->callInsertAttachmentHook(file, text);
        qDebug() << __func__ << " - 'text': " << text;

        QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
        QTextCursor c = textEdit->textCursor();

        // if we try to insert the attachment in the first line of the note
        // (aka. note name) move the cursor to the last line
        if (currentNoteLineNumber() == 1) {
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
 * Returns the cursor's line number in the current note
 */
int MainWindow::currentNoteLineNumber() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    const QTextCursor cursor = textEdit->textCursor();

    QTextDocument *doc = textEdit->document();
    QTextBlock blk = doc->findBlock(cursor.position());
    QTextBlock blk2 = doc->begin();

    int i = 1;
    while (blk != blk2) {
        blk2 = blk2.next();
        ++i;
    }

    return i;
}

/**
 * @brief Opens a browser with the changelog page
 */
void MainWindow::on_actionShow_changelog_triggered() {
    QDesktopServices::openUrl(
        QUrl(QStringLiteral("https://www.qownnotes.org/changelog.html")));
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

    // return if there the note text is already encrypted
    if (currentNote.hasEncryptedNoteText()) {
        return;
    }

    // the password dialog can be disabled by scripts
    const bool dialogDisabled =
        qApp->property("encryptionPasswordDisabled").toBool();

    if (!dialogDisabled) {
        const QString labelText = tr(
            "Please enter your <strong>password</strong> to encrypt the note."
            "<br />Keep in mind that you have to <strong>remember</strong> "
            "your password to read the content of the note<br /> and that you "
            "can <strong>only</strong> do that <strong>in QOwnNotes</strong>!");
        auto *dialog = new PasswordDialog(this, labelText, true);
        const int dialogResult = dialog->exec();

        // if the user didn't pressed ok return
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
void MainWindow::updateEncryptNoteButtons() {
    currentNote.refetch();
    const bool hasEncryptedNoteText = currentNote.hasEncryptedNoteText();

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
            tr("Your note will be decrypted and stored as plain text again. "
               "Keep in mind that the unencrypted note will possibly be "
               "synced to your server and sensitive text may be exposed!"
               "<br />Do you want to decrypt your note?"),
            tr("&Decrypt"), tr("&Cancel"), QString(), 0, 1) == 1) {
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
void MainWindow::on_actionEdit_encrypted_note_triggered() {
    currentNote.refetch();
    if (!currentNote.hasEncryptedNoteText()) {
        return;
    }

    askForEncryptedNotePasswordIfNeeded(
        tr("<br />You will be able to edit your encrypted note."));

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
    QSettings settings;
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

        qDebug() << __func__
                 << " - 'externalEditorPath': " << externalEditorPath;
        qDebug() << __func__ << " - 'path': " << path;

        // open note file in external editor
        Utils::Misc::openFilesWithApplication(externalEditorPath,
                                              QStringList({path}));
    }
}

/**
 * Exports the current note as markdown file
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
            Utils::Misc::openFolderSelect(fileName);
        }
    }
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("dialog/") +
                                                   objectName());
}

void MainWindow::on_actionGet_invloved_triggered() {
    QDesktopServices::openUrl(
        QUrl("https://www.qownnotes.org/contributing/get-involved.html"));
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

    showStatusBarMessage(
        tr("Bookmarked note position at slot %1").arg(QString::number(slot)),
        3000);
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

        showStatusBarMessage(tr("Jumped to bookmark position at slot %1")
                                 .arg(QString::number(slot)),
                             3000);
    }
}

/**
 * Inserts a code block at the current cursor position
 */
void MainWindow::on_actionInsert_code_block_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    QString selectedText = c.selection().toPlainText();

    if (selectedText.isEmpty()) {
        // insert multi-line code block if cursor is in an empty line
        if (c.atBlockStart() && c.atBlockEnd()) {
            c.insertText(QStringLiteral("```\n\n```"));
            c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor, 3);
        } else {
            c.insertText(QStringLiteral("``"));
        }

        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
        textEdit->setTextCursor(c);
    } else {
        bool addNewline = false;

        // if the selected text has multiple lines add a multi-line code block
        if (selectedText.contains(QStringLiteral("\n"))) {
            // add another newline if there is no newline at the end of the
            // selected text
            const QString endNewline =
                selectedText.endsWith(QLatin1String("\n"))
                    ? QString()
                    : QStringLiteral("\n");

            selectedText = QStringLiteral("``\n") + selectedText + endNewline +
                           QStringLiteral("``");
            addNewline = true;
        }

        c.insertText(QStringLiteral("`") + selectedText + QStringLiteral("`"));

        if (addNewline) {
            c.insertText(QStringLiteral("\n"));
        }
    }
}

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
    auto *event = new QContextMenuEvent(QContextMenuEvent::Keyboard, QPoint());
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
    toggleDistractionFreeMode();
}

/**
 * Tracks an action
 */
void MainWindow::trackAction(QAction *action) {
    if (action == Q_NULLPTR) {
        return;
    }

    MetricsService::instance()->sendVisitIfEnabled(QStringLiteral("action/") +
                                                   action->objectName());
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    resizeTagTreeWidgetColumnToContents();
    resizeNoteSubFolderTreeWidgetColumnToContents();
    QMainWindow::resizeEvent(event);
}

void MainWindow::resizeNoteSubFolderTreeWidgetColumnToContents() const {
    ui->noteSubFolderTreeWidget->resizeColumnToContents(0);
    ui->noteSubFolderTreeWidget->resizeColumnToContents(1);
}

void MainWindow::resizeTagTreeWidgetColumnToContents() const {
    ui->tagTreeWidget->resizeColumnToContents(0);
    ui->tagTreeWidget->resizeColumnToContents(1);
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
    QSettings settings;
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
        ScriptingService *scriptingService = ScriptingService::instance();
        QString text =
            scriptingService->callInsertingFromMimeDataHook(mimeData);

        if (!text.isEmpty()) {
            QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
            QTextCursor c = textEdit->textCursor();

            // insert text from QML
            c.insertText(text);

            return;
        }
    }

    if (mimeData->hasHtml()) {
        insertHtmlAsMarkdownIntoCurrentNote(mimeData->html());
    } else if (mimeData->hasUrls()) {
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

                // only allow markdown and text files to be copied as note
                if (isValidNoteFile(file)) {
                    const NoteSubFolder noteSubFolder =
                        NoteSubFolder::activeNoteSubFolder();
                    const QString noteSubFolderPath = noteSubFolder.fullPath();

                    // copy file to notes path
                    const bool success =
                        file->copy(noteSubFolderPath + QDir::separator() +
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
                    showStatusBarMessage(tr("Inserting attachment"));

                    // inserting the attachment
                    insertAttachment(file);

                    showStatusBarMessage(tr("Done inserting attachment"), 3000);
                }

                delete file;
            } else {
                skipCount++;
            }
        }

        QString message;
        if (successCount > 0) {
            message +=
                tr("Copied %n note(s) to %1", "", successCount).arg(notesPath);
        }

        if (failureCount > 0) {
            if (!message.isEmpty()) {
                message += QStringLiteral(", ");
            }

            message +=
                tr("Failed to copy %n note(s) (most likely already existing)",
                   "", failureCount);
        }

        if (skipCount > 0) {
            if (!message.isEmpty()) {
                message += QStringLiteral(", ");
            }

            message +=
                tr("Skipped copying of %n note(s) "
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
                QStringLiteral("qownnotes-media-XXXXXX.png"));

            if (tempFile.open()) {
                // save temporary png image
                image.save(tempFile.fileName(), "PNG");

                // insert media into note
                auto *file = new QFile(tempFile.fileName());

                showStatusBarMessage(tr("Inserting image"));
                insertMedia(file);
                delete file;

                showStatusBarMessage(tr("Done inserting image"), 3000);
            } else {
                showStatusBarMessage(tr("Temporary file can't be opened"),
                                     3000);
            }
        }
    }
}

/**
 * Inserts html as markdown in the current note
 * Images are also downloaded
 */
void MainWindow::insertHtmlAsMarkdownIntoCurrentNote(QString html) {
    // convert html tags to markdown
    html = Utils::Misc::htmlToMarkdown(std::move(html));

    // match image tags
    QRegularExpression re(QStringLiteral("<img.+?src=[\"'](.+?)[\"'].*?>"),
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(html);

    // find, download locally and replace all images
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        const QString imageTag = match.captured(0);
        const QString imageUrlText = match.captured(1).trimmed();
        QString markdownCode;

        // check if image is an inline image
        if (imageUrlText.startsWith(QLatin1String("data:image/"),
                                    Qt::CaseInsensitive)) {
            QStringList parts = imageUrlText.split(QStringLiteral(";base64,"));
            if (parts.count() == 2) {
                markdownCode = currentNote.importMediaFromBase64(parts[1]);
            }
        } else {
            const QUrl imageUrl = QUrl(imageUrlText);

            qDebug() << __func__ << " - 'imageUrl': " << imageUrl;

            if (!imageUrl.isValid()) {
                continue;
            }

            showStatusBarMessage(tr("Downloading %1").arg(imageUrl.toString()));

            // download the image and get the media markdown code for it
            markdownCode = currentNote.downloadUrlToMedia(imageUrl);
        }

        if (!markdownCode.isEmpty()) {
            // replace the image tag with markdown code
            html.replace(imageTag, markdownCode);
        }
    }

    showStatusBarMessage(tr("Downloading images finished"), 3000);

    // remove all html tags
    html.remove(QRegularExpression(QStringLiteral("<.+?>")));

    // unescape some html special characters
    html = Utils::Misc::unescapeHtml(std::move(html)).trimmed();

    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    c.insertText(html);
}

void MainWindow::resetBrokenTagNotesLinkFlag() {
    if (_brokenTagNoteLinksRemoved) _brokenTagNoteLinksRemoved = false;
}

/**
 * Evaluates if file is a media file
 */
bool MainWindow::isValidMediaFile(QFile *file) {
    const QStringList mediaExtensions = QStringList({"jpg", "png", "gif"});
    const QFileInfo fileInfo(file->fileName());
    const QString extension = fileInfo.suffix();
    return mediaExtensions.contains(extension, Qt::CaseInsensitive);
}

/**
 * Evaluates if file is a note file
 */
bool MainWindow::isValidNoteFile(QFile *file) {
    QStringList mediaExtensions = QStringList({"txt", "md"});

    // append the custom extensions
    mediaExtensions.append(Note::customNoteFileExtensionList());

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
    const QString selectedText = c.selectedText();

    // first try to undo an existing formatting
    if (undoFormatting(formatter)) {
        return;
    }

    if (selectedText.isEmpty()) {
        c.insertText(formatter.repeated(2));
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor,
                       formatter.length());
        textEdit->setTextCursor(c);
    } else {
        QRegularExpressionMatch match =
            QRegularExpression(QStringLiteral(R"(^(\s*)(.+?)(\s*)$)"))
                .match(selectedText);
        if (match.hasMatch()) {
            c.insertText(match.captured(1) + formatter + match.captured(2) +
                         formatter + match.captured(3));
        }
    }
}

/**
 * Inserts a bold block at the current cursor position
 */
void MainWindow::on_actionFormat_text_bold_triggered() {
    applyFormatter(QStringLiteral("**"));
}

/**
 * Inserts a underline block at the current cursor position
 */
void MainWindow::on_actionFormat_text_underline_triggered() {
    applyFormatter(QStringLiteral("__"));
}

/**
 * Inserts an italic block at the current cursor position
 */
void MainWindow::on_actionFormat_text_italic_triggered() {
    applyFormatter(QStringLiteral("*"));
}

/**
 * Increases the note text font size by one
 */
void MainWindow::on_action_Increase_note_text_size_triggered() {
    const int fontSize =
        ui->noteTextEdit->modifyFontSize(QOwnNotesMarkdownTextEdit::Increase);
    ui->encryptedNoteTextEdit->setStyles();

    if (isInDistractionFreeMode()) {
        ui->noteTextEdit->setPaperMargins();
        ui->encryptedNoteTextEdit->setPaperMargins();
    }

    showStatusBarMessage(tr("Increased font size to %1 pt").arg(fontSize),
                         3000);
}

/**
 * Decreases the note text font size by one
 */
void MainWindow::on_action_Decrease_note_text_size_triggered() {
    const int fontSize =
        ui->noteTextEdit->modifyFontSize(QOwnNotesMarkdownTextEdit::Decrease);
    ui->encryptedNoteTextEdit->setStyles();

    if (isInDistractionFreeMode()) {
        ui->noteTextEdit->setPaperMargins();
        ui->encryptedNoteTextEdit->setPaperMargins();
    }

    showStatusBarMessage(tr("Decreased font size to %1 pt").arg(fontSize),
                         3000);
}

/**
 * Resets the note text font size
 */
void MainWindow::on_action_Reset_note_text_size_triggered() {
    const int fontSize =
        ui->noteTextEdit->modifyFontSize(QOwnNotesMarkdownTextEdit::Reset);
    ui->encryptedNoteTextEdit->setStyles();
    showStatusBarMessage(tr("Reset font size to %1 pt",
                            "Will be shown after "
                            "the font size is reset by 'Reset note text size'")
                             .arg(fontSize),
                         3000);
}

/**
 * Sets the note folder from the recent note folder combobox
 */
void MainWindow::on_noteFolderComboBox_currentIndexChanged(int index) {
    const int noteFolderId = ui->noteFolderComboBox->itemData(index).toInt();
    const NoteFolder noteFolder = NoteFolder::fetch(noteFolderId);

    if (noteFolder.isFetched()) {
        changeNoteFolder(noteFolderId);
        resetBrokenTagNotesLinkFlag();
    }

    // hide the noteSubFolderDockWidget menu entry if subfolders are
    // not enabled
    QAction *action =
        findAction(QStringLiteral("togglePanel-noteSubFolderDockWidget"));

    if (action != Q_NULLPTR) {
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

    QSettings settings;

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
            noteSubFolderIds
                    << NoteSubFolder::fetchIdsRecursivelyByParentId(id);
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
    // get the notes from the subfolders
    for (int noteSubFolderId : Utils::asConst(noteSubFolderIds)) {
        // get all notes of a note sub folder
        untaggedNoteCount += Note::countAllNotTagged(noteSubFolderId);
        noteIdList << Note::fetchAllIdsByNoteSubFolderId(noteSubFolderId);
    }

    // create an item to view all notes
    int linkCount =
        _showNotesFromAllNoteSubFolders ? Note::countAll() : noteIdList.count();
    QString toolTip = tr("show all notes (%1)").arg(QString::number(linkCount));

    auto *allItem = new QTreeWidgetItem();
    allItem->setText(0, tr("All notes"));
    allItem->setForeground(1, QColor(Qt::gray));
    allItem->setText(1, QString::number(linkCount));
    allItem->setToolTip(0, toolTip);
    allItem->setToolTip(1, toolTip);
    allItem->setData(0, Qt::UserRole, Tag::AllNotesId);
    allItem->setFlags(allItem->flags() & ~Qt::ItemIsSelectable);
    allItem->setIcon(
        0,
        QIcon::fromTheme(QStringLiteral("edit-copy"),
                         QIcon(QStringLiteral(
                             ":icons/breeze-qownnotes/16x16/edit-copy.svg"))));
    // this time, the tags come first
    buildTagTreeForParentItem();
    // and get sorted
    if (settings.value(QStringLiteral("tagsPanelSort")).toInt() ==
        SORT_ALPHABETICAL) {
        ui->tagTreeWidget->sortItems(
            0, toQtOrder(
                   settings.value(QStringLiteral("tagsPanelOrder")).toInt()));
    }
    // now add 'All notes' to the top
    ui->tagTreeWidget->insertTopLevelItem(0, allItem);

    // add an item to view untagged notes if there are any
    linkCount = _showNotesFromAllNoteSubFolders ? Note::countAllNotTagged()
                                                : untaggedNoteCount;

    if (linkCount > 0) {
        toolTip =
            tr("show all untagged notes (%1)").arg(QString::number(linkCount));
        auto *untaggedItem = new QTreeWidgetItem();
        untaggedItem->setText(0, tr("Untagged notes"));
        untaggedItem->setForeground(1, QColor(Qt::gray));
        untaggedItem->setText(1, QString::number(linkCount));
        untaggedItem->setToolTip(0, toolTip);
        untaggedItem->setToolTip(1, toolTip);
        untaggedItem->setData(0, Qt::UserRole, Tag::AllUntaggedNotesId);
        untaggedItem->setFlags(untaggedItem->flags() & ~Qt::ItemIsSelectable);
        untaggedItem->setIcon(
            0, QIcon::fromTheme(
                   QStringLiteral("edit-copy"),
                   QIcon(QStringLiteral(
                       ":icons/breeze-qownnotes/16x16/edit-copy.svg"))));
        ui->tagTreeWidget->addTopLevelItem(untaggedItem);
    }

    // decorate root if there are multiple levels to be able to collapse them,
    // because double clicking will not collapse the first level, but edit
    // the clicked tag
    ui->tagTreeWidget->setRootIsDecorated(Tag::countAllParentId(0) !=
                                          Tag::countAll());

    resizeTagTreeWidgetColumnToContents();
    highlightCurrentNoteTagsInTagTree();
}

/**
 * Reloads the note sub folder tree
 */
void MainWindow::reloadNoteSubFolderTree() {
    qDebug() << __func__;
    ui->noteSubFolderTreeWidget->clear();
    const int activeNoteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    const bool showAllNotesItem =
        !NoteSubFolder::isNoteSubfoldersPanelShowNotesRecursively();
    auto *allItem = new QTreeWidgetItem();

    // add the "all notes" item
    if (showAllNotesItem) {
        int linkCount = Note::countAll();
        QString toolTip = tr("show notes from all note subfolders (%1)")
                              .arg(QString::number(linkCount));

        allItem->setText(0, tr("All notes"));
        allItem->setData(0, Qt::UserRole, -1);
        allItem->setToolTip(0, toolTip);
        allItem->setIcon(
            0, QIcon::fromTheme(
                   QStringLiteral("edit-copy"),
                   QIcon(QStringLiteral(
                       ":icons/breeze-qownnotes/16x16/edit-copy.svg"))));
        allItem->setForeground(1, QColor(Qt::gray));
        allItem->setText(1, QString::number(linkCount));
        allItem->setToolTip(1, toolTip);
        allItem->setFlags(allItem->flags() & ~Qt::ItemIsSelectable);
    }

    // add the "note folder" item
    QSettings settings;
    const int linkCount = Note::countByNoteSubFolderId(
        0, settings
               .value(QStringLiteral("noteSubfoldersPanelShowNotesRecursively"))
               .toBool());
    const QString toolTip = tr("show notes in note root folder (%1)")
                                .arg(QString::number(linkCount));

    auto *item = new QTreeWidgetItem();
    if (settings.value(QStringLiteral("noteSubfoldersPanelShowRootFolderName"))
            .toBool()) {
        item->setText(
            0, NoteFolder::currentRootFolderName(
                   settings
                       .value(QStringLiteral("noteSubfoldersPanelShowFullPath"))
                       .toBool()));
    } else {
        item->setText(0, tr("Note folder"));
    }
    item->setData(0, Qt::UserRole, 0);
    item->setToolTip(0, toolTip);
    item->setIcon(0, _folderIcon);
    item->setForeground(1, QColor(Qt::gray));
    item->setText(1, QString::number(linkCount));
    item->setToolTip(1, toolTip);

    if (settings.value(QStringLiteral("noteSubfoldersPanelDisplayAsFullTree"))
            .toBool()) {
        if (showAllNotesItem) {
            // add 'All Notes'
            ui->noteSubFolderTreeWidget->addTopLevelItem(allItem);
        }

        // add note root folder
        ui->noteSubFolderTreeWidget->addTopLevelItem(item);

        // add all note sub folders recursively as items
        buildNoteSubFolderTreeForParentItem(item);

        if (item->childCount() > 0) {
            item->setExpanded(true);

            if (settings.value(QStringLiteral("noteSubfoldersPanelSort"))
                    .toInt() == SORT_ALPHABETICAL) {
                item->sortChildren(
                    0, toQtOrder(settings
                                     .value(QStringLiteral(
                                         "noteSubfoldersPanelOrder"))
                                     .toInt()));
            }
        }
    } else {    // the less hierarchical view
        // add root note folder first
        ui->noteSubFolderTreeWidget->addTopLevelItem(item);
        // add subfolders recursively
        buildNoteSubFolderTreeForParentItem();

        // sort the widget
        if (settings.value(QStringLiteral("noteSubfoldersPanelSort")).toInt() ==
            SORT_ALPHABETICAL) {
            ui->noteSubFolderTreeWidget->sortItems(
                0,
                toQtOrder(
                    settings.value(QStringLiteral("noteSubfoldersPanelOrder"))
                        .toInt()));
        }

        if (showAllNotesItem) {
            // finally add 'All Notes' to the top
            ui->noteSubFolderTreeWidget->insertTopLevelItem(0, allItem);
        }
    }

    // set the active item
    if (activeNoteSubFolderId == 0) {
        const QSignalBlocker blocker(ui->noteSubFolderTreeWidget);
        Q_UNUSED(blocker)

        ui->noteSubFolderTreeWidget->setCurrentItem(item);
    }

    resizeNoteSubFolderTreeWidgetColumnToContents();

    // send an event to jump to "All notes" in the note subfolder tree widget
    // if that item was last selected
    if ((activeNoteSubFolderId == 0) && _showNotesFromAllNoteSubFolders) {
        selectAllNotesInNoteSubFolderTreeWidget();
    }
}

/**
 * Populates the note sub folder tree recursively with its items
 */
void MainWindow::buildNoteSubFolderTreeForParentItem(QTreeWidgetItem *parent) {
    const int parentId =
        parent == nullptr ? 0 : parent->data(0, Qt::UserRole).toInt();
    const int activeNoteSubFolderId = NoteSubFolder::activeNoteSubFolderId();
    const bool isCurrentNoteTreeEnabled =
        NoteFolder::isCurrentNoteTreeEnabled();

    const auto noteSubFolderList = NoteSubFolder::fetchAllByParentId(parentId);

    // build the next level of note sub folders
    for (const auto &noteSubFolder : noteSubFolderList) {
        QTreeWidgetItem *item =
            addNoteSubFolderToTreeWidget(parent, noteSubFolder);

        if (isCurrentNoteTreeEnabled) {
            // load all notes of the subfolder and add them to the note list
            // widget
            const QVector<Note> noteList =
                Note::fetchAllByNoteSubFolderId(noteSubFolder.getId());
            for (const auto &note : noteList) {
                addNoteToNoteTreeWidget(note, item);
            }
        } else {
            // set the active item
            if (activeNoteSubFolderId == noteSubFolder.getId()) {
                const QSignalBlocker blocker(ui->noteSubFolderTreeWidget);
                Q_UNUSED(blocker)

                ui->noteSubFolderTreeWidget->setCurrentItem(item);
            }
        }

        buildNoteSubFolderTreeForParentItem(item);

        // set the expanded state
        const bool isExpanded = noteSubFolder.treeWidgetExpandState();
        item->setExpanded(isExpanded);

        // sort alphabetically, if necessary
        QSettings settings;
        const int sort =
            settings.value(QStringLiteral("noteSubfoldersPanelSort")).toInt();
        if (sort == SORT_ALPHABETICAL) {
            item->sortChildren(
                0,
                toQtOrder(
                    settings.value(QStringLiteral("noteSubfoldersPanelOrder"))
                        .toInt()));
        }
    }

    // add the notes of the note folder root
    if (parentId == 0 && isCurrentNoteTreeEnabled) {
        const QVector<Note> noteList =
            Note::fetchAllByNoteSubFolderId(0);
        for (const auto &note : noteList) {
            addNoteToNoteTreeWidget(note, parent);
        }
    }
}

/**
 * Populates the tag tree recursively with its tags
 */
void MainWindow::buildTagTreeForParentItem(QTreeWidgetItem *parent,
                                           bool topLevel) {
    const int parentId = (parent == nullptr || topLevel)
                             ? 0
                             : parent->data(0, Qt::UserRole).toInt();
    const int activeTagId = Tag::activeTagId();
    QSettings settings;
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
            item->sortChildren(0, toQtOrder(tagPanelOrder));
        }
    }

    // update the UI
    // this will crash the app sporadically
    // QCoreApplication::processEvents();
}

/**
 * Ads a tag to the tag tree widget
 */
QTreeWidgetItem *MainWindow::addTagToTagTreeWidget(QTreeWidgetItem *parent,
                                                   const TagHeader &tag) {
    const int parentId =
        parent == nullptr ? 0 : parent->data(0, Qt::UserRole).toInt();
    const int tagId = tag._id;
    const QString name = tag._name;
    auto hideCount = QSettings().value("tagsPanelHideNoteCount", false).toBool();

    QVector<int> linkedNoteIds;
    if (!hideCount) {
        const QVector<int> tagIdListToCount = Tag::isTaggingShowNotesRecursively() ?
                    Tag::fetchTagIdsRecursivelyByParentId(tagId) : QVector<int>{tag._id};
        const auto selectedSubFolderItems =
                ui->noteSubFolderTreeWidget->selectedItems();
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

                    linkedNoteIds << Tag::fetchAllLinkedNoteIdsForFolder(
                                         tagIdToCount,
                                         folder, showNotesFromAllSubFolders,
                                         isShowNotesRecursively);
                }
            }
        } else {
            linkedNoteIds.reserve(tagIdListToCount.size());
            for (const int tagToCount : tagIdListToCount) {
                linkedNoteIds << Tag::fetchAllLinkedNoteIds(
                                     tagToCount,
                                     showNotesFromAllSubFolders,
                                     isShowNotesRecursively);
            }
        }

        // remove duplicate note ids
        linkedNoteIds.erase(
                    std::unique(linkedNoteIds.begin(), linkedNoteIds.end()),
                    linkedNoteIds.end());
    }

    const int linkCount = linkedNoteIds.count();
    const QString toolTip = tr("show all notes tagged with '%1' (%2)")
                                .arg(name, QString::number(linkCount));
    auto *item = new QTreeWidgetItem();
    item->setData(0, Qt::UserRole, tagId);
    item->setText(0, name);
    item->setText(1, linkCount > 0 ? QString::number(linkCount) : QString());
    item->setForeground(1, QColor(Qt::gray));
    item->setIcon(0, _tagIcon);
    item->setToolTip(0, toolTip);
    item->setToolTip(1, toolTip);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    // set the color of the tag tree widget item
    handleTreeWidgetItemTagColor(item, tagId);

    if (parentId == 0) {
        // add the item at top level if there was no parent item
        ui->tagTreeWidget->addTopLevelItem(item);
    } else {
        // add the item as child of the parent
        parent->addChild(item);
    }

    return item;
}

void MainWindow::handleTreeWidgetItemTagColor(QTreeWidgetItem *item, int tagId)
{
    const Tag tag = Tag::fetch(tagId);
    if (!tag.isFetched())
        return;
    handleTreeWidgetItemTagColor(item, tag);
}

/**
 * Reads the color from a tag and sets the background color of a tree widget
 * item
 *
 * @param item
 * @param tag
 */
void MainWindow::handleTreeWidgetItemTagColor(QTreeWidgetItem *item,
                                              const Tag &tag) {
    if (item == Q_NULLPTR) {
        return;
    }

    const int columnCount = item->columnCount();

    if (columnCount == 0) {
        return;
    }

    // get the color from the tag
    QColor color = tag.getColor();

    // if no color was set reset it by using a transparent white
    if (!color.isValid()) {
        color = Qt::transparent;
    }

    QBrush brush = QBrush(color);

    // the tree widget events have to be blocked because when called in
    // assignColorToTagItem() the 2nd setBackground() crashes the app,
    // because it seems the tag tree will be reloaded
    const QSignalBlocker blocker(item->treeWidget());
    Q_UNUSED(blocker)

    // set the color for all columns
    for (int column = 0; column < columnCount; ++column) {
        item->setBackground(column, brush);
    }
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
    Utils::Gui::searchForTextInTreeWidget(
        ui->tagTreeWidget, arg1, Utils::Gui::TreeWidgetSearchFlag::IntCheck);
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

    QSettings settings;
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
void MainWindow::linkTagNameToCurrentNote(const QString &tagName,
                                          bool linkToSelectedNotes) {
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

/**
 * Adds or removes a tag from the note text if defined via scripting engine
 *
 * @param note
 * @param tagName
 * @param doRemove
 * @param triggerPostMethods
 */
void MainWindow::handleScriptingNoteTagging(Note note, const Tag &tag,
                                            bool doRemove,
                                            bool triggerPostMethods) {
    const QString oldNoteText = note.getNoteText();
    const QString &action =
        doRemove ? QStringLiteral("remove") : QStringLiteral("add");
    QString noteText =
        ScriptingService::instance()
            ->callNoteTaggingHook(
                note, action,
                tag.getName())
            .toString();

    // try noteTaggingByObjectHook if noteTaggingHook didn't do anything
    if (noteText.isEmpty()) {
        noteText =
            ScriptingService::instance()
                ->callNoteTaggingByObjectHook(note, action, tag).toString();

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
        const QStringList tagNameList =
            ScriptingService::instance()
                ->callNoteTaggingHook(note, QStringLiteral("list"))
                .toStringList();

        if (tagNameList.count() == 0) {
            // if callNoteTaggingHook didn't return anything lets try
            // callNoteTaggingByObjectHook
            const auto variantTagIdList = ScriptingService::instance()
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
void MainWindow::handleScriptingNotesTagRenaming(const Tag &tag,
                                                 const QString &newTagName) {
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
                ->callNoteTaggingHook(note, QStringLiteral("rename"),
                                      tag.getName(), newTagName)
                .toString();

        // if nothing came back from callNoteTaggingHook let's try
        // callNoteTaggingByObjectHook
        if (noteText.isEmpty()) {
            noteText =
                ScriptingService::instance()
                    ->callNoteTaggingByObjectHook(
                        note, QStringLiteral("rename"), tag, newTagName)
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
void MainWindow::handleScriptingNotesTagRemoving(const Tag &tag,
                                                 bool forBulkOperation) {
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
    ui->newNoteTagButton->setToolTip(
        currentNoteOnly ? tr("Add a tag to the current note")
                        : tr("Add a tag to the selected notes"));
    QVector<TagHeader> tagList;

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
        const QString notesSelectedText =
            tr("%n notes selected", "", selectedNotesCount);

        ui->selectedTagsToolButton->setText(
            QString::number(selectedNotesCount));
        ui->selectedTagsToolButton->setToolTip(notesSelectedText);

        ui->notesSelectedLabel->setText(notesSelectedText);

        // overwrite the note preview with a preview of the selected notes
        const QString previewHtml =
            Note::generateMultipleNotesPreviewText(notes);
        ui->noteTextView->setText(previewHtml);
    }

    _lastNoteSelectionWasMultiple = !currentNoteOnly;

    // add all new remove-tag buttons
    for (const TagHeader &tag : Utils::asConst(tagList)) {
        QPushButton *button = new QPushButton(
            Utils::Misc::shorten(tag._name, 25), ui->noteTagButtonFrame);
        button->setIcon(QIcon::fromTheme(
            QStringLiteral("tag-delete"),
            QIcon(QStringLiteral(
                ":icons/breeze-qownnotes/16x16/xml-attribute-delete.svg"))));
        button->setToolTip(
            currentNoteOnly
                ? tr("Remove tag '%1' from the current note").arg(tag._name)
                : tr("Remove tag '%1' from the selected notes")
                      .arg(tag._name));
        button->setObjectName(QStringLiteral("removeNoteTag") +
                              QString::number(tag._id));

        QObject::connect(button, &QPushButton::clicked, this,
                         &MainWindow::removeNoteTagClicked);

        ui->noteTagButtonFrame->layout()->addWidget(button);
    }

    //    // find tags not in common of selected notes
    //    if (selectedNotesCount > 1) {
    //        QLabel *noteTagButtonFrame = new QLabel("+3 tags");
    //        ui->noteTagButtonFrame->layout()->addWidget(noteTagButtonFrame);
    //    }

    // add a spacer to prevent the button items to take the full width
    auto *spacer = new QSpacerItem(0, 20, QSizePolicy::MinimumExpanding,
                                   QSizePolicy::Ignored);
    ui->noteTagButtonFrame->layout()->addItem(spacer);

    highlightCurrentNoteTagsInTagTree();
}

/**
 * Highlights the tags of the current note in the tag tree
 */
void MainWindow::highlightCurrentNoteTagsInTagTree() {
    const int selectedNotesCount = getSelectedNotesCount();
    const bool currentNoteOnly = selectedNotesCount <= 1;
    QVector<TagHeader> tagList;

    if (currentNoteOnly) {
        tagList = Tag::fetchAllOfNote(currentNote);
    } else {
        const QVector<Note> &notes = selectedNotes();
        tagList = Tag::fetchAllOfNotes(notes);
    }

    const QSignalBlocker blocker1(ui->tagTreeWidget);
    Q_UNUSED(blocker1)

    Utils::Gui::resetBoldStateOfAllTreeWidgetItems(ui->tagTreeWidget);

    for (const TagHeader &tag : Utils::asConst(tagList)) {
        QTreeWidgetItem *item = Utils::Gui::getTreeWidgetItemWithUserData(
            ui->tagTreeWidget, tag._id);

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
        const int tagId =
            objectName.remove(QLatin1String("removeNoteTag")).toInt();
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
void MainWindow::on_tagTreeWidget_itemChanged(QTreeWidgetItem *item,
                                              int column) {
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
void MainWindow::on_tagTreeWidget_currentItemChanged(
    QTreeWidgetItem *current, QTreeWidgetItem *previous) {
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
 * Triggers filtering when multiple tags are selected
 */
void MainWindow::on_tagTreeWidget_itemSelectionChanged() {
    const int count = ui->tagTreeWidget->selectedItems().count();

    if (count <= 1) {
        if (count == 1) {
            //           on_tagTreeWidget_currentItemChanged(ui->tagTreeWidget->selectedItems().first(),
            //                                                Q_NULLPTR);
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
                    showStatusBarMessage(tr("Tag could not be created!"), 3000);
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
    const int tagId = item->data(0, Qt::UserRole).toInt();
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
void MainWindow::buildTagMoveMenuTree(QMenu *parentMenu, int parentTagId) {
    const auto tagList =
        Tag::fetchAllByParentId(parentTagId, QStringLiteral("t.name ASC"));

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
            ? tr("Move to the root",
                 "to move a tag to the current tag in the tag context menu")
            : tr("Move to this tag"));
    action->setData(parentTagId);

    connect(action, &QAction::triggered, this,
            [this, parentTagId]() { moveSelectedTagsToTagId(parentTagId); });
}

/**
 * Populates a tag menu tree for bulk note tagging
 */
void MainWindow::buildBulkNoteTagMenuTree(QMenu *parentMenu, int parentTagId) {
    const auto tagList =
        Tag::fetchAllByParentId(parentTagId, QStringLiteral("t.name ASC"));

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

        connect(action, &QAction::triggered, this, [this, parentTagId]() {
            tagSelectedNotesToTagId(parentTagId);
        });
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
                showStatusBarMessage(
                    tr("Cannot move tag '%1' to this tag").arg(tag.getName()),
                    3000);
            } else {
                tagList << tag;
            }
        }
    }

    if (tagList.count() > 0) {
        const bool useScriptingEngine =
            ScriptingService::instance()->noteTaggingHookExists();

        // workaround when signal block doesn't work correctly
        directoryWatcherWorkaround(true, true);

        // move tags
        for (Tag tag : Utils::asConst(tagList)) {
            if (useScriptingEngine) {
                const QVector<Tag> tagsToHandle =
                    Tag::fetchRecursivelyByParentId(tag.getId());

                // check all tags we need to handle
                for (const Tag &tagToHandle : tagsToHandle) {
                    // remove tag from all notes
                    for (const Note &note : tagToHandle.fetchAllLinkedNotes()) {
                        handleScriptingNoteTagging(
                            note, tagToHandle, true, false);
                    }
                }
            }

            tag.setParentId(tagId);
            tag.store();

            if (useScriptingEngine) {
                const QVector<Tag> tagsToHandle =
                    Tag::fetchRecursivelyByParentId(tag.getId());

                // check all tags we need to handle
                for (const Tag &tagToHandle : tagsToHandle) {
                    // add tag to all notes
                    for (const Note &note : tagToHandle.fetchAllLinkedNotes()) {
                        handleScriptingNoteTagging(
                            note, tagToHandle, false, false);
                    }
                }
            }

            showStatusBarMessage(
                tr("Moved tag '%1' to new tag").arg(tag.getName()), 3000);
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
        NoteSubFolder::fetchAllByParentId(parentNoteSubFolderId,
                                          QStringLiteral("name ASC"));

    for (const auto &noteSubFolder : noteSubFolderList) {
        const int noteSubFolderId = noteSubFolder.getId();
        const QString name = noteSubFolder.getName();

        const int count = NoteSubFolder::countAllParentId(noteSubFolderId);
        if (count > 0) {
            // if there are sub-noteSubFolder build a new menu level
            QMenu *noteSubFolderMenu = parentMenu->addMenu(name);
            buildBulkNoteSubFolderMenuTree(noteSubFolderMenu, doCopy,
                                           noteSubFolderId);
        } else {
            // if there are no sub-noteSubFolders just create a named action
            QAction *action = parentMenu->addAction(name);

            connect(
                action, &QAction::triggered, this,
                [this, doCopy, noteSubFolderId]() {
                    doCopy
                        ? copySelectedNotesToNoteSubFolderId(noteSubFolderId)
                        : moveSelectedNotesToNoteSubFolderId(noteSubFolderId);
                });
        }
    }

    // add an action to copy or move to this subfolder
    parentMenu->addSeparator();
    const QString text =
        (parentNoteSubFolderId == 0)
            ? (doCopy ? tr("Copy to note folder") : tr("Move to note folder"))
            : (doCopy ? tr("Copy to this subfolder")
                      : tr("Move to this subfolder"));
    QAction *action = parentMenu->addAction(text);
    action->setData(parentNoteSubFolderId);

    connect(action, &QAction::triggered, this,
            [this, doCopy, parentNoteSubFolderId]() {
                doCopy
                    ? copySelectedNotesToNoteSubFolderId(parentNoteSubFolderId)
                    : moveSelectedNotesToNoteSubFolderId(parentNoteSubFolderId);
            });
}

/**
 * Populates a subfolder menu tree for bulk note moving or copying to
 * subfolders of other note folders
 */
void MainWindow::buildBulkNoteFolderSubFolderMenuTree(
    QMenu *parentMenu, bool doCopy, const QString &parentNoteSubFolderPath,
    bool isRoot) {
    QDir dir(parentNoteSubFolderPath);
    QStringList nameFilters{};

    if (isRoot) {
        nameFilters << QStringList({"media", "trash", "attachments"});
    }

    // show newest entry first
    QStringList directoryNames =
        dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

    if (isRoot) {
        const auto names = QStringList({"media", "trash", "attachments"});
        for (const QString &name : names) {
            directoryNames.removeAll(name);
        }
    }

    for (const QString &directoryName : Utils::asConst(directoryNames)) {
        const QString fullPath =
            parentNoteSubFolderPath + QLatin1Char('/') + directoryName;
        QDir subDir(fullPath);
        const QStringList subDirectoryNames =
            subDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        if (subDirectoryNames.count() > 0) {
            // if there are sub folders build a new menu level
            QMenu *noteSubFolderMenu = parentMenu->addMenu(directoryName);
            buildBulkNoteFolderSubFolderMenuTree(noteSubFolderMenu, doCopy,
                                                 fullPath, false);
        } else {
            // if there are no sub folders just create a named action
            QAction *action = parentMenu->addAction(directoryName);
            action->setToolTip(fullPath);
            action->setStatusTip(fullPath);

            connect(action, &QAction::triggered, this,
                    [this, doCopy, fullPath]() {
                        doCopy ? copySelectedNotesToFolder(fullPath)
                               : moveSelectedNotesToFolder(fullPath);
                    });
        }
    }

    // add an action to copy or move to this subfolder
    parentMenu->addSeparator();
    const QString text = (isRoot) ? (doCopy ? tr("Copy to note folder")
                                            : tr("Move to note folder"))
                                  : (doCopy ? tr("Copy to this subfolder")
                                            : tr("Move to this subfolder"));
    auto *action = parentMenu->addAction(text);
    action->setToolTip(parentNoteSubFolderPath);
    action->setStatusTip(parentNoteSubFolderPath);

    connect(action, &QAction::triggered, this,
            [this, doCopy, parentNoteSubFolderPath]() {
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
void MainWindow::moveSelectedNotesToNoteSubFolder(
    const NoteSubFolder &noteSubFolder) {
    const int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();
    const QString text = tr("Move %n selected note(s) to note subfolder "
                            "<strong>%2</strong>?",
                            "", selectedItemsCount)
                             .arg(noteSubFolder.getName());

    if (Utils::Gui::question(this, tr("Move selected notes"), text,
                             QStringLiteral("move-notes")) ==
        QMessageBox::Yes) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker)

        // unset the current note
        //      unsetCurrentNote();

        int noteSubFolderCount = 0;

        // disable the externally removed check, because it might trigger
        _noteExternallyRemovedCheckEnabled = false;

        const auto selectedItems = ui->noteTreeWidget->selectedItems();
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

            // fetch the tags to tag the note after moving it
            const QVector<TagHeader> tags = Tag::fetchAllOfNote(note);

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
                for (const TagHeader &tagHeader : tags) {
                    Tag::fetch(tagHeader._id).linkToNote(note);
//                    tag.linkToNote(note);
                }

                // handle the replacing of all note links from other notes
                // because the note was moved
                note.handleNoteMoving(oldNote);

                // re-link images
                const bool mediaFileLinksUpdated =
                    note.updateRelativeMediaFileLinks();

                // re-link attachments
                const bool attachmentFileLinksUpdated =
                    note.updateRelativeAttachmentFileLinks();

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
            QTimer::singleShot(150, this,
                               SLOT(buildNotesIndexAndLoadNoteDirectoryList()));
        }

        showStatusBarMessage(
            tr("%n note(s) were moved to note subfolder \"%2\"", "",
               noteSubFolderCount)
                .arg(noteSubFolder.getName()),
            5000);

        // wait some time to enable the check again to prevent troubles on macOS
        QTimer::singleShot(4000, this,
                           SLOT(enableNoteExternallyRemovedCheck()));
    }
}

/**
 * Enables the note externally removed check
 */
void MainWindow::enableNoteExternallyRemovedCheck() {
    _noteExternallyRemovedCheckEnabled = true;
}

/**
 * Copies selected notes to a note subfolder
 */
void MainWindow::copySelectedNotesToNoteSubFolder(
    const NoteSubFolder &noteSubFolder) {
    const int selectedItemsCount = ui->noteTreeWidget->selectedItems().size();
    const QString text = tr("Copy %n selected note(s) to note subfolder "
                            "<strong>%2</strong>?",
                            "", selectedItemsCount)
                             .arg(noteSubFolder.getName());

    if (Utils::Gui::question(this, tr("Copy selected notes"), text,
                             QStringLiteral("copy-notes")) ==
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

            // fetch the tags to tag the note after copying it
            const QVector<TagHeader> tags = Tag::fetchAllOfNote(note);

            // copy note
            const bool result = note.copyToPath(noteSubFolder.fullPath());
            if (result) {
                noteSubFolderCount++;
                qDebug() << "Note was copied:" << note.getName();

                // set the new subfolder so the tags are stored correctly
                note.setNoteSubFolder(noteSubFolder);

                // tag the note again
                for (const TagHeader &tag : tags) {
                    Tag::fetch(tag._id).linkToNote(note);
//                    tag.linkToNote(note);
                }

                // re-link images
                const bool mediaFileLinksUpdated =
                    note.updateRelativeMediaFileLinks();

                // re-link attachments
                const bool attachmentFileLinksUpdated =
                    note.updateRelativeAttachmentFileLinks();

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
            QTimer::singleShot(150, this,
                               SLOT(buildNotesIndexAndLoadNoteDirectoryList()));
        }

        showStatusBarMessage(
            tr("%n note(s) were copied to note subfolder \"%2\"", "",
               noteSubFolderCount)
                .arg(noteSubFolder.getName()),
            5000);
    }
}

/**
 * Returns true if one of the selected notes has a linked tag
 *
 * @return
 */
bool MainWindow::selectedNotesHaveTags() {
    const auto items = ui->noteTreeWidget->selectedItems();
    for (QTreeWidgetItem *item : items) {
        if (item->data(0, Qt::UserRole + 1) != NoteType) {
            continue;
        }

        const int noteId = item->data(0, Qt::UserRole).toInt();
        const Note note = Note::fetch(noteId);

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
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();

    // try to open a link at the cursor position
    if (textEdit->openLinkAtCursorPosition()) {
        showStatusBarMessage(
            tr("An url was opened at the current cursor position"), 5000);
        return;
    }

    // attempt a markdown table auto-format
    if (Utils::Gui::autoFormatTableAtCursor(textEdit)) {
        return;
    }

    QMenu menu;

    double resultValue;
    if (solveEquationInNoteTextEdit(resultValue)) {
        const QString text = QString::number(resultValue);
        auto *action = menu.addAction(QStringLiteral("= ") + text);
        action->setData(text);
        action->setWhatsThis(QStringLiteral("equation"));
    }

    QStringList resultList;
    if (noteTextEditAutoComplete(resultList)) {
        for (const QString &text : Utils::asConst(resultList)) {
            auto *action = menu.addAction(text);
            action->setData(text);
            action->setWhatsThis(QStringLiteral("autocomplete"));
        }
    }

    // load texts from scripts to show in the autocompletion list
    const QStringList autocompletionList =
        ScriptingService::instance()->callAutocompletionHook();
    if (!autocompletionList.isEmpty()) {
        auto *action = menu.addAction(QString());
        action->setSeparator(true);

        for (const QString &text : autocompletionList) {
            auto *newAction = menu.addAction(text);
            newAction->setData(text);
            newAction->setWhatsThis(QStringLiteral("autocomplete"));
        }
    }

    QPoint globalPos =
        textEdit->mapToGlobal(textEdit->cursorRect().bottomRight());

    // compensate viewport margins
    globalPos.setY(globalPos.y() + textEdit->viewportMargins().top());
    globalPos.setX(globalPos.x() + textEdit->viewportMargins().left());

    if (menu.actions().count() > 0) {
        QAction *selectedItem = menu.exec(globalPos);
        if (selectedItem) {
            const QString text = selectedItem->data().toString();
            const QString type = selectedItem->whatsThis();

            if (text.isEmpty()) {
                return;
            }

            if (type == QStringLiteral("autocomplete")) {
                // overwrite the currently written word
                QTextCursor c = textEdit->textCursor();
                c.movePosition(QTextCursor::StartOfWord,
                               QTextCursor::KeepAnchor);
                c.insertText(text + QStringLiteral(" "));
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
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    // get the text from the current cursor to the start of the line
    c.movePosition(QTextCursor::StartOfBlock, QTextCursor::KeepAnchor);
    QString text = c.selectedText();
    qDebug() << __func__ << " - 'text': " << text;

    QString equation = text;

    // replace "," with "." to allow "," as coma
    equation.replace(QLatin1Char(','), QLatin1Char('.'));

    // remove leading list characters
    equation.remove(QRegularExpression(QStringLiteral(R"(^\s*[\-*+] )")));

    // match all numbers and basic operations like +, -, * and /
    QRegularExpressionMatch match =
        QRegularExpression(QStringLiteral(R"(([\d\.,+\-*\/\(\)\s]+)\s*=)"))
            .match(equation);

    if (!match.hasMatch()) {
        if (equation.trimmed().endsWith(QChar('='))) {
            showStatusBarMessage(
                tr("No equation was found in front of the cursor"), 5000);
        }

        return false;
    }

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
                             .arg(equation, QString::number(resultValue)),
                         10000);

    // check if cursor is after the "="
    match = QRegularExpression(QStringLiteral("=\\s*$")).match(text);
    if (!match.hasMatch()) {
        return false;
    }

    returnValue = resultValue;
    return true;
}

/**
 * Returns the text from the current cursor to the start of the word in the
 * note text edit
 *
 * @param withPreviousCharacters also get more characters at the beginning
 *                               to get characters like "@" that are not
 *                               word-characters
 * @return
 */
QString MainWindow::noteTextEditCurrentWord(bool withPreviousCharacters) {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();

    // get the text from the current word
    c.movePosition(QTextCursor::EndOfWord);
    c.movePosition(QTextCursor::StartOfWord, QTextCursor::KeepAnchor);

    QString text = c.selectedText();

    if (withPreviousCharacters) {
        QRegularExpression re(QStringLiteral("^[\\s\\n][^\\s]*"));
        do {
            c.movePosition(QTextCursor::Left, QTextCursor::KeepAnchor);
            text = c.selectedText();
        } while (!(re.match(text).hasMatch() || c.atBlockStart()));
    }

    return text.trimmed();
}

/**
 * Tries to find words that start with the current word in the note text edit
 *
 * @param resultList
 * @return
 */
bool MainWindow::noteTextEditAutoComplete(QStringList &resultList) {
    // get the text from the current cursor to the start of the word
    const QString text = noteTextEditCurrentWord();
    qDebug() << __func__ << " - 'text': " << text;

    if (text.isEmpty()) {
        return false;
    }

    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    const QString noteText = textEdit->toPlainText();

    // find all items that match our current word
    resultList = noteText
                     .split(QRegularExpression(
                                QStringLiteral("[^\\w\\d]"),
                                QRegularExpression::UseUnicodePropertiesOption),
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
                            QString::SkipEmptyParts)
#else
                            Qt::SkipEmptyParts)
#endif
                     .filter(QRegularExpression(
                         QStringLiteral("^") + QRegularExpression::escape(text),
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
    forceRegenerateNotePreview();
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
            out.setCodec("UTF-8");
            out << currentNote.toMarkdownHtml(NoteFolder::currentLocalPath(),
                                              getMaxImageWidth(), true, true,
                                              true);
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
    QDesktopServices::openUrl(QUrl(QStringLiteral(
        "https://kiwiirc.com/client/irc.freenode.net/#qownnotes")));
}

/**
 * Opens the Gitter room in a webpage
 */
void MainWindow::on_actionGitter_triggered() {
    QDesktopServices::openUrl(
        QUrl(QStringLiteral("https://gitter.im/qownnotes/qownnotes")));
}

/**
 * Adds the current search text to the saved searches
 */
void MainWindow::storeSavedSearch() {
    QSettings settings;

    if (settings.value(QStringLiteral("disableSavedSearchesAutoCompletion"))
            .toBool()) {
        return;
    }

    const QString text = ui->searchLineEdit->text();
    if (!text.isEmpty()) {
        int noteFolderId = NoteFolder::currentNoteFolderId();
        QString settingsKey = QStringLiteral("savedSearches/noteFolder-") +
                              QString::number(noteFolderId);
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
    QSettings settings;

    if (!settings.value(QStringLiteral("disableSavedSearchesAutoCompletion"))
             .toBool()) {
        QString settingsKey = QStringLiteral("savedSearches/noteFolder-") +
                              QString::number(noteFolderId);
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
    QSettings settings;
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

    QSettings settings;
    settings.setValue(QStringLiteral("showStatusBar"), checked);
}

void MainWindow::on_noteTreeWidget_currentItemChanged(
    QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    // in case all notes were removed
    if (current == nullptr) {
        return;
    }

    // handle changing of the current item for subfolders
    if (current->data(0, Qt::UserRole + 1).toInt() == FolderType) {
        on_noteSubFolderTreeWidget_currentItemChanged(current, previous);

        return;
    }

    qDebug() << __func__;

    // let's highlight the text from the search line edit and do a "in note
    // search"
    searchForSearchLineTextInNoteTextEdit();
}

void MainWindow::openCurrentNoteInTab() {
    // simulate a newly opened tab by updating the current tab with the last note
    if (_lastNoteId > 0) {
        auto previousNote = Note::fetch(_lastNoteId);
        if (previousNote.isFetched()) {
            updateCurrentTabData(previousNote);
        }
    }

    const QString &noteName = currentNote.getName();
    const int noteId = currentNote.getId();
    int tabIndex = Utils::Gui::getTabWidgetIndexByProperty(
        ui->noteEditTabWidget, QStringLiteral("note-id"), noteId);

    if (tabIndex == -1) {
        auto *widgetPage = new QWidget();
        widgetPage->setLayout(ui->noteEditTabWidgetLayout);
        tabIndex = ui->noteEditTabWidget->addTab(widgetPage, noteName);
    }

    Utils::Gui::updateTabWidgetTabData(ui->noteEditTabWidget,
                                       tabIndex, currentNote);

    ui->noteEditTabWidget->setCurrentIndex(tabIndex);

    // remove the tab initially created by the ui file
    if (ui->noteEditTabWidget->widget(0)->property("note-id").isNull()) {
        ui->noteEditTabWidget->removeTab(0);
    }
}

void MainWindow::on_noteTreeWidget_customContextMenuRequested(
    const QPoint pos) {
    auto *item = ui->noteTreeWidget->itemAt(pos);
    const QPoint globalPos = ui->noteTreeWidget->mapToGlobal(pos);
    const int type = item == nullptr ?
         0 : item->data(0, Qt::UserRole + 1).toInt();

    // if the user clicks at empty space, this is null and if it isn't handled
    // QON crashes
    if (item == nullptr || type == FolderType) {
        openNoteSubFolderContextMenu(globalPos, ui->noteTreeWidget);
    } else if (type == NoteType) {
        openNotesContextMenu(globalPos);
    }
}

void MainWindow::openNotesContextMenu(const QPoint globalPos,
                                      bool multiNoteMenuEntriesOnly) {
    QMenu noteMenu;
    QAction *renameAction = nullptr;

    if (!multiNoteMenuEntriesOnly) {
        auto *createNoteAction = noteMenu.addAction(tr("New note"));
        connect(createNoteAction, &QAction::triggered, this,
                &MainWindow::on_action_New_note_triggered);

        renameAction = noteMenu.addAction(tr("Rename note"));
        renameAction->setToolTip(
            tr("Allows you to rename the filename of "
               "the note"));
    }

    auto *removeAction = noteMenu.addAction(tr("&Remove notes"));
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
                auto *subFolderMoveMenu =
                    moveDestinationMenu->addMenu(noteFolder.getName());
                buildBulkNoteFolderSubFolderMenuTree(subFolderMoveMenu, false,
                                                     noteFolder.getLocalPath());

                auto *subFolderCopyMenu =
                    copyDestinationMenu->addMenu(noteFolder.getName());
                buildBulkNoteFolderSubFolderMenuTree(subFolderCopyMenu, true,
                                                     noteFolder.getLocalPath());
            } else {
                auto *moveAction =
                    moveDestinationMenu->addAction(noteFolder.getName());
                moveAction->setData(noteFolder.getLocalPath());
                moveAction->setToolTip(noteFolder.getLocalPath());
                moveAction->setStatusTip(noteFolder.getLocalPath());

                auto *copyAction =
                    copyDestinationMenu->addAction(noteFolder.getName());
                copyAction->setData(noteFolder.getLocalPath());
                copyAction->setToolTip(noteFolder.getLocalPath());
                copyAction->setStatusTip(noteFolder.getLocalPath());
            }
        }
    }

    QAction *moveToThisSubFolderAction = nullptr;
    const bool showSubFolders = NoteFolder::isCurrentShowSubfolders();
    if (showSubFolders) {
        if (ui->noteTreeWidget->selectedItems().count() == 1) {
            moveToThisSubFolderAction =
                noteMenu.addAction(tr("Jump to the note's subfolder"));
        }

        auto *subFolderMoveMenu =
            noteMenu.addMenu(tr("Move notes to subfolder…"));
        buildBulkNoteSubFolderMenuTree(subFolderMoveMenu, false);

        auto *subFolderCopyMenu =
            noteMenu.addMenu(tr("Copy notes to subfolder…"));
        buildBulkNoteSubFolderMenuTree(subFolderCopyMenu, true);
    }

    int tagCount = Tag::countAll();

    // show the tagging menu if at least one tag is present
    if (tagCount) {
        auto *tagMenu = noteMenu.addMenu(tr("&Tag selected notes with…"));
        buildBulkNoteTagMenuTree(tagMenu);
    }

    QStringList noteNameList;
    const auto items = ui->noteTreeWidget->selectedItems();
    for (QTreeWidgetItem *item : items) {
        // the note names are not unique any more but the note subfolder
        // path will be taken into account in
        // Tag::fetchAllWithLinkToNoteNames
        const QString name = item->text(0);
        const Note note = Note::fetchByName(name);
        if (note.isFetched()) {
            noteNameList << note.getName();
        }
    }

    const QVector<Tag> tagRemoveList =
        Tag::fetchAllWithLinkToNoteNames(noteNameList);

    // show the remove tags menu if at least one tag is present
    QMenu *tagRemoveMenu = nullptr;
    if (tagRemoveList.count() > 0) {
        tagRemoveMenu =
            noteMenu.addMenu(tr("&Remove tag from selected notes…"));

        for (const Tag &tag : tagRemoveList) {
            auto *action = tagRemoveMenu->addAction(tag.getName());
            action->setData(tag.getId());
            action->setToolTip(tag.getName());
            action->setStatusTip(tag.getName());
        }
    }

    QAction *openInExternalEditorAction = nullptr;
    QAction *openNoteWindowAction = nullptr;
    QAction *showInFileManagerAction = nullptr;
    QAction *showNoteGitLogAction = nullptr;

    if (!multiNoteMenuEntriesOnly) {
        noteMenu.addSeparator();
        auto *openNoteInTabAction = noteMenu.addAction(tr("Open note in tab"));
        connect(openNoteInTabAction, &QAction::triggered, this,
                &MainWindow::openCurrentNoteInTab);

        openInExternalEditorAction =
            noteMenu.addAction(tr("Open note in external editor"));
        openNoteWindowAction =
            noteMenu.addAction(tr("Open note in different window"));
        showInFileManagerAction =
            noteMenu.addAction(tr("Show note in file manager"));

        showNoteGitLogAction = new QAction(this);
        if (Utils::Git::isCurrentNoteFolderUseGit() &&
            Utils::Git::hasLogCommand()) {
            showNoteGitLogAction =
                noteMenu.addAction(tr("Show note git versions"));
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
        } else if (selectedItem == showInFileManagerAction) {
            // show the current note in the file manager
            on_actionShow_note_in_file_manager_triggered();
        } else if (selectedItem == showNoteGitLogAction) {
            // show the git log of the current note
            on_actionShow_note_git_versions_triggered();
        } else if (selectedItem == renameAction) {
            QTreeWidgetItem *item = ui->noteTreeWidget->currentItem();

            if (Note::allowDifferentFileName()) {
                if (Utils::Misc::isNoteListPreview()) {
                    bool ok{};
                    const QString name = QInputDialog::getText(
                        this, tr("Rename note"), tr("Name:"), QLineEdit::Normal,
                        currentNote.getName(), &ok);

                    if (ok && !name.isEmpty()) {
                        item->setText(0, name);
                        on_noteTreeWidget_itemChanged(item, 0);
                    }
                } else {
                    ui->noteTreeWidget->editItem(item);
                }
            } else {
                if (QMessageBox::warning(
                        this, tr("Note renaming not enabled!"),
                        tr("If you want to rename your note you have to enable "
                           "the option to allow the note filename to be "
                           "different from the headline."),
                        tr("Open &settings"), tr("&Cancel"), QString(), 0,
                        1) == 0) {
                    openSettingsDialog(SettingsDialog::NoteFolderPage);
                }
            }
        }
    }
}

/**
 * Renames a note file if the note was renamed in the note tree widget
 */
void MainWindow::on_noteTreeWidget_itemChanged(QTreeWidgetItem *item,
                                               int column) {
    if (item == nullptr) {
        return;
    }
    
    // handle note subfolder renaming in a note tree
    if (item->data(0, Qt::UserRole + 1) == FolderType) {
        on_noteSubFolderTreeWidget_itemChanged(item, column);

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
                Tag::renameNoteFileNamesOfLinks(oldNoteName, newNoteName,
                                                note.getNoteSubFolder());

                // handle the replacing of all note urls if a note was renamed
                note.handleNoteMoving(oldNote);

                // reload the directory list if note name has changed
                //                loadNoteDirectoryList();

                // sort notes if note name has changed
                QSettings settings;
                if (settings
                        .value(QStringLiteral("notesPanelSort"),
                               SORT_BY_LAST_CHANGE)
                        .toInt() == SORT_ALPHABETICAL) {
                    ui->noteTreeWidget->sortItems(
                        0, toQtOrder(
                               settings.value(QStringLiteral("notesPanelOrder"))
                                   .toInt()));
                    ui->noteTreeWidget->scrollToItem(item);
                }
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

void MainWindow::on_noteSubFolderTreeWidget_currentItemChanged(
    QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    if (current == nullptr) {
        return;
    }

    // set the note sub folder id as active
    const int noteSubFolderId = current->data(0, Qt::UserRole).toInt();
    _showNotesFromAllNoteSubFolders = noteSubFolderId == -1;

    NoteSubFolder::setAsActive(
        _showNotesFromAllNoteSubFolders ? 0 : noteSubFolderId);

    const auto items = ui->noteSubFolderTreeWidget->selectedItems();
    // if multiple items are selected, we just set the last selected
    // as active and return;
    if (items.count() > 1) {
        return;
    }

    QSettings settings;
    settings.setValue(
        QStringLiteral("MainWindow/showNotesFromAllNoteSubFolders"),
        _showNotesFromAllNoteSubFolders);

    const QSignalBlocker blocker(ui->searchLineEdit);
    Q_UNUSED(blocker)

    ui->searchLineEdit->clear();

    if (!NoteFolder::isCurrentNoteTreeEnabled()) {
        filterNotes();
    }

    reloadTagTree();
}

void MainWindow::on_noteSubFolderTreeWidget_itemSelectionChanged() {
    const auto items = ui->noteSubFolderTreeWidget->selectedItems();
    // if no items selected or only one selected
    if (ui->noteSubFolderTreeWidget->selectedItems().count() <= 1) {
        if (items.count() == 1) {
            // on_noteSubFolderTreeWidget_currentItemChanged(items.first(),
            // Q_NULLPTR);
        }
        return;
    }
    filterNotes();
    reloadTagTree();
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
bool MainWindow::showNotesFromAllNoteSubFolders() {
    QSettings settings;
    return settings
        .value(QStringLiteral("MainWindow/showNotesFromAllNoteSubFolders"),
               _showNotesFromAllNoteSubFolders)
        .toBool();
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
        Utils::Gui::searchForTextInTreeWidget(
            ui->noteSubFolderTreeWidget, arg1,
            Utils::Gui::TreeWidgetSearchFlag::IntCheck);
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
        noteSubFolder.saveTreeWidgetExpandState(item->isExpanded());
    }

    // resize columns so long folder names get displayed
    resizeNoteSubFolderTreeWidgetColumnToContents();
}

void MainWindow::on_noteSubFolderTreeWidget_itemCollapsed(
    QTreeWidgetItem *item) {
    on_noteSubFolderTreeWidget_itemExpanded(item);
}

/**
 * Shows the note subfolder context menu
 */
void MainWindow::openNoteSubFolderContextMenu(const QPoint globalPos,
                                              QTreeWidget *treeWidget) {
    // don't open the context menu if no subfolders are selected
    //   if (treeWidget->selectedItems().count() == 0) {
    //     return;
    // }

    QMenu menu;
    QAction *newNoteAction = menu.addAction(tr("New note"));
    QAction *newAction = menu.addAction(tr("New subfolder"));
    QAction *renameAction = menu.addAction(tr("Rename subfolder"));
    QAction *removeAction = menu.addAction(tr("Remove selected folders"));
    QAction *showInFileManagerAction =
        menu.addAction(tr("Show folder in file manager"));
    menu.addAction(ui->action_Reload_note_folder);

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
            removeSelectedNoteSubFolders(treeWidget);
        } else if (selectedItem == renameAction) {
            QTreeWidgetItem *item = treeWidget->currentItem();

            // rename folder
            treeWidget->editItem(item);
        } else if (selectedItem == showInFileManagerAction) {
            NoteSubFolder noteSubFolder =
                NoteFolder::currentNoteFolder().getActiveNoteSubFolder();

            // show the current folder in the file manager
            Utils::Misc::openPath(noteSubFolder.fullPath());
        }
    }
}

/**
 * Shows the context menu for the note subfolder tree
 */
void MainWindow::on_noteSubFolderTreeWidget_customContextMenuRequested(
    const QPoint pos) {
    const QPoint globalPos = ui->noteSubFolderTreeWidget->mapToGlobal(pos);
    openNoteSubFolderContextMenu(globalPos, ui->noteSubFolderTreeWidget);
}

/**
 * Creates a new note subfolder in the current subfolder
 */
bool MainWindow::createNewNoteSubFolder(QString folderName) {
    bool ok = true;

    if (folderName.isEmpty()) {
        folderName = QInputDialog::getText(this, tr("Create a new folder"),
                                           tr("Folder name:"),
                                           QLineEdit::Normal, QString(), &ok);
    }

    if (!ok || folderName.isEmpty() ||
        NoteSubFolder::willFolderBeIgnored(folderName, true)) {

        return false;
    }

    NoteSubFolder noteSubFolder =
        NoteFolder::currentNoteFolder().getActiveNoteSubFolder();
    const QString path =
        noteSubFolder.fullPath() + QDir::separator() + folderName;
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

/**
 * Renames a note subfolder
 */
void MainWindow::on_noteSubFolderTreeWidget_itemChanged(QTreeWidgetItem *item,
                                                        int column) {
    Q_UNUSED(column)

    if (item == nullptr) {
        return;
    }

    NoteSubFolder noteSubFolder =
        NoteSubFolder::fetch(item->data(0, Qt::UserRole).toInt());
    if (noteSubFolder.isFetched()) {
        const QString name = item->text(0);

#ifdef Q_OS_WIN32
        // clear all paths from the directory watcher, if a subfolder of the
        // folder to rename is watched Windows will block the renaming
        clearNoteDirectoryWatcher();
#endif

        // rename the note subfolder in the file system
        noteSubFolder.rename(name);

        // for some reason the app crashes if we are using this with a note tree
        if (!NoteFolder::isCurrentNoteTreeEnabled()) {
            // reload tags, note subfolder and notes
            on_action_Reload_note_folder_triggered();
        }
#ifdef Q_OS_WIN32
        else {
            // update the noteDirectoryWatcher if the note folder wasn't reloaded
            updateNoteDirectoryWatcher();
        }
#endif
    }
}

void MainWindow::on_actionShare_note_triggered() {
    const QSignalBlocker blocker(ui->actionShare_note);
    Q_UNUSED(blocker)
    ui->actionShare_note->setChecked(currentNote.isShared());

    ShareDialog *dialog = new ShareDialog(currentNote, this);
    dialog->exec();
    delete (dialog);

    currentNote.refetch();

    // update the share button
    updateShareButton();
}

/**
 * Toggles the case of the word under the Cursor or the selected text
 */
void MainWindow::on_actionToggle_text_case_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    // Save positions to restore everything at the end
    const int selectionStart = c.selectionStart();
    const int selectionEnd = c.selectionEnd();
    const int cPos = c.position();

    QString selectedText = c.selectedText();
    const bool textWasSelected = !selectedText.isEmpty();

    // if no text is selected: automatically select the Word under the Cursor
    if (selectedText.isEmpty()) {
        c.select(QTextCursor::WordUnderCursor);
        selectedText = c.selectedText();
    }

    // cycle text through lowercase, uppercase, start case, and sentence case
    c.insertText(Utils::Misc::cycleTextCase(selectedText));

    if (textWasSelected) {
        // select the text again to maybe do another operation on it
        // keep the original cursor position
        if (cPos == selectionStart) {
            c.setPosition(selectionEnd, QTextCursor::MoveAnchor);
            c.setPosition(selectionStart, QTextCursor::KeepAnchor);
        } else {
            c.setPosition(selectionStart, QTextCursor::MoveAnchor);
            c.setPosition(selectionEnd, QTextCursor::KeepAnchor);
        }
    } else {
        // Just restore the Cursor Position if no text was selected
        c.setPosition(cPos, QTextCursor::MoveAnchor);
    }
    // Restore the visible cursor
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
    applyFormatter(QStringLiteral("~~"));
}

/**
 * Initializes the shortcuts for the actions
 *
 * @param setDefaultShortcut
 */
void MainWindow::initShortcuts() {
    const QList<QMenu *> menus = menuList();
    QSettings settings;

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
            oldShortcut.replace(QStringLiteral("Alt+"),
                                QStringLiteral("Meta+"));
#endif

            const QString &key =
                QStringLiteral("Shortcuts/MainWindow-") + action->objectName();
            const bool settingFound = settings.contains(key);

            // try to load a key sequence from the settings
            auto shortcut = QKeySequence(settingFound ?
                settings.value(key).toString() : "");

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
                connect(shortcutItem, &QShortcut::activated, action,
                        &QAction::trigger);
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

    QSettings settings;
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
    QString name = currentNote.getName();
    const QVector<TagHeader> tags = Tag::fetchAllOfNote(currentNote);

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
    previousNoteText +=
        QStringLiteral("\n\n<") + noteLink + QStringLiteral(">");
    previousNote.storeNewText(std::move(previousNoteText));

    // add the previously removed text
    textEdit = activeNoteTextEdit();
    textEdit->insertPlainText(selectedText);

    // link the tags of the old note to the new note
    for (const TagHeader &tag : tags) {
        Tag::fetch(tag._id).linkToNote(currentNote);
    }
}

/**
 * Adds a custom action as menu item and button
 */
void MainWindow::addCustomAction(const QString &identifier,
                                 const QString &menuText,
                                 const QString &buttonText, const QString &icon,
                                 bool useInNoteEditContextMenu,
                                 bool hideButtonInToolbar,
                                 bool useInNoteListContextMenu) {
    //    ui->menuCustom_actions->show();
    QAction *action = ui->menuCustom_actions->addAction(menuText);
    action->setObjectName(QStringLiteral("customAction_") + identifier);
    action->setData(identifier);

    // restore the shortcut of the custom action
    QSettings settings;
    QKeySequence shortcut = QKeySequence(
        settings
            .value(QStringLiteral("Shortcuts/MainWindow-customAction_") +
                   identifier)
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
void MainWindow::addScriptingLabel(const QString &identifier,
                                   const QString &text) {
    _scriptingDockWidget->show();
    QLabel *label = new QLabel(text, _scriptingDockWidget);
    label->setOpenExternalLinks(true);
    label->setTextInteractionFlags(Qt::TextSelectableByMouse |
                                   Qt::LinksAccessibleByMouse);
    label->setWordWrap(true);
    label->setObjectName(QStringLiteral("scriptingLabel-") + identifier);
    ui->scriptingScrollAreaLayout->addWidget(label);
}

/**
 * Sets the text of a label in the scripting dock widget
 */
void MainWindow::setScriptingLabelText(const QString &identifier,
                                       const QString &text) {
    auto *label = ui->scriptingScrollArea->findChild<QLabel *>(
        QStringLiteral("scriptingLabel-") + identifier);
    if (label != Q_NULLPTR) {
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
    QKeyEvent *event =
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
    QCoreApplication::postEvent(ui->noteSubFolderTreeWidget, event);
}

/**
 * Sends an event to jump to "All notes" in the tag tree widget
 */
void MainWindow::selectAllNotesInTagTreeWidget() const {
    QKeyEvent *event =
        new QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier);
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
 * Shows a dialog to delete orphaned images
 */
void MainWindow::on_actionDelete_orphaned_images_triggered() {
    delete (_orphanedImagesDialog);
    _orphanedImagesDialog = new OrphanedImagesDialog(this);
    _orphanedImagesDialog->show();
}

/**
 * Writes text to the note text edit (for ScriptingService)
 *
 * @param text
 */
void MainWindow::writeToNoteTextEdit(const QString &text) {
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

    return selectedText;
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

        if (!_noteEditIsCentralWidget) {
            _noteEditDockWidget->setTitleBarWidget(_noteEditDockTitleBarWidget);
        }

        _noteTagDockWidget->setTitleBarWidget(_noteTagDockTitleBarWidget);
        _notePreviewDockWidget->setTitleBarWidget(
            _notePreviewDockTitleBarWidget);
        _logDockWidget->setTitleBarWidget(_logDockTitleBarWidget);
        _scriptingDockWidget->setTitleBarWidget(_scriptingDockTitleBarWidget);

        for (QDockWidget *dockWidget : dockWidgets) {
            // reset the top margin of the enclosed widget
            dockWidget->widget()->setContentsMargins(0, 0, 0, 0);
        }
    }
}

/**
 * Creates a new workspace with asking for its name
 */
void MainWindow::on_actionStore_as_new_workspace_triggered() {
    const QString name = QInputDialog::getText(this, tr("Create new workspace"),
                                               tr("Workspace name:"))
                             .trimmed();

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
    const QString currentUuid = currentWorkspaceUuid();
    settings.setValue(QStringLiteral("previousWorkspace"), currentUuid);

    const QString uuid = Utils::Misc::createUuidString();
    QStringList workspaces = getWorkspaceUuidList();
    workspaces.append(uuid);

    settings.setValue(QStringLiteral("workspaces"), workspaces);
    settings.setValue(QStringLiteral("currentWorkspace"), uuid);
    settings.setValue(
        QStringLiteral("workspace-") + uuid + QStringLiteral("/name"), name);

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
 * Sets a new current workspace
 */
void MainWindow::setCurrentWorkspace(const QString &uuid) {
    QWidget *focusWidget = qApp->focusWidget();

    // store the current workspace
    storeCurrentWorkspace();

    QSettings settings;
    QString currentUuid = currentWorkspaceUuid();
    settings.setValue(QStringLiteral("previousWorkspace"), currentUuid);
    settings.setValue(QStringLiteral("currentWorkspace"), uuid);

    // restore the new workspace
    restoreCurrentWorkspace();

    // update the menu and combo box (but don't rebuild it)
    updateWorkspaceLists(false);

    if (focusWidget != Q_NULLPTR) {
        // set the focus to the widget that had the focus before
        // the workspace was restored
        focusWidget->setFocus();
    }

    // update the preview in case it was disable previously
    setNoteTextFromNote(&currentNote, true);
}

/**
 * Stores the current workspace
 */
void MainWindow::storeCurrentWorkspace() {
    QSettings settings;
    QString uuid = currentWorkspaceUuid();

    settings.setValue(
        QStringLiteral("workspace-") + uuid + QStringLiteral("/windowState"),
        saveState());
    settings.setValue(QStringLiteral("workspace-") + uuid +
                          QStringLiteral("/noteSubFolderDockWidgetVisible"),
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
        createNewWorkspace(tr("full", "full workspace"));

        _taggingDockWidget->setVisible(false);
        _noteFolderDockWidget->setVisible(false);
        _noteNavigationDockWidget->setVisible(false);
        _noteTagDockWidget->setVisible(false);
        _notePreviewDockWidget->setVisible(false);
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

    restoreState(settings
                     .value(QStringLiteral("workspace-") + uuid +
                            QStringLiteral("/windowState"))
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
            settings.value(QStringLiteral("initialLayoutIdentifier"))
                .toString());

        settings.remove(QStringLiteral("initialWorkspace"));
        centerAndResize();
    }
}

/**
 * Handles the visibility of the note subfolder panel
 */
void MainWindow::handleNoteSubFolderVisibility() const {
    // turn the subfolder dock widget on or off according to whether the
    // subfolders are enabled or not
    bool showSubfolders = NoteFolder::isCurrentShowSubfolders();
    _noteSubFolderDockWidget->setVisible(showSubfolders &&
                                         _noteSubFolderDockWidgetVisible);
}

/**
 * Returns the list of workspace uuids
 * @return
 */
QStringList MainWindow::getWorkspaceUuidList() {
    QSettings settings;
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
                             QStringLiteral("remove-workspace")) !=
        QMessageBox::Yes) {
        return;
    }

    // reset current workspace
    workspaces.removeAll(uuid);
    const QString newUuid = workspaces.at(0);

    // set the new workspace
    setCurrentWorkspace(newUuid);

    QSettings settings;
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

    QSettings settings;
    QString name = settings
                       .value(QStringLiteral("workspace-") + uuid +
                              QStringLiteral("/name"))
                       .toString();

    // ask for the new name
    name = QInputDialog::getText(this, tr("Rename workspace"),
                                 tr("Workspace name:"), QLineEdit::Normal, name)
               .trimmed();

    if (name.isEmpty()) {
        return;
    }

    // rename the workspace
    settings.setValue(
        QStringLiteral("workspace-") + uuid + QStringLiteral("/name"), name);

    // update the menu and combo box
    updateWorkspaceLists();
}

/**
 * Switch to the previous workspace
 */
void MainWindow::on_actionSwitch_to_previous_workspace_triggered() {
    QSettings settings;
    QString uuid =
        settings.value(QStringLiteral("previousWorkspace")).toString();

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

    // update the preview in case it was disable previously
    setNoteTextFromNote(&currentNote, true);

    // filter notes according to selections
    filterNotes();
}

/**
 * Opens the find action dialog
 */
void MainWindow::on_actionFind_action_triggered() {
    if (_actionDialog == Q_NULLPTR) {
        _actionDialog = new ActionDialog(ui->menuBar, this);
    } else {
        _actionDialog->refreshUi();
    }

    _actionDialog->show();
    _actionDialog->activateWindow();
    _actionDialog->raise();
}

/**
 * Opens the table dialog
 */
void MainWindow::on_actionInsert_table_triggered() {
    auto *dialog = new TableDialog(this);
    dialog->exec();
    delete (dialog);
}

/**
 * Inserts a block quote character or formats the selected text as block quote
 */
void MainWindow::on_actionInsert_block_quote_triggered() {
    auto *textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    QString selectedText = c.selectedText();

    if (selectedText.isEmpty()) {
        c.insertText(QStringLiteral("> "));
        textEdit->setTextCursor(c);
    } else {
        // this only applies to the start of the selected block
        selectedText.replace(QRegularExpression(QStringLiteral("^")),
                             QStringLiteral("> "));

        // transform Unicode line endings
        // this newline character seems to be used in multi-line selections
        const QString newLine =
            QString::fromUtf8(QByteArray::fromHex("e280a9"));
        selectedText.replace(newLine, QStringLiteral("\n> "));

        // remove the block quote if it was placed at the end of the text
        selectedText.remove(QRegularExpression(QStringLiteral("> $")));

        c.insertText(selectedText);
    }
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

    QSettings settings;
    typedef Utils::Misc::SearchEngine SearchEngine;
    int selectedSearchEngineId =
        settings
            .value(QStringLiteral("SearchEngineId"),
                   Utils::Misc::getDefaultSearchEngineId())
            .toInt();
    QHash<int, SearchEngine> SearchEngines =
        Utils::Misc::getSearchEnginesHashMap();
    SearchEngine selectedEngine = SearchEngines.value(selectedSearchEngineId);
    QString searchEngineUrl = selectedEngine.searchUrl;
    QUrl url(searchEngineUrl + QUrl::toPercentEncoding(selectedText));
    QDesktopServices::openUrl(url);
}

/**
 * Updates the line number label
 */
void MainWindow::noteEditCursorPositionChanged() {
    if (!_noteEditLineNumberLabel->isVisible()) return;
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();
    QTextCursor cursor = textEdit->textCursor();
    QString selectedText = cursor.selectedText();
    QString text;

    this->noteHistory.updateCursorPositionOfNote(currentNote, textEdit);

    if (!selectedText.isEmpty()) {
        text = tr("%n chars", "characters", selectedText.count()) + "  ";
    }

    text += QString::number(cursor.block().blockNumber() + 1) +
            QStringLiteral(":") + QString::number(cursor.positionInBlock() + 1);

    _noteEditLineNumberLabel->setText(text);
}

/**
 * Deletes the current line in the active note text edit
 */
void MainWindow::on_actionDelete_line_triggered() {
    QOwnNotesMarkdownTextEdit *textEdit = activeNoteTextEdit();

    // if the note text edit doesn't have the focus delegate the default
    // shortcut to the widget with the focus
    if (!textEdit->hasFocus()) {
        QKeyEvent *event =
            new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::AltModifier);

        // we need a special fallback for QLineEdit because it seems to ignore
        // our event
        if (dynamic_cast<QLineEdit *>(QApplication::focusWidget()) != nullptr) {
            auto *lineEdit =
                dynamic_cast<QLineEdit *>(QApplication::focusWidget());
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
        QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace,
                                         Qt::ControlModifier);

        // we need a special fallback for QLineEdit because it seems to ignore
        // our event
        if (dynamic_cast<QLineEdit *>(QApplication::focusWidget()) != nullptr) {
            auto *lineEdit =
                dynamic_cast<QLineEdit *>(QApplication::focusWidget());
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
    QSettings settings;
    settings.setValue(QStringLiteral("notesPanelOrder"), ORDER_ASCENDING);
    ui->noteTreeWidget->sortItems(0, toQtOrder(ORDER_ASCENDING));
}

/**
 * Sets descending note sort order
 */
void MainWindow::on_actionDescending_triggered() {
    QSettings settings;
    settings.setValue(QStringLiteral("notesPanelOrder"), ORDER_DESCENDING);
    ui->noteTreeWidget->sortItems(0, toQtOrder(ORDER_DESCENDING));
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
void MainWindow::gitCommitCurrentNoteFolder() {
    Utils::Git::commitCurrentNoteFolder();
}

/**
 * Shows a git log of the current note
 */
void MainWindow::on_actionShow_note_git_versions_triggered() {
    QString relativeFilePath = currentNote.relativeNoteFilePath();
    //    QString dirPath = NoteFolder::currentLocalPath();

    //    qDebug() << __func__ << " - 'relativeFilePath': " << relativeFilePath;
    //    qDebug() << __func__ << " - 'dirPath': " << dirPath;
    //
    //
    //    QString result = Utils::Misc::startSynchronousProcess(
    //            "/bin/bash",
    //            QStringList() << "-c" << "cd \"" + dirPath + "\" && " +
    //                                  "git log -p \"" + relativeFilePath +
    //                                  "\"");
    //
    //    qDebug() << __func__ << " - 'result': " << result;

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
    resizeTagTreeWidgetColumnToContents();
    storeTagTreeWidgetExpandState();
}

/**
 * Stores the note tag tree expand state
 */
void MainWindow::storeTagTreeWidgetExpandState() const {
    // get all items
    const auto allItems = ui->tagTreeWidget->findItems(
        QLatin1String(""), Qt::MatchContains | Qt::MatchRecursive);

    QStringList expandedList;
    for (QTreeWidgetItem *item : allItems) {
        if (Utils::Gui::isOneTreeWidgetItemChildVisible(item)) {
            if (item->isExpanded()) {
                expandedList << item->data(0, Qt::UserRole).toString();
            }
        }
    }

    QSettings settings;
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

Qt::SortOrder MainWindow::toQtOrder(int order) {
    return order == ORDER_ASCENDING ? Qt::AscendingOrder : Qt::DescendingOrder;
}

void MainWindow::updatePanelsSortOrder() {
    updateNotesPanelSortOrder();
    reloadNoteSubFolderTree();
    // do not reload it again, it has already been reloaded when
    // updateNotesPanelSortOrder() was called
    //reloadTagTree();
}

void MainWindow::updateNotesPanelSortOrder() {
    QSettings settings;
    int sort =
        settings.value(QStringLiteral("notesPanelSort"), SORT_BY_LAST_CHANGE)
            .toInt();
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
    QSettings settings;
    settings.setValue(QStringLiteral("allowNoteEditing"), checked);

    updateNoteTextEditReadOnly();
    setMenuEnabled(ui->menuEditNote, checked);
    setMenuEnabled(ui->menuInsert, checked);
    setMenuEnabled(ui->menuFormat, checked);
    ui->actionPaste_image->setEnabled(checked);
    ui->actionReplace_in_current_note->setEnabled(checked);
    _readOnlyButton->setHidden(checked);

    ui->actionAllow_note_editing->setText(checked
                                              ? tr("Disallow all note editing")
                                              : tr("Allow all note editing"));
}

void MainWindow::allowNoteEditing() {
    if (!ui->actionAllow_note_editing->isChecked()) {
        ui->actionAllow_note_editing->trigger();
    }
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
    auto *dialog = new ScriptRepositoryDialog(this, true);
    dialog->exec();
    delete (dialog);

    // reload the scripting engine
    ScriptingService::instance()->reloadEngine();
}

/**
 * This method checks if scripts need to be updated and open a dialog to ask if
 * the user wants to update them (if the dialog wasn't disabled)
 */
void MainWindow::automaticScriptUpdateCheck() {
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    _scriptUpdateFound = false;
    auto *dialog = new ScriptRepositoryDialog(this, true);

    // show a dialog once if a script update was found
    QObject::connect(
        dialog,
        &ScriptRepositoryDialog::updateFound,
        this, [this] () {
            // we only want to run this once
            if (_scriptUpdateFound) {
                return;
            }

            _scriptUpdateFound = true;
            showStatusBarMessage(tr("A script update was found!"), 4000);

            // open the update question dialog in another thread so the dialog
            // can be deleted meanwhile
            QTimer::singleShot(100, this, [this] () {
                if (Utils::Gui::question(
                    this, tr("Script updates"),
                    tr("Updates to your scripts were found in the script "
                       "repository! Do you want to update them?"),
                    "auto-script-update") == QMessageBox::Yes) {
                    on_actionCheck_for_script_updates_triggered();
                }
            });
        });

    // delete the dialog after 10 sec
    QTimer::singleShot(10000, this, [this, dialog] () {
        delete(dialog);

        if (_scriptUpdateFound) {
            _scriptUpdateFound = false;
        } else {
            showStatusBarMessage(tr("No script updates were found"), 3000);
        }
    });
#endif
}

void MainWindow::noteTextEditResize(QResizeEvent *event) {
    Q_UNUSED(event)
    ui->noteTextEdit->setPaperMargins();
}

void MainWindow::encryptedNoteTextEditResize(QResizeEvent *event) {
    Q_UNUSED(event)
    ui->encryptedNoteTextEdit->setPaperMargins();
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
void MainWindow::on_tagTreeWidget_itemDoubleClicked(QTreeWidgetItem *item,
                                                    int column) {
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
void MainWindow::on_noteTreeWidget_itemDoubleClicked(QTreeWidgetItem *item,
                                                     int column) {
    Q_UNUSED(item)
    Q_UNUSED(column)

    // call a script hook that a new note was double clicked
    const bool hookFound = ScriptingService::instance()->
                      callHandleNoteDoubleClickedHook(&currentNote);

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
    if (ui->noteTreeWidget->selectedItems().size() == 1) {
        int noteId = ui->noteTreeWidget->selectedItems()[0]->data(0, Qt::UserRole).toInt();
        Note note = Note::fetch(noteId);
        setCurrentNote(std::move(note), true, false);
    }

    // we also need to do this in setCurrentNote because of different timings
    reloadCurrentNoteTags();
}

/**
 * Shows a dialog to delete orphaned attachments
 */
void MainWindow::on_actionManage_orphaned_attachments_triggered() {
    delete (_orphanedAttachmentsDialog);
    _orphanedAttachmentsDialog = new OrphanedAttachmentsDialog(this);
    _orphanedAttachmentsDialog->show();
}

void MainWindow::on_noteOperationsButton_clicked() {
    QPoint globalPos = ui->noteOperationsButton->mapToGlobal(
        QPoint(0, ui->noteOperationsButton->height()));
    openNotesContextMenu(globalPos, true);
}

/**
 * Returns the text of the log widget
 */
QString MainWindow::getLogText() {
    auto *widget = dynamic_cast<LogWidget *>(_logDockWidget->widget());
    return widget->getLogText();
}

/**
 * Enables all logging and shows the log panel
 */
void MainWindow::turnOnDebugLogging() {
    QSettings settings;
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
        progressDialog.setLabelText(
            tr("Importing: %1").arg(fileInfo.fileName()));

        file.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&file);
        QString text = ts.readAll().trimmed();

        QRegularExpressionMatch match =
            QRegularExpression(QStringLiteral(R"(^.+\n=+)"),
                               QRegularExpression::MultilineOption)
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
        QRegularExpression(QStringLiteral(R"(^(.+)\n=+)"),
                           QRegularExpression::MultilineOption)
            .match(noteText);

    QString headline;
    if (match.hasMatch()) {
        headline = match.captured(1);
    } else {
        // try alternative headlines
        match = QRegularExpression(QStringLiteral(R"(^#+ (.+)$)"),
                                   QRegularExpression::MultilineOption)
                    .match(noteText);

        if (match.hasMatch()) {
            headline = match.captured(1);
        }
    }

    if (!headline.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(headline);
        showStatusBarMessage(
            tr("Note headline '%1' was copied to the clipboard").arg(headline),
            3000);
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
    QSize availableSize =
        QGuiApplication::primaryScreen()->availableGeometry().size();
    int width = availableSize.width();
    int height = availableSize.height();
    qDebug() << "Available dimensions " << width << "x" << height;
    width *= 0.9;     // 90% of the screen size
    height *= 0.9;    // 90% of the screen size
    qDebug() << "Computed dimensions " << width << "x" << height;
    QSize newSize(width, height);

    setGeometry(QStyle::alignedRect(
        Qt::LeftToRight, Qt::AlignCenter, newSize,
        QGuiApplication::primaryScreen()->availableGeometry()));
}

/**
 * Filters navigation entries in the navigation tree widget
 */
void MainWindow::on_navigationLineEdit_textChanged(const QString &arg1) {
    Utils::Gui::searchForTextInTreeWidget(
        ui->navigationWidget, arg1, Utils::Gui::TreeWidgetSearchFlag::IntCheck);
}

Note MainWindow::getCurrentNote() { return currentNote; }

void MainWindow::on_actionJump_to_note_list_panel_triggered() {
    ui->noteTreeWidget->setFocus();
}

void MainWindow::on_actionJump_to_tags_panel_triggered() {
    ui->tagTreeWidget->setFocus();
}

void MainWindow::on_actionJump_to_note_subfolder_panel_triggered() {
    ui->noteSubFolderTreeWidget->setFocus();
}

void MainWindow::on_actionActivate_context_menu_triggered() {
    activateContextMenu();
}

void MainWindow::on_actionImport_bookmarks_from_server_triggered() {
    OwnCloudService *ownCloud = OwnCloudService::instance();
    ownCloud->fetchBookmarks();
}

void MainWindow::on_actionRiot_triggered() {
    QDesktopServices::openUrl(
        QUrl(QStringLiteral("https://riot.im/app/#/room/"
                            "!rUzrRvrnrOsLasDdbp:matrix.org?via=matrix.org")));
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
        disconnect(_leaveFullScreenModeButton, Q_NULLPTR, Q_NULLPTR, Q_NULLPTR);
        delete _leaveFullScreenModeButton;
        _leaveFullScreenModeButton = nullptr;
    } else {
        _isMaximizedBeforeFullScreen = isMaximized();
        _isMinimizedBeforeFullScreen = isMinimized();
        showFullScreen();

        _leaveFullScreenModeButton->setFlat(true);
        _leaveFullScreenModeButton->setToolTip(tr("Leave full-screen mode"));
        _leaveFullScreenModeButton->setStyleSheet(
            QStringLiteral("QPushButton {padding: 0 5px}"));

        _leaveFullScreenModeButton->setIcon(QIcon::fromTheme(
            QStringLiteral("zoom-original"),
            QIcon(QStringLiteral(
                ":icons/breeze-qownnotes/16x16/zoom-original.svg"))));

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
    QSettings settings;
    settings.setValue(QStringLiteral("Editor/centerCursor"), arg1);
    ui->noteTextEdit->updateSettings();
    ui->encryptedNoteTextEdit->updateSettings();

    if (arg1) {
        // center the cursor immediately if typewriter mode is turned on
        activeNoteTextEdit()->centerTheCursor();
    }
}

void MainWindow::on_actionCheck_spelling_toggled(bool checked) {
    QSettings settings;
    settings.setValue(QStringLiteral("checkSpelling"), checked);
    ui->noteTextEdit->updateSettings();
    ui->encryptedNoteTextEdit->updateSettings();
}

void MainWindow::loadDictionaryNames() {
    QSettings settings;

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
    connect(_languageGroup, &QActionGroup::triggered, this,
            &MainWindow::onLanguageChanged);

    // first add autoDetect
    QAction *autoDetect =
        ui->menuLanguages->addAction(tr("Automatically detect"));
    autoDetect->setCheckable(true);
    autoDetect->setData(QStringLiteral("auto"));
    autoDetect->setActionGroup(_languageGroup);
    QString prevLang =
        settings
            .value(QStringLiteral("spellCheckLanguage"), QStringLiteral("auto"))
            .toString();
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
    QSettings settings;
    settings.setValue(QStringLiteral("spellCheckLanguage"), lang);
    ui->noteTextEdit->updateSettings();
}

void MainWindow::loadSpellingBackends() {
    QSettings settings;
    QString prevBackend = settings
                              .value(QStringLiteral("spellCheckBackend"),
                                     QStringLiteral("Hunspell"))
                              .toString();

    _spellBackendGroup->setExclusive(true);
    connect(_spellBackendGroup, &QActionGroup::triggered, this,
            &MainWindow::onBackendChanged);

    QAction *hs =
        ui->menuSpelling_backend->addAction(QStringLiteral("Hunspell"));
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

void MainWindow::onBackendChanged(QAction *action) {
    QString backend = action->data().toString();
    QSettings settings;
    settings.setValue(QStringLiteral("spellCheckBackend"), backend);
    Utils::Misc::needRestart();
    showRestartNotificationIfNeeded();
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

    if (currentNote.storeNewDecryptedText(
            ui->encryptedNoteTextEdit->toPlainText())) {
        handleNoteTextChanged();
    }
}

void MainWindow::on_actionEditorWidthCustom_triggered() {
    QSettings settings;
    bool ok;
    int characters = QInputDialog::getInt(
        this, tr("Custom editor width"), tr("Characters:"),
        settings
            .value(QStringLiteral("DistractionFreeMode/editorWidthCustom"), 80)
            .toInt(),
        20, 10000, 1, &ok);

    if (ok) {
        settings.setValue(
            QStringLiteral("DistractionFreeMode/editorWidthCustom"),
            characters);
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
    const int noteId = widget->property("note-id").toInt();

    // close the tab if note doesn't exist any more
    if (!Note::noteIdExists(noteId)) {
        removeNoteTab(index);
        return;
    }

    setCurrentNoteFromNoteId(noteId);
    widget->setLayout(ui->noteEditTabWidgetLayout);

    closeOrphanedTabs();
}

void MainWindow::on_noteEditTabWidget_tabCloseRequested(int index) {
    removeNoteTab(index);
}

void MainWindow::on_actionPrevious_note_tab_triggered() {
    int index = ui->noteEditTabWidget->currentIndex() - 1;

    if (index < 0) {
        index = ui->noteEditTabWidget->count() - 1;
    }

    ui->noteEditTabWidget->setCurrentIndex(index);
}

void MainWindow::on_actionNext_note_tab_triggered() {
    int index = ui->noteEditTabWidget->currentIndex() + 1;

    if (index >= ui->noteEditTabWidget->count()) {
        index = 0;
    }

    ui->noteEditTabWidget->setCurrentIndex(index);
}

void MainWindow::on_actionClose_current_note_tab_triggered() {
    removeNoteTab(ui->noteEditTabWidget->currentIndex());
}

void MainWindow::on_actionNew_note_in_new_tab_triggered() {
    on_action_New_note_triggered();
    openCurrentNoteInTab();
}

void MainWindow::removeNoteTab(int index) const {
    if (ui->noteEditTabWidget->count() > 1) {
        ui->noteEditTabWidget->removeTab(index);
    }
}

void MainWindow::on_noteEditTabWidget_tabBarDoubleClicked(int index) {
    Utils::Gui::setTabWidgetTabSticky(ui->noteEditTabWidget, index,
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

    if (!_showNotesFromAllNoteSubFolders &&
        !currentNote.isInCurrentNoteSubFolder()) {
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
    connect(stickAction, &QAction::triggered, this, [this, tabIndex]() {
        on_noteEditTabWidget_tabBarDoubleClicked(tabIndex);
    });

    // Close other note tabs
    auto *closeAction = menu->addAction(tr("Close other note tabs"));
    connect(closeAction, &QAction::triggered, this, [this, tabIndex]() {
        const int maxIndex = ui->noteEditTabWidget->count() - 1;
        const int keepNoteId = Utils::Gui::getTabWidgetNoteId(
            ui->noteEditTabWidget, tabIndex);

        for (int i = maxIndex; i >= 0; i--) {
            const int noteId = Utils::Gui::getTabWidgetNoteId(
                ui->noteEditTabWidget, i);

            if (noteId != keepNoteId) {
                removeNoteTab(i);
            }
        }
    });

    menu->exec(ui->noteEditTabWidget->tabBar()->mapToGlobal(point));
}
