#!/usr/bin/env sh
# In nodejs 12 this error didn't occur, but in nodejs 20 it does.

#
# Fix Date RangeError in node_modules/vuepress-plugin-seo/index.js
#

# Define the original and replacement lines
original_line1='(new Date($page.frontmatter.date)).toISOString()'
replacement_line1="(Number.isNaN(new Date(\$page.frontmatter.date).getTime()) ? '' : new Date(\$page.frontmatter.date).toISOString())"
original_line2='(new Date($page.lastUpdated)).toISOString()'
replacement_line2="(Number.isNaN(new Date(\$page.lastUpdated).getTime()) ? '' : new Date(\$page.lastUpdated).toISOString())"

# Specify the file in which you want to make the replacement
file_to_edit="node_modules/vuepress-plugin-seo/index.js"

# Use sed to perform the replacement in the file
sed -i "s/$original_line1/$replacement_line1/g" "$file_to_edit"
sed -i "s/$original_line2/$replacement_line2/g" "$file_to_edit"

#
# Fix Date RangeError in node_modules/vuepress-plugin-sitemap/index.js
#

# Define the original and replacement line
original_line="new Date(lastUpdated).toISOString()"
replacement_line="Number.isNaN(new Date(lastUpdated).getTime()) ? '' : new Date(lastUpdated).toISOString()"

# Specify the file in which you want to make the replacement
file_to_edit="node_modules/vuepress-plugin-sitemap/index.js"

# Use sed to perform the replacement in the file
sed -i "s/$original_line/$replacement_line/g" "$file_to_edit"
