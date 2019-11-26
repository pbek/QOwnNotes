/*
 * Copyright (c) 2014-2019 Patrizio Bekerle -- http://www.bekerle.com
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

#pragma once

#include <QMainWindow>
#include <QListWidgetItem>
#include <QFileSystemWatcher>
#include <QHash>
#include <QFileDialog>
#include <QEvent>
#include <QSystemTrayIcon>
#include <QTextEdit>
#include <QFile>
#include <QToolBar>
#include <QTreeWidgetItem>
#include <QShortcut>
#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/tag.h>
#include <services/scriptingservice.h>
#include <QScrollArea>
#include <QDockWidget>
#include <QComboBox>
#include <dialogs/actiondialog.h>
#include <dialogs/tododialog.h>
#include <dialogs/settingsdialog.h>
#include <QPrinter>
#include <widgets/logwidget.h>
#include <dialogs/orphanedimagesdialog.h>
#include <dialogs/orphanedattachmentsdialog.h>
#include <dialogs/issueassistantdialog.h>
#include "entities/notehistory.h"
#include "dialogs/notediffdialog.h"
#include "services/updateservice.h"
#include "widgets/qownnotesmarkdowntextedit.h"
#include <libraries/fakevim/fakevim/fakevimhandler.h>
#include "services/websocketserverservice.h"

#define SORT_ALPHABETICAL 0
#define SORT_BY_LAST_CHANGE 1
#define ORDER_ASCENDING 0 // Qt::AscendingOrder // = 0
#define ORDER_DESCENDING 1 // Qt::DescendingOrder // = 1

namespace Ui {
class MainWindow;
}

class QOwnNotesMarkdownTextEdit;

// forward declaration because of "xxx does not name a type"
class TodoDialog;
class SettingsDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_PROPERTY(Note currentNote WRITE setCurrentNote MEMBER currentNote
                       NOTIFY currentNoteChanged)

Q_SIGNALS:
    void currentNoteChanged(Note &note);
    void log(LogWidget::LogType logType, QString text);


public:
    enum CreateNewNoteOption {
        None = 0x0000,

        // use the name parameter as headline too
        UseNameAsHeadline = 0x0001,

        // places the cursor at the end of the note
        CursorAtEnd = 0x0002,

        // don't call loadNoteDirectoryList
        DisableLoadNoteDirectoryList = 0x0004,
    };

    enum NoteTreeWidgetItemType {
        NoteType = 1,
        FolderType = 2,
    };

    Q_DECLARE_FLAGS(CreateNewNoteOptions, CreateNewNoteOption)

    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    void setCurrentNoteText(QString text);

    Note getCurrentNote();

    void createNewNote(
            QString name, QString text,
            CreateNewNoteOptions options = CreateNewNoteOption::None);

    void openSettingsDialog(int page = 0, bool openScriptRepository = false);

    void restoreTrashedNoteOnServer(const QString& fileName, int timestamp);

    void showUpdateAvailableButton(const QString& version);

    void hideUpdateAvailableButton();

    void enableShowVersionsButton();

    void enableShowTrashButton();

    void showStatusBarMessage(const QString & message, int timeout = 0);

    void handleInsertingFromMimeData(const QMimeData *mimeData);

    QOwnNotesMarkdownTextEdit *activeNoteTextEdit();

    static MainWindow *instance();

    QList<QMenu *> menuList();

    QAction *findAction(const QString& objectName);

    void addCustomAction(const QString& identifier, const QString& menuText,
                         const QString& buttonText, const QString& icon,
                         bool useInNoteEditContextMenu = false,
                         bool hideButtonInToolbar = false,
                         bool useInNoteListContextMenu = false);

    void addScriptingLabel(const QString& identifier, const QString& text = "");

    void setScriptingLabelText(const QString& identifier, const QString& text);

    void preReloadScriptingEngine();

    void writeToNoteTextEdit(const QString& text);

    QString selectedNoteTextEditText();

    void linkTagNameToCurrentNote(const QString& tagName,
                                  bool linkToSelectedNotes = false);

    Q_INVOKABLE void reloadTagTree();

    Q_INVOKABLE void reloadNoteSubFolderTree();

    Q_INVOKABLE void buildNotesIndexAndLoadNoteDirectoryList(
            bool forceBuild = false, bool forceLoad = false);

    QList<Note> selectedNotes();

    bool jumpToNoteSubFolder(int noteSubFolderId);

    QString noteTextEditCurrentWord(bool withPreviousCharacters = false);

    Q_INVOKABLE void focusNoteTextEdit();

    QString getLogText();

    void turnOnDebugLogging();

    void openIssueAssistantDialog();

    void insertHtml(QString html);

protected:
    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject *obj, QEvent *event);

    void resizeEvent(QResizeEvent* event);

public slots:
    void setCurrentNoteFromNoteId(int noteId);

    void regenerateNotePreview();

    void storeUpdatedNotesToDisk();

    void changeNoteFolder(int noteFolderId, bool forceChange = false);

    void allowNoteEditing();

    void disallowNoteEditing();

private slots:

    void on_noteTextEdit_textChanged();

    void on_action_Quit_triggered();

    void quitApp();

    void notesDirectoryWasModified(const QString &str);

    void notesWereModified(const QString &str);

    void on_actionSet_ownCloud_Folder_triggered();

    void on_searchLineEdit_textChanged(const QString &arg1);

    void on_action_Find_note_triggered();

    void on_searchLineEdit_returnPressed();

    void on_action_Remove_note_triggered();

    void on_actionAbout_QOwnNotes_triggered();

    void on_action_New_note_triggered();

    void on_noteTextView_anchorClicked(const QUrl &arg1);

    void on_actionCheck_for_updates_triggered();

    void on_actionReport_problems_or_ideas_triggered();

    void on_actionAlphabetical_triggered(bool checked);

    void on_actionBy_date_triggered(bool checked);

    void systemTrayIconClicked(QSystemTrayIcon::ActivationReason reason);

    void on_action_Settings_triggered();

    void on_actionShow_versions_triggered();

    void on_actionShow_trash_triggered();

    void on_actionSelect_all_notes_triggered();

    void jumpToWelcomeNote();

    void on_noteTextEdit_customContextMenuRequested(const QPoint &pos);

    void pasteMediaIntoNote();

    void on_actionInsert_Link_to_note_triggered();

    void on_action_DuplicateText_triggered();

    void on_action_Back_in_note_history_triggered();

    void on_action_Forward_in_note_history_triggered();

    void on_action_Shortcuts_triggered();

    void on_action_Knowledge_base_triggered();

    void on_actionInsert_current_time_triggered();

    void on_actionOpen_List_triggered();

    void frequentPeriodicChecker();

    void on_action_Export_note_as_PDF_markdown_triggered();

    void on_action_Export_note_as_PDF_text_triggered();

    void on_action_Print_note_markdown_triggered();

    void on_action_Print_note_text_triggered();

    void on_actionInsert_image_triggered();

    void on_actionShow_changelog_triggered();

    void openLocalUrl(QString urlString);

    void on_action_Find_text_in_note_triggered();

    void on_action_Encrypt_note_triggered();

    void on_actionDecrypt_note_triggered();

    void on_actionEdit_encrypted_note_triggered();

    void on_encryptedNoteTextEdit_textChanged();

    void on_action_Open_note_in_external_editor_triggered();

    void on_action_Export_note_as_markdown_triggered();

    void showEvent(QShowEvent *event);

    void on_actionGet_invloved_triggered();

    void gotoNoteBookmark(int slot = 0);

    void storeNoteBookmark(int slot = 0);

    void on_actionInsert_code_block_triggered();

    void on_actionNext_note_triggered();

    void on_actionPrevious_Note_triggered();

    void on_actionToggle_distraction_free_mode_triggered();

    void toggleDistractionFreeMode();

    void trackAction(QAction *action);

    void on_actionShow_toolbar_triggered(bool checked);

    void toolbarVisibilityChanged(bool visible);

    void dfmEditorWidthActionTriggered(QAction *action);

    void dragEnterEvent(QDragEnterEvent *e);

    void dropEvent(QDropEvent *e);

    void on_actionPaste_image_triggered();

    void on_actionShow_note_in_file_manager_triggered();

    void on_actionFormat_text_bold_triggered();

    void on_actionFormat_text_italic_triggered();

    void on_action_Increase_note_text_size_triggered();

    void on_action_Decrease_note_text_size_triggered();

    void on_action_Reset_note_text_size_triggered();

    void on_noteFolderComboBox_currentIndexChanged(int index);

    void on_tagLineEdit_returnPressed();

    void on_tagLineEdit_textChanged(const QString &arg1);

    void on_newNoteTagButton_clicked();

    void on_newNoteTagLineEdit_returnPressed();

    void on_newNoteTagLineEdit_editingFinished();

    void removeNoteTagClicked();

    void on_action_new_tag_triggered();

    void on_action_Reload_note_folder_triggered();

    void noteViewUpdateTimerSlot();

    void gitCommitCurrentNoteFolder();

    void noteTextSliderValueChanged(int value, bool force = false);

    void noteViewSliderValueChanged(int value, bool force = false);

    void on_tagTreeWidget_itemChanged(QTreeWidgetItem *item, int column);

    void on_tagTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_tagTreeWidget_customContextMenuRequested(const QPoint &pos);

    void moveSelectedTagsToTagId(int tagId);

    void tagSelectedNotesToTagId(int tagId);

    void on_actionReplace_in_current_note_triggered();

    void onNavigationWidgetPositionClicked(int position);

    void startNavigationParser();

    void onNoteTextViewResize(QSize size, QSize oldSize);

    void on_actionAutocomplete_triggered();

    void restoreDistractionFreeMode();

    void on_actionSelect_note_folder_triggered();

    void on_actionReload_scripting_engine_triggered();

    void on_actionShow_log_triggered();

    void on_actionExport_preview_HTML_triggered();

    void hideNoteFolderComboBoxIfNeeded();

    void generateSystemTrayContextMenu();

    void reloadTodoLists();

    void openTodoDialog(const QString& taskUid = "");

    void showWindow();

    void on_actionOpen_IRC_Channel_triggered();

    void storeSavedSearch();

    void on_actionInsert_headline_from_note_filename_triggered();

    void on_actionUse_softwrap_in_note_editor_toggled(bool arg1);

    void on_actionShow_status_bar_triggered(bool checked);

    void on_noteTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_noteTreeWidget_customContextMenuRequested(const QPoint &pos);

    void on_noteTreeWidget_itemChanged(QTreeWidgetItem *item, int column);

    void on_noteSubFolderTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_noteSubFolderLineEdit_textChanged(const QString &arg1);

    void on_noteSubFolderTreeWidget_itemExpanded(QTreeWidgetItem *item);

    void on_noteSubFolderTreeWidget_itemCollapsed(QTreeWidgetItem *item);

    void on_noteSubFolderTreeWidget_customContextMenuRequested(const QPoint &pos);

    void on_noteSubFolderLineEdit_returnPressed();

    void on_noteSubFolderTreeWidget_itemChanged(QTreeWidgetItem *item, int column);

    void on_actionShare_note_triggered();

    void on_actionToggle_text_case_triggered();

    void on_actionMarkdown_cheatsheet_triggered();

    void on_actionStrike_out_text_triggered();

    void on_actionShow_menu_bar_triggered(bool checked);

    void moveSelectedNotesToNoteSubFolderId(int noteSubFolderId);

    void copySelectedNotesToNoteSubFolderId(int noteSubFolderId);

    void on_actionSplit_note_at_cursor_position_triggered();

    void onCustomActionInvoked(const QString &identifier);

    void on_actionDonate_triggered();

    void on_actionFind_notes_in_all_subfolders_triggered();

    void on_actionImport_notes_from_Evernote_triggered();

    void on_actionDelete_orphaned_images_triggered();

    void on_actionGitter_triggered();

    void on_actionUnlock_panels_toggled(bool arg1);

    void on_actionStore_as_new_workspace_triggered();

    void onWorkspaceComboBoxCurrentIndexChanged(int index);

    void on_actionRemove_current_workspace_triggered();

    void on_actionRename_current_workspace_triggered();

    void setCurrentWorkspace(const QString& uuid);

    void on_actionSwitch_to_previous_workspace_triggered();

    void on_actionShow_all_panels_triggered();

    void restoreCurrentWorkspace();

    void togglePanelVisibility(const QString& objectName);

    void updatePanelMenu();

    void toggleToolbarVisibility(const QString& objectName);

    void updateToolbarMenu();

    void on_actionFind_action_triggered();

    void releaseDockWidgetSizes();

    void on_actionInsert_table_triggered();

    void on_actionInsert_block_quote_triggered();

    void on_actionSearch_text_on_the_web_triggered();

    void noteEditCursorPositionChanged();

    void on_actionDelete_line_triggered();

    void on_actionDelete_word_triggered();

    void on_actionView_note_in_new_window_triggered();

    void on_actionSave_modified_notes_triggered();

    void enableNoteExternallyRemovedCheck();

    void on_actionAscending_triggered();

    void on_actionDescending_triggered();

    bool restoreActiveNoteHistoryItem();

    void on_actionShow_note_git_versions_triggered();

    void on_tagTreeWidget_itemCollapsed(QTreeWidgetItem *item);

    void on_tagTreeWidget_itemExpanded(QTreeWidgetItem *item);

    void on_actionScript_repository_triggered();

    void on_actionScript_settings_triggered();

    void filterNotesByTag();

    void on_actionInsert_attachment_triggered();

    void on_actionAllow_note_editing_triggered(bool checked);

    void on_actionCheck_for_script_updates_triggered();

    void noteTextEditResize(QResizeEvent *event);

    void encryptedNoteTextEditResize(QResizeEvent *event);

    void on_actionShow_local_trash_triggered();

    void on_encryptedNoteTextEdit_customContextMenuRequested(const QPoint &pos);

    void on_actionJump_to_note_text_edit_triggered();

    void on_tagTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_noteTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_noteTreeWidget_itemSelectionChanged();

    void on_actionManage_orphaned_attachments_triggered();

    void on_noteOperationsButton_clicked();

    void on_actionImport_notes_from_text_files_triggered();

    void on_actionTelegram_triggered();

    void on_actionCopy_headline_triggered();

    void on_action_FormatTable_triggered();

    void on_navigationLineEdit_textChanged(const QString &arg1);

    void initWebSocketServerService();

    void on_actionJump_to_note_list_panel_triggered();

    void on_actionJump_to_tags_panel_triggered();

    void on_actionJump_to_note_subfolder_panel_triggered();

    void on_actionActivate_context_menu_triggered();

    void on_actionImport_bookmarks_from_server_triggered();

    void on_actionRiot_triggered();

    void on_actionToggle_fullscreen_triggered();

    void disableFullScreenMode();

    void moveSelectedNotesToFolder(const QString& destinationFolder);

    void copySelectedNotesToFolder(const QString& destinationFolder, const QString& noteFolderPath = "");

    void on_actionTypewriter_mode_toggled(bool arg1);

    //Spellchecker actions
    void on_actionCheck_spelling_toggled(bool checked);

    void onLanguageChanged(QAction* action);

    void on_actionManage_dictionaries_triggered();

private:
    Ui::MainWindow *ui;
    QString notesPath;
    QFileSystemWatcher noteDirectoryWatcher;
    Note currentNote;
    QString _currentNoteTextHash;
    NoteDiffDialog *noteDiffDialog;
    UpdateService *updateService;
    bool showSystemTray;
    QSystemTrayIcon *trayIcon;
    QDateTime currentNoteLastEdited;
    bool notifyAllExternalModifications;
    int noteSaveIntervalTime;
    QTimer *noteSaveTimer;
    QTimer *todoReminderTimer;
    QTimer *_noteViewUpdateTimer;
    QTimer *_gitCommitTimer;
    QTimer *_todoListTimer;
    bool _noteViewNeedsUpdate;
    NoteHistory noteHistory;
    QHash<int, NoteHistoryItem> noteBookmarks;
    QPushButton *_updateAvailableButton;
    QLabel *_noteEditLineNumberLabel;
    QPushButton *_readOnlyButton;
    QPushButton *_leaveDistractionFreeModeButton;
    QPushButton *_leaveFullScreenModeButton;
    QToolBar *_formattingToolbar;
    QToolBar *_customActionToolbar;
    QToolBar *_insertingToolbar;
    QToolBar *_encryptionToolbar;
    QToolBar *_windowToolbar;
    QToolBar *_quitToolbar;
    bool _noteViewIsRegenerated;
    QHash<int, NoteHistoryItem> _activeNoteFolderNotePositions;
    bool _searchLineEditFromCompleter;
    bool _isNotesDirectoryWasModifiedDisabled;
    bool _isNotesWereModifiedDisabled;
    bool _isDefaultShortcutInitialized;
    QList<QShortcut *> _menuShortcuts;
    bool _showNotesFromAllNoteSubFolders;
    QList<int> _buildNotesIndexBeforeNoteIdList;
    QList<int> _buildNotesIndexBeforeNoteSubFolderIdList;
    QList<int> _buildNotesIndexAfterNoteIdList;
    QList<int> _buildNotesIndexAfterNoteSubFolderIdList;
    QScrollArea *_noteTagButtonScrollArea;
    QDockWidget *_taggingDockWidget;
    QDockWidget *_noteSubFolderDockWidget;
    QDockWidget *_noteSearchDockWidget;
    QDockWidget *_noteFolderDockWidget;
    QDockWidget *_noteListDockWidget;
    QDockWidget *_noteNavigationDockWidget;
    QDockWidget *_noteEditDockWidget;
    QDockWidget *_noteTagDockWidget;
    QDockWidget *_notePreviewDockWidget;
    QDockWidget *_logDockWidget;
    QDockWidget *_scriptingDockWidget;
    QWidget *_taggingDockTitleBarWidget;
    QWidget *_noteSubFolderDockTitleBarWidget;
    QWidget *_noteSearchDockTitleBarWidget;
    QWidget *_noteFolderDockTitleBarWidget;
    QWidget *_noteListDockTitleBarWidget;
    QWidget *_noteNavigationDockTitleBarWidget;
    QWidget *_noteEditDockTitleBarWidget;
    QWidget *_noteTagDockTitleBarWidget;
    QWidget *_notePreviewDockTitleBarWidget;
    QWidget *_logDockTitleBarWidget;
    QWidget *_scriptingDockTitleBarWidget;
    QComboBox *_workspaceComboBox;
    bool _noteFolderDockWidgetWasVisible;
    bool _noteSubFolderDockWidgetVisible;
    bool _closeEventWasFired;
    ActionDialog *_actionDialog;
    TodoDialog *_todoDialog;
    IssueAssistantDialog *_issueAssistantDialog;
    OrphanedImagesDialog *_orphanedImagesDialog;
    OrphanedAttachmentsDialog *_orphanedAttachmentsDialog;
    SettingsDialog *_settingsDialog;
    bool _noteExternallyRemovedCheckEnabled;
    QList<QAction *> _noteTextEditContextMenuActions;
    QList<QAction *> _noteListContextMenuActions;
    QString _notePreviewHash;
    int _gitCommitInterval;
    bool _noteEditIsCentralWidget;
    bool _lastNoteSelectionWasMultiple;
    WebSocketServerService *_webSocketServerService;
    QActionGroup *languageGroup;

    void createSystemTrayIcon();

    void loadNoteDirectoryList();

    void readSettings();

    bool buildNotesIndex(int noteSubFolderId = 0, bool forceRebuild = false);

    QString selectOwnCloudNotesFolder();

    void setCurrentNote(
            Note note,
            bool updateNoteText = true,
            bool updateSelectedNote = true,
            bool addPreviousNoteToHistory = true);

    void removeCurrentNote();

    void searchInNoteTextEdit(QString str);

    void searchForSearchLineTextInNoteTextEdit();

    int openNoteDiffDialog(Note changedNote);

    void setNoteTextFromNote(Note *note, bool updateNoteTextViewOnly = false,
                             bool ignorePreviewVisibility = false);

    void loadNoteFolderListMenu();

    void storeRecentNoteFolder(const QString& addFolderName, const QString& removeFolderName);

    void storeSettings();

    void removeSelectedNotes();

    void selectAllNotes();

    void updateCurrentFolderTooltip();

    void handleTextNoteLinking();

    void updateNoteTextFromDisk(Note note);

    void makeCurrentNoteFirstInNoteList();

    void readSettingsFromSettingsDialog();

    void setCurrentNoteFromHistoryItem(NoteHistoryItem item);

    bool prepareExportNoteAsPDFPrinter(QPrinter *printer);

    void exportNoteAsPDF(QPlainTextEdit *textEdit);

    bool preparePrintNotePrinter(QPrinter *printer);

    void printNote(QPlainTextEdit *textEdit);

    void exportNoteAsPDF(QTextEdit *textEdit);

    void exportNoteAsPDF(QTextDocument *doc);

    void printNote(QTextEdit *textEdit);

    void updateEncryptNoteButtons();

    void askForEncryptedNotePasswordIfNeeded(const QString& additionalText = "");

    void showAppMetricsNotificationIfNeeded();

    void resetCurrentNote();

    void setupNoteBookmarkShortcuts();

    void setupStatusBarWidgets();

    void gotoNextNote();

    void activateContextMenu();

    void gotoPreviousNote();

    void setDistractionFreeMode(bool enabled = true);

    void changeDistractionFreeMode(bool enabled);

    bool insertMedia(QFile *file, QString title = "");

    int currentNoteLineNumber();

    static bool isValidMediaFile(QFile *file);

    static bool isValidNoteFile(QFile *file);

    bool isInDistractionFreeMode();

    void removeSelectedTags();

    void setupTags();

    void reloadCurrentNoteTags();

    void highlightCurrentNoteTagsInTagTree();

    void filterNotesBySearchLineEditText();

    void filterNotes(bool searchForText = true);

    bool isTagsEnabled();

    void initStyling();

    void tagSelectedNotes(const Tag &tag);

    static bool isMarkdownViewEnabled();

    void removeTagFromSelectedNotes(const Tag &tag);

    bool isNoteEditPaneEnabled();

    void initToolbars();

    void buildTagTreeForParentItem(QTreeWidgetItem *parent = nullptr, bool topLevel = false);

    void buildTagMoveMenuTree(QMenu *parentMenu,
                              int parentTagId = 0);

    void buildBulkNoteTagMenuTree(QMenu *parentMenu, int parentTagId = 0);

    QTreeWidgetItem *addTagToTagTreeWidget(QTreeWidgetItem *parent, const Tag &tag);

    bool jumpToNoteName(const QString& name);

    bool jumpToNoteHistoryItem(const NoteHistoryItem& historyItem);

    void initScriptingEngine();

    int getMaxImageWidth();

    void updateWindowTitle();

    void initSavedSearchesCompleter();

    void initEditorSoftWrap();

    bool isToolbarVisible();

    static void setTreeWidgetItemToolTipForNote(
            QTreeWidgetItem *item, const Note &note,
            QDateTime *overrideFileLastModified = nullptr);

    QTreeWidgetItem *firstVisibleNoteTreeWidgetItem();

    QTreeWidgetItem *addNoteSubFolderToTreeWidget(
            QTreeWidgetItem *parentItem,
            const NoteSubFolder &noteSubFolder);

    void buildNoteSubFolderTreeForParentItem(QTreeWidgetItem *parent = nullptr);

    void setupNoteSubFolders();

    void filterNotesByNoteSubFolders();

    void updateNoteDirectoryWatcher();

    bool addNoteToNoteTreeWidget(const Note &note, QTreeWidgetItem *parent = nullptr);

    void removeSelectedNoteSubFolders(QTreeWidget *treeWidget);

    bool createNewNoteSubFolder(QString folderName = "");

    QTreeWidgetItem *findNoteInNoteTreeWidget(const Note &note);

    void jumpToNoteOrCreateNew(bool disableLoadNoteDirectoryList = false);

    void updateShareButton();

    void initShortcuts();

    void buildBulkNoteSubFolderMenuTree(QMenu *parentMenu, bool doCopy = true,
                                        int parentNoteSubFolderId = 0);

    void buildBulkNoteFolderSubFolderMenuTree(QMenu *parentMenu, bool doCopy,
                                              const QString &parentNoteSubFolderPath, bool isRoot = true);

    void moveSelectedNotesToNoteSubFolder(const NoteSubFolder &noteSubFolder);

    void copySelectedNotesToNoteSubFolder(const NoteSubFolder &noteSubFolder);

    void createNewNote(QString noteName = "", bool withNameAppend = true);

    bool selectedNotesHaveTags();

    void initTagButtonScrollArea();

    static QIcon getSystemTrayIcon();

    void initShowHidden();

    void initTreeWidgetItemHeight();

    void updateTreeWidgetItemHeight(QTreeWidget *treeWidget, int height);

    bool solveEquationInNoteTextEdit(double &returnValue);

    bool noteTextEditAutoComplete(QStringList &resultList);

    void initDockWidgets();

    void updateWorkspaceLists(bool rebuild = true);

    bool createNewWorkspace(QString name);

    QString currentWorkspaceUuid();

    void storeCurrentWorkspace();

    void initWorkspaceComboBox();

    QStringList getWorkspaceUuidList();

    void updateWindowToolbar();

    void restoreToolbars();

    void handleNoteSubFolderVisibility() const;

    void initPanelMenu();

    void initToolbarMenu();

    void assignColorToTagItem(QTreeWidgetItem *item);

    void handleTreeWidgetItemTagColor(QTreeWidgetItem *item, const Tag &tag) const;

    void disableColorOfTagItem(QTreeWidgetItem *item);

    void assignColorToSelectedTagItems();

    void handleNoteTreeTagColoringForNote(const Note &note);

    bool showRestartNotificationIfNeeded();

    void unsetCurrentNote();

    void setNoteTextEditReadOnly(bool readonly) const;

    void updateNoteSortOrderSelectorVisibility(bool visible);

    void storeTagTreeWidgetExpandState() const;

    void startAppVersionTest() const;

    bool showNotesFromAllNoteSubFolders();

    void selectAllNotesInNoteSubFolderTreeWidget() const;

    bool insertAttachment(QFile *file, const QString &title = "");

    Qt::SortOrder toQtOrder(int order);

    void updatePanelsSortOrder();

    void updateNotesPanelSortOrder();

    void selectAllNotesInTagTreeWidget() const;

    void handleScriptingNoteTagging(Note note, const QString& tagName,
                                    bool doRemove = false,
                                    bool triggerPostMethods = true);

    void handleScriptingNotesTagUpdating();

    void handleScriptingNotesTagRenaming(const QString& oldTagName, const QString& newTagName);

    void handleScriptingNotesTagRemoving(const QString& tagName,
                                         bool forBulkOperation = false);

    void directoryWatcherWorkaround(bool isNotesDirectoryWasModifiedDisabled,
                                    bool alsoHandleNotesWereModified = false);

    void setMenuEnabled(QMenu* menu, bool enabled);

    bool undoFormatting(const QString& formatter);

    void applyFormatter(const QString& formatter);

    bool isNoteTextSelected();

    void noteTextEditCustomContextMenuRequested(
            QOwnNotesMarkdownTextEdit *noteTextEdit, const QPoint &pos);

    void updateNoteTextEditReadOnly();

    int getSelectedNotesCount() const;

    void updateNoteTreeWidgetItem(const Note &note,
                                  QTreeWidgetItem *noteItem = nullptr);

    void initFakeVim(QOwnNotesMarkdownTextEdit *noteTextEdit);

    void openNotesContextMenu(const QPoint &globalPos,
            bool multiNoteMenuEntriesOnly = false);

    void openNoteSubFolderContextMenu(const QPoint &globalPos, QTreeWidget *treeWidget);

    void updateCurrentNoteTextHash();

    void centerAndResize();

    void forceRegenerateNotePreview();

    void removeConflictedNotesDatabaseCopies();

    void doSearchInNote(QString searchText);

    void insertNoteText(const QString &text);

    void handleNoteTextChanged();

    void loadDictionaryNames();
};
