# Visión de conjunto

<template>
<v-carousel cycle show-arrows-on-hover>
  <v-carousel-item>
    <img src="/screenshots/screenshot.png" alt="Captura de pantalla de QOwnNotes" />
    <div class="sheet">
      Edite sus notas con resaltado de rebajas, etiquetas de colores y subcarpetas
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-minimal.png" alt="Vista mínima" />
    <div class="sheet">
      Interfaz de usuario mínima predeterminada que se puede quitar aún más
    </div>
  </v-carousel-item>
  <v-carousel-item>
    <img src="/screenshots/screenshot-vertical.png" alt="Vista vertical" />
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
- QOwnNotes está escrito en C ++ y optimizado para **bajo consumo de recursos** (sin CPU y aplicación de Electron que consume mucha memoria)
- **Importación de Evernote y Joplin**
- **Compatible** con la aplicación Notes de **Nextcloud / ownCloud** y **mobil** y Nextcloud Text
- Soporte para **compartir notas** en su servidor **ownCloud** / **Nextcloud**
- Administre sus **listas de tareas** de Nextcloud / ownCloud (**tareas de Nextcloud** o **Tasks Plus** / **Calendar Plus**) o use otro **servidor CalDAV** para sincronizar sus tareas con
- Las **versiones anteriores** de sus notas se pueden **restaurar** desde su servidor **Nextcloud / ownCloud**
- **Las notas enviadas a la papelera** se pueden **restaurar** desde su servidor **Nextcloud / ownCloud**
- **Los cambios externos** de los archivos de notas son **supervisados** (las notas o la lista de notas se vuelven a cargar)
- Soporte para **etiquetado jerárquico de notas** y **subcarpetas de notas**
- **Cifrado** opcional de notas, (AES-256 está integrado o puede utilizar métodos de cifrado personalizados como **[ Keybase.io](https://keybase.io/)** ([encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-keybase.qml)) o **PGP** ([encriptación-pgp.qml](https://github.com/pbek/QOwnNotes/blob/develop/doc/scripting/encryption-pgp.qml))
- La **carpeta de notas** se puede **elegir libremente** (se pueden utilizar varias carpetas de notas)
- **Es posible buscar partes de palabras** en las notas y los resultados de la búsqueda se resaltan en las notas
- [**Extensión del navegador**](browser-extension.md) para **agregar notas del texto seleccionado**, **hacer capturas de pantalla** o **administrar sus marcadores**
- <0 Soporte de corrección ortográfica</strong></a>
- **Modo portátil** para llevar QOwnNotes en memorias USB
- **Soporte de secuencias de comandos** y un [**repositorio de secuencias de comandos**](https://github.com/qownnotes/scripts) en línea donde puede instalar secuencias de comandos dentro de la aplicación
- **Modo Vim**
- [Aplicación web](web-app.md) para **insertar fotos desde su teléfono móvil** en la nota actual en QOwnNotes en su escritorio


## Apariencia
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
