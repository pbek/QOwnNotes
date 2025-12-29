#!/usr/bin/env node

/**
 * Blog Data Generator
 *
 * This script generates a JSON file containing blog post metadata from markdown files.
 * It runs automatically before:
 * - Development server starts (npm run dev)
 * - Production build (npm run build)
 *
 * The generated file (src/.vuepress/blog-data.json) is:
 * - Used by the BlogIndex.vue component to display blog posts
 * - Ignored by git (listed in .gitignore)
 * - Regenerated on each build/dev run
 */

import fs from "fs";
import { globSync } from "glob";
import markdownIt from "markdown-it";
import meta from "markdown-it-meta";
import { format, parse } from "date-fns";

// Load all blog MD files
const generateBlogData = () => {
  const paths = globSync("src/blog/**/*.md");

  const files = paths
    .map((path) => {
      // Skip the README file (Overview page)
      if (path.endsWith("README.md")) {
        return null;
      }

      // Read file content
      const fileContent = fs.readFileSync(path, "utf8");

      // Instantiate MarkdownIt
      const md = new markdownIt();
      // Add markdown-it-meta
      md.use(meta);
      md.render(fileContent);

      // Extract title from first H1 heading
      const h1Match = fileContent.match(/^#\s+(.+)$/m);
      const title = md.meta.title || (h1Match ? h1Match[1] : "Untitled");

      // Extract description from frontmatter or after title
      const description = md.meta.description || "";

      // Extract date from filename (format: YYYY-MM-DD)
      const dateMatch = path.match(/(\d{4}-\d{2}-\d{2})/);
      const dateFromFilename = dateMatch ? dateMatch[1] : "";

      // Use frontmatter date if available, otherwise use filename date
      const rawDate = md.meta.date || md.meta.order || dateFromFilename;

      // Convert path to URL path
      const urlPath = path.replace("src", "").replace(".md", ".html");

      // Format date
      let formattedDate = dateFromFilename;
      if (rawDate) {
        try {
          // Try parsing with different formats
          let parsedDate;
          if (typeof rawDate === "string" && rawDate.includes("T")) {
            parsedDate = parse(rawDate, "yyyy-MM-dd'T'HH", new Date());
          } else if (dateFromFilename) {
            parsedDate = parse(dateFromFilename, "yyyy-MM-dd", new Date());
          }
          if (parsedDate && !isNaN(parsedDate.getTime())) {
            formattedDate = format(parsedDate, "yyyy-MM-dd");
          }
        } catch (e) {
          // Keep date from filename if parsing fails
        }
      }

      return {
        path: urlPath,
        title: title,
        description: description,
        date: formattedDate,
        rawDate: dateFromFilename || rawDate,
      };
    })
    .filter((item) => item !== null);

  // Sort by date (newest first)
  files.sort((a, b) => {
    const dateA = a.rawDate || "";
    const dateB = b.rawDate || "";
    return String(dateB).localeCompare(String(dateA));
  });

  // Write to JSON file
  const outputPath = "src/.vuepress/blog-data.json";
  fs.writeFileSync(outputPath, JSON.stringify(files, null, 2));

  console.log(
    `✓ Generated blog data with ${files.length} entries at ${outputPath}`,
  );
};

generateBlogData();
