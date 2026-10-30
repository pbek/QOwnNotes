#include "loadingoverlaywidget.h"

#include <utils/misc.h>

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>

#include "libraries/qtwaitingspinner/waitingspinnerwidget.h"

LoadingOverlayWidget::LoadingOverlayWidget(QWidget *parent) : QWidget(parent) {
    // The card is painted by the overlay itself, so it can have rounded corners and a shadow
    _card = new QWidget(this);
    _card->setMinimumWidth(340);
    _card->setMaximumWidth(440);

    _spinner = new WaitingSpinnerWidget(_card, false, false);
    _spinner->setNumberOfLines(12);
    _spinner->setLineLength(8);
    _spinner->setLineWidth(3);
    _spinner->setInnerRadius(9);
    _spinner->setRoundness(100);
    _spinner->setTrailFadePercentage(70);
    _spinner->setMinimumTrailOpacity(15);
    _spinner->setRevolutionsPerSecond(1);

    _titleLabel = new QLabel(_card);
    _titleLabel->setAlignment(Qt::AlignCenter);
    _titleLabel->setWordWrap(true);
    QFont titleFont = _titleLabel->font();
    titleFont.setBold(true);
    titleFont.setPointSizeF(titleFont.pointSizeF() * 1.2);
    _titleLabel->setFont(titleFont);

    _textLabel = new QLabel(_card);
    _textLabel->setAlignment(Qt::AlignCenter);
    _textLabel->setWordWrap(true);

    _progressBar = new QProgressBar(_card);
    _progressBar->setTextVisible(false);
    _progressBar->setMaximumHeight(8);
    _progressBar->setRange(0, 0);

    _statusLabel = new QLabel(_card);
    _statusLabel->setAlignment(Qt::AlignCenter);
    QFont statusFont = _statusLabel->font();
    statusFont.setPointSizeF(statusFont.pointSizeF() * 0.9);
    _statusLabel->setFont(statusFont);

    _cancelButton = new QPushButton(tr("&Cancel"), _card);
    _cancelButton->setIcon(
        QIcon::fromTheme(QStringLiteral("dialog-cancel"),
                         QIcon(":/icons/breeze-qownnotes/16x16/dialog-cancel.svg")));
    _cancelButton->setAutoDefault(false);
    connect(_cancelButton, &QPushButton::clicked, this, &LoadingOverlayWidget::cancelRequested);

    auto *cardLayout = new QVBoxLayout(_card);
    cardLayout->setContentsMargins(32, 28, 32, 24);
    cardLayout->setSpacing(10);
    cardLayout->addWidget(_spinner, 0, Qt::AlignHCenter);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(_titleLabel);
    cardLayout->addWidget(_textLabel);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(_progressBar);
    cardLayout->addWidget(_statusLabel);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(_cancelButton, 0, Qt::AlignHCenter);

    auto *rowLayout = new QHBoxLayout();
    rowLayout->addStretch();
    rowLayout->addWidget(_card);
    rowLayout->addStretch();

    auto *layout = new QVBoxLayout(this);
    layout->addStretch();
    layout->addLayout(rowLayout);
    layout->addStretch();

    _statusTimer = new QTimer(this);
    _statusTimer->setInterval(1000);
    connect(_statusTimer, &QTimer::timeout, this, &LoadingOverlayWidget::updateStatusText);

    updateColors();
    parent->installEventFilter(this);
    hide();
}

void LoadingOverlayWidget::start(const QString &title, const QString &text) {
    _titleLabel->setText(title);
    _textLabel->setText(text);
    _textLabel->setVisible(!text.isEmpty());
    _bytesReceived = 0;
    _bytesTotal = -1;
    _customStatusText.clear();
    _progressBar->setRange(0, 0);
    _elapsedTimer.start();
    updateStatusText();

    setGeometry(parentWidget()->rect());
    raise();
    show();
    _spinner->start();
    _statusTimer->start();
    _cancelButton->setFocus();
}

void LoadingOverlayWidget::stop() {
    _statusTimer->stop();
    _spinner->stop();
    hide();
}

/**
 * Switches the progress bar to a determinate state as soon as the total size is known
 */
void LoadingOverlayWidget::setProgress(qint64 bytesReceived, qint64 bytesTotal) {
    _bytesReceived = bytesReceived;
    _bytesTotal = bytesTotal;

    if (bytesTotal > 0) {
        _progressBar->setRange(0, 1000);
        _progressBar->setValue(static_cast<int>(bytesReceived * 1000 / bytesTotal));
    }

    updateStatusText();
}

/**
 * Shows the progress of locally processed items instead of received bytes
 */
void LoadingOverlayWidget::setCountProgress(int value, int maximum, const QString &statusText) {
    _customStatusText = statusText;
    _progressBar->setRange(0, maximum);
    _progressBar->setValue(value);
    updateStatusText();
}

void LoadingOverlayWidget::updateStatusText() {
    const qint64 seconds = _elapsedTimer.isValid() ? _elapsedTimer.elapsed() / 1000 : 0;
    const QString elapsed =
        QStringLiteral("%1:%2").arg(seconds / 60).arg(seconds % 60, 2, 10, QLatin1Char('0'));

    if (!_customStatusText.isEmpty()) {
        _statusLabel->setText(QStringLiteral("%1 · %2").arg(_customStatusText, elapsed));
    } else if (_bytesTotal > 0) {
        _statusLabel->setText(tr("%1 of %2 received · %3")
                                  .arg(Utils::Misc::toHumanReadableByteSize(_bytesReceived),
                                       Utils::Misc::toHumanReadableByteSize(_bytesTotal), elapsed));
    } else if (_bytesReceived > 0) {
        _statusLabel->setText(
            tr("%1 received · %2")
                .arg(Utils::Misc::toHumanReadableByteSize(_bytesReceived), elapsed));
    } else {
        _statusLabel->setText(tr("Waiting for the server · %1").arg(elapsed));
    }
}

void LoadingOverlayWidget::updateColors() {
    const QColor textColor = palette().color(QPalette::WindowText);
    _spinner->setColor(textColor);

    // Use a muted text color for the secondary labels
    QColor mutedColor = textColor;
    mutedColor.setAlphaF(0.65);

    for (QLabel *label : {_textLabel, _statusLabel}) {
        QPalette labelPalette = label->palette();
        labelPalette.setColor(QPalette::WindowText, mutedColor);
        label->setPalette(labelPalette);
    }
}

bool LoadingOverlayWidget::eventFilter(QObject *obj, QEvent *event) {
    // Keep covering the whole parent widget
    if (obj == parentWidget() && event->type() == QEvent::Resize) {
        setGeometry(parentWidget()->rect());
    }

    return QWidget::eventFilter(obj, event);
}

void LoadingOverlayWidget::changeEvent(QEvent *event) {
    if (event->type() == QEvent::PaletteChange || event->type() == QEvent::StyleChange) {
        updateColors();
    }

    QWidget::changeEvent(event);
}

void LoadingOverlayWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Hide the (still empty) dialog content behind the overlay
    painter.fillRect(rect(), palette().color(QPalette::Window));

    const QRectF cardRect = QRectF(_card->geometry()).adjusted(0.5, 0.5, -0.5, -0.5);
    const qreal radius = 12;

    // Draw a soft shadow below the card
    painter.setPen(Qt::NoPen);
    for (int i = 1; i <= 8; ++i) {
        QColor shadowColor(0, 0, 0, 10 - i);
        painter.setBrush(shadowColor);
        painter.drawRoundedRect(cardRect.adjusted(-i, -i + 2, i, i + 2), radius + i, radius + i);
    }

    QColor borderColor = palette().color(QPalette::WindowText);
    borderColor.setAlphaF(0.12);

    QPainterPath path;
    path.addRoundedRect(cardRect, radius, radius);
    painter.setPen(QPen(borderColor, 1));
    painter.setBrush(palette().color(QPalette::Base));
    painter.drawPath(path);
}
