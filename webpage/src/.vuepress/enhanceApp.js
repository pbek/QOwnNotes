/**
 * Client app enhancement file.
 *
 * https://v1.vuepress.vuejs.org/guide/basic-config.html#app-level-enhancements
 */

import Vuetify from "vuetify";
import "vuetify/dist/vuetify.min.css";
import { findPageForPath } from "@app/util";
import VueFilterDateFormat from "@vuejs-community/vue-filter-date-format";
import VueFilterDateParse from "@vuejs-community/vue-filter-date-parse";

export default ({
  Vue, // the version of Vue being used in the VuePress app
  options, // the options for the root Vue instance
  router, // the router instance for the app
  siteData, // site metadata
}) => {
  // ...apply enhancements for the site.
  Vue.use(Vuetify);
  Vue.use(VueFilterDateFormat);
  Vue.use(VueFilterDateParse);
  options.vuetify = new Vuetify({});

  Vue.mixin({
    methods: {
      getPageKey(pages, path) {
        const result = findPageForPath(pages, path);
        return result.key;
      },
    },
  });
};
