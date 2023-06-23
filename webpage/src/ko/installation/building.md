# QOwnNotes 빌드

## GitHub에서 빌드

소스 코드에서 직접 QOwnNotes를 빌드할 수도 있습니다.

Git 저장소에서 직접 코드를 체크아웃합니다:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

그런 다음 [Qt Creator](https://www.qt.io/download-open-source), 를 다운로드하면 Qt6 (Qt5의 경우 `qtwebsockets` 소켓만 해당) 로 QOwnNotes를 빌드하려면 패키지 `qtwebsockets` 과 `qt5compat`도 필요합니다. Windows에서 빌드할 경우 *MinGw 64-비트*를 고수해야 합니다.

그런 다음 프로젝트 파일 `src/QOwnNotes.pro`를 열고 **빌드** / **빌드 프로젝트 QOwnNotes**를 클릭합니다.

또는 터미널에서 직접 빌드할 수 있습니다:

```bash
cd src

# build binary translation files if you want another language than English
lrelease QOwnNotes.pro

# prepare build process and build the application
qmake
make
```

이 프로젝트에 소스 코드를 제공하거나, 제안을 하거나, [QOwnNotes 문제 페이지](https://github.com/pbek/QOwnNotes/issues)에서 문제를 보고할 수 있습니다.

[GitHub의 QOnNotes](https://github.com/pbek/QOwnNotes)를 방문합니다.

::: 팁 만약 당신이 프로젝트에 소스 코드를 제공하고 싶다면, 당신의 풀 요청을 `메인` 브랜치에 하세요. :::

## 원본 보관소

[QOwnNotes 원본 보관소](https://download.tuxfamily.org/qownnotes/src/)에서 QOwnNotes 원본 보관소을 찾을 수 있습니다.

소스 아카이브는 다음과 같이 액세스할 수 있습니다:

`https://download.tuxfamily.org/qownnotes/src/qownnotes-20.10.1.tar.xz`
