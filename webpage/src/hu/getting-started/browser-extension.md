---
image: /img/bookmarks.png
---

# QOwnNotes Web Companion böngészőbővítmény

Lehetővé teszi a böngészőoldalak kivágását és a böngésző könyvjelzőinek kezelését a böngészők és az operációs rendszerek között.

::: tip Info
**QOwnNotes must be running** for the Web Companion browser extension to work. But no internet connection is needed, the browser extensions works **offline**.
:::

## Telepítés

1. Szerezze meg a bővítményt
   - [Chrome Internetes áruház](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
   - [Firefox kiegészítők oldala](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
   - A kiterjesztést a [GitHub](https://github.com/qownnotes/web-companion/) oldalon is megtalálja.
2. Adja hozzá a biztonsági tokent a bővítmény konfigurálásához.
   - Amikor először kattint a QOwnNotes böngészőbővítmény ikonjára, megjelenik egy párbeszédpanel egy biztonsági tokennel. Másolja a tokent.
   - Nyissa meg a böngésző bővítménykezelési helyét. Kattintson a QOwnNotes bővítmény részleteire.
   - Illessze be a tokent a Biztonsági Token mezőbe.
3. To use the private mode, you need to allow the extension to run in private windows in the extension settings.

## Web clipper

![web-clipper](/img/web-clipper.png)

Kattintson a jobb gombbal egy weboldalra vagy a kiválasztott szövegre a ** web-clipper** funkció használatához. Itt új jegyzetet is létrehozhat az aktuális weboldal **képernyőkép** jével.

::: warning
Info If the web clipper doesn't work, please check if the QOwnNotes desktop application is running, and the Web Companion browser extension is configured with the security token.

Also, make sure you accept the consent dialog when you first click the extension icon in the browser toolbar.

If you are using Firefox, you also need to make sure that you have turned on the optional permission `Access your data for all websites` in the _Permission_ extension settings, otherwise you might get an error `TypeError: chrome.tabs.captureVisibleTab is not a function`.
:::

::: tip
A webleíró is szkriptelhető! Vessen egy pillantást a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) oldalra, ha szeretné szabályozni, hogy mit is klipel a weboldalakról.
:::

## Könyvjelzők

![bookmarks](/img/bookmarks.png)

Ha teljes irányítást szeretne a böngésző könyvjelzői felett, és **különböző böngészőkben és operációs rendszerekben használni szeretné őket**, akkor a QOwnNotes böngészőbővítmény az Ön számára készült.

By default, the browser extension will show all **links of the current note** in a popup when you click the QOwnNotes icon in your browser. Ezek a linkek egy `current` címkét kapnak.

A ** könyvjelzőket jegyzetekben is kezelheti** a `bookmarks` (a beállításokban módosítható) jegyzetcímkével. Ezek a linkek címkékkel és leírással is rendelkezhetnek, amelyek a böngésző kiterjesztésében jelennek meg.

Az új könyvjelzőket a `Könyvjelzők` nevű jegyzet tárolja (a beállításokban is változtatható).

::: tip
A böngésző könyvjelzőit a webkísérő böngésző kiterjesztéssel is importálhatja a QOwnNotes programba!
:::

### A könyvjelző linkek szintaxisa

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) some description only
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags

* [Webpage name](https://www.example.com) the alternative list character also works
```

You are able to search for name, url tags or description in the browser extension. In the current note all other links inside of text are parsed, but without tags or description.
