# Installer sur Microsoft Windows™

## Installer

Download the latest **Windows installer** from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) (look for a file called `QOwnNotes.msi`) and double-click it to install QOwnNotes.

QOwnNotes will be installed for your user account into `%LOCALAPPDATA%\Programs\QOwnNotes`, so no administration permissions are needed and the **automatic updater** will work. A shortcut will be added to the start menu.

## ZIP archive

Alternatively, download the latest **Windows archive** from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) (look for a file called `QOwnNotes.zip`) and unzip it to anywhere you like. Aucune installation n'est nécessaire!

::: warning
Si vous souhaitez utiliser le **programme de mise à jour automatique**, assurez-vous de le décompresser dans un endroit où votre compte utilisateur a un accès en écriture. Par défaut, votre compte utilisateur **n'a probablement pas d'accès en écriture** à des emplacements tels que `C:\Program Files (x86) ` ou `C:\Program Files`.
:::

Vous pouvez ensuite exécuter directement `QOwnNotes.exe` à partir de votre dossier `QOwnNotes`, aucune installation n'est requise.

### Mode portable

Utilisez `QOwnNotesPortable.bat` pour exécuter QOwnNotes en **mode portable** dans lequel tout (y compris vos notes) ne sera stocké que dans votre dossier `QOwnNotes`.

::: tip
Vous n'avez pas absolument besoin du mode portable si vous ne disposez juste pas des autorisations d'administration sur votre ordinateur. QOwnNotes n'a pas besoin d'être installé !
:::

## Windows 8.1 et ultérieur

La version pour Windows est compilée avec **Qt6** et nécessite au moins **Windows 10**.

L’ancienne version avec Qt 5.7 pour Windows XP à Windows 8.1 n’est plus mis à jour. La version finale est **QOwnNotes 26.10.0**, que vous pouvez télécharger avec [QOwnNotes-Qt5.7-legacy.zip](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip) ([somme de contrôle SHA-256](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip.sha256sum)) depuis la [page GitHub de la version 26.10.0](https://github.com/pbek/QOwnNotes/releases/tag/v26.10.0).

Décompressez-le vers un dossier de votre préférence et exécutez `QOwnNotes.exe` depuis ce dossier, aucune installation n’est requise.

::: tip
Info
Le mécanisme de mise à jour automatique ne fonctionne pas avec cette ancienne version !
:::

## Chocolatey

Il existe un paquet [Chocolatey](https://chocolatey.org/packages/qownnotes/) de QOwnNotes géré par la communauté.

Vous pouvez l'installer en tapant :

```shell
choco install qownnotes
```

## Scoop

Il existe un paquet [paquet QOwnNotes maintenun par la communauté](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) sur [Scoop](https://scoop.sh/). Si vous ajoutez le bucket Extras, vous pourrez l'utiliser pour installer QOwnNotes en mode portable.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```

## WinGet

Il existe un [paquet QOwnNotes géré par la communauté](https://github.com/microsoft/winget-pkgs/tree/master/manifests/p/pbek/QOwnNotes) sur [WinGet](https://github.com/microsoft/winget-cli).

Vous pouvez l'installer en tapant :

```shell
winget install qownnotes
```

## MSYS2

Il existe un [paquet QOwnNotes géré par la communauté](https://packages.msys2.org/base/mingw-w64-qownnotes) pour [MSYS2](hhttps://www.msys2.org/).

Vous pouvez l'installer en tapant :

```shell
pacman -S mingw-w64-qownnotes
```
