#ifndef TRASHDIALOG_H
#define TRASHDIALOG_H

#include <QList>
#include <QStringList>

#include "masterdialog.h"

namespace Ui {
class TrashDialog;
}

class MainWindow;
class QAbstractButton;
class QAction;
class QPushButton;
class QJSValue;
class QListWidgetItem;
class QSplitter;

class TrashDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit TrashDialog(const QJSValue &notes, QWidget *parent = nullptr);
    explicit TrashDialog(QWidget *parent = nullptr);
    ~TrashDialog();

    void setNotes(const QJSValue &notes);

   private slots:
    void storeSettings();
    void on_trashListWidget_currentRowChanged(int currentRow);
    void dialogButtonClicked(QAbstractButton *button);
    void on_searchLineEdit_textChanged(const QString &arg1);
    void onSelectAllCheckBoxClicked();
    void selectAllVisibleItems();
    void clearSelection();
    void restoreSelectedNotesOnServer();
    void downloadSelectedNotes();
    void deleteSelectedNotesOnServer();

   private:
    enum ButtonRole {
        Unset,    // nothing was selected
        Download,
        DeleteOnServer,
        RestoreOnServer,
        Cancel
    };

    Ui::TrashDialog *ui;
    QSplitter *trashSplitter;
    QPushButton *_restoreButton = nullptr;
    QPushButton *_downloadButton = nullptr;
    QPushButton *_deleteButton = nullptr;
    QAction *_restoreAction = nullptr;
    QAction *_downloadAction = nullptr;
    QAction *_deleteAction = nullptr;
    QAction *_selectAllAction = nullptr;
    QAction *_clearSelectionAction = nullptr;
    void setupMainSplitter();
    void setupContextMenu();
    void updatePreviewInfo();
    int visibleItemCount() const;
    QList<QListWidgetItem *> selectedVisibleItems() const;
    QString failedNotesHtml(const QStringList &names) const;
};

#endif    // TRASHDIALOG_H
