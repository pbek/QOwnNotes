<template>
  <div>
    <v-snackbar
      id="poll"
      v-model="snackbar"
      fixed
      bottom
      right
      color="#389d70"
      :timeout="-1"
    >
      <v-text-field
        v-model="answer"
        :counter="200"
        label="How did you find out about QOwnNotes?"
        required
        @keydown.enter="submit"
      ></v-text-field>

      <v-btn light @click="submit" title="Submit your answer"> Submit </v-btn>
      <v-btn text title="Close poll" @click="snackbar = false">
        <v-icon>mdi-close</v-icon>
      </v-btn>
    </v-snackbar>
    <v-snackbar
      v-model="success"
      fixed
      bottom
      right
      color="#389d70"
      :timeout="1500"
    >
      Thank you for letting us know!
    </v-snackbar>
  </div>
</template>

<script>
export default {
  props: ["action", "name"],
  data() {
    return {
      snackbar: false,
      success: false,
      answer: "",
      pollId: 1,
      sentPolls: [],
    };
  },
  mounted() {
    this.sentPolls = JSON.parse(
      window.localStorage.getItem("qon-polls") || "[]",
    );

    // turn on poll snackbar if poll wasn't sent already
    if (!this.sentPolls.includes(this.pollId)) {
      this.snackbar = true;
    }
  },
  methods: {
    submit() {
      if (this.answer === "") {
        return;
      }

      // store that poll was sent, so it will not be asked for next time
      // (local storage will be synced across devices by the browser)
      this.sentPolls.push(this.pollId);
      window.localStorage.setItem("qon-polls", JSON.stringify(this.sentPolls));

      // check if the Matomo library is available
      if (typeof _paq !== "undefined") {
        _paq.push(["trackEvent", "Poll", "Found out about QON", this.answer]);
      }

      this.snackbar = false;
      this.success = true;
    },
  },
};
</script>

<style>
#poll .v-snack__wrapper {
  width: 352px;
  min-width: 100px;
}

#poll .v-input {
  padding-top: 5px;
  /* Force LTR so the first part of the question is readable */
  direction: ltr;
}
</style>
