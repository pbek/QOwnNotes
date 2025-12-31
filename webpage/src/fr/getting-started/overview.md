# Aperçu

<ScreenshotCarousel />

<v-divider />

**Q** pour le [framework Qt](https://www.qt.io/) utilisé   
**Own** parce que vous possédez ("to own") vos propres notes   
**Notes** pour les notes que vous prenez

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) est une application **open-source** (GPL) de **prise de notes en MarkDown** dans des fichiers en texte brut pour **GNU / Linux**, **MacOS** et **Windows**, qui peut fonctionner (en option) avec [**l'application Notes**](https://github.com/nextcloud/notes) de [Nextcloud](https://nextcloud.com/) et [ownCloud](https://owncloud.org/).

::: tip
Join our [Telegram Group](https://t.me/QOwnNotes) to discuss, learn, and connect with the QOwnNotes community.
:::

[Comprendre le concept de QOwnNotes](concept.md)

## Caractéristiques

- Vous pouvez utiliser vos **fichiers texte ou Markdown existants**, pas besoin de les importer
- QOwnNotes est écrit en C++ et optimisé pour une **faible consommation des ressources** (ce n'est pas une app Electron gourmande en CPU et en mémoire)
- **Import depuis Evernote et Joplin**
- **Compatible** with the [Notes](https://apps.nextcloud.com/apps/notes) application of **Nextcloud / ownCloud** and **mobile** and Nextcloud Text
- Create **[Nextcloud Deck](https://apps.nextcloud.com/apps/deck) cards** and link to them in your notes
- Support for **sharing notes** on your **ownCloud** / **Nextcloud** server
- Manage your Nextcloud / ownCloud **todo lists** (**Nextcloud tasks** or **Tasks Plus** / **Calendar Plus**) or use another **CalDAV server** to sync your tasks to
- Older **versions** of your notes can be **restored** from your **Nextcloud / ownCloud** server
- **Trashed notes** can be **restored** from your **Nextcloud / ownCloud** server
- **External changes** of note files are **watched** (notes or note list are reloaded)
- Support for **hierarchical note tagging** and **note subfolders**
- Optional **encryption** of notes, AES-256 is built in, or you can use custom encryption methods like **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) or **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml))
- The **notes folder** can be **freely chosen** (multiple note folders can be used)
- **Searching parts of words** in notes is possible and search results are highlighted in the notes
- [**Browser extension**](browser-extension.md) to **add notes from the selected text**, **make screenshots** or **manage your bookmarks**
- [**Spellchecking**](../editor/spellchecking.md) support
- **Portable mode** for carrying QOwnNotes around on USB sticks
- **Scripting support** and an online [**script repository**](https://github.com/qownnotes/scripts) where you can install scripts inside the application
- Implementation of the **[OpenAI completion API](https://www.qownnotes.org/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html)** to be used in scripts
- **Mode Vim**
- **[Web application](web-app.md)** to **insert photos from your mobile phone** into the current note in QOwnNotes on your desktop

## Apparence de l'interface

- **Mise en évidence du Markdown** des notes et un **mode de prévisualisation de Markdown**
- **Prise en charge du thème du mode sombre**
- **Theming support** for the **Markdown syntax highlighting**
- Tous les panneaux **peuvent être placés où vous voulez**, ils peuvent même **flotter** ou **s'empiler** (entièrement ancrable)
- Prise en charge des **icônes du thème freedesktop**, vous pouvez utiliser QOwnNotes avec vos **icônes de bureau natives** et avec votre **thème de bureau sombre préféré**
- **Mode sans distractions**, **mode plein écran** et **mode machine à écrire**
- **Numéros de lignes**
- **Differences** between current note and externally changed note are **shown** in a dialog

## Langues

- **Disponible dans plus de 60 langues différentes** comme l'anglais, l'allemand, le français, le polonais, le chinois, le japonais, le russe, le portugais, le hongrois, le néerlandais et l'espagnol
  - [Votre aide](../contributing/translation.md) est très appréciée pour améliorer ces traductions ou pour traduire QOwnNotes dans plus de langues

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
