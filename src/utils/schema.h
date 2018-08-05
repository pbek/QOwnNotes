/*
 * Copyright (c) 2014-2018 Patrizio Bekerle -- http://www.bekerle.com
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
        /**
         * @brief The Settings class
         *
         * This exists to centralize schema settings access, reducing redundant
         * work whenever possible.
         */
        class Settings {
        public:
            Settings();

            const QStringList& defaultSchemaKeys() const;
            const QSettings& defaultSchemaSettings() const;

            QString currentSchemaKey() const;
            bool currentSchemaIsDefault() const;

            QStringList getSchemaKeys(const QString& schema) const;

            QVariant getSchemaValue(const QString& key,
                                    const QVariant& defaultValue = QVariant()) const;
            QColor getForegroundColor(int index) const;
            QColor getBackgroundColor(int index) const;

            void setFormatStyle(MarkdownHighlighter::HighlighterState index,
                                QTextCharFormat &format) const;

            QFont getEditorTextFont() const;
            QFont getEditorFixedFont() const;
            QFont getEditorFont(int index) const;

            void adaptFontSize(int index, QFont &font) const;

        private:
            const QSettings _defaultSchemaSettings;
            QMap<QString, int> _defaultSchemaSubkeys;
            QStringList _defaultSchemaKeysList;
            QVector<QStringList> _defaultSchemaSubkeylists;
            mutable QFont _defaultTextEditFont;
            mutable bool _defaultFontSet;
        };

        extern Settings* schemaSettings;
        const int TextPresetIndex = -1;

        QString textSettingsKey(const QString& key, int index);
    }
}
