# Installer sur Debian Linux

## Debian 10.0

Exécutez les commandes shell suivantes pour approuver le référentiel.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/Release.key -O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Si vous utilisez ce référentiel pour d'autres versions de Debian Linux, assurez-vous que **Qt** est installé au moins à la **version 5.8**.
:::

[Téléchargement direct](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Debian_10)

## Debian 9.0

Exécutez les commandes shell suivantes pour approuver le référentiel.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/Release.key -O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Si vous utilisez ce référentiel pour d'autres versions de Debian Linux, veuillez vous assurer que **Qt** est installé au moins à la **version 5.5**.
:::

[Téléchargement direct](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Debian_9.0)

## Debian 8.0

Exécutez les commandes shell suivantes pour approuver le référentiel.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_8.0/Release.key -O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le référentiel et installer QOwnNotes à partir de là.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_8.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Si vous utilisez ce référentiel pour d'autres versions de Debian Linux, veuillez vous assurer que **Qt** est installé au moins à la **version 5.3**.
:::

[Téléchargement direct](https://build.opensuse.org/package/binaries/home:pbek:QOwnNotes/desktop/Debian_8.0)
