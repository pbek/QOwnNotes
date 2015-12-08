#ifndef TODODIALOG_H
#define TODODIALOG_H

#include <QDialog>
#include <QSplitter>

namespace Ui {
class TodoDialog;
}

class TodoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TodoDialog(QWidget *parent = 0);
    ~TodoDialog();

private slots:
    void on_TodoDialog_finished(int result);

private:
    Ui::TodoDialog *ui;
    QSplitter *mainSplitter;
    void setupMainSplitter();
    void storeSettings();
};

#endif // TODODIALOG_H
