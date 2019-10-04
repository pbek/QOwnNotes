#ifndef TRASHDIALOG_H
#define TRASHDIALOG_H

#include "mainwindow.h"
#include "masterdialog.h"

#include <QAbstractButton>
#include <QDialog>
#include <QSplitter>
#include <QJSValue>

namespace Ui {
class TrashDialog;
}

class TrashDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit TrashDialog(QJSValue notes, MainWindow *mainWindow,
                         QWidget *parent = 0);
    ~TrashDialog();

private slots:
    void storeSettings();
    void on_trashListWidget_currentRowChanged(int currentRow);
    void dialogButtonClicked(QAbstractButton *button);
    void on_searchLineEdit_textChanged(const QString &arg1);

private:
    enum ButtonRole {
        Unset,  // nothing was selected
        Download,
        RestoreOnServer,
        Cancel
    };

    Ui::TrashDialog *ui;
    QSplitter *trashSplitter;
    QStringList *dataList;
    QList<int> *timestampList;
    MainWindow *mainWindow;
    void setupMainSplitter();
};

#endif // TRASHDIALOG_H
