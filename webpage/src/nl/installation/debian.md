# Installeer op Debian Linux

## Debian 11.0

Voer de volgende shell-opdrachten uit om de repository te vertrouwen.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Voer de volgende shell-opdrachten uit om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
Als u deze opslagplaats voor andere Debian Linux-versies gebruikt, zorg er dan voor dat u **Qt** op zijn minst **versie 5.11** hebt geïnstalleerd.
:::

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11)

## Debian 10.0

Voer de volgende shell-opdrachten uit om de repository te vertrouwen.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/Release.key -O - | sudo apt-key add -
```

Voer de volgende shell-opdrachten uit om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Als u deze opslagplaats voor andere Debian Linux-versies gebruikt, zorg er dan voor dat u **Qt** op zijn minst **versie 5.8** hebt geïnstalleerd.
:::

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10)

## Debian 9.0

Voer de volgende shell-opdrachten uit om de repository te vertrouwen.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/Release.key -O - | sudo apt-key add -
```

Voer de volgende shell-opdrachten uit om de repository toe te voegen en vanaf daar QOwnNotes te installeren.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Als u deze opslagplaats voor andere Debian Linux-versies gebruikt, zorg er dan voor dat u **Qt** op zijn minst **versie 5.5** hebt geïnstalleerd.
:::

[Directe download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0)
