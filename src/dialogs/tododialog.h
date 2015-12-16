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
    void clearTodoList();
private slots:
    void on_TodoDialog_finished(int result);
    void on_todoListSelector_currentIndexChanged(const QString &arg1);
    void on_todoList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_prioritySlider_valueChanged(int value);
    void on_showCompletedItemsCheckBox_clicked();

    void on_toolButton_clicked();

private:
    Ui::TodoDialog *ui;
    QSplitter *mainSplitter;
    SimpleCrypt *crypto;
    CalendarItem *currentCalendarItem;
    void setupMainSplitter();
    void storeSettings();
    void loadTodoListData();
    void reloadTodoList();
    int findTodoItemRowByUID(QString uid);
    void resetEditFrameControls();
    void setupUi();
    void updateCurrentCalendarItemWithFormData();
};

#endif // TODODIALOG_H
