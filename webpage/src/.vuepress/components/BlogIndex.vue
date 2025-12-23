<template>
  <div>
    <v-card
      v-for="page in files"
      :key="page.path"
      class="mx-auto"
      variant="outlined"
    >
      <v-list-item>
        <template v-slot:default>
          <div class="overline mb-4">
            {{ formatDate(page.frontmatter.date) }}
          </div>
          <v-list-item-title class="text-h5 mb-1">
            <a :href="page.path">{{ page.title }}</a>
          </v-list-item-title>
          <v-list-item-subtitle>
            {{ page.frontmatter.description }}
          </v-list-item-subtitle>
        </template>
      </v-list-item>
    </v-card>
  </div>
</template>

<script setup>
import { computed } from "vue";
import { usePageData, useSiteData } from "vuepress/client";
import { format, parse } from "date-fns";

const siteData = useSiteData();

const files = computed(() => {
  const pages = siteData.value.pages;
  if (!pages || !Array.isArray(pages)) {
    return [];
  }
  return [...pages]
    .reverse()
    .filter((p) => p.path.indexOf("/blog/") >= 0 && p.title !== "Overview");
});

const formatDate = (dateString) => {
  try {
    const date = parse(dateString, "yyyy-MM-dd'T'HH", new Date());
    return format(date, "yyyy-MM-dd");
  } catch (e) {
    return dateString;
  }
};
</script>

<style scoped>
.v-card {
  margin: 10px 0;
}
</style>
