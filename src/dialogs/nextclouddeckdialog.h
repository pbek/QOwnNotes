#ifndef NEXTCLOUDDECKDIALOG_H
#define NEXTCLOUDDECKDIALOG_H

#include <QDialog>

namespace Ui {
class NextcloudDeckDialog;
}

class NextcloudDeckDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NextcloudDeckDialog(QWidget *parent = nullptr);
    ~NextcloudDeckDialog();

private slots:
    void on_saveButton_clicked();

    void on_add1HourButton_clicked();

    void on_add10MinButton_clicked();

    void on_add1DayButton_clicked();

    void on_sub10MinButton_clicked();

    void on_sub1HourButton_clicked();

    void on_subd1DayButton_clicked();

private:
    Ui::NextcloudDeckDialog *ui;
};

#endif // NEXTCLOUDDECKDIALOG_H
