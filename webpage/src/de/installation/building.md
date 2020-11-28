# QOwnNotes erstellen

## Von GitHub aus bauen

Sie können QOwnNotes auch selbst aus dem Quellcode erstellen.

Checken Sie den Code direkt aus dem Git-Repository aus:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b master
cd QOwnNotes
git submodule update --init
```

Laden Sie dann [Qt Creator](https://www.qt.io/download-open-source) herunter, öffnen Sie die Projektdatei `src/QOwnNotes.pro` und klicken Sie auf **Build** / **Build Project QOwnNotes** .

Oder Sie können es direkt in Ihrem Terminal erstellen:

```bash
cd src
qmake
make
```

Sie können gerne Quellcode zu diesem Projekt beitragen, Vorschläge machen oder Probleme auf der Seite [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues). melden.

Besuchen Sie [QOwnNotes auf GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Wenn Sie Quellcode zum Projekt beitragen möchten, senden Sie bitte Ihre Pull-Anfragen an den `develop` Zweig.
:::

## Quellarchiv

Sie finden Quellarchive von QOwnNotes im [QOwnNotes-Quellarchiv](https://download.tuxfamily.org/qownnotes/src/).

Auf die Quellarchive kann folgendermaßen zugegriffen werden:

`https://download.tuxfamily.org/qownnotes/src/qownnotes-20.10.1.tar.xz`
