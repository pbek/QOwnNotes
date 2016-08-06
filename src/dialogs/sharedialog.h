#ifndef SHAREDIALOG_H
#define SHAREDIALOG_H

#include <QDialog>
#include <entities/note.h>

namespace Ui {
class ShareDialog;
}

class ShareDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShareDialog(Note note, QWidget *parent = 0);
    ~ShareDialog();

    void updateDialog();

private slots:
    void on_linkCheckBox_toggled(bool checked);

private:
    Note note;
    Ui::ShareDialog *ui;
};

#endif // SHAREDIALOG_H
