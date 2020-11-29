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

[QOwnNotes](https://www.qownnotes.org/) is het **open source** (GPL) platte tekstbestand **markdown notitie**-applicatie **voor GNU / Linux**, **Mac OS X** en **Windows**, die (optioneel) samenwerkt met de [**notitietoepassing**](https://github.com/nextcloud/notes) van [Nextcloud](https://nextcloud.com/) en [ownCloud](https://owncloud.org/) (opent nieuw venster).

::: tip
Word lid van onze [Telegram Group](https://t.me/QOwnNotes) om te discussiëren, te leren en contact te maken met de QOwnNotes-gemeenschap.
:::

[Begrijp het concept van QOwnNotes](concept.md)

## Kenmerken

- De **notitiemap** kan **vrij gekozen worden** (meerdere notitiemappen kunnen worden gebruikt)
- U kunt uw **bestaande tekst- of markdown-bestanden** gebruiken, meestal is importeren niet nodig
- **Substring zoeken** van notities is mogelijk en zoekresultaten worden gemarkeerd in de notities
- **Sneltoetsen** kunnen worden aangepast
- **Ondersteuning voor scripts** en een online [**scriptrepository**](https://github.com/qownnotes/scripts) waar u scripts in de applicatie kunt installeren
- [Browserextensie](browser-extension.md) om notities van de geselecteerde tekst toe te voegen, screenshots te maken of uw bladwijzers te beheren
- **Externe wijzigingen** van notitiebestanden worden **bekeken** (notities of notitielijst worden opnieuw geladen)
- Oudere **versies** van uw notities kunnen **hersteld** worden vanaf uw **Nextcloud / ownCloud** server
- **Notities naar prullenbak** kunnen worden **hersteld** vanaf uw **Nextcloud / ownCloud** -server
- **Verschillen** tussen huidige notitie en extern gewijzigde notitie worden **getoond** in een dialoogvenster
- **Markdown-markering** van notities en een **markdown-voorbeeldmodus**
- [**Spellingcontrole**](../editor/spellchecking.md) ondersteuning
- **Compatibel** met de Notes-applicatie van **Nextcloud / ownCloud** en **mobiel** en Nextcloud Text
- Beheer je Nextcloud / ownCloud **takenlijsten** (**Nextcloud-taken** of **Tasks Plus** / **Calendar Plus**) of gebruik een andere **CalDAV-server** om uw taken mee te synchroniseren
- Optioneel **encryption** van opmerkingen (AES-256 is ingebouwd of u kunt aangepaste versleutelingsmethoden gebruiken zoals **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-keybase.qml)) of **PGP** ([encryptie-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-pgp.qml)))
- **Thema-ondersteuning voor donkere modus**
- **Theming support**  voor de **markdown syntaxisaccentuering **
- Alle **panelen kunnen worden geplaatst waar u maar wilt**, ze kunnen zelfs **zweven** of **stapelen** (volledig dockbaar)
- Ondersteuning voor **freedesktop-themapictogrammen**, u kunt QOwnNotes gebruiken met uw **native bureaubladpictogrammen** en met uw favoriete **donkere bureaubladthema**
- Ondersteuning voor **hiërarchische tagging van notities** en **submappen van notities**
- Ondersteuning voor **het delen van notities** op uw **ownCloud** / **Nextcloud** -server
- **Portable mode** om QOwnNotes mee te nemen op USB-sticks
- **Vim mode**
- **Distraction free mode**, **Full-screen mode** en **Typewriter mode**
- **Evernote import**
- **Beschikbaar in veel verschillende talen** zoals Engels, Duits, Frans, Pools, Chinees, Japans, Russisch, Portugees, Hongaars, Nederlands en Spaans
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
