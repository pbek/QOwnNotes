# QOwnNotes erstellen

## Mit GitHub bauen

Sie können QOwnNotes auch selbst aus dem Quellcode erstellen.

Checkout the code of the last release directly from the git repository:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

For debugging, it's easiest to use QtCreator. When downloading [Qt Creator](https://www.qt.io/download-qt-installer-oss), you will also need the package `qtwebsockets` to build QOwnNotes with Qt6. If you build under Windows, you want to stick to _MinGw 64-bit_. On Linux, you most likely have a package for QtCreator in your repository.

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
If you want to contribute source code to the project please make your pull requests to the `main` branch.
:::

## Quellarchiv

Quellarchive von QOwnNotes finden Sie unter [QOwnNotes Quellarchiv](https://github.com/pbek/QOwnNotes/releases).

Auf die Quellarchive kann so zugegriffen werden:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`

## Building with nix

If you have the [Nix package manager](https://nixos.org/download/) available you can just git-clone the source code, like explained above and build with:

```bash
# Get devenv if you don't have it already
nix-shell -p devenv

# Open a shell with all the tools you need
devenv shell

# Build the application
just nix-build

# Run the built application
just nix-run
```
