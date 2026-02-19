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

#include "container_qpainter.h"
#include "container_qpainter_p.h"
#include "element_checkbox.h"

#if QT_CONFIG(clipboard)
#include <QClipboard>
#endif
#include <QCursor>
#include <QDebug>
#include <QDir>
#include <QFont>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QLoggingCategory>
#include <QPainter>
#include <QPalette>
#include <QRegularExpression>
#include <QScreen>
#include <QTextLayout>
#include <QUrl>

#include <algorithm>
#include <set>

#if Q_OS_WIN
#include <Windows.h>
#endif

const int kDragDistance = 5;

using Font = QFont;
using Context = QPainter;

namespace {
static Q_LOGGING_CATEGORY(log, "qlitehtml", QtCriticalMsg)
}

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
namespace Qt {
namespace {
auto constexpr SkipEmptyParts = QString::SkipEmptyParts;
}
} // namespace Qt
#endif

static QFont toQFont(litehtml::uint_ptr hFont)
{
    return *reinterpret_cast<Font *>(hFont);
}

static QPainter *toQPainter(litehtml::uint_ptr hdc)
{
    return reinterpret_cast<Context *>(hdc);
}

static QRect toQRect(litehtml::position position)
{
    return {position.x, position.y, position.width, position.height};
}

static litehtml::elements_vector path(const litehtml::element::ptr &element)
{
    litehtml::elements_vector result;
    litehtml::element::ptr current = element;
    while (current) {
        result.push_back(current);
        current = current->parent();
    }
    std::reverse(std::begin(result), std::end(result));
    return result;
}

static std::pair<litehtml::element::ptr, size_t> getCommonParent(const litehtml::elements_vector &a,
                                                                 const litehtml::elements_vector &b)
{
    litehtml::element::ptr parent;
    const size_t minSize = std::min(a.size(), b.size());
    for (size_t i = 0; i < minSize; ++i) {
        if (a.at(i) != b.at(i))
            return {parent, i};
        parent = a.at(i);
    }
    return {parent, minSize};
}

static std::pair<Selection::Element, Selection::Element> getStartAndEnd(const Selection::Element &a,
                                                                        const Selection::Element &b)
{
    if (a.element == b.element) {
        if (a.index <= b.index)
            return {a, b};
        return {b, a};
    }
    const litehtml::elements_vector aPath = path(a.element);
    const litehtml::elements_vector bPath = path(b.element);
    litehtml::element::ptr commonParent;
    size_t firstDifferentIndex;
    std::tie(commonParent, firstDifferentIndex) = getCommonParent(aPath, bPath);
    if (!commonParent) {
        qWarning() << "internal error: litehtml elements do not have common parent";
        return {a, b};
    }
    if (commonParent == a.element)
        return {a, a}; // 'a' already contains 'b'
    if (commonParent == b.element)
        return {b, b};
    // find out if a or b is first in the child sub-trees of commonParent
    const litehtml::element::ptr aBranch = aPath.at(firstDifferentIndex);
    const litehtml::element::ptr bBranch = bPath.at(firstDifferentIndex);
    for (int i = 0; i < int(commonParent->get_children_count()); ++i) {
        const litehtml::element::ptr child = commonParent->get_child(i);
        if (child == aBranch)
            return {a, b};
        if (child == bBranch)
            return {b, a};
    }
    qWarning() << "internal error: failed to find out order of litehtml elements";
    return {a, b};
}

static int findChild(const litehtml::element::ptr &child, const litehtml::element::ptr &parent)
{
    for (int i = 0; i < int(parent->get_children_count()); ++i)
        if (parent->get_child(i) == child)
            return i;
    return -1;
}

// 1) stops right away if element == stop, otherwise stops whenever stop element is encountered
// 2) moves down the first children from element until there is none anymore
static litehtml::element::ptr firstLeaf(const litehtml::element::ptr &element,
                                        const litehtml::element::ptr &stop)
{
    if (element == stop)
        return element;
    litehtml::element::ptr current = element;
    while (current != stop && current->get_children_count() > 0)
        current = current->get_child(0);
    return current;
}

// 1) stops right away if element == stop, otherwise stops whenever stop element is encountered
// 2) starts at next sibling (up the hierarchy chain) if possible, otherwise root
// 3) returns first leaf of the element found in 2
static litehtml::element::ptr nextLeaf(const litehtml::element::ptr &element,
                                       const litehtml::element::ptr &stop)
{
    if (element == stop)
        return element;
    litehtml::element::ptr current = element;
    if (current->have_parent()) {
        // find next sibling
        const litehtml::element::ptr parent = current->parent();
        const int childIndex = findChild(current, parent);
        if (childIndex < 0) {
            qWarning() << "internal error: filed to find litehtml child element in parent";
            return stop;
        }
        if (childIndex + 1 >= int(parent->get_children_count())) // no sibling, move up
            return nextLeaf(parent, stop);
        current = parent->get_child(childIndex + 1);
    }
    return firstLeaf(current, stop);
}

static Selection::Element selectionDetails(const litehtml::element::ptr &element,
                                           const QString &text,
                                           const QPoint &pos)
{
    // If element has children, the text contains all descendant text concatenated
    // For better precision, we should find the actual leaf child at this position
    if (element->get_children_count() > 0) {
        // Try to find a child leaf element that contains this position
        const std::function<Selection::Element(litehtml::element::ptr, QPoint)> findLeaf =
            [&findLeaf, pos](const litehtml::element::ptr &elem,
                             const QPoint &offset) -> Selection::Element {
            if (!elem)
                return {};

            const QRect elemPos = toQRect(elem->get_position()).translated(offset);

            if (!elemPos.contains(pos))
                return {};

            // Try children first
            for (int i = 0; i < int(elem->get_children_count()); ++i) {
                const litehtml::element::ptr child = elem->get_child(i);
                Selection::Element result = findLeaf(child, elemPos.topLeft());
                if (result.element)
                    return result;
            }

            // No children found, use this element if it has text
            litehtml::tstring elemText;
            elem->get_text(elemText);
            if (!elemText.empty()) {
                const QString textStr = QString::fromStdString(elemText);
                const QFont &font = toQFont(elem->get_font());
                const QFontMetrics fm(font);
                const QPoint relativePos = pos - elemPos.topLeft();

                int previous = 0;
                for (int i = 0; i < textStr.size(); ++i) {
                    const int width = fm.size(0, textStr.left(i + 1)).width();
                    if ((width + previous) / 2 >= relativePos.x())
                        return {elem, i, previous};
                    previous = width;
                }
                return {elem, int(textStr.size()), previous};
            }

            return {};
        };

        const QRect elementRect = toQRect(element->get_placement());
        Selection::Element result = findLeaf(element, elementRect.topLeft());
        if (result.element)
            return result;

        // Fallback: return the element with index at the start
        return {element, 0, 0};
    }

    // Element has no children - calculate precise character position
    const QFont &font = toQFont(element->get_font());
    const QFontMetrics fm(font);
    int previous = 0;
    for (int i = 0; i < text.size(); ++i) {
        const int width = fm.size(0, text.left(i + 1)).width();
        if ((width + previous) / 2 >= pos.x())
            return {element, i, previous};
        previous = width;
    }
    return {element, int(text.size()), previous};
}

static Selection::Element deepest_child_at_point(const litehtml::document::ptr &document,
                                                 const QPoint &pos,
                                                 const QPoint &viewportPos,
                                                 Selection::Mode mode)
{
    if (!document)
        return {};

    // Find the element at this point
    const litehtml::element::ptr element = document->root()->get_element_by_point(pos.x(),
                                                                                  pos.y(),
                                                                                  viewportPos.x(),
                                                                                  viewportPos.y());
    // Recursively find the deepest child that contains the point and has text
    const std::function<Selection::Element(litehtml::element::ptr, QRect)> recursion =
        [&recursion, pos, mode](const litehtml::element::ptr &element,
                                const QRect &placement) -> Selection::Element {
        if (!element)
            return {};

        // First, try to find a deeper child that contains the point
        Selection::Element deepestResult;
        for (int i = 0; i < int(element->get_children_count()); ++i) {
            const litehtml::element::ptr child = element->get_child(i);
            const QRect childPlacement = toQRect(child->get_position())
                                             .translated(placement.topLeft());
            deepestResult = recursion(child, childPlacement);
            if (deepestResult.element)
                return deepestResult;
        }

        // No deeper child found, check if this element itself has text content
        if (placement.contains(pos)) {
            litehtml::tstring text;
            element->get_text(text);
            if (!text.empty()) {
                return mode == Selection::Mode::Free
                           ? selectionDetails(element,
                                              QString::fromStdString(text),
                                              pos - placement.topLeft())
                           : Selection::Element({element, -1, -1});
            }
        }

        return {};
    };
    return recursion(element, element ? toQRect(element->get_placement()) : QRect());
}

// CSS: 400 == normal, 700 == bold.
// Qt5: 50 == normal, 75 == bold
// Qt6: == CSS
static QFont::Weight cssWeightToQtWeight(int cssWeight)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QFont::Weight(cssWeight);
#else
    if (cssWeight <= 400)
        return QFont::Weight(cssWeight * 50 / 400);
    if (cssWeight >= 700)
        return QFont::Weight(75 + (cssWeight - 700) * 25 / 300);
    return QFont::Weight(50 + (cssWeight - 400) * 25 / 300);
#endif
}

static QFont::Style toQFontStyle(litehtml::font_style style)
{
    switch (style) {
    case litehtml::fontStyleNormal:
        return QFont::StyleNormal;
    case litehtml::fontStyleItalic:
        return QFont::StyleItalic;
    }
    // should not happen
    qWarning(log) << "Unknown litehtml font style:" << style;
    return QFont::StyleNormal;
}

static QColor toQColor(const litehtml::web_color &color)
{
    return {color.red, color.green, color.blue, color.alpha};
}

static Qt::PenStyle borderPenStyle(litehtml::border_style style)
{
    switch (style) {
    case litehtml::border_style_dotted:
        return Qt::DotLine;
    case litehtml::border_style_dashed:
        return Qt::DashLine;
    case litehtml::border_style_solid:
        return Qt::SolidLine;
    default:
        qWarning(log) << "Unsupported border style:" << style;
    }
    return Qt::SolidLine;
}

static QPen borderPen(const litehtml::border &border)
{
    return {toQColor(border.color), qreal(border.width), borderPenStyle(border.style)};
}

static QCursor toQCursor(const QString &c)
{
    if (c == "alias")
        return {Qt::PointingHandCursor}; // ???
    if (c == "all-scroll")
        return {Qt::SizeAllCursor};
    if (c == "auto")
        return {Qt::ArrowCursor}; // ???
    if (c == "cell")
        return {Qt::UpArrowCursor};
    if (c == "context-menu")
        return {Qt::ArrowCursor}; // ???
    if (c == "col-resize")
        return {Qt::SplitHCursor};
    if (c == "copy")
        return {Qt::DragCopyCursor};
    if (c == "crosshair")
        return {Qt::CrossCursor};
    if (c == "default")
        return {Qt::ArrowCursor};
    if (c == "e-resize")
        return {Qt::SizeHorCursor}; // ???
    if (c == "ew-resize")
        return {Qt::SizeHorCursor};
    if (c == "grab")
        return {Qt::OpenHandCursor};
    if (c == "grabbing")
        return {Qt::ClosedHandCursor};
    if (c == "help")
        return {Qt::WhatsThisCursor};
    if (c == "move")
        return {Qt::SizeAllCursor};
    if (c == "n-resize")
        return {Qt::SizeVerCursor}; // ???
    if (c == "ne-resize")
        return {Qt::SizeBDiagCursor}; // ???
    if (c == "nesw-resize")
        return {Qt::SizeBDiagCursor};
    if (c == "ns-resize")
        return {Qt::SizeVerCursor};
    if (c == "nw-resize")
        return {Qt::SizeFDiagCursor}; // ???
    if (c == "nwse-resize")
        return {Qt::SizeFDiagCursor};
    if (c == "no-drop")
        return {Qt::ForbiddenCursor};
    if (c == "none")
        return {Qt::BlankCursor};
    if (c == "not-allowed")
        return {Qt::ForbiddenCursor};
    if (c == "pointer")
        return {Qt::PointingHandCursor};
    if (c == "progress")
        return {Qt::BusyCursor};
    if (c == "row-resize")
        return {Qt::SplitVCursor};
    if (c == "s-resize")
        return {Qt::SizeVerCursor}; // ???
    if (c == "se-resize")
        return {Qt::SizeFDiagCursor}; // ???
    if (c == "sw-resize")
        return {Qt::SizeBDiagCursor}; // ???
    if (c == "text")
        return {Qt::IBeamCursor};
    if (c == "url")
        return {Qt::ArrowCursor}; // ???
    if (c == "w-resize")
        return {Qt::SizeHorCursor}; // ???
    if (c == "wait")
        return {Qt::BusyCursor};
    if (c == "zoom-in")
        return {Qt::ArrowCursor}; // ???
    qWarning(log) << QStringLiteral("unknown cursor property \"%1\"").arg(c).toUtf8().constData();
    return {Qt::ArrowCursor};
}

bool Selection::isValid() const
{
    return !selection.isEmpty();
}

void Selection::update()
{
    const auto addElement = [this](const Selection::Element &element,
                                   const Selection::Element &end = {}) {
        litehtml::tstring elemText;
        element.element->get_text(elemText);
        const QString textStr = QString::fromStdString(elemText);
        if (!textStr.isEmpty()) {
            QRect rect = toQRect(element.element->get_placement()).adjusted(-1, -1, 1, 1);
            if (element.index < 0) { // fully selected
                text += textStr;
            } else if (end.element) { // select from element "to end"
                if (element.element == end.element) {
                    // end.index is guaranteed to be >= element.index by caller, same for x
                    text += textStr.mid(element.index, end.index - element.index);
                    const int left = rect.left();
                    rect.setLeft(left + element.x);
                    rect.setRight(left + end.x);
                } else {
                    text += textStr.mid(element.index);
                    rect.setLeft(rect.left() + element.x);
                }
            } else { // select from start of element
                text += textStr.left(element.index);
                rect.setRight(rect.left() + element.x);
            }
            selection.append(rect);
        }
    };

    if (startElem.element && endElem.element) {
        // Edge cases:
        // start and end elements could be reversed or children of each other
        Selection::Element start;
        Selection::Element end;
        std::tie(start, end) = getStartAndEnd(startElem, endElem);

        selection.clear();
        text.clear();

        // Treats start element as a leaf even if it isn't, because it already contains all its
        // children
        addElement(start, end);
        if (start.element != end.element) {
            litehtml::element::ptr current = start.element;
            do {
                current = nextLeaf(current, end.element);
                if (current == end.element)
                    addElement(end);
                else
                    addElement({current, -1, -1});
            } while (current != end.element);
        }
    } else {
        selection = {};
        text.clear();
    }
#if QT_CONFIG(clipboard)
    QClipboard *cb = QGuiApplication::clipboard();
    if (cb->supportsSelection())
        cb->setText(text, QClipboard::Selection);
#endif
}

QRect Selection::boundingRect() const
{
    QRect rect;
    for (const QRect &r : selection)
        rect = rect.united(r);
    return rect;
}

DocumentContainer::DocumentContainer()
    : d(new DocumentContainerPrivate)
{}

DocumentContainer::~DocumentContainer() = default;

litehtml::uint_ptr DocumentContainerPrivate::create_font(const litehtml::tchar_t *faceName,
                                                         int size,
                                                         int weight,
                                                         litehtml::font_style italic,
                                                         unsigned int decoration,
                                                         litehtml::font_metrics *fm)
{
    const QStringList splitNames = QString::fromUtf8(faceName).split(',', Qt::SkipEmptyParts);
    QStringList familyNames;
    std::transform(splitNames.cbegin(),
                   splitNames.cend(),
                   std::back_inserter(familyNames),
                   [this](const QString &s) {
                       // clean whitespace and quotes
                       QString name = s.trimmed();
                       if (name.startsWith('\"'))
                           name = name.mid(1);
                       if (name.endsWith('\"'))
                           name.chop(1);
                       const QString lowerName = name.toLower();
                       if (lowerName == "serif")
                           return serifFont();
                       if (lowerName == "sans-serif")
                           return sansSerifFont();
                       if (lowerName == "monospace")
                           return monospaceFont();
                       return name;
                   });
    auto font = new QFont();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
    font->setFamilies(familyNames);
#else
    struct CompareCaseinsensitive
    {
        bool operator()(const QString &a, const QString &b) const
        {
            return a.compare(b, Qt::CaseInsensitive) < 0;
        }
    };
    static const QStringList known = QFontDatabase().families();
    static const std::set<QString, CompareCaseinsensitive> knownFamilies(known.cbegin(),
                                                                         known.cend());
    font->setFamily(familyNames.last());
    for (const QString &name : qAsConst(familyNames)) {
        const auto found = knownFamilies.find(name);
        if (found != knownFamilies.end()) {
            font->setFamily(*found);
            break;
        }
    }
#endif
    font->setPixelSize(size);
    font->setWeight(cssWeightToQtWeight(weight));
    font->setStyle(toQFontStyle(italic));
    if (decoration == litehtml::font_decoration_underline)
        font->setUnderline(true);
    if (decoration == litehtml::font_decoration_overline)
        font->setOverline(true);
    if (decoration == litehtml::font_decoration_linethrough)
        font->setStrikeOut(true);
    if (fm) {
        const QFontMetrics metrics(*font);
        fm->height = metrics.height();
        fm->ascent = metrics.ascent();
        fm->descent = metrics.descent();
        fm->x_height = metrics.xHeight();
        fm->draw_spaces = true;
    }
    return reinterpret_cast<litehtml::uint_ptr>(font);
}

void DocumentContainerPrivate::delete_font(litehtml::uint_ptr hFont)
{
    auto font = reinterpret_cast<Font *>(hFont);
    delete font;
}

int DocumentContainerPrivate::text_width(const litehtml::tchar_t *text, litehtml::uint_ptr hFont)
{
    const QFontMetrics fm(toQFont(hFont));
    return fm.horizontalAdvance(QString::fromUtf8(text));
}

void DocumentContainerPrivate::draw_text(litehtml::uint_ptr hdc,
                                         const litehtml::tchar_t *text,
                                         litehtml::uint_ptr hFont,
                                         litehtml::web_color color,
                                         const litehtml::position &pos)
{
    auto painter = toQPainter(hdc);
    painter->setFont(toQFont(hFont));
    painter->setPen(toQColor(color));
    painter->drawText(toQRect(pos), 0, QString::fromUtf8(text));
}

int DocumentContainerPrivate::pt_to_px(int pt) const
{
#if Q_OS_WIN
    HDC dc = GetDC(NULL);
    int ret = MulDiv(pt, GetDeviceCaps(dc, LOGPIXELSY), 72);
    ReleaseDC(NULL, dc);
    return ret;
#endif

    const qreal dpi = m_paintDevice->logicalDpiY();
    return (int) (qreal(pt) * dpi / 72.0);

#if 0
// magic factor of 11/12 to account for differences to webengine/webkit
// return m_paintDevice->physicalDpiY() * pt * 11 / m_paintDevice->logicalDpiY() / 12;
#endif
}

int DocumentContainerPrivate::get_default_font_size() const
{
    int pointSize = m_defaultFont.pointSize();
    if (pointSize <= 0) {
        int pixelSize = m_defaultFont.pixelSize();
        if (pixelSize > 0 && m_paintDevice) {
            pointSize = qRound(m_paintDevice->logicalDpiY() * pixelSize / 72.0);
        }
    }
    if (pointSize <= 0) {
        pointSize = 16;
    }
    return pointSize;
}

const litehtml::tchar_t *DocumentContainerPrivate::get_default_font_name() const
{
    return m_defaultFontFamilyName.constData();
}

void DocumentContainerPrivate::draw_list_marker(litehtml::uint_ptr hdc,
                                                const litehtml::list_marker &marker)
{
    auto painter = toQPainter(hdc);
    if (marker.image.empty()) {
        if (marker.marker_type == litehtml::list_style_type_square) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(toQColor(marker.color));
            painter->drawRect(toQRect(marker.pos));
        } else if (marker.marker_type == litehtml::list_style_type_disc) {
            painter->setPen(Qt::NoPen);
            painter->setBrush(toQColor(marker.color));
            painter->drawEllipse(toQRect(marker.pos));
        } else if (marker.marker_type == litehtml::list_style_type_circle) {
            painter->setPen(toQColor(marker.color));
            painter->setBrush(Qt::NoBrush);
            painter->drawEllipse(toQRect(marker.pos));
        } else {
            // TODO we do not get information about index and font for e.g. decimal / roman
            // at least draw a bullet
            painter->setPen(Qt::NoPen);
            painter->setBrush(toQColor(marker.color));
            painter->drawEllipse(toQRect(marker.pos));
            qWarning(log) << "list marker of type" << marker.marker_type << "not supported";
        }
    } else {
        const QPixmap pixmap = getPixmap(QString::fromStdString(marker.image),
                                         QString::fromStdString(marker.baseurl));
        painter->drawPixmap(toQRect(marker.pos), pixmap);
    }
}

void DocumentContainerPrivate::load_image(const litehtml::tchar_t *src,
                                          const litehtml::tchar_t *baseurl,
                                          bool redraw_on_ready)
{
    const auto qtSrc = QString::fromUtf8(src);
    const auto qtBaseUrl = QString::fromUtf8(baseurl);
    Q_UNUSED(redraw_on_ready)
    qDebug() << "load_image:" << QStringLiteral("src = \"%1\";").arg(qtSrc).toUtf8().constData()
             << QStringLiteral("base = \"%1\"").arg(qtBaseUrl).toUtf8().constData();
    const QUrl url = resolveUrl(qtSrc, qtBaseUrl);
    if (m_pixmaps.contains(url))
        return;

    QPixmap pixmap;
    if (m_dataCallback)
        pixmap.loadFromData(m_dataCallback(url));
    m_pixmaps.insert(url, pixmap);
}

void DocumentContainerPrivate::get_image_size(const litehtml::tchar_t *src,
                                              const litehtml::tchar_t *baseurl,
                                              litehtml::size &sz)
{
    const auto qtSrc = QString::fromUtf8(src);
    const auto qtBaseUrl = QString::fromUtf8(baseurl);
    if (qtSrc.isEmpty()) // for some reason that happens
        return;
    qDebug(log) << "get_image_size:"
                << QStringLiteral("src = \"%1\";").arg(qtSrc).toUtf8().constData()
                << QStringLiteral("base = \"%1\"").arg(qtBaseUrl).toUtf8().constData();
    const QPixmap pm = getPixmap(qtSrc, qtBaseUrl);
    sz.width = pm.width();
    sz.height = pm.height();
}

void DocumentContainerPrivate::drawSelection(QPainter *painter, const QRect &clip) const
{
    painter->save();
    painter->setClipRect(clip, Qt::IntersectClip);
    for (const QRect &r : m_selection.selection) {
        const QRect clientRect = r.translated(-m_scrollPosition);
        const QPalette palette = m_paletteCallback();
        painter->fillRect(clientRect, palette.brush(QPalette::Highlight));
    }
    painter->restore();
}

static QString tagName(const litehtml::element::ptr &e)
{
    litehtml::element::ptr current = e;
    while (current && std::strlen(current->get_tagName()) == 0)
        current = current->parent();
    return current ? QString::fromUtf8(current->get_tagName()) : QString();
}

void DocumentContainerPrivate::buildIndex()
{
    m_index.elementToIndex.clear();
    m_index.indexToElement.clear();
    m_index.text.clear();

    int index = 0;
    bool inBody = false;
    litehtml::element::ptr current = firstLeaf(m_document->root(), nullptr);
    while (current != m_document->root()) {
        m_index.elementToIndex.insert({current, index});
        if (!inBody)
            inBody = tagName(current).toLower() == "body";
        if (inBody && current->is_visible()) {
            litehtml::tstring text;
            current->get_text(text);
            if (!text.empty()) {
                m_index.indexToElement.push_back({index, current});
                const QString str = QString::fromStdString(text);
                m_index.text += str;
                index += str.size();
            }
        }
        current = nextLeaf(current, m_document->root());
    }
}

void DocumentContainerPrivate::updateSelection()
{
    const QString oldText = m_selection.text;
    m_selection.update();
    if (!m_clipboardCallback)
        return;

    const QString newText = m_selection.text;
    if (oldText.isEmpty() && !newText.isEmpty())
        m_clipboardCallback(true);
    else if (!oldText.isEmpty() && newText.isEmpty())
        m_clipboardCallback(false);
}

void DocumentContainerPrivate::clearSelection()
{
    const QString oldText = m_selection.text;
    m_selection = {};
    if (!m_clipboardCallback)
        return;

    if (!oldText.isEmpty())
        m_clipboardCallback(false);
}

void DocumentContainerPrivate::draw_background(litehtml::uint_ptr hdc,
                                               const litehtml::background_paint &bg)
{
    auto painter = toQPainter(hdc);
    if (bg.is_root) {
        // TODO ?
        return;
    }
    painter->save();
    painter->setClipRect(toQRect(bg.clip_box));
    const QRegion horizontalMiddle(
        QRect(bg.border_box.x,
              bg.border_box.y + bg.border_radius.top_left_y,
              bg.border_box.width,
              bg.border_box.height - bg.border_radius.top_left_y - bg.border_radius.bottom_left_y));
    const QRegion horizontalTop(
        QRect(bg.border_box.x + bg.border_radius.top_left_x,
              bg.border_box.y,
              bg.border_box.width - bg.border_radius.top_left_x - bg.border_radius.top_right_x,
              bg.border_radius.top_left_y));
    const QRegion horizontalBottom(QRect(bg.border_box.x + bg.border_radius.bottom_left_x,
                                         bg.border_box.bottom() - bg.border_radius.bottom_left_y,
                                         bg.border_box.width - bg.border_radius.bottom_left_x
                                             - bg.border_radius.bottom_right_x,
                                         bg.border_radius.bottom_left_y));
    const QRegion topLeft(QRect(bg.border_box.left(),
                                bg.border_box.top(),
                                2 * bg.border_radius.top_left_x,
                                2 * bg.border_radius.top_left_y),
                          QRegion::Ellipse);
    const QRegion topRight(QRect(bg.border_box.right() - 2 * bg.border_radius.top_right_x,
                                 bg.border_box.top(),
                                 2 * bg.border_radius.top_right_x,
                                 2 * bg.border_radius.top_right_y),
                           QRegion::Ellipse);
    const QRegion bottomLeft(QRect(bg.border_box.left(),
                                   bg.border_box.bottom() - 2 * bg.border_radius.bottom_left_y,
                                   2 * bg.border_radius.bottom_left_x,
                                   2 * bg.border_radius.bottom_left_y),
                             QRegion::Ellipse);
    const QRegion bottomRight(QRect(bg.border_box.right() - 2 * bg.border_radius.bottom_right_x,
                                    bg.border_box.bottom() - 2 * bg.border_radius.bottom_right_y,
                                    2 * bg.border_radius.bottom_right_x,
                                    2 * bg.border_radius.bottom_right_y),
                              QRegion::Ellipse);
    const QRegion clipRegion = horizontalMiddle.united(horizontalTop)
                                   .united(horizontalBottom)
                                   .united(topLeft)
                                   .united(topRight)
                                   .united(bottomLeft)
                                   .united(bottomRight);
    painter->setClipRegion(clipRegion, Qt::IntersectClip);
    painter->setPen(Qt::NoPen);
    painter->setBrush(toQColor(bg.color));
    painter->drawRect(bg.border_box.x, bg.border_box.y, bg.border_box.width, bg.border_box.height);
    drawSelection(painter, toQRect(bg.border_box));
    if (!bg.image.empty()) {
        const QPixmap pixmap = getPixmap(QString::fromStdString(bg.image),
                                         QString::fromStdString(bg.baseurl));
        if (bg.repeat == litehtml::background_repeat_no_repeat) {
            painter->drawPixmap(QRect(bg.position_x,
                                      bg.position_y,
                                      bg.image_size.width,
                                      bg.image_size.height),
                                pixmap);
        } else if (bg.repeat == litehtml::background_repeat_repeat_x) {
            if (bg.image_size.width > 0) {
                int x = bg.border_box.left();
                while (x <= bg.border_box.right()) {
                    painter->drawPixmap(QRect(x,
                                              bg.border_box.top(),
                                              bg.image_size.width,
                                              bg.image_size.height),
                                        pixmap);
                    x += bg.image_size.width;
                }
            }
        } else {
            qWarning(log) << "unsupported background repeat" << bg.repeat;
        }
    }
    painter->restore();
}

void DocumentContainerPrivate::draw_borders(litehtml::uint_ptr hdc,
                                            const litehtml::borders &borders,
                                            const litehtml::position &draw_pos,
                                            bool root)
{
    Q_UNUSED(root)
    // TODO: special border styles
    auto painter = toQPainter(hdc);
    if (borders.top.style != litehtml::border_style_none
        && borders.top.style != litehtml::border_style_hidden) {
        painter->setPen(borderPen(borders.top));
        painter->drawLine(draw_pos.left() + borders.radius.top_left_x,
                          draw_pos.top(),
                          draw_pos.right() - borders.radius.top_right_x,
                          draw_pos.top());
        painter->drawArc(draw_pos.left(),
                         draw_pos.top(),
                         2 * borders.radius.top_left_x,
                         2 * borders.radius.top_left_y,
                         90 * 16,
                         90 * 16);
        painter->drawArc(draw_pos.right() - 2 * borders.radius.top_right_x,
                         draw_pos.top(),
                         2 * borders.radius.top_right_x,
                         2 * borders.radius.top_right_y,
                         0,
                         90 * 16);
    }
    if (borders.bottom.style != litehtml::border_style_none
        && borders.bottom.style != litehtml::border_style_hidden) {
        painter->setPen(borderPen(borders.bottom));
        painter->drawLine(draw_pos.left() + borders.radius.bottom_left_x,
                          draw_pos.bottom(),
                          draw_pos.right() - borders.radius.bottom_right_x,
                          draw_pos.bottom());
        painter->drawArc(draw_pos.left(),
                         draw_pos.bottom() - 2 * borders.radius.bottom_left_y,
                         2 * borders.radius.bottom_left_x,
                         2 * borders.radius.bottom_left_y,
                         180 * 16,
                         90 * 16);
        painter->drawArc(draw_pos.right() - 2 * borders.radius.bottom_right_x,
                         draw_pos.bottom() - 2 * borders.radius.bottom_right_y,
                         2 * borders.radius.bottom_right_x,
                         2 * borders.radius.bottom_right_y,
                         270 * 16,
                         90 * 16);
    }
    if (borders.left.style != litehtml::border_style_none
        && borders.left.style != litehtml::border_style_hidden) {
        painter->setPen(borderPen(borders.left));
        painter->drawLine(draw_pos.left(),
                          draw_pos.top() + borders.radius.top_left_y,
                          draw_pos.left(),
                          draw_pos.bottom() - borders.radius.bottom_left_y);
    }
    if (borders.right.style != litehtml::border_style_none
        && borders.right.style != litehtml::border_style_hidden) {
        painter->setPen(borderPen(borders.right));
        painter->drawLine(draw_pos.right(),
                          draw_pos.top() + borders.radius.top_right_y,
                          draw_pos.right(),
                          draw_pos.bottom() - borders.radius.bottom_right_y);
    }
}

void DocumentContainerPrivate::set_caption(const litehtml::tchar_t *caption)
{
    m_caption = QString::fromUtf8(caption);
}

void DocumentContainerPrivate::set_base_url(const litehtml::tchar_t *base_url)
{
    m_baseUrl = QString::fromUtf8(base_url);
}

void DocumentContainerPrivate::link(const std::shared_ptr<litehtml::document> &doc,
                                    const litehtml::element::ptr &el)
{
    // TODO
    qDebug(log) << "link";
    Q_UNUSED(doc)
    Q_UNUSED(el)
}

void DocumentContainerPrivate::on_anchor_click(const litehtml::tchar_t *url,
                                               const litehtml::element::ptr &el)
{
    Q_UNUSED(el)
    if (!m_blockLinks)
        m_linkCallback(resolveUrl(QString::fromUtf8(url), m_baseUrl));
}

void DocumentContainerPrivate::set_cursor(const litehtml::tchar_t *cursor)
{
    m_cursorCallback(toQCursor(QString::fromUtf8(cursor)));
}

void DocumentContainerPrivate::transform_text(litehtml::tstring &text, litehtml::text_transform tt)
{
    // TODO
    qDebug(log) << "transform_text";
    Q_UNUSED(text)
    Q_UNUSED(tt)
}

void DocumentContainerPrivate::import_css(litehtml::tstring &text,
                                          const litehtml::tstring &url,
                                          litehtml::tstring &baseurl)
{
    const QUrl actualUrl = resolveUrl(QString::fromStdString(url), QString::fromStdString(baseurl));
    const QString urlString = actualUrl.toString(QUrl::None);
    const int lastSlash = urlString.lastIndexOf('/');
    baseurl = urlString.left(lastSlash).toStdString();
    text = QString::fromUtf8(m_dataCallback(actualUrl)).toStdString();
}

void DocumentContainerPrivate::set_clip(const litehtml::position &pos,
                                        const litehtml::border_radiuses &bdr_radius,
                                        bool valid_x,
                                        bool valid_y)
{
    // TODO
    qDebug(log) << "set_clip";
    Q_UNUSED(pos)
    Q_UNUSED(bdr_radius)
    Q_UNUSED(valid_x)
    Q_UNUSED(valid_y)
}

void DocumentContainerPrivate::del_clip()
{
    // TODO
    qDebug(log) << "del_clip";
}

void DocumentContainerPrivate::get_client_rect(litehtml::position &client) const
{
    client = {m_clientRect.x(), m_clientRect.y(), m_clientRect.width(), m_clientRect.height()};
}

std::shared_ptr<litehtml::element> DocumentContainerPrivate::create_element(
    const litehtml::tchar_t *tag_name,
    const litehtml::string_map &attributes,
    const std::shared_ptr<litehtml::document> &doc)
{
    // TODO

    // We only handle checkbox here
    if (litehtml::t_strcasecmp("input", tag_name) != 0) {
        return {};
    }

    auto it = attributes.find("type");
    if (it == attributes.end() || it->second != "checkbox")
        return {};

    it = attributes.find("checked");
    const bool isChecked = it != attributes.end();

    auto checkBox = std::make_shared<checkbox>(m_document);
    checkBox->set_checked(isChecked);
    return checkBox;

    qDebug(log) << "create_element" << QString::fromUtf8(tag_name);

    Q_UNUSED(attributes)
    Q_UNUSED(doc)
    return {};
}

void DocumentContainerPrivate::get_media_features(litehtml::media_features &media) const
{
    media.type = litehtml::media_type_screen;
    // TODO
    qDebug(log) << "get_media_features";
}

void DocumentContainerPrivate::get_language(litehtml::tstring &language,
                                            litehtml::tstring &culture) const
{
    // TODO
    qDebug(log) << "get_language";
    Q_UNUSED(language)
    Q_UNUSED(culture)
}

void DocumentContainer::setPaintDevice(QPaintDevice *paintDevice)
{
    d->m_paintDevice = paintDevice;
}

void DocumentContainer::setScrollPosition(const QPoint &pos)
{
    d->m_scrollPosition = pos;
}

void DocumentContainer::setDocument(const QByteArray &data, DocumentContainerContext *context)
{
    d->m_pixmaps.clear();
    d->clearSelection();
    d->m_document = litehtml::document::createFromUTF8(data.constData(),
                                                       d.get(),
                                                       &context->d->context);
    d->buildIndex();
}

bool DocumentContainer::hasDocument() const
{
    return d->m_document.get();
}

void DocumentContainer::setBaseUrl(const QString &url)
{
    d->set_base_url(url.toUtf8().constData());
}

void DocumentContainer::render(int width, int height)
{
    d->m_clientRect = {0, 0, width, height};
    if (!d->m_document)
        return;
    d->m_document->render(width);
    d->updateSelection();
}

void DocumentContainer::draw(QPainter *painter, const QRect &clip)
{
    d->drawSelection(painter, clip);
    const QPoint pos = -d->m_scrollPosition;
    const litehtml::position clipRect = {clip.x(), clip.y(), clip.width(), clip.height()};
    d->m_document->draw(reinterpret_cast<litehtml::uint_ptr>(painter), pos.x(), pos.y(), &clipRect);
}

int DocumentContainer::documentWidth() const
{
    return d->m_document->width();
}

int DocumentContainer::documentHeight() const
{
    return d->m_document->height();
}

int DocumentContainer::anchorY(const QString &anchorName) const
{
    litehtml::element::ptr element = d->m_document->root()->select_one(
        QStringLiteral("#%1").arg(anchorName).toStdString());
    if (!element) {
        element = d->m_document->root()->select_one(
            QStringLiteral("[name=%1]").arg(anchorName).toStdString());
    }
    if (element)
        return element->get_placement().y;
    return -1;
}

QVector<QRect> DocumentContainer::mousePressEvent(const QPoint &documentPos,
                                                  const QPoint &viewportPos,
                                                  Qt::MouseButton button)
{
    if (!d->m_document || button != Qt::LeftButton)
        return {};
    QVector<QRect> redrawRects;
    // selection
    if (d->m_selection.isValid())
        redrawRects.append(d->m_selection.boundingRect());
    d->clearSelection();
    d->m_selection.selectionStartDocumentPos = documentPos;
    d->m_selection.startElem = deepest_child_at_point(d->m_document,
                                                      documentPos,
                                                      viewportPos,
                                                      d->m_selection.mode);
    // post to litehtml
    litehtml::position::vector redrawBoxes;
    if (d->m_document->on_lbutton_down(documentPos.x(),
                                       documentPos.y(),
                                       viewportPos.x(),
                                       viewportPos.y(),
                                       redrawBoxes)) {
        for (const litehtml::position &box : redrawBoxes)
            redrawRects.append(toQRect(box));
    }
    return redrawRects;
}

QVector<QRect> DocumentContainer::mouseMoveEvent(const QPoint &documentPos,
                                                 const QPoint &viewportPos)
{
    if (!d->m_document)
        return {};
    QVector<QRect> redrawRects;
    // selection
    if (d->m_selection.isSelecting
        || (!d->m_selection.selectionStartDocumentPos.isNull()
            && (d->m_selection.selectionStartDocumentPos - documentPos).manhattanLength()
                   >= kDragDistance
            && d->m_selection.startElem.element)) {
        const Selection::Element element = deepest_child_at_point(d->m_document,
                                                                  documentPos,
                                                                  viewportPos,
                                                                  d->m_selection.mode);
        if (element.element) {
            redrawRects.append(
                d->m_selection
                    .boundingRect() /*.adjusted(-1, -1, +1, +1)*/); // redraw old selection area
            d->m_selection.endElem = element;
            d->updateSelection();
            redrawRects.append(d->m_selection.boundingRect());
        }
        d->m_selection.isSelecting = true;
    }
    litehtml::position::vector redrawBoxes;
    if (d->m_document->on_mouse_over(documentPos.x(),
                                     documentPos.y(),
                                     viewportPos.x(),
                                     viewportPos.y(),
                                     redrawBoxes)) {
        for (const litehtml::position &box : redrawBoxes)
            redrawRects.append(toQRect(box));
    }
    return redrawRects;
}

QVector<QRect> DocumentContainer::mouseReleaseEvent(const QPoint &documentPos,
                                                    const QPoint &viewportPos,
                                                    Qt::MouseButton button)
{
    if (!d->m_document || button != Qt::LeftButton)
        return {};
    QVector<QRect> redrawRects;
    // selection
    d->m_selection.isSelecting = false;
    d->m_selection.selectionStartDocumentPos = {};
    if (d->m_selection.isValid())
        d->m_blockLinks = true;
    else
        d->clearSelection();
    litehtml::position::vector redrawBoxes;
    if (d->m_document->on_lbutton_up(documentPos.x(),
                                     documentPos.y(),
                                     viewportPos.x(),
                                     viewportPos.y(),
                                     redrawBoxes)) {
        for (const litehtml::position &box : redrawBoxes)
            redrawRects.append(toQRect(box));
    }
    d->m_blockLinks = false;
    return redrawRects;
}

QVector<QRect> DocumentContainer::mouseDoubleClickEvent(const QPoint &documentPos,
                                                        const QPoint &viewportPos,
                                                        Qt::MouseButton button)
{
    if (!d->m_document || button != Qt::LeftButton)
        return {};
    QVector<QRect> redrawRects;
    d->clearSelection();
    d->m_selection.mode = Selection::Mode::Word;
    const Selection::Element element = deepest_child_at_point(d->m_document,
                                                              documentPos,
                                                              viewportPos,
                                                              d->m_selection.mode);
    if (element.element) {
        d->m_selection.startElem = element;
        d->m_selection.endElem = d->m_selection.startElem;
        d->m_selection.isSelecting = true;
        d->updateSelection();
        if (d->m_selection.isValid())
            redrawRects.append(d->m_selection.boundingRect());
    } else {
        if (d->m_selection.isValid())
            redrawRects.append(d->m_selection.boundingRect());
        d->clearSelection();
    }
    return redrawRects;
}

QVector<QRect> DocumentContainer::leaveEvent()
{
    if (!d->m_document)
        return {};
    litehtml::position::vector redrawBoxes;
    if (d->m_document->on_mouse_leave(redrawBoxes)) {
        QVector<QRect> redrawRects;
        for (const litehtml::position &box : redrawBoxes)
            redrawRects.append(toQRect(box));
        return redrawRects;
    }
    return {};
}

QUrl DocumentContainer::linkAt(const QPoint &documentPos, const QPoint &viewportPos)
{
    if (!d->m_document)
        return {};
    const litehtml::element::ptr element = d->m_document->root()
                                               ->get_element_by_point(documentPos.x(),
                                                                      documentPos.y(),
                                                                      viewportPos.x(),
                                                                      viewportPos.y());
    if (!element)
        return {};
    const char *href = element->get_attr("href");
    if (href)
        return d->resolveUrl(QString::fromUtf8(href), d->m_baseUrl);
    return {};
}

QString DocumentContainer::caption() const
{
    return d->m_caption;
}

QString DocumentContainer::selectedText() const
{
    return d->m_selection.text;
}

// Helper function to get computed style properties from a litehtml element
static QString getElementStyles(const litehtml::element::ptr &element)
{
    if (!element)
        return QString();

    QStringList styles;

    // Get font family
    const char *fontFamily = element->get_style_property("font-family", true, nullptr);
    if (fontFamily && strlen(fontFamily) > 0) {
        styles << QStringLiteral("font-family: %1").arg(QString::fromUtf8(fontFamily));
    }

    // Get font size
    int fontSize = element->get_font_size();
    if (fontSize > 0) {
        styles << QStringLiteral("font-size: %1px").arg(fontSize);
    }

    // Get font weight
    const char *fontWeight = element->get_style_property("font-weight", true, nullptr);
    if (fontWeight && strlen(fontWeight) > 0 && strcmp(fontWeight, "normal") != 0) {
        styles << QStringLiteral("font-weight: %1").arg(QString::fromUtf8(fontWeight));
    }

    // Get font style
    const char *fontStyle = element->get_style_property("font-style", true, nullptr);
    if (fontStyle && strlen(fontStyle) > 0 && strcmp(fontStyle, "normal") != 0) {
        styles << QStringLiteral("font-style: %1").arg(QString::fromUtf8(fontStyle));
    }

    // Get text decoration
    const char *textDecoration = element->get_style_property("text-decoration", true, nullptr);
    if (textDecoration && strlen(textDecoration) > 0 && strcmp(textDecoration, "none") != 0) {
        styles << QStringLiteral("text-decoration: %1").arg(QString::fromUtf8(textDecoration));
    }

    // Get color
    litehtml::web_color color = element->get_color("color", true, litehtml::web_color());
    if (color.alpha > 0) {
        styles << QStringLiteral("color: rgb(%1, %2, %3)")
                      .arg(color.red)
                      .arg(color.green)
                      .arg(color.blue);
    }

    // Get background color
    const char *bgColor = element->get_style_property("background-color", false, nullptr);
    if (bgColor && strlen(bgColor) > 0 && strcmp(bgColor, "transparent") != 0) {
        litehtml::web_color bg = litehtml::web_color::from_string(bgColor, nullptr);
        if (bg.alpha > 0) {
            styles << QStringLiteral("background-color: rgb(%1, %2, %3)")
                          .arg(bg.red)
                          .arg(bg.green)
                          .arg(bg.blue);
        }
    }

    // Get text alignment
    const char *textAlign = element->get_style_property("text-align", true, nullptr);
    if (textAlign && strlen(textAlign) > 0 && strcmp(textAlign, "left") != 0) {
        styles << QStringLiteral("text-align: %1").arg(QString::fromUtf8(textAlign));
    }

    return styles.join(QStringLiteral("; "));
}

// Helper structure to track parent elements during traversal
struct ElementContext
{
    litehtml::element::ptr element;
    QString tagName;
    QString styles;
    bool opened = false;
};

// Helper function to serialize a single leaf element with its parent hierarchy
static void serializeLeafWithParents(const litehtml::element::ptr &leafElement,
                                     QVector<ElementContext> &parentStack,
                                     QString &html)
{
    if (!leafElement)
        return;

    // Build the path from root to this leaf
    litehtml::elements_vector leafPath = path(leafElement);

    // Find common ancestor with current stack
    int commonDepth = 0;
    const int minSize = std::min(static_cast<int>(parentStack.size()),
                                 static_cast<int>(leafPath.size()));
    for (int i = 0; i < minSize; ++i) {
        if (i >= parentStack.size() || parentStack[i].element != leafPath[i]) {
            break;
        }
        commonDepth = i + 1;
    }

    // Close tags that are no longer in the path
    for (int i = parentStack.size() - 1; i >= commonDepth; --i) {
        if (parentStack[i].opened && !parentStack[i].tagName.isEmpty()) {
            html += QStringLiteral("</") + parentStack[i].tagName + QStringLiteral(">");
        }
    }
    parentStack.resize(commonDepth);

    // Open new parent tags
    for (size_t i = commonDepth; i < leafPath.size(); ++i) {
        const litehtml::element::ptr &elem = leafPath[i];
        const char *tagName = elem->get_tagName();

        ElementContext ctx;
        ctx.element = elem;

        if (tagName && strlen(tagName) > 0) {
            QString tag = QString::fromUtf8(tagName);
            ctx.tagName = tag;

            // Skip html, head, body wrappers
            if (tag == QLatin1String("html") || tag == QLatin1String("head")
                || tag == QLatin1String("body")) {
                ctx.opened = false;
                parentStack.append(ctx);
                continue;
            }

            // Open tag
            html += QStringLiteral("<") + tag;

            // Get and add inline styles
            QString inlineStyles = getElementStyles(elem);
            if (!inlineStyles.isEmpty()) {
                html += QStringLiteral(" style=\"%1\"").arg(inlineStyles);
            }

            // Add certain important attributes
            if (tag == QLatin1String("a")) {
                const char *href = elem->get_attr("href");
                if (href && strlen(href) > 0) {
                    html += QStringLiteral(" href=\"%1\"").arg(QString::fromUtf8(href));
                }
            } else if (tag == QLatin1String("img")) {
                const char *src = elem->get_attr("src");
                if (src && strlen(src) > 0) {
                    html += QStringLiteral(" src=\"%1\"").arg(QString::fromUtf8(src));
                }
                const char *alt = elem->get_attr("alt");
                if (alt && strlen(alt) > 0) {
                    html += QStringLiteral(" alt=\"%1\"").arg(QString::fromUtf8(alt));
                }
            }

            html += QStringLiteral(">");
            ctx.opened = true;
        }

        parentStack.append(ctx);
    }

    // Add the text content of the leaf
    litehtml::tstring elemText;
    leafElement->get_text(elemText);
    if (!elemText.empty()) {
        QString text = QString::fromStdString(elemText);
        // HTML escape the text
        text.replace(QLatin1Char('&'), QLatin1String("&amp;"));
        text.replace(QLatin1Char('<'), QLatin1String("&lt;"));
        text.replace(QLatin1Char('>'), QLatin1String("&gt;"));
        html += text;
    }
}

QString DocumentContainer::selectedHtml() const
{
    const Selection &sel = d->m_selection;
    if (!sel.startElem.element || !sel.endElem.element) {
        return QString();
    }

    // Get ordered start and end elements (same logic as Selection::update)
    Selection::Element start;
    Selection::Element end;
    std::tie(start, end) = getStartAndEnd(sel.startElem, sel.endElem);

    QString bodyHtml;
    QVector<ElementContext> parentStack;

    // Process start element
    serializeLeafWithParents(start.element, parentStack, bodyHtml);

    // If start and end are different, traverse all elements between them
    if (start.element != end.element) {
        litehtml::element::ptr current = start.element;
        do {
            current = nextLeaf(current, end.element);
            if (current && current != end.element) {
                serializeLeafWithParents(current, parentStack, bodyHtml);
            }
        } while (current && current != end.element);

        // Process end element
        if (end.element) {
            serializeLeafWithParents(end.element, parentStack, bodyHtml);
        }
    }

    // Close all remaining open tags
    for (int i = parentStack.size() - 1; i >= 0; --i) {
        if (parentStack[i].opened && !parentStack[i].tagName.isEmpty()) {
            bodyHtml += QStringLiteral("</") + parentStack[i].tagName + QStringLiteral(">");
        }
    }

    // Wrap in proper HTML document structure
    QString html;
    html += QStringLiteral("<!DOCTYPE html><html><head><meta charset=\"UTF-8\"></head><body>");
    html += bodyHtml;
    html += QStringLiteral("</body></html>");

    return html;
}

void DocumentContainer::findText(const QString &text,
                                 QTextDocument::FindFlags flags,
                                 bool incremental,
                                 bool *wrapped,
                                 bool *success,
                                 QVector<QRect> *oldSelection,
                                 QVector<QRect> *newSelection)
{
    if (success)
        *success = false;
    if (oldSelection)
        oldSelection->clear();
    if (newSelection)
        newSelection->clear();
    if (!d->m_document)
        return;
    const bool backward = flags & QTextDocument::FindBackward;
    int startIndex = backward ? -1 : 0;
    if (d->m_selection.startElem.element && d->m_selection.endElem.element) { // selection
        // poor-man's incremental search starts at beginning of selection,
        // non-incremental at end (forward search) or beginning (backward search)
        Selection::Element start;
        Selection::Element end;
        std::tie(start, end) = getStartAndEnd(d->m_selection.startElem, d->m_selection.endElem);
        Selection::Element searchStart;
        if (incremental || backward) {
            if (start.index < 0) // fully selected
                searchStart = {firstLeaf(start.element, nullptr), 0, -1};
            else
                searchStart = start;
        } else {
            if (end.index < 0) // fully selected
                searchStart = {nextLeaf(end.element, nullptr), 0, -1};
            else
                searchStart = end;
        }
        const auto findInIndex = d->m_index.elementToIndex.find(searchStart.element);
        if (findInIndex == std::end(d->m_index.elementToIndex)) {
            qWarning() << "internal error: cannot find litehmtl element in index";
            return;
        }
        startIndex = findInIndex->second + searchStart.index;
        if (backward)
            --startIndex;
    }

    const auto fillXPos = [](const Selection::Element &e) {
        litehtml::tstring ttext;
        e.element->get_text(ttext);
        const QString text = QString::fromStdString(ttext);
        const QFont &font = toQFont(e.element->get_font());
        const QFontMetrics fm(font);
        return Selection::Element{e.element, e.index, fm.size(0, text.left(e.index)).width()};
    };

    QString term = QRegularExpression::escape(text);
    if (flags & QTextDocument::FindWholeWords)
        term = QStringLiteral("\\b%1\\b").arg(term);
    const QRegularExpression::PatternOptions patternOptions
        = (flags & QTextDocument::FindCaseSensitively) ? QRegularExpression::NoPatternOption
                                                       : QRegularExpression::CaseInsensitiveOption;
    const QRegularExpression expression(term, patternOptions);

    int foundIndex = backward ? d->m_index.text.lastIndexOf(expression, startIndex)
                              : d->m_index.text.indexOf(expression, startIndex);
    if (foundIndex < 0) { // wrap
        foundIndex = backward ? d->m_index.text.lastIndexOf(expression)
                              : d->m_index.text.indexOf(expression);
        if (wrapped && foundIndex >= 0)
            *wrapped = true;
    }
    if (foundIndex >= 0) {
        const Index::Entry startEntry = d->m_index.findElement(foundIndex);
        const Index::Entry endEntry = d->m_index.findElement(foundIndex + text.size());
        if (!startEntry.second || !endEntry.second) {
            qWarning() << "internal error: search ended up with nullptr elements";
            return;
        }
        if (oldSelection)
            *oldSelection = d->m_selection.selection;
        d->clearSelection();
        d->m_selection.startElem = fillXPos({startEntry.second, foundIndex - startEntry.first, -1});
        d->m_selection.endElem = fillXPos(
            {endEntry.second, int(foundIndex + text.size() - endEntry.first), -1});
        d->updateSelection();
        if (newSelection)
            *newSelection = d->m_selection.selection;
        if (success)
            *success = true;
        return;
    }
    return;
}

void DocumentContainer::setDefaultFont(const QFont &font)
{
    d->m_defaultFont = font;
    d->m_defaultFontFamilyName = d->m_defaultFont.family().toUtf8();
    // Since font family name and size are read only once, when parsing html,
    // we need to trigger the reparse of this info.
    if (d->m_document && d->m_document->root()) {
        d->m_document->root()->refresh_styles();
        d->m_document->root()->parse_styles();
    }
}

QFont DocumentContainer::defaultFont() const
{
    return d->m_defaultFont;
}

void DocumentContainer::setDataCallback(const DocumentContainer::DataCallback &callback)
{
    d->m_dataCallback = callback;
}

void DocumentContainer::setCursorCallback(const DocumentContainer::CursorCallback &callback)
{
    d->m_cursorCallback = callback;
}

void DocumentContainer::setLinkCallback(const DocumentContainer::LinkCallback &callback)
{
    d->m_linkCallback = callback;
}

void DocumentContainer::setPaletteCallback(const DocumentContainer::PaletteCallback &callback)
{
    d->m_paletteCallback = callback;
}

void DocumentContainer::setClipboardCallback(const DocumentContainer::ClipboardCallback &callback)
{
    d->m_clipboardCallback = callback;
}

static litehtml::element::ptr elementForY(int y, const litehtml::document::ptr &document)
{
    if (!document)
        return {};

    const std::function<litehtml::element::ptr(int, litehtml::element::ptr)> recursion =
        [&recursion](int y, const litehtml::element::ptr &element) {
            litehtml::element::ptr result;
            const int subY = y - element->get_position().y;
            if (subY <= 0)
                return element;
            for (int i = 0; i < int(element->get_children_count()); ++i) {
                const litehtml::element::ptr child = element->get_child(i);
                result = recursion(subY, child);
                if (result)
                    return result;
            }
            return result;
        };

    return recursion(y, document->root());
}

int DocumentContainer::withFixedElementPosition(int y, const std::function<void()> &action)
{
    const litehtml::element::ptr element = elementForY(y, d->m_document);
    action();
    if (element)
        return element->get_placement().y;
    return -1;
}

QPixmap DocumentContainerPrivate::getPixmap(const QString &imageUrl, const QString &baseUrl)
{
    const QUrl url = resolveUrl(imageUrl, baseUrl);
    if (!m_pixmaps.contains(url)) {
        qWarning(log) << "draw_background: pixmap not loaded for" << url;
        return {};
    }
    return m_pixmaps.value(url);
}

QString DocumentContainerPrivate::serifFont() const
{
    // TODO make configurable
    return {"Times New Roman"};
}

QString DocumentContainerPrivate::sansSerifFont() const
{
    // TODO make configurable
    return {"Arial"};
}

QString DocumentContainerPrivate::monospaceFont() const
{
    // TODO make configurable
    return {"Courier"};
}

QUrl DocumentContainerPrivate::resolveUrl(const QString &url, const QString &baseUrl) const
{
    // several cases:
    // full url: "https://foo.bar/blah.css"
    // relative path: "foo/bar.css"
    // server relative path: "/foo/bar.css"
    // net path: "//foo.bar/blah.css"
    // fragment only: "#foo-fragment"
    const QUrl qurl(url);
    if (qurl.scheme().isEmpty()) {
        if (url.startsWith('#')) // leave alone if just a fragment
            return qurl;
        const QUrl pageBaseUrl = QUrl(baseUrl.isEmpty() ? m_baseUrl : baseUrl);
        if (url.startsWith("//")) // net path
            return QUrl(pageBaseUrl.scheme() + ":" + url);
        QUrl serverUrl = QUrl(pageBaseUrl);
        serverUrl.setPath("");
        const QString actualBaseUrl = url.startsWith('/')
                                          ? serverUrl.toString(QUrl::FullyEncoded)
                                          : pageBaseUrl.toString(QUrl::FullyEncoded);
        QUrl resolvedUrl(actualBaseUrl + '/' + url);
        resolvedUrl.setPath(QDir::cleanPath(resolvedUrl.path(QUrl::FullyEncoded)));
        return resolvedUrl;
    }
    return qurl;
}

Index::Entry Index::findElement(int index) const
{
    const auto upper = std::upper_bound(std::begin(indexToElement),
                                        std::end(indexToElement),
                                        Entry{index, {}},
                                        [](const Entry &a, const Entry &b) {
                                            return a.first < b.first;
                                        });
    if (upper == std::begin(indexToElement)) // should not happen for index >= 0
        return {-1, {}};
    return *(upper - 1);
}

DocumentContainerContext::DocumentContainerContext()
    : d(new DocumentContainerContextPrivate)
{}

DocumentContainerContext::~DocumentContainerContext() = default;

void DocumentContainerContext::setMasterStyleSheet(const QString &css)
{
    d->context.load_master_stylesheet(css.toUtf8().constData());
}
