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

    void setupMainSplitter();
    void createSystemTrayIcon();
    void loadNoteDirectoryList();
    void readSettings();
    void buildNotesIndex();
    QString selectOwnCloudFolder();
    void setCurrentNote(Note note);
    void setCurrentNote(Note note, bool updateNoteText);
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
    void storeRecentNoteFolder(const QString &folderName);
    void storeSettings();
    void setupCrypto();
};

#endif // MAINWINDOW_H
