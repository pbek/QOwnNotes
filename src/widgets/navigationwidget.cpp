/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include "navigationwidget.h"

#include <libraries/qmarkdowntextedit/markdownhighlighter.h>
#include <services/scriptingservice.h>

#include <QDebug>
#include <QMenu>
#include <QMouseEvent>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTreeWidgetItem>
#include <utility>

namespace {
struct HeadingMatch {
    HeadingMatch() = default;
    HeadingMatch(QString text, int elementType) : text(std::move(text)), elementType(elementType) {}

    QString text;
    int elementType = MarkdownHighlighter::NoState;

    bool isValid() const { return MarkdownHighlighter::isHeading(elementType) && !text.isEmpty(); }
};

bool isCodeBlockState(const int state) {
    return MarkdownHighlighter::isCodeBlock(state) || MarkdownHighlighter::isCodeBlockEnd(state) ||
           state == MarkdownHighlighter::CodeBlockIndented;
}

bool isYamlFrontmatterDelimiter(const QString &text) { return text == QLatin1String("---"); }

bool isLeadingYamlFrontmatterBlock(const QTextBlock &block) {
    if (!block.isValid() || block.document() == nullptr) {
        return false;
    }

    QTextBlock currentBlock = block.document()->firstBlock();
    if (!isYamlFrontmatterDelimiter(currentBlock.text())) {
        return false;
    }

    while (currentBlock.isValid()) {
        if (currentBlock == block) {
            return true;
        }

        if (currentBlock.blockNumber() > 0 && isYamlFrontmatterDelimiter(currentBlock.text())) {
            return false;
        }

        currentBlock = currentBlock.next();
    }

    return false;
}

int atxHeadingLevel(const QString &text) {
    int offset = 0;
    while (offset < text.length() && text.at(offset) == QLatin1Char(' ') && offset < 4) {
        ++offset;
    }

    if (offset >= text.length() || offset == 4 || text.at(offset) != QLatin1Char('#')) {
        return 0;
    }

    int headingLevel = 0;
    int i = offset;
    while (i < text.length() && text.at(i) == QLatin1Char('#') && headingLevel < 6) {
        ++i;
        ++headingLevel;
    }

    return (headingLevel > 0 && i < text.length() && text.at(i) == QLatin1Char(' ')) ? headingLevel
                                                                                     : 0;
}

bool isSetextUnderlineText(const QString &text) {
    int offset = 0;
    while (offset < text.length() && text.at(offset) == QLatin1Char(' ') && offset < 4) {
        ++offset;
    }

    if (offset >= text.length() || offset == 4) {
        return false;
    }

    const QChar marker = text.at(offset);
    if (marker != QLatin1Char('=') && marker != QLatin1Char('-')) {
        return false;
    }

    for (int i = offset; i < text.length(); ++i) {
        if (text.at(i) != marker) {
            return false;
        }
    }

    return true;
}

QString extractScriptingHeadingText(
    const QString &text, const QOwnNotesMarkdownHighlighter::ScriptingHighlightingRule &rule,
    const QRegularExpressionMatch &match) {
    if (rule.capturingGroup > 0) {
        const QString capturedText = match.captured(rule.capturingGroup).trimmed();
        if (!capturedText.isEmpty()) {
            return capturedText;
        }
    }

    for (int group = 1; group <= match.lastCapturedIndex(); ++group) {
        const QString capturedText = match.captured(group).trimmed();
        if (!capturedText.isEmpty()) {
            return capturedText;
        }
    }

    QString headingText = text.trimmed();
    const QString marker = rule.shouldContain.trimmed();

    if (!marker.isEmpty() && headingText.startsWith(marker) && headingText.endsWith(marker) &&
        headingText.length() >= marker.length() * 2) {
        headingText = headingText.mid(marker.length(), headingText.length() - (marker.length() * 2))
                          .trimmed();
    }

    return headingText;
}

HeadingMatch parseScriptingHeadingBlock(
    const QTextBlock &block,
    const QVector<QOwnNotesMarkdownHighlighter::ScriptingHighlightingRule> &rules) {
    const QString text = block.text();
    if (text.trimmed().isEmpty()) {
        return {};
    }

    for (const auto &rule : rules) {
        if (!MarkdownHighlighter::isHeading(rule.state) || !text.contains(rule.shouldContain)) {
            continue;
        }

        auto iterator = rule.pattern.globalMatch(text);
        while (iterator.hasNext()) {
            const QRegularExpressionMatch match = iterator.next();
            if (match.capturedStart(0) != 0 || match.capturedLength(0) != text.length()) {
                continue;
            }

            const QString headingText = extractScriptingHeadingText(text, rule, match);
            if (headingText.isEmpty()) {
                continue;
            }

            return {headingText, rule.state};
        }
    }

    return {};
}

HeadingMatch parseHeadingBlock(
    const QTextBlock &block,
    const QVector<QOwnNotesMarkdownHighlighter::ScriptingHighlightingRule> &rules) {
    if (!block.isValid()) {
        return {};
    }

    if (isCodeBlockState(block.userState())) {
        return {};
    }

    if (isLeadingYamlFrontmatterBlock(block)) {
        return {};
    }

    const QString text = block.text();
    const int atxLevel = atxHeadingLevel(text);
    if (atxLevel > 0) {
        return {text, MarkdownHighlighter::H1 + atxLevel - 1};
    }

    if (!text.trimmed().isEmpty() && isSetextUnderlineText(block.next().text())) {
        const QString nextText = block.next().text().trimmed();
        if (!nextText.isEmpty()) {
            const auto state = nextText.at(0) == QLatin1Char('=') ? MarkdownHighlighter::H1
                                                                  : MarkdownHighlighter::H2;
            return {text.trimmed(), state};
        }
    }

    HeadingMatch heading = parseScriptingHeadingBlock(block, rules);
    if (heading.isValid()) {
        return heading;
    }

    const int elementType = block.userState();
    return MarkdownHighlighter::isHeading(elementType)
               ? HeadingMatch{block.text().trimmed(), elementType}
               : HeadingMatch{};
}

QVector<QOwnNotesMarkdownHighlighter::ScriptingHighlightingRule> navigationScriptingRules() {
    const ScriptingService *scriptingService = ScriptingService::instanceOrNull();
    if (scriptingService == nullptr || !scriptingService->hasHighlightingRules()) {
        return {};
    }

    return scriptingService->getHighlightingRules();
}

void setExpandedRecursively(QTreeWidgetItem *item, bool expanded) {
    if (item == nullptr) {
        return;
    }

    if (!expanded) {
        for (int i = 0; i < item->childCount(); ++i) {
            setExpandedRecursively(item->child(i), false);
        }
    }

    item->setExpanded(expanded);

    if (expanded) {
        for (int i = 0; i < item->childCount(); ++i) {
            setExpandedRecursively(item->child(i), true);
        }
    }
}
}    // namespace

bool NavigationWidget::isSetextUnderlineBlock(const QString &text) {
    int offset = 0;
    while (offset < text.length() && text.at(offset) == QLatin1Char(' ') && offset < 4) {
        ++offset;
    }

    if (offset >= text.length() || offset == 4) {
        return false;
    }

    const QChar marker = text.at(offset);
    if (marker != QLatin1Char('=') && marker != QLatin1Char('-')) {
        return false;
    }

    for (int i = offset; i < text.length(); ++i) {
        if (text.at(i) != marker) {
            return false;
        }
    }

    return true;
}

bool NavigationWidget::isAtxHeadingBlock(const QString &text) { return atxHeadingLevel(text) > 0; }

bool NavigationWidget::isNavigationHeadingBlock(const QTextBlock &block) {
    const auto rules = navigationScriptingRules();
    return parseHeadingBlock(block, rules).isValid();
}

NavigationWidget::NavigationWidget(QWidget *parent) : QTreeWidget(parent) {
    // We want to handle currentItemChanged because it also works with the keyboard
    QObject::connect(this, &NavigationWidget::currentItemChanged, this,
                     &NavigationWidget::onCurrentItemChanged);
    // We want to handle itemClicked because it allows to click on an item a 2nd time
    QObject::connect(this, &NavigationWidget::itemClicked, this, &NavigationWidget::onItemClicked);
    // We want to handle itemChanged to allow renaming headings
    QObject::connect(this, &NavigationWidget::itemChanged, this, &NavigationWidget::onItemChanged);

    // Track expand/collapse changes to remember the state per note
    QObject::connect(this, &QTreeWidget::itemExpanded, this,
                     &NavigationWidget::onItemExpandedOrCollapsed);
    QObject::connect(this, &QTreeWidget::itemCollapsed, this,
                     &NavigationWidget::onItemExpandedOrCollapsed);

    // Enable context menu
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeWidget::customContextMenuRequested, this,
            &NavigationWidget::showContextMenu);
}

void NavigationWidget::mousePressEvent(QMouseEvent *event) {
    if ((event != nullptr) && (event->button() == Qt::LeftButton) &&
        event->modifiers().testFlag(Qt::ShiftModifier)) {
        QTreeWidgetItem *item = itemAt(event->pos());

        if ((item != nullptr) && (item->childCount() > 0)) {
            setExpandedRecursively(item, !item->isExpanded());
            setCurrentItem(item);
            event->accept();
            return;
        }
    }

    QTreeWidget::mousePressEvent(event);
}

/**
 * Emits the positionClicked signal to jump to the changed navigation item's
 * position
 */
void NavigationWidget::onCurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    if (current == nullptr) {
        return;
    }

    emit positionClicked(current->data(0, Qt::UserRole).toInt());
}

/**
 * Emits the positionClicked signal to jump to the clicked navigation item's
 * position
 */
void NavigationWidget::onItemClicked(QTreeWidgetItem *current, int column) {
    Q_UNUSED(column)

    if (current == nullptr) {
        return;
    }

    emit positionClicked(current->data(0, Qt::UserRole).toInt());
}

/**
 * Handles renaming of heading items
 * Emits the headingRenamed signal when a heading is renamed
 */
void NavigationWidget::onItemChanged(QTreeWidgetItem *item, int column) {
    Q_UNUSED(column)

    if (item == nullptr) {
        return;
    }

    // Get the position from the item
    int position = item->data(0, Qt::UserRole).toInt();

    // Find the corresponding node in our cache
    for (const auto &node : _navigationTreeNodes) {
        if (node.pos == position) {
            QString oldText = node.text;
            QString newText = item->text(0);

            // Only emit if the text actually changed
            if (oldText != newText && !newText.isEmpty()) {
                emit headingRenamed(position, oldText, newText);
            }
            break;
        }
    }
}

/**
 * Parses a text document and builds the navigation tree for it
 */
void NavigationWidget::parse(const QTextDocument *document, int textCursorPosition, int noteId) {
    const QSignalBlocker blocker(this);
    Q_UNUSED(blocker)

    // Save expand state for the previous note before switching
    if (_currentNoteId >= 0 && _currentNoteId != noteId &&
        _collapsedHeadingsPerNote.contains(_currentNoteId)) {
        saveExpandState();
    }

    _doc = document;
    _cursorPosition = textCursorPosition;
    _currentNoteId = noteId;
    doParse();
}

void NavigationWidget::doParse() {
    const auto nodes = parseDocument(_doc);
    buildNavTree(nodes);
}

QVector<Node> NavigationWidget::parseDocument(const QTextDocument *const document) {
    QVector<Node> nodes;
    const auto rules = navigationScriptingRules();

    for (int i = 0; i < document->blockCount(); ++i) {
        const QTextBlock block = document->findBlockByNumber(i);
        const HeadingMatch heading = parseHeadingBlock(block, rules);
        if (!heading.isValid()) {
            continue;
        }
        static const QRegularExpression re(QStringLiteral("^#+\\s+"));
        QString text = heading.text;
        text.remove(re);
        text = text.trimmed();

        if (text.isEmpty()) {
            continue;
        }

        nodes.append({std::move(text), block.position(), heading.elementType});
    }
    return nodes;
}

int NavigationWidget::headingPositionForCursor(const QTextCursor &cursor) {
    QTextBlock block = cursor.block();
    const auto rules = navigationScriptingRules();

    while (block.isValid()) {
        if (parseHeadingBlock(block, rules).isValid()) {
            return block.position();
        }

        block = block.previous();
    }

    return cursor.block().position();
}

void NavigationWidget::selectItemForCursorPosition(int position) {
    int itemIndex = findItemIndexForCursorPosition(position);

    QTreeWidgetItem *itemToSelect{nullptr};
    if (itemIndex >= 0) {
        QTreeWidgetItemIterator it(this);
        it += itemIndex;
        itemToSelect = *it;
    }

    QSignalBlocker b(this);
    setCurrentItem(itemToSelect);
}

int NavigationWidget::findItemIndexForCursorPosition(int position) const {
    auto fwdIt =
        std::lower_bound(_navigationTreeNodes.begin(), _navigationTreeNodes.end(), position,
                         [](const Node &node, int position) { return node.pos <= position; });

    return fwdIt - std::begin(_navigationTreeNodes) - 1;
}

QString NavigationWidget::stripMarkdown(const QString &input) {
    // Regular expressions for different Markdown syntax patterns
    static const QRegularExpression boldRegex(R"(\*{2}([^*]+)\*{2})");      // **bold**
    static const QRegularExpression italicRegex(R"(\*{1}([^*]+)\*{1})");    // *italic*
    static const QRegularExpression strikethroughRegex(
        R"(\~{2}([^~]+)\~{2})");                                               // ~~strikethrough~~
    static const QRegularExpression linkRegex(R"(\[([^]]+)\]\(([^)]+)\))");    // [link](url)
    static const QRegularExpression angleBracketLinkRegex(R"(<([^>]+)>)");     // <http://link>
    static const QRegularExpression codeRegex(R"(`([^`]+)`+)");                // `code`

    // Replace each Markdown pattern with an empty string
    QString strippedText = input;
    // boldRegex goes first, because italicRegex needs to match the remaining "*bold*" pattern
    strippedText.replace(boldRegex, "\\1");
    strippedText.replace(italicRegex, "\\1");
    strippedText.replace(strikethroughRegex, "\\1");
    strippedText.replace(linkRegex, "\\1");
    strippedText.replace(angleBracketLinkRegex, "\\1");
    strippedText.replace(codeRegex, "\\1");

    return strippedText;
}

void NavigationWidget::buildNavTree(const QVector<Node> &nodes) {
    if (_navigationTreeNodes == nodes) return;

    _navigationTreeNodes = std::move(nodes);

    clear();
    _lastHeadingItemList.clear();

    for (const auto &node : _navigationTreeNodes) {
        const int elementType = node.elementType;
        const int pos = node.pos;

        auto *item = new QTreeWidgetItem();

        // Strip out Markdown syntax from the headline text
        item->setText(0, stripMarkdown(node.text));
        item->setData(0, Qt::UserRole, pos);
        item->setToolTip(0, tr("headline %1").arg(elementType - MarkdownHighlighter::H1 + 1));
        // Make the item editable to allow renaming headings
        item->setFlags(item->flags() | Qt::ItemIsEditable);

        // attempt to find a suitable parent item for the element type
        QTreeWidgetItem *lastHigherItem = findSuitableParentItem(elementType);

        if (lastHigherItem == nullptr) {
            // if there wasn't a last higher level item then add the current
            // item to the top level
            addTopLevelItem(item);
        } else {
            // if there was a last higher level item then add the current
            // item as child of that item
            lastHigherItem->addChild(item);
        }

        _lastHeadingItemList[elementType] = item;

        // Clear cached deeper heading levels so parent lookup stays in document
        // order when heading levels jump (e.g. H2 followed by H4).
        for (int level = elementType + 1; level <= MarkdownHighlighter::H6; ++level) {
            _lastHeadingItemList.remove(level);
        }
    }
    expandAll();
    restoreExpandState();
    selectItemForCursorPosition(_cursorPosition);
}

/**
 * Attempts to find a suitable parent item for the element type
 */
QTreeWidgetItem *NavigationWidget::findSuitableParentItem(int elementType) const {
    --elementType;
    auto lastHigherItem = _lastHeadingItemList.value(elementType);

    return (lastHigherItem == nullptr && elementType > MarkdownHighlighter::H1)
               ? findSuitableParentItem(elementType)
               : lastHigherItem;
}

/**
 * Shows the context menu for the navigation widget
 */
void NavigationWidget::showContextMenu(const QPoint &pos) {
    QTreeWidgetItem *item = itemAt(pos);
    if (item == nullptr) {
        return;
    }

    setCurrentItem(item);

    QMenu menu(this);

    QAction *expandAllAction = menu.addAction(tr("Expand all"));
    connect(expandAllAction, &QAction::triggered, this, [this, item]() {
        setExpandedRecursively(item, true);
        saveExpandState();
    });

    QAction *collapseAllAction = menu.addAction(tr("Collapse all"));
    connect(collapseAllAction, &QAction::triggered, this, [this, item]() {
        setExpandedRecursively(item, false);
        saveExpandState();
    });

    menu.addSeparator();

    QAction *renameAction = menu.addAction(tr("&Rename heading"));
    connect(renameAction, &QAction::triggered, this, &NavigationWidget::renameHeadingTriggered);

    menu.exec(mapToGlobal(pos));
}

/**
 * Triggers the rename action for the currently selected heading
 */
void NavigationWidget::renameHeadingTriggered() {
    QTreeWidgetItem *item = currentItem();
    if (item != nullptr) {
        editItem(item, 0);
    }
}

/**
 * Returns the position of the next heading after the current position
 * Returns -1 if there is no next heading
 */
int NavigationWidget::getNextHeadingPosition(int currentPosition) const {
    if (_navigationTreeNodes.isEmpty()) {
        return -1;
    }

    // Find the first heading that is after the current position
    for (const auto &node : _navigationTreeNodes) {
        if (node.pos > currentPosition) {
            return node.pos;
        }
    }

    return -1;
}

/**
 * Returns the position of the previous heading before the current position
 * Returns -1 if there is no previous heading
 */
int NavigationWidget::getPreviousHeadingPosition(int currentPosition) const {
    if (_navigationTreeNodes.isEmpty()) {
        return -1;
    }

    // Find the last heading that is before the current position
    int previousPos = -1;
    for (const auto &node : _navigationTreeNodes) {
        if (node.pos >= currentPosition) {
            break;
        }
        previousPos = node.pos;
    }

    return previousPos;
}

/**
 * Returns a unique key for a tree widget item based on its heading level and display text
 */
QString NavigationWidget::itemKey(const QTreeWidgetItem *item) {
    const QString text = item->text(0);
    const QString tooltip = item->toolTip(0);

    // Extract the heading level from the tooltip (e.g. "headline 2")
    return tooltip + QStringLiteral(":") + text;
}

/**
 * Saves the current expand/collapse state for the active note.
 * Only items with children are tracked (leaf items have no expand state).
 */
void NavigationWidget::saveExpandState() {
    if (_currentNoteId < 0) {
        return;
    }

    QSet<QString> collapsed;
    QTreeWidgetItemIterator it(this);

    while (*it) {
        QTreeWidgetItem *item = *it;
        if (item->childCount() > 0 && !item->isExpanded()) {
            collapsed.insert(itemKey(item));
        }
        ++it;
    }

    _collapsedHeadingsPerNote[_currentNoteId] = collapsed;
}

/**
 * Restores the saved expand/collapse state for the current note.
 * If the user never changed the state, all items stay expanded (default).
 */
void NavigationWidget::restoreExpandState() {
    if (_currentNoteId < 0 || !_collapsedHeadingsPerNote.contains(_currentNoteId)) {
        return;
    }

    const QSet<QString> &collapsed = _collapsedHeadingsPerNote[_currentNoteId];

    _restoringExpandState = true;
    QTreeWidgetItemIterator it(this);

    while (*it) {
        QTreeWidgetItem *item = *it;
        if (item->childCount() > 0 && collapsed.contains(itemKey(item))) {
            item->setExpanded(false);
        }
        ++it;
    }
    _restoringExpandState = false;
}

/**
 * Tracks user-initiated expand/collapse changes and saves the state for the current note.
 * Ignored during programmatic expand/collapse (tree rebuild).
 */
void NavigationWidget::onItemExpandedOrCollapsed(QTreeWidgetItem *item) {
    Q_UNUSED(item)

    // Ignore changes triggered by buildNavTree / restoreExpandState
    if (_restoringExpandState || _currentNoteId < 0) {
        return;
    }

    saveExpandState();
}

/**
 * Removes any stored collapsed state for the given note
 */
void NavigationWidget::clearCollapsedStateForNote(int noteId) {
    _collapsedHeadingsPerNote.remove(noteId);
}
