# Install on Ubuntu Linux, elementary OS and Linux Mint

## Ubuntu Linux 18.04 or newer, elementary OS, Linux Mint 18 or newer

Install **QOwnNotes** on **Ubuntu Linux** (minimum 18.04) using the PPA repository.

Note that there are also packages for ARM in the repository, in case you want to use QOwnNotes on a **Raspberry Pi**.

Open a terminal and enter the following lines to add the repository and install QOwnNotes.

```bash
sudo add-apt-repository ppa:pbek/qownnotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Direct Download](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes/+packages)

## QOwnNotes Qt6 Preview

If you already want to try the Qt6 packages and are on Ubuntu 22.04 or newer you can use this repository:

```bash
# Remove Qt5 PPA in case you had it installed
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Add Qt6 PPA
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Install QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

[Direct Download](https://launchpad.net/~pbek/+archive/ubuntu/qownnotes-qt6/+packages)

문제가 발견되면 [GitHub의 QOWNNotes 문제](https://github.com/pbek/QOwnNotes/issues)에 보고하십시오!

## Older Ubuntu Linux distributions and derivatives

[GitHub에서 QOWNNotes 릴리즈](https://github.com/pbek/QOwnNotes/releases)의 AppImage를 사용하는 것이 좋습니다.

그런 다음 파일에 대한 실행 권한을 변경할 수 있습니다:

```bash
chmod a+x QOwnNotes-*.AppImage
```

나중에 앱 이미지를 실행하여 QOnNotes를 실행할 수 있습니다.

## OBS Repository

또한 Ubuntu Launchpad에서 더 이상 업데이트되지 않은 Ubuntu 버전에서 QOwnNotes를 사용할 수 있으며 [Open Build Service](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop)에서 저장소를 사용할 수 있습니다.

다음은 xUbuntu 16.10 저장소에 대한 호출 예제입니다.

다음 셸 명령을 실행하여 리포지토리를 신뢰합니다.

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/Release.key -O - | sudo apt-key add -
```

다음 셸 명령을 실행하여 리포지토리를 추가하고 여기서 QOwnNotes를 설치합니다.

```bash
sudo su -
sh -c "echo 'deb http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10/ /' >> /etc/apt/sources.list.d/qownnotes.list"
apt-get update
apt-get install qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/xUbuntu_16.10)
