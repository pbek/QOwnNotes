---
image: /img/bookmarks.png
---

# Browser-extensie QOwnNotes Web Companion

![bookmarks](/img/bookmarks.png)

Bezoekt u het [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) of de [Firefox Add-ons page ](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) om de [**QOwnNotes Web Companion browser extension**](https://github.com/qownnotes/web-companion/) te installeren.

U kunt de extensie ook vinden op [ GitHub ](https://github.com/qownnotes/web-companion/).

## Webclipper

Klik met de rechtermuisknop op een webpagina of geselecteerde tekst om de **webclipper** -functionaliteit te gebruiken. There you can also create a new note with a **screenshot** of the current webpage.

::: tip
De webclipper is ook scriptbaar! Bekijk de [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) als je wilt bepalen wat je van webpagina's knipt.
:::

## Bladwijzers

Standaard toont de browserextensie alle **links van de huidige notitie** in een pop-up wanneer u op het QOwnNotes-pictogram in uw browser klikt. Deze links krijgen een tag `current`.

U kunt ook **uw bladwijzers in notities beheren** met de notitietag `bladwijzers` (wijzigbaar in de instellingen). Deze links kunnen ook tags en een beschrijving hebben die in de browserextensie wordt weergegeven.

::: tip
U kunt ook uw browserbladwijzers importeren in QOwnNotes met de webpartner-browserextensie!
:::

### Syntaxis van bladwijzerkoppelingen

```markdown
- [Webpaginanaam](https://www.example.com)
- [Webpaginanaam](https://www.example.com) #tag1 #tag2
- [Webpaginanaam](https://www.example.com) slechts een beschrijving
- [Webpaginanaam](https://www.example.com) #tag1 #tag2 een beschrijving en tags
* [Webpaginanaam](https://www.example.com) het alternatieve lijstteken werkt ook
```

U kunt zoeken op naam, url-tags of beschrijving in de browserextensie.
