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

#pragma once

#include <QtGlobal>

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)

#include <QDBusConnection>
#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QDBusVariant>
#include <QKeySequence>
#include <QMap>
#include <QObject>
#include <QPair>
#include <QString>
#include <QVariantMap>

/**
 * Manages global keyboard shortcuts on Wayland via the
 * org.freedesktop.portal.GlobalShortcuts XDG Desktop Portal interface.
 *
 * This class communicates with the portal over D-Bus to register shortcuts
 * and receive activation signals. It is used as an alternative to the
 * QHotkey library, which only works on X11.
 */
class XdgGlobalShortcutManager : public QObject {
    Q_OBJECT

   public:
    explicit XdgGlobalShortcutManager(QObject *parent = nullptr);
    ~XdgGlobalShortcutManager() override;

    /**
     * Check if the GlobalShortcuts portal is available on this system
     */
    static bool isAvailable();

    /**
     * Set all shortcuts to be registered. Call this before createSession().
     * The map key is the shortcut ID (action object name), value contains
     * the QKeySequence and description.
     */
    struct ShortcutInfo {
        QKeySequence keySequence;
        QString description;
    };

    void setShortcuts(const QMap<QString, ShortcutInfo> &shortcuts);

    /**
     * Start the session creation and shortcut registration flow.
     * This is asynchronous; shortcuts become active after the portal responds.
     */
    void createSession();

    /**
     * Close the current session and unregister all shortcuts.
     */
    void closeSession();

    /**
     * Returns true if a session is currently active.
     */
    bool hasActiveSession() const;

   signals:
    /**
     * Emitted when a global shortcut is activated by the user.
     * @param shortcutId The action object name of the triggered shortcut
     */
    void shortcutActivated(const QString &shortcutId);

   private slots:
    void onCreateSessionResponse(uint response, QVariantMap results);
    void onBindShortcutsResponse(uint response, QVariantMap results);
    void onShortcutActivated(QDBusObjectPath sessionHandle, QString shortcutId,
                             qulonglong timestamp, QVariantMap options);

   private:
    static constexpr const char *PORTAL_SERVICE = "org.freedesktop.portal.Desktop";
    static constexpr const char *PORTAL_PATH = "/org/freedesktop/portal/desktop";
    static constexpr const char *PORTAL_INTERFACE = "org.freedesktop.portal.GlobalShortcuts";
    static constexpr const char *REQUEST_INTERFACE = "org.freedesktop.portal.Request";

    /**
     * Convert a QKeySequence to the XDG Shortcuts spec trigger string
     * (e.g. "CTRL+SHIFT+a")
     */
    static QString keySequenceToXdgTrigger(const QKeySequence &seq);

    /**
     * Convert a Qt::Key value to an XKB key name string
     */
    static QString qtKeyToXkbName(int key);

    /**
     * Generate a unique token for portal request handles
     */
    static QString generateToken();

    /**
     * Compute the D-Bus request object path from a handle token
     */
    QString computeRequestPath(const QString &handleToken) const;

    /**
     * Bind the stored shortcuts to the active session
     */
    void bindShortcuts();

    /**
     * Subscribe to Activated/Deactivated signals from the portal
     */
    void subscribeToActivation();

    QString _sessionHandle;
    QMap<QString, ShortcutInfo> _shortcuts;
    bool _subscribed = false;
};

#endif    // Q_OS_UNIX && !Q_OS_MACOS
