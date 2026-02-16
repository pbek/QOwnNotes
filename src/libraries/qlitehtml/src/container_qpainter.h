/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of QLiteHtml.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#pragma once

#include "qlitehtml_global.h"

#include <QByteArray>
#include <QPaintDevice>
#include <QPainter>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QTextDocument>
#include <QUrl>
#include <QVector>

#include <functional>
#include <memory>

class DocumentContainerPrivate;
class DocumentContainerContextPrivate;

class QLITEHTML_EXPORT DocumentContainerContext
{
public:
    DocumentContainerContext();
    ~DocumentContainerContext();

    void setMasterStyleSheet(const QString &css);

private:
    std::unique_ptr<DocumentContainerContextPrivate> d;

    friend class DocumentContainer;
    friend class DocumentContainerPrivate;
};

class QLITEHTML_EXPORT DocumentContainer
{
public:
    DocumentContainer();
    virtual ~DocumentContainer();

public: // outside API
    void setPaintDevice(QPaintDevice *paintDevice);
    void setDocument(const QByteArray &data, DocumentContainerContext *context);
    bool hasDocument() const;
    void setBaseUrl(const QString &url);
    void setScrollPosition(const QPoint &pos);
    void render(int width, int height);
    void draw(QPainter *painter, const QRect &clip);
    int documentWidth() const;
    int documentHeight() const;
    int anchorY(const QString &anchorName) const;

    // these return areas to redraw in document space
    QVector<QRect> mousePressEvent(const QPoint &documentPos,
                                   const QPoint &viewportPos,
                                   Qt::MouseButton button);
    QVector<QRect> mouseMoveEvent(const QPoint &documentPos, const QPoint &viewportPos);
    QVector<QRect> mouseReleaseEvent(const QPoint &documentPos,
                                     const QPoint &viewportPos,
                                     Qt::MouseButton button);
    QVector<QRect> mouseDoubleClickEvent(const QPoint &documentPos,
                                         const QPoint &viewportPos,
                                         Qt::MouseButton button);
    QVector<QRect> leaveEvent();

    QUrl linkAt(const QPoint &documentPos, const QPoint &viewportPos);

    QString caption() const;
    QString selectedText() const;

    void findText(const QString &text,
                  QTextDocument::FindFlags flags,
                  bool incremental,
                  bool *wrapped,
                  bool *success,
                  QVector<QRect> *oldSelection,
                  QVector<QRect> *newSelection);

    void setDefaultFont(const QFont &font);
    QFont defaultFont() const;

    using DataCallback = std::function<QByteArray(QUrl)>;
    void setDataCallback(const DataCallback &callback);

    using CursorCallback = std::function<void(QCursor)>;
    void setCursorCallback(const CursorCallback &callback);

    using LinkCallback = std::function<void(QUrl)>;
    void setLinkCallback(const LinkCallback &callback);

    using CheckboxCallback = std::function<void(int)>;
    void setCheckboxCallback(const CheckboxCallback &callback);

    using PaletteCallback = std::function<QPalette()>;
    void setPaletteCallback(const PaletteCallback &callback);

    using ClipboardCallback = std::function<void(bool)>;
    void setClipboardCallback(const ClipboardCallback &callback);

    int withFixedElementPosition(int y, const std::function<void()> &action);

private:
    std::unique_ptr<DocumentContainerPrivate> d;
};
