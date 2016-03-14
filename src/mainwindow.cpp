#include "mainwindow.h"
#include <QSplitter>
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
#include <QMimeData>
#include <QTextBlock>
#include <QClipboard>
#include <QTemporaryFile>
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


MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowTitle(
            "QOwnNotes - version " + QString(VERSION) +
                    " - build " + QString::number(BUILD));

    MetricsService *metricsService = MetricsService::createInstance(this);
    metricsService->sendVisitIfEnabled("app/start", "App Start");

    // sends locale information
    metricsService->sendLocaleEvent();

    ClientProxy proxy;
    // refresh the Qt proxy settings
    proxy.setupQtProxyFromSettings();

    QActionGroup *sorting = new QActionGroup(this);
    sorting->addAction(ui->actionAlphabetical);
    sorting->addAction(ui->actionBy_date);
    sorting->setExclusive(true);

    // hide the encrypted note text edit by default
    ui->encryptedNoteTextEdit->hide();

    // set the search frames for the note text edits
    ui->noteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame);
    ui->encryptedNoteTextEdit->initSearchFrame(ui->noteTextEditSearchFrame);

    // set the main window for accessing it's public methods
    ui->noteTextEdit->setMainWindow(this);
    ui->encryptedNoteTextEdit->setMainWindow(this);

    DatabaseService::createConnection();
    DatabaseService::setupTables();

    this->firstVisibleNoteListRow = 0;
    this->noteHistory = NoteHistory();

    // set our signal mapper
    this->recentNoteFolderSignalMapper = new QSignalMapper(this);

    readSettings();

    // set sorting
    ui->actionBy_date->setChecked(!sortAlphabetically);
    ui->actionAlphabetical->setChecked(sortAlphabetically);

    // set the show in system tray checkbox
    ui->actionShow_system_tray->setChecked(showSystemTray);

    createSystemTrayIcon();
    setupMainSplitter();
    buildNotesIndex();
    loadNoteDirectoryList();

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

    // check if we have a todo reminder every minute
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
    ui->notesListWidget->installEventFilter(this);
    ui->noteTextEdit->installEventFilter(this);
    ui->noteTextEdit->viewport()->installEventFilter(this);
    ui->encryptedNoteTextEdit->installEventFilter(this);
    ui->encryptedNoteTextEdit->viewport()->installEventFilter(this);
    ui->notesListWidget->setCurrentRow(0);

    // ignores note clicks in QMarkdownTextEdit in the note text edit
    ui->noteTextEdit->setIgnoredClickUrlSchemata(QStringList() << "note");
    ui->encryptedNoteTextEdit->setIgnoredClickUrlSchemata(
            QStringList() << "note");

    // handle note url externally in the note text edit
    QObject::connect(
            ui->noteTextEdit,
            SIGNAL(urlClicked(QUrl)),
            this,
            SLOT(openNoteUrl(QUrl)));

    // also handle note url externally in the encrypted note text edit
    QObject::connect(
            ui->encryptedNoteTextEdit,
            SIGNAL(urlClicked(QUrl)),
            this,
            SLOT(openNoteUrl(QUrl)));

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


    // load the recent note folder list in the menu
    this->loadRecentNoteFolderListMenu(notesPath);

    this->updateService = new UpdateService(this);
    this->updateService->checkForUpdates(this, UpdateService::AppStart);

    // update the current folder tooltip
    updateCurrentFolderTooltip();

    // add some different shortcuts for the note history on the mac
#ifdef Q_OS_MAC
    ui->action_Back_in_note_history->
            setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_Left);
    ui->action_Forward_in_note_history->
            setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_Right);
#endif

    // disable the update check menu entry if the release string was set
//    if (!QString(RELEASE).isEmpty()) {
//        ui->actionCheck_for_updates->setVisible(false);
//        ui->actionCheck_for_updates->setEnabled(false);
//    }

    // let the note folder be changed with the recent note folder combo box
    QObject::connect(
            ui->recentNoteFolderComboBox,
            SIGNAL(currentTextChanged(const QString &)),
            this,
            SLOT(changeNoteFolder(const QString &)));

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
        ui->actionShow_toolbar->setChecked(!ui->mainToolBar->isHidden());
    }

    connect(ui->mainToolBar, SIGNAL(visibilityChanged(bool)),
            this, SLOT(mainToolbarVisibilityChanged(bool)));

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
}

MainWindow::~MainWindow() {
    storeUpdatedNotesToDisk();
    delete ui;
}


/*!
 * Methods
 */

/**
 * Restores the distraction free mode
 */
void MainWindow::restoreDistractionFreeMode() {
    QSettings settings;
    bool isInDistractionFreeMode =
            settings.value("DistractionFreeMode/isEnabled").toBool();

    if (isInDistractionFreeMode) {
        setDistractionFreeMode(true);
    }
}

/**
 * Toggles the distraction free mode
 */
void MainWindow::toggleDistractionFreeMode() {
    QSettings settings;
    bool isInDistractionFreeMode =
            settings.value("DistractionFreeMode/isEnabled").toBool();

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
 * Enables or disables the distraction free mode
 */
void MainWindow::setDistractionFreeMode(bool enabled) {
    QSettings settings;
    QString styling = "QTextEdit {background-color: white;}";

    if (enabled) {
        //
        // enter the distraction free mode
        //

        // remember states, geometry and sizes
        settings.setValue("DistractionFreeMode/windowState", saveState());
        settings.setValue("DistractionFreeMode/menuBarGeometry",
                          ui->menuBar->saveGeometry());
        settings.setValue("DistractionFreeMode/mainSplitterSizes",
                          mainSplitter->saveState());
        settings.setValue("DistractionFreeMode/menuBarHeight",
                          ui->menuBar->height());

        // we must not hide the menu bar or else the shortcuts
        // will not work any more
        ui->menuBar->setFixedHeight(0);

        // hide the tool bar
        ui->mainToolBar->hide();

        // hide the search line edit
        ui->searchLineEdit->hide();

        // hide the tab bar of the note tab widget
        ui->noteTabWidget->tabBar()->hide();

        // hide the status bar
//        ui->statusBar->hide();

        // hide the notes list widget
        QList<int> sizes = mainSplitter->sizes();
        int size = sizes.takeFirst() + sizes.takeFirst();
        sizes << 0 << size;
        mainSplitter->setSizes(sizes);

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

        ui->noteTextEdit->setStyleSheet(
                ui->noteTextEdit->styleSheet() + styling);

        ui->encryptedNoteTextEdit->setStyleSheet(
                ui->encryptedNoteTextEdit->styleSheet() + styling);

        ui->noteTextView->setStyleSheet(
                ui->noteTextView->styleSheet() + styling);
    } else {
        //
        // leave the distraction free mode
        //

        statusBar()->removeWidget(_leaveDistractionFreeModeButton);
        disconnect(_leaveDistractionFreeModeButton, 0, 0, 0);

        // restore states and sizes
        QByteArray state = settings.value
                ("DistractionFreeMode/mainSplitterSizes").toByteArray();
        mainSplitter->restoreState(state);
        restoreState(
                settings.value(
                        "DistractionFreeMode/windowState").toByteArray());
        ui->menuBar->restoreGeometry(
                settings.value(
                        "DistractionFreeMode/menuBarGeometry").toByteArray());
        ui->menuBar->setFixedHeight(
                settings.value("DistractionFreeMode/menuBarHeight").toInt());

        // show the tab bar of the note tab widget
        ui->noteTabWidget->tabBar()->show();

        // show the search line edit
        ui->searchLineEdit->show();

        ui->noteTextEdit->setStyleSheet(
                ui->noteTextEdit->styleSheet().replace(styling, ""));

        ui->encryptedNoteTextEdit->setStyleSheet(
                ui->encryptedNoteTextEdit->styleSheet().replace(styling, ""));

        ui->noteTextView->setStyleSheet(
                ui->noteTextView->styleSheet().replace(styling, ""));
    }

    ui->noteTextEdit->setPaperMargins(this->width());
    ui->encryptedNoteTextEdit->setPaperMargins(this->width());
}

/**
 * Sets the distraction free mode if it is currently other than we want it to be
 */
void MainWindow::changeDistractionFreeMode(bool enabled) {
    QSettings settings;
    bool isInDistractionFreeMode =
            settings.value("DistractionFreeMode/isEnabled").toBool();

    if (isInDistractionFreeMode != enabled) {
        setDistractionFreeMode(enabled);
    }
}

/**
 * Shows a status bar message if not in distraction free mode
 */
void MainWindow::showStatusBarMessage(const QString & message, int timeout) {
    QSettings settings;
    bool isInDistractionFreeMode =
            settings.value("DistractionFreeMode/isEnabled").toBool();

    if (!isInDistractionFreeMode) {
        ui->statusBar->showMessage(message, timeout);
    }
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
 * Loads the menu entries for the recent note folders
 */
void MainWindow::loadRecentNoteFolderListMenu(QString currentFolderName) {
    QSettings settings;
    QStringList recentNoteFolders =
            settings.value("recentNoteFolders").toStringList();

    int maxItems = 15;
    // remove items if there are too many
    if (recentNoteFolders.length() > maxItems) {
        // remove an item as long as there are too many of them
        do {
            recentNoteFolders.removeAt(maxItems);
        } while (recentNoteFolders.length() > maxItems);

        settings.setValue("recentNoteFolders", recentNoteFolders);
    }

    // clear menu list
    ui->menuRecentNoteFolders->clear();

    const QSignalBlocker blocker(ui->recentNoteFolderComboBox);
    {
        Q_UNUSED(blocker);
        ui->recentNoteFolderComboBox->clear();
        ui->recentNoteFolderComboBox->addItem(currentFolderName);

        // populate menu list
        Q_FOREACH(QString noteFolder, recentNoteFolders) {
                QDir folder(noteFolder);

                // don't show not existing folders or if path is empty
                if (!folder.exists() || noteFolder.isEmpty()) {
                    continue;
                }

                // add a menu entry
                QAction *action =
                        ui->menuRecentNoteFolders->addAction(noteFolder);
                QObject::connect(
                        action, SIGNAL(triggered()),
                        recentNoteFolderSignalMapper, SLOT(map()));

                // add a parameter to changeNoteFolder with the signal mapper
                recentNoteFolderSignalMapper->setMapping(action, noteFolder);

                // add an entry to the combo box
                ui->recentNoteFolderComboBox->addItem(noteFolder);
            }

        QObject::connect(recentNoteFolderSignalMapper,
                         SIGNAL(mapped(const QString &)),
                         this,
                         SLOT(changeNoteFolder(const QString &)));

        ui->recentNoteFolderComboBox->setCurrentIndex(0);
    }
}

/*
 * Set a new note folder
 */
void MainWindow::changeNoteFolder(const QString &folderName) {
    QString oldPath = this->notesPath;

    // reload notes if notes folder was changed
    if (oldPath != folderName) {
        // store everything before changing folder
        storeUpdatedNotesToDisk();

        // update the recent note folder list
        storeRecentNoteFolder(this->notesPath, folderName);

        // change notes path
        this->notesPath = folderName;

        // store notesPath setting
        QSettings settings;
        settings.setValue("notesPath", folderName);

        // we have to unset the current note otherwise it might show up after
        // switching to an other note folder
        currentNote = Note();

        buildNotesIndex();
        loadNoteDirectoryList();

        const QSignalBlocker blocker(this->ui->noteTextEdit);
        {
            Q_UNUSED(blocker);
            ui->noteTextEdit->clear();
            ui->noteTextEdit->show();
            ui->encryptedNoteTextEdit->hide();
        }

        const QSignalBlocker blocker2(this->ui->searchLineEdit);
        {
            Q_UNUSED(blocker2);
            ui->searchLineEdit->clear();
        }

        this->ui->noteTextView->clear();

        // update the current folder tooltip
        updateCurrentFolderTooltip();

        // clear the note history
        this->noteHistory.clear();
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
    loadRecentNoteFolderListMenu(removeFolderName);
}

int MainWindow::openNoteDiffDialog(Note changedNote) {
    if (this->noteDiffDialog->isVisible()) {
        this->noteDiffDialog->close();
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

void MainWindow::setupMainSplitter() {
    this->mainSplitter = new QSplitter;

    this->mainSplitter->addWidget(ui->notesListFrame);
    this->mainSplitter->addWidget(ui->noteTabWidget);

    // restore splitter sizes
    QSettings settings;
    QByteArray state = settings.value("mainSplitterSizes").toByteArray();
    this->mainSplitter->restoreState(state);

    this->ui->gridLayout->layout()->addWidget(this->mainSplitter);
}

void MainWindow::createSystemTrayIcon() {
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/icon.png"));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(
                    systemTrayIconClicked(QSystemTrayIcon::ActivationReason)));
    if (showSystemTray) {
        trayIcon->show();
    }
}

void MainWindow::loadNoteDirectoryList() {
    {
        const QSignalBlocker blocker(ui->noteTextEdit);
        Q_UNUSED(blocker);

        {
            const QSignalBlocker blocker2(ui->notesListWidget);
            Q_UNUSED(blocker2);

            ui->notesListWidget->clear();

            QStringList nameList = Note::fetchNoteNames();
            ui->notesListWidget->addItems(nameList);

            // clear the text edits if there are no notes
            if (nameList.isEmpty()) {
                ui->noteTextEdit->clear();
                ui->noteTextView->clear();
            }

            int itemCount = nameList.count();
            MetricsService::instance()->sendEventIfEnabled(
                    "note/list/loaded",
                    "note",
                    "note list loaded",
                    QString::number(itemCount) + " notes",
                    itemCount);
        }
    }

    QDir dir(this->notesPath);
    if (dir.exists()) {
        // watch the notes directory for changes
        this->noteDirectoryWatcher.addPath(this->notesPath);
    }

    QStringList fileNameList = Note::fetchNoteFileNames();

    // watch all the notes for changes
    Q_FOREACH(QString fileName, fileNameList) {
        QString path = Note::getFullNoteFilePathForFile(fileName);
        QFile file(path);
        if (file.exists()) {
            this->noteDirectoryWatcher.addPath(path);
        }
    }

    // sort alphabetically again if necessary
    if (sortAlphabetically) {
        ui->notesListWidget->sortItems(Qt::AscendingOrder);
    }

//    QStringList directoryList = this->noteDirectoryWatcher.directories();

//    Q_FOREACH(QString directory, directoryList)
//        qDebug() << "Directory name" << directory <<"\n";
}

/**
 * @brief makes the current note the first item in the note list without reloading the whole list
 */
void MainWindow::makeCurrentNoteFirstInNoteList() {
    QString name = this->currentNote.getName();
    QList<QListWidgetItem *> items =
            this->ui->notesListWidget->findItems(name, Qt::MatchExactly);
    if (items.count() > 0) {
        const QSignalBlocker blocker(this->ui->notesListWidget);
        Q_UNUSED(blocker);

        ui->notesListWidget->takeItem(ui->notesListWidget->row(items[0]));
        ui->notesListWidget->insertItem(0, items[0]);
        this->ui->notesListWidget->setCurrentItem(items[0]);
    }
}

void MainWindow::readSettings() {
    QSettings settings;
    sortAlphabetically = settings.value(
            "SortingModeAlphabetically", QVariant(false)).toBool();
    showSystemTray = settings.value("ShowSystemTray", QVariant(false)).toBool();
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());
    restoreState(settings.value("MainWindow/windowState").toByteArray());
    ui->menuBar->restoreGeometry(
            settings.value("MainWindow/menuBarGeometry").toByteArray());

    // read all relevant settings, that can be set in the settings dialog
    readSettingsFromSettingsDialog();

    // check legacy setting
    this->notesPath = settings.value("General/notesPath").toString();

    // migration: remove old setting if we found one and store new one
    if (this->notesPath != "") {
        settings.setValue("notesPath", this->notesPath);
        settings.remove("General/notesPath");
    } else {
        this->notesPath = settings.value("notesPath").toString();
    }

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
}

/**
 * @brief Reads all relevant settings, that can be set in the settings dialog
 */
void MainWindow::readSettingsFromSettingsDialog() {
    QSettings settings;

    // set the view mode
    setNoteTextEditMode(
            !settings.value("MainWindow/markdownDefaultViewMode").toBool());

    // disable the automatic update dialog per default for repositories and
    // self-builds
    if (settings.value("disableAutomaticUpdateDialog").toString().isEmpty()) {
        QString release = QString(RELEASE);
        bool enabled =
                release.contains("Travis") || release.contains("AppVeyor");
        settings.setValue("disableAutomaticUpdateDialog", !enabled);
    }

    this->notifyAllExternalModifications =
            settings.value("notifyAllExternalModifications").toBool();
    this->noteSaveIntervalTime = settings.value("noteSaveIntervalTime").toInt();

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
        settings.setValue(
                "MainWindow/mainToolBar.iconSize",
                QString::number(toolBarIconSize));
    } else {
        QSize size(toolBarIconSize, toolBarIconSize);
        ui->mainToolBar->setIconSize(size);
    }


    // check if we want to view the recent note folder combo box
    ui->recentNoteFolderComboBox->setVisible(
            settings.value(
                    "MainWindow/showRecentNoteFolderInMainArea").toBool());
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
                    this->currentNote.store();
                    this->currentNote.storeNoteTextFileToDisk();
                    showStatusBarMessage(
                            tr("stored current note to disk"), 1000);

                    // just to make sure everything is uptodate
//                        this->currentNote = note;
//                        this->setNoteTextFromNote( &note, true );

                    // wait 100ms before the block on this->noteDirectoryWatcher
                    // is opened, otherwise we get the event
                    waitMsecs(100);
                }
                    break;

                // reload note file from disk
                case NoteDiffDialog::Reload:
                    updateNoteTextFromDisk(note);
                    break;

                case NoteDiffDialog::Cancel:
                case NoteDiffDialog::Ignore:
                default:
                    // do nothing
                    break;
            }
        } else {
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
                            tr("stored current note to disk"), 1000);

                    // rebuild and reload the notes directory list
                    buildNotesIndex();
                    loadNoteDirectoryList();

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

        // rebuild and reload the notes directory list
        buildNotesIndex();
        loadNoteDirectoryList();
        setCurrentNote(this->currentNote, false);
    }
}

void MainWindow::notesDirectoryWasModified(const QString &str) {
    qDebug() << "notesDirectoryWasModified: " << str;

    // rebuild and reload the notes directory list
    buildNotesIndex();
    loadNoteDirectoryList();

    // also update the text of the text edit if current note has changed
    bool updateNoteText = !this->currentNote.exists();
    qDebug() << "updateNoteText: " << updateNoteText;

    // restore old selected row (but don't update the note text)
    setCurrentNote(this->currentNote, updateNoteText);
}

void MainWindow::storeUpdatedNotesToDisk() {
    {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker);

        QString oldNoteName = this->currentNote.getName();

        // For some reason this->noteDirectoryWatcher gets an event from this.
        // I didn't find an other solution than to wait yet.
        // All flushing and syncing didn't help.
        int count = Note::storeDirtyNotesToDisk(this->currentNote);

        if (count > 0) {
            MetricsService::instance()
                    ->sendEventIfEnabled(
                            "note/notes/stored",
                            "note",
                            "notes stored",
                            QString::number(count) + " notes",
                            count);

            qDebug() << __func__ << " - 'count': " << count;

            showStatusBarMessage(
                    tr("stored %n note(s) to disk", "", count),
                    1000);

            // wait 100ms before the block on this->noteDirectoryWatcher
            // is opened, otherwise we get the event
            waitMsecs(100);

            // just to make sure everything is uptodate
            this->currentNote.refetch();

            QString newNoteName = this->currentNote.getName();
            if (oldNoteName == newNoteName) {
                // if note name has not changed makes the current note
                // the first item in the note list without
                // reloading the whole list
                makeCurrentNoteFirstInNoteList();
            } else {
                // reload the directory list if note name has changed
                loadNoteDirectoryList();
            }
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

void MainWindow::waitMsecs(int msecs) {
    QTime dieTime = QTime::currentTime().addMSecs(msecs);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 50);
}

void MainWindow::buildNotesIndex() {
    // make sure we destroy nothing
    storeUpdatedNotesToDisk();

    QDir notesDir(this->notesPath);

    // only show markdown and text files
    QStringList filters;
    filters << "*.txt" << "*.md";

    // show newest entry first
    QStringList files = notesDir.entryList(filters, QDir::Files, QDir::Time);
    qDebug() << __func__ << " - 'files': " << files;

    bool createDemoNotes = files.count() == 0;

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
                "GitHub Flavored Markdown.md" <<
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

    // delete all notes in the database first
    Note::deleteAll();

    // create all notes from the files
    Q_FOREACH(QString fileName, files) {
            // fetching the content of the file
            QFile file(Note::getFullNoteFilePathForFile(fileName));
            Note note;
            note.createFromFile(file);
        }

    // re-fetch current note (because all the IDs have changed after the
    // buildNotesIndex()
    currentNote.refetch();

    if (cryptoKey != 0) {
        // reset the old crypto key for the current note
        currentNote.setCryptoKey(cryptoKey);
        currentNote.store();
    }
}

/**
 * @brief Jumps to the welcome note in the note selector
 */
void MainWindow::jumpToWelcomeNote() {
    // search for the welcome note
    QList<QListWidgetItem *> items = ui->notesListWidget->
            findItems("Welcome to QOwnNotes", Qt::MatchExactly);
    if (items.count() > 0) {
        // set the welcome note as current note
        ui->notesListWidget->setCurrentItem(items.at(0));
    }
}

QString MainWindow::selectOwnCloudNotesFolder() {
    QString path = this->notesPath;

    if (path == "") {
        path = QDir::homePath() + QDir::separator() +
                "ownCloud" + QDir::separator() + "Notes";
    }

    // TODO(pbek): We sometimes seem to get a "QCoreApplication::postEvent:
    // Unexpected null receiver" here.
    QString dir = QFileDialog::getExistingDirectory(
            this,
            tr("Select ownCloud folder"),
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
        settings.setValue("notesPath", dir);

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
    this->setWindowTitle(name + " - QOwnNotes " + QString(VERSION));

    // set the note text edit to readonly if note file is not writable
    QFileInfo *f = new QFileInfo(
            this->notesPath + QDir::separator() + note.getFileName());
    ui->noteTextEdit->setReadOnly(!f->isWritable());
    ui->encryptedNoteTextEdit->setReadOnly(!f->isWritable());

    // find and set the current item
    if (updateSelectedNote) {
        QList<QListWidgetItem *> items = this->ui->notesListWidget->findItems(
                name, Qt::MatchExactly);
        if (items.count() > 0) {
            const QSignalBlocker blocker(this->ui->notesListWidget);
            Q_UNUSED(blocker);

            this->ui->notesListWidget->setCurrentItem(items[0]);
        }
    }

    // update the text of the text edit
    if (updateNoteText) {
        const QSignalBlocker blocker(this->ui->noteTextEdit);
        Q_UNUSED(blocker);

        this->setNoteTextFromNote(&note);

        // hide the encrypted note text edit by default and show the regular one
        ui->encryptedNoteTextEdit->hide();
        ui->noteTextEdit->show();
    }

    updateEncryptNoteButtons();
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
            QList<QListWidgetItem*> noteList =
                    ui->notesListWidget->findItems(currentNote.getName(),
                                           Qt::MatchExactly);

            if (noteList.count() > 0) {
                const QSignalBlocker blocker1(ui->notesListWidget);
                Q_UNUSED(blocker1);

                const QSignalBlocker blocker2(ui->noteTextEdit);
                Q_UNUSED(blocker2);

                const QSignalBlocker blocker3(ui->noteTextView);
                Q_UNUSED(blocker3);

                const QSignalBlocker blocker4(ui->encryptedNoteTextEdit);
                Q_UNUSED(blocker4);

                const QSignalBlocker blocker5(noteDirectoryWatcher);
                Q_UNUSED(blocker5);

                // delete note in database and on file system
                currentNote.remove(true);

                ui->noteTextEdit->clear();
                ui->noteTextView->clear();
                ui->encryptedNoteTextEdit->clear();

                // delete item in note list widget
                delete noteList[0];

                // set a new first note
                resetCurrentNote();
            }

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
    // set new current note
    if (ui->notesListWidget->count() > 0) {
        const QSignalBlocker blocker(ui->notesListWidget);
        Q_UNUSED(blocker);

        ui->notesListWidget->setCurrentRow(0);

        Note note = Note::fetchByName(
                ui->notesListWidget->currentItem()->text());
        setCurrentNote(note, true, false);
    }
}

void MainWindow::storeSettings() {
    QSettings settings;

    bool isInDistractionFreeMode =
            settings.value("DistractionFreeMode/isEnabled").toBool();

    // don't store the window settings in distraction free mode
    if (!isInDistractionFreeMode) {
        settings.setValue("MainWindow/geometry", saveGeometry());
        settings.setValue("MainWindow/windowState", saveState());
        settings.setValue("mainSplitterSizes", this->mainSplitter->saveState());
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
    MetricsService::instance()->sendVisitIfEnabled("app/end", "app end");
    storeSettings();
    QMainWindow::closeEvent(event);
}

//
// Event filters on the MainWindow
//
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);

        if (obj == ui->searchLineEdit) {
            // set focus to the notes list if Key_Down or Key_Tab were
            // pressed in the search line edit
            if ((keyEvent->key() == Qt::Key_Down) ||
                    (keyEvent->key() == Qt::Key_Tab)) {
                // choose an other selected item if current item is invisible
                QListWidgetItem *item = ui->notesListWidget->currentItem();
                if ((item != NULL) &&
                        ui->notesListWidget->currentItem()->isHidden() &&
                        (this->firstVisibleNoteListRow >= 0)) {
                    ui->notesListWidget->setCurrentRow(
                            this->firstVisibleNoteListRow);
                }

                // give the keyboard focus to the notes list widget
                ui->notesListWidget->setFocus();
                return true;
            }
            return false;
        } else if (obj == ui->notesListWidget) {
            // set focus to the note text edit if Key_Return or Key_Tab were
            // pressed in the notes list
            if ((keyEvent->key() == Qt::Key_Return) ||
                    (keyEvent->key() == Qt::Key_Tab)) {
                setNoteTextEditMode(true);
                focusNoteTextEdit();
                return true;
            } else if ((keyEvent->key() == Qt::Key_Delete)) {
                removeSelectedNotes();
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
 * highlights all occurrences of str in the note text edit
 */
void MainWindow::searchInNoteTextEdit(QString &str) {
    QList<QTextEdit::ExtraSelection> extraSelections;
    QList<QTextEdit::ExtraSelection> extraSelections2;
    QList<QTextEdit::ExtraSelection> extraSelections3;

    if (str.count() >= 2) {
        ui->noteTextEdit->moveCursor(QTextCursor::Start);
        ui->noteTextView->moveCursor(QTextCursor::Start);
        ui->encryptedNoteTextEdit->moveCursor(QTextCursor::Start);
        QColor color = QColor(0, 180, 0, 100);

        while (ui->noteTextEdit->find(str)) {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground(color);

            extra.cursor = ui->noteTextEdit->textCursor();
            extraSelections.append(extra);
        }

        while (ui->noteTextView->find(str)) {
            QTextEdit::ExtraSelection extra;
            extra.format.setBackground(color);

            extra.cursor = ui->noteTextView->textCursor();
            extraSelections2.append(extra);
        }

        while (ui->encryptedNoteTextEdit->find(str)) {
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
 * switch edit mode for noteTextEdit on or off
 */
void MainWindow::setNoteTextEditMode(bool isInEditMode) {
    this->noteTextEditIsInEditMode = isInEditMode;
    this->ui->actionToggleEditMode->setChecked(isInEditMode);
    this->ui->actionToggleEditMode->setToolTip(
            "Toggle edit mode - currently " +
                    QString(isInEditMode ? "editing" : "viewing"));

    // set the tab index
    {
        const QSignalBlocker blocker(this->ui->noteTabWidget);
        Q_UNUSED(blocker);
        this->ui->noteTabWidget->setCurrentIndex(isInEditMode ? 0 : 1);
    }

    if (!isInEditMode) {
        // ask for the password if the note is encrypted and can't be decrypted
        askForEncryptedNotePasswordIfNeeded();
    }

    // make sure the current note is set
    if (this->currentNote.exists()) {
        this->setCurrentNote(this->currentNote, true);
    }

    // restore search after switching between modes
    if (ui->searchLineEdit->text() != "") {
        searchForSearchLineTextInNoteTextEdit();
    }
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
 * Sets the note text according to a note
 */
void MainWindow::setNoteTextFromNote(Note *note, bool updateNoteTextViewOnly) {
    if (!updateNoteTextViewOnly) {
        this->ui->noteTextEdit->setText(note->getNoteText());
    }

    this->ui->noteTextView->setHtml(note->toMarkdownHtml(notesPath));
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
 */
void MainWindow::createNewNote(QString name, QString text) {
    QString extension = Note::defaultNoteFileExtension();
    QFile *f = new QFile(this->notesPath + QDir::separator() + name + "." + extension);

    // change the name and headline if note exists
    if (f->exists()) {
        QDateTime currentDate = QDateTime::currentDateTime();
        name.append(" " + currentDate.toString(Qt::ISODate).replace(":", "."));

        QString preText = name + "\n";

        for (int i = 0; i < name.length(); i++) {
            preText.append("=");
        }

        preText.append("\n\n");
        text.prepend(preText);
    }

    ui->searchLineEdit->setText(name);
    on_searchLineEdit_returnPressed();
    ui->noteTextEdit->setText(text);
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
    ownCloud->restoreTrashedNoteOnServer(
            this->notesPath, fileName, timestamp, this);
}

/**
 * @brief Removes selected notes after a confirmation
 */
void MainWindow::removeSelectedNotes() {
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    int selectedItemsCount = ui->notesListWidget->selectedItems().size();

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

        const QSignalBlocker blocker1(ui->notesListWidget);
        Q_UNUSED(blocker1);

        const QSignalBlocker blocker2(ui->noteTextEdit);
        Q_UNUSED(blocker2);

        const QSignalBlocker blocker3(ui->noteTextView);
        Q_UNUSED(blocker3);

        const QSignalBlocker blocker4(ui->encryptedNoteTextEdit);
        Q_UNUSED(blocker4);

        Q_FOREACH(QListWidgetItem *item, ui->notesListWidget->selectedItems()) {
            QString name = item->text();
            Note note = Note::fetchByName(name);
            note.remove(true);
            qDebug() << "Removed note " << name;
        }

        loadNoteDirectoryList();

        // set a new first note
        resetCurrentNote();
    }
}

/**
 * @brief Select all notes
 */
void MainWindow::selectAllNotes() {
    ui->notesListWidget->selectAll();
}

/**
 * @brief Moves selected notes after a confirmation
 * @param destinationFolder
 */
void MainWindow::moveSelectedNotesToFolder(QString destinationFolder) {
    // store updated notes to disk
    storeUpdatedNotesToDisk();

    int selectedItemsCount = ui->notesListWidget->selectedItems().size();

    if (QMessageBox::information(
            this,
            tr("Move selected notes"),
            tr("Move %n selected note(s) to <strong>%2</strong>?", "",
               selectedItemsCount).arg(destinationFolder),
            tr("&Move"), tr("&Cancel"), QString::null,
            0, 1) == 0) {
        const QSignalBlocker blocker(this->noteDirectoryWatcher);
        Q_UNUSED(blocker);

        Q_FOREACH(QListWidgetItem *item, ui->notesListWidget->selectedItems()) {
                QString name = item->text();
                Note note = Note::fetchByName(name);

                // move note
                bool result = note.move(destinationFolder);
                if (result) {
                    qDebug() << "Note was moved:" << note.getName();
                } else {
                    qDebug() << "Could not move note:" << note.getName();
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
    int selectedItemsCount = ui->notesListWidget->selectedItems().size();

    if (QMessageBox::information(
            this,
            tr("Copy selected notes"),
            tr("Copy %n selected note(s) to <strong>%2</strong>?", "",
               selectedItemsCount).arg(destinationFolder),
            tr("&Copy"), tr("&Cancel"), QString::null, 0, 1) == 0) {
        int copyCount = 0;
        Q_FOREACH(QListWidgetItem *item, ui->notesListWidget->selectedItems()) {
                QString name = item->text();
                Note note = Note::fetchByName(name);

                // copy note
                bool result = note.copy(destinationFolder);
                if (result) {
                    copyCount++;
                    qDebug() << "Note was copied:" << note.getName();
                } else {
                    qDebug() << "Could not copy note:" << note.getName();
                }
            }

        QMessageBox::information(
                this, tr("Done"),
                tr("%n note(s) were copied to <strong>%2</strong>.", "",
                   copyCount).arg(destinationFolder));
    }
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
 * @brief Opens the settings dialog
 */
void MainWindow::openSettingsDialog(int tab) {
    // open the settings dialog
    SettingsDialog *dialog = new SettingsDialog(tab, this);
    int dialogResult = dialog->exec();

    if (dialogResult == QDialog::Accepted) {
        // read all relevant settings, that can be set in the settings dialog
        readSettingsFromSettingsDialog();

        // reset the note save timer
        this->noteSaveTimer->stop();
        this->noteSaveTimer->start(this->noteSaveIntervalTime * 1000);
    }

    // reload recent note folder in case we have cleared
    // the history in the settings
    loadRecentNoteFolderListMenu(notesPath);
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

    QNetworkReply *reply = manager->get(QNetworkRequest(url));
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
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setDirectory(QDir::homePath());
    dialog.setNameFilter(tr("PDF files (*.pdf)"));
    dialog.setWindowTitle(tr("Export current note as PDF"));
    dialog.selectFile(currentNote.getName() + ".pdf");
    int ret = dialog.exec();

    if (ret == QDialog::Accepted) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.count() > 0) {
            QString fileName = fileNames.at(0);

            if (QFileInfo(fileName).suffix().isEmpty()) {
                fileName.append(".pdf");
            }

            QPrinter printer(QPrinter::HighResolution);
            printer.setOutputFormat(QPrinter::PdfFormat);
            printer.setOutputFileName(fileName);
            textEdit->document()->print(&printer);
        }
    }
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
            openSettingsDialog(SettingsDialog::GeneralTab);
        }
    }
}



// *****************************************************************************
// *
// *
// * Slot implementations
// *
// *
// *****************************************************************************

void MainWindow::on_notesListWidget_currentItemChanged(
        QListWidgetItem *current, QListWidgetItem *previous) {
    Q_UNUSED(previous);

    // in case all notes were removed
    if (current == NULL) {
        return;
    }

    qDebug() << "currentItemChanged " << current->text();

    Note note = Note::fetchByName(current->text());
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

void MainWindow::on_noteTextEdit_textChanged() {
    Note note = this->currentNote;
    note.updateNoteTextFromDisk();
    QString noteTextFromDisk = note.getNoteText();

    QString text = this->ui->noteTextEdit->toPlainText();

    if (text != noteTextFromDisk) {
        this->currentNote.storeNewText(text);
        this->currentNote.refetch();
        this->currentNoteLastEdited = QDateTime::currentDateTime();

        updateEncryptNoteButtons();
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

    QString oldPath = this->notesPath;
    selectOwnCloudNotesFolder();

    // reload notes if notes folder was changed
    if (oldPath != this->notesPath) {
        buildNotesIndex();
        loadNoteDirectoryList();

        ui->noteTextEdit->show();
        ui->encryptedNoteTextEdit->hide();

        const QSignalBlocker blocker(this->ui->noteTextEdit);
        {
            Q_UNUSED(blocker);
            ui->noteTextEdit->clear();
        }

        ui->noteTextView->clear();
    }
}

void MainWindow::on_searchLineEdit_textChanged(const QString &arg1) {
    // search notes when at least 2 characters were entered
    if (arg1.count() >= 2) {
        QList<QString> noteNameList = Note::searchAsNameList(arg1);
        this->firstVisibleNoteListRow = -1;

        for (int i = 0; i < this->ui->notesListWidget->count(); ++i) {
            QListWidgetItem *item = this->ui->notesListWidget->item(i);
            if (noteNameList.indexOf(item->text()) < 0) {
                item->setHidden(true);
            } else {
                if (this->firstVisibleNoteListRow < 0) {
                    this->firstVisibleNoteListRow = i;
                }
                item->setHidden(false);
            }
        }
    } else {
        // show all items otherwise
        this->firstVisibleNoteListRow = 0;

        for (int i = 0; i < this->ui->notesListWidget->count(); ++i) {
            QListWidgetItem *item = this->ui->notesListWidget->item(i);
            item->setHidden(false);
        }
    }

    // let's highlight the text from the search line edit
    searchForSearchLineTextInNoteTextEdit();
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
    QString text = this->ui->searchLineEdit->text();
    text = text.trimmed();

    // first let us search for the entered text
    Note note = Note::fetchByName(text);

    // if we can't find a note we create a new one
    if (note.getId() == 0) {
        // create a headline in new notes by adding "=====" as second line
        QString noteText = text + "\n";
        for (int i = 0; i < text.length(); i++) {
            noteText.append("=");
        }
        noteText.append("\n\n");

        note = Note();
        note.setName(text);
        note.setNoteText(noteText);
        note.store();

        // store the note to disk
        {
            const QSignalBlocker blocker(this->noteDirectoryWatcher);
            Q_UNUSED(blocker);

            note.storeNoteTextFileToDisk();
            showStatusBarMessage(
                    tr("stored current note to disk"), 1000);
        }

        buildNotesIndex();
        loadNoteDirectoryList();

        // fetch note new (because all the IDs have changed after
        // the buildNotesIndex()
        note.refetch();

//        // create a new widget item for the note list
//        QListWidgetItem* widgetItem = new QListWidgetItem();
//        widgetItem->setText( text );

//        // insert the note at the top of the note list
//        {
//            const QSignalBlocker blocker( this->ui->notesListWidget );

//            ui->notesListWidget->insertItem( 0, widgetItem );
//        }
    }

    // jump to the found or created note
    setCurrentNote(note);

    // go into edit mode
    setNoteTextEditMode(true);

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
// hotkey to create new note with date in name
//
void MainWindow::on_action_Note_note_triggered() {
    QDateTime currentDate = QDateTime::currentDateTime();

    // replacing ":" with "_" for Windows systems
    QString text =
            "Note " + currentDate.toString(Qt::ISODate).replace(":", ".");
    this->ui->searchLineEdit->setText(text);
    on_searchLineEdit_returnPressed();
}

void MainWindow::on_actionToggleEditMode_triggered() {
    this->setNoteTextEditMode(this->ui->actionToggleEditMode->isChecked());
}

void MainWindow::on_noteTabWidget_currentChanged(int index) {
    this->setNoteTextEditMode(index == 0);
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

    if (url.scheme() == "note") {
        openNoteUrl(url);
    } else {
        ui->noteTextEdit->openUrl(url);
    }
}

/*
 * Handles note urls
 *
 * examples:
 * - <note://MyNote> opens the note "MyNote"
 * - <note://my-note-with-spaces-in-the-name> opens the note "My Note with spaces in the name"
 */
void MainWindow::openNoteUrl(QUrl url) {
    qDebug() << __func__ << " - 'url': " << url;

    if (url.scheme() == "note") {
        QString fileName = url.host();

        // this makes it possible to search for file names containing spaces
        // instead of spaces a "-" has to be used in the note link
        // example: note://my-note-with-spaces-in-the-name
        fileName.replace("-", "?").replace("_", "?");

        // we need to search for the case sensitive filename,
        // we only get it lowercase by QUrl
        QDir currentDir = QDir(this->notesPath);
        QStringList files;
        QStringList fileSearchList =
                QStringList() << fileName + ".txt" << fileName + ".md";

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
        ui->notesListWidget->sortItems(Qt::AscendingOrder);
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
    if (reason == QSystemTrayIcon::Trigger) {
        if (this->isVisible()) {
            this->hide();
        } else {
            this->show();
        }
    }
}

void MainWindow::on_actionShow_system_tray_triggered(bool checked) {
    showSystemTray = checked;
    if (checked) {
        trayIcon->show();
    } else {
        trayIcon->hide();
    }
}

void MainWindow::on_action_Settings_triggered() {
    // open the settings dialog
    openSettingsDialog();
}

void MainWindow::on_actionShow_versions_triggered() {
    ui->actionShow_versions->setDisabled(true);
    showStatusBarMessage(
            tr("note versions are currently loaded from your ownCloud server"),
            20000);

    OwnCloudService *ownCloud = new OwnCloudService(this);
    ownCloud->loadVersions(
            this->notesPath, this->currentNote.getFileName(), this);
}

void MainWindow::enableShowVersionsButton() {
    ui->actionShow_versions->setDisabled(false);
}

void MainWindow::on_actionShow_trash_triggered() {
    ui->actionShow_trash->setDisabled(true);
    showStatusBarMessage(
            tr("trashed notes are currently loaded from your ownCloud server"),
            20000);

    OwnCloudService *ownCloud = new OwnCloudService(this);
    ownCloud->loadTrash(this->notesPath, this);
}

void MainWindow::enableShowTrashButton() {
    ui->actionShow_trash->setDisabled(false);
}

void MainWindow::on_notesListWidget_customContextMenuRequested(
        const QPoint &pos) {
    QPoint globalPos = ui->notesListWidget->mapToGlobal(pos);
    QMenu noteMenu;
    QMenu *moveDestinationMenu = new QMenu();
    QMenu *copyDestinationMenu = new QMenu();
    QSettings settings;
    QStringList recentNoteFolders =
            settings.value("recentNoteFolders").toStringList();

    // show copy and move menu entries only
    // if there is at least one notes folder
    if (recentNoteFolders.size() > 0) {
        moveDestinationMenu = noteMenu.addMenu(tr("&Move notes to..."));
        copyDestinationMenu = noteMenu.addMenu(tr("&Copy notes to..."));

        // add actions for the recent note folders
        Q_FOREACH(QString noteFolder, recentNoteFolders) {
                if (noteFolder != this->notesPath) {
                    moveDestinationMenu->addAction(noteFolder);
                    copyDestinationMenu->addAction(noteFolder);
                }
            }
    }

    QAction *removeAction = noteMenu.addAction(tr("&Remove notes"));
    noteMenu.addSeparator();
    QAction *selectAllAction = noteMenu.addAction(tr("Select &all notes"));

    QAction *selectedItem = noteMenu.exec(globalPos);
    if (selectedItem) {
        // move notes
        if (selectedItem->parent() == moveDestinationMenu) {
            QString destinationFolder = selectedItem->text();
            moveSelectedNotesToFolder(destinationFolder);
        } else if (selectedItem->parent() == copyDestinationMenu) {
            // copy notes
            QString destinationFolder = selectedItem->text();
            copySelectedNotesToFolder(destinationFolder);
        } else if (selectedItem == removeAction) {
            // remove notes
            removeSelectedNotes();
        } else if (selectedItem == selectAllAction) {
            // select all notes
            selectAllNotes();
        }
    }
}

void MainWindow::on_actionSelect_all_notes_triggered() {
    selectAllNotes();
}

/**
 * @brief create the additional menu entries for the note text edit field
 * @param pos
 */
void MainWindow::on_noteTextEdit_customContextMenuRequested(const QPoint &pos) {
    QPoint globalPos = ui->notesListWidget->mapToGlobal(pos);
    QMenu *menu = ui->noteTextEdit->createStandardContextMenu();

    menu->addSeparator();

    QString linkTextActionName =
            ui->noteTextEdit->textCursor().selectedText() != "" ?
                tr("&Link selected text") : tr("Insert &link");
    QAction *linkTextAction = menu->addAction(linkTextActionName);
    linkTextAction->setShortcut(QKeySequence("Ctrl+L"));

    QAction *selectedItem = menu->exec(globalPos);
    if (selectedItem) {
        if (selectedItem == linkTextAction) {
            // handle the linking of text with a note
            handleTextNoteLinking();
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
    QSettings settings;
    QStringList todoCalendarEnabledUrlList =
            settings.value("ownCloud/todoCalendarEnabledUrlList")
                    .toStringList();

    // check if we have got any todo list enabled
    if (todoCalendarEnabledUrlList.count() == 0) {
        if (QMessageBox::warning(
                0, tr("No selected todo lists!"),
                tr("You have not selected any todo lists.<br />"
                           "Please check your <strong>Todo</strong>"
                           "configuration in the settings!"),
                tr("Open &settings"),
                tr("&Cancel"),
                QString::null, 0, 1) == 0) {
            openSettingsDialog();
        }

        return;
    }

    TodoDialog *dialog = new TodoDialog(this);
    dialog->exec();
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
    QString text = getInsertMediaMarkdown(file);
    if (!text.isEmpty()) {
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
 * Returns the markdown of the inserted media file into a note
 */
QString MainWindow::getInsertMediaMarkdown(QFile *file) {
    if (file->exists() && (file->size() > 0)) {
        QDir mediaDir(notesPath + QDir::separator() + "media");

        // created the media folder if it doesn't exist
        if (!mediaDir.exists()) {
            mediaDir.mkpath(mediaDir.path());
        }

        QFileInfo fileInfo(file->fileName());

        // find a random name for the new file
        QString newFileName =
                QString::number(qrand()) + "." + fileInfo.suffix();

        // copy the file the the media folder
        file->copy(mediaDir.path() + QDir::separator() + newFileName);

        // return the image link
        // we add a "\n" in the end so that hoedown recognizes multiple images
        return "![" + fileInfo.baseName() + "](file://media/" +
                newFileName + ")\n";
    }

    return "";
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
    // if we are not in edit mode go into edit mode
    if (ui->noteTabWidget->currentIndex() != 0) {
        this->setNoteTextEditMode(true);
    }

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

    QString labelText =
            tr("Please enter your <strong>password</strong> to encrypt the note."
            "<br />Keep in mind that you have to <strong>remember</strong> "
            "your password to read the content of the note<br /> and that you "
           "can <strong>only</strong> do that <strong>in QOwnNotes</strong>!");
    PasswordDialog* dialog = new PasswordDialog(this, labelText, true);
    int dialogResult = dialog->exec();

    // if user pressed ok take the password
    if (dialogResult == QDialog::Accepted) {
        QString password = dialog->password();

        // if password wasn't empty encrypt the note
        if (!password.isEmpty()) {
            currentNote.setCryptoPassword(password);
            currentNote.store();
            QString noteText = currentNote.encryptNoteText();
            ui->noteTextEdit->setPlainText(noteText);
        }
    }
}

/**
 * Enables or disables the encrypt note buttons
 */
void MainWindow::updateEncryptNoteButtons()
{
    currentNote.refetch();
    bool hasEncryptedNoteText = currentNote.hasEncryptedNoteText();

    ui->action_Encrypt_note->setEnabled(!hasEncryptedNoteText);
    ui->actionEdit_encrypted_note->setEnabled(hasEncryptedNoteText);
    ui->actionDecrypt_note->setEnabled(hasEncryptedNoteText);
}

/**
 * Attempt to decrypt note text
 */
void MainWindow::on_actionDecrypt_note_triggered()
{
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
void MainWindow::on_actionEdit_encrypted_note_triggered()
{
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
    }
}

/**
 * Puts the encrypted text back to the note text edit
 */
void MainWindow::on_encryptedNoteTextEdit_textChanged()
{
    askForEncryptedNotePasswordIfNeeded();

    if (currentNote.canDecryptNoteText()) {
        // encrypt the note text
        currentNote.setNoteText(ui->encryptedNoteTextEdit->toPlainText());
        QString noteText = currentNote.encryptNoteText();

        // put it into the note text edit to be stored
        ui->noteTextEdit->setText(noteText);
    }
}

/**
 * Opens the current note in an external editor
 */
void MainWindow::on_action_Open_note_in_external_editor_triggered()
{
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
        qDebug() << __func__ << " - 'path': " << path;

        // open note file in external editor
        system(QString(
                externalEditorPath + " \"" + path + "\"")
                       .toStdString().c_str());
    }
}

/**
 * Exports the current note as markdown file
 */
void MainWindow::on_action_Export_note_as_markdown_triggered()
{
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

void MainWindow::on_actionGet_invloved_triggered()
{
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
            tr("bookmarked note position at slot %1").arg(
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
                tr("jumped to bookmark position at slot %1").arg(
                        QString::number(slot)), 3000);
    }
}

/**
 * Inserts a code block at the current cursor position
 */
void MainWindow::on_actionInset_code_block_triggered()
{
    QMarkdownTextEdit* textEdit = activeNoteTextEdit();
    QTextCursor c = textEdit->textCursor();
    QString selectedText = textEdit->textCursor().selectedText();

    if (selectedText.isEmpty()) {
        c.insertText("``");
        c.movePosition(QTextCursor::Left, QTextCursor::MoveAnchor);
        textEdit->setTextCursor(c);
    } else {
        c.insertText("`" + selectedText + "`");
    }
}

void MainWindow::on_actionNext_note_triggered()
{
    gotoNextNote();
}

/**
 * Jumps to the next visible note
 */
void MainWindow::gotoNextNote(int nextRow)
{
    if (firstVisibleNoteListRow == -1) {
        return;
    }

    // if no next row was set get one after the current row
    if (nextRow == -1) {
        nextRow = ui->notesListWidget->currentRow() + 1;
    }

    // if the row doesn't exist start with 0
    if (nextRow >= ui->notesListWidget->count()) {
        return gotoNextNote(0);
    }

    QListWidgetItem * item = ui->notesListWidget->item(nextRow);

    // if item is hidden try the next row
    if (item->isHidden()) {
        return gotoNextNote(nextRow + 1);
    }

    ui->notesListWidget->setCurrentRow(nextRow);
}

void MainWindow::on_actionPrevious_Note_triggered()
{
    gotoPreviousNote();
}

/**
 * Jumps to the previous visible note
 */
void MainWindow::gotoPreviousNote(int previousRow)
{
    if (firstVisibleNoteListRow == -1) {
        return;
    }

    // if no previous row was set get one before the current row
    if (previousRow == -1) {
        previousRow = ui->notesListWidget->currentRow() -1;
    }

    // if the row is below 0 use the last row
    if (previousRow < 0) {
        return gotoPreviousNote(ui->notesListWidget->count() - 1);
    }

    QListWidgetItem * item = ui->notesListWidget->item(previousRow);

    // if the item is hidden try the previous
    if (item->isHidden()) {
        previousRow--;

        // if the row is below 0 use the last row
        if (previousRow < 0) {
            previousRow = ui->notesListWidget->count() - 1;
        }

        return gotoPreviousNote(previousRow);
    }

    ui->notesListWidget->setCurrentRow(previousRow);
}

void MainWindow::on_actionToggle_distraction_free_mode_triggered()
{
    toggleDistractionFreeMode();
}

/**
 * Tracks an action
 */
void MainWindow::trackAction(QAction *action) {
    MetricsService::instance()->sendVisitIfEnabled(
            "action/" + action->objectName());
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    ui->noteTextEdit->setPaperMargins(event->size().width());
    ui->encryptedNoteTextEdit->setPaperMargins(event->size().width());
}

/**
 * Toggles the visibility of the main toolbar
 */
void MainWindow::on_actionShow_toolbar_triggered(bool checked)
{
    ui->mainToolBar->setVisible(checked);
}

/**
 * Toggles the checked state of the "show toolbar" checkbox in the main menu
 */
void MainWindow::mainToolbarVisibilityChanged(bool visible)
{
    const QSignalBlocker blocker(ui->actionShow_toolbar);
    {
        Q_UNUSED(blocker);
        ui->actionShow_toolbar->setChecked(visible);
    }
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
                        showStatusBarMessage(tr("inserting image"));

                        // insert the image
                        insertMedia(file);

                        showStatusBarMessage(tr("done inserting image"), 3000);
                    } else {
                        skipCount++;
                    }
                } else {
                    skipCount++;
                }
            }

        QString message;
        if (successCount > 0) {
            message += tr("copied %n note(s) to %1", "", successCount)
                    .arg(notesPath);
        }

        if (failureCount > 0) {
            if (!message.isEmpty()) {
                message += ", ";
            }

            message += tr(
                    "failed to copy %n note(s) (most likely already existing)",
                    "", failureCount);
        }

        if (skipCount > 0) {
            if (!message.isEmpty()) {
                message += ", ";
            }

            message += tr(
                    "skipped copying of %n note(s) "
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
            showStatusBarMessage(tr("saving temporary image"));

            QTemporaryFile tempFile(
                    QDir::tempPath() + QDir::separator() +
                    "qownnotes-media-XXXXXX.png");

            if (tempFile.open()) {
                // save temporary png image
                image.save(tempFile.fileName(), "PNG");

                // insert media into note
                QFile *file = new QFile(tempFile.fileName());

                showStatusBarMessage(tr("inserting image"));
                insertMedia(file);

                showStatusBarMessage(tr("done inserting image"), 3000);
            } else {
                showStatusBarMessage(
                        tr("temporary file can't be opened"), 3000);
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

    // remove some blocks
    html.remove(QRegularExpression(
            "<head[^>]*>([^<]+)<\\/head>",
            QRegularExpression::CaseInsensitiveOption));

    html.remove(QRegularExpression(
            "<script[^>]*>([^<]+)<\\/script>",
            QRegularExpression::CaseInsensitiveOption));

    html.remove(QRegularExpression(
            "<style[^>]*>([^<]+)<\\/style>",
            QRegularExpression::CaseInsensitiveOption));

    // replace some html tags with markdown
    html.replace(QRegularExpression(
            "<strong[^>]*>([^<]+)<\\/strong>",
            QRegularExpression::CaseInsensitiveOption), "**\\1**");
    html.replace(QRegularExpression(
            "<b[^>]*>([^<]+)<\\/b>",
            QRegularExpression::CaseInsensitiveOption), "**\\1**");
    html.replace(QRegularExpression(
            "<em[^>]*>([^<]+)<\\/em>",
            QRegularExpression::CaseInsensitiveOption), "*\\1*");
    html.replace(QRegularExpression(
            "<i[^>]*>([^<]+)<\\/i>",
            QRegularExpression::CaseInsensitiveOption), "*\\1*");
    html.replace(QRegularExpression(
            "<h1[^>]*>([^<]+)<\\/h1>",
            QRegularExpression::CaseInsensitiveOption), "\n# \\1\n");
    html.replace(QRegularExpression(
            "<h2[^>]*>([^<]+)<\\/h2>",
            QRegularExpression::CaseInsensitiveOption), "\n## \\1\n");
    html.replace(QRegularExpression(
            "<h3[^>]*>([^<]+)<\\/h3>",
            QRegularExpression::CaseInsensitiveOption), "\n### \\1\n");
    html.replace(QRegularExpression("<h4[^>]*>([^<]+)<\\/h4>",
            QRegularExpression::CaseInsensitiveOption), "\n#### \\1\n");
    html.replace(QRegularExpression(
            "<h5[^>]*>([^<]+)<\\/h5>",
            QRegularExpression::CaseInsensitiveOption), "\n##### \\1\n");
    html.replace(QRegularExpression(
            "<br[^>]*>",
            QRegularExpression::CaseInsensitiveOption), "\n");
    html.replace(QRegularExpression(
            "<a[^>]+href=\"([^\"]+)\"[^>]*>([^<]+)<\\/a>",
            QRegularExpression::CaseInsensitiveOption), "[\\2](\\1)");

    // match image tags
    QRegularExpression re("<img[^>]+src=\"([^\"]+)\"[^>]*>",
                          QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator i = re.globalMatch(html);

    // find, download locally and replace all images
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString imageTag = match.captured(0);
        QUrl imageUrl = QUrl(match.captured(1) );

        qDebug() << __func__ << " - 'imageUrl': " << imageUrl;

        if (!imageUrl.isValid()) {
            continue;
        }

        showStatusBarMessage(tr("downloading %1").arg(imageUrl.toString()));

        // try to get the suffix from the url
        QString suffix =
                imageUrl.toString().split(".", QString::SkipEmptyParts).last();
        if (suffix.isEmpty()) {
            suffix = "image";
        }

        QTemporaryFile *tempFile = new QTemporaryFile(
                QDir::tempPath() + QDir::separator() + "media-XXXXXX." +
                        suffix);

        if (tempFile->open()) {
            // download the image to the temporary file
            if (downloadUrlToFile(imageUrl, tempFile)) {
                // copy image to media folder and generate markdown code for
                // the image
                QString markdownCode = getInsertMediaMarkdown(tempFile);
                if (!markdownCode.isEmpty()) {
                    // replace image tag with markdown code
                    html.replace(imageTag, markdownCode);
                }
            }
        }
    }

    showStatusBarMessage(tr("done downloading images"));

    // remove all html tags
    html.remove(QRegularExpression("<[^>]*>"));

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

void MainWindow::on_actionShow_note_in_file_manager_triggered()
{
    Utils::Misc::openFolderSelect(currentNote.fullNoteFilePath());
}
