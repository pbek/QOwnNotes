# Ubuntu Linux, elementary OS 및 Linux Mint에 설치

## Ubuntu Linux 18.04 이상, 기본 OS, Linux Mint 18 이상

PPA 저장소를 사용하여 **Ubuntu Linux** (최소 18.04)에 **QOwnNotes**를 설치합니다.

**Raspberry Pi**에서 QOnNotes를 사용하려는 경우 저장소에 ARM용 패키지도 있습니다.

터미널을 열고 다음 행을 입력하여 저장소를 추가하고 QOwnNotes를 설치합니다.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[직접 다운로드](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## QOwnNotes Qt6 미리보기

If you already want to try the Qt6 packages, built with cmake and are on Ubuntu 24.04 or newer, you can use this repository:

```bash
# Qt5 PPA를 설치한 경우 제거
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Qt6 PPA 추가
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# QOwnNotes 설치
sudo apt-get update
sudo apt-get install qownnotes
```

[직접 다운로드](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

If you find any issues, please report them on [QOwnNotes Issues on GitHub](https://github.com/pbek/QOwnNotes/issues)!

## 이전 Ubuntu Linux 배포 및 파생 제품

[GitHub에서 QOwnNotes 릴리즈](https://github.com/pbek/QOwnNotes/releases)의 AppImage를 사용하는 것이 좋습니다.

그런 다음 파일에 대한 실행 권한을 변경할 수 있습니다:

```bash
chmod a+x QOwnNotes-*.AppImage
```

나중에 앱 이미지를 실행하여 QOwnNotes를 실행할 수 있습니다.

## OBS 저장소

You may also be able to use the QOwnNotes on Ubuntu versions that aren't anymore updated on Ubuntu Launchpad then you can use the repositories from the [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop). The packages for Ubuntu 24.04+ are built with cmake and Qt6.

Below are example calls for the xUbuntu 24.04 repository.

다음 셸 명령을 실행하여 리포지토리를 신뢰합니다.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/Release.key -O - | sudo apt-key add -
```

다음 셸 명령을 실행하여 저장소를 추가하고 여기서 QOwnNotes를 설치합니다.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_24.04)
