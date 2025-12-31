#!/bin/sh

set -e

echo "⚠️  Replacing corrupted translation files with English versions..."
# Replace corrupted files with English versions instead of removing them
# This allows the build to complete successfully while corrupted translations are being fixed

replace_with_english() {
  corrupted_file="$1"
  # Extract the path after the language code (e.g., src/ko/editor/time-formats.md -> editor/time-formats.md)
  relative_path=$(echo "$corrupted_file" | sed -E 's|^src/[a-z]{2}(-[A-Z]{2})?/||')
  english_file="src/$relative_path"

  if [ -f "$english_file" ]; then
    cp "$english_file" "$corrupted_file" && echo "  ✓ Replaced $corrupted_file with English version"
  else
    rm -f "$corrupted_file" && echo "  ✓ Removed $corrupted_file (no English version found)"
  fi
}

# Run detection script and replace found files
if [ -x "./scripts/find-corrupted-files.sh" ]; then
  corrupted_files=$(./scripts/find-corrupted-files.sh | grep "^src/" || true)
  if [ -n "$corrupted_files" ]; then
    echo "$corrupted_files" | while read -r file; do
      if [ -f "$file" ]; then
        replace_with_english "$file"
      fi
    done
  fi
else
  # Fallback: manually specified files
  [ -f "src/ko/getting-started/markdown.md" ] && replace_with_english "src/ko/getting-started/markdown.md"
  [ -f "src/ko/editor/time-formats.md" ] && replace_with_english "src/ko/editor/time-formats.md"
  [ -f "src/fa/editor/time-formats.md" ] && replace_with_english "src/fa/editor/time-formats.md"
  [ -f "src/fa/contributing/translation.md" ] && replace_with_english "src/fa/contributing/translation.md"
  [ -f "src/it/getting-started/overview.md" ] && replace_with_english "src/it/getting-started/overview.md"
fi

#echo ""
#echo "🔧 Fixing HTML issues in markdown files..."
#python3 ./scripts/fix-markdown-html.py

echo ""
echo "📦 Copying assets..."

# ...existing code...
cp -R ../screenshots src/.vuepress/public 2>/dev/null || echo "⚠️  Screenshots not found, skipping"
cp ../CHANGELOG.md src/changelog.md 2>/dev/null || echo "⚠️  CHANGELOG.md not found, skipping"

echo ""
echo "📝 Generating blog index data..."
node scripts/generate-blog-data.js

echo ""
echo "🏗️  Building with VuePress 2..."
echo ""

# VuePress 2 build (CSS handling via Vite plugin)
npx vuepress build src

echo ""
echo "✨ Setting RTL styles for Arabic and Farsi..."

# Set more RTL styles
# shellcheck disable=SC2046
sed -E 's/^<html /&dir="rtl" /' -i $(find src/.vuepress/dist/fa -name '*.html' 2>/dev/null) 2>/dev/null || echo "⚠️  Farsi pages not found"
# shellcheck disable=SC2046
sed -E 's/^<html /&dir="rtl" /' -i $(find src/.vuepress/dist/ar -name '*.html' 2>/dev/null) 2>/dev/null || echo "⚠️  Arabic pages not found"

echo "✅ Build complete!"
