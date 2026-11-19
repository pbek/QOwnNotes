#pragma once

#include <QDate>
#include <QDateTime>
#include <QFrame>
#include <QPointer>
#include <QTime>
#include <QToolButton>

class QDateTimeEdit;
class QLabel;
class QPushButton;
class QVariantAnimation;

/**
 * Round clock face to pick the hour (two rings, 0-23) or the minute of a time
 */
class ClockDialWidget : public QWidget {
    Q_OBJECT

   public:
    enum class Mode { Hours, Minutes };
    Q_ENUM(Mode)

    explicit ClockDialWidget(QWidget *parent = nullptr);

    QTime time() const;
    void setTime(const QTime &time, bool animated = false);
    Mode mode() const;
    void setMode(Mode mode, bool animated = false);

    // Returns the hour or minute (depending on the mode) at a position of the widget
    int valueAt(const QPointF &position) const;
    // Returns the center of the number bubble of an hour or minute
    QPointF positionForValue(int value) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

   Q_SIGNALS:
    void timeChanged(const QTime &time);
    void modeChanged(ClockDialWidget::Mode mode);
    // Emitted after the user picked a value with the mouse
    void valuePicked(ClockDialWidget::Mode mode);

   protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool focusNextPrevChild(bool next) override;

   private:
    QTime _time = QTime(12, 0);
    Mode _mode = Mode::Hours;
    bool _dragging = false;
    int _hoveredValue = -1;
    int _wheelDelta = 0;
    QVariantAnimation *_animation = nullptr;
    qreal _fromAngle = 0;
    qreal _fromRadius = 0;
    bool _modeAnimating = false;

    QPointF dialCenter() const;
    qreal dialRadius() const;
    qreal bubbleRadius() const;
    qreal outerRingRadius() const;
    qreal innerRingRadius() const;
    int currentValue() const;
    void setValue(int value, bool animated);
    void stepValue(int steps);
    qreal handAngle() const;
    qreal handRadius() const;
    qreal animationProgress() const;
    void beginTransition(bool animated);
    void paintNumbers(QPainter &painter, const QColor &color, const QColor &innerColor,
                      qreal opacity, qreal scale) const;
    void updateHover(const QPointF &position);
};

/**
 * Popup with a header to switch between hours and minutes, the clock face,
 * quick time presets and buttons to set the current time or to close the popup
 */
class TimePickerPopup : public QFrame {
    Q_OBJECT

   public:
    explicit TimePickerPopup(const QTime &time, const QDate &date, QWidget *parent = nullptr);

    QTime time() const;
    void setTime(const QTime &time);
    ClockDialWidget *dial() const;

   Q_SIGNALS:
    void timeChanged(const QTime &time);
    void accepted();
    void rejected();

   protected:
    void keyPressEvent(QKeyEvent *event) override;

   private:
    ClockDialWidget *_dial = nullptr;
    QToolButton *_hourButton = nullptr;
    QToolButton *_minuteButton = nullptr;
    QList<QToolButton *> _presetButtons;
    bool _finished = false;

    void updateHeader();
    void accept();
    void reject();
};

/**
 * Button with a clock icon that opens a time picker for a time edit,
 * so the time doesn't need to be typed
 */
class TimePickerButton : public QToolButton {
    Q_OBJECT

   public:
    explicit TimePickerButton(QWidget *parent = nullptr);

    // The edit with the time that is picked, the button follows its enabled state
    void setTimeEdit(QDateTimeEdit *timeEdit);
    QDateTimeEdit *timeEdit() const;
    // Optional edit with the date, that is shown in the popup as a reminder
    void setDateEdit(QDateTimeEdit *dateEdit);
    QDateTimeEdit *dateEdit() const;
    TimePickerPopup *popup() const;
    bool isPopupVisible() const;

    static QIcon clockIcon(const QColor &color, int size = 16);

   public Q_SLOTS:
    void showPopup();

   protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
    void changeEvent(QEvent *event) override;

   private:
    QPointer<QDateTimeEdit> _timeEdit;
    QPointer<QDateTimeEdit> _dateEdit;
    QPointer<TimePickerPopup> _popup;

    void updateIcon();
    void updateEnabledState();
};
