#include "test_todosettingswidget.h"

#include <QListWidget>
#include <QRadioButton>
#include <QSignalSpy>
#include <QUrl>
#include <QtTest>

#include "services/settingsservice.h"
#include "widgets/settings/todosettingswidget.h"

namespace {
const QString serverUrl = QStringLiteral("https://cloud.example/nextcloud");
const QStringList names = {QStringLiteral("Work"), QStringLiteral("Personal")};
const QStringList urls = {
    QStringLiteral("https://cloud.example/nextcloud/remote.php/dav/calendars/user/work/"),
    QStringLiteral("https://cloud.example/nextcloud/remote.php/dav/calendars/user/personal/")};

void storeCachedCalendars(const QStringList &enabledNames, const QStringList &enabledUrls) {
    SettingsService settings;
    settings.setValue(QStringLiteral("ownCloud/todoCalendarUrlList"), urls);
    settings.setValue(QStringLiteral("ownCloud/todoCalendarDisplayNameList"), names);
    settings.setValue(QStringLiteral("ownCloud/todoCalendarEnabledList"), enabledNames);
    settings.setValue(QStringLiteral("ownCloud/todoCalendarEnabledUrlList"), enabledUrls);
}
}    // namespace

void TestTodoSettingsWidget::init() {
    SettingsService settings;
    settings.clear();
    settings.setValue(QStringLiteral("todoCalendarSupport"), true);
    settings.setValue(QStringLiteral("ownCloud/todoCalendarCalDAVServerUrl"), serverUrl);
}

void TestTodoSettingsWidget::cleanup() { SettingsService().clear(); }

void TestTodoSettingsWidget::testCachedSelectionsSurviveReadAndStore_data() {
    QTest::addColumn<int>("backend");
    QTest::addColumn<bool>("selectCalendar");

    for (int backend :
         {int(CloudService::DefaultCloudCalendar), int(CloudService::LegacyOwnCloudCalendar),
          int(CloudService::CalendarPlus), int(CloudService::CalDAVCalendar)}) {
        QTest::newRow(qPrintable(QStringLiteral("backend-%1-selected").arg(backend)))
            << backend << true;
        QTest::newRow(qPrintable(QStringLiteral("backend-%1-unselected").arg(backend)))
            << backend << false;
    }
}

void TestTodoSettingsWidget::testCachedSelectionsSurviveReadAndStore() {
    QFETCH(int, backend);
    QFETCH(bool, selectCalendar);
    const QStringList enabledNames = selectCalendar ? QStringList{names.first()} : QStringList{};
    const QStringList enabledUrls = selectCalendar ? QStringList{urls.first()} : QStringList{};
    storeCachedCalendars(enabledNames, enabledUrls);
    SettingsService settings;
    settings.setValue(QStringLiteral("ownCloud/todoCalendarBackend"), backend);

    TodoSettingsWidget widget;
    QSignalSpy reloadSpy(&widget, &TodoSettingsWidget::reloadCalendarListRequested);
    QSignalSpy storeSpy(&widget, &TodoSettingsWidget::storeSettingsRequested);
    // CalDAV must use its own configured URL, not the cloud settings URL.
    widget.readSettings(backend == CloudService::CalDAVCalendar ? QString() : serverUrl);

    auto *list = widget.findChild<QListWidget *>(QStringLiteral("todoCalendarListWidget"));
    QVERIFY(list);
    QCOMPARE(list->count(), 2);
    QCOMPARE(list->item(0)->checkState(), selectCalendar ? Qt::Checked : Qt::Unchecked);
    QCOMPARE(list->item(1)->checkState(), Qt::Unchecked);
    widget.storeSettings();
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarUrlList")).toStringList(), urls);
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarDisplayNameList")).toStringList(),
             names);
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarEnabledList")).toStringList(),
             enabledNames);
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarEnabledUrlList")).toStringList(),
             enabledUrls);
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarBackend")).toInt(), backend);
    QTest::qWait(350);
    QCOMPARE(reloadSpy.count(), 0);
    QCOMPARE(storeSpy.count(), 0);
}

void TestTodoSettingsWidget::testInvalidRefreshPreservesSelections_data() {
    QTest::addColumn<QString>("invalidUrl");
    QTest::newRow("empty") << QString();
    QTest::newRow("malformed") << QStringLiteral("https://[invalid");
}

void TestTodoSettingsWidget::testInvalidRefreshPreservesSelections() {
    QFETCH(QString, invalidUrl);
    storeCachedCalendars({names.first()}, {urls.first()});
    TodoSettingsWidget widget;
    widget.readSettings(serverUrl);
    widget.refreshTodoCalendarList({}, invalidUrl);
    widget.storeSettings();

    SettingsService settings;
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarUrlList")).toStringList(), urls);
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarEnabledList")).toStringList(),
             QStringList{names.first()});
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarEnabledUrlList")).toStringList(),
             QStringList{urls.first()});
}

void TestTodoSettingsWidget::testRefreshPreservesCheckedState() {
    storeCachedCalendars({names.first()}, {urls.first()});
    TodoSettingsWidget widget;
    widget.readSettings(serverUrl);
    QList<CalDAVCalendarData> calendars;
    for (int i = 0; i < names.count(); ++i) {
        CalDAVCalendarData data;
        data.displayName = names.at(i);
        // Server replies may contain paths relative to the server's origin.
        data.url = QUrl(urls.at(i)).path();
        calendars.append(data);
    }
    widget.refreshTodoCalendarList(calendars, serverUrl);
    widget.storeSettings();

    SettingsService settings;
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarUrlList")).toStringList(), urls);
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarEnabledList")).toStringList(),
             QStringList{names.first()});
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarEnabledUrlList")).toStringList(),
             QStringList{urls.first()});
}

void TestTodoSettingsWidget::testRefreshPreservesAbsoluteUrls() {
    const QString otherServerUrl = QStringLiteral("https://tasks.example/calendars/user/tasks/");
    SettingsService settings;
    settings.setValue(QStringLiteral("ownCloud/todoCalendarUrlList"), QStringList{otherServerUrl});
    settings.setValue(QStringLiteral("ownCloud/todoCalendarDisplayNameList"),
                      QStringList{names.first()});
    settings.setValue(QStringLiteral("ownCloud/todoCalendarEnabledList"),
                      QStringList{names.first()});
    settings.setValue(QStringLiteral("ownCloud/todoCalendarEnabledUrlList"),
                      QStringList{otherServerUrl});

    TodoSettingsWidget widget;
    widget.readSettings(serverUrl);
    widget.storeSettings();
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarUrlList")).toStringList(),
             QStringList{otherServerUrl});
    QCOMPARE(settings.value(QStringLiteral("ownCloud/todoCalendarEnabledUrlList")).toStringList(),
             QStringList{otherServerUrl});
}

void TestTodoSettingsWidget::testValidEmptyRefreshClearsCalendars() {
    storeCachedCalendars({names.first()}, {urls.first()});
    TodoSettingsWidget widget;
    widget.readSettings(serverUrl);
    widget.refreshTodoCalendarList({}, serverUrl);
    widget.storeSettings();

    SettingsService settings;
    QVERIFY(
        settings.value(QStringLiteral("ownCloud/todoCalendarUrlList")).toStringList().isEmpty());
    QVERIFY(settings.value(QStringLiteral("ownCloud/todoCalendarEnabledUrlList"))
                .toStringList()
                .isEmpty());
}

void TestTodoSettingsWidget::testDisabledSupportDoesNotRequestReload() {
    SettingsService().setValue(QStringLiteral("todoCalendarSupport"), false);
    TodoSettingsWidget widget;
    QSignalSpy reloadSpy(&widget, &TodoSettingsWidget::reloadCalendarListRequested);
    widget.readSettings(serverUrl);
    QTest::qWait(350);
    QCOMPARE(reloadSpy.count(), 0);
}

void TestTodoSettingsWidget::testEmptyCacheRequestsReload() {
    TodoSettingsWidget widget;
    QSignalSpy reloadSpy(&widget, &TodoSettingsWidget::reloadCalendarListRequested);
    widget.readSettings(serverUrl);
    QCOMPARE(reloadSpy.count(), 1);

    CalDAVCalendarData data;
    data.displayName = names.first();
    data.url = urls.first();
    widget.refreshTodoCalendarList({data}, serverUrl);
    widget.storeSettings();
    QCOMPARE(SettingsService()
                 .value(QStringLiteral("ownCloud/todoCalendarEnabledUrlList"))
                 .toStringList(),
             QStringList{urls.first()});
}

void TestTodoSettingsWidget::testBackendReloadIsDeferred() {
    storeCachedCalendars({names.first()}, {urls.first()});
    TodoSettingsWidget widget;
    widget.readSettings(serverUrl);
    QSignalSpy reloadSpy(&widget, &TodoSettingsWidget::reloadCalendarListRequested);
    QSignalSpy storeSpy(&widget, &TodoSettingsWidget::storeSettingsRequested);
    auto *calDav = widget.findChild<QRadioButton *>(QStringLiteral("calDavCalendarRadioButton"));
    auto *calendarPlus =
        widget.findChild<QRadioButton *>(QStringLiteral("calendarPlusRadioButton"));
    QVERIFY(calDav);
    QVERIFY(calendarPlus);
    calDav->setChecked(true);
    calendarPlus->setChecked(true);
    QCOMPARE(reloadSpy.count(), 0);
    QCOMPARE(storeSpy.count(), 0);
    QTRY_COMPARE(reloadSpy.count(), 1);
    QCOMPARE(storeSpy.count(), 1);
    QTest::qWait(350);
    QCOMPARE(reloadSpy.count(), 1);
}
