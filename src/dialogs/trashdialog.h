#ifndef TRASHDIALOG_H
#define TRASHDIALOG_H

#include "masterdialog.h"

namespace Ui {
class TrashDialog;
}

class MainWindow;
class QAbstractButton;
class QJSValue;
class QSplitter;

class TrashDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit TrashDialog(const QJSValue &notes, QWidget *parent = nullptr);
    explicit TrashDialog(QWidget *parent = nullptr);
    ~TrashDialog();

    void setNotes(const QJSValue &notes);

   private slots:
    void storeSettings();
    void on_trashListWidget_currentRowChanged(int currentRow);
    void dialogButtonClicked(QAbstractButton *button);
    void on_searchLineEdit_textChanged(const QString &arg1);

   private:
    enum ButtonRole {
        Unset,    // nothing was selected
        Download,
        DeleteOnServer,
        RestoreOnServer,
        Cancel
    };

    Ui::TrashDialog *ui;
    QSplitter *trashSplitter;
    void setupMainSplitter();
};

#endif    // TRASHDIALOG_H
