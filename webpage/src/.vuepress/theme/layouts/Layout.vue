<template>
  <ParentLayout>
    <template #page-bottom>
      <div>
          Made by <a href="https://www.keybase.io/pbek">Patrizio Bekerle</a> with ❤️
        </div>
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
