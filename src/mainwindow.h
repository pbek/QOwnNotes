#pragma once

#include <QMainWindow>
#include <QListWidgetItem>
#include <QFileSystemWatcher>
#include <QHash>
#include <QFileDialog>
#include <QEvent>
#include <QSignalMapper>
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
#include "entities/notehistory.h"
#include "dialogs/notediffdialog.h"
#include "services/updateservice.h"
#include "libraries/qmarkdowntextedit/qmarkdowntextedit.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
    Q_PROPERTY(Note currentNote WRITE setCurrentNote MEMBER currentNote
                       NOTIFY currentNoteChanged)

Q_SIGNALS:
    void currentNoteChanged(Note &note);

public:
    explicit MainWindow(QWidget *parent = 0);

    ~MainWindow();

    void setCurrentNoteText(QString text);

    void createNewNote(QString name, QString text, bool cursorAtEnd = false);

    void openSettingsDialog(int page = 0);

    void restoreTrashedNoteOnServer(QString fileName, int timestamp);

    void showUpdateAvailableButton(QString version);

    void hideUpdateAvailableButton();

    void enableShowVersionsButton();

    void enableShowTrashButton();

    void showStatusBarMessage(const QString & message, int timeout = 0);

    void handleInsertingFromMimeData(const QMimeData *mimeData);

    QMarkdownTextEdit *activeNoteTextEdit();

    static MainWindow *instance();

    QList<QMenu *> menuList();

    QAction *findAction(QString objectName);

    static bool isOneTreeWidgetItemChildVisible(QTreeWidgetItem *item);

    static void searchForTextInTreeWidget(QTreeWidget *treeWidget,
                                          QString text);

    void addCustomAction(QString identifier, QString menuText,
                         QString buttonText, QString icon);

    void preReloadScriptingEngine();

    void writeToNoteTextEdit(QString text);

    QString selectedNoteTextEditText();

    void linkTagNameToCurrentNote(QString tagName);

protected:
    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject *obj, QEvent *event);

    void resizeEvent(QResizeEvent* event);

private slots:

    void on_noteTextEdit_textChanged();

    void on_action_Quit_triggered();

    void quitApp();

    void notesDirectoryWasModified(const QString &str);

    void notesWereModified(const QString &str);

    void storeUpdatedNotesToDisk();

    void on_actionSet_ownCloud_Folder_triggered();

    void on_searchLineEdit_textChanged(const QString &arg1);

    void on_action_Find_note_triggered();

    void on_searchLineEdit_returnPressed();

    void on_action_Remove_note_triggered();

    void on_actionAbout_QOwnNotes_triggered();

    void on_action_New_note_triggered();

    void changeNoteFolder(int noteFolderId, bool forceChange = false);

    void on_noteTextView_anchorClicked(const QUrl &arg1);

    void on_actionCheck_for_updates_triggered();

    void on_actionReport_problems_or_ideas_triggered();

    void on_actionAlphabetical_triggered(bool checked);

    void on_actionBy_date_triggered(bool checked);

    void systemTrayIconClicked(QSystemTrayIcon::ActivationReason reason);

    void on_actionShow_system_tray_triggered(bool checked);

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

    void on_actionInset_code_block_triggered();

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

    void regenerateNotePreview();

    void on_actionAutocomplete_triggered();

    void restoreDistractionFreeMode();

    void on_actionSelect_note_folder_triggered();

    void on_actionReload_scripting_engine_triggered();

    void on_actionShow_log_triggered();

    void on_actionExport_preview_HTML_triggered();

    void hideNoteFolderComboBoxIfNeeded();

    void generateSystemTrayContextMenu();

    void setCurrentNoteFromNoteId(int noteId);

    void reloadTodoLists();

    void openTodoDialog(QString taskUid = "");

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

    void buildNotesIndexAndLoadNoteDirectoryList(bool forceBuild = false,
                                                 bool forceLoad = false);

    void onCustomActionInvoked(QString identifier);

    void on_actionDonate_triggered();

    void on_actionFind_notes_in_all_subfolders_triggered();

    void on_actionStart_hidden_triggered(bool checked);

    void on_actionImport_notes_from_Evernote_triggered();

    void on_actionDelete_orphaned_images_triggered();

    void on_actionGitter_triggered();

    void on_actionLock_panels_toggled(bool arg1);

    void on_actionStore_as_new_workspace_triggered();

    void onWorkspaceComboBoxCurrentIndexChanged(int index);

    void on_actionRemove_current_workspace_triggered();

    void on_actionRename_current_workspace_triggered();

    void setCurrentWorkspace(QString uuid);

    void on_actionSwitch_to_previous_workspace_triggered();

    void on_actionShow_all_panels_triggered();

    void restoreCurrentWorkspace();

    void togglePanelVisibility(QString objectName);

    void updatePanelMenu();

private:
    Ui::MainWindow *ui;
    QString notesPath;
    QFileSystemWatcher noteDirectoryWatcher;
    Note currentNote;
    NoteDiffDialog *noteDiffDialog;
    QSignalMapper *recentNoteFolderSignalMapper;
    QSignalMapper *storeNoteBookmarkSignalMapper;
    QSignalMapper *gotoNoteBookmarkSignalMapper;
    QSignalMapper *_customActionSignalMapper;
    QSignalMapper *_workspaceSignalMapper;
    QSignalMapper *_panelSignalMapper;
    UpdateService *updateService;
    bool sortAlphabetically;
    bool showSystemTray;
    QSystemTrayIcon *trayIcon;
    QDateTime currentNoteLastEdited;
    bool notifyAllExternalModifications;
    int noteSaveIntervalTime;
    QTimer *noteSaveTimer;
    QTimer *todoReminderTimer;
    QTimer *_noteViewUpdateTimer;
    QTimer *_todoListTimer;
    bool _noteViewNeedsUpdate;
    NoteHistory noteHistory;
    QHash<int, NoteHistoryItem> noteBookmarks;
    QPushButton *_updateAvailableButton;
    QPushButton *_leaveDistractionFreeModeButton;
    QToolBar *_formattingToolbar;
    QToolBar *_customActionToolbar;
    QToolBar *_insertingToolbar;
    QToolBar *_encryptionToolbar;
    QToolBar *_windowToolbar;
    QToolBar *_quitToolbar;
    QFrame *_verticalNoteFrame;
    bool _noteViewIsRegenerated;
    QHash<int, QString> _activeNoteFolderNoteNames;
    QHash<int, int> _activeNoteFolderNotePositions;
    bool _searchLineEditFromCompleter;
    bool _isNotesDirectoryWasModifiedDisabled;
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
    QComboBox *_workspaceComboBox;
    bool _noteFolderDockWidgetWasVisible;

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

    void focusNoteTextEdit();

    void removeCurrentNote();

    void searchInNoteTextEdit(QString str);

    void searchForSearchLineTextInNoteTextEdit();

    int openNoteDiffDialog(Note changedNote);

    void setNoteTextFromNote(Note *note, bool updateNoteTextViewOnly = false);

    void loadNoteFolderListMenu();

    void storeRecentNoteFolder(QString addFolderName, QString removeFolderName);

    void storeSettings();

    void removeSelectedNotes();

    void moveSelectedNotesToFolder(QString destinationFolder);

    void copySelectedNotesToFolder(QString destinationFolder);

    void selectAllNotes();

    void updateCurrentFolderTooltip();

    void handleTextNoteLinking();

    bool downloadUrlToFile(QUrl url, QFile *file);

    void updateNoteTextFromDisk(Note note);

    void makeCurrentNoteFirstInNoteList();

    void readSettingsFromSettingsDialog();

    void setCurrentNoteFromHistoryItem(NoteHistoryItem item);

    void exportNoteAsPDF(QTextEdit *textEdit);

    void printNote(QTextEdit *textEdit);

    void updateEncryptNoteButtons();

    void askForEncryptedNotePasswordIfNeeded(QString additionalText = "");

    void showAppMetricsNotificationIfNeeded();

    void resetCurrentNote();

    void setupNoteBookmarkShortcuts();

    void setupUpdateAvailableButton();

    void gotoNextNote();

    void gotoPreviousNote();

    void setDistractionFreeMode(bool enabled = true);

    void changeDistractionFreeMode(bool enabled);

    bool insertMedia(QFile *file);

    int currentNoteLineNumber();

    bool isValidMediaFile(QFile *file);

    bool isValidNoteFile(QFile *file);

    void insertHtml(QString html);

    bool isInDistractionFreeMode();

    void reloadTagTree();

    void removeSelectedTags();

    void setupTags();

    void reloadCurrentNoteTags();

    void filterNotesByTag();

    void filterNotesBySearchLineEditText();

    void filterNotes(bool searchForText = true);

    bool isTagsEnabled();

    void initStyling();

    void tagSelectedNotes(Tag tag);

    bool isMarkdownViewEnabled();

    void removeTagFromSelectedNotes(Tag tag);

    bool isNoteEditPaneEnabled();

    void initToolbars();

    void buildTagTreeForParentItem(QTreeWidgetItem *parent = 0);

    void buildTagMoveMenuTree(QMenu *parentMenu,
                              int parentTagId = 0);

    void buildBulkNoteTagMenuTree(QMenu *parentMenu, int parentTagId = 0);

    QTreeWidgetItem *addTagToTagTreeWidget(QTreeWidgetItem *parent, Tag tag);

    void jumpToNoteName(QString name);

    void initScriptingEngine();

    int getMaxImageWidth();

    void updateWindowTitle();

    void initSavedSearchesCompleter();

    void initEditorSoftWrap();

    bool isToolbarVisible();

    void setTreeWidgetItemToolTipForNote(QTreeWidgetItem *item, Note *note,
                                         QDateTime *overrideFileLastModified
                                         = 0);

    QTreeWidgetItem *firstVisibleNoteTreeWidgetItem();

    QTreeWidgetItem *addNoteSubFolderToTreeWidget(
            QTreeWidgetItem *parentItem,
            NoteSubFolder noteSubFolder);

    void reloadNoteSubFolderTree();

    void buildNoteSubFolderTreeForParentItem(QTreeWidgetItem *parent = 0);

    void setupNoteSubFolders();

    void filterNotesByNoteSubFolders();

    void updateNoteDirectoryWatcher();

    bool addNoteToNoteTreeWidget(Note note);

    void removeSelectedNoteSubFolders();

    bool createNewNoteSubFolder(QString folderName = "");

    QTreeWidgetItem *findNoteInNoteTreeWidget(Note note);

    void jumpToNoteOrCreateNew();

    void updateShareButton();

    void initShortcuts();

    void buildBulkNoteSubFolderMenuTree(QMenu *parentMenu, bool doCopy = true,
                                        int parentNoteSubFolderId = 0);

    void moveSelectedNotesToNoteSubFolder(NoteSubFolder noteSubFolder);

    void copySelectedNotesToNoteSubFolder(NoteSubFolder noteSubFolder);

    void createNewNote(QString noteName = "");

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
};
