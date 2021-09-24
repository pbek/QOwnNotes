---
image: /img/bookmarks.png
---

# Estensione del browser QOwnNotes Web Companion

Visita il [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp) o la [pagina dei componenti aggiuntivi di Firefox](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion) per installare l '[** estensione del browser QOwnNotes Web Companion **](https://github.com/qownnotes/web-companion/).

Puoi trovare l'estensione anche su [GitHub](https://github.com/qownnotes/web-companion/).

::: tip Info
QOwnNotes deve essere in esecuzione affinché l'estensione del browser Web Companion funzioni. L'estensione del browser funziona **offline**, senza alcuna connessione internet.
:::

## Web clipper

![Strumento di ritaglio web](/img/web-clipper.png)

Fai clic con il pulsante destro del mouse su una pagina web o sul testo selezionato per utilizzare la funzionalità **web-clipper**. Lì puoi anche creare una nuova nota con uno **screenshot** della pagina web corrente.

::: tip
Il web clipper è anche scriptabile! Dai un'occhiata a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) se desideri controllare ciò che ritagli dalle pagine web.
:::

## Segnalibri

![Segnalibri](/img/bookmarks.png)

Se vuoi il pieno controllo dei tuoi segnalibri del browser e **usarli tra diversi browser e sistemi operativi**, allora l'estensione del browser di QOwnNotes fa per te.

Per impostazione predefinita, l'estensione del browser mostrerà tutti i collegamenti ** della nota corrente** in un popup quando fai clic sull'icona QOwnNotes nel tuo browser. Questi collegamenti avranno l'etichetta `corrente`.

Puoi anche **gestire i tuoi segnalibri nelle note** con l'etichetta `segnalibri` (modificabile nelle impostazioni). Questi collegamenti possono anche avere etichette e una descrizione che verrà mostrata nell'estensione del browser.

I nuovi segnalibri sono memorizzati in una nota chiamata `Segnalibri` (anch'essa modificabile nelle impostazioni).

::: tip
Puoi anche importare i segnalibri del tuo browser in QOwnNotes con l'estensione browser web companion!
:::

### Sintassi dei collegamenti segnalibri

```markdown
- [Nome pagina web](https://www.example.com)
- [Nome pagina web](https://www.example.com) #etichetta1 #etichetta2
- [Nome pagina web](https://www.example.com) solo descrizione
- [Nome pagina web](https://www.example.com) #etichetta1 #etichetta2 descrizione e etichette
* [Nome pagina web](https://www.example.com) funziona anche il carattere lista alternativo
```

Puoi cercare nome, etichetta URL o descrizione nell'estensione del browser.
