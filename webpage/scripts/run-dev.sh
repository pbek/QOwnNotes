#!/usr/bin/env sh

set -e

#echo "🔧 Fixing HTML issues in markdown files..."
#python3 ./scripts/fix-markdown-html.py

echo ""
echo "📦 Copying assets..."
cp -R ../screenshots src/.vuepress/public 2>/dev/null || echo "⚠️  Screenshots not found, skipping"
cp ../CHANGELOG.md src/changelog.md 2>/dev/null || echo "⚠️  CHANGELOG.md not found, skipping"

echo "🚀 Starting VuePress dev server..."
echo ""

# VuePress 2 uses the vuepress CLI directly
npx vuepress dev src
