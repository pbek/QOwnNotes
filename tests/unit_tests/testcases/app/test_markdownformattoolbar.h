#pragma once

#include <QObject>

class TestMarkdownFormatToolbar : public QObject {
    Q_OBJECT

   private slots:
    void testInlineFormatting_data();
    void testInlineFormatting();
    void testWhitespaceAndMultiline();
    void testLists_data();
    void testLists();
    void testCodeAndLinks();
    void testActionStateAndMenu();
    void testShortcutsAndFocus();
};
