<template>
  <v-card class="mx-auto">
    <v-row>
      <v-col align-self="start" cols="12" md="3" lg="2">
        <v-avatar class="profile" color="grey" :rounded="0" size="100">
          <v-img :src="img"></v-img>
        </v-avatar>
      </v-col>
      <v-col cols="12" md="9" lg="10">
        <v-list-item>
          <template v-slot:default>
            <v-list-item-title class="text-h6">
              {{ name }}
            </v-list-item-title>
            <v-list-item-subtitle>{{ functions }}</v-list-item-subtitle>
            <div class="mt-2">
              <a
                v-if="github"
                :href="'https://github.com/' + github"
                target="_blank"
                class="mr-3"
                >GitHub</a
              >
              <a
                v-if="keybase"
                :href="'https://keybase.io/' + keybase"
                target="_blank"
                class="mr-3"
                >Keybase</a
              >
              <a
                v-if="crowdin"
                :href="'https://crowdin.com/profile/' + crowdin"
                target="_blank"
                >Crowdin</a
              >
            </div>
          </template>
        </v-list-item>
      </v-col>
    </v-row>
  </v-card>
</template>

<script setup>
defineProps({
  backgroundImg: String,
  img: String,
  name: String,
  functions: String,
  github: String,
  keybase: String,
  crowdin: String,
});
</script>

<style scoped>
.v-card {
  margin-bottom: 20px;
}

.v-avatar {
  margin-left: 10px;
}
</style>
