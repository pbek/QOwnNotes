#!/usr/bin/env bash
# Fix common HTML tag issues in markdown files

echo "Searching for problematic HTML tags in markdown files..."

# Find files with </noscript> without opening tag
echo "Fixing </noscript> issues..."
find src -name "*.md" -type f -exec sed -i 's/<\/noscript>//g' {} \;

# Find files with </p> at end of lines without opening <p>
echo "Fixing standalone </p> tags..."
find src -name "*.md" -type f -exec sed -i 's/\([^>]\)<\/p>/\1/g' {} \;

# Find files with numbered closing tags like </0>, </1>, etc.
echo "Fixing numbered closing tags..."
find src -name "*.md" -type f -exec sed -i 's/<\/[0-9]>//g' {} \;

# Find and remove literal newlines inside backticks in tables
echo "Note: Newlines in backticks need manual review"

echo "Done! Please test the build now."
