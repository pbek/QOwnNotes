# Visión de conjunto

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="QOwnNotes screenshot" />
    <div class="sheet">
      Edit your notes with markdown highlighting, colored tags and subfolders
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="Minimal view" />
    <div class="sheet">
      Minimal default user interface that can be stripped even more
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-vertical.png" alt="Vertical view" />
    <div class="sheet">
      Vea sus notas en una vista de rebaja vertical moviendo los paneles
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-portable-mode.png" alt="Modo portátil" />
    <div class="sheet">
      Modo portátil para memorias USB
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-1col.png" alt="Una columna" />
    <div class="sheet">
      Todos los paneles se pueden colocar donde quieras
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-darkmode.png" alt="screenshot darkmode" />
    <div class="sheet">
      Modo oscuro
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-distraction-free-mode.png" alt="screenshot-distraction-free-mode" />
    <div class="sheet">
      Modo sin distracciones
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note-decrypted.png" alt="Cifrado de notas" />
    <div class="sheet">
      Cifrado de notas AES opcional (también programable)
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-encrypted-note.png" alt="Nota cifrada" />
    <div class="sheet">
      Las notas cifradas siguen siendo texto
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-diff.png" alt="screenshot diff" />
    <div class="sheet">
      Muestre la diferencia entre notas cuando se cambió externamente
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-export-print.png" alt="screenshot-export-print" />
    <div class="sheet">
      Nota Exportación e impresión de PDF
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
      Puedes tener diferentes espacios de trabajo
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
      Traducido a muchos idiomas
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-all-notes.png" alt="screenshot-search-in-all-notes" />
    <div class="sheet">
      Buscar en todas las notas
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-search-in-current-note.png" alt="screenshot-search-in-current-note" />
    <div class="sheet">
      Buscar en la nota actual
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-settings-note-folders.png" alt="screenshot-settings-note-folders" />
    <div class="sheet">
      Capaz de usar múltiples carpetas de notas
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-todo.png" alt="screenshot-todo" />
    <div class="sheet">
      Administre sus listas de Todo a través de CalDAV
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-trash.png" alt="screenshot-trash" />
    <div class="sheet">
      Administre las notas enviadas a la papelera en su servidor Nextcloud
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-versioning.png" alt="screenshot-versioning" />
    <div class="sheet">
      Administre las versiones de sus notas en su servidor Nextcloud
    </div>
  </v-carousel-item>
</v-carousel>
</template>

<v-divider />

**Q** para el [marco Qt](https://www.qt.io/) que se utiliza  
**Posee** porque eres dueño de tus propias notas  
**Notas** para las notas que toma

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) es el archivo de texto sin formato de **código abierto** (GPL) **aplicación de toma de notas de rebajas** para **GNU / Linux**, **macOS** y **Windows**, que (opcionalmente) funciona junto con la [**aplicación de notas**](https://github.com/nextcloud/notes) de [Nextcloud](https://nextcloud.com/) y [ownCloud](https://owncloud.org/).

::: tip
Únase a nuestro [Telegram Group](https://t.me/QOwnNotes) para discutir, aprender y conectarse con la comunidad QOwnNotes.
:::

[Comprender el concepto de QOwnNotes](concept.md)

## Características
- Puede usar sus **archivos de texto o rebajas existentes**, sin necesidad de importar la mayoría de las veces
- QOwnNotes is written in C++ and optimized for **low resource consumption** (no CPU and memory-hungry Electron app)
- **Evernote import**
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
- **Markdown resaltando** de notas y un **modo de vista previa de Markdown**
- **Soporte de tema de modo oscuro**
- **Soporte de temas** para la sintaxis de rebajas **resaltando**
- Todos los paneles **pueden colocarse donde desee**, incluso pueden **flotar** o **apilar** (totalmente acoplables)
- Soporte para **iconos de tema de escritorio gratuito**, puede usar QOwnNotes con sus **iconos de escritorio nativos** y con su **tema de escritorio oscuro favorito**
- **Modo sin distracciones**, **Modo de pantalla completa** y **Modo de máquina de escribir**
- **Números de línea**
- **Las diferencias** entre la nota actual y la nota modificada externamente se **se muestran** en un diálogo

## Lenguajes
- **Disponible en más de 60 idiomas diferentes** como inglés, alemán, francés, polaco, chino, japonés, ruso, portugués, húngaro, holandés y español
  - [Su ayuda](../contributing/translation.md) es muy apreciada para mejorar estas traducciones o para traducir QOwnNotes en más idiomas.

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
