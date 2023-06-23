# Übersicht

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="QOwnNotes Screenshot" />
    <div class="sheet">
      Bearbeiten Sie Ihre Notizen mit Markdown-Hervorhebungen, farbigen Tags und Unterordnern
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="Minimale Ansicht" />
    <div class="sheet">
      Minimale Standardbenutzeroberfläche, die noch weiter entfernt werden kann
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-vertical.png" alt="Vertikale Ansicht" />
    <div class="sheet">
      Zeigen Sie Ihre Notizen in einer vertikalen Markdown-Ansicht an, indem Sie die Bedienfelder verschieben
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-portable-mode.png" alt="Portabler Modus" />
    <div class="sheet">
      Tragbarer Modus für USB-Sticks
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-1col.png" alt="Eine Spalte" />
    <div class="sheet">
      Alle Bedienflächen können beliebig platziert werden
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-darkmode.png" alt="Screenshot-Dunkelmodus" />
    <div class="sheet">
      Dunkelmodus
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-distraction-free-mode.png" alt="Screenshot-ablenkungsfreier-Modus" />
    <div class="sheet">
      Ablenkungsfreier Modus
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note-decrypted.png" alt="Notizenverschlüsselung" />
    <div class="sheet">
      Optionale AES-Notizenverschlüsselung (auch skriptfähig)
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note.png" alt="Verschlüsselte Notiz" />
    <div class="sheet">
      Verschlüsselte Notizen sind immer noch Text
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-diff.png" alt="screenshot diff" />
    <div class="sheet">
      Zeigen Sie den Unterschied zwischen Notizen an, wenn diese extern geändert wurden
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-export-print.png" alt="screenshot-export-print" />
    <div class="sheet">
      Notizen-PDF-Export und -Druck
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-freedesktop-theme.png" alt="screenshot-freedesktop-theme" />
    <div class="sheet">
      Symbole über Freedesktop-Theme
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-other-workspace.png" alt="screenshot-other-workspace" />
    <div class="sheet">
      Ihnen stehen verschiedene Arbeitsbereiche zur Verfügung
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-qml.png" alt="screenshot-qml" />
    <div class="sheet">
      Skriptfähig
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-russian.png" alt="screenshot-russian" />
    <div class="sheet">
      In viele Sprachen übersetzt
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-all-notes.png" alt="screenshot-search-in-all-notes" />
    <div class="sheet">
      Durchsuchen Sie alle Notizen
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-current-note.png" alt="screenshot-search-in-current-note" />
    <div class="sheet">
      Suchen Sie in der aktuellen Notiz
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-settings-note-folders.png" alt="screenshot-settings-note-folders" />
    <div class="sheet">
      Kann mehrere Notizordner verwenden
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-todo.png" alt="screenshot-todo" />
    <div class="sheet">
      Verwalten Sie Ihre Todo-Listen über CalDAV
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-trash.png" alt="screenshot-trash" />
    <div class="sheet">
      Verwalten Sie verworfene Notizen auf Ihrem Nextcloud-Server
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-versioning.png" alt="screenshot-versioning" />
    <div class="sheet">
      Verwalten Sie Ihre Notizversionen auf Ihrem Nextcloud-Server
    </div>
  </v-carousel-item>
</v-carousel>
</template>

<v-divider />

**Q** für das verwendete [Qt-Framework](https://www.qt.io/)   
**Own**, weil Sie Ihre eigenen Notizen besitzen  
**Notes** für die Notizen, die Sie erstellen

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) ist die **Open Source** (GPL) -Nur-Text-Datei-**Markdown-Notizanwendung** für **GNU/ Linux**, **MacOS** und **Windows**, die (optional) mit der [**Notizenanwendung**](https://github.com/nextcloud/notes) von [Nextcloud](https://nextcloud.com/) und [ownCloud](https://owncloud.org/) zusammenarbeitet.

::: tip
Treten Sie unserer [Telegram Gruppe](https://t.me/QOwnNotes) bei, um zu diskutieren, zu lernen und sich mit der QOwnNotes-Community zu verbinden.
:::

[Verstehen Sie das Konzept von QOwnNotes](concept.md)

## Eigenschaften
- You can use your **existing text or markdown files**, no need for an import
- QOwnNotes ist in C++ geschrieben und für **geringen Ressourcenverbrauch** optimiert (keine CPU- und speicherhungrige Electron-App)
- **Evernote- und Joplin-Import**
- **Compatible** with the [Notes](https://apps.nextcloud.com/apps/notes) application of **Nextcloud / ownCloud** and **mobile** and Nextcloud Text
- Create **[Nextcloud Deck](https://apps.nextcloud.com/apps/deck) cards** and link to them in your notes
- Support for **sharing notes** on your **ownCloud** / **Nextcloud** server
- Manage your Nextcloud / ownCloud **todo lists** (**Nextcloud tasks** or **Tasks Plus** / **Calendar Plus**) or use another **CalDAV server** to sync your tasks to
- Older **versions** of your notes can be **restored** from your **Nextcloud / ownCloud** server
- **Trashed notes** can be **restored** from your **Nextcloud / ownCloud** server
- **External changes** of note files are **watched** (notes or note list are reloaded)
- Support for **hierarchical note tagging** and **note subfolders**
- Optional **encryption** of notes, AES-256 is built in, or you can use custom encryption methods like **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/doc/scripting/encryption-keybase.qml)) or **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/doc/scripting/encryption-pgp.qml))
- The **notes folder** can be **freely chosen** (multiple note folders can be used)
- **Searching parts of words** in notes is possible and search results are highlighted in the notes
- [**Browser extension**](browser-extension.md) to **add notes from the selected text**, **make screenshots** or **manage your bookmarks**
- [**Spellchecking**](../editor/spellchecking.md) support
- **Portable mode** for carrying QOwnNotes around on USB sticks
- **Scripting support** and an online [**script repository**](https://github.com/qownnotes/scripts) where you can install scripts inside the application
- **Vim-Modus**
- **[Web application](web-app.md)** to **insert photos from your mobile phone** into the current note in QOwnNotes on your desktop


## Aussehen
- **Markdown-Hervorhebung** von Notizen und ein **Markdown-Vorschaumodus**
- **Unterstützung für Schemata im Dunkelmodus**
- **Theming support** for the **Markdown syntax highlighting**
- Alle **Panels können platziert werden, wo immer Sie möchten**, sie können sogar **schweben** oder **gestapelt werden** (vollständig andockbar).
- Unterstützung für **Freedesktop-Schemasymbole**. Sie können QOwnNotes mit Ihren **originalen Desktop-Symbolen** und mit Ihrem bevorzugten **dunklen Desktop-Design** verwenden
- **Ablenkungsfreier Modus**, **Vollbildmodus** und **Schreibmaschinenmodus**
- **Zeilennummern**
- **Unterschiede** zwischen aktueller Notiz und extern geänderter Notiz werden in einem Dialog **angezeigt**

## Sprachen
- **Verfügbar in mehr als 60 verschiedenen Sprachen** wie Englisch, Deutsch, Französisch, Polnisch, Chinesisch, Japanisch, Russisch, Portugiesisch, Ungarisch, Niederländisch und Spanisch
  - [Ihre Hilfe](../contributing/translation.md) wird sehr geschätzt, um diese Übersetzungen zu verbessern oder QOwnNotes in mehr Sprachen zu übersetzen

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
