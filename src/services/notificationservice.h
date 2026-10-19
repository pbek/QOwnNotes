#pragma once

#include <QHash>
#include <QObject>
#include <QPointer>
#include <functional>

class QSystemTrayIcon;

/**
 * Shows desktop notifications
 *
 * KDE builds use KNotification, so events can be configured in Plasma's notification
 * settings. Otherwise, events that allow it fall back to the system tray and then to the
 * status bar.
 */
class NotificationService : public QObject {
    Q_OBJECT

   public:
    enum class Event {
        // Notification requested by a script, with tray and status bar fallback
        ScriptNotification,
        // The current note was modified externally while QOwnNotes was in the background,
        // only shown as a desktop notification
        ExternalNoteConflict,
    };

    static NotificationService *instance();

    explicit NotificationService(QObject *parent = nullptr);

    static QString eventId(Event event);
    static bool isDesktopNotificationServiceAvailable();

    void setTrayIcon(QSystemTrayIcon *trayIcon);

    bool notify(Event event, const QString &title, const QString &text,
                const QString &actionLabel = QString(),
                const std::function<void()> &action = std::function<void()>());

   signals:
    // Emitted if a notification can only be shown in the status bar
    void statusMessageRequested(const QString &message);

   private:
    struct RecentNotification {
        qint64 timestamp = 0;
        bool shown = false;
    };

    bool showTrayMessage(const QString &title, const QString &text,
                         const std::function<void()> &action);
#ifdef QON_KDE_INTEGRATION
    bool sendDesktopNotification(Event event, const QString &title, const QString &text,
                                 const QString &actionLabel, const std::function<void()> &action);
#endif

    QPointer<QSystemTrayIcon> _trayIcon;
    std::function<void()> _trayMessageAction;
    QHash<QString, RecentNotification> _recentNotifications;
};
