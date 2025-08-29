#ifndef NEXTCLOUDDECKDIALOG_H
#define NEXTCLOUDDECKDIALOG_H

#include <QDialog>
#include <QSplitter>

#include "masterdialog.h"

namespace Ui {
class NextcloudDeckDialog;
}

class NextcloudDeckDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit NextcloudDeckDialog(QWidget *parent = nullptr);
    ~NextcloudDeckDialog();
    void setTitle(const QString &title);

   private slots:
    void on_saveButton_clicked();

    void on_add1HourButton_clicked();

    void on_add10MinButton_clicked();

    void on_add1DayButton_clicked();

    void on_sub10MinButton_clicked();

    void on_sub1HourButton_clicked();

    void on_subd1DayButton_clicked();

    void on_titleLineEdit_textChanged(const QString &arg1);

    void on_dueDateTimeCheckBox_toggled(bool checked);

    void reloadCardList();

   private:
    Ui::NextcloudDeckDialog *ui;
    void setupMainSplitter();
    QSplitter *mainSplitter;
    void setupUi();
    void refreshUi();
};

#endif    // NEXTCLOUDDECKDIALOG_H
