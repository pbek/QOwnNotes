# QOwnNotes 빌드

## GitHub에서 빌드

소스 코드에서 직접 QOwnNotes를 빌드할 수도 있습니다.

git 저장소에서 마지막 릴리스의 코드를 직접 확인하세요:

```bash
git clone https://github.com/pbek/QOwnNotes.git -b release
cd QOwnNotes
git submodule update --init
```

디버깅을 위해서는 QtCreator를 사용하는 것이 가장 쉽습니다. [Qt Creator](https://www.qt.io/download-qt-installer-oss)을 다운로드할 때 Qt6로 QOwnNotes를 구축하려면 패키지 `qtwebsockets`도 필요합니다. Windows에서 빌드하는 경우 _MinGw 64비트_을 고수해야 합니다. 리눅스의 경우 저장소에 QtCreator용 패키지가 있을 가능성이 높습니다.

그런 다음 프로젝트 파일 `src/QOwnNotes.pro`를 열고 **빌드** / **빌드 프로젝트 QOwnNotes**를 클릭합니다.

사용법을 잘 알고 있다면 터미널에서 직접 QOwnNotes를 만들 수도 있습니다:

```bash
# 영어가 아닌 다른 언어를 원하는 경우 이진 번역 파일 만들기
lrelease QOwnNotes.pro
```

이 프로젝트에 소스 코드를 제공하거나, 제안을 하거나, [QOwnNotes 문제 페이지](https://github.com/pbek/QOwnNotes/issues)에서 문제를 보고할 수 있습니다.

[GitHub의 QOnNotes](https://github.com/pbek/QOwnNotes)를 방문합니다.

::: tip
프로젝트에 소스 코드를 기여하고 싶으시면 `메인` 지점에 풀 요청을 해주시기 바랍니다. 하지만 먼저 [코드 기여 지침](../contributing/code-contributions.md)을 반드시 읽어주시기 바랍니다.
:::

## 원본 보관소

[QOwnNotes 소스 보관함](https://github.com/pbek/QOwnNotes/releases) 에서 QOwnNotes 소스 보관함을 찾을 수 있습니다.

소스 아카이브는 다음과 같이 액세스할 수 있습니다:

`https://github.com/pbek/QOwnNotes/releases/download/v23.7.0/qownnotes-23.7.0.tar.xz`

## nix로 빌드

[Nix 패키지 관리자](https://nixos.org/download/) 및 [Devenv](https://devenv.sh/getting-started/)이 설치되어 있다면 위에서 설명한 것처럼 저장소를 git-복제하고 빌드하기만 하면 됩니다:

```bash
# 필요한 모든 도구로 셸 열기
devenv shell

# 응용 프로그램 빌드
just nix-build

# 빌드된 응용 프로그램 실행
just nix-run
```
