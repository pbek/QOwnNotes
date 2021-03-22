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
      Icons via Freedesktop theme
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
- Sie können Ihre **vorhandenen Text- oder Markdown-Dateien** verwenden, ohne dass ein Import erforderlich ist
- **Evernote-Import**
- **Kompatibel** mit der Notes-Anwendung von **Nextcloud / ownCloud** und **mobile** und Nextcloud Text
- Unterstützung für **das Teilen von Notizen** auf Ihrem **ownCloud** / **Nextcloud** -Server
- Verwalten Sie Ihre Nextcloud / ownCloud **Aufgabenlisten** (**Nextcloud-Aufgaben** oder **Tasks Plus** / **Calendar Plus**) oder verwenden Sie einen anderen **CalDAV-Server**, mit dem Sie Ihre Aufgaben synchronisieren können
- Ältere **Versionen** Ihrer Notizen können von Ihrem **Nextcloud / ownCloud **Server** wiederhergestellt werden**
- **Gelöschten Notizen** können von Ihrem **Nextcloud / ownCloud** Server **wiederhergestellt werden**
- **Externe Änderungen** von Notizdateien werden **überwacht** (Notizen oder Notizliste werden neu geladen)
- Unterstützung für **hierarchische Notizkennzeichnung** und **Notizunterordner**
- Optionale **Verschlüsselung** von Notizen (AES-256 ist integriert oder Sie können benutzerdefinierte Verschlüsselungsmethoden wie **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-keybase.qml)) oder **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-pgp.qml)) verwenden.)
- Der **Notizenordner** kann **frei gewählt werden** (mehrere Notizordner können verwendet werden)
- **Die Suche nach Teilzeichenfolgen** von Notizen ist möglich, und die Suchergebnisse werden in den Notizen hervorgehoben
- [Browser-Erweiterung](browser-extension.md), um Notizen aus dem ausgewählten Text hinzuzufügen, Screenshots zu erstellen oder Ihre Lesezeichen zu verwalten
- Unterstützung für [**Rechtschreibprüfung**](../editor/spellchecking.md)
- **Tragbarer Modus** zum Mitnehmen von QOwnNotes auf USB-Sticks
- **Skriptunterstützung** und ein [**Online-Skript-Repository**](https://github.com/qownnotes/scripts), in dem Sie Skripte in der Anwendung installieren können
- **Vim-Modus**


## Aussehen
- **Unterstützung für Schemata im Dunkelmodus**
- **Schemata-Unterstützung** für die Hervorhebung der **Markdown-Syntax**
- Alle **Panels können platziert werden, wo immer Sie möchten**, sie können sogar **schweben** oder **gestapelt werden** (vollständig andockbar).
- Unterstützung für **Freedesktop-Schemasymbole**. Sie können QOwnNotes mit Ihren **originalen Desktop-Symbolen** und mit Ihrem bevorzugten **dunklen Desktop-Design** verwenden
- **Ablenkungsfreier Modus**, **Vollbildmodus** und **Schreibmaschinenmodus**
- **Unterschiede** zwischen aktueller Notiz und extern geänderter Notiz werden in einem Dialog **angezeigt**
- **Markdown-Hervorhebung** von Notizen und ein **Markdown-Vorschaumodus**

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
