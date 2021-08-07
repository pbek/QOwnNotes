# Panoramica

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="Schermata di QOwnNotes" />
    <div class="sheet">
      Modifica le tue note con evidenziazione del markdown, tag colorati e sottocartelle
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="Vista minima" />
    <div class="sheet">
      Interfaccia utente predefinita minima che può essere rimossa ancora di più
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
      Modalità portatile per chiavette USB
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
      Modalità notte
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-distraction-free-mode.png" alt="schermata-modalità-senza-distrazioni" />
    <div class="sheet">
      Modalità senza distrazioni
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
      Mostra la differenza tra le note quando è stata modificata esternamente
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-export-print.png" alt="schermata-esporta-stampa" />
    <div class="sheet">
      Nota esportazione e stampa in PDF
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
      Puoi avere diversi spazi di lavoro
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-qml.png" alt="schermata-qml" />
    <div class="sheet">
      Scriptable
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
- Nella maggior parte dei casi puoi usare i tuoi **file di testo o markdown esistenti**, senza bisogno di importarli
- QOwnNotes è scritto in C++ ed è ottimizzato a basso **consumo di risorse** (nessuna CPU e app Electron affamata di memoria)
- **Importa Evernote e Joplin**
- **Compatibile** con l'applicazione Notes di **Nextcloud / ownCloud** e **mobile** e Nextcloud Text
- Supporta la **condivisione di note** sul tuo server **ownCloud** / **Nextcloud**
- Gestisci i tuoi **elenchi di cose da fare** (**attività Nextcloud** o **Tasks Plus** / **Calendar Plus**) Nextcloud / ownCloud o usa un altro **Server CalDAV** su cui sincronizzare le tue attività
- Le **versioni** precedenti delle tue note possono essere **ripristinate** dal tuo server **Nextcloud / ownCloud**
- Le **note cancellate** possono essere **ripristinate** dal tuo server **Nextcloud / ownCloud**
- Le **modifiche esterne** ai file delle note sono **monitorate** (le note o l'elenco delle note sono ricaricate)
- Supporta l'**etichettatura gerarchica delle note** e **sottocartelle delle note**
- **Cifratura** opzionale delle note. È disponibile AES-256 oppure puoi usare metodi di cifratura personalizzabili come **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-keybase.qml)) o **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-pgp.qml))
- La **cartella note** può essere **scelta liberamente** (è possibile utilizzare più cartelle note)
- La **ricerca di sotto-stringhe** di note è possibile e i risultati della ricerca sono evidenziati nelle note
- [**Estensione del browser**](browser-extension.md) per **aggiungere note dal testo selezionato**, **catturare schermate** o **gestire i tuoi segnalibri**
- Supporta il [**il controllo ortografico**](../editor/spellchecking.md)
- **Modalità portatile** per portare in giro QOwnNotes su chiavette USB
- **Supporto per script** e un [**repository di script**](https://github.com/qownnotes/scripts) online in cui è possibile installare script all'interno dell'applicazione
- **Modalità Vim**
- [Applicazione web](web-app.md) per **inserire foto dal tuo telefono cellulare** nella nota corrente di QOwnNotes sul tuo desktop


## Aspetto
- **Evidenziazione del markdown** delle note e **modalità di anteprima del markdown**
- **Supporto per temi in modalità scura**
- **Supporto per temi** per l'evidenziazione della **sintassi del markdown**
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
