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

#include <QAbstractScrollArea>
#include <QTextDocument>

#include <functional>

class QLiteHtmlWidgetPrivate;

class QLITEHTML_EXPORT QLiteHtmlWidget : public QAbstractScrollArea
{
    Q_OBJECT
public:
    explicit QLiteHtmlWidget(QWidget *parent = nullptr);
    ~QLiteHtmlWidget() override;

    // declaring the getters Q_INVOKABLE to make them Squish-testable
    void setUrl(const QUrl &url);
    Q_INVOKABLE QUrl url() const;
    void setHtml(const QString &content);
    Q_INVOKABLE QString html() const;
    Q_INVOKABLE QString title() const;

    void setZoomFactor(qreal scale);
    qreal zoomFactor() const;

    bool findText(const QString &text,
                  QTextDocument::FindFlags flags,
                  bool incremental,
                  bool *wrapped = nullptr);

    void setDefaultFont(const QFont &font);
    QFont defaultFont() const;

    void scrollToAnchor(const QString &name);

    using ResourceHandler = std::function<QByteArray(QUrl)>;
    void setResourceHandler(const ResourceHandler &handler);

    // declaring this Q_INVOKABLE to make it Squish-testable
    Q_INVOKABLE QString selectedText() const;
    Q_INVOKABLE QString selectedHtml() const;

signals:
    void linkClicked(const QUrl &url);
    void linkHighlighted(const QUrl &url);
    void copyAvailable(bool available);
    void contextMenuRequested(const QPoint &pos, const QUrl &url);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void updateHightlightedLink();
    void setHightlightedLink(const QUrl &url);
    void withFixedTextPosition(const std::function<void()> &action);
    void render();
    QPoint scrollPosition() const;
    void htmlPos(const QPoint &pos, QPoint *viewportPos, QPoint *htmlPos) const;
    QPoint toVirtual(const QPoint &p) const;
    QSize toVirtual(const QSize &s) const;
    QRect toVirtual(const QRect &r) const;
    QRect fromVirtual(const QRect &r) const;

    QLiteHtmlWidgetPrivate *d;
};
