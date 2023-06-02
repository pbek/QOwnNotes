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

private:
    Ui::NextcloudDeckDialog *ui;
};

#endif // NEXTCLOUDDECKDIALOG_H
