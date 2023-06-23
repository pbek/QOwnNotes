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

**Q** voor het [Qt framework](https://www.qt.io/) dat wordt gebruikt  
**Own** omdat je je eigen notities   
bezit **Notes** voor de notities die u maakt

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) is het **open source** (GPL) platte tekstbestand **markdown notitie**-applicatie **voor GNU / Linux**, **MacOS** en **Windows**, die (optioneel) samenwerkt met de [**notitietoepassing**](https://github.com/nextcloud/notes) van [Nextcloud](https://nextcloud.com/) en [ownCloud](https://owncloud.org/) (opent nieuw venster).

::: tip
Word lid van onze [Telegram Group](https://t.me/QOwnNotes) om te discussiëren, te leren en contact te maken met de QOwnNotes-gemeenschap.
:::

[Begrijp het concept van QOwnNotes](concept.md)

## Kenmerken
- You can use your **existing text or markdown files**, no need for an import
- QOwnNotes is geschreven in C++ en geoptimaliseerd voor **laag verbruik van bronnen** (geen CPU en geheugenverslindende Electron-app)
- **Evernote en Joplin importeren**
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
- **Vim mode**
- **[Web application](web-app.md)** to **insert photos from your mobile phone** into the current note in QOwnNotes on your desktop


## Uiterlijk
- **Markdown-markering** van notities en een **markdown-voorbeeldmodus**
- **Thema-ondersteuning voor donkere modus**
- **Theming support** for the **Markdown syntax highlighting**
- Alle **panelen kunnen worden geplaatst waar u maar wilt**, ze kunnen zelfs **zweven** of **stapelen** (volledig dockbaar)
- Ondersteuning voor **freedesktop-themapictogrammen**, u kunt QOwnNotes gebruiken met uw **native bureaubladpictogrammen** en met uw favoriete **donkere bureaubladthema**
- **Distraction free mode**, **Full-screen mode** en **Typewriter mode**
- **Regelnummers**
- **Verschillen** tussen huidige notitie en extern gewijzigde notitie worden **getoond** in een dialoogvenster

## Talen
- **Beschikbaar in meer dan 60 verschillende talen** zoals Engels, Duits, Frans, Pools, Chinees, Japans, Russisch, Portugees, Hongaars, Nederlands en Spaans
  - [Uw hulp](../contributing/translation.md) wordt zeer op prijs gesteld om deze vertalingen te verbeteren of om QOwnNotes in meer talen te vertalen

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
