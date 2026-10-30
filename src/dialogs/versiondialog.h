#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include "masterdialog.h"

class MainWindow;
class QAbstractButton;
class QJSValue;
class QPushButton;
class QSplitter;

namespace Ui {
class VersionDialog;
}

class VersionDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit VersionDialog(const QJSValue &versions, QWidget *parent = nullptr);
    explicit VersionDialog(QWidget *parent = nullptr);
    ~VersionDialog();

    void setVersions(const QJSValue &versions);

   private slots:
    void storeSettings();
    void on_versionListWidget_currentRowChanged(int currentRow);
    void dialogButtonClicked(QAbstractButton *button);
    void on_searchLineEdit_textChanged(const QString &text);

   private:
    enum ButtonRole {
        Unset,    // nothing was selected
        Restore,
        Cancel
    };

    Ui::VersionDialog *ui;
    QSplitter *versionSplitter;
    QPushButton *_restoreButton;
    QStringList *_diffList;
    QStringList *_dataList;
    void setupMainSplitter();
    void updateVersionPreview();
};

#endif    // VERSIONDIALOG_H
