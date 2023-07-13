# Compiler QOwnNotes

## Compiler à partir de GitHub

Vous pouvez également compiler vous-même QOwnNotes à partir du code source.

Récupérez le code directement depuis le dépôt git :

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

Then download [Qt Creator](https://www.qt.io/download-open-source), you will also need the packages `qtwebsockets` and `qt5compat` to build QOwnNotes with Qt6 (only `qtwebsockets` for Qt5). If you build under Windows, you want to stick to *MinGw 64-bit*.

Afterward open the project file `src/QOwnNotes.pro` and click on **Build** / **Build Project QOwnNotes**.

Or you can build it directly in your terminal:

```bash
cd src

# build binary translation files if you want another language than English
lrelease QOwnNotes.pro

# prepare build process and build the application
qmake
make
```

Please feel free to contribute source code to this project, make suggestions or report troubles on the [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues).

Visit [QOwnNotes on GitHub](https://github.com/pbek/QOwnNotes).

::: tip
If you want to contribute source code to the project please make your pull requests to the  `main` branch.
:::

## Archive source

You will find source archives of QOwnNotes on [QOwnNotes source archive](https://github.com/pbek/QOwnNotes/releases).

The source archives will be accessible like that:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`
