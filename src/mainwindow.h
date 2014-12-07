#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QSplitter>
#include <QFileSystemWatcher>
#include <QHash>
#include <QFileDialog>
#include <QEvent>
#include <note.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

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

private:
    Ui::MainWindow *ui;
    QSplitter *mainSplitter;
    QString notesPath;
    QFileSystemWatcher noteDirectoryWatcher;
    Note currentNote;
    int firstVisibleNoteListRow;

    void setupMainSplitter();
    void loadNoteDirectoryList();
    void loadNote( QString &fileName );
    void readSettings();
    void buildNotesIndex();
    QString selectOwnCloudFolder();
    void setCurrentNote(Note note);
    void setCurrentNote(Note note, bool updateNoteText);
    void handlePressed();
    void focusNoteTextEdit();
    void removeCurrentNote();
};

#endif // MAINWINDOW_H
