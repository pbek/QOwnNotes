/*
 * Copyright (C) 2016 Patrizio Bekerle -- http://www.bekerle.com
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
#include <QStringList>
#include <QSettings>
#include <QVariant>
#include <QTextCharFormat>
#include <libraries/qmarkdowntextedit/markdownhighlighter.h>

namespace Utils {
    namespace Schema {
        const int TextPresetIndex = -1;

        QStringList defaultSchemaKeys();

        QString currentSchemaKey();

        bool currentSchemaIsDefault();

        QSettings *getSchemaSettings();

        QVariant getSchemaValue(QString key,
                                QVariant defaultValue = QVariant());

        QString textSettingsKey(QString key, int index);

        QVariant getDefaultTextSchemaValue(QString key,
                                           QVariant defaultValue = QVariant());

        QColor getForegroundColor(int index);

        QColor getBackgroundColor(int index);

        void setFormatStyle(MarkdownHighlighter::HighlighterState index,
                            QTextCharFormat
        &format);

        QFont getEditorTextFont();

        QFont getEditorFixedFont();

        QFont getEditorFont(int index);

        void adaptFontSize(int index, QFont &font);
    }
}
