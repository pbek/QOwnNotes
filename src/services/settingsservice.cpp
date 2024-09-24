/*
 * Copyright (c) 2014-2024 Patrizio Bekerle -- <patrizio@bekerle.com>
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

SettingsService::SettingsService(QObject *parent) : QObject(parent), m_settings() {}

SettingsService &SettingsService::instance() {
    static SettingsService instance;
    return instance;
}

QVariant SettingsService::value(const QString &key, const QVariant &defaultValue) const {
    QString fullKey = m_group.isEmpty() ? key : m_group + '/' + key;
    if (!m_cache.contains(fullKey)) {
        m_cache[fullKey] = m_settings.value(fullKey, defaultValue);
    }
    return m_cache[fullKey];
}

void SettingsService::setValue(const QString &key, const QVariant &value) {
    QString fullKey = m_group.isEmpty() ? key : m_group + '/' + key;
    m_cache[fullKey] = value;
    m_settings.setValue(fullKey, value);
}

void SettingsService::remove(const QString &key) {
    QString fullKey = m_group.isEmpty() ? key : m_group + '/' + key;
    m_cache.remove(fullKey);
    m_settings.remove(fullKey);
}

bool SettingsService::contains(const QString &key) const {
    QString fullKey = m_group.isEmpty() ? key : m_group + '/' + key;
    return m_cache.contains(fullKey) || m_settings.contains(fullKey);
}

void SettingsService::sync() { m_settings.sync(); }

QStringList SettingsService::allKeys() const { return m_settings.allKeys(); }

void SettingsService::clear() {
    m_cache.clear();
    m_settings.clear();
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
