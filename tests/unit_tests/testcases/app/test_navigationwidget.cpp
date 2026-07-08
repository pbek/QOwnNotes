#include "test_navigationwidget.h"

#include <QTextBlock>
#include <QTextDocument>
#include <QtTest>

#include "libraries/qmarkdowntextedit/markdownhighlighter.h"
#include "services/scriptingservice.h"
#include "widgets/navigationwidget.h"

void TestNavigationWidget::testParseDocumentSupportsScriptedHeadings() {
    ScriptingService *scriptingService = ScriptingService::instance();
    scriptingService->initComponents();

    scriptingService->addHighlightingRule(QStringLiteral("^@ +.+? +@\\s*$"), QStringLiteral("@"),
                                          MarkdownHighlighter::H1);
    scriptingService->addHighlightingRule(QStringLiteral("^@@ +.+? +@@\\s*$"), QStringLiteral("@@"),
                                          MarkdownHighlighter::H2);
    scriptingService->addHighlightingRule(QStringLiteral("^@@@ +.+? +@@@\\s*$"),
                                          QStringLiteral("@@@"), MarkdownHighlighter::H3);

    QTextDocument document;
    document.setPlainText(
        QStringLiteral("@ Introduction @\n"
                       "\n"
                       "This heading uses the fictitious \"@ ... @\" syntax.\n"
                       "\n"
                       "@@ Background @@\n"
                       "\n"
                       "@@@ Details @@@\n"
                       "\n"
                       "Normal paragraph\n"
                       "\n"
                       "@ Conclusion @\n"));

    const auto nodes = NavigationWidget::parseDocument(&document);

    QCOMPARE(nodes.count(), 4);
    QCOMPARE(nodes.at(0).text, QStringLiteral("Introduction"));
    QCOMPARE(nodes.at(0).elementType, int(MarkdownHighlighter::H1));
    QCOMPARE(nodes.at(1).text, QStringLiteral("Background"));
    QCOMPARE(nodes.at(1).elementType, int(MarkdownHighlighter::H2));
    QCOMPARE(nodes.at(2).text, QStringLiteral("Details"));
    QCOMPARE(nodes.at(2).elementType, int(MarkdownHighlighter::H3));
    QCOMPARE(nodes.at(3).text, QStringLiteral("Conclusion"));
    QCOMPARE(nodes.at(3).elementType, int(MarkdownHighlighter::H1));

    scriptingService->initComponents();
}

void TestNavigationWidget::testParseDocumentIgnoresHeadingsInsideFencedCodeBlocks() {
    QTextDocument document;
    document.setPlainText(
        QStringLiteral("# Visible heading\n"
                       "```\n"
                       "# ls -l\n"
                       "## test\n"
                       "```\n"
                       "## Still visible\n"));

    for (QTextBlock block = document.begin(); block.isValid(); block = block.next()) {
        const QString text = block.text();

        if (text == QStringLiteral("```")) {
            block.setUserState(block.blockNumber() == 1 ? MarkdownHighlighter::CodeBlock
                                                        : MarkdownHighlighter::CodeBlockEnd);
        } else if (text == QStringLiteral("# ls -l") || text == QStringLiteral("## test")) {
            block.setUserState(MarkdownHighlighter::CodeBlock);
        }
    }

    const auto nodes = NavigationWidget::parseDocument(&document);

    QCOMPARE(nodes.count(), 2);
    QCOMPARE(nodes.at(0).text, QStringLiteral("Visible heading"));
    QCOMPARE(nodes.at(0).elementType, int(MarkdownHighlighter::H1));
    QCOMPARE(nodes.at(1).text, QStringLiteral("Still visible"));
    QCOMPARE(nodes.at(1).elementType, int(MarkdownHighlighter::H2));
}

void TestNavigationWidget::testParseDocumentIgnoresFrontmatterSetextLookalike() {
    QTextDocument document;
    document.setPlainText(
        QStringLiteral("---\n"
                       "title: non breaking space test\n"
                       "headingDivider: 1\n"
                       "---\n"
                       "\n"
                       "# Test 1 classic NBSP2\n"
                       "\n"
                       "text\n"));

    const auto nodes = NavigationWidget::parseDocument(&document);

    QCOMPARE(nodes.count(), 1);
    QCOMPARE(nodes.at(0).text, QStringLiteral("Test 1 classic NBSP2"));
    QCOMPARE(nodes.at(0).elementType, int(MarkdownHighlighter::H1));
}
