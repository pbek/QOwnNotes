<template>
  <div>
    <v-snackbar
      id="poll"
      v-model="snackbar"
      location="bottom right"
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

      <template v-slot:actions>
        <v-btn variant="flat" @click="submit" title="Submit your answer"
          >Submit</v-btn
        >
        <v-btn variant="text" title="Close poll" @click="snackbar = false">
          <v-icon>mdi-close</v-icon>
        </v-btn>
      </template>
    </v-snackbar>
    <v-snackbar
      v-model="success"
      location="bottom right"
      color="#389d70"
      :timeout="1500"
    >
      Thank you for letting us know!
    </v-snackbar>
  </div>
</template>

<script setup>
import { ref, onMounted } from "vue";

defineProps({
  action: String,
  name: String,
});

const snackbar = ref(false);
const success = ref(false);
const answer = ref("");
const pollId = ref(1);
const sentPolls = ref([]);

onMounted(() => {
  if (typeof window !== "undefined") {
    sentPolls.value = JSON.parse(
      window.localStorage.getItem("qon-polls") || "[]",
    );

    // turn on poll snackbar if poll wasn't sent already
    if (!sentPolls.value.includes(pollId.value)) {
      snackbar.value = true;
    }
  }
});

const submit = () => {
  if (answer.value === "") {
    return;
  }

  // store that poll was sent, so it will not be asked for next time
  // (local storage will be synced across devices by the browser)
  sentPolls.value.push(pollId.value);
  if (typeof window !== "undefined") {
    window.localStorage.setItem("qon-polls", JSON.stringify(sentPolls.value));

    // check if the Matomo library is available
    if (typeof window._paq !== "undefined") {
      window._paq.push([
        "trackEvent",
        "Poll",
        "Found out about QON",
        answer.value,
      ]);
    }
  }

  snackbar.value = false;
  success.value = true;
};
</script>

<style>
#poll .v-snackbar__content {
  width: 352px;
  min-width: 100px;
}

#poll .v-input {
  padding-top: 5px;
  /* Force LTR so the first part of the question is readable */
  direction: ltr;
}
</style>
