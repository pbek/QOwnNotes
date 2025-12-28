<template>
  <Layout>
    <template #page-bottom>
      <div class="page-nav">
        <div>
          Crafted by
          <a href="https://bekerle.com" target="_blank">Patrizio Bekerle</a>
          with ❤️
        </div>
        <div class="social">
          <a href="https://github.com/pbek/QOwnNotes" target="_blank">
            <v-icon>mdi-github</v-icon>
          </a>
          <a href="https://social.qownnotes.org/@qownnotes" target="_blank">
            <v-icon>mdi-mastodon</v-icon>
          </a>
        </div>
      </div>
    </template>
  </Layout>
</template>

<script setup>
import Layout from "@vuepress/theme-default/layouts/Layout.vue";
</script>

<style scoped>
.page-nav {
  display: grid;
  grid-template-columns: 50% auto;
}

.social {
  text-align: right;
}
</style>

<style>
.theme-default-content:not(.custom) {
  max-width: 900px;
}
</style>
