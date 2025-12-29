#!/bin/sh

set -e

echo "⚠️  Removing severely corrupted translation files..."
# Remove files that are too corrupted to fix automatically
rm -f src/ko/getting-started/markdown.md && echo "  ✓ Removed src/ko/getting-started/markdown.md" || true

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
