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

#include "services/krunnerservice.h"

class MainWindow;

/**
 * Offers the notes of the current note folder in KDE Plasma's KRunner search (Linux only)
 *
 * Only note names and their folder are returned, never note text. The service is only
 * registered if KRunner's plugin file is installed.
 */
class KRunnerManager : public QObject {
    Q_OBJECT

   public:
    explicit KRunnerManager(MainWindow *mainWindow, QObject *parent = nullptr);

    static const QString pluginFileName;

    static bool isPluginInstalled();
    static KRunnerMatches searchNotes(const QString &query, int limit);

   private:
    MainWindow *_mainWindow;
    KRunnerService *_service;

    void openNote(const QString &matchId, const QString &xdgActivationToken);
};
