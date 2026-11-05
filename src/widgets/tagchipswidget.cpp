#include "tagchipswidget.h"

#include <QApplication>
#include <QButtonGroup>
#include <QEvent>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QSignalBlocker>
#include <QToolButton>
#include <QVBoxLayout>
#include <algorithm>

#include "helpers/flowlayout.h"

namespace {
// Marks the list item that creates a new tag instead of toggling an existing one
const int CreateTagItemId = -1;

qreal applicationDevicePixelRatio() { return qApp != nullptr ? qApp->devicePixelRatio() : 1.0; }

QString cssColor(const QColor &color) {
    return QStringLiteral("rgba(%1, %2, %3, %4)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(color.alpha());
}

QColor withAlpha(QColor color, qreal alpha) {
    color.setAlphaF(alpha);
    return color;
}

QFont smallerFont(const QFont &font) {
    QFont result = font;

    if (result.pointSizeF() > 0) {
        result.setPointSizeF(result.pointSizeF() * 0.9);
    } else if (result.pixelSize() > 0) {
        result.setPixelSize(qMax(8, qRound(result.pixelSize() * 0.9)));
    }

    return result;
}
}    // namespace

QString TagChipsWidget::Item::path() const {
    QStringList names = parentNames;
    names.append(name);
    return names.join(QStringLiteral(" › "));
}

TagChipsWidget::TagChipsWidget(QWidget *parent) : QWidget(parent) {
    _flowLayout = new FlowLayout(this, 0, 4, 4);
    _addButtonToolTip = tr("Add or remove tags, or create new tags");
    _chipRemoveToolTip = tr("Remove tag \"%1\"");

    _addButton = new QToolButton(this);
    _addButton->setObjectName(QStringLiteral("addTagButton"));
    _addButton->setIcon(
        QIcon::fromTheme(QStringLiteral("list-add"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/list-add.svg"))));
    _addButton->setAutoRaise(true);
    connect(_addButton, &QToolButton::clicked, this, &TagChipsWidget::showPopup);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    updateAddButton();
    rebuildChips();
}

void TagChipsWidget::setItems(const QList<Item> &items) {
    _items = items;

    // Sort the tags alphabetically, nested tags are grouped below their parents
    std::sort(_items.begin(), _items.end(), [](const Item &a, const Item &b) {
        return QString::localeAwareCompare(a.path().toLower(), b.path().toLower()) < 0;
    });

    // Drop selected tags that aren't available anymore
    QList<int> selectedIds;
    for (const int id : _selectedIds) {
        if (itemIndex(id) >= 0) {
            selectedIds.append(id);
        }
    }

    QList<int> partiallySelectedIds;
    for (const int id : _partiallySelectedIds) {
        if (selectedIds.contains(id)) {
            partiallySelectedIds.append(id);
        }
    }

    const bool selectionChanged = selectedIds != _selectedIds;
    _selectedIds = selectedIds;
    _partiallySelectedIds = partiallySelectedIds;
    rebuildChips();
    updatePopupList();

    if (selectionChanged) {
        emit selectedIdsChanged();
    }
}

QList<TagChipsWidget::Item> TagChipsWidget::items() const { return _items; }

void TagChipsWidget::addItem(const Item &item, bool select) {
    if (item.id < 1) {
        return;
    }

    QList<Item> items = _items;
    const int index = itemIndex(item.id);

    if (index >= 0) {
        items[index] = item;
    } else {
        items.append(item);
    }

    if (select && !_selectedIds.contains(item.id)) {
        _selectedIds.append(item.id);
    }

    // Start with a fresh search after a tag was created from the popup
    if (_popupSearchEdit) {
        const QSignalBlocker blocker(_popupSearchEdit);
        _popupSearchEdit->clear();
    }

    setItems(items);

    if (select) {
        emit selectedIdsChanged();
    }
}

void TagChipsWidget::setSelectedIds(const QList<int> &ids) {
    QList<int> selectedIds;
    for (const int id : ids) {
        if (itemIndex(id) >= 0 && !selectedIds.contains(id)) {
            selectedIds.append(id);
        }
    }

    if (selectedIds == _selectedIds) {
        return;
    }

    _selectedIds = selectedIds;

    for (int i = _partiallySelectedIds.count() - 1; i >= 0; --i) {
        if (!_selectedIds.contains(_partiallySelectedIds.at(i))) {
            _partiallySelectedIds.removeAt(i);
        }
    }

    rebuildChips();
    updatePopupList();
    emit selectedIdsChanged();
}

QList<int> TagChipsWidget::selectedIds() const { return _selectedIds; }

/**
 * Returns the selected tags in the order of the available tags
 */
QList<TagChipsWidget::Item> TagChipsWidget::selectedItems() const {
    QList<Item> items;
    for (const auto &item : _items) {
        if (_selectedIds.contains(item.id)) {
            items.append(item);
        }
    }

    return items;
}

bool TagChipsWidget::hasSameSelection(const QList<int> &ids) const {
    if (ids.count() != _selectedIds.count()) {
        return false;
    }

    for (const int id : ids) {
        if (!_selectedIds.contains(id)) {
            return false;
        }
    }

    return true;
}

void TagChipsWidget::setPartiallySelectedIds(const QList<int> &ids) {
    QList<int> partiallySelectedIds;
    for (const int id : ids) {
        if (_selectedIds.contains(id) && !partiallySelectedIds.contains(id)) {
            partiallySelectedIds.append(id);
        }
    }

    if (partiallySelectedIds == _partiallySelectedIds) {
        return;
    }

    _partiallySelectedIds = partiallySelectedIds;
    rebuildChips();
    updatePopupList();
}

QList<int> TagChipsWidget::partiallySelectedIds() const { return _partiallySelectedIds; }

/**
 * Sets the colors that can be chosen for new tags, without colors no color can be chosen
 */
void TagChipsWidget::setNewTagColors(const QStringList &colors) { _newTagColors = colors; }

void TagChipsWidget::setPopupHintText(const QString &text) { _popupHintText = text; }

void TagChipsWidget::setAddButtonToolTip(const QString &toolTip) {
    _addButtonToolTip = toolTip;
    updateAddButton();
}

void TagChipsWidget::setAddButtonShortcut(const QKeySequence &shortcut) {
    _addButton->setShortcut(shortcut);
    updateAddButton();
}

/**
 * Sets the tooltips of the chips, "%1" is replaced by the name of the tag
 */
void TagChipsWidget::setChipToolTipTemplates(const QString &clickToolTip,
                                             const QString &removeToolTip) {
    _chipClickToolTip = clickToolTip;
    _chipRemoveToolTip = removeToolTip;
    rebuildChips();
}

void TagChipsWidget::setPartialSelectionToolTip(const QString &toolTip) {
    _partialSelectionToolTip = toolTip;
    rebuildChips();
}

void TagChipsWidget::setChipsClickable(bool clickable) {
    _chipsClickable = clickable;
    rebuildChips();
}

bool TagChipsWidget::isPopupVisible() const { return _popup != nullptr && _popup->isVisible(); }

/**
 * Height of the chips, they are as high as the add button to line up in the flow layout
 */
int TagChipsWidget::rowHeight() const { return qMax(fontMetrics().height() + 10, 24); }

/**
 * Returns a readable text color for the background color, based on its perceived brightness
 */
QColor TagChipsWidget::textColorForBackground(const QColor &color) {
    const int brightness = (color.red() * 299 + color.green() * 587 + color.blue() * 114) / 1000;
    return brightness > 150 ? QColor(0x22, 0x22, 0x22) : QColor(Qt::white);
}

/**
 * Returns a round color swatch, an invalid color results in an outlined circle
 */
QIcon TagChipsWidget::colorIcon(const QColor &color, int size) {
    const qreal ratio = applicationDevicePixelRatio();
    QPixmap pixmap(QSize(size, size) * ratio);
    pixmap.setDevicePixelRatio(ratio);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    const QRectF rect(1, 1, size - 2, size - 2);

    if (color.isValid()) {
        painter.setPen(QPen(color.darker(130), 1));
        painter.setBrush(color);
    } else {
        painter.setPen(
            QPen(QApplication::palette().color(QPalette::Disabled, QPalette::Text), 1.5));
        painter.setBrush(Qt::NoBrush);
    }

    painter.drawEllipse(rect);
    painter.end();

    return QIcon(pixmap);
}

/**
 * Returns the first color that isn't used yet, or cycles through the colors if all are used
 */
QString TagChipsWidget::nextUnusedColor(const QStringList &colors,
                                        const QList<QColor> &usedColors) {
    if (colors.isEmpty()) {
        return QString();
    }

    for (const auto &color : colors) {
        const QColor candidate(QStringLiteral("#") + color);
        bool isUsed = false;

        for (const auto &usedColor : usedColors) {
            if (usedColor.isValid() && usedColor.rgb() == candidate.rgb()) {
                isUsed = true;
                break;
            }
        }

        if (!isUsed) {
            return color;
        }
    }

    return colors.at(usedColors.count() % colors.count());
}

QString TagChipsWidget::nextNewTagColor() const {
    QList<QColor> usedColors;
    for (const auto &item : _items) {
        usedColors.append(item.color);
    }

    return nextUnusedColor(_newTagColors, usedColors);
}

int TagChipsWidget::itemIndex(int id) const {
    for (int i = 0; i < _items.count(); ++i) {
        if (_items.at(i).id == id) {
            return i;
        }
    }

    return -1;
}

int TagChipsWidget::itemIndexForName(const QString &name) const {
    for (int i = 0; i < _items.count(); ++i) {
        if (_items.at(i).name.compare(name, Qt::CaseInsensitive) == 0) {
            return i;
        }
    }

    return -1;
}

void TagChipsWidget::updateAddButton() {
    const bool isEmpty = _selectedIds.isEmpty();
    _addButton->setText(isEmpty ? tr("Add tag") : QString());
    _addButton->setToolButtonStyle(isEmpty ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonIconOnly);

    // The button is styled like a chip, with the same border width in every state, so it
    // doesn't move when a style sheet (like the dark mode one) only adds a border on hover
    const int height = rowHeight();
    const QColor text = palette().color(QPalette::WindowText);
    _addButton->setFixedHeight(height);
    if (isEmpty) {
        _addButton->setMinimumWidth(0);
        _addButton->setMaximumWidth(QWIDGETSIZE_MAX);
    } else {
        _addButton->setFixedWidth(height);
    }
    _addButton->setStyleSheet(
        QStringLiteral("QToolButton#addTagButton { color: %1; background-color: transparent;"
                       " border: 1px solid %2; border-radius: %3px; margin: 0px;"
                       " padding: 0px %4px; }"
                       "QToolButton#addTagButton:hover { background-color: %5;"
                       " border: 1px solid %6; }"
                       "QToolButton#addTagButton:pressed { background-color: %7;"
                       " border: 1px solid %6; }"
                       "QToolButton#addTagButton:focus { border: 1px solid %6; }"
                       "QToolButton#addTagButton:disabled { color: %8; }")
            .arg(text.name(), cssColor(withAlpha(text, 0.35)), QString::number(height / 2),
                 QString::number(isEmpty ? height / 2 - 2 : 0), cssColor(withAlpha(text, 0.08)),
                 cssColor(withAlpha(text, 0.45)), cssColor(withAlpha(text, 0.16)),
                 palette().color(QPalette::Disabled, QPalette::WindowText).name()));

    const QKeySequence shortcut = _addButton->shortcut();
    _addButton->setToolTip(
        shortcut.isEmpty() ? _addButtonToolTip
                           : QStringLiteral("%1 (%2)").arg(
                                 _addButtonToolTip, shortcut.toString(QKeySequence::NativeText)));
}

void TagChipsWidget::rebuildChips() {
    QLayoutItem *layoutItem;
    while ((layoutItem = _flowLayout->takeAt(0)) != nullptr) {
        QWidget *widget = layoutItem->widget();

        // Chips are deleted later, because the click of their remove button may still be handled
        if (widget != nullptr && widget != _addButton) {
            widget->setObjectName(QString());
            widget->hide();
            widget->deleteLater();
        }

        delete layoutItem;
    }

    const auto items = selectedItems();
    for (const auto &item : items) {
        _flowLayout->addWidget(createChip(item));
    }

    _flowLayout->addWidget(_addButton);
    updateAddButton();
    updateGeometry();
}

QWidget *TagChipsWidget::createChip(const Item &item) {
    const bool isPartial = _partiallySelectedIds.contains(item.id);
    const int height = rowHeight();
    const QColor text = palette().color(QPalette::WindowText);
    QColor background;
    QColor foreground;
    QColor border;

    if (item.color.isValid() && !isPartial) {
        background = item.color;
        foreground = textColorForBackground(background);
        border = background.darker(115);
    } else if (item.color.isValid()) {
        // Tags that are only on some of the tagged objects are shown faded with a dashed border
        background = item.color;
        background.setAlphaF(0.3);
        foreground = text;
        border = item.color;
    } else {
        // Neutral chips are translucent, so they fit any background color
        background = text;
        background.setAlphaF(isPartial ? 0.04 : 0.1);
        border = text;
        border.setAlphaF(isPartial ? 0.45 : 0.25);
        foreground = text;
    }

    // The hover color of the remove button depends on how dark the chip looks
    const bool isDarkChip = item.color.isValid() && !isPartial
                                ? textColorForBackground(background) == Qt::white
                                : text.lightness() > 128;

    auto *chip = new QFrame(this);
    chip->setObjectName(QStringLiteral("tagChip"));
    chip->setProperty("tagId", item.id);
    chip->setFixedHeight(height);
    chip->setStyleSheet(
        QStringLiteral("QFrame#tagChip { background-color: %1; border: 1px %7 %2;"
                       " border-radius: %3px; padding: 0px; }"
                       "QFrame#tagChip QLabel { color: %4; background: transparent; border: none;"
                       " padding: 0px; }"
                       "QFrame#tagChip QToolButton { color: %4; background: transparent;"
                       " border: none; border-radius: %5px; margin: 0px; padding: 0px;"
                       " font-weight: bold; }"
                       "QFrame#tagChip QToolButton:hover { background-color: %6; }")
            .arg(cssColor(background), cssColor(border), QString::number(height / 2),
                 foreground.name(), QString::number((height - 6) / 2),
                 isDarkChip ? QStringLiteral("rgba(255, 255, 255, 60)")
                            : QStringLiteral("rgba(0, 0, 0, 40)"),
                 isPartial ? QStringLiteral("dashed") : QStringLiteral("solid")));

    QStringList toolTipLines;
    toolTipLines.append(item.path());
    if (isPartial && !_partialSelectionToolTip.isEmpty()) {
        toolTipLines.append(_partialSelectionToolTip);
    }
    if (_chipsClickable && !_chipClickToolTip.isEmpty()) {
        toolTipLines.append(_chipClickToolTip.arg(item.name));
    }
    chip->setToolTip(toolTipLines.join(QLatin1Char('\n')));

    auto *layout = new QHBoxLayout(chip);
    layout->setContentsMargins(height / 2, 0, 3, 0);
    layout->setSpacing(2);

    auto *textLabel = new QLabel(item.name, chip);
    textLabel->setTextFormat(Qt::PlainText);
    layout->addWidget(textLabel);

    if (_chipsClickable) {
        chip->setCursor(Qt::PointingHandCursor);
        chip->installEventFilter(this);
        textLabel->installEventFilter(this);
    }

    auto *removeButton = new QToolButton(chip);
    removeButton->setObjectName(QStringLiteral("removeTagButton"));
    removeButton->setText(QStringLiteral("✕"));
    removeButton->setFixedSize(height - 6, height - 6);
    removeButton->setCursor(Qt::ArrowCursor);
    removeButton->setFocusPolicy(Qt::TabFocus);
    removeButton->setToolTip(_chipRemoveToolTip.arg(item.name));
    removeButton->setAccessibleName(removeButton->toolTip());
    const int id = item.id;
    connect(removeButton, &QToolButton::clicked, this, [this, id]() { removeItem(id); });
    layout->addWidget(removeButton);

    return chip;
}

void TagChipsWidget::removeItem(int id) {
    if (!_selectedIds.removeOne(id)) {
        return;
    }

    _partiallySelectedIds.removeOne(id);
    rebuildChips();
    updatePopupList();
    _addButton->setFocus();
    emit tagToggled(id, false);
    emit selectedIdsChanged();
}

/**
 * Toggles a tag, partially selected tags get selected for all objects
 */
void TagChipsWidget::toggleItem(int id) {
    if (itemIndex(id) < 0) {
        return;
    }

    bool selected;

    if (_partiallySelectedIds.contains(id)) {
        _partiallySelectedIds.removeOne(id);
        selected = true;
    } else if (_selectedIds.contains(id)) {
        _selectedIds.removeOne(id);
        selected = false;
    } else {
        _selectedIds.append(id);
        selected = true;
    }

    rebuildChips();
    updatePopupList();
    emit tagToggled(id, selected);
    emit selectedIdsChanged();
}

void TagChipsWidget::showPopup() {
    if (_popup) {
        _popup->close();
    }

    // Allows loading the available tags only when they are needed
    emit popupAboutToShow();

    auto *popup = new QFrame(this, Qt::Popup);
    popup->setObjectName(QStringLiteral("tagChipsPopup"));
    popup->setAttribute(Qt::WA_DeleteOnClose);
    popup->setFrameShape(QFrame::StyledPanel);
    popup->setFrameShadow(QFrame::Raised);
    popup->installEventFilter(this);
    // Keep a visible border, even if a style sheet of a parent removes the borders of frames.
    // The color buttons keep the same border width in every state, so they don't move on hover.
    const QColor popupText = palette().color(QPalette::WindowText);
    popup->setStyleSheet(
        QStringLiteral("QFrame#tagChipsPopup { border: 1px solid %1; background-color: %2; }"
                       "QToolButton#tagColorButton { background-color: transparent;"
                       " border: 1px solid transparent; border-radius: 4px; margin: 0px;"
                       " padding: 2px; }"
                       "QToolButton#tagColorButton:hover { background-color: %3; }"
                       "QToolButton#tagColorButton:checked { background-color: %4;"
                       " border: 1px solid %5; }")
            .arg(palette().color(QPalette::Mid).name(), palette().color(QPalette::Window).name(),
                 cssColor(withAlpha(popupText, 0.08)), cssColor(withAlpha(popupText, 0.14)),
                 palette().color(QPalette::Highlight).name()));
    _popup = popup;

    auto *layout = new QVBoxLayout(popup);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(6);

    auto *headerLabel = new QLabel(tr("Tags"), popup);
    QFont headerFont = headerLabel->font();
    headerFont.setBold(true);
    headerLabel->setFont(headerFont);
    layout->addWidget(headerLabel);

    auto *searchEdit = new QLineEdit(popup);
    searchEdit->setObjectName(QStringLiteral("tagSearchEdit"));
    searchEdit->setPlaceholderText(tr("Search or create tag…"));
    searchEdit->setClearButtonEnabled(true);
    searchEdit->addAction(
        QIcon::fromTheme(QStringLiteral("edit-find"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/edit-find.svg"))),
        QLineEdit::LeadingPosition);
    searchEdit->installEventFilter(this);
    _popupSearchEdit = searchEdit;
    layout->addWidget(searchEdit);

    auto *list = new QListWidget(popup);
    list->setObjectName(QStringLiteral("tagListWidget"));
    list->setIconSize(QSize(14, 14));
    list->setUniformItemSizes(true);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setTextElideMode(Qt::ElideMiddle);
    list->installEventFilter(this);
    _popupList = list;
    layout->addWidget(list);
    connect(list, &QListWidget::itemClicked, this, &TagChipsWidget::activatePopupItem);

    QPalette mutedPalette = popup->palette();
    mutedPalette.setColor(QPalette::WindowText,
                          mutedPalette.color(QPalette::Disabled, QPalette::WindowText));

    auto *emptyLabel = new QLabel(popup);
    emptyLabel->setWordWrap(true);
    emptyLabel->setAlignment(Qt::AlignCenter);
    emptyLabel->setPalette(mutedPalette);
    _popupEmptyLabel = emptyLabel;
    layout->addWidget(emptyLabel);

    // Color choice for new tags, only shown while a new tag can be created
    auto *colorFrame = new QWidget(popup);
    auto *colorLayout = new QHBoxLayout(colorFrame);
    colorLayout->setContentsMargins(0, 0, 0, 0);
    colorLayout->setSpacing(2);
    colorLayout->addWidget(new QLabel(tr("Color:"), colorFrame));
    auto *colorGroup = new QButtonGroup(colorFrame);
    colorGroup->setExclusive(true);
    _popupColorGroup = colorGroup;
    const QString selectedColor = nextNewTagColor();

    for (const auto &color : _newTagColors) {
        auto *colorButton = new QToolButton(colorFrame);
        colorButton->setObjectName(QStringLiteral("tagColorButton"));
        colorButton->setCheckable(true);
        colorButton->setAutoRaise(true);
        colorButton->setIcon(colorIcon(QColor(QStringLiteral("#") + color), 16));
        colorButton->setIconSize(QSize(16, 16));
        colorButton->setToolTip(tr("Use the color #%1 for the new tag").arg(color));
        colorButton->setProperty("tagColor", color);
        colorButton->setChecked(color == selectedColor);
        colorButton->setFocusPolicy(Qt::NoFocus);
        colorGroup->addButton(colorButton);
        colorLayout->addWidget(colorButton);
    }

    colorLayout->addStretch();
    _popupColorFrame = colorFrame;
    layout->addWidget(colorFrame);

    if (!_popupHintText.isEmpty()) {
        auto *hintLabel = new QLabel(_popupHintText, popup);
        hintLabel->setWordWrap(true);
        hintLabel->setPalette(mutedPalette);
        hintLabel->setFont(smallerFont(hintLabel->font()));
        layout->addWidget(hintLabel);
    }

    connect(searchEdit, &QLineEdit::textChanged, this, [this]() { updatePopupList(); });
    connect(searchEdit, &QLineEdit::returnPressed, this, &TagChipsWidget::activateFromSearch);

    // Show the popup below the add button and keep it on the screen
    popup->setFixedWidth(qMax(280, fontMetrics().averageCharWidth() * 36));
    updatePopupList();
    popup->adjustSize();
    QPoint position = _addButton->mapToGlobal(QPoint(0, _addButton->height() + 2));

    QScreen *screen = QGuiApplication::screenAt(position);
    if (screen != nullptr) {
        const QRect available = screen->availableGeometry();

        if (position.x() + popup->width() > available.right()) {
            position.setX(qMax(available.left(), available.right() - popup->width()));
        }

        if (position.y() + popup->height() > available.bottom()) {
            position.setY(_addButton->mapToGlobal(QPoint(0, 0)).y() - popup->height() - 2);
        }
    }

    popup->move(position);
    popup->show();
    searchEdit->setFocus();
}

void TagChipsWidget::updatePopupList() {
    if (!_popupList || !_popupSearchEdit) {
        return;
    }

    const QString searchText = _popupSearchEdit->text().trimmed();
    const int previousId = _popupList->currentItem() != nullptr
                               ? _popupList->currentItem()->data(Qt::UserRole).toInt()
                               : 0;

    _popupList->clear();
    QListWidgetItem *currentItem = nullptr;

    for (const auto &item : _items) {
        const QString path = item.path();

        if (!searchText.isEmpty() && !path.contains(searchText, Qt::CaseInsensitive)) {
            continue;
        }

        auto *listItem = new QListWidgetItem(colorIcon(item.color), path, _popupList);
        listItem->setData(Qt::UserRole, item.id);
        listItem->setToolTip(path);
        // Clicks are handled by the list, so a click anywhere in the row toggles the tag
        listItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        listItem->setCheckState(_partiallySelectedIds.contains(item.id) ? Qt::PartiallyChecked
                                : _selectedIds.contains(item.id)        ? Qt::Checked
                                                                        : Qt::Unchecked);

        if (item.id == previousId || (currentItem == nullptr && !searchText.isEmpty())) {
            currentItem = listItem;
        }
    }

    const bool canCreate = !searchText.isEmpty() && itemIndexForName(searchText) < 0;

    if (canCreate) {
        auto *listItem = new QListWidgetItem(
            QIcon::fromTheme(QStringLiteral("list-add"),
                             QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/list-add.svg"))),
            tr("Create tag \"%1\"").arg(searchText), _popupList);
        listItem->setData(Qt::UserRole, CreateTagItemId);
        listItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        QFont font = listItem->font();
        font.setItalic(true);
        listItem->setFont(font);

        if (currentItem == nullptr) {
            currentItem = listItem;
        }
    }

    if (currentItem != nullptr) {
        _popupList->setCurrentItem(currentItem);
    }

    const bool isEmpty = _popupList->count() == 0;

    // Fit the list to its rows, long lists get a scrollbar
    if (!isEmpty) {
        const int visibleRows = qMin(_popupList->count(), 10);
        _popupList->setFixedHeight(visibleRows * _popupList->sizeHintForRow(0) +
                                   2 * _popupList->frameWidth());
    }

    _popupList->setVisible(!isEmpty);
    _popupEmptyLabel->setVisible(isEmpty);
    _popupEmptyLabel->setText(_items.isEmpty()
                                  ? tr("There are no tags yet. Type a name to create one.")
                                  : tr("No matching tags"));
    _popupColorFrame->setVisible(canCreate && !_newTagColors.isEmpty());

    if (_popup) {
        _popup->adjustSize();
    }
}

void TagChipsWidget::activatePopupItem(QListWidgetItem *item) {
    if (item == nullptr) {
        return;
    }

    const int id = item->data(Qt::UserRole).toInt();

    if (id == CreateTagItemId) {
        requestTagCreation();
    } else {
        toggleItem(id);
    }
}

/**
 * Toggles the current tag of the popup list, or creates a new tag from the search text
 */
void TagChipsWidget::activateFromSearch() {
    if (!_popupSearchEdit || !_popupList) {
        return;
    }

    const QString searchText = _popupSearchEdit->text().trimmed();
    const int exactIndex = itemIndexForName(searchText);

    if (!searchText.isEmpty() && exactIndex >= 0) {
        toggleItem(_items.at(exactIndex).id);

        if (_popupSearchEdit) {
            _popupSearchEdit->clear();
        }
        return;
    }

    activatePopupItem(_popupList->currentItem());
}

void TagChipsWidget::requestTagCreation() {
    if (!_popupSearchEdit) {
        return;
    }

    const QString name = _popupSearchEdit->text().trimmed();

    if (name.isEmpty()) {
        return;
    }

    const int existingIndex = itemIndexForName(name);
    if (existingIndex >= 0) {
        const int id = _items.at(existingIndex).id;
        if (!_selectedIds.contains(id) || _partiallySelectedIds.contains(id)) {
            toggleItem(id);
        }
        return;
    }

    QString color = nextNewTagColor();
    if (_popupColorGroup && _popupColorGroup->checkedButton() != nullptr) {
        color = _popupColorGroup->checkedButton()->property("tagColor").toString();
    }

    emit createTagRequested(name, color);

    // The request may have opened a message box that closed the popup
    if (_popupSearchEdit) {
        if (itemIndexForName(name) >= 0) {
            _popupSearchEdit->clear();
        }

        _popupSearchEdit->setFocus();
    }
}

bool TagChipsWidget::eventFilter(QObject *obj, QEvent *event) {
    // Return the focus to the tags after the popup was closed
    if (obj == _popup && event->type() == QEvent::Hide && isVisible() && isEnabled()) {
        _addButton->setFocus();
    }

    // Clicking a chip (but not its remove button)
    if (event->type() == QEvent::MouseButtonRelease && _chipsClickable && obj != _popup) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        auto *widget = qobject_cast<QWidget *>(obj);

        if (mouseEvent->button() == Qt::LeftButton && widget != nullptr &&
            widget->rect().contains(mouseEvent->pos())) {
            QWidget *chip =
                widget->objectName() == QStringLiteral("tagChip") ? widget : widget->parentWidget();

            if (chip != nullptr && chip->property("tagId").isValid()) {
                emit tagClicked(chip->property("tagId").toInt());
                return true;
            }
        }
    }

    if (event->type() == QEvent::KeyPress && _popup) {
        auto *keyEvent = static_cast<QKeyEvent *>(event);
        const int key = keyEvent->key();

        if (key == Qt::Key_Escape) {
            _popup->close();
            return true;
        }

        if (obj == _popupSearchEdit && _popupList && _popupList->count() > 0) {
            if (key == Qt::Key_Down || key == Qt::Key_PageDown) {
                _popupList->setFocus();
                if (_popupList->currentItem() == nullptr) {
                    _popupList->setCurrentRow(0);
                }
                return true;
            }

            // Tab adds the current tag, like the tag name completion did before
            if (key == Qt::Key_Tab && !_popupSearchEdit->text().trimmed().isEmpty()) {
                activateFromSearch();
                return true;
            }
        } else if (obj == _popupList) {
            if (key == Qt::Key_Return || key == Qt::Key_Enter || key == Qt::Key_Space) {
                activatePopupItem(_popupList->currentItem());
                return true;
            }

            if (key == Qt::Key_Up && _popupList->currentRow() <= 0 && _popupSearchEdit) {
                _popupSearchEdit->setFocus();
                return true;
            }

            // Continue typing in the search field
            if (!keyEvent->text().isEmpty() && keyEvent->text().at(0).isPrint() &&
                _popupSearchEdit) {
                _popupSearchEdit->setFocus();
                _popupSearchEdit->insert(keyEvent->text());
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}

void TagChipsWidget::changeEvent(QEvent *event) {
    QWidget::changeEvent(event);

    if (_flowLayout == nullptr || _addButton == nullptr) {
        return;
    }

    if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange ||
        event->type() == QEvent::PaletteChange) {
        rebuildChips();
    }

    if (event->type() == QEvent::EnabledChange && !isEnabled() && _popup) {
        _popup->close();
    }
}
