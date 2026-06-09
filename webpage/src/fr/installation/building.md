# Compiler QOwnNotes

## Compiler à partir de GitHub

Vous pouvez également compiler vous-même QOwnNotes à partir du code source.

Récupérez le code de la dernière version directement depuis le dépôt git :

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

Pour le débogage, il est plus simple d’utiliser QtCreator. Lors du téléchargement de [Qt Creator](https://www.qt.io/download-qt-installer-oss), vous aurez aussi besoin du paquet `qtwebsockets` pour compiler QOwnNotes avec Qt6. Si vous compilez sous Windows, vous devrez utiliser _MinGw 64-bits_. Sous Linux, vous devriez normalement avoir un paquet pour QtCreator dans votre dépôt.

Ouvrez ensuite le fichier de projet `src/QOwnNotes.pro` et cliquez sur **Compiler** / **Compiler le projet QOwnNotes**.

Si vous savez ce que vous faites, vous pouvez aussi compiler QOwnNotes directement dans votre terminal :

```bash
cd src

# compiler les fichiers de traductions si vous souhaitez d’autres langues que l’anglais
lrelease QOwnNotes.pro

# préparer le processus de compilation puis compiler l’application
qmake
make
```

N'hésitez pas à contribuer au code source de ce projet, à faire des suggestions ou à signaler des problèmes sur la [page des issues de QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

Voir [QOwnNotes sur GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Si vous souhaitez contribuer au code source du projet, veuillez envoyer vos requêtes de fusion à la branche `main`. Assurez-vous d’abord de lire les [bonnes pratiques à la contribution au code](../contributing/code-contributions.md).
:::

## Archive source

Vous trouverez les archives source de QOwnNotes sur [QOwnNotes source archive](https://github.com/pbek/QOwnNotes/releases).

Les archives source seront accessibles via des liens tels que :

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`

## Compiler avec nix

Si vous avez le [gestionnaire de paquets Nix](https://nixos.org/download/) et [Devenv](https://devenv.sh/getting-started/) installés, vous pouvez simplement cloner le dépôt git comme expliqué plus haut puis compiler avec :

```bash
# Ouvrir un shell avec tous les outils nécessaires
devenv shell

# Compiler l’application
just nix-build

# Exécuter l’application compilée
just nix-run
```
