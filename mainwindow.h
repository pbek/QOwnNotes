#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QSplitter>

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
    void on_mainSplitter_splitterMoved ( int pos, int index );

private:
    Ui::MainWindow *ui;
    QSplitter *mainSplitter;

    void setupMainSplitter();
    void loadNoteDirectoryList();
    void loadNote( QString &fileName );
    void storeNote( QString &fileName, QString &text );
    void readSettings();

    QString notesPath;
};

#endif // MAINWINDOW_H
