#pragma once

#include <QStyledItemDelegate>

#include "services/nextclouddeckservice.h"
#include "widgets/tagchipswidget.h"

/**
 * Paints the tags (Deck labels) of a card as colored pills in an item view
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
    static QColor labelColor(const QString &color);
    static TagChipsWidget::Item labelToItem(const NextcloudDeckService::Label &label);
    static QList<TagChipsWidget::Item> labelsToItems(
        const QList<NextcloudDeckService::Label> &labels);

   private:
    static QFont pillFont(const QFont &font);
};
