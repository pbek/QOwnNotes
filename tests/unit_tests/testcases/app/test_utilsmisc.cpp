#include "test_utilsmisc.h"

#include <QTest>

#include "utils/misc.h"

using namespace Utils::Misc;

void TestUtilsMisc::initTestCase() {}

void TestUtilsMisc::testRemoveIfStartsWith() {
    QString s = QStringLiteral("--*--testString");
    QString result = removeIfStartsWith(s, QStringLiteral("--*--"));
    QString expected = QStringLiteral("testString");
    QVERIFY(result == expected);
}

void TestUtilsMisc::testRemoveIfEndsWith() {
    QString s = QStringLiteral("testString--*--");
    QString result = removeIfEndsWith(s, QStringLiteral("--*--"));
    QString expected = QStringLiteral("testString");
    QVERIFY(result == expected);
}

void TestUtilsMisc::testPrependIfDoesNotStartWith() {
    QString s = QStringLiteral("testString");
    QString result = prependIfDoesNotStartWith(s, QStringLiteral("--*--"));
    QString expected = QStringLiteral("--*--testString");
    QVERIFY(result == expected);
}

void TestUtilsMisc::testAppendIfDoesNotEndWith() {
    QString s = QStringLiteral("testString");
    QString result = appendIfDoesNotEndWith(s, QStringLiteral("--*--"));
    QString expected = QStringLiteral("testString--*--");
    QVERIFY(result == expected);
}

void TestUtilsMisc::testStartDetachedProcess() {
#ifdef Q_OS_UNIX
    auto result = startDetachedProcess("pwd", {});
    QVERIFY2(result == true, "Failed to start detached process");
#else
    QSKIP("This test is skipped on non unix OS");
#endif
}

void TestUtilsMisc::testShorten() {
    QString s = QStringLiteral("A Long test string with lots of words");
    const auto result = shorten(s, 10);
    QString expected = QStringLiteral("A Long ...");
    QVERIFY(result == expected);
}

void TestUtilsMisc::testCycleTextCase() {
    // test empty string
    QString s = "";
    QString result = cycleTextCase(s);
    QString expected = "";
    QVERIFY(result == expected);

    // lower
    s = "lower case sentence";
    result = cycleTextCase(s);
    expected = "LOWER CASE SENTENCE";
    QVERIFY(result == expected);

    // upper
    s = "LOWER CASE SENTENCE";
    result = cycleTextCase(s);
    expected = "Lower Case Sentence";
    QVERIFY(result == expected);

    // start case
    s = "Lower Case Sentence";
    result = cycleTextCase(s);
    expected = "Lower case sentence";
    QVERIFY(result == expected);

    // sentence case
    s = "Lower case sentence";
    result = cycleTextCase(s);
    expected = "lower case sentence";
    QVERIFY(result == expected);

    // random 1
    s = "LOWER CASE Sentence";
    result = cycleTextCase(s);
    expected = "lower case sentence";
    QVERIFY(result == expected);

    // random 2
    s = "lower CASE Sentence";
    result = cycleTextCase(s);
    expected = "lower case sentence";
    QVERIFY(result == expected);
}

void TestUtilsMisc::testHtmlToMarkdown() {
    QString html = "<head><meta charset=\"utf-8\"></head>";
    html += "<script>script</script>";
    html += "<style>style</style>";
    html += "<h1>Heading <em>italic</em></h1>";
    html += "<h2>Heading <strong>italic</strong></h2>";
    html += "<h3>Heading <em>italic</em></h3>";
    html += "<h4>Heading <em>italic</em></h4";
    html += "<h5>Heading <em>italic</em></h5>";
    html += "<h6>Heading <em>italic</em></h6>";
    html += "<code>hello</code>";
    html += "<i>hello</i>";

    QString result = htmlToMarkdown(html);
    QString expected =
        "\n# Heading *italic*\n\n## Heading **italic**\n\n### Heading "
        "*italic*\n<h4>Heading *italic*</h4\n##### Heading *italic*\n\n###### "
        "Heading *italic*\n\n```\nhello\n```\n*hello*";
    QVERIFY(result == expected);
}

void TestUtilsMisc::testParseTaskList() {
    const auto taskListTag = QStringLiteral("<li class=\"task-list-item-checkbox\">");
    const auto normalListTag = QStringLiteral("<li class=\"normal-list-item\">");
    const QString &t1 = "<li> [ ] task 1</li>";
    const QString &r1 = parseTaskList(t1, true);
    QString expec = taskListTag +
        " <a class=\"task-list-item-checkbox\" "
        "href=\"checkbox://_0\">&#9744;</a> task 1</li>";
    QVERIFY(r1 == expec);

    const QString &t2 = "<li> [x] task 2</li>";
    const QString &r2 = parseTaskList(t2, true);
    expec = taskListTag +
        " <a class=\"task-list-item-checkbox\" "
        "href=\"checkbox://_0\">&#9745;</a> task 2</li>";
    QVERIFY(r2 == expec);

    const QString &t3 = "<li> [x] task 3</li>";
    const QString &r3 = parseTaskList(t3, false);
    expec = taskListTag + " &#9745; task 3</li>";
    QVERIFY(r3 == expec);

    const QString &t4 = "<li> [ ] task 4</li>";
    const QString &r4 = parseTaskList(t4, false);
    expec = taskListTag + " &#9744; task 4</li>";
    QVERIFY(r4 == expec);
}

void TestUtilsMisc::testUnescapeHtml() {
    QString html = "<h1>hello</h1>\n<p>hello</p>";
    QString result = unescapeHtml(html);
    QString expected = "hello\n\n\nhello";
    QVERIFY(result == expected);
}

void TestUtilsMisc::testHtmlSpecialChars() {
    QString str = "& \" \' < >";
    QString result = htmlspecialchars(str);
    QString expected = "&amp; &quot; &apos; &lt; &gt;";
    QVERIFY(result == expected);
}

void TestUtilsMisc::testToHumanReadableByteSize() {
    qint64 bytes = 8712492;
    QString result = toHumanReadableByteSize(bytes);
    QString expected = "8.31 MB";
    QVERIFY(result == expected);
}
