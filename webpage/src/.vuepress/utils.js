import _ from "lodash-es";
import fs from "fs";
import { globSync } from "glob";
import markdownIt from "markdown-it";
import meta from "markdown-it-meta";

// Load all MD files in a specified directory and order by metadata 'order' value
export const getChildren = (parent_path, dir, reverseOrder = false) => {
  let files = globSync(parent_path + (dir ? `/${dir}` : "") + "/**/*.md").map(
    (path) => {
      // Instantiate MarkdownIt
      let md = new markdownIt();
      // Add markdown-it-meta
      md.use(meta);
      // Get the order value
      let file = fs.readFileSync(path, "utf8");
      md.render(file);
      let order = md.meta.order;
      // Remove "parent_path" and ".md"
      path = path.slice(parent_path.length + 1, -3);
      // Remove "README", making it the de facto index page
      if (path.endsWith("README")) {
        path = path.slice(0, -6);
      }

      return {
        path,
        order,
      };
    },
  );

  // Return the ordered list of files, sort by 'order' then 'path'
  let result = _.sortBy(files, ["order", "path"]);

  if (reverseOrder) {
    result = result.reverse();
  }

  return result.map((file) => file.path);
};

// Get blog children with titles for sidebar (used for non-English pages)
export const getBlogChildrenWithTitles = (
  parent_path,
  dir,
  reverseOrder = false,
) => {
  let files = globSync(parent_path + (dir ? `/${dir}` : "") + "/**/*.md").map(
    (path) => {
      // Instantiate MarkdownIt
      let md = new markdownIt();
      // Add markdown-it-meta
      md.use(meta);
      // Get the metadata and content
      let file = fs.readFileSync(path, "utf8");
      md.render(file);
      let order = md.meta.order;

      // Extract title from first H1 heading
      const h1Match = file.match(/^#\s+(.+)$/m);
      const title = md.meta.title || (h1Match ? h1Match[1] : "Untitled");

      // Remove "parent_path" and ".md"
      let cleanPath = path.slice(parent_path.length + 1, -3);
      // Remove "README", making it the de facto index page
      if (cleanPath.endsWith("README")) {
        cleanPath = cleanPath.slice(0, -6);
      }

      return {
        text: title,
        link: "/" + cleanPath.replace(/\.md$/, ""),
        order,
      };
    },
  );

  // Return the ordered list of files, sort by 'order' then 'link'
  let result = _.sortBy(files, ["order", "link"]);

  if (reverseOrder) {
    result = result.reverse();
  }

  return result;
};

export const getNavItemsForLanguage = (navItems, language) => {
  // VuePress 2 automatically adds locale prefix to relative paths in sidebar
  // So we just return the items as-is without adding the language prefix
  return navItems;
};

export const getNavItemForLanguage = (navItem, language) => {
  return "/" + language + navItem;
};
