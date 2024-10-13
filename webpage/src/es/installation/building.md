# Construyendo QOwnNotes

## Construyendo desde GitHub

También puede crear QOwnNotes usted mismo a partir del código fuente.

Obtenga el código directamente desde el repositorio de git:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

For debugging, it's easiest to use QtCreator. When downloading [Qt Creator](https://www.qt.io/download-qt-installer-oss), you will also need the package `qtwebsockets` to build QOwnNotes with Qt6. If you build under Windows, you want to stick to *MinGw 64-bit*. On Linux, you most likely have a package for QtCreator in your repository.

Afterward open the project file `src/QOwnNotes.pro` and click on **Build** / **Build Project QOwnNotes**.

Si sabe lo que está haciendo, también puede construir QOwnNotes directamente desde su terminal:

```bash
cd src

# construir los archivos binarios de traducción si desea algún idioma distinto al inglés
lrelease QOwnNotes.pro

# preparar el proceso de construcción y construir la aplicación
qmake
make
```

No dude en contribuir con el código fuente a este proyecto, realizar sugerencias o informar problemas en la [página de problemas de QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

Visitar [QOwnNotes en GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Si desea contribuir código fuente al proyecto, por favor haga sus pull requests en la rama `main`.
:::

## Archivo fuente

You will find source archives of QOwnNotes on [QOwnNotes source archive](https://github.com/pbek/QOwnNotes/releases).

The source archives will be accessible like that:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`
