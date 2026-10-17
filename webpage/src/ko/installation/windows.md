# Microsoft Windows™에 설치

[GitHub의 QOwnNotes Release](https://github.com/pbek/QOwnNotes/releases)에서 최신 **Windows 압축파일** (`QOwnNotes.zip`라는 파일을 찾습니다)를 다운로드한 후 원하는 곳으로 압축을 풉니다. 설치할 필요가 없습니다!

::: warning
**자동 업데이트 프로그램**을 사용하려면 사용자 계정에 쓰기 권한이 있는 위치로 압축을 풉니다. 기본적으로 사용자 계정은 `C:\Program Files(x86)` 또는 `C:\Program Files`과 같은 곳에 대한 **쓰기 액세스 권한이 없을** 가능성이 높습니다.
:::

그런 다음 설치가 필요 없이 `QOwnNotes` 폴더에서 `QOwnNotes.exe`를 바로 실행할 수 있습니다.

### 휴대용 모드

`QOwnNotesPortable.bat`을 사용하여 모든 항목 (노트 포함)이 `QOwnNotes` 폴더에만 저장되는 **휴대용 모드**에서 QOwnNotes를 실행합니다.

::: tip

컴퓨터에 대한 관리 권한이 없는 경우에는 휴대용 모드가 필요하지 않습니다. QOwnNotes 를 설치할 필요가 없습니다!
:::

## Windows 8.1 이상

The Windows release is built with **Qt6** and needs at least **Windows 10**.

The legacy Qt 5.7 build for Windows XP to Windows 8.1 is no longer updated. The final version is **QOwnNotes 26.10.0**, which you can download as [QOwnNotes-Qt5.7-legacy.zip](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip) ([SHA-256 checksum](https://github.com/pbek/QOwnNotes/releases/download/v26.10.0/QOwnNotes-Qt5.7-legacy.zip.sha256sum)) from the [26.10.0 release on GitHub](https://github.com/pbek/QOwnNotes/releases/tag/v26.10.0).

Unzip it to a folder of your liking and run `QOwnNotes.exe` from that folder, no installation is required.

::: tip
Info
The automatic update mechanism does not work with the legacy build!
:::

## Chocolatey

[Chocolatey](https://chocolatey.org/packages/qownnotes/)에는 커뮤니티에서 관리하는 QOwnNotes 패키지가 있습니다.

다음을 사용하여 설치할 수 있습니다:

```shell
choco install qownnotes
```

## Scoop

[Scoop](https://scoop.sh/)에는 [커뮤니티에서 관리하는 QOwnNotes 패키지](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json)가 있습니다. Extra 버킷을 추가하면 QOwnNotes를 포터블 모드로 설치하는 데 사용할 수 있습니다.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```

## WinGet

[WinGet](https://github.com/microsoft/winget-cli)에는 [커뮤니티에서 관리하는 QOwnNotes 패키지](https://github.com/microsoft/winget-pkgs/tree/master/manifests/p/pbek/QOwnNotes)가 있습니다.

다음 명령어로 설치할 수 있습니다:

```shell
winget 설치 qownnotes
```

## MSYS2

[MSYS2](hhttps://www.msys2.org/)용 [커뮤니티에서 관리하는 QOwnNotes 패키지](https://packages.msys2.org/base/mingw-w64-qownnotes)가 있습니다.

다음 명령어로 설치할 수 있습니다:

```shell
pacman -S mingw-w64-qownnotes
```
