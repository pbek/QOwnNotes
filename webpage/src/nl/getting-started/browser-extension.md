---
image: /img/bookmarks.png
---

# Browser-extensie QOwnNotes Web Companion

Visit the [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) or the [Firefox Add-ons page](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) to install the [**QOwnNotes Web Companion browser extension**](https://github.com/qownnotes/web-companion/).

You can also find the extension on [GitHub](https://github.com/qownnotes/web-companion/).

::: tip Info
QOwnNotes needs to be running for the Web Companion browser extension to work. The browser extensions works **offline**, no internet connection needed.
:::

## Webclipper

![web-clipper](/img/web-clipper.png)

Klik met de rechtermuisknop op een webpagina of geselecteerde tekst om de **webclipper** -functionaliteit te gebruiken. Daar kun je ook een nieuwe notitie maken met een **screenshot** van de huidige webpagina.

::: tip
De webclipper is ook scriptbaar! Bekijk de [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) als je wilt bepalen wat je van webpagina's knipt.
:::

## Bladwijzers

![bookmarks](/img/bookmarks.png)

If you want full control over your browser bookmarks and **use them across different browsers and operating systems** then the QOwnNotes browser extension is for you.

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
