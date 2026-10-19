#include "windowactivationservice.h"

#include <QByteArrayList>
#include <QGuiApplication>
#include <QList>
#include <QWidget>
#include <QWindow>

#ifdef QON_KDE_INTEGRATION
#include <KStartupInfo>
#include <KWindowSystem>
#endif

namespace {
const char xdgActivationTokenEnv[] = "XDG_ACTIVATION_TOKEN";
const char startupIdEnv[] = "DESKTOP_STARTUP_ID";

const QByteArray xdgActivationTokenKey = QByteArrayLiteral("xdgActivationToken:");
const QByteArray startupIdKey = QByteArrayLiteral("startupId:");
const QByteArray startupActionKey = QByteArrayLiteral("startupAction:");
const QByteArray activateMessage = QByteArrayLiteral("activate");

QByteArray s_startupXdgActivationToken;
QByteArray s_startupId;

bool isWayland() { return QGuiApplication::platformName().startsWith(QLatin1String("wayland")); }

// Values must not contain line breaks, because they separate the entries of the message
QByteArray singleLine(const QByteArray &value) {
    const int index = value.indexOf('\n');
    return index < 0 ? value : value.left(index);
}
}    // namespace

void WindowActivationService::captureStartupEnvironment() {
    s_startupXdgActivationToken = singleLine(qgetenv(xdgActivationTokenEnv).trimmed());
    s_startupId = singleLine(qgetenv(startupIdEnv).trimmed());
}

QByteArray WindowActivationService::secondaryInstanceMessage(const QString &startupAction) {
    QList<QByteArray> lines;

    if (!s_startupXdgActivationToken.isEmpty()) {
        lines.append(xdgActivationTokenKey + s_startupXdgActivationToken);
    }

    if (!s_startupId.isEmpty()) {
        lines.append(startupIdKey + s_startupId);
    }

    // Only an action results in the same message as in older versions
    if (!startupAction.isEmpty()) {
        lines.append(startupActionKey + singleLine(startupAction.toUtf8()));
    }

    if (lines.isEmpty()) {
        return activateMessage;
    }

    return lines.join('\n');
}

WindowActivationService::Request WindowActivationService::parseSecondaryInstanceMessage(
    const QByteArray &message) {
    Request request;
    const QList<QByteArray> lines = message.split('\n');

    for (const QByteArray &line : lines) {
        if (line.startsWith(xdgActivationTokenKey)) {
            request.xdgActivationToken = QString::fromUtf8(line.mid(xdgActivationTokenKey.size()));
        } else if (line.startsWith(startupIdKey)) {
            request.startupId = line.mid(startupIdKey.size());
        } else if (line.startsWith(startupActionKey)) {
            request.startupAction = QString::fromUtf8(line.mid(startupActionKey.size()));
        }
    }

    return request;
}

void WindowActivationService::setXdgActivationToken(const QString &token) {
    if (token.isEmpty() || !isWayland()) {
        return;
    }

#ifdef QON_KDE_INTEGRATION
    KWindowSystem::setCurrentXdgActivationToken(token);
#else
    qputenv(xdgActivationTokenEnv, token.toUtf8());
#endif
}

void WindowActivationService::clearXdgActivationToken(const QString &token) {
    if (!token.isEmpty() && qgetenv(xdgActivationTokenEnv) == token.toUtf8()) {
        qunsetenv(xdgActivationTokenEnv);
    }
}

void WindowActivationService::activateWindow(QWidget *window, const QString &xdgActivationToken,
                                             const QByteArray &startupId) {
    if (window == nullptr) {
        return;
    }

    window->show();

    // In case the window was minimized show it normal again
    // (it didn't come up when it was minimized on KDE)
    if (window->isMinimized()) {
        window->showNormal();
    }

#ifdef QON_KDE_INTEGRATION
    // The window manager activates the window for the startup notification of the launch
    if (KWindowSystem::isPlatformX11() && !startupId.isEmpty() &&
        window->windowHandle() != nullptr) {
        window->raise();
        KStartupInfo::setNewStartupId(window->windowHandle(), startupId);
        return;
    }
#else
    Q_UNUSED(startupId)
#endif

    setXdgActivationToken(xdgActivationToken);
    window->raise();
    window->activateWindow();
    clearXdgActivationToken(xdgActivationToken);
}
