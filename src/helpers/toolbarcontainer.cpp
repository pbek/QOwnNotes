#include "toolbarcontainer.h"
#include <QMenu>
#include <QMainWindow>
#include <QDebug>
#include <QSettings>
#include <mainwindow.h>

ToolbarContainer::ToolbarContainer(QToolBar *toolbar)
    : name(toolbar->objectName()), title(toolbar->windowTitle()) {
    foreach(QAction* a, toolbar->actions())
        actions.push_back(a->objectName());
}

QToolBar *ToolbarContainer::create(QMainWindow *w) const {
    QToolBar *toolbar = new QToolBar(title, w);
    toolbar->setObjectName(name);

    w->addToolBar(Qt::TopToolBarArea, toolbar);

    foreach(const QString&item, actions)     {
        if ( item.isEmpty() ) {
            toolbar->addSeparator();
        } else {
            QAction* act = w->findChild<QAction*>(item);

            if (!act) {
                QMenu *menu = w->findChild<QMenu*>(item);
                if ( menu ) {
                    act = menu->menuAction();
                }
            }

            if (act) {
                toolbar->addAction(act);
            } else {
                qWarning() << QObject::tr("Unknown action %1").arg(item);
            }

            updateIconSize(toolbar);
        }
    }

    return toolbar;
}

bool ToolbarContainer::toolbarFound() {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == Q_NULLPTR) {
        return false;
    }

    QToolBar* toolbar = mainWindow->findChild<QToolBar*>(name);
    return toolbar != Q_NULLPTR;
}

void ToolbarContainer::updateToolbar() {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == Q_NULLPTR) {
        return;
    }

    QToolBar* toolbar = mainWindow->findChild<QToolBar*>(name);
    if (toolbar == Q_NULLPTR) {
        return;
    }

    toolbar->clear();

    foreach(const QString &item, actions)     {
        if ( item.isEmpty() ) {
            toolbar->addSeparator();
        } else {
            QAction* act = mainWindow->findChild<QAction*>(item);

            if (!act) {
                QMenu *menu = mainWindow->findChild<QMenu*>(item);
                if ( menu ) {
                    act = menu->menuAction();
                }
            }

            if (act) {
                toolbar->addAction(act);
            } else {
                qWarning() << QObject::tr("Unknown action %1").arg(item);
            }

            updateIconSize(toolbar);
        }
    }
}

/**
 * Updates the icon size of a toolbar
 *
 * @param toolbar
 */
void ToolbarContainer::updateIconSize(QToolBar *toolbar) {
    QSettings settings;
    int toolBarIconSize = settings.value(
            "MainWindow/mainToolBar.iconSize").toInt();
    QSize size(toolBarIconSize, toolBarIconSize);
    toolbar->setIconSize(size);
}
