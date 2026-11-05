# Building QOwnNotes

## Building from GitHub

You can also build QOwnNotes yourself from the source code.

Current sources require **Qt 5.13 or newer**, or **Qt 6.0 or newer**. Qt 5.13.2
is the oldest version covered by the build CI; qmake and CMake reject older Qt 5 versions.

Checkout the code of the last release directly from the git repository:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

For debugging, it's easiest to use QtCreator. When downloading
[Qt Creator](https://www.qt.io/download-qt-installer-oss), you will also
need the package `qtwebsockets` to build QOwnNotes with Qt6. If you build under Windows, you want to stick to
_MinGw 64-bit_. On Linux, you most likely have a package for QtCreator in your repository.

Afterward open the project file `src/QOwnNotes.pro` and click on
**Build** / **Build Project QOwnNotes**.

If you know what you are doing, you can also build QOwnNotes directly in your terminal:

```bash
cd src

# build binary translation files if you want another language than English
lrelease QOwnNotes.pro

# prepare build process and build the application
qmake
make
```

Please feel free to contribute source code to this project, make suggestions or
report troubles on the [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues).

Visit [QOwnNotes on GitHub](https://github.com/pbek/QOwnNotes).

::: tip
If you want to contribute source code to the project please make your pull requests
to the `main` branch. But please make sure to read the
[code contribution guidelines](../contributing/code-contributions.md) first.
:::

## Source archive

You will find source archives of QOwnNotes on [QOwnNotes source archive](https://github.com/pbek/QOwnNotes/releases).

The source archives will be accessible like that:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`

## Building with nix

If you have the [Nix package manager](https://nixos.org/download/) and [Devenv](https://devenv.sh/getting-started/)
installed you can just git-clone the repository, like explained above and build with:

```bash
# Open a shell with all the tools you need
devenv shell

# Build the application
just nix-build

# Run the built application
just nix-run
```
