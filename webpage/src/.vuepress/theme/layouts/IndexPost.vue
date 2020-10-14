<template>
  <ParentLayout>
    <template #page-content>
      <div>
        <v-card
          :to="page.path"
          class="mb-4"
          v-for="page in $pagination.pages"
          :key="page.key"
        >
          <v-card-title primary-title>
            <div>
              <div class="display-1 mb-2">{{page.frontmatter.title}}</div>
              <span>{{page.frontmatter.date}}</span>
            </div>
          </v-card-title>

          <v-card-text class="subheading"
            >{{page.frontmatter.abstract}}</v-card-text
          >
          <div></div>
          <v-card-actions>
            <v-btn block flat color="primary">Read more!</v-btn>
          </v-card-actions>
        </v-card>
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
