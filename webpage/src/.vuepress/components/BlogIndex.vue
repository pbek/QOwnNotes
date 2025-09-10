<template>
  <div>
    <v-card v-for="page in files" class="mx-auto" outlined>
      <v-list-item three-line>
        <v-list-item-content>
          <div class="overline mb-4">
            {{
              page.frontmatter.date
                | dateParse("YYYY-MM-DDTHH")
                | dateFormat("YYYY-MM-DD")
            }}
          </div>
          <v-list-item-title class="headline mb-1">
            <a v-bind:href="page.path">{{ page.title }}</a>
          </v-list-item-title>
          <v-list-item-subtitle>{{
            page.frontmatter.description
          }}</v-list-item-subtitle>
        </v-list-item-content>
      </v-list-item>
    </v-card>
  </div>
</template>

<script>
export default {
  computed: {
    files() {
      // console.log("this.$site.pages.reverse()", this.$site.pages.reverse());
      return this.$site.pages.reverse().filter((p) => {
        return p.path.indexOf("/blog/") >= 0 && p.title !== "Overview";
      });
    },
  },
};
</script>
<style scoped>
.v-card {
  /*padding: 0;*/
  margin: 10px 0;
}
</style>
