---
image: /img/bookmarks.png
---

# QOwnNotes Web Companion böngészőbővítmény

Lehetővé teszi a böngészőoldalak kivágását és a böngésző könyvjelzőinek kezelését a böngészők és az operációs rendszerek között.

::: tip
Információ
- A Web Companion böngészőbővítmény működéséhez a QOwnNotes-nak futnia kell.
- Nincs szükség internet kapcsolatra. A böngészőbővítmények **offline** működnek.
:::

## Telepítés

1. Szerezze meg a bővítményt
    - [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
    - [Firefox Add-ons page](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
    - A kiterjesztést a [GitHub](https://github.com/qownnotes/web-companion/) oldalon is megtalálja.
2. Adja hozzá a biztonsági tokent a bővítmény konfigurálásához.
    - Amikor először kattint a QOwnNotes böngészőbővítmény ikonjára, megjelenik egy párbeszédpanel egy biztonsági tokennel. Másolja a tokent.
    - Nyissa meg a böngésző bővítménykezelési helyét. Kattintson a QOwnNotes bővítmény részleteire.
    - Illessze be a tokent a Biztonsági Token mezőbe.

## Web clipper

![web-clipper](/img/web-clipper.png)

Kattintson a jobb gombbal egy weboldalra vagy a kiválasztott szövegre a ** web-clipper** funkció használatához. Itt új jegyzetet is létrehozhat az aktuális weboldal **képernyőkép** jével.

::: tip
A webleíró is szkriptelhető! Vessen egy pillantást a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) oldalra, ha szeretné szabályozni, hogy mit is klipel a weboldalakról.
:::

## Könyvjelzők

![bookmarks](/img/bookmarks.png)

Ha teljes irányítást szeretne a böngésző könyvjelzői felett, és **különböző böngészőkben és operációs rendszerekben használni szeretné őket**, akkor a QOwnNotes böngészőbővítmény az Ön számára készült.

Alapértelmezés szerint a böngészőbővítmény egy felugró ablakban megjeleníti az aktuális **jegyzet összes linkjét**, amikor a böngészőben a QOwnNotes ikonra kattint. Ezek a linkek egy `current` címkét kapnak.

A ** könyvjelzőket jegyzetekben is kezelheti** a `bookmarks` (a beállításokban módosítható) jegyzetcímkével. Ezek a linkek címkékkel és leírással is rendelkezhetnek, amelyek a böngésző kiterjesztésében jelennek meg.

Az új könyvjelzőket a `Könyvjelzők` nevű jegyzet tárolja (a beállításokban is változtatható).

::: tip
A böngésző könyvjelzőit a webkísérő böngésző kiterjesztéssel is importálhatja a QOwnNotes programba!
:::

### A könyvjelző linkek szintaxisa

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) csak néhány leírás
- [Webpage name](https://www.example.com) #tag1 #tag2 néhány leírás és címke
* [Webpage name](https://www.example.com) az alternatív lista karakter is működik
```

A böngésző kiterjesztésében kereshet nevet, URL-címkéket vagy leírást.
