#include "timepickerbutton.h"

#include <QApplication>
#include <QButtonGroup>
#include <QDateTimeEdit>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLocale>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScreen>
#include <QVBoxLayout>
#include <QVariantAnimation>
#include <QWheelEvent>
#include <QtMath>
#include <cmath>

namespace {
QColor withAlpha(QColor color, qreal alpha) {
    color.setAlphaF(alpha);
    return color;
}

QString cssColor(const QColor &color) {
    return QStringLiteral("rgba(%1, %2, %3, %4)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(color.alpha());
}

QFont scaledFont(const QFont &font, qreal factor) {
    QFont result = font;

    if (result.pointSizeF() > 0) {
        result.setPointSizeF(result.pointSizeF() * factor);
    } else if (result.pixelSize() > 0) {
        result.setPixelSize(qMax(6, qRound(result.pixelSize() * factor)));
    }

    return result;
}

QString twoDigits(int value) { return QStringLiteral("%1").arg(value, 2, 10, QLatin1Char('0')); }

QPointF mousePosition(const QMouseEvent *event) {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

// Returns the clockwise angle in degrees from the 12 o'clock position
qreal clockAngle(const QPointF &vector) {
    qreal angle = qRadiansToDegrees(qAtan2(vector.x(), -vector.y()));
    return angle < 0 ? angle + 360 : angle;
}

QPointF pointOnCircle(const QPointF &center, qreal angle, qreal radius) {
    const qreal radians = qDegreesToRadians(angle);
    return center + QPointF(qSin(radians) * radius, -qCos(radians) * radius);
}

// The outer ring shows the hours 1 to 12, the inner ring 00 and 13 to 23
bool isInnerRingHour(int hour) { return hour == 0 || hour > 12; }
}    // namespace

ClockDialWidget::ClockDialWidget(QWidget *parent) : QWidget(parent) {
    setObjectName(QStringLiteral("clockDialWidget"));
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setAccessibleName(tr("Clock face"));
    setToolTip(
        tr("Click or drag to pick a value, use the mouse wheel or the arrow keys for fine "
           "adjustments and Tab to switch between hours and minutes"));

    _animation = new QVariantAnimation(this);
    _animation->setStartValue(0.0);
    _animation->setEndValue(1.0);
    _animation->setDuration(220);
    _animation->setEasingCurve(QEasingCurve::OutCubic);
    connect(_animation, &QVariantAnimation::valueChanged, this, [this]() { update(); });
    connect(_animation, &QVariantAnimation::finished, this, [this]() {
        _modeAnimating = false;
        update();
    });
}

QTime ClockDialWidget::time() const { return _time; }

void ClockDialWidget::setTime(const QTime &time, bool animated) {
    const QTime newTime = time.isValid() ? QTime(time.hour(), time.minute()) : QTime(0, 0);

    if (newTime == _time) {
        return;
    }

    beginTransition(animated);
    _modeAnimating = false;
    _time = newTime;
    update();
    emit timeChanged(_time);
}

ClockDialWidget::Mode ClockDialWidget::mode() const { return _mode; }

void ClockDialWidget::setMode(Mode mode, bool animated) {
    if (mode == _mode) {
        return;
    }

    beginTransition(animated);
    _modeAnimating = _animation->state() == QAbstractAnimation::Running;
    _mode = mode;
    _hoveredValue = -1;
    update();
    emit modeChanged(_mode);
}

int ClockDialWidget::valueAt(const QPointF &position) const {
    const QPointF vector = position - dialCenter();
    const qreal angle = clockAngle(vector);

    if (_mode == Mode::Hours) {
        const int index = qRound(angle / 30) % 12;
        const qreal distance = qSqrt(QPointF::dotProduct(vector, vector));
        const bool inner = distance < (outerRingRadius() + innerRingRadius()) / 2;

        if (inner) {
            return index == 0 ? 0 : index + 12;
        }

        return index == 0 ? 12 : index;
    }

    // Minutes next to a labeled value snap to it, so the labels are easy to hit
    int minute = qRound(angle / 6);
    const int nearestLabeled = qRound(minute / 5.0) * 5;
    if (qAbs(minute - nearestLabeled) <= 1) {
        minute = nearestLabeled;
    }

    return minute % 60;
}

QPointF ClockDialWidget::positionForValue(int value) const {
    if (_mode == Mode::Hours) {
        return pointOnCircle(dialCenter(), (value % 12) * 30,
                             isInnerRingHour(value) ? innerRingRadius() : outerRingRadius());
    }

    return pointOnCircle(dialCenter(), (value % 60) * 6, outerRingRadius());
}

QSize ClockDialWidget::sizeHint() const {
    const int size = qMax(232, fontMetrics().height() * 14);
    return {size, size};
}

QSize ClockDialWidget::minimumSizeHint() const { return sizeHint(); }

QPointF ClockDialWidget::dialCenter() const { return QRectF(rect()).center(); }

qreal ClockDialWidget::dialRadius() const { return qMin(width(), height()) / 2.0 - 1; }

qreal ClockDialWidget::bubbleRadius() const {
    return qMax<qreal>(14, fontMetrics().height() * 0.95);
}

qreal ClockDialWidget::outerRingRadius() const { return dialRadius() - bubbleRadius() - 3; }

qreal ClockDialWidget::innerRingRadius() const {
    return outerRingRadius() - bubbleRadius() * 2 - 2;
}

int ClockDialWidget::currentValue() const {
    return _mode == Mode::Hours ? _time.hour() : _time.minute();
}

void ClockDialWidget::setValue(int value, bool animated) {
    if (_mode == Mode::Hours) {
        setTime(QTime(value, _time.minute()), animated);
    } else {
        setTime(QTime(_time.hour(), value), animated);
    }
}

void ClockDialWidget::stepValue(int steps) {
    const int range = _mode == Mode::Hours ? 24 : 60;
    setValue(((currentValue() + steps) % range + range) % range, true);
}

qreal ClockDialWidget::animationProgress() const {
    return _animation->state() == QAbstractAnimation::Running ? _animation->currentValue().toReal()
                                                              : 1.0;
}

qreal ClockDialWidget::handAngle() const {
    const qreal target = _mode == Mode::Hours ? (_time.hour() % 12) * 30 : _time.minute() * 6;
    const qreal progress = animationProgress();

    if (progress >= 1.0) {
        return target;
    }

    // Turn the hand the shorter way around the clock
    const qreal delta = std::fmod(target - _fromAngle + 540, 360) - 180;
    return _fromAngle + delta * progress;
}

qreal ClockDialWidget::handRadius() const {
    const qreal target = _mode == Mode::Hours && isInnerRingHour(_time.hour()) ? innerRingRadius()
                                                                               : outerRingRadius();
    const qreal progress = animationProgress();

    return progress >= 1.0 ? target : _fromRadius + (target - _fromRadius) * progress;
}

/**
 * Remembers the current position of the hand, so it can move smoothly to the new value
 */
void ClockDialWidget::beginTransition(bool animated) {
    if (!animated || !isVisible()) {
        _animation->stop();
        return;
    }

    _fromAngle = handAngle();
    _fromRadius = handRadius();
    _animation->stop();
    _animation->start();
}

void ClockDialWidget::paintNumbers(QPainter &painter, const QColor &color, const QColor &innerColor,
                                   qreal opacity, qreal scale) const {
    const QPointF center = dialCenter();
    const qreal radius = bubbleRadius();
    const QFont outerFont = font();
    const QFont innerFont = scaledFont(font(), 0.85);

    painter.save();
    painter.setOpacity(opacity);

    const auto drawLabel = [&](const QString &label, qreal angle, qreal ringRadius,
                               const QFont &labelFont, const QColor &labelColor) {
        const QPointF position = pointOnCircle(center, angle, ringRadius * scale);
        painter.setFont(labelFont);
        painter.setPen(labelColor);
        painter.drawText(
            QRectF(position.x() - radius, position.y() - radius, radius * 2, radius * 2),
            Qt::AlignCenter, label);
    };

    for (int i = 0; i < 12; ++i) {
        if (_mode == Mode::Hours) {
            drawLabel(QString::number(i == 0 ? 12 : i), i * 30, outerRingRadius(), outerFont,
                      color);
            drawLabel(i == 0 ? QStringLiteral("00") : QString::number(i + 12), i * 30,
                      innerRingRadius(), innerFont, innerColor);
        } else {
            drawLabel(twoDigits(i * 5), i * 30, outerRingRadius(), outerFont, color);
        }
    }

    painter.restore();
}

void ClockDialWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    const QPalette pal = palette();
    const QColor text = pal.color(QPalette::WindowText);
    const QColor highlight = pal.color(QPalette::Highlight);
    const QColor highlightedText = pal.color(QPalette::HighlightedText);
    const QPointF center = dialCenter();
    const qreal radius = dialRadius();
    const qreal bubble = bubbleRadius();

    // Clock face
    painter.setPen(Qt::NoPen);
    painter.setBrush(withAlpha(text, 0.06));
    painter.drawEllipse(center, radius, radius);

    if (hasFocus()) {
        painter.setPen(QPen(withAlpha(highlight, 0.6), 1.5));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(center, radius - 0.75, radius - 0.75);
        painter.setPen(Qt::NoPen);
    }

    // Subtle bubble below the value under the mouse cursor
    if (_hoveredValue >= 0 && _hoveredValue != currentValue() && !_dragging) {
        painter.setBrush(withAlpha(text, 0.08));
        painter.drawEllipse(positionForValue(_hoveredValue), bubble, bubble);
    }

    // The numbers of a newly shown mode fade in and settle into place
    const qreal progress = _modeAnimating ? animationProgress() : 1.0;
    const qreal opacity = progress;
    const qreal scale = 1.0 + 0.12 * (1.0 - progress);
    paintNumbers(painter, text, withAlpha(text, 0.65), opacity, scale);

    // Hand with the selection bubble
    const QPointF bubbleCenter = pointOnCircle(center, handAngle(), handRadius());
    painter.setPen(QPen(highlight, 2, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(center, bubbleCenter);
    painter.setPen(Qt::NoPen);
    painter.setBrush(highlight);
    painter.drawEllipse(center, 3.5, 3.5);
    painter.drawEllipse(bubbleCenter, bubble, bubble);

    // Draw the parts of the numbers inside the bubble in a contrasting color,
    // so the selected number stays readable while the hand moves
    QPainterPath bubblePath;
    bubblePath.addEllipse(bubbleCenter, bubble, bubble);
    painter.save();
    painter.setClipPath(bubblePath);
    paintNumbers(painter, highlightedText, highlightedText, opacity, scale);
    painter.restore();

    // Minutes without a label get a dot in the bubble
    if (_mode == Mode::Minutes && _time.minute() % 5 != 0) {
        painter.setBrush(highlightedText);
        painter.drawEllipse(bubbleCenter, 2.5, 2.5);
    }
}

void ClockDialWidget::mousePressEvent(QMouseEvent *event) {
    const QPointF position = mousePosition(event);
    const QPointF vector = position - dialCenter();

    if (event->button() != Qt::LeftButton ||
        QPointF::dotProduct(vector, vector) > dialRadius() * dialRadius()) {
        QWidget::mousePressEvent(event);
        return;
    }

    _dragging = true;
    setValue(valueAt(position), true);
    update();
    event->accept();
}

void ClockDialWidget::mouseMoveEvent(QMouseEvent *event) {
    const QPointF position = mousePosition(event);

    if (_dragging) {
        const int value = valueAt(position);

        // Follow the mouse cursor directly while dragging
        if (value != currentValue()) {
            setValue(value, false);
        }

        event->accept();
        return;
    }

    updateHover(position);
    QWidget::mouseMoveEvent(event);
}

void ClockDialWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (!_dragging || event->button() != Qt::LeftButton) {
        QWidget::mouseReleaseEvent(event);
        return;
    }

    _dragging = false;
    updateHover(mousePosition(event));
    event->accept();
    emit valuePicked(_mode);
}

void ClockDialWidget::leaveEvent(QEvent *event) {
    _hoveredValue = -1;
    update();
    QWidget::leaveEvent(event);
}

void ClockDialWidget::wheelEvent(QWheelEvent *event) {
    // Collect the small steps of high-resolution touchpads to whole steps
    _wheelDelta += event->angleDelta().y();
    const int steps = _wheelDelta / 120;

    if (steps != 0) {
        _wheelDelta -= steps * 120;
        stepValue(steps);
    }

    event->accept();
}

void ClockDialWidget::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_Right:
        case Qt::Key_Plus:
            stepValue(1);
            break;
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Minus:
            stepValue(-1);
            break;
        case Qt::Key_PageUp:
            stepValue(_mode == Mode::Hours ? 6 : 5);
            break;
        case Qt::Key_PageDown:
            stepValue(_mode == Mode::Hours ? -6 : -5);
            break;
        case Qt::Key_Tab:
        case Qt::Key_Backtab:
        case Qt::Key_Colon:
            setMode(_mode == Mode::Hours ? Mode::Minutes : Mode::Hours, true);
            break;
        default:
            QWidget::keyPressEvent(event);
            return;
    }

    event->accept();
}

/**
 * Tab switches between hours and minutes instead of moving the focus
 */
bool ClockDialWidget::focusNextPrevChild(bool next) {
    Q_UNUSED(next)
    return false;
}

void ClockDialWidget::updateHover(const QPointF &position) {
    const QPointF vector = position - dialCenter();
    const int value =
        QPointF::dotProduct(vector, vector) <= dialRadius() * dialRadius() ? valueAt(position) : -1;

    if (value != _hoveredValue) {
        _hoveredValue = value;
        update();
    }
}

TimePickerPopup::TimePickerPopup(const QTime &time, const QDate &date, QWidget *parent)
    : QFrame(parent, Qt::Popup) {
    setObjectName(QStringLiteral("timePickerPopup"));
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);

    // Read the colors and the font only after the application style sheet was applied,
    // because the dark mode sets them with a style sheet instead of the palette
    ensurePolished();
    const QPalette pal = palette();
    const QColor text = pal.color(QPalette::WindowText);
    const QColor highlight = pal.color(QPalette::Highlight);
    const QColor highlightedText = pal.color(QPalette::HighlightedText);
    const int presetHeight = fontMetrics().height() + 10;
    const int headerFontSize = QFontInfo(scaledFont(font(), 2.4)).pixelSize();
    const int smallFontSize = QFontInfo(scaledFont(font(), 0.9)).pixelSize();

    // Keep a visible border, even if a style sheet of a parent removes the borders of frames.
    // The buttons keep the same border width in every state, so they don't move on hover.
    // Font sizes are set in the style sheet, because an application style sheet with a
    // font size would override fonts that are set on the widgets.
    setStyleSheet(
        QStringLiteral(
            "QFrame#timePickerPopup { border: 1px solid %1; background-color: %2; }"
            "QToolButton#timePickerHourButton, QToolButton#timePickerMinuteButton {"
            " color: %3; background-color: %4; border: 1px solid transparent;"
            " border-radius: 8px; padding: 0px 10px; margin: 0px; font-size: %13px; }"
            "QToolButton#timePickerHourButton:hover, QToolButton#timePickerMinuteButton:hover {"
            " background-color: %5; }"
            "QToolButton#timePickerHourButton:checked, QToolButton#timePickerMinuteButton:checked {"
            " background-color: %6; border: 1px solid %7; }"
            "QLabel#timePickerColonLabel { color: %14; background-color: transparent;"
            " font-size: %13px; }"
            "QLabel#timePickerDateLabel { color: %14; background-color: transparent;"
            " font-size: %15px; }"
            "QToolButton#timePresetButton { color: %3; background-color: transparent;"
            " border: 1px solid %8; border-radius: %9px; padding: 0px 8px; margin: 0px; }"
            "QToolButton#timePresetButton:hover { background-color: %5; }"
            "QToolButton#timePresetButton:checked { color: %10; background-color: %11;"
            " border: 1px solid %11; }"
            "QPushButton#timePickerNowButton { color: %3; background-color: transparent;"
            " border: 1px solid transparent; border-radius: 6px; padding: 4px 12px; }"
            "QPushButton#timePickerNowButton:hover { background-color: %5; }"
            "QPushButton#timePickerDoneButton { color: %10; background-color: %11;"
            " border: 1px solid %11; border-radius: 6px; padding: 4px 16px; font-weight: bold; }"
            "QPushButton#timePickerDoneButton:hover { background-color: %12;"
            " border: 1px solid %12; }")
            .arg(cssColor(withAlpha(text, 0.3)), pal.color(QPalette::Window).name(), text.name(),
                 cssColor(withAlpha(text, 0.06)), cssColor(withAlpha(text, 0.1)),
                 cssColor(withAlpha(highlight, 0.22)), cssColor(withAlpha(highlight, 0.6)),
                 cssColor(withAlpha(text, 0.25)), QString::number(presetHeight / 2))
            .arg(highlightedText.name(), highlight.name(),
                 (highlight.lightness() > 128 ? highlight.darker(110) : highlight.lighter(115))
                     .name(),
                 QString::number(headerFontSize), cssColor(withAlpha(text, 0.6)),
                 QString::number(smallFontSize)));

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(14, 12, 14, 12);
    layout->setSpacing(10);

    // Large hour and minute display, that also switches between picking hours and minutes
    auto *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(4);
    headerLayout->addStretch();

    const auto createHeaderButton = [this](const QString &objectName, const QString &toolTip) {
        auto *button = new QToolButton(this);
        button->setObjectName(objectName);
        button->setCheckable(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setCursor(Qt::PointingHandCursor);
        button->setToolTip(toolTip);
        button->setAccessibleName(toolTip);
        return button;
    };

    _hourButton = createHeaderButton(QStringLiteral("timePickerHourButton"), tr("Pick the hour"));
    _minuteButton =
        createHeaderButton(QStringLiteral("timePickerMinuteButton"), tr("Pick the minute"));

    auto *colonLabel = new QLabel(QStringLiteral(":"), this);
    colonLabel->setObjectName(QStringLiteral("timePickerColonLabel"));

    headerLayout->addWidget(_hourButton);
    headerLayout->addWidget(colonLabel);
    headerLayout->addWidget(_minuteButton);
    headerLayout->addStretch();
    layout->addLayout(headerLayout);

    // Show the date the time is picked for, as a reminder
    if (date.isValid()) {
        auto *dateLabel = new QLabel(QLocale().toString(date, QLocale::LongFormat), this);
        dateLabel->setObjectName(QStringLiteral("timePickerDateLabel"));
        dateLabel->setAlignment(Qt::AlignCenter);
        layout->addWidget(dateLabel);
    }

    _dial = new ClockDialWidget(this);
    _dial->setTime(time);
    layout->addWidget(_dial, 0, Qt::AlignHCenter);

    // Times that are often used for due dates
    auto *presetLayout = new QHBoxLayout();
    presetLayout->setSpacing(6);
    const QList<QPair<QTime, QString>> presets = {{QTime(9, 0), tr("Morning")},
                                                  {QTime(12, 0), tr("Noon")},
                                                  {QTime(15, 0), tr("Afternoon")},
                                                  {QTime(18, 0), tr("Evening")}};

    for (const auto &preset : presets) {
        const QTime presetTime = preset.first;
        auto *button = new QToolButton(this);
        button->setObjectName(QStringLiteral("timePresetButton"));
        button->setText(QLocale().toString(presetTime, QLocale::ShortFormat));
        button->setToolTip(tr("%1: set the time to %2").arg(preset.second, button->text()));
        button->setAccessibleName(button->toolTip());
        button->setProperty("presetTime", presetTime);
        button->setCheckable(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setCursor(Qt::PointingHandCursor);
        button->setFixedHeight(presetHeight);
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(button, &QToolButton::clicked, this, [this, presetTime]() {
            _dial->setTime(presetTime, true);
            updateHeader();
        });
        _presetButtons.append(button);
        presetLayout->addWidget(button);
    }

    layout->addLayout(presetLayout);

    auto *buttonLayout = new QHBoxLayout();
    auto *nowButton = new QPushButton(tr("Now"), this);
    nowButton->setObjectName(QStringLiteral("timePickerNowButton"));
    nowButton->setToolTip(tr("Set the current time"));
    nowButton->setFocusPolicy(Qt::NoFocus);
    nowButton->setCursor(Qt::PointingHandCursor);
    auto *doneButton = new QPushButton(tr("Done"), this);
    doneButton->setObjectName(QStringLiteral("timePickerDoneButton"));
    doneButton->setToolTip(tr("Use the selected time (Enter), Escape restores the previous time"));
    doneButton->setFocusPolicy(Qt::NoFocus);
    doneButton->setCursor(Qt::PointingHandCursor);
    buttonLayout->addWidget(nowButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(doneButton);
    layout->addLayout(buttonLayout);

    connect(nowButton, &QPushButton::clicked, this,
            [this]() { _dial->setTime(QTime::currentTime(), true); });
    connect(doneButton, &QPushButton::clicked, this, &TimePickerPopup::accept);
    connect(_hourButton, &QToolButton::clicked, this, [this]() {
        _dial->setMode(ClockDialWidget::Mode::Hours, true);
        updateHeader();
    });
    connect(_minuteButton, &QToolButton::clicked, this, [this]() {
        _dial->setMode(ClockDialWidget::Mode::Minutes, true);
        updateHeader();
    });
    connect(_dial, &ClockDialWidget::timeChanged, this, [this](const QTime &newTime) {
        updateHeader();
        emit timeChanged(newTime);
    });
    connect(_dial, &ClockDialWidget::modeChanged, this, &TimePickerPopup::updateHeader);

    // Continue with the minutes after an hour was picked, like on mobile clocks
    connect(_dial, &ClockDialWidget::valuePicked, this, [this](ClockDialWidget::Mode mode) {
        if (mode == ClockDialWidget::Mode::Hours) {
            _dial->setMode(ClockDialWidget::Mode::Minutes, true);
        }
    });

    setFocusProxy(_dial);
    updateHeader();
}

QTime TimePickerPopup::time() const { return _dial->time(); }

void TimePickerPopup::setTime(const QTime &time) { _dial->setTime(time, true); }

ClockDialWidget *TimePickerPopup::dial() const { return _dial; }

void TimePickerPopup::updateHeader() {
    const QTime time = _dial->time();
    const bool hoursMode = _dial->mode() == ClockDialWidget::Mode::Hours;

    _hourButton->setText(twoDigits(time.hour()));
    _minuteButton->setText(twoDigits(time.minute()));
    _hourButton->setChecked(hoursMode);
    _minuteButton->setChecked(!hoursMode);

    for (int i = 0; i < _presetButtons.count(); ++i) {
        QToolButton *button = _presetButtons.at(i);
        button->setChecked(button->property("presetTime").toTime() == time);
    }
}

void TimePickerPopup::keyPressEvent(QKeyEvent *event) {
    switch (event->key()) {
        case Qt::Key_Escape:
            reject();
            event->accept();
            return;
        case Qt::Key_Return:
        case Qt::Key_Enter:
            accept();
            event->accept();
            return;
        default:
            QFrame::keyPressEvent(event);
    }
}

void TimePickerPopup::accept() {
    if (_finished) {
        return;
    }

    _finished = true;
    emit accepted();
    close();
}

void TimePickerPopup::reject() {
    if (_finished) {
        return;
    }

    _finished = true;
    emit rejected();
    close();
}

TimePickerButton::TimePickerButton(QWidget *parent) : QToolButton(parent) {
    setToolTip(tr("Pick a time"));
    setAccessibleName(toolTip());
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    updateIcon();
    connect(this, &QToolButton::clicked, this, &TimePickerButton::showPopup);
}

void TimePickerButton::setTimeEdit(QDateTimeEdit *timeEdit) {
    if (_timeEdit) {
        _timeEdit->removeEventFilter(this);
    }

    _timeEdit = timeEdit;

    if (_timeEdit) {
        _timeEdit->installEventFilter(this);
        const int size = _timeEdit->sizeHint().height();
        setFixedSize(size, size);
    }

    updateEnabledState();
}

QDateTimeEdit *TimePickerButton::timeEdit() const { return _timeEdit; }

void TimePickerButton::setDateEdit(QDateTimeEdit *dateEdit) { _dateEdit = dateEdit; }

QDateTimeEdit *TimePickerButton::dateEdit() const { return _dateEdit; }

TimePickerPopup *TimePickerButton::popup() const { return _popup; }

bool TimePickerButton::isPopupVisible() const { return _popup && _popup->isVisible(); }

/**
 * Returns a clock icon in the given color, so it fits light and dark themes
 */
QIcon TimePickerButton::clockIcon(const QColor &color, int size) {
    const qreal ratio = qApp != nullptr ? qApp->devicePixelRatio() : 1.0;
    QPixmap pixmap(QSize(size, size) * ratio);
    pixmap.setDevicePixelRatio(ratio);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    const QPointF center(size / 2.0, size / 2.0);
    const qreal radius = size / 2.0 - 1.25;

    painter.setPen(QPen(color, 1.3));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(center, radius, radius);

    painter.setPen(QPen(color, 1.4, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(center, center + QPointF(0, -radius * 0.62));
    painter.drawLine(center, center + QPointF(radius * 0.45, radius * 0.22));
    painter.end();

    return QIcon(pixmap);
}

void TimePickerButton::showPopup() {
    if (!_timeEdit || !isEnabled()) {
        return;
    }

    if (_popup) {
        _popup->close();
    }

    // Show the date as a reminder, if it is known
    QDate date;
    if (_dateEdit) {
        date = _dateEdit->date();
    } else if (_timeEdit->displayedSections() & QDateTimeEdit::DateSections_Mask) {
        date = _timeEdit->date();
    }

    const QTime originalTime = _timeEdit->time();
    auto *popup = new TimePickerPopup(originalTime, date, this);
    popup->setAttribute(Qt::WA_DeleteOnClose);
    popup->installEventFilter(this);
    _popup = popup;

    // Show the picked time right away in the time edit, Escape restores the previous one
    connect(popup, &TimePickerPopup::timeChanged, this, [this](const QTime &time) {
        if (_timeEdit) {
            _timeEdit->setTime(time);
        }
    });
    connect(popup, &TimePickerPopup::rejected, this, [this, originalTime]() {
        if (_timeEdit) {
            _timeEdit->setTime(originalTime);
        }
    });

    popup->ensurePolished();
    popup->adjustSize();

    // Show the popup below the button, aligned to its right edge, and keep it on the screen
    QPoint position = mapToGlobal(QPoint(width() - popup->width(), height() + 2));
    QScreen *screen = QGuiApplication::screenAt(mapToGlobal(rect().center()));

    if (screen != nullptr) {
        const QRect available = screen->availableGeometry();

        position.setX(
            qBound(available.left(), position.x(), available.right() - popup->width() + 1));

        if (position.y() + popup->height() > available.bottom()) {
            position.setY(
                qMax(available.top(), mapToGlobal(QPoint(0, 0)).y() - popup->height() - 2));
        }
    }

    popup->move(position);
    popup->show();
    popup->dial()->setFocus();
}

bool TimePickerButton::eventFilter(QObject *obj, QEvent *event) {
    if (obj == _timeEdit) {
        if (event->type() == QEvent::EnabledChange) {
            updateEnabledState();
        } else if (event->type() == QEvent::Resize) {
            const int size = _timeEdit->height();
            setFixedSize(size, size);
        }
    } else if (obj == _popup && event->type() == QEvent::Hide && isVisible() && isEnabled()) {
        // Return the focus to the button after the popup was closed
        setFocus();
    }

    return QToolButton::eventFilter(obj, event);
}

void TimePickerButton::changeEvent(QEvent *event) {
    QToolButton::changeEvent(event);

    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange) {
        updateIcon();
    }

    if (event->type() == QEvent::EnabledChange && !isEnabled() && _popup) {
        _popup->close();
    }
}

void TimePickerButton::updateIcon() { setIcon(clockIcon(palette().color(QPalette::ButtonText))); }

/**
 * Follows the explicitly set enabled state of the time edit, a disabled parent
 * already disables both
 */
void TimePickerButton::updateEnabledState() {
    if (_timeEdit) {
        setEnabled(!_timeEdit->testAttribute(Qt::WA_ForceDisabled));
    }
}
