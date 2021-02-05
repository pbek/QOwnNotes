# Building QOwnNotes

## Bouwen vanuit GitHub

U kunt QOwnNotes ook zelf bouwen vanuit de broncode.

Check de code rechtstreeks uit de git-repository:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b master
cd QOwnNotes
git submodule update --init
```

Download vervolgens [Qt Creator](https://www.qt.io/download-open-source), open het projectbestand `src/QOwnNotes.pro` en klik op **Build** / **Build Project QOwnNotes**.

Of u kunt het rechtstreeks in uw terminal bouwen:

```bash
cd src
qmake
make
```

Voel je vrij om broncode bij te dragen aan dit project, suggesties te doen of problemen te melden op de [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues).

Bezoek [QOwnNotes op GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Als u broncode aan het project wilt bijdragen, dient u uw pull-verzoeken in bij de `develop` branch.
:::

## Bronarchief

U vindt bronarchieven van QOwnNotes op [QOwnNotes source archive](https://download.tuxfamily.org/qownnotes/src/).

De bronarchieven zullen als volgt toegankelijk zijn:

`https://download.tuxfamily.org/qownnotes/src/qownnotes-20.10.1.tar.xz`
