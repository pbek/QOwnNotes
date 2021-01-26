# A QOwnNotes építése

## A GitHubról épít

A forráskódból saját maga is elkészítheti a QOwnNotes programot.

Ellenőrizze a kódot közvetlenül a git tárházból:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b master
cd QOwnNotes
git submodule update --init
```

Ezután töltse le a [Qt Creator](https://www.qt.io/download-open-source) t, nyissa meg a `src/QOwnNotes.pro` projektfájlt, és kattintson a **Build** / **Project QOwnNotes build** elemre. .

Vagy építheti közvetlenül a terminálján:

```bash
cd src
qmake
make
```

Kérjük, bátran adjon hozzá forráskódot a projekthez, tegyen javaslatokat vagy jelentse a problémákat a [QOwnNotes kérdések oldalon](https://github.com/pbek/QOwnNotes/issues).

Látogassa meg: [QOwnNotes on GitHub](https://github.com/pbek/QOwnNotes).

::: tip Ha forráskóddal szeretne hozzájárulni a projekthez, kérjük, tegye meg a lekérési kérelmeket az `develop` ághoz. :::

## Forrásarchívum

A QOwnNotes forrásarchívumát a [QOwnNotes forrásarchívum](https://download.tuxfamily.org/qownnotes/src/)ban találja meg.

A forrásarchívum így lesz elérhető:

`https://download.tuxfamily.org/qownnotes/src/qownnotes-20.10.1.tar.xz`
