#include "nextclouddecklabelswidget.h"

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
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QSignalBlocker>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>
#include <algorithm>

#include "helpers/flowlayout.h"

namespace {
// Marks the list item that creates a new tag instead of toggling an existing one
const int CreateLabelItemId = -1;

int textWidth(const QFontMetrics &metrics, const QString &text) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    return metrics.horizontalAdvance(text);
#else
    return metrics.width(text);
#endif
}

qreal applicationDevicePixelRatio() { return qApp != nullptr ? qApp->devicePixelRatio() : 1.0; }
}    // namespace

NextcloudDeckLabelsWidget::NextcloudDeckLabelsWidget(QWidget *parent) : QWidget(parent) {
    _flowLayout = new FlowLayout(this, 0, 4, 4);

    _addButton = new QToolButton(this);
    _addButton->setObjectName(QStringLiteral("addLabelButton"));
    _addButton->setText(tr("Add tag"));
    _addButton->setIcon(
        QIcon::fromTheme(QStringLiteral("list-add"),
                         QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/list-add.svg"))));
    _addButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    _addButton->setAutoRaise(true);
    _addButton->setShortcut(QKeySequence(QStringLiteral("Ctrl+T")));
    _addButton->setToolTip(tr("Add or remove tags of the card, or create new tags (%1)")
                               .arg(_addButton->shortcut().toString(QKeySequence::NativeText)));
    connect(_addButton, &QToolButton::clicked, this, &NextcloudDeckLabelsWidget::showLabelPopup);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    rebuildChips();
}

void NextcloudDeckLabelsWidget::setAvailableLabels(const QList<Label> &labels) {
    _availableLabels = labels;

    // Sort tags alphabetically, like the tag picker in the Deck web interface
    std::sort(_availableLabels.begin(), _availableLabels.end(), [](const Label &a, const Label &b) {
        return QString::localeAwareCompare(a.title, b.title) < 0;
    });

    // Drop selected tags that don't exist in the board
    QList<int> selectedLabelIds;
    for (const int labelId : _selectedLabelIds) {
        if (labelIndex(labelId) >= 0) {
            selectedLabelIds.append(labelId);
        }
    }

    const bool selectionChanged = selectedLabelIds != _selectedLabelIds;
    _selectedLabelIds = selectedLabelIds;
    rebuildChips();
    updatePopupList();

    if (selectionChanged) {
        emit selectedLabelsChanged();
    }
}

QList<NextcloudDeckLabelsWidget::Label> NextcloudDeckLabelsWidget::availableLabels() const {
    return _availableLabels;
}

void NextcloudDeckLabelsWidget::addAvailableLabel(const Label &label, bool select) {
    if (label.id < 1) {
        return;
    }

    QList<Label> labels = _availableLabels;
    const int index = labelIndex(label.id);

    if (index >= 0) {
        labels[index] = label;
    } else {
        labels.append(label);
    }

    if (select && !_selectedLabelIds.contains(label.id)) {
        _selectedLabelIds.append(label.id);
    }

    // Start with a fresh search after a tag was created from the popup
    if (_popupSearchEdit) {
        const QSignalBlocker blocker(_popupSearchEdit);
        _popupSearchEdit->clear();
    }

    setAvailableLabels(labels);

    if (select) {
        emit selectedLabelsChanged();
    }
}

void NextcloudDeckLabelsWidget::setSelectedLabelIds(const QList<int> &labelIds) {
    QList<int> selectedLabelIds;
    for (const int labelId : labelIds) {
        if (labelIndex(labelId) >= 0 && !selectedLabelIds.contains(labelId)) {
            selectedLabelIds.append(labelId);
        }
    }

    if (selectedLabelIds == _selectedLabelIds) {
        return;
    }

    _selectedLabelIds = selectedLabelIds;
    rebuildChips();
    updatePopupList();
    emit selectedLabelsChanged();
}

QList<int> NextcloudDeckLabelsWidget::selectedLabelIds() const { return _selectedLabelIds; }

/**
 * Returns the selected tags in the order of the available tags
 */
QList<NextcloudDeckLabelsWidget::Label> NextcloudDeckLabelsWidget::selectedLabels() const {
    QList<Label> labels;
    for (const auto &label : _availableLabels) {
        if (_selectedLabelIds.contains(label.id)) {
            labels.append(label);
        }
    }

    return labels;
}

bool NextcloudDeckLabelsWidget::hasSameSelection(const QList<int> &labelIds) const {
    if (labelIds.count() != _selectedLabelIds.count()) {
        return false;
    }

    for (const int labelId : labelIds) {
        if (!_selectedLabelIds.contains(labelId)) {
            return false;
        }
    }

    return true;
}

bool NextcloudDeckLabelsWidget::isLabelPopupVisible() const {
    return _popup != nullptr && _popup->isVisible();
}

/**
 * Height of the chips, they are as high as the add button to line up in the flow layout
 */
int NextcloudDeckLabelsWidget::rowHeight() const {
    return qMax(_addButton->sizeHint().height(), fontMetrics().height() + 6);
}

QColor NextcloudDeckLabelsWidget::labelColor(const Label &label) {
    const QColor color(QStringLiteral("#") + label.color);
    return color.isValid() ? color : QColor(0x99, 0x99, 0x99);
}

/**
 * Returns a readable text color for the background color, based on its perceived brightness
 */
QColor NextcloudDeckLabelsWidget::textColorForBackground(const QColor &color) {
    const int brightness = (color.red() * 299 + color.green() * 587 + color.blue() * 114) / 1000;
    return brightness > 150 ? QColor(0x22, 0x22, 0x22) : QColor(Qt::white);
}

QIcon NextcloudDeckLabelsWidget::colorIcon(const QColor &color, int size) {
    const qreal ratio = applicationDevicePixelRatio();
    QPixmap pixmap(QSize(size, size) * ratio);
    pixmap.setDevicePixelRatio(ratio);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(color.darker(130), 1));
    painter.setBrush(color);
    painter.drawEllipse(QRectF(0.5, 0.5, size - 1, size - 1));
    painter.end();

    return QIcon(pixmap);
}

int NextcloudDeckLabelsWidget::labelIndex(int labelId) const {
    for (int i = 0; i < _availableLabels.count(); ++i) {
        if (_availableLabels.at(i).id == labelId) {
            return i;
        }
    }

    return -1;
}

int NextcloudDeckLabelsWidget::labelIndexForTitle(const QString &title) const {
    for (int i = 0; i < _availableLabels.count(); ++i) {
        if (_availableLabels.at(i).title.compare(title, Qt::CaseInsensitive) == 0) {
            return i;
        }
    }

    return -1;
}

void NextcloudDeckLabelsWidget::rebuildChips() {
    QLayoutItem *item;
    while ((item = _flowLayout->takeAt(0)) != nullptr) {
        if (item->widget() != _addButton) {
            delete item->widget();
        }
        delete item;
    }

    const auto labels = selectedLabels();
    for (const auto &label : labels) {
        _flowLayout->addWidget(createChip(label));
    }

    _flowLayout->addWidget(_addButton);
    _addButton->setText(labels.isEmpty() ? tr("Add tag") : QString());
    _addButton->setToolButtonStyle(labels.isEmpty() ? Qt::ToolButtonTextBesideIcon
                                                    : Qt::ToolButtonIconOnly);
    updateGeometry();
}

QWidget *NextcloudDeckLabelsWidget::createChip(const Label &label) {
    const QColor background = labelColor(label);
    const QColor foreground = textColorForBackground(background);
    const int height = rowHeight();

    auto *chip = new QFrame(this);
    chip->setObjectName(QStringLiteral("deckLabelChip"));
    chip->setProperty("labelId", label.id);
    chip->setFixedHeight(height);
    chip->setToolTip(tr("Tag: %1").arg(label.title));
    chip->setStyleSheet(
        QStringLiteral("QFrame#deckLabelChip { background-color: %1; border: 1px solid %2;"
                       " border-radius: %3px; }"
                       "QFrame#deckLabelChip QLabel { color: %4; background: transparent; }"
                       "QFrame#deckLabelChip QToolButton { color: %4; background: transparent;"
                       " border: none; border-radius: %5px; padding: 0px; font-weight: bold; }"
                       "QFrame#deckLabelChip QToolButton:hover { background-color: %6; }")
            .arg(background.name(), background.darker(115).name(), QString::number(height / 2),
                 foreground.name(), QString::number((height - 6) / 2),
                 foreground == Qt::white ? QStringLiteral("rgba(255, 255, 255, 60)")
                                         : QStringLiteral("rgba(0, 0, 0, 40)")));

    auto *layout = new QHBoxLayout(chip);
    layout->setContentsMargins(height / 2, 0, 3, 0);
    layout->setSpacing(2);

    auto *textLabel = new QLabel(label.title, chip);
    textLabel->setTextFormat(Qt::PlainText);
    layout->addWidget(textLabel);

    auto *removeButton = new QToolButton(chip);
    removeButton->setObjectName(QStringLiteral("removeLabelButton"));
    removeButton->setText(QStringLiteral("✕"));
    removeButton->setFixedSize(height - 6, height - 6);
    removeButton->setCursor(Qt::PointingHandCursor);
    removeButton->setFocusPolicy(Qt::TabFocus);
    removeButton->setToolTip(tr("Remove tag \"%1\"").arg(label.title));
    removeButton->setAccessibleName(removeButton->toolTip());
    const int labelId = label.id;
    connect(removeButton, &QToolButton::clicked, this, [this, labelId]() { removeLabel(labelId); });
    layout->addWidget(removeButton);

    return chip;
}

void NextcloudDeckLabelsWidget::removeLabel(int labelId) {
    if (!_selectedLabelIds.removeOne(labelId)) {
        return;
    }

    // The chip with the clicked button must not be deleted while its signal is being handled
    QTimer::singleShot(0, this, [this]() {
        rebuildChips();
        updatePopupList();
        _addButton->setFocus();
    });
    emit selectedLabelsChanged();
}

void NextcloudDeckLabelsWidget::toggleLabel(int labelId) {
    if (_selectedLabelIds.contains(labelId)) {
        _selectedLabelIds.removeOne(labelId);
    } else if (labelIndex(labelId) >= 0) {
        _selectedLabelIds.append(labelId);
    } else {
        return;
    }

    rebuildChips();
    updatePopupList();
    emit selectedLabelsChanged();
}

/**
 * Picks a color for a new tag that isn't used in the board yet, if possible
 */
QString NextcloudDeckLabelsWidget::nextLabelColor() const {
    const QStringList colors = NextcloudDeckService::defaultLabelColors();

    for (const auto &color : colors) {
        bool isUsed = false;
        for (const auto &label : _availableLabels) {
            if (label.color.compare(color, Qt::CaseInsensitive) == 0) {
                isUsed = true;
                break;
            }
        }

        if (!isUsed) {
            return color;
        }
    }

    return colors.at(_availableLabels.count() % colors.count());
}

void NextcloudDeckLabelsWidget::showLabelPopup() {
    if (_popup) {
        _popup->close();
    }

    auto *popup = new QFrame(this, Qt::Popup);
    popup->setObjectName(QStringLiteral("deckLabelPopup"));
    popup->setAttribute(Qt::WA_DeleteOnClose);
    popup->setFrameShape(QFrame::StyledPanel);
    popup->setFrameShadow(QFrame::Raised);
    popup->installEventFilter(this);
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
    searchEdit->setObjectName(QStringLiteral("labelSearchEdit"));
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
    list->setObjectName(QStringLiteral("labelListWidget"));
    list->setIconSize(QSize(14, 14));
    list->setUniformItemSizes(true);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->installEventFilter(this);
    _popupList = list;
    layout->addWidget(list);
    connect(list, &QListWidget::itemClicked, this, &NextcloudDeckLabelsWidget::activatePopupItem);

    auto *emptyLabel = new QLabel(popup);
    emptyLabel->setWordWrap(true);
    emptyLabel->setAlignment(Qt::AlignCenter);
    QPalette emptyPalette = emptyLabel->palette();
    emptyPalette.setColor(QPalette::WindowText,
                          emptyPalette.color(QPalette::Disabled, QPalette::WindowText));
    emptyLabel->setPalette(emptyPalette);
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
    const QString selectedColor = nextLabelColor();

    for (const auto &color : NextcloudDeckService::defaultLabelColors()) {
        auto *colorButton = new QToolButton(colorFrame);
        colorButton->setCheckable(true);
        colorButton->setAutoRaise(true);
        colorButton->setIcon(colorIcon(QColor(QStringLiteral("#") + color), 16));
        colorButton->setIconSize(QSize(16, 16));
        colorButton->setToolTip(tr("Use the color #%1 for the new tag").arg(color));
        colorButton->setProperty("labelColor", color);
        colorButton->setChecked(color == selectedColor);
        colorButton->setFocusPolicy(Qt::NoFocus);
        colorGroup->addButton(colorButton);
        colorLayout->addWidget(colorButton);
    }

    colorLayout->addStretch();
    _popupColorFrame = colorFrame;
    layout->addWidget(colorFrame);

    auto *hintLabel =
        new QLabel(tr("New tags are created right away, the tags of the card are stored when "
                      "the card is saved."),
                   popup);
    hintLabel->setWordWrap(true);
    hintLabel->setPalette(emptyPalette);
    hintLabel->setFont(NextcloudDeckLabelDelegate::pillFont(hintLabel->font()));
    layout->addWidget(hintLabel);

    connect(searchEdit, &QLineEdit::textChanged, this, [this]() { updatePopupList(); });
    connect(searchEdit, &QLineEdit::returnPressed, this,
            &NextcloudDeckLabelsWidget::activateFromSearch);
    // Show the popup below the add button and keep it on the screen
    popup->setFixedWidth(qMax(280, fontMetrics().averageCharWidth() * 36));
    updatePopupList();
    popup->adjustSize();
    QPoint position = _addButton->mapToGlobal(QPoint(0, _addButton->height() + 2));

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    QScreen *screen = QGuiApplication::screenAt(position);
#else
    QScreen *screen = QGuiApplication::primaryScreen();
#endif
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

void NextcloudDeckLabelsWidget::updatePopupList() {
    if (!_popupList || !_popupSearchEdit) {
        return;
    }

    const QString searchText = _popupSearchEdit->text().trimmed();
    const int previousId = _popupList->currentItem() != nullptr
                               ? _popupList->currentItem()->data(Qt::UserRole).toInt()
                               : 0;

    _popupList->clear();
    QListWidgetItem *currentItem = nullptr;

    for (const auto &label : _availableLabels) {
        if (!searchText.isEmpty() && !label.title.contains(searchText, Qt::CaseInsensitive)) {
            continue;
        }

        auto *item = new QListWidgetItem(colorIcon(labelColor(label)), label.title, _popupList);
        item->setData(Qt::UserRole, label.id);
        // Clicks are handled by the list, so a click anywhere in the row toggles the tag
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        item->setCheckState(_selectedLabelIds.contains(label.id) ? Qt::Checked : Qt::Unchecked);

        if (label.id == previousId || (currentItem == nullptr && !searchText.isEmpty())) {
            currentItem = item;
        }
    }

    const bool canCreate = !searchText.isEmpty() && labelIndexForTitle(searchText) < 0;

    if (canCreate) {
        auto *item = new QListWidgetItem(
            QIcon::fromTheme(QStringLiteral("list-add"),
                             QIcon(QStringLiteral(":icons/breeze-qownnotes/16x16/list-add.svg"))),
            tr("Create tag \"%1\"").arg(searchText), _popupList);
        item->setData(Qt::UserRole, CreateLabelItemId);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        QFont font = item->font();
        font.setItalic(true);
        item->setFont(font);

        if (currentItem == nullptr) {
            currentItem = item;
        }
    }

    if (currentItem != nullptr) {
        _popupList->setCurrentItem(currentItem);
    }

    const bool isEmpty = _popupList->count() == 0;

    // Fit the list to its rows, long lists get a scrollbar
    if (!isEmpty) {
        const int visibleRows = qMin(_popupList->count(), 8);
        _popupList->setFixedHeight(visibleRows * _popupList->sizeHintForRow(0) +
                                   2 * _popupList->frameWidth());
    }

    _popupList->setVisible(!isEmpty);
    _popupEmptyLabel->setVisible(isEmpty);
    _popupEmptyLabel->setText(
        _availableLabels.isEmpty()
            ? tr("This board has no tags yet. Type a name to create the first one.")
            : tr("No matching tags"));
    _popupColorFrame->setVisible(canCreate);

    if (_popup) {
        _popup->adjustSize();
    }
}

void NextcloudDeckLabelsWidget::activatePopupItem(QListWidgetItem *item) {
    if (item == nullptr) {
        return;
    }

    const int labelId = item->data(Qt::UserRole).toInt();

    if (labelId == CreateLabelItemId) {
        requestLabelCreation();
    } else {
        toggleLabel(labelId);
    }
}

/**
 * Toggles the current tag of the popup list, or creates a new tag from the search text
 */
void NextcloudDeckLabelsWidget::activateFromSearch() {
    if (!_popupSearchEdit || !_popupList) {
        return;
    }

    const QString searchText = _popupSearchEdit->text().trimmed();
    const int exactIndex = labelIndexForTitle(searchText);

    if (!searchText.isEmpty() && exactIndex >= 0) {
        toggleLabel(_availableLabels.at(exactIndex).id);
        _popupSearchEdit->clear();
        return;
    }

    activatePopupItem(_popupList->currentItem());
}

void NextcloudDeckLabelsWidget::requestLabelCreation() {
    if (!_popupSearchEdit) {
        return;
    }

    const QString title = _popupSearchEdit->text().trimmed();

    if (title.isEmpty()) {
        return;
    }

    const int existingIndex = labelIndexForTitle(title);
    if (existingIndex >= 0) {
        const int labelId = _availableLabels.at(existingIndex).id;
        if (!_selectedLabelIds.contains(labelId)) {
            toggleLabel(labelId);
        }
        return;
    }

    QString color = nextLabelColor();
    if (_popupColorGroup && _popupColorGroup->checkedButton() != nullptr) {
        color = _popupColorGroup->checkedButton()->property("labelColor").toString();
    }

    emit createLabelRequested(title, color);

    // The request may have opened a message box that closed the popup
    if (_popupSearchEdit) {
        _popupSearchEdit->setFocus();
    }
}

bool NextcloudDeckLabelsWidget::eventFilter(QObject *obj, QEvent *event) {
    // Return the focus to the tags after the popup was closed
    if (obj == _popup && event->type() == QEvent::Hide && isVisible() && isEnabled()) {
        _addButton->setFocus();
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

void NextcloudDeckLabelsWidget::changeEvent(QEvent *event) {
    QWidget::changeEvent(event);

    if (_flowLayout == nullptr || _addButton == nullptr) {
        return;
    }

    if (event->type() == QEvent::FontChange || event->type() == QEvent::StyleChange) {
        rebuildChips();
    }

    if (event->type() == QEvent::EnabledChange && !isEnabled() && _popup) {
        _popup->close();
    }
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
        const QColor background = NextcloudDeckLabelsWidget::labelColor(
            {0, title, label.value(QStringLiteral("color")).toString()});
        const int available = right - x;
        const int remaining = labels.count() - i;
        const QString moreText = QStringLiteral("+%1").arg(remaining);
        const int minimumWidth = 2 * horizontalPadding + textWidth(metrics, QStringLiteral("…"));

        if (available < minimumWidth) {
            // Show how many tags didn't fit, if there is room for it
            if (available >= textWidth(metrics, moreText)) {
                painter->setPen(opt.palette.color(QPalette::Disabled, QPalette::Text));
                painter->drawText(QRect(x, top, available, pillHeight),
                                  Qt::AlignLeft | Qt::AlignVCenter, moreText);
            }
            break;
        }

        int pillWidth = textWidth(metrics, title) + 2 * horizontalPadding;
        QString text = title;

        if (pillWidth > available) {
            text = metrics.elidedText(title, Qt::ElideRight, available - 2 * horizontalPadding);
            pillWidth = available;
        }

        const QRectF pillRect(x + 0.5, top + 0.5, pillWidth - 1, pillHeight - 1);
        painter->setPen(QPen(background.darker(115), 1));
        painter->setBrush(background);
        painter->drawRoundedRect(pillRect, pillHeight / 2.0, pillHeight / 2.0);
        painter->setPen(NextcloudDeckLabelsWidget::textColorForBackground(background));
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
            textWidth(metrics, value.toMap().value(QStringLiteral("title")).toString()) + 2 * 7 + 4;
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
