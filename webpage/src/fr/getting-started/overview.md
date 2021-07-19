# Aperçu

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="Capture d'écran de QOwnNotes" />
    <div class="sheet">
      Modifiez vos notes avec la mise en évidence des démarques, les balises colorées et les sous-dossiers
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="Vue minimale" />
    <div class="sheet">
      Interface utilisateur par défaut minimale qui peut être supprimée encore plus
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-vertical.png" alt="Vue verticale" />
    <div class="sheet">
      Affichez vos notes dans une vue de démarque verticale en déplaçant les panneaux
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-portable-mode.png" alt="Mode portable" />
    <div class="sheet">
      Mode portable pour clés USB
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-1col.png" alt="Une colonne" />
    <div class="sheet">
      Tous les panneaux peuvent être placés où vous le souhaitez
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-darkmode.png" alt="screenshot darkmode" />
    <div class="sheet">
      Mode sombre
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-distraction-free-mode.png" alt="screenshot-distraction-free-mode" />
    <div class="sheet">
      Mode sans distractions
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note-decrypted.png" alt="Remarque cryptage" />
    <div class="sheet">
      Chiffrement facultatif des notes AES (également scriptable)
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note.png" alt="Note cryptée" />
    <div class="sheet">
      Les notes chiffrées sont toujours du texte
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-diff.png" alt="screenshot diff" />
    <div class="sheet">
      Afficher la différence entre les notes lorsqu'elles ont été modifiées en externe
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-export-print.png" alt="screenshot-export-print" />
    <div class="sheet">
      Note PDF export and printing
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
      Vous pouvez avoir différents espaces de travail
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-qml.png" alt="screenshot-qml" />
    <div class="sheet">
      Scriptable
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-russian.png" alt="screenshot-russian" />
    <div class="sheet">
      Traduit dans de nombreuses langues
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-all-notes.png" alt="screenshot-search-in-all-notes" />
    <div class="sheet">
      Rechercher dans toutes les notes
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-current-note.png" alt="screenshot-search-in-current-note" />
    <div class="sheet">
      Rechercher dans la note actuelle
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-settings-note-folders.png" alt="screenshot-settings-note-folders" />
    <div class="sheet">
      Capable d'utiliser plusieurs dossiers de notes
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-todo.png" alt="screenshot-todo" />
    <div class="sheet">
      Gérez vos listes Todo via CalDAV
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-trash.png" alt="screenshot-trash" />
    <div class="sheet">
      Gérez les notes mises à la corbeille sur votre serveur Nextcloud
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-versioning.png" alt="screenshot-versioning" />
    <div class="sheet">
      Gérez vos versions de notes sur votre serveur Nextcloud
    </div>
  </v-carousel-item>
</v-carousel>
</template>

<v-divider />

**Q** pour le [framework Qt](https://www.qt.io/) utilisé   
**Posséder** parce que vous possédez vos propres notes   
**Notes** pour les notes que vous prenez

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) est l’application **open source** (GPL) en texte brut **prise de notes de démarquage** pour **GNU / Linux**, **MacOS** et **Windows**, qui fonctionnent (en option) avec [**l'application notes**](https://github.com/nextcloud/notes) de [Nextcloud](https://nextcloud.com/) et [ownCloud](https://owncloud.org/).

::: tip
Rejoignez notre [Groupe Telegram](https://t.me/QOwnNotes) pour discuter, apprendre et vous connecter avec la communauté QOwnNotes.
:::

[Comprendre le concept de QOwnNotes](concept.md)

## Traits
- Vous pouvez utiliser vos **fichiers texte ou markdown existants**, pas besoin d'importer la plupart du temps
- QOwnNotes is written in C++ and optimized for **low resource consumption** (no CPU and memory-hungry Electron app)
- **Evernote and Joplin import**
- **Compatible** with the Notes application of **Nextcloud / ownCloud** and **mobile** and Nextcloud Text
- Support for **sharing notes** on your **ownCloud** / **Nextcloud** server
- Manage your Nextcloud / ownCloud **todo lists** (**Nextcloud tasks** or **Tasks Plus** / **Calendar Plus**) or use another **CalDAV server** to sync your tasks to
- Older **versions** of your notes can be **restored** from your **Nextcloud / ownCloud** server
- **Trashed notes** can be **restored** from your **Nextcloud / ownCloud** server
- **External changes** of note files are **watched** (notes or note list are reloaded)
- Support for **hierarchical note tagging** and **note subfolders**
- Optional **encryption** of notes, AES-256 is built in or you can use custom encryption methods like **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-keybase.qml)) or **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-pgp.qml))
- The **notes folder** can be **freely chosen** (multiple note folders can be used)
- **Sub-string searching** of notes is possible and search results are highlighted in the notes
- [**Browser extension**](browser-extension.md) to **add notes from the selected text**, **make screenshots** or **manage your bookmarks**
- [**Spellchecking**](../editor/spellchecking.md) support
- **Portable mode** for carrying QOwnNotes around on USB sticks
- **Scripting support** and an online [**script repository**](https://github.com/qownnotes/scripts) where you can install scripts inside the application
- **Vim mode**
- [Web application](web-app.md) to **insert photos from your mobile phone** into the current note in QOwnNotes on your desktop


## Appearance
- **Mise en évidence de Markdown** des notes et un **mode de prévisualisation de Markdown**
- **Prise en charge du thème du mode sombre**
- **Prise en charge de la thématisation** pour la mise en évidence de la syntaxe de démarque****
- Tous les panneaux **peuvent être placés où vous voulez**, ils peuvent même **flotter** ou **empiler** (entièrement ancrable)
- Prise en charge des **icônes de thème freedesktop**, vous pouvez utiliser QOwnNotes avec vos **icônes de bureau natives** et avec votre **thème de bureau sombre préféré**
- **Distraction free mode**, **Full-screen mode** en **Typewriter mode**
- **Nombre de lignes**
- **Les différences** entre la note actuelle et la note modifiée de l'extérieur sont **affichées** dans une boîte de dialogue

## Langues
- **Disponible dans plus de 60 langues différentes** comme l'anglais, l'allemand, le français, le polonais, le chinois, le japonais, le russe, le portugais, le hongrois, le néerlandais et l'espagnol
  - [Votre aide](../contributing/translation.md) est très appréciée pour améliorer ces traductions ou pour traduire QOwnNotes dans plus de langues

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
