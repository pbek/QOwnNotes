---
image: /img/bookmarks.png
---

# Browser-extensie QOwnNotes Web Companion

Maakt het mogelijk om vanaf een browserpagina te knippen en browserbladwijzers te beheren in verschillende browsers en besturingssystemen.

::: tip Info
**QOwnNotes must be running** for the Web Companion browser extension to work. But no internet connection is needed, the browser extensions works **offline**.
:::

## Installatie

1. Verkrijg de extensie
   - [Chrome webshop](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
   - [Firefox Add-ons-pagina](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
   - U kunt de extensie ook vinden op [ GitHub ](https://github.com/qownnotes/web-companion/).
2. Voeg het beveiligingstoken toe om de extensie te configureren.
   - De eerste keer dat u op het QOwnNotes-pictogram voor de browserextensie klikt, ontvangt u een dialoogvenster met een beveiligingstoken. Kopieer de token.
   - Ga naar de locatie voor extensiebeheer van uw browser. Klik in de details van de QOwnNotes-extensie.
   - Plak het token in het veld Beveiligingstoken.
3. Om de privémodus te gebruiken, moet u in de extensie-instellingen toestemming verlenen om de extensie in privévensters te laten draaien.

## Webclipper

![web-clipper](/img/web-clipper.png)

Klik met de rechtermuisknop op een webpagina of geselecteerde tekst om de **webclipper** -functionaliteit te gebruiken. Daar kun je ook een nieuwe notitie maken met een **screenshot** van de huidige webpagina.

::: waarschuwing Info Als de webclipper niet werkt, controleer dan of de QOwnNotes-desktopapplicatie wel actief is en de browserextensie Web Companion is geconfigureerd met het beveiligingstoken.

Also, make sure you accept the consent dialog when you first click the extension icon in the browser toolbar.

If you are using Firefox, you also need to make sure that you have turned on the optional permission `Access your data for all websites` in the _Permission_ extension settings, otherwise you might get an error `TypeError: chrome.tabs.captureVisibleTab is not a function`.
:::

::: tip
De webclipper is ook scriptbaar! Bekijk de [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) als je wilt bepalen wat je van webpagina's knipt.
:::

## Bladwijzers

![bookmarks](/img/bookmarks.png)

Als u volledige controle wilt over uw browserbladwijzers en **ze wilt gebruiken in verschillende browsers en besturingssystemen**, dan is de QOwnNotes-browserextensie iets voor u.

Standaard toont de browserextensie alle ** links van de huidige notitie** in een pop-up wanneer u op het QOwnNotes-pictogram in uw browser klikt. Deze links krijgen een tag `current`.

U kunt ook **uw bladwijzers in notities beheren** met de notitietag `bladwijzers` (wijzigbaar in de instellingen). Deze links kunnen ook tags en een beschrijving hebben die in de browserextensie wordt weergegeven.

Nieuwe bladwijzers worden opgeslagen in een notitie genaamd `Bladwijzers` (ook te wijzigen in de instellingen).

::: tip
U kunt ook uw browserbladwijzers importeren in QOwnNotes met de webpartner-browserextensie!
:::

### Syntaxis van bladwijzerkoppelingen

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) some description only
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags

* [Webpage name](https://www.example.com) the alternative list character also works
```

Je kunt zoeken op naam, url-tags of beschrijving in de browserextensie. In de huidige notitie worden alle andere koppelingen in de tekst doorzocht, maar zonder tags of beschrijving.
