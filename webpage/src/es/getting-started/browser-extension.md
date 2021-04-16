---
image: /img/bookmarks.png
---

# Extensión del navegador QOwnNotes Web Companion

![marcadores](/img/bookmarks.png)

Visit the [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) or the [Firefox Add-ons page](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) to install the [**QOwnNotes Web Companion browser extension**](https://github.com/qownnotes/web-companion/).

También puede encontrar la extensión en [GitHub](https://github.com/qownnotes/web-companion/).

::: tip Info
QOwnNotes needs to be running for the Web Companion browser extension to work.
:::

## Cortadora web

Right-click on a webpage or selected text to use the **web-clipper** functionality. There you can also create a new note with a **screenshot** of the current webpage.

::: tip
The web clipper is also scriptable! Take a look at the [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) if you want to control what you clip from webpages.
:::

## Marcadores

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
