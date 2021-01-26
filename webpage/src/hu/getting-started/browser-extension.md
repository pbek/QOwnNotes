---
image: /img/bookmarks.png
---

# QOwnNotes Web Companion böngészőbővítmény

![bookmarks](/img/bookmarks.png)

Látogasson el a [Chrome Internetes áruházba](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) vagy a  [Firefox Add-ons page](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) a [**QOwnNotes Web Companion böngészőbővítmény**](https://github.com/qownnotes/web-companion/) telepítéséhez.

A kiterjesztést a [GitHub](https://github.com/qownnotes/web-companion/) oldalon is megtalálja.

## Web clipper

Kattintson a jobb gombbal egy weboldalra vagy a kiválasztott szövegre a ** web-clipper** funkció használatához. Itt új jegyzetet is létrehozhat az aktuális weboldal **képernyőkép** jével.

::: tip A webleíró is szkriptelhető! Vessen egy pillantást a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) oldalra, ha szeretné szabályozni, hogy mit is klipel a weboldalakról. :::

## Könyvjelzők

Alapértelmezés szerint a böngészőbővítmény egy felugró ablakban megjeleníti az aktuális **jegyzet összes linkjét**, amikor a böngészőben a QOwnNotes ikonra kattint. Ezek a linkek egy `current` címkét kapnak.

A ** könyvjelzőket jegyzetekben is kezelheti** a `bookmarks` (a beállításokban módosítható) jegyzetcímkével. Ezek a linkek címkékkel és leírással is rendelkezhetnek, amelyek a böngésző kiterjesztésében jelennek meg.

Az új könyvjelzőket a `Könyvjelzők` nevű jegyzet tárolja (a beállításokban is változtatható).

::: tip A böngésző könyvjelzőit a webkísérő böngésző kiterjesztéssel is importálhatja a QOwnNotes programba! :::

### A könyvjelző linkek szintaxisa

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) csak néhány leírás
- [Webpage name](https://www.example.com) #tag1 #tag2 néhány leírás és címke
* [Webpage name](https://www.example.com) az alternatív lista karakter is működik
```

A böngésző kiterjesztésében kereshet nevet, URL-címkéket vagy leírást.
