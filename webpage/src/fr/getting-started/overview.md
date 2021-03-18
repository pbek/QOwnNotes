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
- **Import Evernote**
- **Compatible** avec l'application Notes de **Nextcloud / ownCloud** et **mobile** et Nextcloud Text
- Prise en charge du **partage de notes** sur votre serveur **ownCloud** / **Nextcloud**
- Gérez vos **listes de tâches** Nextcloud / ownCloud  (**tâches Nextcloud** ou **tâches Plus** / **calendrier Plus**) ou utilisez un autre **Serveur CalDAV** pour synchroniser vos tâches avec
- Les ** versions** plus anciennes de vos notes peuvent être **restaurées** à partir de votre serveur **Nextcloud / ownCloud**
- **Les notes mises dans la corbeille** peuvent être **restaurées** depuis votre serveur **Nextcloud / ownCloud**
- **Les modifications externes** des fichiers de notes sont **surveillées** (les notes ou la liste de notes sont rechargées)
- Prise en charge du **balisage hiérarchique des notes** et **sous-dossiers de notes**
- **cryptage** facultatif des notes (AES-256 est intégré ou vous pouvez utiliser des méthodes de cryptage personnalisées telles que **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-keybase.qml)) ou **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-pgp.qml)))
- Le **dossier de notes** peut être **librement choisi**(plusieurs dossiers de notes peuvent être utilisés)
- **La recherche de sous-chaînes** de notes est possible et les résultats de la recherche sont mis en évidence dans les notes
- [Extension de navigateur](browser-extension.md) pour ajouter des notes à partir du texte sélectionné, faire des captures d'écran ou gérer vos signets
- [**Spellchecking**](../editor/spellchecking.md) soutien
- **Mode portable** pour transporter QOwnNotes sur des clés USB
- **Prise en charge des scripts** et un [**référentiel de scripts**](https://github.com/qownnotes/scripts) en ligne où vous pouvez installer des scripts à l'intérieur de l'application
- **Mode Vim**


## Appearance
- **Prise en charge du thème du mode sombre**
- **Prise en charge de la thématisation** pour la mise en évidence de la syntaxe de démarque****
- Tous les panneaux **peuvent être placés où vous voulez**, ils peuvent même **flotter** ou **empiler** (entièrement ancrable)
- Prise en charge des **icônes de thème freedesktop**, vous pouvez utiliser QOwnNotes avec vos **icônes de bureau natives** et avec votre **thème de bureau sombre préféré**
- **Distraction free mode**, **Full-screen mode** en **Typewriter mode**
- **Les différences** entre la note actuelle et la note modifiée de l'extérieur sont **affichées** dans une boîte de dialogue
- **Mise en évidence de Markdown** des notes et un **mode de prévisualisation de Markdown**

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
