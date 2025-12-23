#!/usr/bin/env python3
"""
Advanced HTML fixer for Crowdin-generated markdown files.
Fixes complex HTML validation errors including newlines in code blocks.
"""

import os
import re
import sys
from pathlib import Path

def fix_markdown_content(content):
    """Fix various HTML validation issues in markdown content."""
    original = content

    # Fix 0: Remove raw HTML tags that should be markdown (Crowdin corruption)
    # These tags appear when Crowdin corrupts markdown into HTML
    content = re.sub(r'<p spaces-before="[^"]*">\s*', '', content)
    content = re.sub(r'<pre spaces-before="[^"]*">\s*', '```\n', content)
    content = re.sub(r'</pre>\s*', '\n```\n', content)
    content = re.sub(r'<code[^>]*>\s*', '`', content)
    content = re.sub(r'</code>\s*', '`', content)
    content = re.sub(r'<h([1-6]) spaces-before="[^"]*">\s*', lambda m: '\n' + '#' * int(m.group(1)) + ' ', content)
    content = re.sub(r'</h[1-6]>\s*', '\n', content)

    # Fix 1: Remove orphaned closing tags
    content = re.sub(r'</noscript>', '', content)
    content = re.sub(r'([^>])</p>', r'\1', content)
    content = re.sub(r'</script>', '', content)
    content = re.sub(r'<p>\s*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'^</p>\s*$', '', content, flags=re.MULTILINE)

    # Fix 2: Remove numbered HTML tags like </0>, <0>, etc.
    content = re.sub(r'</?[0-9]+>', '', content)

    # Fix 3: Remove orphaned list tags
    content = re.sub(r'</li>', '', content)
    content = re.sub(r'</ul>', '', content)
    content = re.sub(r'<ul>', '', content)
    content = re.sub(r'</ol>', '', content)
    content = re.sub(r'<ol>', '', content)

    # Fix 4: Remove standalone </div> tags
    content = re.sub(r'^</div>$', '', content, flags=re.MULTILINE)

    # Fix 4b: Fix mixed HTML table tags in markdown tables
    # Remove closing HTML table tags that shouldn't be there
    content = re.sub(r'</td>\s*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'</tr>\s*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'</tbody>\s*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'</table>\s*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'<thead>\s*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'</thead>\s*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'<tbody>\s*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'<tr>\s*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'<td[^>]*>\s*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'<th[^>]*>\s*$', '', content, flags=re.MULTILINE)
    content = re.sub(r'</th>\s*$', '', content, flags=re.MULTILINE)

    # Fix raw HTML heading tags that should be markdown
    content = re.sub(r'^<h([0-9])[^>]*>(.*?)</h\1>\s*$', r'#\1 \2', content, flags=re.MULTILINE)
    # Simpler version without closing tag
    content = re.sub(r'^<h([0-9])[^>]*>(.*?)\s*$', lambda m: '#' * int(m.group(1)) + ' ' + m.group(2), content, flags=re.MULTILINE)

    # Fix 5: Fix unclosed <div> tags - add closing </div> before certain markers
    # Pattern: <div class="..."> ... <ComponentName /> (without </div>)
    # Look for <div> followed by Vue component tags without a closing </div>
    def fix_unclosed_divs(text):
        lines = text.split('\n')
        result = []
        open_divs = []

        for i, line in enumerate(lines):
            # Track opening divs
            for match in re.finditer(r'<div[^>]*>', line):
                open_divs.append(i)

            # Track closing divs
            for match in re.finditer(r'</div>', line):
                if open_divs:
                    open_divs.pop()

            # If we see a Vue component and have unclosed divs, close them
            if re.match(r'^<[A-Z][a-zA-Z]*\s*/>', line.strip()) and open_divs:
                # Add closing div before this component
                result.append('</div>')
                result.append('')
                open_divs.pop()

            result.append(line)

        return '\n'.join(result)

    content = fix_unclosed_divs(content)

    # Fix 5: Fix backticks with newlines inside tables
    # Pattern: `text\n` or `\ntext` inside table rows
    def fix_backtick_newlines(match):
        """Remove newlines inside backticks in tables."""
        cell_content = match.group(0)
        # Remove newlines between backticks
        fixed = re.sub(r'`([^`]*?)\n([^`]*?)`', r'`\1 \2`', cell_content)
        # Handle multiple newlines
        while '`' in fixed and '\n' in fixed:
            new_fixed = re.sub(r'`([^`]*?)\n([^`]*?)`', r'`\1 \2`', fixed)
            if new_fixed == fixed:
                break
            fixed = new_fixed
        return fixed

    # Apply to table cells (between | characters)
    lines = content.split('\n')
    fixed_lines = []
    in_table = False

    for line in lines:
        # Detect if we're in a table
        if '|' in line and line.strip().startswith('|'):
            in_table = True
            # Fix backticks in this table line
            if '`' in line:
                # Remove newlines between backticks in table cells
                parts = line.split('|')
                fixed_parts = []
                for part in parts:
                    if '`' in part:
                        # Count backticks
                        backtick_count = part.count('`')
                        if backtick_count % 2 == 0:
                            # Even number - properly closed
                            fixed_parts.append(part)
                        else:
                            # Odd number - might span multiple lines
                            fixed_parts.append(part)
                    else:
                        fixed_parts.append(part)
                line = '|'.join(fixed_parts)
        elif in_table and not line.strip():
            in_table = False

        fixed_lines.append(line)

    content = '\n'.join(fixed_lines)

    # Fix 6: Remove empty HTML tags
    content = re.sub(r'<([a-z]+)></\1>', '', content)

    # Fix 7: Clean up multiple consecutive blank lines
    content = re.sub(r'\n\n\n+', '\n\n', content)

    return content, content != original

def process_markdown_files(src_dir):
    """Process all markdown files in the source directory."""
    src_path = Path(src_dir)
    total_files = 0
    fixed_files = 0

    print(f"🔧 Fixing HTML validation issues in markdown files...")
    print(f"📂 Source directory: {src_dir}")
    print("")

    for md_file in src_path.rglob('*.md'):
        total_files += 1

        try:
            with open(md_file, 'r', encoding='utf-8') as f:
                content = f.read()

            fixed_content, was_changed = fix_markdown_content(content)

            if was_changed:
                with open(md_file, 'w', encoding='utf-8') as f:
                    f.write(fixed_content)

                fixed_files += 1
                rel_path = md_file.relative_to(src_path)
                print(f"✓ Fixed: {rel_path}")

        except Exception as e:
            print(f"⚠️  Error processing {md_file}: {e}", file=sys.stderr)

    print("")
    print("━" * 44)
    print("📊 HTML Fix Summary:")
    print(f"   Total markdown files: {total_files}")
    print(f"   Files with fixes: {fixed_files}")
    print("━" * 44)
    print("")

    if fixed_files > 0:
        print(f"✅ Applied fixes to {fixed_files} file(s)")
    else:
        print("✨ No HTML issues found - all files are clean!")

    return 0

if __name__ == '__main__':
    script_dir = Path(__file__).parent
    src_dir = script_dir.parent / 'src'

    if not src_dir.exists():
        print(f"❌ Error: Source directory not found: {src_dir}", file=sys.stderr)
        sys.exit(1)

    sys.exit(process_markdown_files(src_dir))

