# Install on Microsoft Windows™

Téléchargez la dernière **archive Windows** à partir de [QOwnNotes Releases sur GitHub](https://github.com/pbek/QOwnNotes/releases) et décompressez-la où vous le souhaitez. Aucune installation n'est nécessaire!

::: warning
Si vous souhaitez utiliser le **programme de mise à jour automatique**, assurez-vous de le décompresser dans un endroit où votre compte utilisateur a un accès en écriture. Par défaut, votre compte utilisateur **n'a probablement pas d'accès en écriture** à des emplacements tels que `C: \ Program Files (x86) ` ou `C:\Program Files`.
:::

Vous pouvez ensuite exécuter directement `QOwnNotes.exe` à partir de votre dossier `QOwnNotes`, aucune installation n'est requise.

### Mode portable

Utilisez `QOwnNotesPortable.bat` pour exécuter QOwnNotes en **mode portable** où tout (y compris vos notes) ne sera stocké que dans votre dossier `QOwnNotes`.

::: tip
Vous n'avez pas besoin du mode portable si vous ne disposez tout simplement pas des autorisations d'administration sur votre ordinateur. QOwnNotes n'a pas besoin d'être installé!
:::

## Windows XP

Qt a abandonné la prise en charge de Windows XP avec la version 5.8, mais QOwnNotes est désormais également construit avec Qt 5.7 pour permettre aux utilisateurs de Windows XP de continuer à l'utiliser.

Bien que vous deviez rechercher vous-même la dernière version principale de [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/history), choisissez l '*environnement Qt 5.7* et téléchargez l' *artefact*.

::: tip Info
Le mécanisme de mise à jour automatique ne fonctionne pas avec la version AppVeyor pour Windows XP! Vous devrez télécharger vous-même les nouvelles versions.
:::

## Chocolatey

Il existe un package de QOwnNotes géré par la communauté à [Chocolatey](https://chocolatey.org/packages/qownnotes/).

Vous pouvez l'installer avec:

```shell
choco install qownnotes
```
