/*
 * Copyright (c) 2014-2025 Patrizio Bekerle -- <patrizio@bekerle.com>
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

#include "settingsservice.h"

#include <QDebug>

SettingsService::SettingsService(QObject *parent) : QObject(parent), m_settings() {}

SettingsService &SettingsService::instance() {
    static SettingsService instance;
    return instance;
}

QHash<QString, QVariant> *SettingsService::cache() {
    static QHash<QString, QVariant> cache;
    return &cache;
}

QVariant SettingsService::value(const QString &key, const QVariant &defaultValue) const {
    const QString fullKey = getFullKey(key);

    if (!cache()->contains(fullKey)) {
        // We are using "key" instead of "fullKey", because QSettings is already
        // in the group if there was one
        cache()->insert(fullKey, m_settings.value(key, defaultValue));
    }

    return cache()->value(fullKey);
}

void SettingsService::setValue(const QString &key, const QVariant &value) {
    const QString fullKey = getFullKey(key);
    cache()->insert(fullKey, value);

    // We are using "key" instead of "fullKey", because QSettings is already
    // in the group if there was one
    m_settings.setValue(key, value);
}

void SettingsService::remove(const QString &key) {
    const QString fullKey = getFullKey(key);

    // Handle group removal
    if (key.isEmpty() && !m_group.isEmpty()) {
        // Remove all keys in group in QHash cache
        for (auto it = cache()->begin(); it != cache()->end();) {
            if (it.key().startsWith(fullKey)) {
                it = cache()->erase(it);
            } else {
                ++it;
            }
        }

        // Remove all keys in group in QSettings
        m_settings.remove(QLatin1String(""));
    } else {
        // Remove single key
        cache()->remove(fullKey);

        // We are using "key" instead of "fullKey", because QSettings is already
        // in the group if there was one
        m_settings.remove(key);
    }
}

bool SettingsService::contains(const QString &key) const {
    const QString fullKey = getFullKey(key);

    // For m_settings we are using "key" instead of "fullKey", because QSettings
    // is already in the group if there was one
    return cache()->contains(fullKey) || m_settings.contains(key);
}

QString SettingsService::getFullKey(const QString &key) const {
    QString fullKey = m_group.isEmpty() ? key : m_group + '/' + key;

    if (!m_arrayStack.isEmpty()) {
        fullKey = m_arrayStack.last() + '/' + QString::number(m_arrayIndex) + '/' + fullKey;
    }

    return fullKey;
}

void SettingsService::sync() { m_settings.sync(); }

QStringList SettingsService::allKeys() const { return m_settings.allKeys(); }

void SettingsService::clear() {
    cache()->clear();
    m_settings.clear();
    m_settings.sync();
    m_group.clear();
    m_arrayStack.clear();
    m_arrayIndex = 0;
    m_arrayPrefix.clear();
}

void SettingsService::beginGroup(const QString &prefix) {
    m_group = m_group.isEmpty() ? prefix : m_group + '/' + prefix;
    m_settings.beginGroup(prefix);
}

void SettingsService::endGroup() {
    int lastSeparator = m_group.lastIndexOf('/');
    m_group = lastSeparator != -1 ? m_group.left(lastSeparator) : QString();
    m_settings.endGroup();
}

QString SettingsService::group() const { return m_group; }

QString SettingsService::fileName() const { return m_settings.fileName(); }

void SettingsService::beginWriteArray(const QString &prefix, int size) {
    m_arrayStack.append(prefix);
    m_arrayIndex = 0;
    m_settings.beginWriteArray(prefix, size);
}

void SettingsService::setArrayIndex(int i) {
    m_arrayIndex = i;
    m_settings.setArrayIndex(i);
}

void SettingsService::endArray() {
    m_arrayStack.removeLast();
    m_arrayIndex = -1;
    m_settings.endArray();
}

int SettingsService::beginReadArray(const QString &prefix) {
    m_arrayStack.append(prefix);
    m_arrayIndex = 0;
    return m_settings.beginReadArray(prefix);
}
