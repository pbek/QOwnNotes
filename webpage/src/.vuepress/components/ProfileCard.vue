<template>
  <v-card class="mx-auto" tile>
    <v-row>
      <v-col align-self="start" cols="12" md="3" lg="2">
        <v-avatar class="profile" color="grey" rounded="" size="100" tile>
          <v-img v-bind:src="img"></v-img>
        </v-avatar>
      </v-col>
      <v-col cols="12" md="9" lg="10">
        <v-list-item>
          <v-list-item-content>
            <v-list-item-title class="title">
              {{ name }}
            </v-list-item-title>
            <v-list-item-subtitle>{{ functions }}</v-list-item-subtitle>
            <a
              v-if="github"
              v-bind:href="'https://github.com/' + github"
              target="_blank"
              >GitHub</a
            >
            <a
              v-if="keybase"
              v-bind:href="'https://keybase.io/' + keybase"
              target="_blank"
              >Keybase</a
            >
            <a
              v-if="crowdin"
              v-bind:href="'https://crowdin.com/profile/' + crowdin"
              target="_blank"
              >Crowdin</a
            >
          </v-list-item-content>
        </v-list-item>
      </v-col>
    </v-row>
  </v-card>
</template>

<script>
export default {
  props: [
    "backgroundImg",
    "img",
    "name",
    "functions",
    "github",
    "keybase",
    "crowdin",
  ],
};
</script>

<style scoped>
.v-card {
  margin-bottom: 20px;
}

.v-avatar {
  margin-left: 10px;
}
</style>
