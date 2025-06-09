# Construir QOwnNotes

## Construyendo desde GitHub

También puede crear QOwnNotes usted mismo a partir del código fuente.

Obtenga el código del último lanzamiento directamente desde el repositorio de git:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

Para la depuración, es más fácil usar QtCreator. Al descargar [Qt Creator](https://www.qt.io/download-qt-installer-oss), también necesitará el paquete `qtwebsockets` para construir QOwnNotes con Qt6. Si lo construye en Windows, querrá ceñirse a _MinGw de 64-bits_. En Linux, lo más probable es que tenga un paquete para QtCreator en su repositorio.

Posteriormente, abra el archivo de proyecto `src/QOwnNotes.pro` y haga clic en **Construir** / **Construir Proyecto QOwnNotes**.

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

Encontrará los archivos fuente de QOwnNotes en el [archivo fuente de QOwnNotes](https://github.com/pbek/QOwnNotes/releases).

Los archivos fuente serán accesibles de esta forma:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`

## Construir con Nix

Si tiene el [gestos de paquetes de Nix](https://nixos.org/download/) disponible puede simplemente clonar el código fuente con git-clone, como se explicó anteriormente, y construirlo con:

```bash
# Obtener devenv si aún no lo tiene
nix-shell -p devenv

# Abrir una consola con todas las herramientas que necesita
devenv shell

# Construir la aplicación
just nix-build

# Ejecutar la aplicación construida
just nix-run
```
