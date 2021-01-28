# Construyendo QOwnNotes

## Construyendo desde GitHub

También puede crear QOwnNotes usted mismo a partir del código fuente.

Descarga el código directamente desde el repositorio git:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b master
cd QOwnNotes
git submodule update --init
```

Luego descargue [Qt Creator](https://www.qt.io/download-open-source), abra el archivo del proyecto `src / QOwnNotes.pr` y haga clic en **Build** / **BuildProject QOwnNotes** .

O puede construirlo directamente en su terminal:

```bash
cd src
qmake
make
```

No dude en contribuir con el código fuente a este proyecto, hacer sugerencias o informar problemas en la [página de problemas de QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

Visite [QOwnNotes en GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Si desea contribuir con el código fuente al proyecto, haga sus solicitudes de extracción a la rama `desarrollar`.
:::

## Archivo fuente

Encontrará archivos fuente de QOwnNotes en [archivo fuente QOwnNotes](https://download.tuxfamily.org/qownnotes/src/).

Los archivos de origen serán accesibles así:

`https://download.tuxfamily.org/qownnotes/src/qownnotes-20.10.1.tar.xz`
