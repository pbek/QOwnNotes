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

#include "filenavigationwidget.h"

#include <entities/notefolder.h>
#include <utils/misc.h>

#include <QDir>
#include <QMenu>
#include <QRegularExpression>
#include <QSignalBlocker>
#include <QTextDocument>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QUrl>
#include <algorithm>
#include <limits>

namespace {
constexpr int PositionRole = Qt::UserRole;
constexpr int FilePathRole = Qt::UserRole + 1;
constexpr int InvalidPosition = -1;
}    // namespace

FileNavigationWidget::FileNavigationWidget(QWidget *parent) : QTreeWidget(parent) {
    QObject::connect(this, &FileNavigationWidget::currentItemChanged, this,
                     &FileNavigationWidget::onCurrentItemChanged);
    QObject::connect(this, &FileNavigationWidget::itemClicked, this,
                     &FileNavigationWidget::onItemClicked);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTreeWidget::customContextMenuRequested, this,
            &FileNavigationWidget::showContextMenu);
}

void FileNavigationWidget::parse(const QTextDocument *document, int textCursorPosition) {
    const QSignalBlocker blocker(this);
    Q_UNUSED(blocker)

    _doc = document;
    _cursorPosition = textCursorPosition;
    buildTree(parseDocument(_doc));
}

QVector<FileNavigationWidget::FileLinkNode> FileNavigationWidget::parseDocument(
    const QTextDocument *document) {
    QVector<FileLinkNode> nodes;

    if (document == nullptr) {
        return nodes;
    }

    const QString text = document->toPlainText();

    struct FilePattern {
        QRegularExpression regex;
        FileItemType type;
        QString basePath;
    };

    const QVector<FilePattern> patterns = {
        {QRegularExpression(QStringLiteral(R"(!\[.*?\]\(.*media/(.+?)\))")),
         FileItemType::MediaFile, NoteFolder::currentMediaPath()},
        {QRegularExpression(QStringLiteral(R"(\[.*?\]\(.*attachments/(.+?)\))")),
         FileItemType::AttachmentFile, NoteFolder::currentAttachmentsPath()},
    };

    for (const auto &pattern : patterns) {
        auto it = pattern.regex.globalMatch(text);

        while (it.hasNext()) {
            const auto match = it.next();
            const QString fileName = QUrl::fromPercentEncoding(match.captured(1).toUtf8());
            const qsizetype matchPosition = match.capturedStart(0);

            if ((matchPosition < 0) || (matchPosition > std::numeric_limits<int>::max())) {
                continue;
            }

            nodes.append({fileName, static_cast<int>(matchPosition), pattern.type,
                          pattern.basePath + QDir::separator() + fileName});
        }
    }

    std::sort(nodes.begin(), nodes.end(), [](const FileLinkNode &left, const FileLinkNode &right) {
        return left.pos < right.pos;
    });

    return nodes;
}

void FileNavigationWidget::buildTree(const QVector<FileLinkNode> &nodes) {
    if (_fileLinkNodes == nodes) {
        selectItemForCursorPosition(_cursorPosition);
        return;
    }

    _fileLinkNodes = nodes;

    clear();

    auto *mediaRootItem = new QTreeWidgetItem();
    mediaRootItem->setText(0, tr("Media files"));
    mediaRootItem->setData(0, PositionRole, InvalidPosition);
    mediaRootItem->setFlags(mediaRootItem->flags() & ~Qt::ItemIsSelectable);
    addTopLevelItem(mediaRootItem);

    auto *attachmentsRootItem = new QTreeWidgetItem();
    attachmentsRootItem->setText(0, tr("Attachments"));
    attachmentsRootItem->setData(0, PositionRole, InvalidPosition);
    attachmentsRootItem->setFlags(attachmentsRootItem->flags() & ~Qt::ItemIsSelectable);
    addTopLevelItem(attachmentsRootItem);

    for (const auto &node : _fileLinkNodes) {
        auto *item = new QTreeWidgetItem();
        item->setText(0, node.text);
        item->setData(0, PositionRole, node.pos);
        item->setData(0, FilePathRole, node.filePath);
        item->setToolTip(0, node.filePath);

        if (node.type == FileItemType::MediaFile) {
            mediaRootItem->addChild(item);
        } else {
            attachmentsRootItem->addChild(item);
        }
    }

    expandAll();
    selectItemForCursorPosition(_cursorPosition);
}

void FileNavigationWidget::selectItemForCursorPosition(int position) {
    QTreeWidgetItem *itemToSelect = nullptr;
    int matchedPosition = InvalidPosition;

    QTreeWidgetItemIterator it(this);
    while (*it != nullptr) {
        auto *item = *it;
        const int itemPosition = item->data(0, PositionRole).toInt();

        if ((itemPosition != InvalidPosition) && (itemPosition <= position) &&
            (itemPosition >= matchedPosition)) {
            matchedPosition = itemPosition;
            itemToSelect = item;
        }

        ++it;
    }

    QSignalBlocker blocker(this);
    setCurrentItem(itemToSelect);
}

bool FileNavigationWidget::hasFileLinks() const { return !_fileLinkNodes.isEmpty(); }

void FileNavigationWidget::emitPositionForItem(const QTreeWidgetItem *item) {
    if (item == nullptr) {
        return;
    }

    const int position = item->data(0, PositionRole).toInt();
    if (position != InvalidPosition) {
        emit positionClicked(position);
    }
}

void FileNavigationWidget::onCurrentItemChanged(QTreeWidgetItem *current,
                                                QTreeWidgetItem *previous) {
    Q_UNUSED(previous)

    emitPositionForItem(current);
}

void FileNavigationWidget::onItemClicked(QTreeWidgetItem *current, int column) {
    Q_UNUSED(column)

    emitPositionForItem(current);
}

void FileNavigationWidget::showContextMenu(const QPoint &pos) {
    auto *item = itemAt(pos);
    if ((item == nullptr) || item->data(0, FilePathRole).toString().isEmpty()) {
        return;
    }

    setCurrentItem(item);

    QMenu menu(this);
    auto *openAction = menu.addAction(tr("&Open file externally"));

    QAction *selectedAction = menu.exec(mapToGlobal(pos));
    if (selectedAction != openAction) {
        return;
    }

    const QString filePath = item->data(0, FilePathRole).toString();
    if (!filePath.isEmpty()) {
        Utils::Misc::openPath(filePath);
    }
}
