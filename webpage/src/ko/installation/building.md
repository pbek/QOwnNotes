# QOwnNotes 빌드

## GitHub에서 빌드

소스 코드에서 직접 QOwnNotes를 빌드할 수도 있습니다.

git 저장소에서 마지막 릴리스의 코드를 직접 확인하세요:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

디버깅을 위해서는 QtCreator를 사용하는 것이 가장 쉽습니다. [Qt Creator](https://www.qt.io/download-qt-installer-oss)을 다운로드할 때 Qt6로 QOwnNotes를 구축하려면 패키지 `qtwebsockets`도 필요합니다. If you build under Windows, you want to stick to _MinGw 64-bit_. 리눅스의 경우 저장소에 QtCreator용 패키지가 있을 가능성이 높습니다.

그런 다음 프로젝트 파일 `src/QOwnNotes.pro`를 열고 **빌드** / **빌드 프로젝트 QOwnNotes**를 클릭합니다.

사용법을 잘 알고 있다면 터미널에서 직접 QOwnNotes를 만들 수도 있습니다:

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

::: tip If you want to contribute source code to the project please make your pull requests to the `main` branch. :::

## 원본 보관소

[QOwnNotes 소스 보관함](https://github.com/pbek/QOwnNotes/releases) 에서 QOwnNotes 소스 보관함을 찾을 수 있습니다.

소스 아카이브는 다음과 같이 액세스할 수 있습니다:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`

## nix로 빌드

[Nix 패키지 관리자](https://nixos.org/download/)을 사용할 수 있다면 위에서 설명한 것처럼 소스 코드를 git-복제하고 빌드하기만 하면 됩니다:

```bash
# 아직 개발되지 않은 경우 개발 환경 보호
nix-shell -p devenv

# 필요한 모든 도구로 셸 열기
devenv shell

# 응용 프로그램 빌드
just nix-build

# 빌드된 응용 프로그램실행
just nix-run
```
