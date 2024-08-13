# QOwnNotes bouwen

## Bouwen vanuit GitHub

U kunt QOwnNotes ook zelf bouwen vanuit de broncode.

Check de code rechtstreeks uit de git-repository:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

Voor foutopsporing is het het gemakkelijkst om QtCreator te gebruiken. Met het downloaden van [Qt Creator](https://www.qt.io/download-qt-installer-oss), heb je ook het pakket `qtwebsockets` nodig om QOwnNotes te bouwen met Qt6. Als je onder Windows bouwt, blijf dan bij *MinGw 64-bit*. Op Linux heb je waarschijnlijk een pakket voor QtCreator in je repository.

Open daarna het projectbestand `src/QOwnNotes.pro` en klik op **Build** / **Build Project QOwnNotes**.

Als u weet wat u doet, kunt u QOwnNotes ook rechtstreeks in uw terminal bouwen:

```bash
cd src

# build binary translation files if you want another language than English
lrelease QOwnNotes.pro

# prepare build process and build the application
qmake
make
```

Voel je vrij om broncode aan dit project bij te dragen, suggesties te doen of meld problemen met de [QOwnNotes issues page](https://github.com/pbek/QOwnNotes/issues).

Bezoek [QOwnNotes op GitHub](https://github.com/pbek/QOwnNotes).

::: tip
Als u broncode aan het project wilt bijdragen, dient u uw pull-verzoeken in naar de `main` tak.
:::

## Bronarchief

U kunt de broncode-archieven van QOwnNotes vinden op [QOwnNotes source archive](https://github.com/pbek/QOwnNotes/releases).

De bronarchieven zullen als volgt toegankelijk zijn:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`
