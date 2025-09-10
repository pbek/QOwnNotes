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
    <img src="/screenshots/screenshot-note-relations.png" alt="Note relations" />
    <div class="sheet">
      Note relations panel
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

[QOwnNotes](https://www.qownnotes.org/) ist die **Open Source** (GPL) Klartextdatei-**Markdown-Notizanwendung** für **GNU/Linux**, **MacOS** und **Windows**, die (optional) mit der [**Notizenanwendung**](https://github.com/nextcloud/notes) von [Nextcloud](https://nextcloud.com/) und [ownCloud](https://owncloud.org/) zusammenarbeitet.

::: tip
Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community.
:::

[Verstehen Sie das Konzept von QOwnNotes](concept.md)

## Features

- Sie können Ihre **bereits vorhandenen Text- oder Markdowndateien** verwenden; kein Import nötig
- QOwnNotes ist in C++ geschrieben und für **geringen Ressourcenverbrauch** optimiert (keine CPU- und speicherhungrige Electron-App)
- **Evernote- und Joplin-Import**
- **Kompatibel** mit der [Notes](https://apps.nextcloud.com/apps/notes)-App von **Nextcloud / ownCloud** und **Mobile** und Nextcloud Text
- Erstellen Sie **[Nextcloud Deck](https://apps.nextcloud.com/apps/deck) Karten** und verlinken Sie sie in Ihren Notizen
- Unterstützt das **Teilen von Notizen** auf Ihrem **ownCloud** / **Nextcloud** Server
- Verwalten Sie Ihre Nextcloud / ownCloud **To-do-Listen** (**Nextcloud tasks** oder **Tasks Plus** / **Calendar Plus**) oder verwenden Sie einen anderen **CalDAV**, um Ihre Aufgaben darauf zu synchronisieren
- Ältere **Versionen** Ihrer Notizen können von Ihrem **Nextcloud / ownCloud**-Server **wiederhergestellt** werden
- **Gelöschte Notizen** können von Ihrem **Nextcloud / ownCloud**-Server **wiederhergestellt** werden
- **Externe Änderungen** von Notizdateien werden **überwacht** (Notizen oder Notizliste werden neu geladen)
- Unterstützung für **hierarchische Notizschlagwörter** und **Notiz-Unterordner**
- Optionale **Verschlüsselung** von Notizen, AES-256 ist eingebaut, oder Sie können personalisierte Verschlüsselungsmethoden wie **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) oder **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml)) benutzen
- Der **Notizenordner** kann **frei gewählt** werden (mehrere Notizenordner können verwendet werden)
- **Das Suchen von Wortstücken** in Notizen ist möglich und Suchergebnisse werden in den Notizen hervorgehoben
- [**Browsererweiterung**](browser-extension.md) um **Notizen aus ausgewähltem Text hinzuzufügen**, **Screenshots zu erstellen** oder **um Ihre Lesezeichen zu verwalten**
- [**Rechtschreibprüfung**](../editor/spellchecking.md) wird unterstützt
- **Portabler Modus** um QOwnNotes auf USB-Sticks mitzunehmen
- **Skriptunterstützung** und ein online [**Skriptverzeichnis**](https://github.com/qownnotes/scripts), wo Sie Skripte in der App installieren können
- Implementation of the **[OpenAI completion API](https://www.qownnotes.org/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html)** to be used in scripts
- **Vim-Modus**
- **[Web-App](web-app.md)** um **Fotos von Ihrem mobilen Telefon** in Notizen auf QOwnNote auf Ihrem Desktop einzufügen

## Aussehen

- **Markdown-Hervorhebung** von Notizen und ein **Markdown-Vorschaumodus**
- **Unterstützung für Schemata im Dunkelmodus**
- **Schemata-Unterstützung** für die Hervorhebung von **Markdown-Syntax**
- Alle **Schaltflächen können platziert werden, wo immer Sie möchten**, sie können sogar **schweben** oder **gestapelt werden** (vollständig dockbar)
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
