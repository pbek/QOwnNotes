# Building QOwnNotes

## Bouwen vanuit GitHub

U kunt QOwnNotes ook zelf bouwen vanuit de broncode.

Check de code rechtstreeks uit de git-repository:

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

Bezoek [QOwnNotes op GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Als u broncode aan het project wilt bijdragen, dient u uw pull-verzoeken in naar de `main` tak.
:::

## Bronarchief

U vindt bronarchieven van QOwnNotes op [QOwnNotes bronarchief](https://download.tuxfamily.org/qownnotes/src/).

De bronarchieven zullen als volgt toegankelijk zijn:

`https://download.tuxfamily.org/qownnotes/src/qownnotes-20.10.1.tar.xz`
