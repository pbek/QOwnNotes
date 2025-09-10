<template>
  <v-subheader class="blog"
    >Date:
    {{
      fm.date | dateParse("YYYY-MM-DDTHH") | dateFormat("YYYY-MM-DD")
    }}</v-subheader
  >
</template>

<script>
export default {
  props: ["fm"],
};
</script>
