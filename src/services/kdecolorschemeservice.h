#pragma once

#include <QList>
#include <QString>

/**
 * Applies KDE color schemes to the application palette (KDE builds only)
 *
 * KColorSchemeManager isn't used, because it stores the scheme in its own configuration
 * file and automatically applies Breeze on desktops without the Plasma platform theme.
 * Here, the system palette is left untouched unless a scheme was selected.
 */
class KdeColorSchemeService {
   public:
    struct Scheme {
        // File name without the ".colors" extension, as used by KColorSchemeManager
        QString id;
        QString name;
        QString path;
    };

    // Settings key of the selected scheme ID; empty follows the system
    static const QString settingsKey;

    static QList<Scheme> availableSchemes();

    /**
     * Applies the scheme with the given ID, or restores the system palette if the ID is
     * empty, unknown, or if the QDarkStyle dark mode theme is used, because its stylesheet
     * takes precedence over the palette
     *
     * Returns true if a scheme is applied
     */
    static bool apply(const QString &schemeId, bool darkModeTheme);

    // ID of the applied scheme, empty if the system palette is used
    static QString activeSchemeId();
};
