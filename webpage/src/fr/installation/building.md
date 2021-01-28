# Bâtiment QOwnNotes

## Construire à partir de GitHub

Vous pouvez également créer vous-même QOwnNotes à partir du code source.

Récupérez le code directement depuis le référentiel git:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b master
cd QOwnNotes
git submodule update --init
```

Ensuite, téléchargez [Qt Creator](https://www.qt.io/download-open-source), ouvrez le fichier projet `src/QOwnNotes.pro` et cliquez sur **Build** / **Build Project QOwnNotes** .

Ou vous pouvez le construire directement dans votre terminal:

```bash
cd src
qmake
make
```

N'hésitez pas à contribuer au code source de ce projet, à faire des suggestions ou à signaler des problèmes sur la [page des problèmes de QOwnNotes](https://github.com/pbek/QOwnNotes/issues).

Visitez [QOwnNotes sur GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Si vous souhaitez contribuer au code source du projet, veuillez envoyer vos requêtes d'extraction à la branche `develop`.
:::

## Archive source

Vous trouverez les archives source de QOwnNotes sur [archive source QOwnNotes](https://download.tuxfamily.org/qownnotes/src/).

Les archives sources seront accessibles comme ça:

`https://download.tuxfamily.org/qownnotes/src/qownnotes-20.10.1.tar.xz`
