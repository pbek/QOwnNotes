#pragma once

#include <QString>
#include <QToolBar>

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
    ToolbarContainer(QString name, QString title, QStringList actions):
        name(name), title(title), actions(actions)
    {}
    ToolbarContainer(QToolBar* toolbar);
    QToolBar *create(QMainWindow* w) const;

    void updateToolbar();

    bool toolbarFound();

    static void updateIconSize(QToolBar *toolbar);
};
