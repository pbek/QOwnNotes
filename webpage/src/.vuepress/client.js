import { defineClientConfig } from "vuepress/client";
import { createVuetify } from "vuetify";
import {
  VBtn,
  VCard,
  VCardText,
  VCardTitle,
  VChip,
  VCol,
  VContainer,
  VIcon,
  VList,
  VListItem,
  VListItemSubtitle,
  VListItemTitle,
  VListSubheader,
  VRow,
  VSnackbar,
  VTextField,
} from "vuetify/components";
import * as directives from "vuetify/directives";
import "vuetify/styles";
import "@mdi/font/css/materialdesignicons.css";

// Import custom components
import Poll from "./components/Poll.vue";
import BlogDate from "./components/BlogDate.vue";
import BlogIndex from "./components/BlogIndex.vue";
import ProfileCard from "./components/ProfileCard.vue";

export default defineClientConfig({
  enhance({ app, router, siteData }) {
    // Create Vuetify instance with only the components we use
    const vuetify = createVuetify({
      components: {
        VBtn,
        VCard,
        VCardText,
        VCardTitle,
        VChip,
        VCol,
        VContainer,
        VIcon,
        VList,
        VListItem,
        VListItemSubtitle,
        VListItemTitle,
        VListSubheader,
        VRow,
        VSnackbar,
        VTextField,
      },
      directives,
      theme: {
        defaultTheme: "light",
      },
    });

    // Use Vuetify
    app.use(vuetify);

    // Register custom components globally
    app.component("Poll", Poll);
    app.component("BlogDate", BlogDate);
    app.component("BlogIndex", BlogIndex);
    app.component("ProfileCard", ProfileCard);
  },
  setup() {},
  rootComponents: [],
});
