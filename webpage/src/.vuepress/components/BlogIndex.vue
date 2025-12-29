<template>
  <div>
    <v-card
      v-for="page in blogPages"
      :key="page.path"
      class="mx-auto"
      variant="outlined"
    >
      <v-list-item>
        <template v-slot:default>
          <div class="overline mb-4">
            {{ page.date }}
          </div>
          <v-list-item-title class="text-h5 mb-1">
            <a :href="page.path">{{ page.title }}</a>
          </v-list-item-title>
          <v-list-item-subtitle>
            {{ page.description }}
          </v-list-item-subtitle>
        </template>
      </v-list-item>
    </v-card>
  </div>
</template>

<script setup>
import { ref, onMounted } from "vue";
import blogData from "../blog-data.json";

const blogPages = ref([]);

onMounted(() => {
  // Load blog data from generated JSON file
  blogPages.value = blogData;
});
</script>

<style scoped>
.v-card {
  margin: 10px 0;
}
</style>
