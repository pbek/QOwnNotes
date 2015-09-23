#ifndef TRASHDIALOG_H
#define TRASHDIALOG_H

#include "mainwindow.h"

#include <QAbstractButton>
#include <QDialog>
#include <QScriptValue>
#include <QSplitter>

namespace Ui {
class TrashDialog;
}

class TrashDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TrashDialog(QScriptValue notes, MainWindow *mainWindow, QWidget *parent = 0);
    ~TrashDialog();

private slots:
    void storeSettings();
    void on_trashListWidget_currentRowChanged(int currentRow);
    void dialogButtonClicked(QAbstractButton *button);
private:
    enum ButtonRole {
        Unset,  // nothing was selected
        Restore,
        Cancel
    };

    Ui::TrashDialog *ui;
    QSplitter *trashSplitter;
    QStringList *dataList;
    MainWindow *mainWindow;
    void setupMainSplitter();
};

#endif // TRASHDIALOG_H
