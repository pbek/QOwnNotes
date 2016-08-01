#pragma once

#include <QMainWindow>
#include <QListWidgetItem>
#include <QSplitter>
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
#include <entities/note.h>
#include <entities/notefolder.h>
#include <entities/tag.h>
#include <services/scriptingservice.h>
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

    void createNewNote(QString name, QString text);

    void openSettingsDialog(int tab = 0);

    void restoreTrashedNoteOnServer(QString fileName, int timestamp);

    void showUpdateAvailableButton(QString version);

    void hideUpdateAvailableButton();

    void enableShowVersionsButton();

    void enableShowTrashButton();

    void showStatusBarMessage(const QString & message, int timeout = 0);

    void handleInsertingFromMimeData(const QMimeData *mimeData);

    QMarkdownTextEdit *activeNoteTextEdit();

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

    void on_action_Note_note_triggered();

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

    void on_actionToggle_tag_pane_toggled(bool arg1);

    void on_newNoteTagButton_clicked();

    void on_newNoteTagLineEdit_returnPressed();

    void on_newNoteTagLineEdit_editingFinished();

    void removeNoteTagClicked();

    void on_action_new_tag_triggered();

    void on_action_Reload_note_folder_triggered();

    void noteViewUpdateTimerSlot();

    void on_actionToggle_markdown_preview_toggled(bool arg1);

    void noteTextSliderValueChanged(int value);

    void noteViewSliderValueChanged(int value);

    void on_actionToggle_note_edit_pane_toggled(bool arg1);

    void on_actionUse_vertical_preview_layout_toggled(bool arg1);

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

    void setupNoteEditPane();

    void restoreDistractionFreeMode();

    void on_actionSelect_note_folder_triggered();

    void on_actionReload_scripting_engine_triggered();

    void on_actionShow_log_dialog_triggered();

    void on_actionExport_preview_HTML_triggered();

    void hideNoteFolderComboBoxIfNeeded() const;

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

private:
    Ui::MainWindow *ui;
    QSplitter *mainSplitter;
    QSplitter *_noteListSplitter;
    QSplitter *_tagFrameSplitter;
    QString notesPath;
    QFileSystemWatcher noteDirectoryWatcher;
    Note currentNote;
    NoteDiffDialog *noteDiffDialog;
    QSignalMapper *recentNoteFolderSignalMapper;
    QSignalMapper *storeNoteBookmarkSignalMapper;
    QSignalMapper *gotoNoteBookmarkSignalMapper;
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
    QToolBar *_insertingToolbar;
    QToolBar *_encryptionToolbar;
    QToolBar *_windowToolbar;
    QToolBar *_quitToolbar;
    QFrame *_verticalNoteFrame;
    QSplitter *_verticalNoteFrameSplitter;
    bool _noteViewIsRegenerated;
    QHash<int, QString> _activeNoteFolderNoteNames;
    QHash<int, int> _activeNoteFolderNotePositions;
    bool _searchLineEditFromCompleter;
    bool _isNotesDirectoryWasModifiedDisabled;

    void initMainSplitter();

    void setupMainSplitter();

    void createSystemTrayIcon();

    void loadNoteDirectoryList();

    void readSettings();

    void buildNotesIndex(int noteSubFolderId = 0);

    QString selectOwnCloudNotesFolder();

    void setCurrentNote(
            Note note,
            bool updateNoteText = true,
            bool updateSelectedNote = true,
            bool addPreviousNoteToHistory = true);

    void focusNoteTextEdit();

    void removeCurrentNote();

    void waitMsecs(int msecs);

    void searchInNoteTextEdit(QString &str);

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

    QString getInsertMediaMarkdown(QFile *file);

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

    void setupMarkdownView();

    void removeTagFromSelectedNotes(Tag tag);

    bool isNoteEditPaneEnabled();

    void initToolbars();

    bool isVerticalPreviewModeEnabled();

    void buildTagTreeForParentItem(QTreeWidgetItem *parent = 0);

    void buildTagMoveMenuTree(QMenu *parentMenu,
                              int parentTagId = 0);

    bool isOneTreeWidgetItemChildVisible(QTreeWidgetItem *item);

    void buildBulkNoteTagMenuTree(QMenu *parentMenu, int parentTagId = 0);

    QTreeWidgetItem *addTagToTagTreeWidget(QTreeWidgetItem *parent, Tag tag);

    void initNoteListSplitter();

    void jumpToNoteName(QString name);

    void initScriptingEngine();

    void showLogDialog();

    void initLogDialog() const;

    int getMaxImageWidth();

    void saveMainSplitterState(
            bool invertTagState = false, bool invertMarkdownState = false,
            bool invertEditState = false, bool invertVerticalModeState = false);

    QString getMainSplitterStateKey(
            bool invertTagState = false, bool invertMarkdownState = false,
            bool invertEditState = false, bool invertVerticalModeState = false);

    void restoreMainSplitterState(
            bool invertTagState = false, bool invertMarkdownState = false,
            bool invertEditState = false, bool invertVerticalModeState = false);

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

    void initTagFrameSplitter();

    void filterNotesByNoteSubFolders();

    void searchForTextInTreeWidget(QTreeWidget *treeWidget, QString text);

    void updateNoteDirectoryWatcher();

    bool addNoteToNoteTreeWidget(Note note);

    void removeSelectedNoteSubFolders();

    bool createNewNoteSubFolder(QString folderName = "");

    QTreeWidgetItem *findNoteInNoteTreeWidget(Note note);
};
