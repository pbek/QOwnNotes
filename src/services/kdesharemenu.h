#pragma once

#include <QObject>
#include <QStringList>

class QJsonObject;
class QMenu;
class QWidget;

namespace Purpose {
class Menu;
}

/**
 * Menu to share files with installed Purpose targets, e.g. KDE Connect or email
 * (KDE builds only)
 *
 * Purpose only offers targets whose constraints are fulfilled, e.g. KDE Connect only if
 * its service is running. The menu is hidden if no target is available.
 *
 * The menu needs to outlive the share job, so it is reused instead of being created for
 * each context menu.
 */
class KdeShareMenu : public QObject {
    Q_OBJECT

   public:
    explicit KdeShareMenu(const QString &title, QWidget *parent);

    QMenu *menu() const;

    /**
     * Sets the files that are shared if a target is selected
     *
     * Returns true if targets are available, otherwise the menu is hidden.
     */
    bool setFiles(const QStringList &filePaths);

    static QJsonObject inputData(const QStringList &filePaths);

   signals:
    // Emitted before the files are shared, e.g. to store modified notes
    void aboutToShare();
    // Emitted with a status message if sharing finished or failed, e.g. with a link to an
    // uploaded file
    void finished(const QString &message, bool failed);

   private:
    Purpose::Menu *_menu;
};
