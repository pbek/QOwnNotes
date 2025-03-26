# Descripción general

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="Captura de pantalla de QOwnNotes" />
    <div class="sheet">
      Edite sus notas con resaltado de Markdown, etiquetas de colores y subcarpetas
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="Vista mínima" />
    <div class="sheet">
      Interfaz de usuario mínima predeterminada que se puede simplificar aún más
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-vertical.png" alt="Vista vertical" />
    <div class="sheet">
      Vea sus notas en una vista vertical de Markdown moviendo los paneles
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-note-relations.png" alt="Relaciones de la nota" />
    <div class="sheet">
      Panel de relaciones de notas
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
      Todos los paneles pueden ser colocados donde usted desee
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-darkmode.png" alt="captura de pantalla modo oscuro" />
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
      Iconos a través del tema Freedesktop
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
      Administre sus listas de tareas pendientes a través de CalDAV
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-trash.png" alt="screenshot-trash" />
    <div class="sheet">
      Administre las notas enviadas a la papelera en su servidor de Nextcloud
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-versioning.png" alt="screenshot-versioning" />
    <div class="sheet">
      Administre las versiones de sus notas en su servidor de Nextcloud
    </div>
  </v-carousel-item>
</v-carousel>
</template>

<v-divider />

**Q** es por el [framework Qt](https://www.qt.io/) que se utiliza  
**Own** es debido a que es dueño de sus propias notas  
**Notes** es por las notas que redacta

<v-divider />

[QOwnNotes](https://www.qownnotes.org/) es la aplicación de **bloc de notas en formato Markdown** en archivos de texto plano de **código abierto** (GPL) para **GNU/Linux**, **macOS** y **Windows**, que (opcionalmente) funciona junto a las [**aplicaciones de notas**](https://github.com/nextcloud/notes) de [Nextcloud](https://nextcloud.com/) y [ownCloud](https://owncloud.org/).

::: tip
Únase a nuestro [grupo de Telegram](https://t.me/QOwnNotes) para discutir, aprender y conectarse con la comunidad de QOwnNotes.
:::

[Comprender el concepto de QOwnNotes](concept.md)

## Características

- Puede usar sus **archivos de texto o markdown existentes**, sin necesidad de importarlos
- QOwnNotes está escrito en C ++ y optimizado para **bajo consumo de recursos** (sin CPU y aplicación de Electron que consume mucha memoria)
- **Importación de Evernote y Joplin**
- **Compatible** con la aplicación de [notas](https://apps.nextcloud.com/apps/notes) de **Nextcloud / ownCloud**, las versiones **móviles** y Nextcloud Text
- Cree **tarjetas de [Nextcloud Deck](https://apps.nextcloud.com/apps/deck)** y enlácelas a sus notas
- Soporte para **compartir notas** en su servidor de **ownCloud** / **Nextcloud**
- Administre sus **listas de elementos pendientes** de Nextcloud / ownCloud (**Nextcloud tasks** o **Tasks Plus** / **Calendar Plus**) o use otro servidor de **CalDAV** con el cual sincronizar sus tareas
- Las versiones **antiguas** de sus notas pueden ser **restauradas** desde su servidor de **Nextcloud / ownCloud**
- **Las notas en la papelera** pueden ser **restauradas** desde su servidor de **Nextcloud / ownCloud**
- **Los cambios externos** de los archivos de notas son **supervisados** (las notas o la lista de notas se vuelven a cargar)
- Soporte para **etiquetado jerárquico de notas** y **subcarpetas de notas**
- **Cifrado** opcional de notas, AES-256 está integrado, o puede utilizar métodos de cifrado personalizados como **[Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)) o **PGP** ([encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml))
- La **carpeta de notas** se puede **elegir libremente** (se pueden utilizar múltiples carpetas de notas)
- Es posible **buscar partes de palabras** en las notas. Los resultados de la búsqueda se resaltan en las notas
- [**Extensión del navegador**](browser-extension.md) para **agregar notas a partir del texto seleccionado**, **hacer capturas de pantalla** o **administrar sus marcadores**
- Compatibilidad con [**revisores ortográficos**](../editor/spellchecking.md)
- **Modo portátil** para llevar consigo QOwnNotes en memorias USB
- **Soporte de secuencias de órdenes** y un [**repositorio de scripts**](https://github.com/qownnotes/scripts) en línea desde el que puede instalar scripts dentro de la aplicación
- Implementación de la **[API de finalización de OpenAI](https://www.qownnotes.org/blog/2024-05-17-AI-support-was-added-to-QOwnNotes.html)** para su uso en scripts
- **Modo Vim**
- **[Aplicación web](web-app.md)** para **insertar fotos desde su teléfono móvil** en la nota actual en QOwnNotes en su escritorio

## Apariencia

- **Resaltado de Markdown** de notas y un **modo de vista previa de Markdown**
- **Soporte de tema de modo oscuro**
- **Soporte de temas** para el **resaltador de sintáxis de Markdown**
- Todos los paneles **pueden colocarse donde desee**, incluso pueden **flotar** o **apilar** (totalmente acoplables)
- Soporte para **iconos de tema de escritorio gratuito**, puede usar QOwnNotes con sus **iconos de escritorio nativos** y con su **tema de escritorio oscuro favorito**
- **Modo sin distracciones**, **Modo de pantalla completa** y **Modo de máquina de escribir**
- **Números de línea**
- **Las diferencias** entre la nota actual y la nota modificada externamente se **se muestran** en un diálogo

## Idioma

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
