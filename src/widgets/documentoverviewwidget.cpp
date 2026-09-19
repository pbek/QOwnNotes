#include "documentoverviewwidget.h"

#include <QApplication>
#include <QEvent>
#include <QFrame>
#include <QGuiApplication>
#include <QHideEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPlainTextEdit>
#include <QResizeEvent>
#include <QScopedValueRollback>
#include <QScreen>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextLayout>
#include <QTimer>
#include <QWheelEvent>
#include <QWindow>
#include <QtGlobal>

#include "libraries/qmarkdowntextedit/qmarkdowntextedit.h"

namespace {
constexpr int kMargin = 4;
constexpr int kMaximumColumns = 120;
constexpr int kUpdateDelay = 300;
constexpr int kPreviewDelay = 250;
constexpr qreal kPreviewScale = 0.8;

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

class DocumentOverviewPreview : public QFrame {
   public:
    explicit DocumentOverviewPreview(DocumentOverviewWidget *overview)
        : QFrame(overview, Qt::ToolTip | Qt::FramelessWindowHint | Qt::BypassWindowManagerHint),
          _overview(overview) {
        setAttribute(Qt::WA_ShowWithoutActivating);
        setFrameStyle(QFrame::StyledPanel);
        setLineWidth(1);
    }

   protected:
    void paintEvent(QPaintEvent *event) override {
        QFrame::paintEvent(event);
        QPainter painter(this);
        _overview->paintPreview(painter, contentsRect());
    }

   private:
    DocumentOverviewWidget *_overview;
};

DocumentOverviewWidget::DocumentOverviewWidget(QPlainTextEdit *noteTextEdit,
                                               QPlainTextEdit *encryptedNoteTextEdit,
                                               QWidget *parent)
    : QWidget(parent),
      _noteTextEdit(noteTextEdit),
      _encryptedNoteTextEdit(encryptedNoteTextEdit),
      _preview(new DocumentOverviewPreview(this)),
      _previewTimer(new QTimer(this)),
      _updateTimer(new QTimer(this)) {
    setAttribute(Qt::WA_OpaquePaintEvent);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
    setMouseTracking(true);
    setMinimumWidth(80);
    setToolTip(tr("Document overview: click or drag to navigate"));
    setAccessibleName(tr("Document overview"));

    _updateTimer->setInterval(kUpdateDelay);
    _updateTimer->setSingleShot(true);
    connect(_updateTimer, &QTimer::timeout, this,
            &DocumentOverviewWidget::invalidateRepresentation);

    _previewTimer->setInterval(kPreviewDelay);
    _previewTimer->setSingleShot(true);
    connect(_previewTimer, &QTimer::timeout, this, &DocumentOverviewWidget::showPreview);

    connectTextEdit(noteTextEdit);
    connectTextEdit(encryptedNoteTextEdit);
    qApp->installEventFilter(this);
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
    if (event->type() == QEvent::WindowDeactivate) {
        hidePreview();
    }

    if ((watched == _noteTextEdit || watched == _encryptedNoteTextEdit) &&
        (event->type() == QEvent::Show || event->type() == QEvent::Hide ||
         event->type() == QEvent::Resize || event->type() == QEvent::FontChange ||
         event->type() == QEvent::PaletteChange)) {
        invalidateRepresentation();
    }

    return QWidget::eventFilter(watched, event);
}

void DocumentOverviewWidget::hideEvent(QHideEvent *event) {
    hidePreview();
    QWidget::hideEvent(event);
}

void DocumentOverviewWidget::leaveEvent(QEvent *event) {
    hidePreview();
    QWidget::leaveEvent(event);
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

    if (_preview->isVisible() && _previewVisualLine >= 0) {
        QColor previewLineColor = palette().color(QPalette::Highlight);
        previewLineColor.setAlpha(230);
        painter.setPen(QPen(previewLineColor, 2));
        const int previewY =
            kMargin + int(qreal(_previewVisualLine) / visualLineCount * contentHeight);
        painter.drawLine(kMargin, previewY, width() - kMargin, previewY);
    }

    setToolTip(tr("Document overview: visible lines %1-%2; click or drag to navigate")
                   .arg(firstVisibleCursor.blockNumber() + 1)
                   .arg(lastVisibleCursor.blockNumber() + 1));
}

void DocumentOverviewWidget::invalidateRepresentation() {
    _representationDirty = true;
    update();
    if (_preview->isVisible() && !_updatingPreview) {
        QTimer::singleShot(0, this, [this]() {
            if (_preview->isVisible()) {
                showPreview();
            }
        });
    }
}

void DocumentOverviewWidget::scheduleRepresentationUpdate() {
    _previewHighlightedBlocks.clear();
    _updateTimer->start();
}

void DocumentOverviewWidget::hidePreview() {
    const bool wasVisible = _preview->isVisible();
    const bool wasPending = _previewTimer->isActive();
    if (!wasVisible && !wasPending && _previewVisualLine < 0) {
        return;
    }

    _previewTimer->stop();
    _preview->hide();
    _previewVisualLine = -1;
    if (wasVisible) {
        update();
    }
}

void DocumentOverviewWidget::showPreview() {
    QPlainTextEdit *textEdit = activeTextEdit();
    if (textEdit == nullptr || _previewVisualLine < 0 || !underMouse() ||
        !window()->isActiveWindow()) {
        hidePreview();
        return;
    }

    const QScopedValueRollback<bool> updatingPreview(_updatingPreview, true);

    const QSize representationSize(width() - kMargin * 2, height() - kMargin * 2);
    if (_representationDirty || _representedTextEdit != textEdit ||
        _representation.size() != representationSize) {
        rebuildRepresentation(textEdit, representationSize);
    }
    _previewVisualLine = visualLineAtPosition(_previewAnchorY);
    if (_previewVisualLine < 0) {
        hidePreview();
        return;
    }

    const QPoint anchor = mapToGlobal(QPoint(0, _previewAnchorY));
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    QScreen *screen = QGuiApplication::screenAt(anchor);
#else
    QScreen *screen = window()->windowHandle() == nullptr ? QGuiApplication::primaryScreen()
                                                          : window()->windowHandle()->screen();
#endif
    const QRect availableGeometry = screen == nullptr ? QRect() : screen->availableGeometry();
    int previewWidth = qBound(360, textEdit->viewport()->width() * 3 / 4, 900);
    int previewHeight = qBound(120, textEdit->viewport()->height() / 4, 260);
    if (availableGeometry.isValid()) {
        previewWidth = qMin(previewWidth, availableGeometry.width());
        previewHeight = qMin(previewHeight, availableGeometry.height());
    }
    _preview->resize(previewWidth, previewHeight);

    if (highlightPreviewBlocks(textEdit, previewHeight)) {
        rebuildRepresentation(textEdit, representationSize);
        _previewVisualLine = visualLineAtPosition(_previewAnchorY);
        if (_previewVisualLine < 0) {
            hidePreview();
            return;
        }
    }

    const int leftX = mapToGlobal(QPoint(0, 0)).x() - previewWidth - 8;
    const int rightX = mapToGlobal(QPoint(width(), 0)).x() + 8;
    int previewX = leftX;
    int previewY = anchor.y() - previewHeight / 2;
    if (availableGeometry.isValid()) {
        if (leftX < availableGeometry.left() &&
            rightX + previewWidth <= availableGeometry.right() + 1) {
            previewX = rightX;
        }
        previewX = qBound(availableGeometry.left(), previewX,
                          availableGeometry.right() - previewWidth + 1);
        previewY = qBound(availableGeometry.top(), previewY,
                          availableGeometry.bottom() - previewHeight + 1);
    }

    _preview->move(previewX, previewY);
    _preview->raise();
    _preview->show();
    _preview->update();
    update();
}

bool DocumentOverviewWidget::highlightPreviewBlocks(QPlainTextEdit *textEdit, int previewHeight) {
    auto *markdownTextEdit = qobject_cast<QMarkdownTextEdit *>(textEdit);
    if (markdownTextEdit == nullptr || markdownTextEdit->highlighter() == nullptr ||
        _visualLines.isEmpty()) {
        return false;
    }

    if (_previewHighlightedTextEdit != textEdit) {
        _previewHighlightedTextEdit = textEdit;
        _previewHighlightedBlocks.clear();
    }

    const int previewLineCount =
        qMax(1, int(previewHeight / kPreviewScale) / qMax(1, textEdit->fontMetrics().height()));
    const int firstVisualLine = qMax(0, _previewVisualLine - previewLineCount / 2 - 2);
    const int lastVisualLine =
        qMin(_visualLines.size() - 1, _previewVisualLine + previewLineCount / 2 + 2);
    const int firstBlockNumber = _visualLines.at(firstVisualLine).blockNumber;
    const int lastBlockNumber = _visualLines.at(lastVisualLine).blockNumber;
    QTextBlock firstBlock = textEdit->document()->findBlockByNumber(firstBlockNumber);
    while (firstBlock.previous().isValid() && firstBlock.previous().userState() < 0) {
        firstBlock = firstBlock.previous();
    }

    bool changed = false;
    for (QTextBlock block = firstBlock; block.isValid() && block.blockNumber() <= lastBlockNumber;
         block = block.next()) {
        const int blockNumber = block.blockNumber();
        if (!_previewHighlightedBlocks.contains(blockNumber)) {
            // Mark the block first because rehighlighting can synchronously relayout the editor.
            _previewHighlightedBlocks.insert(blockNumber);
            markdownTextEdit->highlighter()->rehighlightBlock(block);
            changed = true;
        }

        if (blockNumber >= firstBlockNumber) {
            QTextLayout *layout = block.layout();
            const int oldLineCount = layout == nullptr ? 0 : layout->lineCount();
            textEdit->document()->documentLayout()->blockBoundingRect(block);
            layout = block.layout();
            changed = changed || (layout != nullptr && layout->lineCount() != oldLineCount);
        }
    }

    return changed;
}

void DocumentOverviewWidget::paintPreview(QPainter &painter, const QRect &rect) const {
    QPlainTextEdit *textEdit = activeTextEdit();
    if (textEdit == nullptr || _previewVisualLine < 0 || _visualLines.isEmpty()) {
        return;
    }

    painter.fillRect(rect, textEdit->palette().color(QPalette::Base));
    painter.setClipRect(rect);
    painter.scale(kPreviewScale, kPreviewScale);

    const int margin = 8;
    const int logicalHeight = int(rect.height() / kPreviewScale);
    QTextDocument *document = textEdit->document();

#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    const int lineNumberTextWidth =
        textEdit->fontMetrics().horizontalAdvance(QString::number(document->blockCount()));
#else
    const int lineNumberTextWidth =
        textEdit->fontMetrics().width(QString::number(document->blockCount()));
#endif
    const int lineNumberWidth = lineNumberTextWidth + margin * 2;
    const auto lineAt = [this, document](int visualLineNumber) {
        const VisualLine &visualLine = _visualLines.at(visualLineNumber);
        QTextLayout *layout = document->findBlockByNumber(visualLine.blockNumber).layout();
        return layout == nullptr ? QTextLine() : layout->lineForTextPosition(visualLine.textStart);
    };
    const auto lineHeight = [&lineAt, textEdit](int visualLineNumber) {
        const QTextLine line = lineAt(visualLineNumber);
        return qMax(1,
                    line.isValid() ? int(line.height() + 0.5) : textEdit->fontMetrics().height());
    };

    int startVisualLine = _previewVisualLine;
    int y = logicalHeight / 2 - lineHeight(_previewVisualLine) / 2;
    while (startVisualLine > 0 && y > margin) {
        --startVisualLine;
        y -= lineHeight(startVisualLine);
    }

    int previousBlockNumber = -1;
    for (int visualLineNumber = startVisualLine;
         visualLineNumber < _visualLines.size() && y < logicalHeight; ++visualLineNumber) {
        const VisualLine &visualLine = _visualLines.at(visualLineNumber);
        const QTextBlock block = document->findBlockByNumber(visualLine.blockNumber);
        const QTextLine line = lineAt(visualLineNumber);
        const int currentLineHeight = lineHeight(visualLineNumber);

        if (visualLineNumber == _previewVisualLine) {
            QColor highlightColor = textEdit->palette().color(QPalette::Highlight);
            highlightColor.setAlpha(55);
            painter.fillRect(QRect(0, y, int(rect.width() / kPreviewScale), currentLineHeight),
                             highlightColor);
        }

        painter.setPen(textEdit->palette().color(QPalette::Mid));
        if (visualLine.blockNumber != previousBlockNumber) {
            painter.drawText(QRect(margin, y, lineNumberWidth - margin * 2, currentLineHeight),
                             Qt::AlignRight | Qt::AlignVCenter,
                             QString::number(visualLine.blockNumber + 1));
            previousBlockNumber = visualLine.blockNumber;
        }

        if (line.isValid()) {
            line.draw(&painter,
                      QPointF(lineNumberWidth - line.position().x(), y - line.position().y()));
        } else {
            painter.setPen(textEdit->palette().color(QPalette::Text));
            painter.drawText(QPointF(lineNumberWidth, y + textEdit->fontMetrics().ascent()),
                             block.text());
        }
        y += currentLineHeight;
    }
}

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

int DocumentOverviewWidget::visualLineAtPosition(int y) const {
    if (_visualLines.isEmpty() || y < kMargin || y >= kMargin + _documentHeight) {
        return -1;
    }

    return qBound(0, int(qreal(y - kMargin) / qMax(1, _documentHeight) * _visualLines.size()),
                  _visualLines.size() - 1);
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
    hidePreview();
    if (event->button() == Qt::LeftButton) {
        scrollToPosition(event->pos().y());
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void DocumentOverviewWidget::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        hidePreview();
        scrollToPosition(event->pos().y());
        event->accept();
        return;
    }

    const int previewVisualLine = visualLineAtPosition(event->pos().y());
    if (previewVisualLine < 0) {
        hidePreview();
    } else {
        const bool previewLineChanged = _previewVisualLine != previewVisualLine;
        _previewVisualLine = previewVisualLine;
        _previewAnchorY = event->pos().y();
        if (_preview->isVisible() && previewLineChanged) {
            showPreview();
        } else if (!_preview->isVisible() && !_previewTimer->isActive()) {
            _previewTimer->start();
        }
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
