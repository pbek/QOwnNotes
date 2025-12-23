#!/usr/bin/env bash
# Comprehensive HTML validation fixer for markdown files
# This script finds and fixes common HTML validation errors in translation files

echo "🔍 Searching for HTML validation issues in markdown files..."
echo ""

# Function to fix a file
fix_file() {
  local file="$1"
  local changes=0

  # Create backup
  cp "$file" "$file.bak"

  # Fix </noscript> without opening tag
  if grep -q "</noscript>" "$file"; then
    sed -i 's/<\/noscript>//g' "$file"
    ((changes++))
    echo "  ✓ Removed </noscript>"
  fi

  # Fix standalone </p> tags
  if grep -q "[^>]</p>" "$file"; then
    sed -i 's/\([^>]\)<\/p>/\1/g' "$file"
    ((changes++))
    echo "  ✓ Removed standalone </p>"
  fi

  # Fix numbered closing tags
  if grep -E -q "</[0-9]>" "$file"; then
    sed -i -E 's/<\/[0-9]>//g' "$file"
    ((changes++))
    echo "  ✓ Removed numbered closing tags"
  fi

  # Fix numbered opening tags
  if grep -E -q "<[0-9]>" "$file"; then
    sed -i -E 's/<[0-9]>//g' "$file"
    ((changes++))
    echo "  ✓ Removed numbered opening tags"
  fi

  # Fix orphaned </li> tags
  if grep -q "^- .*</li>" "$file"; then
    sed -i 's/\(^- .*\)<\/li>/\1/g' "$file"
    ((changes++))
    echo "  ✓ Removed </li> from list items"
  fi

  # Fix orphaned </ul> tags
  if grep -q "</ul>" "$file"; then
    sed -i 's/<\/ul>//g' "$file"
    ((changes++))
    echo "  ✓ Removed </ul>"
  fi

  # Fix orphaned <ul> tags
  if grep -q "<ul>" "$file"; then
    sed -i 's/<ul>//g' "$file"
    ((changes++))
    echo "  ✓ Removed <ul>"
  fi

  if [ $changes -eq 0 ]; then
    rm "$file.bak"
    return 1
  else
    echo "  📝 $changes fix(es) applied"
    return 0
  fi
}

# Find all markdown files with potential issues
echo "📂 Scanning markdown files..."
echo ""

count=0
fixed=0

while IFS= read -r file; do
  if [ -f "$file" ]; then
    ((count++))
    echo "🔧 Checking: $file"
    if fix_file "$file"; then
      ((fixed++))
    else
      echo "  ℹ️  No issues found"
    fi
    echo ""
  fi
done < <(find src -name "*.md" -type f -exec grep -l "</p>\|</noscript>\|</[0-9]>\|<[0-9]>\|</li>\|</ul>\|<ul>" {} \;)

echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "📊 Summary:"
echo "   Files scanned: $count"
echo "   Files fixed: $fixed"
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo ""

if [ $fixed -gt 0 ]; then
  echo "✅ Fixed $fixed file(s)!"
  echo ""
  echo "💡 Next steps:"
  echo "   1. Test the build: npm run build"
  echo "   2. If successful, test Docker: docker build ."
  echo "   3. Backup files (.bak) can be removed if successful"
else
  echo "ℹ️  No issues found in markdown files."
  echo ""
  echo "💡 If build still fails:"
  echo "   1. Run: npx vuepress build src 2>&1 | grep 'error \[vite:vue\]' -A 5"
  echo "   2. Check the error message for the problematic file"
  echo "   3. Manually inspect and fix that specific file"
fi

echo ""
echo "🔍 To restore backups if needed:"
echo "   find src -name '*.bak' -exec sh -c 'mv \"\$1\" \"\${1%.bak}\"' _ {} \\;"
echo ""
echo "🗑️  To remove backups after success:"
echo "   find src -name '*.bak' -delete"
