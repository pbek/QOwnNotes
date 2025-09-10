# Perché le metriche?

## Perché le metriche sono utili?

QOwnNotes (as in the application itself) is **developed by mostly one person in his spare time**.

I rilasci sono preparati per molte piattaforme e distribuzioni (spesso per versioni molto vecchie delle stesse). Mantenerle attive costa un sacco di lavoro.

Le metriche sono la sola fonte di informazione attendibile su quali versioni di piattaforme e distribuzioni gira QOwnNotes. Mostrano anche quali funzionalità sono le più popolari, in modo tale che si possano prendere decisioni su quali versioni ritirare, quali funzionalità debbano essere migliorate o quali siano poco usate.

Lo stesso vale per la scelta dei linguaggi dell'utente, che favorisce il miglioramento delle traduzioni corrispondenti.

## Dove sono salvate le metriche?

Le metriche vengono trasmesse e immagazzinate in un server [Matomo](https://matomo.org/) auto-gestito. Solo i due sviluppatori principali hanno accesso ai dati. **Nessun dato è condiviso con terze parti.**

## Come sono raccolte le metriche?

Le metriche sono **rese anonime** (p.es. l'indirizzo IP è rimosso) sul server Matomo dopo che sono ricevute.

Nessuna informazione personale, come il testo che inserisci nelle note o il nome delle note, è raccolta o immagazzinata nel server!

## Cosa è raccolto?

Sono trasmesse alcune informazioni come la risoluzione dello schermo, il sistema operativo e la versione di QOwnNotes in uso.

In più, sono inviati alcuni eventi al server, per esempio quando certe azioni vengono fatte nell'applicazione.

QOwnNotes è un software libero, il cui codice è aperto (free open source software). Puoi vedere le metriche in ogni momento cercando, per esempio [sendVisitIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendVisitIfEnabled) e [sendEventIfEnabled](https://github.com/pbek/QOwnNotes/search?q=sendEventIfEnabled) nel codice sorgente per vedere cosa esattamente è mandato.

**Non è raccolta alcuna informazione personale, né il testo che inserisci né i nomi delle note!**

## Posso spegnere le metriche?

Puoi spegnerle al primo avvio dell'applicazione nella finestra di benvenuto o, successivamente, nella finestra delle impostazioni.

## Perché le metriche non sono inizialmente spente?

Le impostazioni predefinite comandano. Spegnere le metriche come impostazione predefinita risultebbere nel non avere metriche significative.

A molte persone non interessano le metriche. Quelli a cui interessano le metriche ma a cui non interessa decidere su cosa si debba intervenire o supportare, possono tranquillamente spegnere all'avvio dell'applicazione.

Vedrai anche una seconda finestra che ti spiega le metriche, nel caso chiudessi la finestra di benvenuto troppo presto.
