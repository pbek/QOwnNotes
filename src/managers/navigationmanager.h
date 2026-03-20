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

#include <QObject>

class MainWindow;

namespace Ui {
class MainWindow;
}

class NavigationManager : public QObject {
    Q_OBJECT

   public:
    explicit NavigationManager(MainWindow *mainWindow, Ui::MainWindow *ui,
                               QObject *parent = nullptr);

    void startNavigationParser();
    void updateFileNavigationTab();
    void updateBacklinkNavigationTab();
    void selectNavigationItemAtPosition(int position);
    void setOptionalNavigationTabVisible(QWidget *tab, const QString &title, int preferredIndex,
                                         bool visible);
    void onNavigationWidgetPositionClicked(int position);
    void onFileNavigationWidgetPositionClicked(int position);
    void onNavigationWidgetHeadingRenamed(int position, const QString &oldText,
                                          const QString &newText);
    void updateBacklinksAfterHeadingRename(const QString &oldHeading, const QString &newHeading);
    void noteEditCursorPositionChanged();

   public slots:
    void on_navigationLineEdit_textChanged(const QString &arg1);
    void on_navigationTabWidget_currentChanged(int index);
    void on_actionJump_to_navigation_panel_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
};
