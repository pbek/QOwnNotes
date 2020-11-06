# Überblick

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="QOwnNotes screenshot" />
    <div class="sheet">
      Edit your notes with markdown highlighting, colored tags and subfolders
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="Minimale Ansicht" />
    <div class="sheet">
      Minimal default user interface that can be stripped even more
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-vertical.png" alt="Vertikale Ansicht" />
    <div class="sheet">
      View your notes in a vertical markdown view by moving the panels
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-portable-mode.png" alt="Portabler Modus" />
    <div class="sheet">
      Portable mode for USB sticks
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-1col.png" alt="Eine Spalte" />
    <div class="sheet">
      All panels can be placed wherever you want
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-darkmode.png" alt="Screenshot-Dunkelmodus" />
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

** Q ** für das verwendete [ Qt-Framework ](https://www.qt.io/)   
** Eigene **, weil Sie Ihre eigenen Notizen besitzen   
** Notizen ** für die Notizen, die Sie machen

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) ist die **Open Source** (GPL) -Nur-Text-Datei-**Markdown-Notizanwendung** für **GNU/ Linux**, **Mac OS X** und **Windows**, die (optional) mit der [**Notizenanwendung**](https://github.com/nextcloud/notes) von [Nextcloud](https://nextcloud.com/) und [ownCloud](https://owncloud.org/) zusammenarbeitet.

::: tip
Treten Sie unserer [ Telegrammgruppe ](https://t.me/QOwnNotes) bei, um zu diskutieren, zu lernen und sich mit der QOwnNotes-Community zu verbinden.
:::

[Verstehen Sie das Konzept von QOwnNotes](concept.md)

## Eigenschaften

- Der **Notizenordner ** kann ** frei gewählt werden ** (mehrere Notizordner können verwendet werden)
- Sie können Ihre ** vorhandenen Text- oder Markdown-Dateien ** verwenden, ohne dass ein Import erforderlich ist
- ** Die Suche nach Teilzeichenfolgen ** von Notizen ist möglich, und die Suchergebnisse werden in den Notizen hervorgehoben
- ** Tastaturkürzel ** können angepasst werden
- ** Skriptunterstützung ** und ein Online-Skript-Repository [** **](https://github.com/qownnotes/scripts), in dem Sie Skripte in der Anwendung installieren können
- [ Browser-Erweiterung ](browser-extension.md), um Notizen aus dem ausgewählten Text hinzuzufügen, Screenshots zu erstellen oder Ihre Lesezeichen zu verwalten
- ** Externe Änderungen ** von Notizdateien werden ** überwacht ** (Notizen oder Notizliste werden neu geladen)
- Ältere ** Versionen ** Ihrer Notizen können ** von Ihrem ** Nextcloud / ownCloud ** -Server <0> wiederhergestellt werden</li>
- ** Papierkorbnotizen ** können ** von Ihrem ** Nextcloud / ownCloud ** -Server wiederhergestellt ** werden
- ** Unterschiede ** zwischen aktueller Note und extern geänderter Note werden ** in einem Dialog ** angezeigt
- ** Markdown-Hervorhebung ** von Notizen und ein ** Markdown-Vorschaumodus **
- Unterstützung für [** Rechtschreibprüfung **](../editor/spellchecking.md)
- ** Kompatibel ** mit der Notes-Anwendung von ** Nextcloud / ownCloud ** und ** mobile ** und Nextcloud Text
- Verwalten Sie Ihre Nextcloud / ownCloud-**Aufgabenlisten** (**Nextcloud-Aufgaben** oder **Tasks Plus**/ **Calendar Plus**) oder verwenden Sie einen anderen CalDAV-Server, um Ihre Aufgaben mit zu synchronisieren
- Optionale **Verschlüsselung** von Notizen (AES-256 ist integriert oder Sie können benutzerdefinierte Verschlüsselungsmethoden wie **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-keybase.qml)) oder **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-pgp.qml)) verwenden.)
- **Unterstützung für Themen im dunklen Modus**
- ** Theming-Unterstützung ** für die Hervorhebung der ** Markdown-Syntax **
- Alle ** Panels können platziert werden, wo immer Sie möchten **, sie können sogar ** schweben ** oder ** stapeln ** (vollständig andockbar)
- Unterstützung für ** Freedesktop-Themensymbole **. Sie können QOwnNotes mit Ihren ** nativen Desktop-Symbolen ** und mit Ihrem bevorzugten ** dunklen Desktop-Design ** verwenden
- Unterstützung für ** hierarchische Notizkennzeichnung ** und ** Notizunterordner **
- Unterstützung für ** das Teilen von Notizen ** auf Ihrem ** ownCloud ** / ** Nextcloud ** -Server
- ** Tragbarer Modus ** zum Mitnehmen von QOwnNotes auf USB-Sticks
- **Vim-Modus**
- ** Ablenkungsfreier Modus **, ** Vollbildmodus ** und ** Schreibmaschinenmodus **
- **Evernote-Import**
- ** Verfügbar in vielen verschiedenen Sprachen ** wie Englisch, Deutsch, Französisch, Polnisch, Chinesisch, Japanisch, Russisch, Portugiesisch, Ungarisch, Niederländisch und Spanisch
  - [ Ihre Hilfe ](../contributing/translation.md) wird sehr geschätzt, um diese Übersetzungen zu verbessern oder QOwnNotes in mehr Sprachen zu übersetzen</ul>

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
