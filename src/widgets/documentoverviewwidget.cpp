#include "documentoverviewwidget.h"

#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPlainTextEdit>
#include <QResizeEvent>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>
#include <QTimer>
#include <QWheelEvent>
#include <QtGlobal>

namespace {
constexpr int kMargin = 4;
constexpr int kMaximumColumns = 120;
constexpr int kUpdateDelay = 300;

bool isAtxHeading(const QString &text, int firstCharacter) {
    if (firstCharacter < 0 || firstCharacter > 3 || text.at(firstCharacter) != QLatin1Char('#')) {
        return false;
    }

    int index = firstCharacter;
    while (index < text.size() && index - firstCharacter < 6 &&
           text.at(index) == QLatin1Char('#')) {
        ++index;
    }

    return index > firstCharacter && (index == text.size() || text.at(index).isSpace());
}

template <typename FormatRanges>
QColor characterColor(const FormatRanges &formats, int position, const QColor &defaultColor) {
    for (const QTextLayout::FormatRange &range : formats) {
        if (position >= range.start && position < range.start + range.length &&
            range.format.foreground().style() != Qt::NoBrush) {
            return range.format.foreground().color();
        }
    }

    return defaultColor;
}
}    // namespace

DocumentOverviewWidget::DocumentOverviewWidget(QPlainTextEdit *noteTextEdit,
                                               QPlainTextEdit *encryptedNoteTextEdit,
                                               QWidget *parent)
    : QWidget(parent),
      _noteTextEdit(noteTextEdit),
      _encryptedNoteTextEdit(encryptedNoteTextEdit),
      _updateTimer(new QTimer(this)) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    setMinimumWidth(80);
    setToolTip(tr("Document overview: click or drag to navigate"));
    setAccessibleName(tr("Document overview"));

    _updateTimer->setInterval(kUpdateDelay);
    _updateTimer->setSingleShot(true);
    connect(_updateTimer, &QTimer::timeout, this,
            &DocumentOverviewWidget::invalidateRepresentation);

    connectTextEdit(noteTextEdit);
    connectTextEdit(encryptedNoteTextEdit);
}

QSize DocumentOverviewWidget::sizeHint() const { return QSize(140, 400); }

void DocumentOverviewWidget::connectTextEdit(QPlainTextEdit *textEdit) {
    if (textEdit == nullptr) {
        return;
    }

    textEdit->installEventFilter(this);
    connect(textEdit->document(), &QTextDocument::contentsChanged, this,
            &DocumentOverviewWidget::scheduleRepresentationUpdate);
    connect(textEdit, &QPlainTextEdit::cursorPositionChanged, this,
            static_cast<void (QWidget::*)()>(&QWidget::update));

    QScrollBar *scrollBar = textEdit->verticalScrollBar();
    connect(scrollBar, &QScrollBar::valueChanged, this,
            static_cast<void (QWidget::*)()>(&QWidget::update));
    connect(scrollBar, &QScrollBar::rangeChanged, this,
            &DocumentOverviewWidget::invalidateRepresentation);
}

QPlainTextEdit *DocumentOverviewWidget::activeTextEdit() const {
    if (_noteTextEdit != nullptr && !_noteTextEdit->isHidden()) {
        return _noteTextEdit;
    }

    return _encryptedNoteTextEdit;
}

bool DocumentOverviewWidget::eventFilter(QObject *watched, QEvent *event) {
    if ((watched == _noteTextEdit || watched == _encryptedNoteTextEdit) &&
        (event->type() == QEvent::Show || event->type() == QEvent::Hide ||
         event->type() == QEvent::Resize || event->type() == QEvent::FontChange ||
         event->type() == QEvent::PaletteChange)) {
        invalidateRepresentation();
    }

    return QWidget::eventFilter(watched, event);
}

void DocumentOverviewWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.fillRect(rect(), palette().color(QPalette::Base));

    QPlainTextEdit *textEdit = activeTextEdit();
    if (textEdit == nullptr || width() <= kMargin * 2 || height() <= kMargin * 2) {
        return;
    }

    const QSize representationSize(width() - kMargin * 2, height() - kMargin * 2);
    if (_representationDirty || _representedTextEdit != textEdit ||
        _representation.size() != representationSize) {
        rebuildRepresentation(textEdit, representationSize);
    }
    painter.drawPixmap(kMargin, kMargin, _representation);

    const int contentHeight = _documentHeight;
    const int contentWidth = representationSize.width();
    const int visualLineCount = qMax(1, _visualLines.size());
    const QTextCursor firstVisibleCursor = textEdit->cursorForPosition(QPoint(0, 0));
    const QTextCursor lastVisibleCursor =
        textEdit->cursorForPosition(QPoint(0, qMax(0, textEdit->viewport()->height() - 1)));
    const int firstVisibleLine = visualLineForCursor(firstVisibleCursor);
    const int lastVisibleLine = qMax(firstVisibleLine, visualLineForCursor(lastVisibleCursor));
    qreal viewportTop = qreal(firstVisibleLine) / visualLineCount * contentHeight;
    qreal viewportHeight =
        qreal(lastVisibleLine - firstVisibleLine + 1) / visualLineCount * contentHeight;
    viewportHeight = qMin<qreal>(contentHeight, qMax<qreal>(1.0, viewportHeight));
    viewportTop = qMin<qreal>(viewportTop, contentHeight - viewportHeight);

    QColor outsideViewportColor = palette().color(QPalette::Base);
    outsideViewportColor.setAlpha(110);
    painter.fillRect(QRectF(kMargin, kMargin, contentWidth, viewportTop), outsideViewportColor);
    painter.fillRect(QRectF(kMargin, kMargin + viewportTop + viewportHeight, contentWidth,
                            contentHeight - viewportTop - viewportHeight),
                     outsideViewportColor);

    QColor viewportColor = palette().color(QPalette::Highlight);
    viewportColor.setAlpha(55);
    QColor viewportBorderColor = palette().color(QPalette::Highlight);
    viewportBorderColor.setAlpha(230);
    const QRectF viewportRect(kMargin - 1, kMargin + viewportTop, contentWidth + 2, viewportHeight);
    painter.fillRect(viewportRect, viewportColor);
    QPen viewportPen(viewportBorderColor);
    viewportPen.setWidth(2);
    painter.setPen(viewportPen);
    painter.drawRect(viewportRect.adjusted(0, 0, -1, -1));

    const int cursorVisualLine = visualLineForCursor(textEdit->textCursor());
    const int cursorY =
        kMargin +
        qMin(contentHeight - 1, int(qreal(cursorVisualLine) / visualLineCount * contentHeight));
    painter.drawLine(kMargin, cursorY, width() - kMargin, cursorY);

    setToolTip(tr("Document overview: visible lines %1-%2; click or drag to navigate")
                   .arg(firstVisibleCursor.blockNumber() + 1)
                   .arg(lastVisibleCursor.blockNumber() + 1));
}

void DocumentOverviewWidget::invalidateRepresentation() {
    _representationDirty = true;
    update();
}

void DocumentOverviewWidget::scheduleRepresentationUpdate() { _updateTimer->start(); }

void DocumentOverviewWidget::rebuildRepresentation(QPlainTextEdit *textEdit, const QSize &size) {
    _representation = QPixmap(size);
    _representation.fill(Qt::transparent);
    _representedTextEdit = textEdit;
    _representationDirty = false;

    QPainter painter(&_representation);
    QTextDocument *document = textEdit->document();
    const int contentWidth = size.width();
    const qreal columnWidth = qreal(contentWidth) / kMaximumColumns;
    const QColor textColor = palette().color(QPalette::Text);
    QColor headingColor = palette().color(QPalette::Highlight);
    headingColor.setAlpha(230);

    _visualLines.clear();
    _blockFirstVisualLines.fill(-1, document->blockCount());
    _blockVisualLineCounts.fill(0, document->blockCount());

    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        if (!block.isVisible()) {
            continue;
        }

        QTextLayout *layout = block.layout();
        const int lineCount = qMax(1, layout == nullptr ? 0 : layout->lineCount());
        _blockFirstVisualLines[block.blockNumber()] = _visualLines.size();
        _blockVisualLineCounts[block.blockNumber()] = lineCount;
        for (int lineNumber = 0; lineNumber < lineCount; ++lineNumber) {
            const QTextLine line = layout == nullptr ? QTextLine() : layout->lineAt(lineNumber);
            _visualLines.append({block.blockNumber(), line.isValid() ? line.textStart() : 0,
                                 line.isValid() ? line.textLength() : block.length() - 1});
        }
    }

    if (_visualLines.isEmpty()) {
        _visualLines.append({0, 0, 0});
    }

    _documentHeight = qMin(size.height(), qMax(1, _visualLines.size() * 2));

    int lastPaintedY = -1;
    for (int visualLineNumber = 0; visualLineNumber < _visualLines.size(); ++visualLineNumber) {
        const VisualLine &visualLine = _visualLines.at(visualLineNumber);
        const QTextBlock block = document->findBlockByNumber(visualLine.blockNumber);
        const QString text = block.text();
        int firstCharacter = visualLine.textStart;
        while (firstCharacter < text.size() && text.at(firstCharacter).isSpace()) {
            ++firstCharacter;
        }
        const bool heading = firstCharacter < text.size() && isAtxHeading(text, firstCharacter);
        const int y = qMin(_documentHeight - 1,
                           int(qreal(visualLineNumber) / _visualLines.size() * _documentHeight));
        if (y == lastPaintedY && !heading) {
            continue;
        }
        lastPaintedY = y;

        QTextLayout *layout = block.layout();
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
        const auto formats = layout == nullptr ? decltype(layout->formats())() : layout->formats();
#else
        const auto formats = layout == nullptr ? decltype(layout->additionalFormats())()
                                               : layout->additionalFormats();
#endif
        int column = 0;
        const int end = qMin(text.size(), visualLine.textStart + visualLine.textLength);
        for (int position = visualLine.textStart; position < end && column < kMaximumColumns;
             ++position) {
            const QChar character = text.at(position);
            if (character == QLatin1Char('\t')) {
                column += 4 - column % 4;
                continue;
            }
            if (!character.isSpace()) {
                QColor color =
                    heading ? headingColor : characterColor(formats, position, textColor);
                color.setAlpha(heading ? 230 : 170);
                painter.setPen(color);
                const int x = qMin(contentWidth - 1, int(column * columnWidth));
                painter.drawPoint(x, y);
                if (heading && y + 1 < _documentHeight) {
                    painter.drawPoint(x, y + 1);
                }
            }
            ++column;
        }
    }
}

int DocumentOverviewWidget::visualLineForCursor(const QTextCursor &cursor) const {
    const int blockNumber = cursor.blockNumber();
    if (blockNumber < 0 || blockNumber >= _blockFirstVisualLines.size()) {
        return 0;
    }

    int firstVisualLine = _blockFirstVisualLines.at(blockNumber);
    if (firstVisualLine < 0) {
        for (int visibleBlock = blockNumber - 1; visibleBlock >= 0; --visibleBlock) {
            if (_blockFirstVisualLines.at(visibleBlock) >= 0) {
                return _blockFirstVisualLines.at(visibleBlock) +
                       _blockVisualLineCounts.at(visibleBlock) - 1;
            }
        }
        return 0;
    }

    QTextLayout *layout = cursor.block().layout();
    if (layout == nullptr || layout->lineCount() == 0) {
        return firstVisualLine;
    }

    const QTextLine line = layout->lineForTextPosition(cursor.positionInBlock());
    const int lineNumber = line.isValid() ? line.lineNumber() : 0;
    return qBound(0, firstVisualLine + lineNumber, _visualLines.size() - 1);
}

void DocumentOverviewWidget::resizeEvent(QResizeEvent *event) {
    invalidateRepresentation();
    QWidget::resizeEvent(event);
}

void DocumentOverviewWidget::scrollToPosition(int y) {
    QPlainTextEdit *textEdit = activeTextEdit();
    if (textEdit == nullptr) {
        return;
    }

    QScrollBar *scrollBar = textEdit->verticalScrollBar();
    const int contentHeight = qMax(1, _documentHeight);
    const qreal ratio = qBound<qreal>(0.0, qreal(y - kMargin) / contentHeight, 1.0);
    const int scrollExtent = scrollBar->maximum() + scrollBar->pageStep();
    const int value = int(ratio * scrollExtent - scrollBar->pageStep() / 2.0);
    scrollBar->setValue(qBound(scrollBar->minimum(), value, scrollBar->maximum()));
}

void DocumentOverviewWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        scrollToPosition(event->pos().y());
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void DocumentOverviewWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        scrollToPosition(event->pos().y());
        event->accept();
        return;
    }

    QWidget::mouseMoveEvent(event);
}

void DocumentOverviewWidget::wheelEvent(QWheelEvent *event) {
    QPlainTextEdit *textEdit = activeTextEdit();
    if (textEdit == nullptr) {
        QWidget::wheelEvent(event);
        return;
    }

    QScrollBar *scrollBar = textEdit->verticalScrollBar();
    const int pixelDelta = event->pixelDelta().y();
    if (pixelDelta != 0) {
        scrollBar->setValue(scrollBar->value() - pixelDelta);
    } else {
        _wheelAngleDelta += event->angleDelta().y();
        const int steps = _wheelAngleDelta / 120;
        _wheelAngleDelta %= 120;
        scrollBar->setValue(scrollBar->value() - steps * scrollBar->singleStep() * 3);
    }
    event->accept();
}
