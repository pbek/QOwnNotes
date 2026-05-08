# Concepto

<style>
  /* Remove max-width for content so there is enough space for the Mermaid diagram */
  /* This targets the VuePress content container on this page only using :has() */
  body:has(.concept-page-marker) main.vp-page > div {
    max-width: none !important;
  }
</style>

<div class="concept-page-marker" style="display: none;"></div>

```mermaid
graph TB
    subgraph Your computer
        qon((QOwnNotes))-->md{{"Markdown files"}}
        sync("Nextcloud Sync")-->md
        qon-comp("Browser extension")-->qon
        qc("Command-line snippet manager")-->qon
        homepage("Homepage dashboard")-->qon
    end
    subgraph Your Nextcloud server
        qon-api("QOwnNotesApi")-->ncs[("Nextcloud server")]
        nc-notes-app("Nextcloud Notes")-->ncs
        nc-deck-app("Nextcloud Deck")-->ncs
    end

    nc-notes-mob("Nextcloud Notes mobile app")-->nc-notes-app
    qon-web-app("QOwnNotes web application")-->qon
    qon-->qon-api
    qon-->ncs
    qon-->nc-deck-app
    sync-->ncs
    qon-.->lt("LanguageTool service")
    qon-.->harper("Harper service")
    qon-.->qon-web-api("api.qownnotes.org")
    qon-web-api-->github("GitHub")

    style qon fill:#d0d0ff,stroke:#333,stroke-width:4px
    click qon "/getting-started/concept.html#qownnotes" "QOwnNotes Desktop Application for managing your notes on your desktop computer"
    click md "/getting-started/concept.html#markdown-note-files" "Markdown, media and attachment files in your note folder"
    click qon-comp "/getting-started/concept.html#qownnotes-browser-extension" "QOwnNotes browser extension for managing bookmarks in markdown files and as web clipper"
    click qc "/getting-started/concept.html#qownnotes-command-line-snippet-manager" "QOwnNotes command-line snippet manager"
    click homepage "/getting-started/concept.html#homepage-dashboard" "Homepage dashboard using QOwnNotes bookmark suggestions"
    click sync "/getting-started/concept.html#nextcloud-desktop-sync-client" "Nextcloud desktop sync client to sync your notes to your server"
    click ncs "/getting-started/concept.html#nextcloud-server" "Nextcloud server to host your notes and other files"
    click qon-api "/getting-started/concept.html#qownnotesapi-nextcloud-app" "QOwnNotesAPI Nextcloud app to access your server-side trash and note versions"
    click nc-notes-app "/getting-started/concept.html#nextcloud-notes-server-app" "Nextcloud Notes server app to manage your notes in the web"
    click nc-notes-mob "/getting-started/concept.html#nextcloud-notes-mobile-app" "Nextcloud Notes mobile app to manage your notes on your mobile phone"
    click nc-deck-app "/getting-started/concept.html#nextcloud-deck-server-app" "Nextcloud Deck server app to manage reminders and todo lists in the web"
    click qon-web-app "/getting-started/concept.html#qownnotes-web-app" "QOwnNotes Web App to send photos from your mobile phone"
    click lt "/editor/languagetool.html" "Optional LanguageTool server for grammar and style checking"
    click harper "/editor/harper.html" "Optional Harper service for offline grammar and style checking"
    click qon-web-api "/getting-started/concept.html#api-qownnotes-org"
```

## QOwnNotes

- QOwnNotes **almacena notas** en su carpeta de notas **como archivos de Markdown**
- Puede hablar con su servidor Nextcloud / ownCloud **para compartir notas** públicamente con otras personas
- También puede **acceder a su historial de notas y a la papelera** en su servidor Nextcloud / ownCloud a través de la [aplicación QOwnNotesApi Nextcloud](#qownnotesapi-nextcloud-app)
- Se puede acceder a las listas de tareas en su servidor Nextcloud / ownCloud desde QOwnNotes
- **¡QOwnNotes no sincroniza sus notas** y archivos multimedia / adjuntos!
  - La sincronización de archivos es una tarea compleja, ya existen excelentes soluciones de sincronización de archivos (consulte [Cliente de sincronización de escritorio de Nextcloud](#nextcloud-desktop-sync-client))

## Archivos de notas de Markdown

- ¡Usted es **dueño** de todas sus notas y archivos multimedia/adjuntos!
- Sus notas se almacenan como **archivos Markdown de texto sin formato** en su computadora de escritorio
- Puede utilizar cualquier editor de texto que desee junto a QOwnNotes para ver o editar sus archivos de notas
- **Sincronice sus notas** con otros dispositivos (escritorio y móvil) con su cliente de sincronización [Nextcloud](https://nextcloud.com/) o [ownCloud](https://owncloud.org/) con su servidor

## Extensión del navegador QOwnNotes

Puede administrar los **marcadores de su navegador** con QOwnNotes o usarlo como **web clipper**.

El mismo análisis e indexación de marcadores también puede alimentar una API de sugerencias local para [Homepage](https://github.com/gethomepage/homepage).

::: tip
Las extensiones del navegador funcionan **fuera de línea**. No se requiere de una conexión a Internet. Por favor visite la [ extensión del navegador QOwnNotes Web Companion](browser-extension.md) para mayor información.
:::

## Panel de control de Homepage

QOwnNotes puede exponer un endpoint HTTP local para la compatibilidad con [Homepage](https://github.com/gethomepage/homepage) y `suggestionUrl`, respaldado por el mismo análisis e indexación de marcadores que utiliza la fuente de datos de Web Companion.

- Habilítelo en `Configuración -> Extensión del navegador/fragmentos de comandos`
- Habilite `Habilitar servidor de sockets`
- En `API de sugerencias de marcadores`, habilite `Activar API de sugerencias de marcadores compatible con Homepage`
- Establece un puerto para el endpoint local
- Opcionalmente, configure un token de seguridad si desea que las solicitudes a Homepage requieran autenticación
- El servicio solo se conecta a `127.0.0.1`

El endpoint está disponible como `GET /suggest?q=home` y admite un parámetro opcional `limit` (valor predeterminado `10`, máximo `50`) y un parámetro opcional `token`.

Si utiliza los recursos personalizados de Homepage de `docs/homepage/custom.js`, establezca `QON_TOKEN` con el mismo token de seguridad configurado en QOwnNotes.

Configuración de ejemplo de Homepage:

```yaml
search:
  provider: custom
  url: https://ejemplo.com/search?q=
  suggestionUrl: http://127.0.0.1:22224/suggest?q=
  showSearchSuggestions: true
```

::: tip
Visite la [API de sugerencias de Homepage](homepage-suggestion-api.md) para obtener más información, incluyendo qué archivo de configuración de la página de inicio editar y cómo usar los recursos personalizados de `docs/homepage`.
:::

## Gestor de fragmentos de comandos de terminal de QOwnNotes

Puede administrar sus **fragmentos de comandos** con QOwnNotes y ejecutarlos en las órdenes de terminal.

::: tip
Por favor visite el [gestor de fragmentos de comandos de terminal de QOwnNotes](command-line-snippet-manager.md) para mayor información.
:::

## Servicio de LanguageTool

QOwnNotes puede usar un servidor de [LanguageTool](https://languagetool.org/) local o remoto para la **comprobación de gramática y estilo** en el editor.

- Es **opcional** y funciona en conjunto con el soporte actual de correctores ortográficos
- Actívelo en `Ajustes -> Editor`, luego configure su **URL del servidor**, **idioma**, **clave de API opcional** y las categorías que desea consultar
- También puede activarlo rápidamente desde el menú **Ortografía** con `Verificar gramática con LanguageTool`
- En el menú contextual del editor se ofrecen sugerencias y acciones para los problemas detectados

::: tip
Por favor visite [LanguageTool](../editor/languagetool.md) para más información.
:::

## Servicio de Harper

QOwnNotes can use a local [Harper](https://writewithharper.com/) service for **offline grammar and style checking** in the editor.

- It is **optional** and works alongside the existing spell checker support
- Enable it in `Settings -> Editor -> Harper`, then configure the **transport**, `harper-ls` command or TCP target, **dialect**, and the linters you want to use
- You can also toggle it quickly from the **Edit** menu with `Check grammar with Harper`
- Suggestions and ignore actions are available from the editor context menu for detected issues

::: tip
Please visit [Harper](../editor/harper.md) for more information.
:::

## Cliente de sincronización de escritorio de Nextcloud

**Sincronice sus notas** con otros dispositivos (desktop & mobile) con su cliente de sincronización [Nextcloud](https://nextcloud.com/) o [ownCloud](https://owncloud.org/) con su servidor.

::: tip
Por supuesto, otras soluciones, como **Dropbox**, **Syncthing**, **Seafile** o BitTorrent Sync también se pueden utilizar para sincronizar sus notas y otros archivos.

También puede usar **git** para sincronizar con herramientas como [gitomatic](https://github.com/muesli/gitomatic/).
:::

## Servidor Nextcloud

Para trabajar con sus notas en línea, puede utilizar servidores como [Nextcloud](https://nextcloud.com/) o [ownCloud](https://owncloud.org/).

Puede alojar su propio servidor o utilizar soluciones alojadas.

Hay una [lista mantenida por la comunidad de proveedores de Nextcloud](https://github.com/nextcloud/providers#providers), así como una [lista de dispositivos con Nextcloud](https://nextcloud.com/devices/).

[Portknox](https://portknox.net) ha informado que tienen [QOwnNotesAPI instalado](https://portknox.net/en/app_listing).

::: tip
Por supuesto, otras soluciones, como **Dropbox**, **Syncthing**, **Seafile** o BitTorrent Sync también se pueden utilizar para alojar sus notas y otros archivos.
:::

## QOwnNotesAPI Nextcloud app

[**QOwnNotesAPI**](https://github.com/pbek/qownnotesapi) le permite acceder a su **notas enviadas a la papelera** y **versiones de notas** del lado del servidor.

::: tip
Por favor visite [Aplicación QOwnNotesAPI Nextcloud](qownnotesapi.md) para mayor información.
:::

## Aplicación de servidor de Nextcloud Notes

Utilice [**Nextcloud Notes**](https://github.com/nextcloud/notes) para editar sus notas en la **web**.

::: warning
Tenga en cuenta que Nextcloud Notes actualmente solo admite un nivel de subcarpetas.
:::

## Aplicación de Nextcloud Deck para servidores

Es posible utilizar QOwnNotes para crear con rapidez **tarjetas** en [**Nextcloud Deck**](https://github.com/nextcloud/deck).

## Aplicación móvil Nexcloud Notes

Para acceder a sus notas de Nextcloud / ownCloud desde su **dispositivo móvil**, puede utilizar diferentes aplicaciones.

### Android

- [ Nextcloud Notes para Android ](https://play.google.com/store/apps/details?id=it.niedermann.owncloud.notes) (terceros)

::: tip
También puede usar cualquier herramienta de sincronización, como _Synchronize Ultimate_ o _FolderSync_, para sincronizar sus archivos de notas, y usar software como _neutriNotes_ o [**Markor**](https://f-droid.org/packages/net.gsantner.markor/) para editarlas.
:::

### iOS

- [CloudNotes para iOS](https://itunes.apple.com/de/app/cloudnotes-owncloud-notes/id813973264?mt=8) (terceros)

::: tip
También puede utilizar [Notebooks](https://itunes.apple.com/us/app/notebooks-write-and-organize/id780438662) y sincronizar sus notas por medio de WebDAV. Hay un buen tutorial en [Tomar notas con Nexcloud, QOwnNotes, y Notebooks](https://lifemeetscode.com/blog/taking-notes-with-nextcloud-qownnotes-and-notebooks)
:::

## api.qownnotes.org

Este es un servicio en línea proporcionado por QOwnNotes para verificar si hay una nueva versión de la aplicación disponible.

Se comunica con GitHub y comprueba si existe una versión más reciente, obtiene una URL de descarga adecuada y compila los cambios del registro de novedades comparado con la versión de QOwnNotes que está utilizando actualmente en formato HTML para su visualización en el cuadro de diálogo Actualización.

Además, también proporciona el [feed RSS de versiones](http://api.qownnotes.org/rss/app-releases) y una implementación de la API heredada de comprobación de actualizaciones para las versiones anteriores de QOwnNotes.

::: tip
Se puede acceder al código fuente de [api.qownnotes.org](https://api.qownnotes.org) en [GitHub](https://github.com/qownnotes/api).
:::

## Aplicación web de QOwnNotes

Es posible insertar fotografías de su teléfono móvil en la nota actual en QOwnNotes en su equipo de escritorio por medio de la **aplicación web** disponible en [app.qownnotes.org](https://app.qownnotes.org/).

::: tip
Por favor visite la [aplicación web de QOwnNotes](web-app.md) para mayor información.
:::
