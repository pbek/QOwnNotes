#ifndef NOTEDIFFDIALOG_H
#define NOTEDIFFDIALOG_H

#include "masterdialog.h"

namespace Ui {
class NoteDiffDialog;
}

class QButtonGroup;
class QCheckBox;
class QAbstractButton;

class NoteDiffDialog : public MasterDialog {
    Q_OBJECT

   public:
    enum ButtonRole {
        Unset,    // nothing was selected
        Overwrite,
        Reload,
        Ignore,
        Cancel
    };

    explicit NoteDiffDialog(QWidget *parent = nullptr, const QString &html = QString());
    ~NoteDiffDialog();
    int resultActionRole();

   private slots:
    void dialogButtonClicked(QAbstractButton *button);

    void notificationButtonGroupPressed(QAbstractButton *button);

    void notificationNoneCheckBoxCheck();

   private:
    Ui::NoteDiffDialog *ui;
    int actionRole;
    QButtonGroup *_notificationButtonGroup;
    QCheckBox *_notificationNoneCheckBox;
};

#endif    // NOTEDIFFDIALOG_H
