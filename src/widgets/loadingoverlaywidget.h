#pragma once

#include <QElapsedTimer>
#include <QWidget>

class QLabel;
class QProgressBar;
class QPushButton;
class QTimer;
class WaitingSpinnerWidget;

/**
 * Overlay that covers its parent widget while data is loaded
 *
 * It shows a centered card with a spinner, a title, a status line (elapsed time
 * or received bytes), a progress bar and a cancel button.
 */
class LoadingOverlayWidget : public QWidget {
    Q_OBJECT

   public:
    explicit LoadingOverlayWidget(QWidget *parent);

    void start(const QString &title, const QString &text = QString());

    void stop();

    void setProgress(qint64 bytesReceived, qint64 bytesTotal);

    void setCountProgress(int value, int maximum, const QString &statusText);

   signals:
    void cancelRequested();

   protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

    void paintEvent(QPaintEvent *event) override;

    void changeEvent(QEvent *event) override;

   private:
    QWidget *_card;
    WaitingSpinnerWidget *_spinner;
    QLabel *_titleLabel;
    QLabel *_textLabel;
    QLabel *_statusLabel;
    QProgressBar *_progressBar;
    QPushButton *_cancelButton;
    QTimer *_statusTimer;
    QElapsedTimer _elapsedTimer;
    qint64 _bytesReceived = 0;
    qint64 _bytesTotal = -1;
    QString _customStatusText;

    void updateStatusText();

    void updateColors();
};
