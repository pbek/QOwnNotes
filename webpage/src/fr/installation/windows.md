# Installer sur Microsoft Windows™

Download the latest **Windows archive** from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) (look for a file called `QOwnNotes.zip`) and unzip it to anywhere you like. Aucune installation n'est nécessaire!

::: warning
Si vous souhaitez utiliser le **programme de mise à jour automatique**, assurez-vous de le décompresser dans un endroit où votre compte utilisateur a un accès en écriture. By default, your user account most likely **doesn't have write access** to places like `C:\Program Files (x86)` or `C:\Program Files`.
:::

Vous pouvez ensuite exécuter directement `QOwnNotes.exe` à partir de votre dossier `QOwnNotes`, aucune installation n'est requise.

### Mode portable

Utilisez `QOwnNotesPortable.bat` pour exécuter QOwnNotes en **mode portable** dans lequel tout (y compris vos notes) ne sera stocké que dans votre dossier `QOwnNotes`.

::: tip
Vous n'avez pas absolument besoin du mode portable si vous ne disposez pas des autorisations d'administration sur votre ordinateur. QOwnNotes n'a pas besoin d'être installé !
:::

## Windows XP

Qt a abandonné la prise en charge de Windows XP avec la version 5.8, mais QOwnNotes est désormais également compilé avec Qt 5.7 pour permettre aux utilisateurs de Windows XP de continuer à l'utiliser.

Vous devrez vous-même télécharger le fichier ZIP d'[AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) et le dézipper dans le dossier de votre choix.

Vous pouvez ensuite exécuter directement `QOwnNotes.exe` depuis ce dossier, aucune installation n'est requise.

::: tip
Info
Le mécanisme de mise à jour automatique ne fonctionne pas avec la version d'AppVeyor pour Windows XP !
Vous devrez télécharger vous-même les nouvelles versions.
:::

## Chocolatey

There is a community-maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

Vous pouvez l'installer en tapant :

```shell
choco install qownnotes
```

## Scoop

There is a [community-maintained package of QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) at [Scoop](https://scoop.sh/). If you add the Extras bucket, you can use it to install QOwnNotes in portable mode.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```

## WinGet

There is a [community-maintained package of QOwnNotes](https://github.com/microsoft/winget-pkgs/tree/master/manifests/p/pbek/QOwnNotes) for [WinGet](https://github.com/microsoft/winget-cli).

You can install it with:

```shell
winget install qownnotes
```

## MSYS2

There is a [community-maintained package of QOwnNotes](https://packages.msys2.org/base/mingw-w64-qownnotes) for [MSYS2](hhttps://www.msys2.org/).

You can install it with:

```shell
pacman -S mingw-w64-qownnotes
```
