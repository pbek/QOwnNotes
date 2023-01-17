# Installa su Debian Linux

## Debian 11.0

Eseguire i seguenti comandi della shell per considerare attendibile il repository.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Eseguire i seguenti comandi della shell per aggiungere il repository e installare QOwnNotes da lì.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
Se usi questo repository per altre versioni di Debian Linux, assicurati di avere **Qt** installato almeno alla **versione 5.11**.
:::

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11)

## Debian 10.0

Eseguire i seguenti comandi della shell per considerare attendibile il repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/Release.key -O - | sudo apt-key add -
```

Eseguire i seguenti comandi della shell per aggiungere il repository e installare QOwnNotes da lì.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Se usi questo repository per altre versioni di Debian Linux, assicurati di avere **Qt** installato almeno alla **versione 5.8**.
:::

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10)

## Debian 9.0

Eseguire i seguenti comandi della shell per considerare attendibile il repository.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/Release.key -O - | sudo apt-key add -
```

Eseguire i seguenti comandi della shell per aggiungere il repository e installare QOwnNotes da lì.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Se usi questo repository per altre versioni di Debian Linux, assicurati di avere **Qt** installato almeno alla **versione 5.5**.
:::

[Download diretto](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0)
