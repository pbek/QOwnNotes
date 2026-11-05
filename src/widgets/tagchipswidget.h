#pragma once

#include <QColor>
#include <QList>
#include <QPointer>
#include <QStringList>
#include <QWidget>

class FlowLayout;
class QButtonGroup;
class QFrame;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QToolButton;

/**
 * Shows tags as colored chips that can be removed, with a popup to search, toggle and create tags
 *
 * It is used for the tags of Nextcloud Deck cards and for the tags of notes.
 */
class TagChipsWidget : public QWidget {
    Q_OBJECT

   public:
    struct Item {
        int id = 0;
        QString name;
        // An invalid color shows a neutral chip
        QColor color;
        // Names of the parent tags, starting at the top level, for nested tags
        QStringList parentNames;

        QString path() const;
    };

    explicit TagChipsWidget(QWidget *parent = nullptr);

    void setItems(const QList<Item> &items);
    QList<Item> items() const;
    void addItem(const Item &item, bool select = true);

    void setSelectedIds(const QList<int> &ids);
    QList<int> selectedIds() const;
    QList<Item> selectedItems() const;
    bool hasSameSelection(const QList<int> &ids) const;

    // Selected tags that only apply to a part of the tagged objects, like some selected notes
    void setPartiallySelectedIds(const QList<int> &ids);
    QList<int> partiallySelectedIds() const;

    void setNewTagColors(const QStringList &colors);
    void setPopupHintText(const QString &text);
    void setAddButtonToolTip(const QString &toolTip);
    void setAddButtonShortcut(const QKeySequence &shortcut);
    void setChipToolTipTemplates(const QString &clickToolTip, const QString &removeToolTip);
    void setPartialSelectionToolTip(const QString &toolTip);
    void setChipsClickable(bool clickable);

    bool isPopupVisible() const;
    int rowHeight() const;

    static QColor textColorForBackground(const QColor &color);
    static QIcon colorIcon(const QColor &color, int size = 14);
    static QString nextUnusedColor(const QStringList &colors, const QList<QColor> &usedColors);

   public slots:
    void showPopup();

   signals:
    void selectedIdsChanged();
    // Emitted for every toggle by the user, also for partially selected tags
    void tagToggled(int id, bool selected);
    void createTagRequested(const QString &name, const QString &color);
    void tagClicked(int id);
    void popupAboutToShow();

   protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void changeEvent(QEvent *event) override;

   private:
    QList<Item> _items;
    QList<int> _selectedIds;
    QList<int> _partiallySelectedIds;
    QStringList _newTagColors;
    QString _popupHintText;
    QString _chipClickToolTip;
    QString _chipRemoveToolTip;
    QString _partialSelectionToolTip;
    QString _addButtonToolTip;
    bool _chipsClickable = false;
    FlowLayout *_flowLayout = nullptr;
    QToolButton *_addButton = nullptr;
    QPointer<QFrame> _popup;
    QPointer<QLineEdit> _popupSearchEdit;
    QPointer<QListWidget> _popupList;
    QPointer<QWidget> _popupColorFrame;
    QPointer<QLabel> _popupEmptyLabel;
    QPointer<QButtonGroup> _popupColorGroup;

    void rebuildChips();
    QWidget *createChip(const Item &item);
    void updatePopupList();
    void fitPopupSize();
    void toggleItem(int id);
    void removeItem(int id);
    void activatePopupItem(QListWidgetItem *item);
    void activateFromSearch();
    void requestTagCreation();
    QString nextNewTagColor() const;
    int itemIndex(int id) const;
    int itemIndexForName(const QString &name) const;
    void updateAddButton();
};
