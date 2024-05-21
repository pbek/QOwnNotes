# Building QOwnNotes

## Bouwen vanuit GitHub

U kunt QOwnNotes ook zelf bouwen vanuit de broncode.

Check de code rechtstreeks uit de git-repository:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

For debugging, it's easiest to use QtCreator. When downloading [Qt Creator](https://www.qt.io/download-qt-installer-oss), you will also need the packages `qtwebsockets` and `qt5compat` to build QOwnNotes with Qt6 (only `qtwebsockets` for Qt5). If you build under Windows, you want to stick to *MinGw 64-bit*. On Linux, you most likely have a package for QtCreator in your repository.

Afterward open the project file `src/QOwnNotes.pro` and click on **Build** / **Build Project QOwnNotes**.

If you k now what you are doing, you can also build QOwnNotes directly in your terminal:

```bash
cd src

# build binary translation files if you want another language than English
lrelease QOwnNotes.pro

# prepare build process and build the application
qmake
make
```

Please feel free to contribute source code to this project, make suggestions or report troubles on the [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues).

Bezoek [QOwnNotes op GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Als u broncode aan het project wilt bijdragen, dient u uw pull-verzoeken in naar de `main` tak.
:::

## Bronarchief

U kunt de broncode-archieven van QOwnNotes vinden op [QOwnNotes source archive](https://github.com/pbek/QOwnNotes/releases).

De bronarchieven zullen als volgt toegankelijk zijn:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`
