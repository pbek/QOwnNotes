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

#include "settingsservice.h"

#include <services/cryptoservice.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMutexLocker>

namespace {
QStringList keychainReferencesForRemoval(const QSettings &settings, const QString &key) {
    QStringList references;
    const QString value = settings.value(key).toString();

    if (CryptoService::isKeychainReference(value)) {
        references.append(value);
    }

    const QString prefix = key + QLatin1Char('/');
    for (const QString &childKey : settings.allKeys()) {
        if (!childKey.startsWith(prefix)) {
            continue;
        }

        const QString childValue = settings.value(childKey).toString();
        if (CryptoService::isKeychainReference(childValue) && !references.contains(childValue)) {
            references.append(childValue);
        }
    }

    return references;
}
}    // namespace

SettingsService::SettingsService(QObject *parent) : m_settings() { Q_UNUSED(parent) }

SettingsService &SettingsService::instance() {
    static SettingsService instance;
    return instance;
}

QHash<QString, QVariant> *SettingsService::cache() {
    static QHash<QString, QVariant> cache;
    return &cache;
}

QMutex *SettingsService::cacheMutex() {
    static QMutex mutex;
    return &mutex;
}

QSet<QString> *SettingsService::overrideKeys() {
    static QSet<QString> keys;
    return &keys;
}

QSet<QString> *SettingsService::overrideOverwrittenKeys() {
    static QSet<QString> keys;
    return &keys;
}

QString *SettingsService::overrideFileName() {
    static QString fileName;
    return &fileName;
}

bool *SettingsService::overrideLoaded() {
    static bool loaded = false;
    return &loaded;
}

QString SettingsService::overrideSettingsFileName() {
    QSettings settings;
    const QFileInfo fileInfo(settings.fileName());
    const QString suffix = fileInfo.completeSuffix();
    const QString fileName =
        suffix.isEmpty() ? fileInfo.fileName() + QStringLiteral(".override")
                         : fileInfo.completeBaseName() + QStringLiteral(".override.") + suffix;

    return fileInfo.dir().filePath(fileName);
}

void SettingsService::loadOverrideSettings(bool forceReload) {
    {
        QMutexLocker locker(cacheMutex());

        if (*overrideLoaded() && !forceReload) {
            return;
        }
    }

    const QString currentOverrideFileName = overrideSettingsFileName();

    if (!QFileInfo::exists(currentOverrideFileName)) {
        QMutexLocker locker(cacheMutex());

        if (*overrideLoaded() && !forceReload) {
            return;
        }

        for (const QString &key : *overrideKeys()) {
            cache()->remove(key);
        }
        overrideKeys()->clear();
        overrideOverwrittenKeys()->clear();
        *overrideFileName() = currentOverrideFileName;
        *overrideLoaded() = true;
        return;
    }

    QHash<QString, QVariant> loadedValues;
    QStringList loadedKeys;
    QStringList overwrittenKeys;

    QSettings normalSettings;
    QSettings overrideSettings(currentOverrideFileName, QSettings::IniFormat);
    loadedKeys = overrideSettings.allKeys();

    for (const QString &key : loadedKeys) {
        loadedValues.insert(key, overrideSettings.value(key));

        if (normalSettings.contains(key)) {
            overwrittenKeys.append(key);
        }
    }

    {
        QMutexLocker locker(cacheMutex());

        if (*overrideLoaded() && !forceReload) {
            return;
        }

        for (const QString &key : *overrideKeys()) {
            cache()->remove(key);
        }
        overrideKeys()->clear();
        overrideOverwrittenKeys()->clear();

        *overrideFileName() = currentOverrideFileName;

        for (const QString &key : loadedKeys) {
            overrideKeys()->insert(key);
            cache()->insert(key, loadedValues.value(key));
        }

        for (const QString &key : overwrittenKeys) {
            overrideOverwrittenKeys()->insert(key);
        }

        *overrideLoaded() = true;
    }

    qDebug() << "Loaded settings override file:" << currentOverrideFileName
             << "loaded override settings:" << loadedKeys
             << "existing settings overwritten by override:" << overwrittenKeys;
}

bool SettingsService::overrideSettingsLoaded() { return *overrideLoaded(); }

QStringList SettingsService::overrideSettingsKeys() {
    QMutexLocker locker(cacheMutex());
    QStringList keys;
    for (const QString &key : *overrideKeys()) {
        keys.append(key);
    }
    return keys;
}

QStringList SettingsService::overrideSettingsOverwrittenKeys() {
    QMutexLocker locker(cacheMutex());
    QStringList keys;
    for (const QString &key : *overrideOverwrittenKeys()) {
        keys.append(key);
    }
    return keys;
}

QVariant SettingsService::value(const QString &key, const QVariant &defaultValue) const {
    const QString fullKey = getFullKey(key);
    QMutexLocker locker(cacheMutex());

    if (!cache()->contains(fullKey)) {
        // We are using "key" instead of "fullKey", because QSettings is already
        // in the group if there was one
        cache()->insert(fullKey, m_settings.value(key, defaultValue));
    }

    return cache()->value(fullKey);
}

void SettingsService::setValue(const QString &key, const QVariant &value) {
    const QString fullKey = getFullKey(key);
    {
        QMutexLocker locker(cacheMutex());
        if (!overrideKeys()->contains(fullKey)) {
            cache()->insert(fullKey, value);
        }
    }

    // We are using "key" instead of "fullKey", because QSettings is already
    // in the group if there was one
    m_settings.setValue(key, value);
}

void SettingsService::remove(const QString &key) {
    const QString fullKey = getFullKey(key);
    QStringList keychainReferences;

    // Handle group removal
    if (key.isEmpty() && !m_group.isEmpty()) {
        keychainReferences = CryptoService::keychainReferencesFromSettings(m_settings);

        // Remove all keys in group in QHash cache
        QMutexLocker locker(cacheMutex());
        for (auto it = cache()->begin(); it != cache()->end();) {
            if (it.key().startsWith(fullKey) && !overrideKeys()->contains(it.key())) {
                it = cache()->erase(it);
            } else {
                ++it;
            }
        }

        // Remove all keys in group in QSettings
        m_settings.remove(QLatin1String(""));
    } else {
        keychainReferences = keychainReferencesForRemoval(m_settings, key);

        // Remove single key
        QMutexLocker locker(cacheMutex());
        if (!overrideKeys()->contains(fullKey)) {
            cache()->remove(fullKey);
        }

        // We are using "key" instead of "fullKey", because QSettings is already
        // in the group if there was one
        m_settings.remove(key);
    }

    if (!keychainReferences.isEmpty() && qApp != nullptr) {
        CryptoService::instance()->deleteSecrets(keychainReferences);
    }
}

bool SettingsService::contains(const QString &key) const {
    const QString fullKey = getFullKey(key);
    QMutexLocker locker(cacheMutex());

    // For m_settings we are using "key" instead of "fullKey", because QSettings
    // is already in the group if there was one
    return cache()->contains(fullKey) || m_settings.contains(key);
}

QString SettingsService::getFullKey(const QString &key) const {
    QString fullKey = m_group.isEmpty() ? key : m_group + '/' + key;

    if (!m_arrayStack.isEmpty()) {
        fullKey = m_arrayStack.last() + '/' + QString::number(m_arrayIndex + 1) + '/' + fullKey;
    }

    return fullKey;
}

void SettingsService::sync() { m_settings.sync(); }

QStringList SettingsService::allKeys() const {
    QStringList keys = m_settings.allKeys();

    {
        QMutexLocker locker(cacheMutex());
        for (const QString &key : *overrideKeys()) {
            if (!keys.contains(key)) {
                keys.append(key);
            }
        }
    }

    return keys;
}

void SettingsService::clear() {
    const QStringList keychainReferences =
        CryptoService::keychainReferencesFromSettings(m_settings);

    {
        QMutexLocker locker(cacheMutex());
        for (auto it = cache()->begin(); it != cache()->end();) {
            if (overrideKeys()->contains(it.key())) {
                ++it;
            } else {
                it = cache()->erase(it);
            }
        }
    }
    m_settings.clear();
    m_settings.sync();
    m_group.clear();
    m_arrayStack.clear();
    m_arrayIndex = 0;
    m_arrayPrefix.clear();

    if (!keychainReferences.isEmpty() && qApp != nullptr) {
        CryptoService::instance()->deleteSecrets(keychainReferences);
    }
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
    const int size = m_settings.beginReadArray(prefix);

    const QString fullPrefix = m_group.isEmpty() ? prefix : m_group + '/' + prefix;
    const QString sizeKey = fullPrefix + QStringLiteral("/size");
    QMutexLocker locker(cacheMutex());
    return cache()->contains(sizeKey) ? cache()->value(sizeKey).toInt() : size;
}
