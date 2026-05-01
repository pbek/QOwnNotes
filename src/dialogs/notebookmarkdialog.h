#ifndef NOTEBOOKMARKDIALOG_H
#define NOTEBOOKMARKDIALOG_H

#include <QHash>

#include "entities/notehistory.h"
#include "masterdialog.h"

namespace Ui {
class NoteBookmarkDialog;
}

class NoteBookmarkDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit NoteBookmarkDialog(QWidget *parent = nullptr);
    ~NoteBookmarkDialog();

    void setBookmarks(const QHash<int, NoteHistoryItem> &bookmarks);

   signals:
    void jumpToBookmarkRequested(int slot);
    void deleteBookmarkRequested(int slot);
    void reloadRequested();

   protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

   private slots:
    void onJumpButtonClicked();
    void onDeleteButtonClicked();
    void onReloadButtonClicked();
    void onBookmarkTableDoubleClicked(int row, int column);

   private:
    Ui::NoteBookmarkDialog *ui;

    void reloadTable();
    int selectedSlot() const;
};

#endif    // NOTEBOOKMARKDIALOG_H
