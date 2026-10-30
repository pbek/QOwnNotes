#ifndef NEXTCLOUDDECKDIALOG_H
#define NEXTCLOUDDECKDIALOG_H

#include <QDialog>
#include <QHash>
#include <QList>
#include <QPointer>
#include <QSplitter>
#include <QTreeWidgetItem>

#include "masterdialog.h"
#include "services/nextclouddeckservice.h"

namespace Ui {
class NextcloudDeckDialog;
}

class QPushButton;

class NextcloudDeckDialog : public MasterDialog {
    Q_OBJECT
   Q_SIGNALS:
    void searchInNotes(QString text);
    void cardLinksSelected(const QString &links);

   public:
    enum class Mode { Manage, InsertLinks };
    explicit NextcloudDeckDialog(QWidget *parent = nullptr, Mode mode = Mode::Manage);
    ~NextcloudDeckDialog();
    void setTitle(const QString &title);
    void setCardId(int id, int boardId = -1);

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

    void on_newItemEdit_textChanged(const QString &arg1);

    void on_newItemEdit_returnPressed();

    void on_deleteCardButton_clicked();

    void on_archiveCardButton_clicked();

    void on_moveCardButton_clicked();

    void on_cardItemTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void on_cardItemTreeWidget_customContextMenuRequested(const QPoint &pos);

    void searchLinkInNotes(QTreeWidgetItem *item);

    void on_showArchivedCardsCheckBox_toggled(bool checked);

    void on_boardStackComboBox_currentIndexChanged(int index);

    void on_reloadCardListButton_clicked();

   protected:
    void showEvent(QShowEvent *event) override;

   private:
    bool _initialLoadPending = false;
    bool _initialLoadScheduled = false;
    int _pendingCardId = -1;
    int _pendingBoardId = -1;
    int _loadingDepth = 0;
    bool _loadingCanceled = false;
    QPointer<NextcloudDeckService> _activeDeckService;
    void initialLoad();
    void beginLoading(const QString &title);
    void endLoading();
    void trackDeckService(NextcloudDeckService &nextcloudDeckService);
    Ui::NextcloudDeckDialog *ui;
    const Mode _mode;
    QPushButton *_insertLinksButton = nullptr;
    void insertSelectedCardLinks();
    void updateInsertLinksButton();
    void setupMainSplitter();
    QSplitter *mainSplitter;
    void setupUi();
    void refreshUi();
    void reloadDeckData();
    void loadBoardStackComboBox();
    void persistSelectedBoardAndStack() const;
    bool selectBoardStack(int boardId, int stackId = -1);
    bool selectBoardForCard(int boardId, int cardId);
    void resetEditFrameControls();
    void jumpToCard(int id);
    int selectedBoardId() const;
    int selectedStackId() const;
    void configureDeckService(NextcloudDeckService &nextcloudDeckService) const;
    void configureDeckServiceForCard(NextcloudDeckService &nextcloudDeckService, int cardId) const;
    void updateDeckControlsEnabledState();
    void deleteSelectedCards();
    void moveCards(const QList<int> &cardIds);
    QHash<int, NextcloudDeckService::Card> _cards;
    NextcloudDeckService::Card _currentCard;

    void openUrlInBrowserForItem(const QTreeWidgetItem *item);
    void addCardLinkToCurrentNote(const QTreeWidgetItem *item);
    void selectCardInList(int cardId);
    bool eventFilter(QObject *obj, QEvent *event) override;
};

#endif    // NEXTCLOUDDECKDIALOG_H
