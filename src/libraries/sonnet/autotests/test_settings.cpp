/**
 * test_settings.cpp
 *
 * Copyright (C)  2015  Kåre Särs <kare.sars@iki.fi>
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

#include "test_settings.h"

#include "speller.h"
#include "settings_p.h"

#include <QTest>
#include <QSettings>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QStandardPaths>

QTEST_GUILESS_MAIN(SonnetSettingsTest)

using namespace Sonnet;

void SonnetSettingsTest::initTestCase()
{
    QStandardPaths::setTestModeEnabled(true);
}

void SonnetSettingsTest::testRestoreDoesNotSave()
{
    QSettings settings(QStringLiteral("KDE"), QStringLiteral("Sonnet"));
    QString fileName = settings.fileName();

    QDateTime startTime = QFileInfo(fileName).lastModified();

    // NOTE: We use new/delete to be able to test that the settings are not
    // needlessly saved on deletion of Speller
    Speller *speller = new Speller();
    // NOTE: This test works on Unix, but should _not_ fail on Windows as
    // QFileInfo::lastModified() always returns invalid QDateTime
    QCOMPARE(QFileInfo(fileName).lastModified(), startTime);

    speller->language();
    QCOMPARE(QFileInfo(fileName).lastModified(), startTime);

    QStringList langs = speller->availableLanguages();
    for (int i = 0; i < langs.count(); ++i) {
        speller->setLanguage(langs[i]);
    }
    QCOMPARE(QFileInfo(fileName).lastModified(), startTime);

    speller->availableLanguages();
    QCOMPARE(QFileInfo(fileName).lastModified(), startTime);

    speller->restore();
    QCOMPARE(QFileInfo(fileName).lastModified(), startTime);

    // Test that the settings are not saved needlessly on delete
    delete speller;
    QCOMPARE(QFileInfo(fileName).lastModified(), startTime);
}

void SonnetSettingsTest::testSpellerAPIChangeSaves()
{
    QSettings settings(QStringLiteral("KDE"), QStringLiteral("Sonnet"));
    Speller speller;

    // Default Language
    QString defLang = speller.defaultLanguage();
    QString settingsLang = settings.value(QStringLiteral("defaultLanguage"),
                                          QLocale::system().name()).toString();
    QCOMPARE(defLang, settingsLang);

    QStringList langs = speller.availableLanguages();
    for (int i = 0; i < langs.count(); ++i) {
        if (langs[i] != defLang) {
            speller.setDefaultLanguage(langs[i]);
            settingsLang = settings.value(QStringLiteral("defaultLanguage"),
                                          QLocale::system().name()).toString();
            QCOMPARE(settingsLang, langs[i]);
            QCOMPARE(speller.defaultLanguage(), langs[i]);
            break;
        }
    }
    // set the original value
    speller.setDefaultLanguage(defLang);
    settingsLang
        = settings.value(QStringLiteral("defaultLanguage"),
                         QLocale::system().name()).toString();
    QCOMPARE(settingsLang, defLang);
    QCOMPARE(speller.defaultLanguage(), defLang);

    // Default Client
    QString defClient = speller.defaultClient();
    QString settingsClient = settings.value(QStringLiteral("defaultClient"), QString()).toString();
    QCOMPARE(defClient, settingsClient);

    QStringList clients = speller.availableBackends();
    qDebug() << clients;
    for (int i = 0; i < clients.count(); ++i) {
        if (clients[i] != defLang) {
            speller.setDefaultClient(clients[i]);
            settingsClient = settings.value(QStringLiteral("defaultClient"), QString()).toString();
            QCOMPARE(settingsClient, clients[i]);
            QCOMPARE(speller.defaultClient(), clients[i]);
            break;
        }
    }
    // set the original value
    if (defClient.isEmpty()) {
        // setting default to "" does not work.
        settings.remove(QStringLiteral("defaultClient"));
    } else {
        speller.setDefaultClient(defClient);
    }
    settingsClient = settings.value(QStringLiteral("defaultClient"), QString()).toString();
    QCOMPARE(settingsClient, defClient);
    if (!defClient.isEmpty()) {
        QCOMPARE(speller.defaultClient(), defClient);
    }

    // Check uppercase
    bool checkUppercase = speller.testAttribute(Speller::CheckUppercase);
    bool settingsUppercase = settings.value(QStringLiteral("checkUppercase"), true).toBool();
    QCOMPARE(checkUppercase, settingsUppercase);
    // Change the atribute
    speller.setAttribute(Speller::CheckUppercase, !checkUppercase);
    settingsUppercase = settings.value(QStringLiteral("checkUppercase"), true).toBool();
    QCOMPARE(!checkUppercase, settingsUppercase);
    QCOMPARE(!checkUppercase, speller.testAttribute(Speller::CheckUppercase));
    //now set it back to what it was
    speller.setAttribute(Speller::CheckUppercase, checkUppercase);
    settingsUppercase = settings.value(QStringLiteral("checkUppercase"), true).toBool();
    QCOMPARE(checkUppercase, settingsUppercase);
    QCOMPARE(checkUppercase, speller.testAttribute(Speller::CheckUppercase));

    // Skip Run Together
    bool skipRunTogether = speller.testAttribute(Speller::SkipRunTogether);
    bool settingsSkipRunTogether = settings.value(QStringLiteral("skipRunTogether"), true).toBool();
    QCOMPARE(skipRunTogether, settingsSkipRunTogether);
    // Change the atribute
    speller.setAttribute(Speller::SkipRunTogether, !skipRunTogether);
    settingsSkipRunTogether = settings.value(QStringLiteral("skipRunTogether"), true).toBool();
    QCOMPARE(!skipRunTogether, settingsSkipRunTogether);
    QCOMPARE(!skipRunTogether, speller.testAttribute(Speller::SkipRunTogether));
    //now set it back to what it was
    speller.setAttribute(Speller::SkipRunTogether, skipRunTogether);
    settingsSkipRunTogether = settings.value(QStringLiteral("skipRunTogether"), true).toBool();
    QCOMPARE(skipRunTogether, settingsSkipRunTogether);
    QCOMPARE(skipRunTogether, speller.testAttribute(Speller::SkipRunTogether));

    // Auto Detect Language
    bool autodetectLanguage = speller.testAttribute(Speller::AutoDetectLanguage);
    bool settingsAutoDetectLanguage = settings.value(QStringLiteral("autodetectLanguage"), true).toBool();
    QCOMPARE(autodetectLanguage, settingsAutoDetectLanguage);
    // Change the atribute
    speller.setAttribute(Speller::AutoDetectLanguage, !autodetectLanguage);
    settingsAutoDetectLanguage = settings.value(QStringLiteral("autodetectLanguage"), true).toBool();
    QCOMPARE(!autodetectLanguage, settingsAutoDetectLanguage);
    QCOMPARE(!autodetectLanguage, speller.testAttribute(Speller::AutoDetectLanguage));
    //now set it back to what it was
    speller.setAttribute(Speller::AutoDetectLanguage, autodetectLanguage);
    settingsAutoDetectLanguage = settings.value(QStringLiteral("autodetectLanguage"), true).toBool();
    QCOMPARE(autodetectLanguage, settingsAutoDetectLanguage);
    QCOMPARE(autodetectLanguage, speller.testAttribute(Speller::AutoDetectLanguage));
}
