# Building QOwnNotes

## Building from GitHub

You can also build QOwnNotes yourself from the source code.

Checkout the code directly from the git repository:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b master
cd QOwnNotes
git submodule update --init
```

Then download [Qt Creator](https://www.qt.io/download-open-source), open the project file `src/QOwnNotes.pro`
and click on **Build** / **Build Project QOwnNotes**.

Or you can build it directly in your terminal:

```bash
cd src
qmake
make
```

Please feel free to contribute source code to this project, make suggestions or
report troubles on the [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues).

Visit [QOwnNotes on GitHub](https://github.com/pbek/QOwnNotes).

::: tip
If you want to contribute source code to the project please make your pull requests
to the  `develop` branch.
:::

## Source archive

You will find source archives of QOwnNotes on [QOwnNotes source archive](https://download.tuxfamily.org/qownnotes/src/).

The source archives will be accessible like that:

`https://download.tuxfamily.org/qownnotes/src/qownnotes-20.10.1.tar.xz`
