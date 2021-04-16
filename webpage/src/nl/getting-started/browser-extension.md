---
image: /img/bookmarks.png
---

# Browser-extensie QOwnNotes Web Companion

![bookmarks](/img/bookmarks.png)

Bezoekt u het [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) of de [Firefox Add-ons page ](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) om de [**QOwnNotes Web Companion browser extension**](https://github.com/qownnotes/web-companion/) te installeren.

U kunt de extensie ook vinden op [ GitHub ](https://github.com/qownnotes/web-companion/).

::: tip Info
QOwnNotes needs to be running for the Web Companion browser extension to work.
:::

## Webclipper

Right-click on a webpage or selected text to use the **web-clipper** functionality. There you can also create a new note with a **screenshot** of the current webpage.

::: tip
The web clipper is also scriptable! Take a look at the [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) if you want to control what you clip from webpages.
:::

## Bladwijzers

By default the browser extension will show all **links of the current note** in a popup when you click the QOwnNotes icon in your browser. These links will get a tag `current`.

You can also **manage your bookmarks in notes** with the note tag `bookmarks` (changeable in the settings). These links can also have tags and a description that will be shown in the browser extension.

New bookmarks are stored in a note called `Bookmarks` (also changeable in the settings).

::: tip
You can also import your browser bookmarks into QOwnNotes with the web companion browser extension!
:::

### Syntaxis van bladwijzerkoppelingen

```markdown
- [Webpaginanaam](https://www.example.com)
- [Webpaginanaam](https://www.example.com) #tag1 #tag2
- [Webpaginanaam](https://www.example.com) slechts een beschrijving
- [Webpaginanaam](https://www.example.com) #tag1 #tag2 een beschrijving en tags
* [Webpaginanaam](https://www.example.com) het alternatieve lijstteken werkt ook
```

You are able to search for name, url tags or description in the browser extension.
