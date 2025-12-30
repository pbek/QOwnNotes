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
    // Handle if date is already a Date object or ISO string
    let date;
    if (props.fm.date instanceof Date) {
      date = props.fm.date;
    } else if (typeof props.fm.date === "string") {
      // Try to parse as ISO string first (handles both "2016-11-05T10:51:20+00:00" and "2021-08-25")
      date = new Date(props.fm.date);

      // Check if the date is valid
      if (isNaN(date.getTime())) {
        // If not valid, try parsing with date-fns
        date = parse(props.fm.date, "yyyy-MM-dd", new Date());
      }
    } else {
      return props.fm.date;
    }

    return format(date, "yyyy-MM-dd");
  } catch (e) {
    return props.fm.date;
  }
});
</script>
