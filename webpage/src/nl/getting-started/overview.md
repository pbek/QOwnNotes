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
    <img src="/screenshots/screenshot-note-relations.png" alt="Notitierelaties" />
    <div class="sheet">
      Paneel Notitierelaties
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
Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community.
:::

[Begrijp het concept van QOwnNotes](concept.md)

## Kenmerken

- U kunt uw **bestaande tekst- of markdown-bestanden** gebruiken. U hoeft niets te importeren
- QOwnNotes is geschreven in C++ en geoptimaliseerd voor **laag verbruik van bronnen** (geen CPU en geheugenverslindende Electron-app)
- **Evernote en Joplin importeren**
- **Compatibel** met de [Notities](https://apps.nextcloud.com/apps/notes) applicatie van **Nextcloud / ownCloud** en **mobile** en Nextcloud Text
- Maak **[Nextcloud Deck](https://apps.nextcloud.com/apps/deck) kaarten aan** en koppel ernaar in je notities
- Ondersteuning voor **Delen van notities** op uw **OwnCloud** / **Nextcloud** server
- Beheer uw Nextcloud / ownCloud **todolijsten** (**Nextcloud-taken ** of **Tasks Plus** / **Calendar Plus**) of gebruik een andere **CalDAV-server** om uw taken te synchroniseren
- Verouderde **versies** van uw notities kunnen **hersteld** worden vanaf uw **Nextcloud / ownCloud **server
- **Gewiste notities** kunnen worden **hersteld** vanaf uw **Nextcloud / ownCloud** server
- **Externe wijzigingen** van notitiebestanden worden **bewaakt** (notities of notitielijsten worden opnieuw geladen)
- Ondersteuning voor **hiërarchische labeling van notities** en **notitie-submappen**
- Optionele **versleuteling** van notities. AES-256 is ingebouwd, of u kunt aangepaste coderingsmethoden gebruiken zoals **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) of **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml))
- De **Notites-map** is **vrij te kiezen** (meerdere mappen met notities zijn ook mogelijk)
- **Het zoeken naar delen van woorden** in notities is mogelijk en zoekresultaten worden gemarkeerd in de notities
- [**Browser-extensie**](browser-extension.md) om **notities toe te voegen uit geselecteerde tekst**, **maak schermopnamen** of **beheer uw bladwijzers**
- Ondersteuning voor [**Spellingcontrole**](../editor/spellchecking.md)
- **Portable modus** om QOwnNotes mee te nemen op USB-sticks
- Ondersteuning voor **Scripting** en een online [**scriptrepository**](https://github.com/qownnotes/scripts) waar u scripts in de applicatie kunt installeren
- Implementatie van de **[OpenAI completion API](https://www.qownnotes.org/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html)** voor het gebruik in scripts
- **Vim-modus**
- **[Webapplicatie](web-app.md)** om **foto's van uw mobiele telefoon toe te voegen** in de huidige notitie in QOwnNotes op uw PC/laptop

## Uiterlijk

- **Markdown-markering** van notities en een **markdown-voorbeeldmodus**
- **Thema-ondersteuning voor donkere modus**
- Ondersteuning voor **Thema's** bij de **Markdown syntaxisaccentuering**
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
