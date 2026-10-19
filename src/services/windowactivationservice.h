#pragma once

#include <QByteArray>
#include <QString>

class QWidget;

/**
 * Activates windows with the activation information of the user interaction that caused it
 *
 * Wayland compositors only activate a window with an XDG activation token of a recent user
 * interaction, e.g. from the launcher that started another instance or from a global
 * shortcut portal. Qt uses the token from the XDG_ACTIVATION_TOKEN environment variable
 * when a window is activated. On X11, KDE builds pass the startup notification ID of
 * another instance to the window manager.
 *
 * Without such information the compositor's focus stealing prevention decides, e.g. it may
 * only mark the window as demanding attention.
 */
class WindowActivationService {
   public:
    struct Request {
        QString xdgActivationToken;
        QByteArray startupId;
        QString startupAction;
    };

    /**
     * Stores the activation information from the environment of this process
     *
     * Needs to be called before the application object is created, because Qt removes the
     * startup notification ID from the environment on X11.
     */
    static void captureStartupEnvironment();

    /**
     * Message that another instance sends to the primary instance, containing the
     * captured activation information and the startup action
     *
     * The message is never empty, so the primary instance knows that it doesn't need to
     * activate its window without it.
     */
    static QByteArray secondaryInstanceMessage(const QString &startupAction);
    static Request parseSecondaryInstanceMessage(const QByteArray &message);

    /**
     * Lets the next window activation use the token (Wayland only)
     */
    static void setXdgActivationToken(const QString &token);

    /**
     * Removes the token if it wasn't used, so it isn't used for an unrelated activation
     * later or inherited by started processes
     */
    static void clearXdgActivationToken(const QString &token);

    /**
     * Shows, restores, raises, and activates the window
     */
    static void activateWindow(QWidget *window, const QString &xdgActivationToken = QString(),
                               const QByteArray &startupId = QByteArray());
};
