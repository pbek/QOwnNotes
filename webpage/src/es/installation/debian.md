# Instalar en Debian Linux

## Debian Inestable

Ejecute las órdenes de consola siguientes para marcar el repositorio como de fiar.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_Unstable/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Ejecute las órdenes de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_Unstable/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
Este paquete está construido con Qt6.
:::

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_Unstable)

## Debian 13.0

Ejecute las órdenes de consola siguientes para marcar el repositorio como de fiar.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_13/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Ejecute las órdenes de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_13/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
Este paquete está construido con Qt6.
:::

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_13)

## Debian 12.0

Ejecute las órdenes de consola siguientes para marcar el repositorio como de fiar.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_12/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Ejecute las órdenes de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_12/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
Este paquete está construido con Qt6.
:::

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_12)

## Debian 11.0

Ejecute las órdenes de consola siguientes para marcar el repositorio como de fiar.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

Ejecute las órdenes de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: tip
Si utiliza este repositorio para otras versiones de Debian Linux, por favor asegúrese de tener **Qt** instalado al menos en la **versión 5.11**.
:::

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11)

## Debian 10.0

Ejecute las órdenes de consola siguientes para marcar el repositorio como de fiar.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/Release.key -O - | sudo apt-key add -
```

Ejecute las órdenes de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Si utiliza este repositorio para otras versiones de Debian Linux, por favor asegúrese de tener **Qt** instalado al menos en la **versión 5.8**.
:::

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10)

## Debian 9.0

Ejecute las órdenes de consola siguientes para marcar el repositorio como de fiar.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/Release.key -O - | sudo apt-key add -
```

Ejecute las órdenes de consola siguientes para añadir el repositorio e instalar QOwnNotes desde allí.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: tip
Si utiliza este repositorio para otras versiones de Debian Linux, por favor asegúrese de tener **Qt** instalado al menos en la **versión 5.5**.
:::

[Descarga directa](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0)
