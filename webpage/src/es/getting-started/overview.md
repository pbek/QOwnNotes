# Descripción general

<ScreenshotCarousel />

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
- **Differences** between current note and externally changed note are **shown** in a dialog

## Idioma

- **Disponible en más de 60 idiomas diferentes** como inglés, alemán, francés, polaco, chino, japonés, ruso, portugués, húngaro, holandés y español
  - [Su ayuda](../contributing/translation.md) es muy apreciada para mejorar estas traducciones o para traducir QOwnNotes en más idiomas.

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
