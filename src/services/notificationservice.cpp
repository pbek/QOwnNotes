#include "notificationservice.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QSystemTrayIcon>

#ifdef QON_KDE_INTEGRATION
#include <KNotification>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QFileInfo>
#endif

namespace {
// Identical notifications within this time are only shown once
const qint64 duplicateIntervalMs = 10000;

#ifdef QON_KDE_INTEGRATION
// Matches the embedded and installed "qownnotes.notifyrc"
const QString componentName = QStringLiteral("qownnotes");
#endif
}    // namespace

NotificationService *NotificationService::instance() {
    static auto *service = new NotificationService(QCoreApplication::instance());
    return service;
}

NotificationService::NotificationService(QObject *parent) : QObject(parent) {}

QString NotificationService::eventId(Event event) {
    switch (event) {
        case Event::ExternalNoteConflict:
            return QStringLiteral("externalNoteConflict");
        case Event::ScriptNotification:
        default:
            return QStringLiteral("scriptNotification");
    }
}

/**
 * Checks if a desktop notification server (or the notification portal in a sandbox) can
 * be reached, without starting one
 */
bool NotificationService::isDesktopNotificationServiceAvailable() {
#ifdef QON_KDE_INTEGRATION
    QDBusConnectionInterface *bus = QDBusConnection::sessionBus().interface();
    if (bus == nullptr) {
        return false;
    }

    // KNotification uses the notification portal in Flatpak and Snap sandboxes
    if (QFileInfo::exists(QStringLiteral("/.flatpak-info")) || qEnvironmentVariableIsSet("SNAP")) {
        return bus->isServiceRegistered(QStringLiteral("org.freedesktop.portal.Desktop"));
    }

    const QString service = QStringLiteral("org.freedesktop.Notifications");
    if (bus->isServiceRegistered(service)) {
        return true;
    }

    const QDBusReply<QStringList> activatable = bus->activatableServiceNames();
    return activatable.isValid() && activatable.value().contains(service);
#else
    return false;
#endif
}

void NotificationService::setTrayIcon(QSystemTrayIcon *trayIcon) {
    if (_trayIcon == trayIcon) {
        return;
    }

    if (_trayIcon != nullptr) {
        disconnect(_trayIcon, nullptr, this, nullptr);
    }

    _trayIcon = trayIcon;
    _trayMessageAction = nullptr;

    if (_trayIcon != nullptr) {
        connect(_trayIcon, &QSystemTrayIcon::messageClicked, this, [this]() {
            if (_trayMessageAction) {
                _trayMessageAction();
            }
        });
    }
}

/**
 * Shows a notification for an event
 *
 * @param actionLabel label of the default action, which is triggered by clicking the
 *                    notification
 * @return true if the notification was handed to the desktop notification system or the
 *         system tray, false if it was only shown in the status bar or not at all
 */
bool NotificationService::notify(Event event, const QString &title, const QString &text,
                                 const QString &actionLabel, const std::function<void()> &action) {
    const QString key = eventId(event) + QLatin1Char('\n') + title + QLatin1Char('\n') + text;
    const qint64 now = QDateTime::currentMSecsSinceEpoch();

    // Forget notifications that are too old to count as duplicates
    for (auto it = _recentNotifications.begin(); it != _recentNotifications.end();) {
        if (now - it.value().timestamp >= duplicateIntervalMs) {
            it = _recentNotifications.erase(it);
        } else {
            ++it;
        }
    }

    // Avoid flooding the desktop, e.g. if a script shows the same notification in a loop
    if (_recentNotifications.contains(key)) {
        return _recentNotifications.value(key).shown;
    }

    bool shown = false;
#ifdef QON_KDE_INTEGRATION
    const bool useDesktopNotification = isDesktopNotificationServiceAvailable();
    if (useDesktopNotification) {
        shown = sendDesktopNotification(event, title, text, actionLabel, action);
    }
#else
    const bool useDesktopNotification = false;
    Q_UNUSED(actionLabel)
#endif

    // Other events already have their own status bar messages
    if (!useDesktopNotification && event == Event::ScriptNotification) {
        shown = showTrayMessage(title, text, action);

        if (!shown) {
            emit statusMessageRequested(title.isEmpty() ? text
                                                        : title + QStringLiteral(": ") + text);
        }
    }

    RecentNotification recent;
    recent.timestamp = now;
    recent.shown = shown;
    _recentNotifications.insert(key, recent);

    return shown;
}

bool NotificationService::showTrayMessage(const QString &title, const QString &text,
                                          const std::function<void()> &action) {
    if (_trayIcon == nullptr || !_trayIcon->isVisible() ||
        !QSystemTrayIcon::isSystemTrayAvailable() || !QSystemTrayIcon::supportsMessages()) {
        return false;
    }

    _trayMessageAction = action;
    _trayIcon->showMessage(title.isEmpty() ? QStringLiteral("QOwnNotes") : title, text,
                           QSystemTrayIcon::Information);
    return true;
}

#ifdef QON_KDE_INTEGRATION
bool NotificationService::sendDesktopNotification(Event event, const QString &title,
                                                  const QString &text, const QString &actionLabel,
                                                  const std::function<void()> &action) {
    // The notification deletes itself after it was closed. Whether and how it is shown is
    // configured by the notifyrc file and the user's Plasma notification settings.
    auto *notification = new KNotification(eventId(event));
    notification->setComponentName(componentName);
    notification->setTitle(title);
    // The text is plain text, so it can't inject markup or links
    notification->setText(text.toHtmlEscaped());

    if (action) {
        KNotificationAction *defaultAction = notification->addDefaultAction(
            actionLabel.isEmpty() ? tr("Open QOwnNotes") : actionLabel);
        connect(defaultAction, &KNotificationAction::activated, this, [notification, action]() {
            // Allow raising the window on Wayland with the token of the notification click
            const QString token = notification->xdgActivationToken();
            if (!token.isEmpty()) {
                qputenv("XDG_ACTIVATION_TOKEN", token.toUtf8());
            }
            action();
        });
    }

    notification->sendEvent();
    return true;
}
#endif
