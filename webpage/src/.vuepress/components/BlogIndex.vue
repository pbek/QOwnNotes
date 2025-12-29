<template>
  <div class="blog-index">
    <article v-for="page in blogPages" :key="page.path" class="blog-post">
      <time class="blog-date">{{ page.date }}</time>
      <h2 class="blog-title">
        <a :href="page.path" class="blog-link">{{ page.title }}</a>
      </h2>
      <p v-if="page.description" class="blog-description">
        {{ page.description }}
      </p>
    </article>
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
.blog-index {
  max-width: 740px;
  margin: 0 auto;
  padding: 0 1.5rem;

  h2 {
    padding-top: 0;
  }
}

.blog-post {
  margin-bottom: 0.75rem;
  padding-bottom: 0.5rem;
  border-bottom: 1px solid var(--c-border, #eaecef);
}

.blog-post:last-child {
  border-bottom: none;
}

.blog-date {
  display: block;
  font-size: 0.85rem;
  color: var(--c-text-lighter, #7f8c8d);
  margin-bottom: 0.15rem;
}

.blog-title {
  margin: 0 0 0.15rem;
  font-size: 1.35rem;
  font-weight: 600;
  line-height: 1.3;
}

.blog-link {
  color: var(--c-text, #2c3e50);
  text-decoration: none;
  border-bottom: 2px solid transparent;
  transition: border-color 0.3s;
}

.blog-link:hover {
  color: var(--c-brand, #3eaf7c);
  border-bottom-color: var(--c-brand, #3eaf7c);
}

.blog-description {
  margin: 0;
  font-size: 0.95rem;
  line-height: 1.5;
  color: var(--c-text, #2c3e50);
}
</style>
