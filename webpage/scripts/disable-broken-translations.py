#!/usr/bin/env python3
"""
Emergency fix: Rename severely corrupted translation files to .md.disabled
This allows the build to succeed while we fix translations properly.
"""

import os
import sys
from pathlib import Path

# List of known problematic files that cause build failures
PROBLEMATIC_FILES = [
    "src/ko/getting-started/markdown.md",
    # Add more as discovered
]

def disable_problematic_files(src_dir):
    """Temporarily disable problematic translation files."""
    disabled_count = 0

    for file_path in PROBLEMATIC_FILES:
        full_path = src_dir / file_path
        if full_path.exists():
            disabled_path = full_path.with_suffix('.md.disabled')
            try:
                full_path.rename(disabled_path)
                print(f"⚠️  Disabled: {file_path}")
                disabled_count += 1
            except Exception as e:
                print(f"❌ Error disabling {file_path}: {e}", file=sys.stderr)

    return disabled_count

if __name__ == '__main__':
    script_dir = Path(__file__).parent
    src_dir = script_dir.parent / 'src'

    if not src_dir.exists():
        print(f"❌ Error: Source directory not found: {src_dir}", file=sys.stderr)
        print("⚠️  Skipping disable step", file=sys.stderr)
        sys.exit(0)  # Exit successfully to continue build

    print("⚠️  Temporarily disabling severely corrupted translation files...")
    print("")

    count = disable_problematic_files(src_dir)

    if count > 0:
        print("")
        print(f"⚠️  Disabled {count} file(s) to allow build to succeed")
        print("💡 These files need proper manual fixes")
    else:
        print("✨ No files needed disabling")

    sys.exit(0)

