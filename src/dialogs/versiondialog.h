#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include "masterdialog.h"

class MainWindow;
class QAbstractButton;
class QJSValue;
class QSplitter;

namespace Ui {
class VersionDialog;
}

class VersionDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit VersionDialog(const QJSValue &versions, QWidget *parent = 0);
    ~VersionDialog();

   private slots:
    void storeSettings();
    void on_versionListWidget_currentRowChanged(int currentRow);
    void dialogButtonClicked(QAbstractButton *button);

   private:
    enum ButtonRole {
        Unset,    // nothing was selected
        Restore,
        Cancel
    };

    Ui::VersionDialog *ui;
    QSplitter *versionSplitter;
    QStringList *_diffList;
    QStringList *_dataList;
    void setupMainSplitter();
};

#endif    // VERSIONDIALOG_H
