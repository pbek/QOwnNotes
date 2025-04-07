/**
 *
 * Copyright 2006  Zack Rusin <zack@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */
#include "spellerplugin_p.h"

namespace Sonnet {
class SpellerPluginPrivate {
   public:
    QString language;
};

SpellerPlugin::SpellerPlugin(const QString &lang) : d(new SpellerPluginPrivate) {
    d->language = lang;
}

SpellerPlugin::~SpellerPlugin() { delete d; }

QString SpellerPlugin::language() const { return d->language; }

bool SpellerPlugin::isMisspelled(const QString &word) const { return !isCorrect(word); }

bool SpellerPlugin::checkAndSuggest(const QString &word, QStringList &suggestions) const {
    bool c = isCorrect(word);
    if (!c) {
        suggestions = suggest(word);
    }
    return c;
}
}    // namespace Sonnet
