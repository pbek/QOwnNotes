# QOwnNotes erstellen

## Mit GitHub bauen

Sie können QOwnNotes auch selbst aus dem Quellcode erstellen.

Checken Sie den Code direkt aus dem Git-Repository aus:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

For debugging, it's easiest to use QtCreator. When downloading [Qt Creator](https://www.qt.io/download-qt-installer-oss), you will also need the package `qtwebsockets` to build QOwnNotes with Qt6. Wenn Sie QOwnNotes unter Windows bauen, bleiben Sie am besten bei *MinGw 64-bit*. On Linux, you most likely have a package for QtCreator in your repository.

Öffnen Sie danach die Projektdatei `src/QOwnNotes.pro` und klicken Sie auf **Build** / **Build Project QOwnNotes**.

If you know what you are doing, you can also build QOwnNotes directly in your terminal:

```bash
cd src

# erstellt Binary-Überseztungsdateien, wenn Sie eine andere Sprache als Englisch möchten
lrelease QOwnNotes.pro

# bereite Build-Prozess vor und baue die Anwendung
qmake
make
```

Zögern Sie nicht, diesem Projekt Quellcode beizutragen, Vorschläge zu machen oder auf der [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues) Probleme zu melden.

Besuchen Sie [QOwnNotes auf GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Falls Sie dem Projekt Quellcode beitragen möchten, führen Sie Ihre Pull-Anfrage bitte im `main`-Branch durch.
:::

## Quellarchiv

Quellarchive von QOwnNotes finden Sie unter [QOwnNotes Quellarchiv](https://github.com/pbek/QOwnNotes/releases).

Auf die Quellarchive kann so zugegriffen werden:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`
