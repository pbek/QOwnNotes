<template>
  <v-list>
    <v-list-tile v-for="tag in $tag.list" :to="tag.path">
      <v-list-tile-content>{{ tag.name }}</v-list-tile-content>
    </v-list-tile>
  </v-list>
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
