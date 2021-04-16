---
image: /img/bookmarks.png
---

# Extension de navigateur QOwnNotes Web Companion

![bookmarks](/img/bookmarks.png)

Accédez au [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) ou à la [page des modules complémentaires Firefox](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) pour installer l '[**extension de navigateur QOwnNotes Web Companion**](https://github.com/qownnotes/web-companion/).

Vous pouvez également trouver l'extension sur [GitHub](https://github.com/qownnotes/web-companion/).

::: tip Info
QOwnNotes needs to be running for the Web Companion browser extension to work.
:::

## Clipper Web

Right-click on a webpage or selected text to use the **web-clipper** functionality. There you can also create a new note with a **screenshot** of the current webpage.

::: tip
The web clipper is also scriptable! Take a look at the [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) if you want to control what you clip from webpages.
:::

## Favoris

By default the browser extension will show all **links of the current note** in a popup when you click the QOwnNotes icon in your browser. These links will get a tag `current`.

You can also **manage your bookmarks in notes** with the note tag `bookmarks` (changeable in the settings). These links can also have tags and a description that will be shown in the browser extension.

New bookmarks are stored in a note called `Bookmarks` (also changeable in the settings).

::: tip
You can also import your browser bookmarks into QOwnNotes with the web companion browser extension!
:::

### Syntaxe des liens de signets

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) some description only
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
* [Webpage name](https://www.example.com) the alternative list character also works
```

You are able to search for name, url tags or description in the browser extension.
