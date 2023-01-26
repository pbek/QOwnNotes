#pragma once

#include <QString>
#include <QStringList>
class QToolBar;
class QMainWindow;

/**
 *  This class holds a string definition of toolbars and actions
 *
 *  Separators are stored as empty strings
 */
struct ToolbarContainer {
    QString name;
    QString title;
    QStringList actions;

    ToolbarContainer() {}
    ToolbarContainer(const QString &name, const QString &title, const QStringList &actions)
        : name(name), title(title), actions(actions) {}
    ToolbarContainer(QToolBar *toolbar);
    QToolBar *create(QMainWindow *w) const;

    void updateToolbar();

    bool toolbarFound();

    static void updateIconSize(QToolBar *toolbar);
};
