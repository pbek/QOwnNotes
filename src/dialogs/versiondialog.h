#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include "mainwindow.h"
#include "masterdialog.h"

#include <QAbstractButton>
#include <QDialog>
#include <QScriptValue>
#include <QSplitter>

namespace Ui {
class VersionDialog;
}

class VersionDialog : public MasterDialog
{
    Q_OBJECT

public:
    explicit VersionDialog(QScriptValue versions, MainWindow *mainWindow, QWidget *parent = 0);
    ~VersionDialog();

private slots:
    void storeSettings();
    void on_versionListWidget_currentRowChanged(int currentRow);
    void dialogButtonClicked(QAbstractButton *button);
private:
    enum ButtonRole {
        Unset,  // nothing was selected
        Restore,
        Cancel
    };

    Ui::VersionDialog *ui;
    QSplitter *versionSplitter;
    QStringList *diffList;
    QStringList *dataList;
    MainWindow *mainWindow;
    void setupMainSplitter();
};

#endif // VERSIONDIALOG_H
