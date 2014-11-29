#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QSplitter>
#include <QFileSystemWatcher>
#include <QHash>

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

private slots:
    void on_notesListWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_noteTextEdit_textChanged();
    void notesWereModified( const QString& str );
    void checkForNoteChanges();

private:
    Ui::MainWindow *ui;
    QSplitter *mainSplitter;
    QString notesPath;
    QFileSystemWatcher noteDirectoryWatcher;
    QHash<QString,QString> notesTextHash;
    QHash<QString,QString> notesNameHash;

    void setupMainSplitter();
    void loadNoteDirectoryList();
    void loadNote( QString &fileName );
    void storeNote( QString &fileName, QString &text );
    void readSettings();
    void buildNotesIndex();
    QString fullNoteFilePath( QString fileName );
};

#endif // MAINWINDOW_H
