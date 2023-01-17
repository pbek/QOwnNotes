# Installer sur Debian Linux

## Debian 11.0

Exécutez les commandes shell suivantes pour approuver le dépôt.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Exécutez les commandes shell suivantes pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
Si vous utilisez ce dépôt pour d'autres versions de Debian Linux, veuillez vous assurer que vous avez installé **Qt version 5.11** ou plus récent.
:::

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11)

## Debian 10.0

Exécutez les commandes shell suivantes pour approuver le dépôt.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/Release.key -O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Si vous utilisez ce dépôt pour d'autres versions de Debian Linux, veuillez vous assurer que vous avez installé **Qt version 5.8** ou plus récent.
:::

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10)

## Debian 9.0

Exécutez les commandes shell suivantes pour approuver le dépôt.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/Release.key -O - | sudo apt-key add -
```

Exécutez les commandes shell suivantes pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Si vous utilisez ce dépôt pour d'autres versions de Debian Linux, veuillez vous assurer que vous avez installé **Qt version 5.5** ou plus récent.
:::

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0)
