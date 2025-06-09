# Ubuntu Linux, elementary OS 및 Linux Mint에 설치

## Ubuntu Linux 24.04 이상, elementary OS, Linux Mint 22.1 이상

PPA 저장소를 사용하여 **Ubuntu Linux** (최소 24.04)에 **QOwnNotes**을 설치합니다. 이 버전의 QOwnNotes는 Qt6으로 제작되었습니다.

**Raspberry Pi**에서 QOnNotes를 사용하려는 경우 저장소에 ARM용 패키지도 있습니다.

터미널을 열고 다음 행을 입력하여 저장소를 추가하고 QOwnNotes를 설치합니다.

```bash
# Qt5 PPA가 설치된 경우 제거
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Qt6 PPA 추가
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# QOwnNotes 설치
sudo apt-get update
sudo apt-get install qownnotes
```

[직접 다운로드](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

문제가 발견되면 [GitHub의 QOwnNotes 이슈](https://github.com/pbek/QOwnNotes/issues)에 대해 보고해 주세요!

## QOwnNotes Qt5 레거시

Ubuntu Linux의 이전 버전인 18.04 이상, 기본 OS, Linux Mint 18 이상의 경우 QOwnNotes의 Qt5 버전을 사용할 수 있습니다.

PPA 저장소를 사용하여 **Ubuntu Linux** (최소 18.04)에 **QOwnNotes**을 설치합니다.

저장소에는 **Raspberry Pi**에서 QOwnNotes를 사용하려는 경우를 대비한 ARM 패키지도 있습니다.

터미널을 열고 다음 줄을 입력하여 저장소를 추가하고 QOwnNotes를 설치합니다.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[직접 다운로드](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## 이전 Ubuntu Linux 배포 및 파생 제품

[GitHub에서 QOwnNotes 릴리스](https://github.com/pbek/QOwnNotes/releases)의 AppImage를 사용해 보세요.

자세한 내용은 [AppImage로 설치](./appimage.md)을 참조하세요.

## OBS 저장소

Ubuntu Launchpad에서 더 이상 업데이트되지 않은 Ubuntu 버전에서도 QOwnNotes를 사용할 수 있으며, 그 후에는 [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop)의 저장소를 사용할 수 있습니다. Ubuntu 24.04+용 패키지는 cmake와 Qt6로 제작되었습니다.

다음은 xUbuntu 24.04 리포지토리에 대한 예제 호출입니다.

다음 셸 명령을 실행하여 저장소를 신뢰합니다.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/Release.key -O - | sudo apt-key add -
```

다음 셸 명령을 실행하여 저장소를 추가하고 거기에서 QOwnNotes를 설치합니다.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04)
