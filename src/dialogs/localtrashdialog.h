#pragma once

#include <QList>
#include <QStringList>

#include "masterdialog.h"

namespace Ui {
class LocalTrashDialog;
}

class MainWindow;
class QTreeWidgetItem;
class QSplitter;
class QAbstractButton;
class QPushButton;
class QAction;

class LocalTrashDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit LocalTrashDialog(QWidget *parent = 0);
    ~LocalTrashDialog();

   private slots:
    void storeSettings();
    void dialogButtonClicked(QAbstractButton *button);
    void on_trashTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
    void on_searchLineEdit_textChanged(const QString &arg1);
    void onSelectAllCheckBoxClicked();
    void selectAllVisibleItems();
    void clearSelection();

   private:
    enum ButtonRole {
        Unset,    // nothing was selected
        Restore,
        Remove,
        Cancel
    };

    Ui::LocalTrashDialog *ui;
    QSplitter *trashSplitter;
    QPushButton *_restoreButton = nullptr;
    QPushButton *_removeButton = nullptr;
    QAction *_restoreAction = nullptr;
    QAction *_removeAction = nullptr;
    QAction *_selectAllAction = nullptr;
    QAction *_clearSelectionAction = nullptr;

    void setupMainSplitter();

    void setupContextMenu();

    void loadTrashedNotes();

    void restoreSelectedTrashItems();

    void removeSelectedTrashItems();

    void updatePreviewInfo();

    int visibleItemCount() const;

    QList<QTreeWidgetItem *> selectedVisibleItems() const;

    QString failedNotesHtml(const QStringList &names) const;
};
