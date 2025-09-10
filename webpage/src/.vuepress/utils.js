const _ = require("lodash");
const fs = require("fs");
const glob = require("glob");
const markdownIt = require("markdown-it");
const meta = require("markdown-it-meta");

// Load all MD files in a specified directory and order by metadata 'order' value
const getChildren = (parent_path, dir, reverseOrder = false) => {
  files = glob
    .sync(parent_path + (dir ? `/${dir}` : "") + "/**/*.md")
    .map((path) => {
      // Instantiate MarkdownIt
      md = new markdownIt();
      // Add markdown-it-meta
      md.use(meta);
      // Get the order value
      file = fs.readFileSync(path, "utf8");
      md.render(file);
      order = md.meta.order;
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
    });

  // Return the ordered list of files, sort by 'order' then 'path'
  let result = _.sortBy(files, ["order", "path"]);

  if (reverseOrder) {
    result = result.reverse();
  }

  return result.map((file) => file.path);
};

const getNavItemsForLanguage = (navItems, language) => {
  let resultNavItems = [];

  navItems.forEach((navItem) => {
    resultNavItems.push(language + "/" + navItem);
  });

  return resultNavItems;
};

const getNavItemForLanguage = (navItem, language) => {
  return "/" + language + navItem;
};

module.exports = {
  getChildren,
  getNavItemsForLanguage,
  getNavItemForLanguage,
};
