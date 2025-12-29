# Overzicht

<ScreenshotCarousel />

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
- **Differences** between current note and externally changed note are **shown** in a dialog

## Talen

- **Beschikbaar in meer dan 60 verschillende talen** zoals Engels, Duits, Frans, Pools, Chinees, Japans, Russisch, Portugees, Hongaars, Nederlands en Spaans
  - [Uw hulp](../contributing/translation.md) wordt zeer op prijs gesteld om deze vertalingen te verbeteren of om QOwnNotes in meer talen te vertalen

<style>
.v-divider {
  margin: 20px;
}

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
