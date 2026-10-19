<script setup>
import { mount, render } from "pele";
import { computed, onBeforeUnmount, onMounted, ref, watch } from "vue";
import { useRouter } from "vuepress/client";

const props = defineProps({
  code: { type: String, required: true },
  // "desktop" or "mobile" to only show the diagram on wide or narrow content
  // areas, an empty string shows it everywhere
  variant: {
    type: String,
    default: "",
    validator: (value) => ["", "desktop", "mobile"].includes(value),
  },
});

// Diagrams that Pele can't lay out narrow enough are not shrunk below this
// scale, the container scrolls horizontally instead to keep labels readable
const MIN_SCALE = 0.65;

const router = useRouter();
const container = ref(null);
const source = computed(() => decodeURIComponent(props.code));
let diagram = null;

const minWidth = (width) => `${Math.round(width * MIN_SCALE)}px`;

// Render once up front, so the diagram is part of the static HTML for
// search engines and visitors without JavaScript
const renderInitialSvg = () => {
  try {
    const { svg, width } = render(source.value);

    return svg.replace(
      /^<svg([^>]*?) style="/,
      `<svg$1 style="min-width:${minWidth(width)};`,
    );
  } catch {
    return "";
  }
};
const initialSvg = renderInitialSvg();

// Navigate internal diagram links with the router instead of reloading the page
const onClick = (event) => {
  if (
    event.defaultPrevented ||
    event.button !== 0 ||
    event.metaKey ||
    event.ctrlKey ||
    event.shiftKey ||
    event.altKey
  ) {
    return;
  }

  const link = event.target.closest?.("a[href]");
  const href = link?.getAttribute("href");

  if (!href || !href.startsWith("/") || href.startsWith("//")) {
    return;
  }

  event.preventDefault();
  router.push(href);
};

onMounted(() => {
  // Takes over from the static SVG and adapts the layout to the container width
  let centered = false;

  diagram = mount(container.value, source.value, {
    // Scrolling replaces zooming for diagrams that are too wide
    zoom: false,
    onRender: ({ width }) => {
      const element = container.value;
      const svg = element?.querySelector("svg.pele");

      if (!svg) {
        return;
      }

      svg.style.minWidth = minWidth(width);

      // Start in the middle of a scrollable diagram instead of its left edge
      if (!centered && element.scrollWidth > element.clientWidth) {
        element.scrollLeft = (element.scrollWidth - element.clientWidth) / 2;
        centered = true;
      }
    },
  });
});

watch(source, (value) => diagram?.update(value));

onBeforeUnmount(() => {
  diagram?.destroy();
  diagram = null;
});
</script>

<template>
  <div class="pele-diagram-wrapper">
    <div
      ref="container"
      :class="['pele-diagram', variant && `pele-diagram--${variant}`]"
      data-allow-mismatch="children"
      @click="onClick"
      v-html="initialSvg"
    />
  </div>
</template>

<style>
.pele-diagram {
  --pele-bg: var(--vp-c-bg);
  --pele-fg: var(--vp-c-text);
  --pele-muted: var(--vp-c-text-mute);
  --pele-line: var(--vp-c-text-mute);
  --pele-surface: var(--vp-c-bg-alt);
  --pele-surface-alt: var(--vp-c-control);
  --pele-border: var(--vp-c-border);
  --pele-accent: var(--vp-c-accent);
  --pele-font: var(--font-family);
  --pele-font-mono: var(--code-font-family, monospace);

  display: block;
  margin: 1rem 0;
  overflow-x: auto;
  /* Pele lays out diagrams left to right, also on RTL pages */
  direction: ltr;
}

.pele-diagram svg.pele {
  display: block;
  margin: 0 auto;
}

.pele-diagram a {
  cursor: pointer;
  text-decoration: none;
}

/* Switch between desktop and mobile diagrams based on the width of the
   content area instead of the window, which also accounts for the sidebar.
   The breakpoint is chosen so the concept page desktop diagram still fits at
   MIN_SCALE without scrolling */
.pele-diagram-wrapper {
  container-type: inline-size;
  /* The container would otherwise shrink next to floats, like the heading
     anchors on RTL pages */
  clear: both;
}

@container (max-width: 959px) {
  .pele-diagram--desktop {
    display: none;
  }
}

@container (min-width: 960px) {
  .pele-diagram--mobile {
    display: none;
  }
}
</style>
