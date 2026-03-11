/*
 * Copyright (c) 2014-2026 Patrizio Bekerle -- <patrizio@bekerle.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 */

#include "xdgglobalshortcutmanager.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusPendingCall>
#include <QDBusReply>
#include <QDebug>
#include <QGuiApplication>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QRandomGenerator>
#else
#include <QUuid>
#endif

XdgGlobalShortcutManager::XdgGlobalShortcutManager(QObject *parent) : QObject(parent) {
    // Register D-Bus metatypes for the a(sa{sv}) shortcut list argument
    qDBusRegisterMetaType<QPair<QString, QVariantMap>>();
    qDBusRegisterMetaType<QList<QPair<QString, QVariantMap>>>();
}

XdgGlobalShortcutManager::~XdgGlobalShortcutManager() { closeSession(); }

bool XdgGlobalShortcutManager::isAvailable() {
    QDBusInterface portal(PORTAL_SERVICE, PORTAL_PATH,
                          QStringLiteral("org.freedesktop.DBus.Properties"),
                          QDBusConnection::sessionBus());

    if (!portal.isValid()) {
        return false;
    }

    QDBusReply<QDBusVariant> reply =
        portal.call(QStringLiteral("Get"), QStringLiteral("org.freedesktop.portal.GlobalShortcuts"),
                    QStringLiteral("version"));

    return reply.isValid();
}

void XdgGlobalShortcutManager::setShortcuts(const QMap<QString, ShortcutInfo> &shortcuts) {
    _shortcuts = shortcuts;
}

void XdgGlobalShortcutManager::createSession() {
    // Close existing session first
    closeSession();

    auto bus = QDBusConnection::sessionBus();

    // Generate unique tokens
    QString handleToken = generateToken();
    QString sessionToken = generateToken();

    // Pre-compute request path and subscribe BEFORE calling to avoid race
    QString requestPath = computeRequestPath(handleToken);

    bus.connect(PORTAL_SERVICE, requestPath, REQUEST_INTERFACE, QStringLiteral("Response"), this,
                SLOT(onCreateSessionResponse(uint, QVariantMap)));

    // Build options
    QVariantMap options;
    options[QStringLiteral("handle_token")] = handleToken;
    options[QStringLiteral("session_handle_token")] = sessionToken;

    QDBusMessage msg = QDBusMessage::createMethodCall(PORTAL_SERVICE, PORTAL_PATH, PORTAL_INTERFACE,
                                                      QStringLiteral("CreateSession"));
    msg << options;

    QDBusPendingCall pendingCall = bus.asyncCall(msg);

    // Log errors on completion
    auto *watcher = new QDBusPendingCallWatcher(pendingCall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [watcher](QDBusPendingCallWatcher *w) {
                if (w->isError()) {
                    qWarning() << "XDG GlobalShortcuts CreateSession D-Bus error:"
                               << w->error().message();
                }
                w->deleteLater();
            });
}

void XdgGlobalShortcutManager::closeSession() {
    if (_sessionHandle.isEmpty()) {
        return;
    }

    auto bus = QDBusConnection::sessionBus();

    // Close the session by calling Close on the session object
    QDBusMessage msg = QDBusMessage::createMethodCall(
        PORTAL_SERVICE, _sessionHandle, QStringLiteral("org.freedesktop.portal.Session"),
        QStringLiteral("Close"));
    bus.asyncCall(msg);

    _sessionHandle.clear();
    _subscribed = false;
}

bool XdgGlobalShortcutManager::hasActiveSession() const { return !_sessionHandle.isEmpty(); }

void XdgGlobalShortcutManager::onCreateSessionResponse(uint response, QVariantMap results) {
    if (response != 0) {
        qWarning() << "XDG GlobalShortcuts CreateSession failed with response:" << response;
        return;
    }

    _sessionHandle = results.value(QStringLiteral("session_handle")).toString();

    if (_sessionHandle.isEmpty()) {
        qWarning() << "XDG GlobalShortcuts CreateSession returned empty "
                      "session handle";
        return;
    }

    qDebug() << "XDG GlobalShortcuts session created:" << _sessionHandle;

    // Subscribe to Activated signal BEFORE binding shortcuts to avoid race
    // condition — shortcuts may become active before the BindShortcuts
    // response arrives
    subscribeToActivation();

    // Now bind the shortcuts
    bindShortcuts();
}

void XdgGlobalShortcutManager::bindShortcuts() {
    if (_sessionHandle.isEmpty() || _shortcuts.isEmpty()) {
        return;
    }

    auto bus = QDBusConnection::sessionBus();
    QString handleToken = generateToken();
    QString requestPath = computeRequestPath(handleToken);

    // Subscribe to response before calling
    bus.connect(PORTAL_SERVICE, requestPath, REQUEST_INTERFACE, QStringLiteral("Response"), this,
                SLOT(onBindShortcutsResponse(uint, QVariantMap)));

    // Build the shortcuts list as QList<QPair<QString, QVariantMap>>
    // which maps to D-Bus type a(sa{sv}) via registered metatypes
    QList<QPair<QString, QVariantMap>> shortcutsList;

    for (auto it = _shortcuts.constBegin(); it != _shortcuts.constEnd(); ++it) {
        QVariantMap props;
        props[QStringLiteral("description")] = it.value().description;

        // Set preferred trigger if a key sequence is configured
        QString trigger = keySequenceToXdgTrigger(it.value().keySequence);
        if (!trigger.isEmpty()) {
            props[QStringLiteral("preferred_trigger")] = trigger;
        }

        shortcutsList.append(qMakePair(it.key(), props));
    }

    QVariantMap options;
    options[QStringLiteral("handle_token")] = handleToken;

    QDBusMessage msg = QDBusMessage::createMethodCall(PORTAL_SERVICE, PORTAL_PATH, PORTAL_INTERFACE,
                                                      QStringLiteral("BindShortcuts"));
    msg << QVariant::fromValue(QDBusObjectPath(_sessionHandle))
        << QVariant::fromValue(shortcutsList) << QString()    // parent_window (empty = no parent)
        << options;

    QDBusPendingCall pendingCall = bus.asyncCall(msg);

    auto *watcher = new QDBusPendingCallWatcher(pendingCall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this,
            [watcher](QDBusPendingCallWatcher *w) {
                if (w->isError()) {
                    qWarning() << "XDG GlobalShortcuts BindShortcuts D-Bus error:"
                               << w->error().message();
                }
                w->deleteLater();
            });
}

void XdgGlobalShortcutManager::onBindShortcutsResponse(uint response, QVariantMap results) {
    Q_UNUSED(results)

    if (response != 0) {
        qWarning() << "XDG GlobalShortcuts BindShortcuts failed with response:" << response;

        // Close the session so a fresh session can be created on retry
        // (the portal only allows binding shortcuts once per session)
        closeSession();
        return;
    }

    qDebug() << "XDG GlobalShortcuts: shortcuts bound successfully";
}

void XdgGlobalShortcutManager::subscribeToActivation() {
    if (_subscribed) {
        return;
    }

    auto bus = QDBusConnection::sessionBus();
    bus.connect(PORTAL_SERVICE, PORTAL_PATH, PORTAL_INTERFACE, QStringLiteral("Activated"), this,
                SLOT(onShortcutActivated(QDBusObjectPath, QString, qulonglong, QVariantMap)));

    _subscribed = true;
}

void XdgGlobalShortcutManager::onShortcutActivated(QDBusObjectPath sessionHandle,
                                                   QString shortcutId, qulonglong timestamp,
                                                   QVariantMap options) {
    Q_UNUSED(timestamp)
    Q_UNUSED(options)

    if (sessionHandle.path() != _sessionHandle) {
        return;
    }

    qDebug() << "XDG GlobalShortcuts: shortcut activated:" << shortcutId;
    emit shortcutActivated(shortcutId);
}

QString XdgGlobalShortcutManager::keySequenceToXdgTrigger(const QKeySequence &seq) {
    if (seq.isEmpty()) {
        return {};
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const int keyCombination = seq[0].toCombined();
#else
    const int keyCombination = seq[0];
#endif

    auto modifiers = Qt::KeyboardModifiers(keyCombination & Qt::KeyboardModifierMask);
    int key = keyCombination & ~Qt::KeyboardModifierMask;

    QStringList parts;

    if (modifiers & Qt::ControlModifier) {
        parts << QStringLiteral("CTRL");
    }
    if (modifiers & Qt::AltModifier) {
        parts << QStringLiteral("ALT");
    }
    if (modifiers & Qt::ShiftModifier) {
        parts << QStringLiteral("SHIFT");
    }
    if (modifiers & Qt::MetaModifier) {
        parts << QStringLiteral("LOGO");
    }

    QString keyName = qtKeyToXkbName(key);
    if (keyName.isEmpty()) {
        return {};
    }

    parts << keyName;
    return parts.join(QStringLiteral("+"));
}

QString XdgGlobalShortcutManager::qtKeyToXkbName(int key) {
    // Map special Qt keys to XKB key names
    // See: https://xkbcommon.org/doc/current/xkbcommon-keysyms_8h.html
    switch (key) {
        case Qt::Key_Escape:
            return QStringLiteral("Escape");
        case Qt::Key_Tab:
            return QStringLiteral("Tab");
        case Qt::Key_Backspace:
            return QStringLiteral("BackSpace");
        case Qt::Key_Return:
            return QStringLiteral("Return");
        case Qt::Key_Enter:
            return QStringLiteral("KP_Enter");
        case Qt::Key_Insert:
            return QStringLiteral("Insert");
        case Qt::Key_Delete:
            return QStringLiteral("Delete");
        case Qt::Key_Pause:
            return QStringLiteral("Pause");
        case Qt::Key_Print:
            return QStringLiteral("Print");
        case Qt::Key_Home:
            return QStringLiteral("Home");
        case Qt::Key_End:
            return QStringLiteral("End");
        case Qt::Key_Left:
            return QStringLiteral("Left");
        case Qt::Key_Up:
            return QStringLiteral("Up");
        case Qt::Key_Right:
            return QStringLiteral("Right");
        case Qt::Key_Down:
            return QStringLiteral("Down");
        case Qt::Key_PageUp:
            return QStringLiteral("Page_Up");
        case Qt::Key_PageDown:
            return QStringLiteral("Page_Down");
        case Qt::Key_CapsLock:
            return QStringLiteral("Caps_Lock");
        case Qt::Key_NumLock:
            return QStringLiteral("Num_Lock");
        case Qt::Key_ScrollLock:
            return QStringLiteral("Scroll_Lock");
        case Qt::Key_F1:
            return QStringLiteral("F1");
        case Qt::Key_F2:
            return QStringLiteral("F2");
        case Qt::Key_F3:
            return QStringLiteral("F3");
        case Qt::Key_F4:
            return QStringLiteral("F4");
        case Qt::Key_F5:
            return QStringLiteral("F5");
        case Qt::Key_F6:
            return QStringLiteral("F6");
        case Qt::Key_F7:
            return QStringLiteral("F7");
        case Qt::Key_F8:
            return QStringLiteral("F8");
        case Qt::Key_F9:
            return QStringLiteral("F9");
        case Qt::Key_F10:
            return QStringLiteral("F10");
        case Qt::Key_F11:
            return QStringLiteral("F11");
        case Qt::Key_F12:
            return QStringLiteral("F12");
        case Qt::Key_Space:
            return QStringLiteral("space");
        case Qt::Key_Minus:
            return QStringLiteral("minus");
        case Qt::Key_Plus:
            return QStringLiteral("plus");
        case Qt::Key_Equal:
            return QStringLiteral("equal");
        case Qt::Key_BracketLeft:
            return QStringLiteral("bracketleft");
        case Qt::Key_BracketRight:
            return QStringLiteral("bracketright");
        case Qt::Key_Backslash:
            return QStringLiteral("backslash");
        case Qt::Key_Semicolon:
            return QStringLiteral("semicolon");
        case Qt::Key_Apostrophe:
            return QStringLiteral("apostrophe");
        case Qt::Key_Comma:
            return QStringLiteral("comma");
        case Qt::Key_Period:
            return QStringLiteral("period");
        case Qt::Key_Slash:
            return QStringLiteral("slash");
        case Qt::Key_QuoteLeft:
            return QStringLiteral("grave");
        case Qt::Key_MediaPlay:
        case Qt::Key_MediaTogglePlayPause:
            return QStringLiteral("XF86AudioPlay");
        case Qt::Key_MediaStop:
            return QStringLiteral("XF86AudioStop");
        case Qt::Key_MediaPrevious:
            return QStringLiteral("XF86AudioPrev");
        case Qt::Key_MediaNext:
            return QStringLiteral("XF86AudioNext");
        case Qt::Key_VolumeUp:
            return QStringLiteral("XF86AudioRaiseVolume");
        case Qt::Key_VolumeDown:
            return QStringLiteral("XF86AudioLowerVolume");
        case Qt::Key_VolumeMute:
            return QStringLiteral("XF86AudioMute");
        default:
            break;
    }

    // For alphanumeric keys, use lowercase character representation
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        return QString(QChar(key)).toLower();
    }

    // For digits
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        return QString(QChar(key));
    }

    // Fallback: use QKeySequence string representation
    QString keyStr = QKeySequence(key).toString(QKeySequence::PortableText);
    if (!keyStr.isEmpty()) {
        return keyStr;
    }

    qWarning() << "XDG GlobalShortcuts: unable to map Qt key to XKB name:" << key;
    return {};
}

QString XdgGlobalShortcutManager::generateToken() {
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return QStringLiteral("qownnotes_%1").arg(QRandomGenerator::global()->generate());
#else
    QString token = QUuid::createUuid().toString(QUuid::WithoutBraces);
    token.remove(QLatin1Char('-'));
    return QStringLiteral("qownnotes_%1").arg(token);
#endif
}

QString XdgGlobalShortcutManager::computeRequestPath(const QString &handleToken) const {
    auto bus = QDBusConnection::sessionBus();
    QString senderName = bus.baseService();
    // Remove leading ':' and replace '.' with '_'
    senderName.remove(0, 1);
    senderName.replace(QLatin1Char('.'), QLatin1Char('_'));

    return QStringLiteral("/org/freedesktop/portal/desktop/request/%1/%2")
        .arg(senderName, handleToken);
}
