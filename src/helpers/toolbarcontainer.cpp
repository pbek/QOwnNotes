#include "toolbarcontainer.h"

#include <mainwindow.h>

#include <QComboBox>
#include <QDebug>
#include <QMainWindow>
#include <QMenu>
#include <QSettings>
#include <QToolBar>
#include <QWidgetAction>

ToolbarContainer::ToolbarContainer(QToolBar *toolbar)
    : name(toolbar->objectName()), title(toolbar->windowTitle()) {
    foreach (QAction *a, toolbar->actions()) actions.push_back(a->objectName());
}

QToolBar *ToolbarContainer::create(QMainWindow *w) const {
    QToolBar *toolbar = new QToolBar(title, w);
    toolbar->setObjectName(name);

    w->addToolBar(Qt::TopToolBarArea, toolbar);

    foreach (const QString &item, actions) {
        if (item.isEmpty()) {
            toolbar->addSeparator();
        } else {
            QAction *act = w->findChild<QAction *>(item);

            if (!act) {
                QMenu *menu = w->findChild<QMenu *>(item);
                if (menu) {
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
    if (mainWindow == nullptr) {
        return false;
    }

    auto *toolbar = mainWindow->findChild<QToolBar *>(name);
    return toolbar != nullptr;
}

void ToolbarContainer::updateToolbar() {
    MainWindow *mainWindow = MainWindow::instance();
    if (mainWindow == nullptr) {
        return;
    }

    auto *toolbar = mainWindow->findChild<QToolBar *>(name);
    if (toolbar == nullptr) {
        return;
    }

    toolbar->clear();

    foreach (const QString &item, actions) {
        if (item.isEmpty()) {
            toolbar->addSeparator();
        } else {
            // TODO(pbek): we will enable that again later
            if (false) {
                //            if (item == "actionWorkspaceComboBox") {
                qDebug() << __func__ << " - 'actionWorkspaceComboBox': " << item;

                // TODO(pbek): for some reason we can't find the combobox
                auto *workspaceComboBox =
                    mainWindow->findChild<QComboBox *>(QStringLiteral("workspaceComboBox"));

                qDebug() << __func__ << " - 'workspaceComboBox': " << workspaceComboBox;

                auto *widgetAction = mainWindow->findChild<QWidgetAction *>(item);

                qDebug() << __func__ << " - 'widgetAction': " << widgetAction;

                if (widgetAction == nullptr) {
                    widgetAction = new QWidgetAction(mainWindow);
                    widgetAction->setObjectName(QStringLiteral("actionWorkspaceComboBox"));
                    widgetAction->setText(QObject::tr("Workspace selector"));
                }

                widgetAction->setDefaultWidget(workspaceComboBox);
                toolbar->addAction(widgetAction);
            } else {
                auto *action = mainWindow->findChild<QAction *>(item);

                if (!action) {
                    auto *menu = mainWindow->findChild<QMenu *>(item);
                    if (menu) {
                        action = menu->menuAction();
                    }
                }

                if (action != nullptr) {
                    toolbar->addAction(action);
                } else {
                    qWarning() << QObject::tr("Unknown action %1").arg(item);
                }
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
    int toolBarIconSize = settings.value(QStringLiteral("MainWindow/mainToolBar.iconSize")).toInt();
    QSize size(toolBarIconSize, toolBarIconSize);
    toolbar->setIconSize(size);
}
