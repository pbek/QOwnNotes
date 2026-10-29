#include "test_previewdetails.h"

#include <QElapsedTimer>
#include <QRegularExpression>
#include <QScrollBar>
#include <QSignalSpy>
#include <QTextDocument>
#include <QtTest>

#include "entities/note.h"
#include "utils/misc.h"
#include "utils/previewdetails.h"
#include "widgets/notepreviewwidget.h"
#ifdef USE_QLITEHTML
#include "widgets/htmlpreviewwidget.h"
#endif

namespace {
QList<QUrl> toggleUrls(const QString &html) {
    QList<QUrl> urls;
    const QRegularExpression pattern(QStringLiteral("href=\"(qownnotes-details:[^\"]+)\""));
    auto matches = pattern.globalMatch(html);
    while (matches.hasNext()) urls.append(QUrl(matches.next().captured(1)));
    return urls;
}
}    // namespace

void TestPreviewDetails::toggleSections() {
    Utils::PreviewDetails details;
    details.setHtml(
        QStringLiteral("before<details><summary><b>Commands</b></summary>hidden"
                       "</details>after<details open><summary>Visible</summary>shown</details>"));
    auto html = details.renderedHtml();
    QVERIFY(html.contains(QStringLiteral("before")));
    QVERIFY(html.contains(QStringLiteral("after")));
    QVERIFY(html.contains(QStringLiteral("<b>Commands</b>")));
    QVERIFY(!html.contains(QStringLiteral("hidden")));
    QVERIFY(html.contains(QStringLiteral("shown")));
    const auto urls = toggleUrls(html);
    QCOMPARE(urls.size(), 2);
    QVERIFY(details.toggle(urls.first()));
    QVERIFY(details.renderedHtml().contains(QStringLiteral("hidden")));
    QVERIFY(details.toggle(urls.first()));
    QVERIFY(!details.renderedHtml().contains(QStringLiteral("hidden")));
    QVERIFY(!details.toggle(QUrl(QStringLiteral("https://example.org"))));
    QVERIFY(!details.toggle(QUrl(QStringLiteral("qownnotes-details:unknown"))));
}

void TestPreviewDetails::nestedSections() {
    Utils::PreviewDetails details;
    details.setHtml(
        QStringLiteral("<details><summary>Outer</summary>outer body"
                       "<details><summary>Inner</summary>inner body</details></details>"));
    QCOMPARE(toggleUrls(details.renderedHtml()).size(), 1);
    const QUrl outer = toggleUrls(details.renderedHtml()).first();
    QVERIFY(details.toggle(outer));
    auto urls = toggleUrls(details.renderedHtml());
    QCOMPARE(urls.size(), 2);
    QVERIFY(details.renderedHtml().contains(QStringLiteral("outer body")));
    QVERIFY(!details.renderedHtml().contains(QStringLiteral("inner body")));
    QVERIFY(details.toggle(urls.last()));
    QVERIFY(details.renderedHtml().contains(QStringLiteral("inner body")));
    QVERIFY(details.toggle(outer));
    QVERIFY(!details.renderedHtml().contains(QStringLiteral("inner body")));
    QVERIFY(details.toggle(outer));
    QVERIFY(details.renderedHtml().contains(QStringLiteral("inner body")));
}

void TestPreviewDetails::tokenization_data() {
    QTest::addColumn<QString>("source");
    QTest::addColumn<int>("sections");
    QTest::addColumn<bool>("visible");
    QTest::newRow("case-and-quotes")
        << QStringLiteral(
               "<DETAILS title='> </details>' OPEN='false'><SUMMARY>Label</SUMMARY>body</DETAILS>")
        << 1 << true;
    QTest::newRow("not-open-attribute")
        << QStringLiteral(
               "<details data-open='yes' title='open'><summary>Label</summary>body</details>")
        << 1 << false;
    QTest::newRow("unquoted-attribute")
        << QStringLiteral("<details title=open><summary>Label</summary>body</details>") << 1
        << false;
    QTest::newRow("default-summary") << QStringLiteral("<details>body</details>") << 1 << false;
    QTest::newRow("nested-summary-not-direct-child")
        << QStringLiteral("<details><div><summary>Nested label</summary></div>body</details>") << 1
        << false;
    QTest::newRow("comment") << QStringLiteral(
                                    "<!-- <details><summary>Label</summary>body</details> -->")
                             << 0 << true;
    QTest::newRow("escaped-code")
        << QStringLiteral("<pre><code>&lt;details&gt;body&lt;/details&gt;</code></pre>") << 0
        << true;
    QTest::newRow("literal-pre")
        << QStringLiteral("<pre><details><summary>Label</summary>body</details></pre>") << 0
        << true;
    QTest::newRow("literal-code")
        << QStringLiteral("<code><details><summary>Label</summary>body</details></code>") << 0
        << true;
    QTest::newRow("script") << QStringLiteral(
                                   "<script>const x = '<details>body</details>';</script>")
                            << 0 << true;
    QTest::newRow("style") << QStringLiteral("<style>/* <details>body</details> */</style>") << 0
                           << true;
    QTest::newRow("unclosed-details")
        << QStringLiteral("<details><summary>Label</summary>body") << 0 << true;
    QTest::newRow("unterminated-attribute")
        << QStringLiteral("<details title='body > <details>broken</details>") << 0 << true;
}

void TestPreviewDetails::tokenization() {
    QFETCH(QString, source);
    QFETCH(int, sections);
    QFETCH(bool, visible);
    Utils::PreviewDetails details;
    details.setHtml(source);
    const QString html = details.renderedHtml();
    QCOMPARE(toggleUrls(html).size(), sections);
    QCOMPARE(html.contains(QStringLiteral("body")), visible);
    if (sections == 0) QCOMPARE(html, source);
}

void TestPreviewDetails::preserveState() {
    Utils::PreviewDetails details;
    const QString source = QStringLiteral("<details><summary>One</summary>body</details>");
    details.setHtml(source, QStringLiteral("note-1"));
    const QUrl url = toggleUrls(details.renderedHtml()).first();
    QVERIFY(details.toggle(url));
    details.setHtml(
        QStringLiteral("<details><summary>Inserted</summary>other</details>") +
            QString(source).replace(QStringLiteral("body"), QStringLiteral("edited body")),
        QStringLiteral("note-1"));
    QVERIFY(details.renderedHtml().contains(QStringLiteral("edited body")));
    QCOMPARE(toggleUrls(details.renderedHtml()).last(), url);
    details.setHtml(source, QStringLiteral("note-2"));
    QVERIFY(!details.renderedHtml().contains(QStringLiteral("body")));
    const QString openSource =
        QString(source).replace(QStringLiteral("<details>"), QStringLiteral("<details open>"));
    details.setHtml(openSource, QStringLiteral("note-2"));
    QVERIFY(details.renderedHtml().contains(QStringLiteral("body")));
    QVERIFY(details.toggle(toggleUrls(details.renderedHtml()).first()));
    details.setHtml(openSource, QStringLiteral("note-2"));
    QVERIFY(!details.renderedHtml().contains(QStringLiteral("body")));
    details.setHtml(source + source, QStringLiteral("note-2"));
    const auto urls = toggleUrls(details.renderedHtml());
    QCOMPARE(urls.size(), 2);
    QVERIFY(urls.first() != urls.last());
    QVERIFY(details.toggle(urls.last()));
    QCOMPARE(details.renderedHtml().count(QStringLiteral("body")), 1);
    details.setHtml(QString(), QStringLiteral("note-2"));
    details.setHtml(source, QStringLiteral("note-2"));
    QVERIFY(!details.renderedHtml().contains(QStringLiteral("body")));
}

void TestPreviewDetails::exportAllContent() {
    const QString source = QStringLiteral(
        "<details><summary>Outer</summary>body"
        "<details><summary>Inner</summary>nested</details></details>");
    Utils::PreviewDetails details;
    details.setHtml(source);
    const QString html = details.renderedHtml(true);
    QVERIFY(html.contains(QStringLiteral("body")));
    QVERIFY(html.contains(QStringLiteral("nested")));
    QVERIFY(!html.contains(QStringLiteral("qownnotes-details:")));
    QVERIFY(!html.contains(QStringLiteral("<details>")));
    QTextDocument document;
    document.setHtml(html);
    QVERIFY(document.toPlainText().contains(QStringLiteral("Outer")));
    QVERIFY(document.toPlainText().contains(QStringLiteral("Inner")));
    QVERIFY(!details.renderedHtml().contains(QStringLiteral("body")));
}

void TestPreviewDetails::markdownExample() {
    Note note;
    note.setNoteText(
        QStringLiteral("#### Optional: keep NetworkManager away from guest ports\n\n"
                       "NetworkManager normally leaves externally created taps/veths alone.\n\n"
                       "<details>\n<summary>Commands</summary>\n\n"
                       "```ini\n# /etc/NetworkManager/conf.d/90-unmanaged-guest-ports.conf\n"
                       "[keyfile]\nunmanaged-devices=interface-name:vnet*;interface-name:tap*\n"
                       "```\n</details>\n"));
    Utils::PreviewDetails details;
    details.setHtml(note.toMarkdownHtml(QString()));
    QCOMPARE(toggleUrls(details.renderedHtml()).size(), 1);
    QVERIFY(!details.renderedHtml().contains(QStringLiteral("unmanaged-devices")));
    QVERIFY(details.toggle(toggleUrls(details.renderedHtml()).first()));
    QVERIFY(details.renderedHtml().contains(QStringLiteral("unmanaged-devices")));
    QVERIFY(details.renderedHtml().contains(QStringLiteral("<pre")));
}

void TestPreviewDetails::revealAnchor() {
    Utils::PreviewDetails details;
    details.setHtml(QStringLiteral(
        "<a href=\"#target\">jump</a><details><summary>Outer</summary>outer"
        "<details><summary>Inner</summary><a id=\"target\">inner</a></details></details>"
        "<details><summary>Other</summary>other<pre>id=\"literal\"</pre></details>"));
    QVERIFY(!details.revealAnchor(QStringLiteral("missing")));
    QVERIFY(!details.revealAnchor(QStringLiteral("literal")));
    QVERIFY(details.revealAnchor(QStringLiteral("target")));
    const QString html = details.renderedHtml();
    QVERIFY(html.contains(QStringLiteral("inner")));
    QVERIFY(!html.contains(QStringLiteral(">other")));
    QVERIFY(!details.revealAnchor(QStringLiteral("target")));
}

void TestPreviewDetails::notesWithoutDetailsAreUnchanged() {
    Note note;
    note.setNoteText(QStringLiteral(
        "# Heading\n\nSome **bold**, _italic_, ~~strike~~ and `<details>` code.\n\n"
        "- [ ] open task\n- [x] done task\n\n| a | b |\n|---|---|\n| 1 | 2 |\n\n"
        "```cpp\nauto x = \"<details>\"; // <summary>\n```\n\n"
        "    indented <details>\n\n> quote with <span title='a > b'>html</span>\n\n"
        "<!-- <details><summary>hidden</summary></details> -->\n\n"
        "Text with a footnote[^1].\n\n[^1]: The footnote.\n\n"
        "![image](file:///tmp/image.png) [link](https://example.org) [[Wiki Link]]\n\n"
        "<div><b>raw html</b><br><img src=\"x.png\"></div>\n"));
    const QString html = Utils::Misc::parseTaskList(note.toMarkdownHtml(QString()), true);
    Utils::PreviewDetails details;
    details.setHtml(html);
    QCOMPARE(details.renderedHtml(), html);
    QCOMPARE(details.renderedHtml(true), html);
}

void TestPreviewDetails::previewAnchorScrolling() {
    NotePreviewWidget widget;
    QString filler;
    for (int i = 0; i < 200; ++i) filler += QStringLiteral("<p>line %1</p>").arg(i);
    widget.setHtml(filler + QStringLiteral("<details><summary>Notes</summary>"
                                           "<p><a name=\"qon-footnote-1\">footnote</a></p>"
                                           "</details>"),
                   QStringLiteral("note"));
    widget.QWidget::resize(600, 300);
    widget.show();
    QApplication::processEvents();
    QVERIFY(!widget.toPlainText().contains(QStringLiteral("footnote")));
    widget.scrollToAnchor(QStringLiteral("qon-footnote-1"));
    QVERIFY(widget.toPlainText().contains(QStringLiteral("footnote")));
    QVERIFY(widget.verticalScrollBar()->value() > 0);
}

void TestPreviewDetails::largeDocumentPerformance() {
    QString html;
    for (int i = 0; i < 5000; ++i) {
        html += QStringLiteral(
            "<p>text <code>inline &lt;details&gt;</code></p>"
            "<pre><code class=\"ini\">key=value</code></pre>");
    }
    html += QStringLiteral("<details><summary>End</summary>body</details>");
    QElapsedTimer timer;
    timer.start();
    Utils::PreviewDetails details;
    details.setHtml(html);
    const QString rendered = details.renderedHtml();
    QCOMPARE(toggleUrls(rendered).size(), 1);
    QVERIFY2(timer.elapsed() < 2000, qPrintable(QString::number(timer.elapsed())));
}

void TestPreviewDetails::previewLinks() {
    NotePreviewWidget widget;
    QSignalSpy spy(&widget, &NotePreviewWidget::previewLinkClicked);
    const QString source = QStringLiteral(
        "<details><summary><a href=\"https://example.org\">Label</a>"
        "</summary>body <a href=\"https://example.org\">link</a>"
        "<ul><li>[ ] task</li></ul></details>");
    widget.setHtml(source, QStringLiteral("note"));
    QVERIFY(!widget.toPlainText().contains(QStringLiteral("body")));
    widget.QWidget::resize(600, 400);
    widget.show();
    QApplication::processEvents();
    QTextCursor label = widget.document()->find(QStringLiteral("Label"));
    QVERIFY(!label.isNull());
    label.setPosition(label.selectionStart() + 2);
    QTest::mouseClick(widget.viewport(), Qt::LeftButton, Qt::NoModifier,
                      widget.cursorRect(label).center());
    QVERIFY(widget.toPlainText().contains(QStringLiteral("body")));
    QCOMPARE(spy.size(), 0);
    const QUrl ordinary(QStringLiteral("https://example.org"));
    QVERIFY(QMetaObject::invokeMethod(&widget, "anchorClicked", Q_ARG(QUrl, ordinary)));
    QCOMPARE(spy.size(), 1);
    QCOMPARE(spy.first().first().toUrl(), ordinary);
    QVERIFY(widget.toHtml().contains(QStringLiteral("checkbox://")));
    widget.setHtml(source, QStringLiteral("note"));
    QVERIFY(widget.toPlainText().contains(QStringLiteral("body")));
    widget.setHtml(source, QStringLiteral("other-note"));
    QVERIFY(!widget.toPlainText().contains(QStringLiteral("body")));
}

#ifdef USE_QLITEHTML
void TestPreviewDetails::qliteHtmlPreviewLinks() {
    HtmlPreviewWidget widget(nullptr);
    QSignalSpy spy(&widget, &HtmlPreviewWidget::anchorClicked);
    const QString source = QStringLiteral(
        "<details><summary>Commands</summary>body "
        "<a href=\"https://example.org\">link</a>"
        "<ul><li>[ ] task</li></ul></details>");
    widget.setHtml(source, QStringLiteral("note"));
    auto *internal = widget.findChild<HtmlPreviewWidgetInternal *>();
    QVERIFY(internal);
    QVERIFY(!internal->html().contains(QStringLiteral("body")));
    const auto urls = toggleUrls(internal->html());
    QCOMPARE(urls.size(), 1);
    QVERIFY(QMetaObject::invokeMethod(internal, "linkClicked", Q_ARG(QUrl, urls.first())));
    QVERIFY(internal->html().contains(QStringLiteral("body")));
    QVERIFY(internal->html().contains(QStringLiteral("checkbox://")));
    QCOMPARE(spy.size(), 0);
    const QUrl ordinary(QStringLiteral("https://example.org"));
    QVERIFY(QMetaObject::invokeMethod(internal, "linkClicked", Q_ARG(QUrl, ordinary)));
    QCOMPARE(spy.size(), 1);
    widget.setHtml(source, QStringLiteral("note"));
    QVERIFY(internal->html().contains(QStringLiteral("body")));
    widget.setHtml(source, QStringLiteral("other-note"));
    QVERIFY(!internal->html().contains(QStringLiteral("body")));
    widget.setHtml(QStringLiteral("<details><summary>Notes</summary>"
                                  "<a id=\"qon-footnote-1\">footnote</a></details>"),
                   QStringLiteral("other-note"));
    QVERIFY(!internal->html().contains(QStringLiteral("footnote</a>")));
    widget.scrollToAnchor(QStringLiteral("qon-footnote-1"));
    QVERIFY(internal->html().contains(QStringLiteral("footnote</a>")));
}
#endif
