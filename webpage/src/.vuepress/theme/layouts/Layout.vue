<template>
  <ParentLayout>
    <template #page-bottom>
      <div class="page-nav">
        <div>
          Made by <a href="https://www.keybase.io/pbek" target="_blank">Patrizio Bekerle</a> with ❤️
        </div>
        <div class="social">
          <a href="https://github.com/pbek/QOwnNotes" target="_blank"><v-icon>mdi-github</v-icon></a>
          <a href="https://twitter.com/QOwnNotes" target="_blank"><v-icon>mdi-twitter</v-icon></a>
          <a href="https://www.facebook.com/QOwnNotes" target="_blank"><v-icon>mdi-facebook</v-icon></a>
        </div>
      </div>

      <Poll />
    </template>
  </ParentLayout>
</template>

<script>
import ParentLayout from '@parent-theme/layouts/Layout.vue'

export default {
  name: 'Layout',

  components: {
    ParentLayout
  }
}
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