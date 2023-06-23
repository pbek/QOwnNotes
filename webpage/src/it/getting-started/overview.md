# Panoramica

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="Schermata di QOwnNotes" />
    <div class="sheet">
      Modifica le tue note con evidenziazione markdown, etichette colorate e sottocartelle
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="Vista minima" />
    <div class="sheet">
      Interfaccia utente predefinita minima che può essere ulteriormente semplificata
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-vertical.png" alt="Vista verticale" />
    <div class="sheet">
      Visualizza le tue note in una vista markdown verticale spostando i pannelli
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-portable-mode.png" alt="Modalità portatile" />
    <div class="sheet">
      Modalità portabile per chiavette USB
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-1col.png" alt="Una colonna" />
    <div class="sheet">
      Tutti i pannelli possono essere posizionati dove vuoi
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-darkmode.png" alt="Schermata in modalità scura" />
    <div class="sheet">
      Modalità scura
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-distraction-free-mode.png" alt="schermata-modalità-senza-distrazioni" />
    <div class="sheet">
      Modalità anti-distrazione
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note-decrypted.png" alt="Cifratura nota" />
    <div class="sheet">
      Crittografia delle note AES opzionale (anche con script)
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note.png" alt="Nota crittografata" />
    <div class="sheet">
      Le note crittografate sono ancora testo
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-diff.png" alt="schermata diff" />
    <div class="sheet">
      Mostra la differenza tra le note quando subiscono modifiche esterne all'applicazione
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-export-print.png" alt="schermata-esporta-stampa" />
    <div class="sheet">
      Exportazione in PDF e stampa della nota
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-freedesktop-theme.png" alt="schermata-tema-freedesktop" />
    <div class="sheet">
      Icone tramite il tema Freedesktop
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-other-workspace.png" alt="schermata-altro-spazio" />
    <div class="sheet">
      Puoi avere diversi aree di lavoro
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-qml.png" alt="schermata-qml" />
    <div class="sheet">
      Gestisce gli script
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-russian.png" alt="schermata-russo" />
    <div class="sheet">
      Tradotto in molte lingue
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-all-notes.png" alt="schermata-cerca-in-tutte-le-note" />
    <div class="sheet">
      Cerca in tutte le note
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-current-note.png" alt="schermata-cerca-in-nota-corrente" />
    <div class="sheet">
      Cerca nella nota corrente
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-settings-note-folders.png" alt="schermata-impostazioni-cartelle-note" />
    <div class="sheet">
      In grado di utilizzare più cartelle di note
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-todo.png" alt="schermata-todo" />
    <div class="sheet">
      Gestisci le tue liste di cose da fare tramite CalDAV
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-trash.png" alt="schermata-cestino" />
    <div class="sheet">
      Gestisci le note cestinate sul tuo server Nextcloud
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-versioning.png" alt="schermata-versionamento" />
    <div class="sheet">
      Gestisci le versioni delle note sul tuo server Nextcloud
    </div>
  </v-carousel-item>
</v-carousel>
</template>

<v-divider />

**Q** per il [framework Qt](https://www.qt.io/) usato  
**Own** perché possiedi le tue note  
**Notes** per le note che prendi

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) è l'applicazione **open source** (GPL) file di testo semplice per **prendere note in markdown** per **GNU / Linux**, **macOS** e **Windows**, che (opzionalmente) funziona insieme all '[**applicazione note**](https://github.com/nextcloud/notes) di [Nextcloud](https://nextcloud.com/) e [ownCloud](https://owncloud.org/).

::: tip
Unisciti al nostro [Gruppo Telegram](https://t.me/QOwnNotes) per discutere, imparare e entrare in contatto con la comunità QOwnNotes.
:::

[Comprendere il concetto di QOwnNotes](concept.md)

## Caratteristiche
- You can use your **existing text or markdown files**, no need for an import
- QOwnNotes è scritto in C++ ed è ottimizzato a basso **consumo di risorse** (nessuna CPU e app Electron affamata di memoria)
- **Importa Evernote e Joplin**
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


## Aspetto
- **Evidenziazione del markdown** delle note e **modalità di anteprima del markdown**
- **Supporto per temi in modalità scura**
- **Theming support** for the **Markdown syntax highlighting**
- Tutti i **pannelli possono essere posizionati dove vuoi**, possono anche essere **separati** o essere **impilati** (completamente agganciabili)
- Supporto per **icone del tema freedesktop**, puoi usare QOwnNotes con le tue **icone desktop native** e con il tuo **tema desktop scuro** preferito
- **Modalità senza distrazioni**, **Modalità a schermo intero** e **Modalità macchina da scrivere**
- **Numeri di riga**
- **Le differenze** tra la nota corrente e la nota modificata esternamente sono **mostrate** in una finestra di dialogo

## Lingue
- **Disponibile in più di 60 lingue diverse** come inglese, tedesco, francese, polacco, cinese, giapponese, russo, portoghese, ungherese, olandese e spagnolo
  - [Il tuo aiuto](../contributing/translation.md) è molto apprezzato per migliorare queste traduzioni o per tradurre QOwnNotes in più lingue

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
