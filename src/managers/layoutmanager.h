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

#include <QHash>
#include <QObject>
#include <QString>
#include <QStringList>

class MainWindow;
class QComboBox;
class QDockWidget;

namespace Ui {
class MainWindow;
}

class LayoutManager : public QObject {
    Q_OBJECT

   public:
    explicit LayoutManager(MainWindow *mainWindow, Ui::MainWindow *ui, QObject *parent = nullptr);

    void initLayoutComboBox();
    void updateLayoutLists(bool rebuild = true);
    void setCurrentLayout(const QString &uuid);
    void storeCurrentLayout();
    void restoreCurrentLayout();
    bool createNewLayout(QString name);
    bool removeLayout(const QString &uuid);
    bool renameLayout(const QString &uuid, const QString &name);
    bool setLayoutOrder(const QStringList &uuids);
    bool layoutNoteEditIsCentralWidget(const QString &uuid) const;
    void setLayoutNoteEditIsCentralWidget(const QString &uuid, bool enabled);
    QString currentLayoutUuid();
    QStringList getLayoutUuidList();
    QString getLayoutUuid(const QString &layoutName);

    QComboBox *layoutComboBox() const { return _layoutComboBox; }

    // Panel management
    void handleDockWidgetLocking(QDockWidget *dockWidget);

   public slots:
    void onLayoutComboBoxCurrentIndexChanged(int index);
    void on_actionStore_as_new_layout_triggered();
    void on_actionRemove_current_layout_triggered();
    void on_actionRename_current_layout_triggered();
    void on_actionSwitch_to_previous_layout_triggered();
    void on_actionUnlock_panels_toggled(bool arg1);
    void on_actionShow_all_panels_triggered();
    void on_actionReattach_panels_triggered();

   private:
    MainWindow *_mainWindow;
    Ui::MainWindow *_ui;
    QComboBox *_layoutComboBox = nullptr;
    QHash<QString, QString> _layoutNameUuidMap;
};
