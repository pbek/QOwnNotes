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

#include <QString>

namespace Utils {
namespace ListUtils {
QString normalizeSelectedText(QString text);
QString toggleCheckboxes(const QString &text);
QString createUnorderedList(const QString &text);
QString createOrderedList(const QString &text);
QString createAlphabeticalList(const QString &text);
QString createCheckboxList(const QString &text);
QString clearListFormatting(const QString &text);
QString orderCheckboxes(const QString &text);
}    // namespace ListUtils
}    // namespace Utils
