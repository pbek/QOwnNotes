<template>
  <v-snackbar
      v-model="snackbar"
      absolute
      bottom
      right
      color="#389d70"
      :timeout="-1"
  >
    <form>
      <v-text-field
          v-model="name"
          :counter="200"
          label="Name"
          required
      ></v-text-field>

      <v-btn
          class=""
          @click="submit"
      >
        submit
      </v-btn>
    </form>

    <template v-slot:action="{ attrs }">
      <v-btn
          color="red"
          text
          v-bind="attrs"
          @click="snackbar = false"
      >
        Close
      </v-btn>
    </template>
  </v-snackbar>
</template>

<script>
  export default {
    props: ['action', 'name'],
    data() {
      return {
        snackbar: false,
      }
    }
  }
</script>

<style scoped>
</style>
