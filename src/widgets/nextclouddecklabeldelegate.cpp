#include "nextclouddecklabeldelegate.h"

#include <QApplication>
#include <QPainter>

QColor NextcloudDeckLabelDelegate::labelColor(const QString &color) {
    const QColor result(QStringLiteral("#") + color);
    return result.isValid() ? result : QColor(0x99, 0x99, 0x99);
}

TagChipsWidget::Item NextcloudDeckLabelDelegate::labelToItem(
    const NextcloudDeckService::Label &label) {
    TagChipsWidget::Item item;
    item.id = label.id;
    item.name = label.title;
    item.color = labelColor(label.color);
    return item;
}

QList<TagChipsWidget::Item> NextcloudDeckLabelDelegate::labelsToItems(
    const QList<NextcloudDeckService::Label> &labels) {
    QList<TagChipsWidget::Item> items;
    for (const auto &label : labels) {
        items.append(labelToItem(label));
    }

    return items;
}

QFont NextcloudDeckLabelDelegate::pillFont(const QFont &font) {
    QFont result = font;

    if (result.pointSizeF() > 0) {
        result.setPointSizeF(result.pointSizeF() * 0.9);
    } else if (result.pixelSize() > 0) {
        result.setPixelSize(qMax(8, qRound(result.pixelSize() * 0.9)));
    }

    return result;
}

void NextcloudDeckLabelDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                       const QModelIndex &index) const {
    const QVariantList labels = index.data(LabelsRole).toList();

    if (labels.isEmpty()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    // Let the style draw the background and selection, but not the plain text
    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.text.clear();
    const QWidget *widget = opt.widget;
    QStyle *style = widget != nullptr ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

    const QFont font = pillFont(opt.font);
    const QFontMetrics metrics(font);
    const int horizontalPadding = 7;
    const int spacing = 4;
    const int pillHeight = qMin(opt.rect.height() - 4, metrics.height() + 4);
    const int top = opt.rect.top() + (opt.rect.height() - pillHeight) / 2;
    const int right = opt.rect.right() - 3;
    int x = opt.rect.left() + 4;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setFont(font);

    for (int i = 0; i < labels.count(); ++i) {
        const QVariantMap label = labels.at(i).toMap();
        const QString title = label.value(QStringLiteral("title")).toString();
        const QColor background = labelColor(label.value(QStringLiteral("color")).toString());
        const int available = right - x;
        const int remaining = labels.count() - i;
        const QString moreText = QStringLiteral("+%1").arg(remaining);
        const int minimumWidth =
            2 * horizontalPadding + metrics.horizontalAdvance(QStringLiteral("…"));

        if (available < minimumWidth) {
            // Show how many tags didn't fit, if there is room for it
            if (available >= metrics.horizontalAdvance(moreText)) {
                painter->setPen(opt.palette.color(QPalette::Disabled, QPalette::Text));
                painter->drawText(QRect(x, top, available, pillHeight),
                                  Qt::AlignLeft | Qt::AlignVCenter, moreText);
            }
            break;
        }

        int pillWidth = metrics.horizontalAdvance(title) + 2 * horizontalPadding;
        QString text = title;

        if (pillWidth > available) {
            text = metrics.elidedText(title, Qt::ElideRight, available - 2 * horizontalPadding);
            pillWidth = available;
        }

        const QRectF pillRect(x + 0.5, top + 0.5, pillWidth - 1, pillHeight - 1);
        painter->setPen(QPen(background.darker(115), 1));
        painter->setBrush(background);
        painter->drawRoundedRect(pillRect, pillHeight / 2.0, pillHeight / 2.0);
        painter->setPen(TagChipsWidget::textColorForBackground(background));
        painter->drawText(pillRect, Qt::AlignCenter, text);

        x += pillWidth + spacing;
    }

    painter->restore();
}

QSize NextcloudDeckLabelDelegate::sizeHint(const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const {
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    const QVariantList labels = index.data(LabelsRole).toList();

    if (labels.isEmpty()) {
        return size;
    }

    const QFontMetrics metrics(pillFont(option.font));
    int width = 8;
    for (const auto &value : labels) {
        width +=
            metrics.horizontalAdvance(value.toMap().value(QStringLiteral("title")).toString()) +
            2 * 7 + 4;
    }

    size.setWidth(width);
    size.setHeight(qMax(size.height(), metrics.height() + 8));
    return size;
}

QVariant NextcloudDeckLabelDelegate::labelsToVariant(
    const QList<NextcloudDeckService::Label> &labels) {
    QVariantList list;
    for (const auto &label : labels) {
        QVariantMap map;
        map.insert(QStringLiteral("title"), label.title);
        map.insert(QStringLiteral("color"), label.color);
        list.append(map);
    }

    return list;
}
