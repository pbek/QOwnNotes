import { defaultTheme } from "@vuepress/theme-default";
import { getDirname, path } from "vuepress/utils";

const __dirname = getDirname(import.meta.url);

export default (options) => ({
  name: "vuepress-theme-local",
  extends: defaultTheme(options),
  layouts: {
    Layout: path.resolve(__dirname, "layouts/Layout.vue"),
  },
});
