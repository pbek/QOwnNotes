#ifndef TODODIALOG_H
#define TODODIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QSplitter>

#include <simplecrypt/simplecrypt.h>

#include <entities/calendaritem.h>

namespace Ui {
class TodoDialog;
}

class TodoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TodoDialog(SimpleCrypt *crypto, QWidget *parent = 0);
    ~TodoDialog();

    void reloadTodoListItems();
    void reloadTodoList();
    void clearTodoList();
    void todoItemLoadingProgressBarIncrement();
    void todoItemLoadingProgressBarSetMaximum(int value);
    void todoItemLoadingProgressBarHide();
    void todoItemLoadingProgressBarHideIfOnMaximum();
private slots:
    void on_TodoDialog_finished(int result);
    void on_todoListSelector_currentIndexChanged(const QString &arg1);
    void on_todoList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_prioritySlider_valueChanged(int value);
    void on_showCompletedItemsCheckBox_clicked();
    void on_saveButton_clicked();
    void on_todoItemLoadingProgressBar_valueChanged(int value);

    void on_newItemEdit_returnPressed();

private:
    Ui::TodoDialog *ui;
    QSplitter *mainSplitter;
    SimpleCrypt *crypto;
    CalendarItem currentCalendarItem;
    CalendarItem lastCreatedCalendarItem;
    void setupMainSplitter();
    void storeSettings();
    void loadTodoListData();
    int findTodoItemRowByUID(QString uid);
    void resetEditFrameControls();
    void setupUi();
    void updateCurrentCalendarItemWithFormData();
};

#endif // TODODIALOG_H
