#pragma once

#include <QPointer>
#include <QStyledItemDelegate>
#include <QWidget>

#include "services/nextclouddeckservice.h"

class FlowLayout;
class QButtonGroup;
class QFrame;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QToolButton;

/**
 * Editor for the tags (Deck labels) of a Nextcloud Deck card
 *
 * The selected tags are shown as colored chips that can be removed. A popup allows
 * searching, toggling and creating tags of the board.
 */
class NextcloudDeckLabelsWidget : public QWidget {
    Q_OBJECT

   public:
    using Label = NextcloudDeckService::Label;

    explicit NextcloudDeckLabelsWidget(QWidget *parent = nullptr);

    void setAvailableLabels(const QList<Label> &labels);
    QList<Label> availableLabels() const;
    void addAvailableLabel(const Label &label, bool select = true);

    void setSelectedLabelIds(const QList<int> &labelIds);
    QList<int> selectedLabelIds() const;
    QList<Label> selectedLabels() const;
    bool hasSameSelection(const QList<int> &labelIds) const;

    bool isLabelPopupVisible() const;
    int rowHeight() const;

    static QColor labelColor(const Label &label);
    static QColor textColorForBackground(const QColor &color);
    static QIcon colorIcon(const QColor &color, int size = 14);

   public slots:
    void showLabelPopup();

   signals:
    void selectedLabelsChanged();
    void createLabelRequested(const QString &title, const QString &color);

   protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void changeEvent(QEvent *event) override;

   private:
    QList<Label> _availableLabels;
    QList<int> _selectedLabelIds;
    FlowLayout *_flowLayout = nullptr;
    QToolButton *_addButton = nullptr;
    QPointer<QFrame> _popup;
    QPointer<QLineEdit> _popupSearchEdit;
    QPointer<QListWidget> _popupList;
    QPointer<QWidget> _popupColorFrame;
    QPointer<QLabel> _popupEmptyLabel;
    QPointer<QButtonGroup> _popupColorGroup;

    void rebuildChips();
    QWidget *createChip(const Label &label);
    void updatePopupList();
    void toggleLabel(int labelId);
    void removeLabel(int labelId);
    void activatePopupItem(QListWidgetItem *item);
    void activateFromSearch();
    void requestLabelCreation();
    QString nextLabelColor() const;
    int labelIndex(int labelId) const;
    int labelIndexForTitle(const QString &title) const;
};

/**
 * Paints the tags of a card as colored pills in an item view
 */
class NextcloudDeckLabelDelegate : public QStyledItemDelegate {
   public:
    // Role holding a list of maps with "title" and "color" entries
    static const int LabelsRole = Qt::UserRole + 10;

    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    static QVariant labelsToVariant(const QList<NextcloudDeckService::Label> &labels);
    static QFont pillFont(const QFont &font);
};
