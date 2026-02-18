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

#include "qlitehtmlwidget.h"

#include "container_qpainter.h"

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>

const int kScrollBarStep = 40;

// TODO copied from litehtml/include/master.css
const char mastercss[] = R"RAW(
html {
    display: block;
height:100%;
width:100%;
position: relative;
}

head {
    display: none
}

meta {
    display: none
}

title {
    display: none
}

link {
    display: none
}

style {
    display: none
}

script {
    display: none
}

body {
display:block;
    margin:8px;
    height:100%;
width:100%;
}

p {
display:block;
    margin-top:1em;
    margin-bottom:1em;
}

b, strong {
display:inline;
    font-weight:bold;
}

i, em {
display:inline;
    font-style:italic;
}

center
{
    text-align:center;
display:block;
}

a:link
{
    text-decoration: underline;
color: #00f;
cursor: pointer;
}

h1, h2, h3, h4, h5, h6, div {
display:block;
}

h1 {
    font-weight:bold;
    margin-top:0.67em;
    margin-bottom:0.67em;
    font-size: 2em;
}

h2 {
    font-weight:bold;
    margin-top:0.83em;
    margin-bottom:0.83em;
    font-size: 1.5em;
}

h3 {
    font-weight:bold;
    margin-top:1em;
    margin-bottom:1em;
    font-size:1.17em;
}

h4 {
    font-weight:bold;
    margin-top:1.33em;
    margin-bottom:1.33em
}

h5 {
    font-weight:bold;
    margin-top:1.67em;
    margin-bottom:1.67em;
    font-size:.83em;
}

h6 {
    font-weight:bold;
    margin-top:2.33em;
    margin-bottom:2.33em;
    font-size:.67em;
}

br {
display:inline-block;
}

br[clear="all"]
{
clear:both;
}

br[clear="left"]
{
clear:left;
}

br[clear="right"]
{
clear:right;
}

span {
    display:inline
}

img {
display: inline-block;
}

img[align="right"]
{
    float: right;
}

img[align="left"]
{
    float: left;
}

hr {
display: block;
    margin-top: 0.5em;
    margin-bottom: 0.5em;
    margin-left: auto;
    margin-right: auto;
    border-style: inset;
    border-width: 1px
}


/***************** TABLES ********************/

table {
display: table;
    border-collapse: separate;
    border-spacing: 2px;
    border-top-color:gray;
    border-left-color:gray;
    border-bottom-color:black;
    border-right-color:black;
}

tbody, tfoot, thead {
display:table-row-group;
    vertical-align:middle;
}

tr {
display: table-row;
    vertical-align: inherit;
    border-color: inherit;
}

td, th {
display: table-cell;
    vertical-align: inherit;
    border-width:1px;
padding:1px;
}

th {
    font-weight: bold;
}

table[border] {
    border-style:solid;
}

table[border|=0] {
    border-style:none;
}

table[border] td, table[border] th {
    border-style:solid;
    border-top-color:black;
    border-left-color:black;
    border-bottom-color:gray;
    border-right-color:gray;
}

table[border|=0] td, table[border|=0] th {
    border-style:none;
}

caption {
display: table-caption;
}

td[nowrap], th[nowrap] {
    white-space:nowrap;
}

tt, code, kbd, samp {
    font-family: monospace
}
pre, xmp, plaintext, listing {
display: block;
    font-family: monospace;
    white-space: pre;
margin: 1em 0
}

/***************** LISTS ********************/

ul, menu, dir {
display: block;
    list-style-type: disc;
    margin-top: 1em;
    margin-bottom: 1em;
    margin-left: 0;
    margin-right: 0;
    padding-left: 40px
}

ol {
display: block;
    list-style-type: decimal;
    margin-top: 1em;
    margin-bottom: 1em;
    margin-left: 0;
    margin-right: 0;
    padding-left: 40px
}

li {
display: list-item;
}

ul ul, ol ul {
    list-style-type: circle;
}

ol ol ul, ol ul ul, ul ol ul, ul ul ul {
    list-style-type: square;
}

dd {
display: block;
    margin-left: 40px;
}

dl {
display: block;
    margin-top: 1em;
    margin-bottom: 1em;
    margin-left: 0;
    margin-right: 0;
}

dt {
display: block;
}

ol ul, ul ol, ul ul, ol ol {
    margin-top: 0;
    margin-bottom: 0
}

blockquote {
display: block;
    margin-top: 1em;
    margin-bottom: 1em;
    margin-left: 40px;
    margin-left: 40px;
}

/*********** FORM ELEMENTS ************/

form {
display: block;
    margin-top: 0em;
}

option {
display: none;
}

input, textarea, keygen, select, button, isindex {
margin: 0em;
color: initial;
    line-height: normal;
    text-transform: none;
    text-indent: 0;
    text-shadow: none;
display: inline-block;
}
input[type="hidden"] {
display: none;
}


article, aside, footer, header, hgroup, nav, section
{
display: block;
}
)RAW";

class QLiteHtmlWidgetPrivate
{
public:
    QString html;
    DocumentContainerContext context;
    QUrl url;
    DocumentContainer documentContainer;
    qreal zoomFactor = 1;
    QUrl lastHighlightedLink;
};

QLiteHtmlWidget::QLiteHtmlWidget(QWidget *parent)
    : QAbstractScrollArea(parent)
    , d(new QLiteHtmlWidgetPrivate)
{
    setMouseTracking(true);
    horizontalScrollBar()->setSingleStep(kScrollBarStep);
    verticalScrollBar()->setSingleStep(kScrollBarStep);

    d->documentContainer.setCursorCallback([this](const QCursor &c) { viewport()->setCursor(c); });
    d->documentContainer.setPaletteCallback([this] { return palette(); });
    d->documentContainer.setLinkCallback([this](const QUrl &url) {
        QUrl fullUrl = url;
        if (url.isRelative() && url.path(QUrl::FullyEncoded).isEmpty()) { // fragment/anchor only
            fullUrl = d->url;
            fullUrl.setFragment(url.fragment(QUrl::FullyEncoded));
        }
        // delay because document may not be changed directly during this callback
        QMetaObject::invokeMethod(
            this, [this, fullUrl] { emit linkClicked(fullUrl); }, Qt::QueuedConnection);
    });
    d->documentContainer.setClipboardCallback([this](bool yes) { emit copyAvailable(yes); });

    // TODO adapt mastercss to palette (default text & background color)
    d->context.setMasterStyleSheet(mastercss);
}

QLiteHtmlWidget::~QLiteHtmlWidget()
{
    delete d;
}

void QLiteHtmlWidget::setUrl(const QUrl &url)
{
    d->url = url;
    QUrl baseUrl = url;
    baseUrl.setFragment({});
    const QString path = baseUrl.path(QUrl::FullyEncoded);
    const int lastSlash = path.lastIndexOf('/');
    const QString basePath = lastSlash >= 0 ? path.left(lastSlash) : QString();
    baseUrl.setPath(basePath);
    d->documentContainer.setBaseUrl(baseUrl.toString(QUrl::FullyEncoded));
    QMetaObject::invokeMethod(this, [this] { updateHightlightedLink(); }, Qt::QueuedConnection);
}

QUrl QLiteHtmlWidget::url() const
{
    return d->url;
}

void QLiteHtmlWidget::setHtml(const QString &content)
{
    d->html = content;
    d->documentContainer.setPaintDevice(viewport());
    d->documentContainer.setDocument(content.toUtf8(), &d->context);
    verticalScrollBar()->setValue(0);
    horizontalScrollBar()->setValue(0);
    render();
    QMetaObject::invokeMethod(this, [this] { updateHightlightedLink(); }, Qt::QueuedConnection);
}

QString QLiteHtmlWidget::html() const
{
    return d->html;
}

QString QLiteHtmlWidget::title() const
{
    return d->documentContainer.caption();
}

void QLiteHtmlWidget::setZoomFactor(qreal scale)
{
    Q_ASSERT(scale != 0);
    d->zoomFactor = scale;
    withFixedTextPosition([this] { render(); });
}

qreal QLiteHtmlWidget::zoomFactor() const
{
    return d->zoomFactor;
}

bool QLiteHtmlWidget::findText(const QString &text,
                               QTextDocument::FindFlags flags,
                               bool incremental,
                               bool *wrapped)
{
    bool success = false;
    QVector<QRect> oldSelection;
    QVector<QRect> newSelection;
    d->documentContainer
        .findText(text, flags, incremental, wrapped, &success, &oldSelection, &newSelection);
    // scroll to search result position and/or redraw as necessary
    QRect newSelectionCombined;
    for (const QRect &r : std::as_const(newSelection))
        newSelectionCombined = newSelectionCombined.united(r);
    QScrollBar *vBar = verticalScrollBar();
    const int top = newSelectionCombined.top();
    const int bottom = newSelectionCombined.bottom() - toVirtual(viewport()->size()).height();
    if (success && top < vBar->value() && vBar->minimum() <= top) {
        vBar->setValue(top);
    } else if (success && vBar->value() < bottom && bottom <= vBar->maximum()) {
        vBar->setValue(bottom);
    } else {
        viewport()->update(fromVirtual(newSelectionCombined.translated(-scrollPosition())));
        for (const QRect &r : std::as_const(oldSelection))
            viewport()->update(fromVirtual(r.translated(-scrollPosition())));
    }
    return success;
}

void QLiteHtmlWidget::setDefaultFont(const QFont &font)
{
    withFixedTextPosition([this, &font] {
        d->documentContainer.setDefaultFont(font);
        render();
    });
}

QFont QLiteHtmlWidget::defaultFont() const
{
    return d->documentContainer.defaultFont();
}

void QLiteHtmlWidget::scrollToAnchor(const QString &name)
{
    if (!d->documentContainer.hasDocument())
        return;
    horizontalScrollBar()->setValue(0);
    if (name.isEmpty()) {
        verticalScrollBar()->setValue(0);
        return;
    }
    const int y = d->documentContainer.anchorY(name);
    if (y >= 0)
        verticalScrollBar()->setValue(std::min(y, verticalScrollBar()->maximum()));
}

void QLiteHtmlWidget::setResourceHandler(const QLiteHtmlWidget::ResourceHandler &handler)
{
    d->documentContainer.setDataCallback(handler);
}

QString QLiteHtmlWidget::selectedText() const
{
    return d->documentContainer.selectedText();
}

void QLiteHtmlWidget::paintEvent(QPaintEvent *event)
{
    if (!d->documentContainer.hasDocument())
        return;
    d->documentContainer.setScrollPosition(scrollPosition());
    QPainter p(viewport());
    p.setWorldTransform(QTransform().scale(d->zoomFactor, d->zoomFactor));
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    p.setRenderHint(QPainter::Antialiasing, true);
    d->documentContainer.draw(&p, toVirtual(event->rect()));
}

void QLiteHtmlWidget::resizeEvent(QResizeEvent *event)
{
    withFixedTextPosition([this, event] {
        QAbstractScrollArea::resizeEvent(event);
        render();
    });
}

void QLiteHtmlWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint viewportPos;
    QPoint pos;
    htmlPos(event->pos(), &viewportPos, &pos);
    const QVector<QRect> areas = d->documentContainer.mouseMoveEvent(pos, viewportPos);
    for (const QRect &r : areas)
        viewport()->update(fromVirtual(r.translated(-scrollPosition())));

    updateHightlightedLink();
}

void QLiteHtmlWidget::mousePressEvent(QMouseEvent *event)
{
    QPoint viewportPos;
    QPoint pos;
    htmlPos(event->pos(), &viewportPos, &pos);
    const QVector<QRect> areas = d->documentContainer.mousePressEvent(pos,
                                                                      viewportPos,
                                                                      event->button());
    for (const QRect &r : areas)
        viewport()->update(fromVirtual(r.translated(-scrollPosition())));
}

void QLiteHtmlWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QPoint viewportPos;
    QPoint pos;
    htmlPos(event->pos(), &viewportPos, &pos);
    const QVector<QRect> areas = d->documentContainer.mouseReleaseEvent(pos,
                                                                        viewportPos,
                                                                        event->button());
    for (const QRect &r : areas)
        viewport()->update(fromVirtual(r.translated(-scrollPosition())));
}

void QLiteHtmlWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint viewportPos;
    QPoint pos;
    htmlPos(event->pos(), &viewportPos, &pos);
    const QVector<QRect> areas = d->documentContainer.mouseDoubleClickEvent(pos,
                                                                            viewportPos,
                                                                            event->button());
    for (const QRect &r : areas) {
        viewport()->update(fromVirtual(r.translated(-scrollPosition())));
    }
}

void QLiteHtmlWidget::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    const QVector<QRect> areas = d->documentContainer.leaveEvent();
    for (const QRect &r : areas)
        viewport()->update(fromVirtual(r.translated(-scrollPosition())));
    setHightlightedLink(QUrl());
}

void QLiteHtmlWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint viewportPos;
    QPoint pos;
    htmlPos(event->pos(), &viewportPos, &pos);
    emit contextMenuRequested(event->pos(), d->documentContainer.linkAt(pos, viewportPos));
}

static QAbstractSlider::SliderAction getSliderAction(int key)
{
    if (key == Qt::Key_Home)
        return QAbstractSlider::SliderToMinimum;
    if (key == Qt::Key_End)
        return QAbstractSlider::SliderToMaximum;
    if (key == Qt::Key_PageUp)
        return QAbstractSlider::SliderPageStepSub;
    if (key == Qt::Key_PageDown)
        return QAbstractSlider::SliderPageStepAdd;
    return QAbstractSlider::SliderNoAction;
}

void QLiteHtmlWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::KeypadModifier) {
        const QAbstractSlider::SliderAction sliderAction = getSliderAction(event->key());
        if (sliderAction != QAbstractSlider::SliderNoAction) {
            verticalScrollBar()->triggerAction(sliderAction);
            event->accept();
        }
    }

    QAbstractScrollArea::keyPressEvent(event);
}

void QLiteHtmlWidget::updateHightlightedLink()
{
    QPoint viewportPos;
    QPoint pos;
    htmlPos(mapFromGlobal(QCursor::pos()), &viewportPos, &pos);
    setHightlightedLink(d->documentContainer.linkAt(pos, viewportPos));
}

void QLiteHtmlWidget::setHightlightedLink(const QUrl &url)
{
    if (d->lastHighlightedLink == url)
        return;
    d->lastHighlightedLink = url;
    emit linkHighlighted(d->lastHighlightedLink);
}

void QLiteHtmlWidget::withFixedTextPosition(const std::function<void()> &action)
{
    // remember element to which to scroll after re-rendering
    QPoint viewportPos;
    QPoint pos;
    htmlPos({}, &viewportPos, &pos); // top-left
    const int y = d->documentContainer.withFixedElementPosition(pos.y(), action);
    if (y >= 0)
        verticalScrollBar()->setValue(std::min(y, verticalScrollBar()->maximum()));
}

void QLiteHtmlWidget::render()
{
    if (!d->documentContainer.hasDocument())
        return;
    const int fullWidth = width() / d->zoomFactor;
    const QSize vViewportSize = toVirtual(viewport()->size());
    const int scrollbarWidth = style()->pixelMetric(QStyle::PM_ScrollBarExtent, nullptr, this);
    const int w = fullWidth - scrollbarWidth - 2;
    d->documentContainer.render(w, vViewportSize.height());
    // scroll bars reflect virtual/scaled size of html document
    horizontalScrollBar()->setPageStep(vViewportSize.width());
    horizontalScrollBar()->setRange(0, std::max(0, d->documentContainer.documentWidth() - w));
    verticalScrollBar()->setPageStep(vViewportSize.height());
    verticalScrollBar()
        ->setRange(0, std::max(0, d->documentContainer.documentHeight() - vViewportSize.height()));
    viewport()->update();
}

QPoint QLiteHtmlWidget::scrollPosition() const
{
    return {horizontalScrollBar()->value(), verticalScrollBar()->value()};
}

void QLiteHtmlWidget::htmlPos(const QPoint &pos, QPoint *viewportPos, QPoint *htmlPos) const
{
    *viewportPos = toVirtual(viewport()->mapFromParent(pos));
    *htmlPos = *viewportPos + scrollPosition();
}

QPoint QLiteHtmlWidget::toVirtual(const QPoint &p) const
{
    return {int(p.x() / d->zoomFactor), int(p.y() / d->zoomFactor)};
}

QSize QLiteHtmlWidget::toVirtual(const QSize &s) const
{
    return {int(s.width() / d->zoomFactor), int(s.height() / d->zoomFactor)};
}

QRect QLiteHtmlWidget::toVirtual(const QRect &r) const
{
    return {toVirtual(r.topLeft()), toVirtual(r.size())};
}

QRect QLiteHtmlWidget::fromVirtual(const QRect &r) const
{
    const QPoint tl{int(r.x() * d->zoomFactor), int(r.y() * d->zoomFactor)};
    // round size up, and add one since the topleft point was rounded down
    const QSize s{int(r.width() * d->zoomFactor + 0.5) + 1,
                  int(r.height() * d->zoomFactor + 0.5) + 1};
    return {tl, s};
}
