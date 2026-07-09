# Installer sur Debian Linux

## Debian Unstable

Exécutez les commandes shell suivantes pour approuver le dépôt. Vous pourriez devoir répéter ces étapes quand la clé expirera.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_Unstable/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Exécutez les commandes shell suivantes pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_Unstable/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
Ce paquet est compilé avec Qt6.
:::

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_Unstable)

## Debian 13.0

Exécutez les commandes shell suivantes pour approuver le dépôt. Vous pourriez devoir répéter ces étapes quand la clé expirera.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_13/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Exécutez les commandes shell suivantes pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_13/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
Ce paquet est compilé avec Qt6.
:::

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_13)

## Debian 12.0

Exécutez les commandes shell suivantes pour approuver le dépôt. Vous pourriez devoir répéter ces étapes quand la clé expirera.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_12/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Exécutez les commandes shell suivantes pour ajouter le dépôt et installer QOwnNotes à partir de là.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_12/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
Ce paquet est compilé avec Qt6.
:::

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_12)

## Debian 11.0

Exécutez les commandes shell suivantes pour approuver le dépôt. Vous pourriez devoir répéter ces étapes quand la clé expirera.

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
Si vous utilisez ce dépôt pour d'autres versions de Debian Linux, veuillez vous assurer que vous avez installé **Qt** dans sa **version 5.11** ou plus récent.
:::

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11)

## Debian 10.0

Exécutez les commandes shell suivantes pour approuver le dépôt. Vous pourriez devoir répéter ces étapes quand la clé expirera.

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
Si vous utilisez ce dépôt pour d'autres versions de Debian Linux, veuillez vous assurer que vous avez installé **Qt** dans sa **version 5.8** ou plus récent.
:::

[Téléchargement direct](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10)

::: tip
Si QOwnNotes affiche `Could not write secret to keychain`, installez les paquets Secret Service manquants et redémarrez votre session de bureau.

Pour GNOME et les autres environnements reposant sur Secret Service :

```bash
sudo apt install gnome-keyring libsecret-1-0 seahorse
```

Pour KDE Plasma :

```bash
sudo apt install kwalletmanager
```

QOwnNotes repassera sur le chiffrement obsolète si le porte-clés de l’environnement n’est pas disponible.
:::
