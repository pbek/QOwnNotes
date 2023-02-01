# Compiler QOwnNotes

## Compiler à partir de GitHub

Vous pouvez également compiler vous-même QOwnNotes à partir du code source.

Récupérez le code directement depuis le dépôt git :

```bash
git clone https://github.com/pbek/QOwnNotes.git -b master
cd QOwnNotes
git submodule update --init
```

Téléchargez ensuite [Qt Creator](https://www.qt.io/download-open-source), ouvrez le fichier projet `src/QOwnNotes.pro` et cliquez sur **Build** / **Build Project QOwnNotes** .

Vous pouvez également compiler directement depuis le terminal :

```bash
cd src
qmake
make
```

N'hésitez pas à contribuer au code source de ce projet, à faire des suggestions ou à signaler des problèmes sur la [page des problèmes de QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

Visitez [QOwnNotes sur GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Si vous souhaitez contribuer au code source du projet, veuillez envoyer vos requêtes d'extraction à la branche `main`.
:::

## Archive source

Vous trouverez les archives source de QOwnNotes sur [archive source QOwnNotes](https://download.tuxfamily.org/qownnotes/src/).

Les archives sources seront accessibles sous cette forme :

`https://download.tuxfamily.org/qownnotes/src/qownnotes-20.10.1.tar.xz`
