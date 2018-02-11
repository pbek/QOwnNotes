#pragma once

#include "mainwindow.h"
#include "masterdialog.h"

#include <QAbstractButton>
#include <QDialog>
#include <QSplitter>
#include <QJSValue>

namespace Ui {
class LocalTrashDialog;
}

class LocalTrashDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit LocalTrashDialog(MainWindow *mainWindow, QWidget *parent = 0);
    ~LocalTrashDialog();

private slots:
    void storeSettings();
    void on_trashListWidget_currentRowChanged(int currentRow);
    void dialogButtonClicked(QAbstractButton *button);
private:
    enum ButtonRole {
        Unset,  // nothing was selected
        Restore,
        Remove,
        Cancel
    };

    Ui::LocalTrashDialog *ui;
    QSplitter *trashSplitter;
    QStringList *dataList;
    QList<int> *timestampList;
    MainWindow *mainWindow;
    void setupMainSplitter();

    void loadTrashedNotes();
};
