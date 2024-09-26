#include "test_settingsservice.h"

#include <QDebug>
#include <QtTest>

#include "services/settingsservice.h"

void TestSettingsService::initTestCase() {
    SettingsService settings;
    settings.clear();
}

void TestSettingsService::testSimpleSetAndRead() {
    const auto key = QStringLiteral("test-key");
    const auto value1 = QStringLiteral("test-value");
    const auto value2 = QStringLiteral("test-value2");

    SettingsService settings;

    // Set and test a value
    settings.setValue(key, value1);
    QVERIFY(settings.contains(key));
    QVERIFY(settings.value(key).toString() == value1);
    QVERIFY(QSettings().value(key).toString() == value1);

    // Set and test another value
    settings.setValue(key, value2);
    QVERIFY(settings.value(key).toString() == value2);
    QVERIFY(QSettings().value(key).toString() == value2);

    // Remove the key and test if it was removed
    settings.remove(key);
    QVERIFY(!settings.contains(key));
    QVERIFY(!QSettings().contains(key));
    QVERIFY(settings.value(key).toString() != value1);
    QVERIFY(settings.value(key).toString() != value2);
    QVERIFY(QSettings().value(key).toString() != value1);
    QVERIFY(QSettings().value(key).toString() != value2);
}

void TestSettingsService::testClear() {
    const auto key = QStringLiteral("test-key");
    const auto value = QStringLiteral("test-value");

    SettingsService settings;

    // Set and test a value
    settings.setValue(key, value);
    QVERIFY(settings.contains(key));
    QVERIFY(settings.value(key).toString() == value);
#ifndef Q_OS_MACOS
    // Don't test allKeys() on macOS, because it always puts extra keys in the settings
    QVERIFY(settings.allKeys().count() == 1);
    QVERIFY(QSettings().allKeys().count() == 1);
#endif

    // Clear the settings and test if they were removed
    settings.clear();
    QVERIFY(!settings.contains(key));
    QVERIFY(settings.value(key).toString() != value);
    QVERIFY(!QSettings().contains(key));
    QVERIFY(QSettings().value(key).toString() != value);
#ifndef Q_OS_MACOS
    // Don't test allKeys() on macOS, because it always puts extra keys in the settings
    QVERIFY(settings.allKeys().isEmpty());
    QVERIFY(QSettings().allKeys().isEmpty());
#endif
}

void TestSettingsService::testGroupRemove() {
    const auto group = QStringLiteral("test-group");
    const auto key = QStringLiteral("test-key");
    const auto value = QStringLiteral("test-value");

    SettingsService settings;

    // Set and test a value in a group manually
    settings.setValue(group + "/" + key, value);
    QVERIFY2(settings.value(group + "/" + key).toString() == value, "Value set failed");
    QVERIFY2(QSettings().value(group + "/" + key).toString() == value,
             "QSettings() value set failed");

    // Remove the group and test if it was removed
    settings.beginGroup(group);
    settings.remove(QLatin1String(""));
    settings.endGroup();
    QVERIFY2(settings.value(group + "/" + key).toString() != value, "Group remove failed");
    QVERIFY2(QSettings().value(group + "/" + key).toString() != value,
             "QSettings() group remove failed");
}

void TestSettingsService::testGroupRead() {
    const auto group = QStringLiteral("test-group");
    const auto key = QStringLiteral("test-key");
    const auto value = QStringLiteral("test-value");

    SettingsService settings;

    // Set and test a value in a group manually
    settings.setValue(group + "/" + key, value);
    QVERIFY2(settings.value(group + "/" + key).toString() == value, "Value set failed");
    QVERIFY2(QSettings().value(group + "/" + key).toString() == value,
             "QSettings value set failed");

    // Read the value in the group and test if it was read
    settings.beginGroup(group);
    QVERIFY2(settings.value(key).toString() == value, "Group read failed");
    settings.endGroup();
}

void TestSettingsService::testGroupSet() {
    const auto group = QStringLiteral("test-group");
    const auto key = QStringLiteral("test-key");
    const auto value = QStringLiteral("test-value");

    SettingsService settings;

    // Set value in a group and test if it was set
    settings.beginGroup(group);
    settings.setValue(key, value);
    QVERIFY2(settings.value(key).toString() == value, "Value group set failed");
    settings.endGroup();

    // Test if the value was set in the group manually
    QVERIFY2(settings.value(group + "/" + key).toString() == value, "Value set failed");
    QVERIFY2(QSettings().value(group + "/" + key).toString() == value,
             "QSettings value set failed");
}

void TestSettingsService::testArraySetAndRead() {
    const auto prefix = QStringLiteral("test-array");
    const auto key = QStringLiteral("test-key");
    const auto value1 = QStringLiteral("test-value1");
    const auto value2 = QStringLiteral("test-value2");

    SettingsService settings;

    // Set and test an array value
    settings.beginWriteArray(prefix, 2);
    settings.setArrayIndex(0);
    settings.setValue(key, value1);
    settings.setArrayIndex(1);
    settings.setValue(key, value2);
    settings.endArray();

    QVERIFY(settings.beginReadArray(prefix) == 2);
    settings.setArrayIndex(0);
    QVERIFY(settings.value(key).toString() == value1);
    settings.setArrayIndex(1);
    QVERIFY(settings.value(key).toString() == value2);
    settings.endArray();

    // Index starts from 1 (see https://doc.qt.io/qt-6/qsettings.html#beginWriteArray)
    QVERIFY(QSettings().value(prefix + "/1/" + key).toString() == value1);
    QVERIFY(QSettings().value(prefix + "/2/" + key).toString() == value2);
    QVERIFY(QSettings().value(prefix + "/size").toInt() == 2);
}
