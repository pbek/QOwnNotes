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
    <img src="/screenshots/screenshot-note-relations.png" alt="Note relations" />
    <div class="sheet">
      Note relations panel
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
Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community.
:::

[Comprendere il concetto di QOwnNotes](concept.md)

## Caratteristiche

- Puoi usare i tuoi **pre-esistenti documenti di testo o markdown** senza la necessità di importarli
- QOwnNotes è scritto in C++ ed è ottimizzato a basso **consumo di risorse** (nessuna CPU e app Electron affamata di memoria)
- **Importa Evernote e Joplin**
- **Compatibilità** con l'applicazione [Notes](https://apps.nextcloud.com/apps/notes) di **Nextcloud / Owncloud**, la versione **mobile** e Nextcloud Text
- Crea un **[Nextcloud Deck](https://apps.nextcloud.com/apps/deck) cards** e collegalo alle tue note
- Supporto per la **condivisione delle note** sul tuo server **ownCLoud** / **Nextcloud**
- Gestisci la tua **lista delle cose da fare** di Nextcloud / OwnCloud (**Nextcloud tasks** o **Tasks Plus** / **Calendar Plus**) o usa un altro **server CalDAV** per sincronizzare i tuoi incarichi
- Le **note cancellate** possono essere **ripristinate** dal tuo server **Nextcloud / ownCloud**
- ** Le note cestinate** possono essere **ripristrinate** dal tuo server **Nextcloud / ownCloud**
- Le ** modifiche esterne** ai files delle note vengono **prese in considerazione** (le note o l'elenco di esse vengono aggiornati)
- Supporto per la **gerarchia dell'etichettatura alle note** e alle **sottocartelle delle note**
- **Cifratura** opzionale delle note. Sistema AES-256 preinstallato, altrimenti puoi usare il tuo metodo di cifratura come **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) o **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml))
- La **cartella delle note** può essere **scelta liberamente** (è possibile scegliere più cartelle delle note)
- **La ricerca di parole parziali** nelle note è possibile e i risultati di essa sono evidenziati nelle note
- [**Estensione browser**](browser-extension.md) per **inserire note da testo selezionato**, **fare catture di schermata** o **gestire i tuoi segnalibri**
- [**Controllo ortografico**](../editor/spellchecking.md) supportato
- **Modalità portatile** per portare QOwnNotes con te su una chiavetta USB
- **Supporto allo scripting** e un [**repository di script**](https://github.com/qownnotes/scripts) online da cui puoi installare script nella tua applicazione
- Implementazione di **[API per l'autocompilazione di OpenAI](https://www.qownnotes.org/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html)** per usarla negli scripts
- **Modalità Vim**
- **[Applicazione web](web-app.md)** per **inserire foto dal tuo telefono cellulare** nella nota corrente di QOwnNotes sul tuo desktop

## Aspetto

- **Evidenziazione del markdown** delle note e **modalità di anteprima del markdown**
- **Supporto per temi in modalità scura**
- **Supporto dei temi** per l'**evidenziazione della sintassi di Markdown**
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
