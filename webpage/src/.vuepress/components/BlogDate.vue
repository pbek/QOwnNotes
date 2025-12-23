<template>
  <v-list-subheader class="blog"> Date: {{ formattedDate }} </v-list-subheader>
</template>

<script setup>
import { computed } from "vue";
import { format, parse } from "date-fns";

const props = defineProps({
  fm: {
    type: Object,
    required: true,
  },
});

const formattedDate = computed(() => {
  try {
    const date = parse(props.fm.date, "yyyy-MM-dd'T'HH", new Date());
    return format(date, "yyyy-MM-dd");
  } catch (e) {
    return props.fm.date;
  }
});
</script>
