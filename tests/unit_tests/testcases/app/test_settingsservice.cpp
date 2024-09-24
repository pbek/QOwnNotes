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

    // Set and test another value
    settings.setValue(key, value2);
    QVERIFY(settings.value(key).toString() == value2);

    // Remove the key and test if it was removed
    settings.remove(key);
    QVERIFY(!settings.contains(key));
    QVERIFY(settings.value(key).toString() != value1);
    QVERIFY(settings.value(key).toString() != value2);
}

void TestSettingsService::testGroupRemove() {
    const auto group = QStringLiteral("test-group");
    const auto key = QStringLiteral("test-key");
    const auto value = QStringLiteral("test-value");

    SettingsService settings;

    // Set and test a value in a group manually
    settings.setValue(group + "/" + key, value);
    QVERIFY2(settings.value(group + "/" + key).toString() == value, "Value set failed");

    // Remove the group and test if it was removed
    settings.beginGroup(group);
    settings.remove(QLatin1String(""));
    settings.endGroup();
    QVERIFY2(settings.value(group + "/" + key).toString() != value, "Group remove failed");
}

void TestSettingsService::testGroupRead() {
    const auto group = QStringLiteral("test-group");
    const auto key = QStringLiteral("test-key");
    const auto value = QStringLiteral("test-value");

    SettingsService settings;

    // Set and test a value in a group manually
    settings.setValue(group + "/" + key, value);
    QVERIFY2(settings.value(group + "/" + key).toString() == value, "Value set failed");

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
}

void TestSettingsService::testArraySetAndRead() {
    const auto key = QStringLiteral("test-array");
    const auto value1 = QStringLiteral("test-value1");
    const auto value2 = QStringLiteral("test-value2");

    SettingsService settings;

    // Set and test an array value
    settings.beginWriteArray(key, 2);
    settings.setArrayIndex(0);
    settings.setValue(key, value1);
    settings.setArrayIndex(1);
    settings.setValue(key, value2);
    settings.endArray();

    QVERIFY(settings.beginReadArray(key) == 2);
    settings.setArrayIndex(0);
    QVERIFY(settings.value(key).toString() == value1);
    settings.setArrayIndex(1);
    QVERIFY(settings.value(key).toString() == value2);
    settings.endArray();
}
