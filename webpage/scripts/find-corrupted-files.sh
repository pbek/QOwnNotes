#!/bin/sh

# Script to identify markdown files with severe HTML corruption
# These files will be excluded from the build

echo "🔍 Scanning for corrupted translation files..." >&2

# Array to store corrupted files (sh compatible)
corrupted_files=""

# Find all markdown files in translation directories (any 2-letter language code)
find src -name "*.md" -type f 2>/dev/null | grep -E 'src/[a-z]{2}(-[A-Z]{2})?/' | while IFS= read -r file; do
  # Check for common corruption patterns
  has_error=false
  error_details=""

  # Pattern 1: Tags with numbers like </0>, </1> or <0>, <1>, including with spaces like </0 >
  if grep -qE '</?[0-9]+\s*>' "$file" 2>/dev/null || grep -qE '</\s*[0-9]+\s*>' "$file" 2>/dev/null; then
    has_error=true
    error_details="${error_details}numeric tag (<0>, </0>, </0 >, etc); "
  fi

  # Pattern 2: Unclosed code tags followed by table syntax
  if grep -q '<code>[^<]*`.*|' "$file" 2>/dev/null; then
    has_error=true
    error_details="${error_details}unclosed <code> with table syntax; "
  fi

  # Pattern 3: Mixing HTML table with markdown table syntax
  if grep -q '<table' "$file" 2>/dev/null && grep -q '<code>.*|.*|' "$file" 2>/dev/null; then
    has_error=true
    error_details="${error_details}mixed HTML/markdown table; "
  fi

  # Pattern 4: Orphaned closing paragraph tags or other malformed closing tags
  if grep -qE '^[^<]*</p>|</[0-9 ]+>' "$file" 2>/dev/null; then
    has_error=true
    error_details="${error_details}orphaned/malformed closing tag; "
  fi

  # Pattern 5: Opening tags with numbers
  if grep -qE '<[0-9]+[^>]*>' "$file" 2>/dev/null; then
    has_error=true
    error_details="${error_details}numeric opening tag; "
  fi

  if [ "$has_error" = true ]; then
    corrupted_files="$corrupted_files$file
"
    echo "  ⚠️  $file" >&2
    echo "      └─ Errors: $error_details" >&2
    # Output to stdout for the build script to capture
    echo "$file"
  fi
done
