# Creazione di QOwnNotes

## Compilare da GitHub

Puoi anche creare QOwnNotes da solo dal codice sorgente.

Ottieni il codice direttamente dal repository git:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b master
cd QOwnNotes
git submodule update --init
```

Quindi scarica [Qt Creator](https://www.qt.io/download-open-source), apri il file di progetto `src/QOwnNotes.pro` e fai clic su **Build** / **Build Project QOwnNotes**.

Oppure puoi crearlo direttamente dal tuo terminale:

```bash
cd src
qmake
make
```

Sentiti libero di contribuire con il codice sorgente a questo progetto, dare suggerimenti o segnalare problemi nella [pagina dei problemi di QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

Visita [QOwnNotes su GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Se vuoi contribuire con il codice sorgente al progetto, invia le tue richieste pull al ramo `develop`.
:::

## Archivio sorgente

Troverai gli archivi sorgente di QOwnNotes in [archivio sorgente QOwnNotes](https://download.tuxfamily.org/qownnotes/src/).

Gli archivi sorgente saranno accessibili in questo modo:

`https://download.tuxfamily.org/qownnotes/src/qownnotes-20.10.1.tar.xz`
