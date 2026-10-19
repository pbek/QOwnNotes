import { defineAsyncComponent } from "vue";
import { defineClientConfig } from "vuepress/client";

export default defineClientConfig({
  enhance({ app }) {
    // Load Pele in its own chunk, so only pages with diagrams download it
    app.component(
      "PeleDiagram",
      defineAsyncComponent(() => import("./PeleDiagram.vue")),
    );
  },
});
