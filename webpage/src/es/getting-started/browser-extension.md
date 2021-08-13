---
image: /img/bookmarks.png
---

# Extensión del navegador QOwnNotes Web Companion

![marcadores](/img/bookmarks.png)

Visite [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) o la [página de complementos de Firefox](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) para instalar la [**extensión del navegador QOwnNotes Web Companion**](https://github.com/qownnotes/web-companion/).

También puede encontrar la extensión en [GitHub](https://github.com/qownnotes/web-companion/).

::: tip Info
QOwnNotes debe estar ejecutándose para que funcione la extensión del navegador Web Companion. The browser extensions works **offline**, no internet connection needed.
:::

## Cortadora web

Haga clic con el botón derecho en una página web o en el texto seleccionado para usar la funcionalidad **web-clipper**. Allí también puede crear una nueva nota con una ** captura de pantalla** de la página web actual.

::: tip
¡El cortador web también es programable! Eche un vistazo a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) si desea controlar lo que recorta de las páginas web.
:::

## Marcadores

If you want full control over your browser bookmarks and **use them across different browsers and operating systems** then the QOwnNotes browser extension is for you.

By default the browser extension will show all **links of the current note** in a popup when you click the QOwnNotes icon in your browser. These links will get a tag `current`.

You can also **manage your bookmarks in notes** with the note tag `bookmarks` (changeable in the settings). These links can also have tags and a description that will be shown in the browser extension.

New bookmarks are stored in a note called `Bookmarks` (also changeable in the settings).

::: tip
You can also import your browser bookmarks into QOwnNotes with the web companion browser extension!
:::

### Sintaxis de los enlaces de marcadores

```markdown
- [nombre de la página web] (https://www.example.com)
- [nombre de la página web] (https://www.example.com) #tag1 #tag2
- [Nombre de la página web] (https://www.example.com) solo una descripción
- [Nombre de la página web] (https://www.example.com) #tag1 #tag2 alguna descripción y etiquetas
* [Nombre de la página web] (https://www.example.com) el carácter de lista alternativo también funciona
```

You are able to search for name, url tags or description in the browser extension.
