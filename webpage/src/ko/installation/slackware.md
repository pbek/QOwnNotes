# Slackware Linux에 설치

## Slackware 14.2

먼저 Qt 5, libxkb common, libproxy 및 js185를 설치해야 합니다.

```bash
cd /tmp
wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/qt5/qt5-5.6.1_1-x86_64-1alien.txz"
wget http://bear.alienbase.nl/mirrors/people/alien/sbrepos/current/x86_64/libxkbcommon/libxkbcommon-0.5.0-x86_64-1alien.txz
sudo installpkg libxkbcommon-0.5.0-x86_64-1alien.txz qt5-5.6.1_1-x86_64-1alien.txz
slackpkg update
slackpkg install libproxy js185-1.0.0-x86_64-1
```

그런 다음 QOwnNotes slackbuilds repository를 체크아웃하고 응용프로그램을 빌드할 수 있습니다.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.2/qownnotes
./dobuild.sh
```

그런 다음 생성된 패키지를 `installpkg`로 설치할 수 있습니다.

## Slackware 14.1

먼저 Qt 5를 설치해야 합니다.

```bash
cd /tmp wget "http://bear.alienbase.nl/mirrors/people/alien/sbrepos/14.1/x86_64/qt5/qt5-5.5.1-x86_64-4alien.txz"
sudo installpkg qt5-5.5.1-x86_64-4alien.txz
```

그런 다음 QOwnNotes slackbuilds repository를 체크아웃하고 응용프로그램을 빌드할 수 있습니다.

```bash
cd /tmp
git clone https://github.com/pbek/qownnotes-slackbuilds.git
cd qownnotes-slackbuilds/14.1/qownnotes
./dobuild.sh
```

그런 다음 생성된 패키지를 `installpkg`로 설치할 수 있습니다.

[GitHub의QOwnNotes Slackbuild](https://github.com/pbek/qownnotes-slackbuilds/)
