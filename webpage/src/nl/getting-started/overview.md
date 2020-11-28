# Overzicht

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="Schermafbeelding van QOwnNotes" />
    <div class="sheet">
      Bewerk uw notities met markdown-markering, gekleurde tags en submappen
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="Minimaal zicht" />
    <div class="sheet">
      Minimale standaard gebruikersinterface die nog meer kan worden verwijderd
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-vertical.png" alt="Verticale weergave" />
    <div class="sheet">
      Bekijk uw notities in een verticale markdown-weergave door de panelen te verplaatsen
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-portable-mode.png" alt="Draagbare modus" />
    <div class="sheet">
      Draagbare modus voor USB-sticks
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-1col.png" alt="Een kolom" />
    <div class="sheet">
      Alle panelen kunnen worden geplaatst waar u maar wilt
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-darkmode.png" alt="screenshot darkmode" />
    <div class="sheet">
      Donkere modus
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-distraction-free-mode.png" alt="screenshot-distraction-free-mode" />
    <div class="sheet">
      Afleidingsvrije modus
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note-decrypted.png" alt="Let op versleuteling" />
    <div class="sheet">
      Optionele AES-notitie-encryptie (ook scriptbaar)
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note.png" alt="Versleutelde notitie" />
    <div class="sheet">
      Versleutelde notities zijn nog steeds tekst
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-diff.png" alt="screenshot diff" />
    <div class="sheet">
      Laat het verschil zien tussen notities wanneer deze extern is gewijzigd
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-export-print.png" alt="screenshot-export-print" />
    <div class="sheet">
      Let op PDF-export en afdrukken
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-freedesktop-theme.png" alt="screenshot-freedesktop-theme" />
    <div class="sheet">
      Pictogrammen via Freedesktop-thema
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-other-workspace.png" alt="screenshot-other-workspace" />
    <div class="sheet">
      U kunt verschillende werkruimten hebben
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-qml.png" alt="screenshot-qml" />
    <div class="sheet">
      Scriptbaar
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-russian.png" alt="screenshot-russian" />
    <div class="sheet">
      Vertaald naar vele talen
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-all-notes.png" alt="screenshot-search-in-all-notes" />
    <div class="sheet">
      Doorzoek alle notities
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-current-note.png" alt="screenshot-search-in-current-note" />
    <div class="sheet">
      Zoek in de huidige notitie
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-settings-note-folders.png" alt="screenshot-settings-note-folders" />
    <div class="sheet">
      In staat om meerdere notitiemappen te gebruiken
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-todo.png" alt="screenshot-todo" />
    <div class="sheet">
      Beheer uw Todo-lijsten via CalDAV
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-trash.png" alt="screenshot-trash" />
    <div class="sheet">
      Beheer verwijderde notities op uw Nextcloud-server
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-versioning.png" alt="screenshot-versioning" />
    <div class="sheet">
      Beheer uw notitieversies op uw Nextcloud-server
    </div>
  </v-carousel-item>
</v-carousel>
</template>

<v-divider />

**Q** for the [Qt framework](https://www.qt.io/) that is used  
**Own** because you own your own notes  
**Notes** for the notes you take

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) is the **open source** (GPL) plain-text file **markdown note taking** application for **GNU/Linux**, **Mac OS X** and **Windows**, that (optionally) works together with the [**notes application**](https://github.com/nextcloud/notes) of [Nextcloud](https://nextcloud.com/) and [ownCloud](https://owncloud.org/).

::: tip
Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community.
:::

[Begrijp het concept van QOwnNotes](concept.md)

## Kenmerken

- The **notes folder** can be **freely chosen** (multiple note folders can be used)
- You can use your **existing text or markdown files**, no need for an import most of the times
- **Sub-string searching** of notes is possible and search results are highlighted in the notes
- **Keyboard shortcuts** are customizable
- **Scripting support** and an online [**script repository**](https://github.com/qownnotes/scripts) where you can install scripts inside the application
- [Browser extension](browser-extension.md) to add notes from the selected text, make screenshots or manage your bookmarks
- **External changes** of note files are **watched** (notes or note list are reloaded)
- Older **versions** of your notes can be **restored** from your **Nextcloud / ownCloud** server
- **Notities naar prullenbak** kunnen worden **hersteld** vanaf uw **Nextcloud / ownCloud** -server
- **Differences** tussen huidige notitie en extern gewijzigde notitie worden **showed** in een dialoogvenster
- **Markdown highlighting** of notes and a **markdown preview mode**
- [**Spellchecking**](../editor/spellchecking.md) support
- **Compatible** with the Notes application of **Nextcloud / ownCloud** and **mobile** and Nextcloud Text
- Beheer je Nextcloud / ownCloud **takenlijsten** (**Nextcloud-taken** of **Tasks Plus** / **Calendar Plus**) of gebruik een andere **CalDAV-server** om uw taken mee te synchroniseren
- Optional **encryption** of notes (AES-256 is built in or you can use custom encryption methods like **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-keybase.qml)) or **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-pgp.qml)))
- **Thema-ondersteuning voor donkere modus**
- **Theming support**  voor de **markdown syntaxisaccentuering **
- Alle **panelen kunnen worden geplaatst waar u maar wilt**, ze kunnen zelfs **zweven** of **stapelen** (volledig dockbaar)
- Ondersteuning voor **freedesktop-themapictogrammen**, u kunt QOwnNotes gebruiken met uw **native bureaubladpictogrammen** en met uw favoriete **donkere bureaubladthema**
- Ondersteuning voor **hiërarchische tagging van notities** en **submappen van notities**
- Ondersteuning voor **het delen van notities** op uw **ownCloud** / **Nextcloud** -server
- **Portable mode** om QOwnNotes mee te nemen op USB-sticks
- **Vim mode**
- **Distraction free mode**, **Full-screen mode** and **Typewriter mode**
- **Evernote import**
- ** Beschikbaar in veel verschillende talen ** zoals Engels, Duits, Frans, Pools, Chinees, Japans, Russisch, Portugees, Hongaars, Nederlands en Spaans
  - [Your help](../contributing/translation.md) wordt zeer op prijs gesteld om deze vertalingen te verbeteren of om QOwnNotes in meer talen te vertalen

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
