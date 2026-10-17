# Instalar en Microsoft Windows ™

Descargue el **archivo de Windows**más reciente de los [lanzamientos de QOwnNotes en GitHub](https://github.com/pbek/QOwnNotes/releases) (busque un archivo llamado `QOwnNotes`) y descomprímalo en cualquier lugar que desee. ¡No se necesita instalación!

::: advertencia Si desea utilizar el **actualizador automático**, asegúrese de descomprimirlo en un lugar donde su cuenta de usuario tenga acceso de escritura. De forma pretederminada, su es probable que su cuenta de usuario **no tenga acceso de escritura** a lugares como `C:\Archivos de programa (x86)` o `C:\Archivos de programa`.
:::

Luego, puede ejecutar directamente `QOwnNotes.exe` desde su carpeta `QOwnNotes`, sin requerir ninguna instalación.

### Modo portátil

Utilice `QOwnNotesPortable.bat` para ejecutar QOwnNotes en **modo portátil**, donde todo (incluidas sus notas) solo se almacenará en su carpeta `QOwnNotes`.

::: tip
No necesita el modo portátil si simplemente no tiene permisos de administración en su computadora. ¡QOwnNotes no necesita ser instalado!
:::

## Windows 8.1 y anteriores

The Windows release is built with **Qt6** and needs at least **Windows 10**.

The legacy Qt 5.7 build for Windows XP to Windows 8.1 is no longer updated. The final version is **QOwnNotes 26.10.0**, which you can download as [QOwnNotes-Qt5.7-legacy.zip](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip) ([SHA-256 checksum](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip.sha256sum)) from the [26.10.0 release on GitHub](https://github.com/pbek/QOwnNotes/releases/tag/v26.10.0).

Unzip it to a folder of your liking and run `QOwnNotes.exe` from that folder, no installation is required.

::: tip
Info
The automatic update mechanism does not work with the legacy build!
:::

## Chocolatey

Hay un paquete de QOwnNotes mantenido por la comunidad en [Chocolatey](https://chocolatey.org/packages/qownnotes/).

Puede instalarlo con:

```shell
choco install qownnotes
```

## Scoop

Hay un [paquete de QOwnNotes mantenido por la comunidad](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) en [Scoop](https://scoop.sh/). Si agrega el bucket Extras, puede usarlo para instalar QOwnNotes en modo portátil.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```

## WinGet

Hay un [paquete de QOwnNotes mantenido por la comunidad](https://github.com/microsoft/winget-pkgs/tree/master/manifests/p/pbek/QOwnNotes) en [WinGet](https://github.com/microsoft/winget-cli).

Puede instalarlo con:

```shell
winget install qownnotes
```

## MSYS2

Hay un [paquete de QOwnNotes mantenido por la comunidad](https://packages.msys2.org/base/mingw-w64-qownnotes) en [MSYS2](hhttps://www.msys2.org/).

Puede instalarlo con:

```shell
pacman -S mingw-w64-qownnotes
```
