---
image: /img/bookmarks.png
---

# Estensione del browser QOwnNotes Web Companion

Consente di ritagliare da una pagina del browser e gestire i segnalibri del browser su diversi browser e sistemi operativi.

::: tip
Info

- QOwnNotes deve essere in esecuzione affinché l'estensione del browser Web Companion funzioni.
- Non è necessaria una connessione Internet. L'estensione del browser lavora **offline**.
  :::

## Installazione

1. Scarica l'estensione
   - [Chrome Web Store](https://chrome.google.com/webstore/detail/qownnotes-web-companion/pkgkfnampapjbopomdpnkckbjdnpkbkp)
   - [Pagina delle Estensioni Firefox](https://addons.mozilla.org/firefox/addon/qownnotes-web-companion)
   - Puoi trovare l'estensione anche su [GitHub](https://github.com/qownnotes/web-companion/).
2. Aggiungi il token di sicurezza per configurare l'estensione.
   - La prima volta che clicchi sull'icona dell'estensione del browser di QOwnNotes, vedrai una finestra di dialogo col token di sicurezza. Copialo.
   - Vai alla gestione delle tue estensioni del browser. Clicca sui dettagli dell'estensione QOwnNotes.
   - Incolla il token nel campo del token di sicurezza.
3. Per usare la modalità in incognito devi prima consentire all'estensione di funzionare nelle finestre in incognito dalle impostazioni.

## Strumento di ritaglio web

![strumento di ritaglio web](/img/web-clipper.png)

Fai clic con il pulsante destro del mouse su una pagina web o sul testo selezionato per usare la funzionalità **web-clipper**. Lì puoi anche creare una nuova nota con uno **screenshot** della pagina web corrente.

::: warning
Info Se il web clipper non funziona, si prega di controlare che l'applicazione desktop di QOwnNotes sia aperta e che l'estensione web di supporto sia configurata con il token di sicurezza.

Inoltre assicurati di aver accettato il consenso tramite la finestra di avviso che compare quando clicchi l'icona dell'estensione sulla barra degli strumenti del browser.

Se stai usando Firefox dovresti accertarti di aver attivato il permesso facoltativo ` Accedere ai dati dei siti web` nelle impostazioni dei _permessi_ dell'estensione. Altrimenti potresti riscontrare l'errore `TypeError: chrome.tabs.captureVisibleTab non sta funzionando`.
:::

::: tip
Il web clipper è anche scriptabile! Dai un'occhiata a [websocketRawDataHook](../scripting/hooks.md#websocketrawdatahook) se desideri controllare ciò che ritagli dalle pagine web.
:::

## Segnalibri

![segnalibri](/img/bookmarks.png)

Se vuoi il pieno controllo dei tuoi segnalibri del browser e **usarli tra diversi browser e sistemi operativi**, allora l'estensione del browser di QOwnNotes fa per te.

Di base l'estensione del browser ti permette di mostrare tutti i **collegamenti inseriti sulla nota aperta nell'applicazione** in un popup quandi clicchi sull'icona di QOwnNotes dal browser. Questi collegamenti avranno l'etichetta `corrente`.

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

You are able to search for name, url tags or description in the browser extension. In the current note all other links inside of text are parsed, but without tags or description.
