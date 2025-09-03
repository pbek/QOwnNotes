#ifndef NEXTCLOUDDECKDIALOG_H
#define NEXTCLOUDDECKDIALOG_H

#include <QDialog>
#include <QHash>
#include <QList>
#include <QSplitter>
#include <QTreeWidgetItem>

#include "masterdialog.h"
#include "services/nextclouddeckservice.h"

namespace Ui {
class NextcloudDeckDialog;
}

class NextcloudDeckDialog : public MasterDialog {
    Q_OBJECT

   public:
    explicit NextcloudDeckDialog(QWidget *parent = nullptr, bool listMode = false);
    ~NextcloudDeckDialog();
    void setTitle(const QString &title);

   private slots:
    void on_saveButton_clicked();

    void on_add1HourButton_clicked();

    void on_add10MinButton_clicked();

    void on_add1DayButton_clicked();

    void on_sub10MinButton_clicked();

    void on_sub1HourButton_clicked();

    void on_subd1DayButton_clicked();

    void on_titleLineEdit_textChanged(const QString &arg1);

    void on_dueDateTimeCheckBox_toggled(bool checked);

    void reloadCardList();

    void on_cardItemTreeWidget_currentItemChanged(QTreeWidgetItem *current,
                                                  QTreeWidgetItem *previous);

   private:
    Ui::NextcloudDeckDialog *ui;
    void setupMainSplitter();
    QSplitter *mainSplitter;
    void setupUi();
    void refreshUi();
    void resetEditFrameControls();
    QHash<int, NextcloudDeckService::Card> _cards;
    NextcloudDeckService::Card _currentCard;
};

#endif    // NEXTCLOUDDECKDIALOG_H
