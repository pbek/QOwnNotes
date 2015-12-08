#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QSplitter>
#include <QFileSystemWatcher>
#include <QHash>
#include <QFileDialog>
#include <QEvent>
#include <QSignalMapper>
#include <QSystemTrayIcon>
#include <note.h>
#include "markdown-highlight/highlighter.h"
#include "notehistory.h"
#include "notediffdialog.h"
#include "updateservice.h"
#include "libraries/simplecrypt/simplecrypt.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setCurrentNoteText(QString text);
    void createNewNote(QString name, QString text);
    void openSettingsDialog();
    void restoreTrashedNoteOnServer(QString fileName, int timestamp);
protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_notesListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_noteTextEdit_textChanged();
    void on_action_Quit_triggered();
    void notesDirectoryWasModified( const QString &str );
    void notesWereModified( const QString& str );
    void storeUpdatedNotesToDisk();
    void on_actionSet_ownCloud_Folder_triggered();
    void on_searchLineEdit_textChanged(const QString &arg1);
    void on_action_Find_note_triggered();
    void on_searchLineEdit_returnPressed();
    void on_action_Remove_note_triggered();
    void on_actionAbout_QOwnNotes_triggered();
    void on_action_Note_note_triggered();
    void on_actionToggleEditMode_triggered();
    void on_noteTabWidget_currentChanged(int index);
    void changeNoteFolder(const QString &folderName);
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
    void updateCheckTimerTimeout();
    void on_notesListWidget_customContextMenuRequested(const QPoint &pos);
    void on_actionSelect_all_notes_triggered();
    void jumpToWelcomeNote();
    void on_noteTextEdit_customContextMenuRequested(const QPoint &pos);
    void on_actionInsert_Link_to_note_triggered();
    void on_action_Find_text_in_note_triggered();
    void on_searchInNoteCloseButton_clicked();
    void on_searchInNoteDownButton_clicked();
    void on_searchInNoteEdit_textChanged(const QString &arg1);
    void on_searchInNoteUpButton_clicked();
    void on_action_DuplicateText_triggered();
    void on_action_Back_in_note_history_triggered();
    void on_action_Forward_in_note_history_triggered();
    void on_action_Shortcuts_triggered();
    void on_action_Knowledge_base_triggered();
    void on_actionInsert_current_time_triggered();
    void on_actionOpen_List_triggered();

private:
    Ui::MainWindow *ui;
    QSplitter *mainSplitter;
    QString notesPath;
    QFileSystemWatcher noteDirectoryWatcher;
    Note currentNote;
    int firstVisibleNoteListRow;
    HGMarkdownHighlighter *highlighter;
    NoteDiffDialog *noteDiffDialog;
    bool noteTextEditIsInEditMode;
    QSignalMapper *signalMapper;
    UpdateService *updateService;
    bool sortAlphabetically;
    bool showSystemTray;
    QSystemTrayIcon  *trayIcon;
    SimpleCrypt crypto;
    QDateTime currentNoteLastEdited;
    bool notifyAllExternalModifications;
    int noteSaveIntervalTime;
    QTimer *noteSaveTimer;
    NoteHistory noteHistory;

    void setupMainSplitter();
    void createSystemTrayIcon();
    void loadNoteDirectoryList();
    void readSettings();
    void buildNotesIndex();
    QString selectOwnCloudNotesFolder();
    void setCurrentNote(Note note, bool updateNoteText = true, bool updateSelectedNote = true, bool addPreviousNoteToHistory = true);
    void handlePressed();
    void focusNoteTextEdit();
    void removeCurrentNote();
    void waitMsecs(int msecs);
    void searchInNoteTextEdit(QString &str);
    void searchForSearchLineTextInNoteTextEdit();
    int openNoteDiffDialog(Note changedNote);
    void setNoteTextEditMode(bool isInEditMode);
    void setNoteTextFromNote(Note *note);
    void setNoteTextFromNote(Note *note, bool updateNoteTextViewOnly);
    void loadRecentNoteFolderListMenu();
    void storeRecentNoteFolder(QString addFolderName, QString removeFolderName);
    void storeSettings();
    void setupCrypto();
    void removeSelectedNotes();
    void moveSelectedNotesToFolder(QString destinationFolder);
    void copySelectedNotesToFolder(QString destinationFolder);
    void selectAllNotes();
    void updateCurrentFolderTooltip();
    void handleTextNoteLinking();
    void closeSearchInNoteWidget();
    void doSearchInNote(bool searchDown = true);
    void updateNoteTextFromDisk(Note note);
    void makeCurrentNoteFirstInNoteList();
    void readSettingsFromSettingsDialog();
    void openLinkAtCurrentNoteTextEditPosition();
    bool increaseSelectedTextIndentionInNoteTextEdit(bool reverse = false);
    void setCurrentNoteFromHistoryItem(NoteHistoryItem item);
};

#endif // MAINWINDOW_H
