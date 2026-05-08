# Installer sur Microsoft Windows™

Téléchargez la dernière **archive Windows** à partir de [la pages des Releases de QOwnNotes sur GitHub](https://github.com/pbek/QOwnNotes/releases) (cherchez le fichier nommé `QOwnNotes.zip`) et décompressez-la où vous le souhaitez. Aucune installation n'est nécessaire!

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

Qt a abandonné la prise en charge de Windows XP avec la version 5.8, mais QOwnNotes est désormais également compilé avec Qt 5.7 pour permettre aux utilisateurs de Windows XP de continuer à l'utiliser. Windows 8.1 a des problèmes avec le build normal de QOwnNotes, parce qu’il a été compilé sur une version du Windows SDK plus récente.

Vous devrez vous-même télécharger le fichier ZIP d'[AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts) et le décompresser dans le dossier de votre choix.

Vous pouvez ensuite exécuter directement `QOwnNotes.exe` depuis ce dossier, aucune installation n'est requise.

::: tip
Info
Le mécanisme de mise à jour automatique ne fonctionne pas avec la version d'AppVeyor pour Windows XP !
Vous devrez télécharger vous-même les nouvelles versions.
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
