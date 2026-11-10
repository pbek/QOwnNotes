#!/usr/bin/env python3
"""Regression tests for the website's Markdown preprocessing."""

import importlib.util
from pathlib import Path
import unittest

SCRIPT_DIR = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location("fix_markdown_html", SCRIPT_DIR / "fix-markdown-html.py")
fixer = importlib.util.module_from_spec(spec)
spec.loader.exec_module(fixer)


class MarkdownHtmlTests(unittest.TestCase):
    def test_nested_div_layout(self):
        content = '<div style="display: flex;">\n  <div>\n\n**Content**\n\n  </div>\n</div>\n'
        self.assertEqual(fixer.fix_markdown_content(content), (content, False))

    def test_orphaned_div(self):
        content = 'Translated text\n</div>\n  </div>\n'
        self.assertNotIn('</div>', fixer.fix_markdown_content(content)[0])

    def test_inline_and_case_insensitive_divs(self):
        content = '<DIV><div>Content</div>\n</DIV>\n'
        self.assertEqual(fixer.fix_markdown_content(content), (content, False))

    def test_android_pages_keep_layout_closings(self):
        for path in ('blog/2026-11-10-QOwnNotes-Android-local-folders.md',
                     'getting-started/qownnotes-android.md'):
            with self.subTest(path=path):
                content = (SCRIPT_DIR.parent / 'src' / path).read_text()
                fixed, _ = fixer.fix_markdown_content(content)
                self.assertEqual(fixed.count('</div>'), content.count('</div>'))
                self.assertEqual(fixed.count('<div '), fixed.count('</div>'))


if __name__ == '__main__':
    unittest.main()
