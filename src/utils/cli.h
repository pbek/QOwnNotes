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

#include <QCommandLineOption>
#include <QList>
#include <QString>

/*  Cli functions that can be useful */

namespace Utils {
namespace Cli {
void generateFishCompletionScript(const QList<QCommandLineOption>& options, const QString& appName);
void generateBashCompletionScript(const QList<QCommandLineOption>& options, const QString& appName);
void generateZshCompletionScript(const QList<QCommandLineOption>& options, const QString& appName);
}    // namespace Cli
}    // namespace Utils
