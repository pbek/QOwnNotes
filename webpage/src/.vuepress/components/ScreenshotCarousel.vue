<template>
  <v-carousel
    v-if="isMounted"
    :key="carouselKey"
    cycle
    show-arrows-on-hover
    height="auto"
  >
    <v-carousel-item>
      <img src="/screenshots/screenshot.png" alt="QOwnNotes screenshot" />
      <div class="sheet">
        Edit your notes with markdown highlighting, colored tags and subfolders
      </div>
    </v-carousel-item>
    <v-carousel-item>
      <img src="/screenshots/screenshot-minimal.png" alt="Minimal view" />
      <div class="sheet">
        Minimal default user interface that can be stripped even more
      </div>
    </v-carousel-item>
    <v-carousel-item>
      <img src="/screenshots/screenshot-vertical.png" alt="Vertical view" />
      <div class="sheet">
        View your notes in a vertical markdown view by moving the panels
      </div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-note-relations.png"
        alt="Note relations"
      />
      <div class="sheet">Note relations panel</div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-portable-mode.png"
        alt="Portable mode"
      />
      <div class="sheet">Portable mode for USB sticks</div>
    </v-carousel-item>
    <v-carousel-item>
      <img src="/screenshots/screenshot-1col.png" alt="One column" />
      <div class="sheet">All panels can be placed wherever you want</div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-darkmode.png"
        alt="screenshot darkmode"
      />
      <div class="sheet">Dark mode</div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-distraction-free-mode.png"
        alt="screenshot-distraction-free-mode"
      />
      <div class="sheet">Distraction free mode</div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-encrypted-note-decrypted.png"
        alt="Note encryption"
      />
      <div class="sheet">Optional AES note encryption (also scriptable)</div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-encrypted-note.png"
        alt="Encrypted note"
      />
      <div class="sheet">Encrypted notes are still text</div>
    </v-carousel-item>
    <v-carousel-item>
      <img src="/screenshots/screenshot-diff.png" alt="screenshot diff" />
      <div class="sheet">
        Show the difference between notes when it was changed externally
      </div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-export-print.png"
        alt="screenshot-export-print"
      />
      <div class="sheet">Note PDF export and printing</div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-freedesktop-theme.png"
        alt="screenshot-freedesktop-theme"
      />
      <div class="sheet">Icons via Freedesktop theme</div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-other-workspace.png"
        alt="screenshot-other-workspace"
      />
      <div class="sheet">You can have different workspaces</div>
    </v-carousel-item>
    <v-carousel-item>
      <img src="/screenshots/screenshot-qml.png" alt="screenshot-qml" />
      <div class="sheet">Scriptable</div>
    </v-carousel-item>
    <v-carousel-item>
      <img src="/screenshots/screenshot-russian.png" alt="screenshot-russian" />
      <div class="sheet">Translated to many languages</div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-search-in-all-notes.png"
        alt="screenshot-search-in-all-notes"
      />
      <div class="sheet">Search over all notes</div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-search-in-current-note.png"
        alt="screenshot-search-in-current-note"
      />
      <div class="sheet">Search in the current note</div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-settings-note-folders.png"
        alt="screenshot-settings-note-folders"
      />
      <div class="sheet">Able to use multiple note folders</div>
    </v-carousel-item>
    <v-carousel-item>
      <img src="/screenshots/screenshot-todo.png" alt="screenshot-todo" />
      <div class="sheet">Manage your Todo lists via CalDAV</div>
    </v-carousel-item>
    <v-carousel-item>
      <img src="/screenshots/screenshot-trash.png" alt="screenshot-trash" />
      <div class="sheet">Manage trashed notes on your Nextcloud server</div>
    </v-carousel-item>
    <v-carousel-item>
      <img
        src="/screenshots/screenshot-versioning.png"
        alt="screenshot-versioning"
      />
      <div class="sheet">
        Manage your note versions on your Nextcloud server
      </div>
    </v-carousel-item>
  </v-carousel>
</template>

<script setup>
import { ref, onMounted, nextTick } from "vue";

const isMounted = ref(false);
const carouselKey = ref(0);

onMounted(() => {
  // Use nextTick to ensure DOM is fully ready
  nextTick(() => {
    // Small delay to ensure Vuetify is fully initialized
    setTimeout(() => {
      isMounted.value = true;
      carouselKey.value = Date.now(); // Unique key to force re-render
    }, 100);
  });
});
</script>

<style scoped>
.v-carousel {
  margin: 2rem 0;
}

.v-carousel img {
  width: 100%;
  height: auto;
  display: block;
}

.sheet {
  background: rgba(0, 0, 0, 0.7);
  color: white;
  padding: 1rem;
  text-align: center;
  position: absolute;
  bottom: 0;
  left: 0;
  right: 0;
}
</style>
