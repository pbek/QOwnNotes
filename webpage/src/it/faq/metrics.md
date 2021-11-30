# Perché le metriche?

## Perché le metriche sono utili?

QOwnNotes (l'applicazione) è **sviluppata da una persona, qualche volta due, nel tempo libero**.

I rilasci sono preparati per molte piattaforme e distribuzioni (spesso per versioni molto vecchie delle stesse). Mantenerle attive costa un sacco di lavoro.

The metrics are the only dependable source of information about which versions of platforms and distributions QOwnNotes runs on. They also show which features are the most popular, so that decisions can be made concerning which versions to retire, which features need more work, and which are hardly used.

The same applies to the user's selection of preferred languages, which promotes the improvement of the corresponding translations.

## Dove sono salvate le metriche?

Le metriche vengono trasmesse e immagazzinate in un server [Matomo](https://matomo.org/) auto-gestito. Solo i due sviluppatori principali hanno accesso ai dati. **Nessun dato è condiviso con terze parti.**

## Come sono raccolte le metriche?

Le metriche sono **rese anonime** (p.es. l'indirizzo IP è rimosso) sul server Matomo dopo che sono ricevute.

Nessuna informazione personale, come il testo che inserisci nelle note o il nome delle note, è raccolta o immagazzinata nel server!

## Cosa è raccolto?

Sono trasmesse alcune informazioni come la risoluzione dello schermo, il sistema operativo e la versione di QOwnNotes in uso.

In più, sono inviati alcuni eventi al server, per esempio quando certe azioni vengono fatte nell'applicazione.

QOwnNotes è un software libero, il cui codice è aperto (free open source software). Puoi vedere le metriche in ogni momento cercando, per esempio [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) e [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) nel codice sorgente per vedere cosa esattamente è mandato.

**Nessuna informazione personale, come il testo che inserisci o i nomi delle note, è raccolta!**

## Posso spegnere le metriche?

Puoi spegnerle al primo avvio dell'applicazione nella finestra di benvenuto o, successivamente, nella finestra delle impostazioni.

## Perché le metriche non sono inizialmente spente?

Le impostazioni predefinite comandano. Spegnere le metriche come impostazione predefinita risultebbere nel non avere metriche significative.

A molte persone non interessano le metriche. Quelli a cui interessano le metriche ma a cui non interessa decidere su cosa si debba intervenire o supportare, possono tranquillamente spegnere all'avvio dell'applicazione.

Vedrai anche una seconda finestra che ti spiega le metriche, nel caso chiudessi la finestra di benvenuto troppo presto.
