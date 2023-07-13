# Creazione di QOwnNotes

## Compilare da GitHub

Puoi anche creare QOwnNotes da solo dal codice sorgente.

Ottieni il codice direttamente dal repository git:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

Then download [Qt Creator](https://www.qt.io/download-open-source), you will also need the packages `qtwebsockets` and `qt5compat` to build QOwnNotes with Qt6 (only `qtwebsockets` for Qt5). Se fai un build in Windows, cerca di usare *MinGw 64-bit*.

Afterward open the project file `src/QOwnNotes.pro` and click on **Build** / **Build Project QOwnNotes**.

Oppure puoi crearlo direttamente dal tuo terminale:

```bash
cd src

# costruisci i file binari delle traduzioni se vuoi una lingua diversa dall'inglese
lrelease QOwnNotes.pro

# prepara il processo di build dell'applicazione
qmake
make
```

Sentiti libero di contribuire con il codice sorgente a questo progetto, dare suggerimenti o segnalare problemi nella [pagina dei problemi di QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

Visita [QOwnNotes su GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Se vuoi contribuire con il codice sorgente al progetto, invia le tue richieste pull al ramo `develop`.
:::

## Archivio sorgente

You will find source archives of QOwnNotes on [QOwnNotes source archive](https://github.com/pbek/QOwnNotes/releases).

Gli archivi sorgente saranno accessibili in questo modo:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`
