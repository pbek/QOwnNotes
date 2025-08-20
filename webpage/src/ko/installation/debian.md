# Debian Linux에 설치

## Debian 불안정

다음 셸 명령을 실행하여 저장소를 신뢰합니다.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_Unstable/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

다음 셸 명령을 실행하여 저장소를 추가하고 여기서 QOwnNotes를 설치합니다.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_Unstable/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: 팁
이 패키지는 Qt6으로 제작되었습니다.
:::

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_Unstable)

## Debian 13.0

다음 셸 명령을 실행하여 저장소를 신뢰합니다.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_13/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

다음 셸 명령을 실행하여 저장소를 추가하고 여기서 QOwnNotes를 설치합니다.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_13/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: 팁
이 패키지는 Qt6으로 제작되었습니다.
:::

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_13)

## Debian 12.0

다음 셸 명령을 실행하여 저장소를 신뢰합니다.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_12/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

다음 셸 명령을 실행하여 저장소를 추가하고 여기서 QOwnNotes를 설치합니다.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_12/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: 팁
이 패키지는 Qt6으로 제작되었습니다.
:::

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_12)

## Debian 11.0

저장소를 신뢰하려면 다음 셸 명령을 실행합니다.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
sudo mkdir -p "$(dirname "${SIGNED_BY}")"
curl --silent --show-error --location http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/Release.key | gpg --dearmor | sudo tee "${SIGNED_BY}" > /dev/null
sudo chmod u=rw,go=r "${SIGNED_BY}"
```

다음 셸 명령을 실행하여 리포지토리를 추가하고 여기서 QOwnNotes를 설치합니다.

```bash
SIGNED_BY='/etc/apt/keyrings/qownnotes.gpg'
ARCHITECTURE="$(dpkg --print-architecture)"
echo "deb [arch=${ARCHITECTURE} signed-by=${SIGNED_BY}] http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11/ /" | sudo tee /etc/apt/sources.list.d/qownnotes.list > /dev/null
sudo apt update
sudo apt install qownnotes
```

::: 팁 다른 데비안 리눅스 버전의 경우 이 저장소를 사용하는 경우에는 적어도 **버전 5.11** 이상의 **Qt**가 설치되어 있어야 합니다. :::

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_11)

## Debian 10.0

다음 셸 명령을 실행하여 저장소를 신뢰합니다.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/Release.key -O - | sudo apt-key add -
```

다음 셸 명령을 실행하여 리포지토리를 추가하고 거기에서 QOwnNotes를 설치합니다.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: 팁 다른 데비안 리눅스 버전의 경우 이 저장소를 사용하는 경우에는 적어도 **버전 5.8** 이상의 **Qt**가 설치되어 있어야 합니다. :::

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_10)

## Debian 9.0

다음 셸 명령을 실행하여 저장소를 신뢰합니다.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/Release.key -O - | sudo apt-key add -
```

다음 셸 명령을 실행하여 리포지토리를 추가하고 거기에서 QOwnNotes를 설치합니다.

```bash
sudo bash -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0/ /' >> /etc/apt/sources.list.d/qownnotes.list"
sudo apt-get update
sudo apt-get install qownnotes
```

::: 팁 다른 데비안 리눅스 버전의 경우 이 저장소를 사용하는 경우에는 적어도 **버전 5.5** 이상의 **Qt**가 설치되어 있어야 합니다. :::

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Debian_9.0)
