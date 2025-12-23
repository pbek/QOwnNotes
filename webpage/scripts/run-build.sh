#!/usr/bin/env sh

set -e

echo "📦 Copying assets..."

# Copy missing assets (skip if can't find, because we are using this script in a Dockerfile)
cp -R ../screenshots src/.vuepress/public 2>/dev/null || echo "⚠️  Screenshots not found, skipping"
cp ../CHANGELOG.md src/changelog.md 2>/dev/null || echo "⚠️  CHANGELOG.md not found, skipping"

echo "🏗️  Building with VuePress 2..."
echo ""

# VuePress 2 uses the vuepress CLI directly
npx vuepress build src

echo ""
echo "✨ Setting RTL styles for Arabic and Farsi..."

# Set more RTL styles
# shellcheck disable=SC2046
sed -E 's/^<html /&dir="rtl" /' -i $(find src/.vuepress/dist/fa -name '*.html' 2>/dev/null) 2>/dev/null || echo "⚠️  Farsi pages not found"
# shellcheck disable=SC2046
sed -E 's/^<html /&dir="rtl" /' -i $(find src/.vuepress/dist/ar -name '*.html' 2>/dev/null) 2>/dev/null || echo "⚠️  Arabic pages not found"

echo "✅ Build complete!"
