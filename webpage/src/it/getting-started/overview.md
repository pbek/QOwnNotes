# Panoramica

<ScreenshotCarousel />

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
- **Compatibilità** con l'applicazione [Notes](https://apps.nextcloud.com/apps/notes) di **Nextcloud / Owncloud**, la versione <0>mobile** e Nextcloud Text</p></li>
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
- **[Applicazione web](web-app.md)** per **inserire foto dal tuo telefono cellulare** nella nota corrente di QOwnNotes sul tuo desktop</ul>

## Aspetto

- **Evidenziazione del markdown** delle note e **modalità di anteprima del markdown**
- **Supporto per temi in modalità scura**
- **Supporto dei temi** per l'**evidenziazione della sintassi di Markdown**
- Tutti i **pannelli possono essere posizionati dove vuoi**, possono anche essere **separati** o essere **impilati** (completamente agganciabili)
- Supporto per **icone del tema freedesktop**, puoi usare QOwnNotes con le tue **icone desktop native** e con il tuo **tema desktop scuro** preferito
- **Modalità senza distrazioni**, **Modalità a schermo intero** e **Modalità macchina da scrivere**
- **Numeri di riga**
- **Differences** between current note and externally changed note are **shown** in a dialog

## Lingue

- **Disponibile in più di 60 lingue diverse** come inglese, tedesco, francese, polacco, cinese, giapponese, russo, portoghese, ungherese, olandese e spagnolo
  - [Il tuo aiuto](../contributing/translation.md) è molto apprezzato per migliorare queste traduzioni o per tradurre QOwnNotes in più lingue

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
