# Overview

<template>
<v-carousel cycle show-arrows-on-hover>
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
    <img src="/screenshots/screenshot-portable-mode.png" alt="Portable mode" />
    <div class="sheet">
      Portable mode for USB sticks
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-1col.png" alt="One column" />
    <div class="sheet">
      All panels can be placed wherever you want
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-darkmode.png" alt="screenshot darkmode" />
    <div class="sheet">
      Dark mode
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-distraction-free-mode.png" alt="screenshot-distraction-free-mode" />
    <div class="sheet">
      Distraction free mode
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note-decrypted.png" alt="Note encryption" />
    <div class="sheet">
      Optional AES note encryption (also scriptable)
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note.png" alt="Encrypted note" />
    <div class="sheet">
      Encrypted notes are still text
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-diff.png" alt="screenshot diff" />
    <div class="sheet">
      Show the difference between notes when it was changed externally
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-export-print.png" alt="screenshot-export-print" />
    <div class="sheet">
      Note PDF export and printing
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-freedesktop-theme.png" alt="screenshot-freedesktop-theme" />
    <div class="sheet">
      Icons via Freedesktop theme
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-other-workspace.png" alt="screenshot-other-workspace" />
    <div class="sheet">
      You can have different workspaces
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-qml.png" alt="screenshot-qml" />
    <div class="sheet">
      Scriptable
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-russian.png" alt="screenshot-russian" />
    <div class="sheet">
      Translated to many languages
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-all-notes.png" alt="screenshot-search-in-all-notes" />
    <div class="sheet">
      Search over all notes
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-current-note.png" alt="screenshot-search-in-current-note" />
    <div class="sheet">
      Search in the current note
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-settings-note-folders.png" alt="screenshot-settings-note-folders" />
    <div class="sheet">
      Able to use multiple note folders
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-todo.png" alt="screenshot-todo" />
    <div class="sheet">
      Manage your Todo lists via CalDAV
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-trash.png" alt="screenshot-trash" />
    <div class="sheet">
      Manage trashed notes on your Nextcloud server
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-versioning.png" alt="screenshot-versioning" />
    <div class="sheet">
      Manage your note versions on your Nextcloud server
    </div>
  </v-carousel-item>
</v-carousel>
</template>

<v-divider />

**Q** for the [Qt framework](https://www.qt.io/) that is used  
**Own** because you own your own notes  
**Notes** for the notes you take

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) is the **open source** (GPL) plain-text file
**markdown note taking** application for **GNU/Linux**, **Mac OS X** and **Windows**, that
(optionally) works together with the [**notes application**](https://github.com/nextcloud/notes)
of [Nextcloud](https://nextcloud.com/) and [ownCloud](https://owncloud.org/).

::: tip
  Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community.
:::

[Understand the concept of QOwnNotes](concept.md)

## Features

- The **notes folder** can be **freely chosen** (multiple note folders can be used)
- You can use your **existing text or markdown files**, no need for an import most of the times
- **Sub-string searching** of notes is possible and search results are highlighted in the notes
- **Keyboard shortcuts** are customizable
- **Scripting support** and an online [**script repository**](https://github.com/qownnotes/scripts) where you can install scripts inside the application
- [Browser extension](browser-extension.md) to add notes from the selected text, make screenshots or manage your bookmarks
- **External changes** of note files are **watched** (notes or note list are reloaded)
- Older **versions** of your notes can be **restored** from your **Nextcloud / ownCloud** server
- **Trashed notes** can be **restored** from your **Nextcloud / ownCloud** server
- **Differences** between current note and externally changed note are **showed** in a dialog
- **Markdown highlighting** of notes and a **markdown preview mode**
- [**Spellchecking**](../editor/spellchecking.md) support
- **Compatible** with the Notes application of **Nextcloud / ownCloud** and **mobile** and Nextcloud Text
- Manage your Nextcloud / ownCloud **todo lists** (**Nextcloud tasks** or **Tasks Plus** / **Calendar Plus**) or use an other **CalDAV server** to sync your tasks to
- Optional **encryption** of notes (AES-256 is built in or you can use custom encryption methods like **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-keybase.qml)) or **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-pgp.qml)))
- **Dark mode theme support**
- **Theming support** for the **markdown syntax highlighting**
- All **panels can be placed wherever you want**,  they can even **float** or **stack** (fully dockable)
- Support for **freedesktop theme icons**, you can use QOwnNotes with your **native desktop icons** and with your favorite **dark desktop theme**
- Support for **hierarchical note tagging** and **note subfolders**
- Support for **sharing notes** on your **ownCloud** / **Nextcloud** server
- **Portable mode** for carrying QOwnNotes around on USB sticks
- **Vim mode**
- **Distraction free mode**, **Full-screen mode** and **Typewriter mode**
- **Evernote import**
- **Available in many different languages** like English, German, French, Polish, Chinese, Japanese, Russian, Portuguese, Hungarian, Dutch and Spanish
  - [Your help](../contributing/translation.md) is very much appreciated to improve these translations or to translate QOwnNotes in more languages

<style>
.sheet {
  position: absolute;
  bottom: 50px;
  background-color: rgba(0,0,0, 0.5);
  color: white;
  text-align: center;
  display: flex;
  align-items:center;
  justify-content:center;
  height: 50px;
  width: 100%;
}

.v-window__next {
  right: 0;
}

@media (max-width: 500px) {
  .v-carousel {
    height: 400px!important;
  }
}

@media (max-width: 350px) {
  .v-carousel {
    height: 250px!important;
  }
}

@media (max-width: 200px) {
  .v-carousel {
    height: 150px!important;
  }
}
</style>
