#ifndef TODODIALOG_H
#define TODODIALOG_H

#include <QTreeWidgetItem>

#include "entities/calendaritem.h"
#include "masterdialog.h"

namespace Ui {
class TodoDialog;
}

class MainWindow;
class QSplitter;
class QListWidgetItem;

class TodoDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit TodoDialog(const QString &taskUid = QString(), QWidget *parent = nullptr);
    ~TodoDialog();

    void reloadTodoListItems();
    void clearTodoList();
    void todoItemLoadingProgressBarIncrement();
    void todoItemLoadingProgressBarSetMaximum(int value);
    void todoItemLoadingProgressBarHide();
    void todoItemLoadingProgressBarHideIfOnMaximum();
    void jumpToTask(const QString &taskUid);
    void refreshUi();

   public slots:
    void reloadTodoList();

   private slots:
    void on_TodoDialog_finished(int result);
    void on_todoListSelector_currentIndexChanged(const QString &arg1);
    void on_prioritySlider_valueChanged(int value);
    void on_showCompletedItemsCheckBox_clicked();
    void on_saveButton_clicked();
    void on_todoItemLoadingProgressBar_valueChanged(int value);
    void on_newItemEdit_returnPressed();
    void on_removeButton_clicked();
    void on_reminderCheckBox_clicked();
    void on_summaryEdit_returnPressed();
    void on_newItemEdit_textChanged();
    void onSaveAndInsertButtonClicked();
    void onImportAsNoteButtonClicked();
    void clearCacheAndReloadTodoList();
    void on_todoItemTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                  QTreeWidgetItem *previous);
    void on_todoItemTreeWidget_itemChanged(QTreeWidgetItem *item, int column);
    void on_todoItemTreeWidget_customContextMenuRequested(QPoint pos);
    void on_showDueTodayItemsOnlyCheckBox_clicked();

   private:
    Ui::TodoDialog *ui;
    QSplitter *mainSplitter;
    CalendarItem currentCalendarItem;
    CalendarItem lastCreatedCalendarItem;
    QString _jumpToCalendarItemUid;
    bool _setFocusToDescriptionEdit;
    QTreeWidgetItem *firstVisibleTodoItemTreeItem;
    void setupMainSplitter();
    void storeSettings();
    void loadTodoListData();
    QTreeWidgetItem *findTodoItemTreeWidgetItemByUID(const QString &uid);
    void resetEditFrameControls();
    void setupUi();
    void updateCurrentCalendarItemWithFormData();
    void searchForSearchLineTextInNoteTextEdit();
    void searchInDescriptionTextEdit(QString &str);
    void createNewTodoItem(const QString &name = QLatin1String(""),
                           const QString &relatedUid = QLatin1String(""));

   protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

    void jumpToTodoListItem();
   private slots:
    void updateCalendarItem(CalendarItem item);
};

#endif    // TODODIALOG_H
