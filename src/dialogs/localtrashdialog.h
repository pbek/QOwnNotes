#pragma once

#include "masterdialog.h"
#include <QStringList>

namespace Ui {
class LocalTrashDialog;
}

class MainWindow;
class QTreeWidgetItem;
class QSplitter;
class QAbstractButton;

class LocalTrashDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit LocalTrashDialog(MainWindow *mainWindow, QWidget *parent = 0);
    ~LocalTrashDialog();

   private slots:
    void storeSettings();
    void dialogButtonClicked(QAbstractButton *button);
    void on_trashTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                               QTreeWidgetItem *previous);
    void on_searchLineEdit_textChanged(const QString &arg1);

   private:
    enum ButtonRole {
        Unset,    // nothing was selected
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

    void restoreSelectedTrashItems();

    void removeSelectedTrashItems();
};
