---
image: /img/bookmarks.png
---

# Estensione del browser QOwnNotes Web Companion

![bookmarks](/img/bookmarks.png)

Visita il [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) o la [pagina dei componenti aggiuntivi di Firefox](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) per installare l '[** estensione del browser QOwnNotes Web Companion **](https://github.com/qownnotes/web-companion/).

Puoi anche trovare l'estensione su [GitHub](https://github.com/qownnotes/web-companion/).

::: tip Info
QOwnNotes deve essere in esecuzione affinché l'estensione del browser Web Companion funzioni.
:::

## Web clipper

Fai clic con il pulsante destro del mouse su una pagina web o sul testo selezionato per utilizzare la funzionalità **web-clipper**. Lì puoi anche creare una nuova nota con uno **screenshot** della pagina web corrente.

::: tip
Il web clipper è anche scriptabile! Dai un'occhiata a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) se desideri controllare ciò che ritagli dalle pagine web.
:::

## Segnalibri

Per impostazione predefinita, l'estensione del browser mostrerà tutti i collegamenti ** della nota corrente** in un popup quando fai clic sull'icona QOwnNotes nel tuo browser. La tesi a sinistra vuole ottenere un tag `current`.

Puoi anche **gestire i tuoi segnalibri nelle note** con il tag nota `segnalibri` (modificabile nelle impostazioni). Questi collegamenti possono anche avere tag e una descrizione che verrà mostrata nell'estensione del browser.

I nuovi segnalibri vengono memorizzati in una nota chiamata `Segnalibri` (anch'essa modificabile nelle impostazioni).

::: tip
Puoi anche importare i segnalibri del tuo browser in QOwnNotes con l'estensione browser web companion!
:::

### Syntax of bookmark links

```markdown
- [Webpage name](https://www.example.com)
- [Webpage name](https://www.example.com) #tag1 #tag2
- [Webpage name](https://www.example.com) some description only
- [Webpage name](https://www.example.com) #tag1 #tag2 some description and tags
* [Webpage name](https://www.example.com) the alternative list character also works
```

Puoi cercare nome, tag URL o descrizione nell'estensione del browser.
