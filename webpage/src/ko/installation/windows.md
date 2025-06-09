# Microsoft Windows™에 설치

[GitHub의 QOwnNotes Release](https://github.com/pbek/QOwnNotes/releases)에서 최신 **Windows 압축파일** (`QOwnNotes.zip`라는 파일을 찾습니다)를 다운로드한 후 원하는 곳으로 압축을 풉니다. 설치할 필요가 없습니다!

::: 경고 **자동 업데이트 프로그램**을 사용하려면 사용자 계정에 쓰기 권한이 있는 위치로 압축을 풉니다. 기본적으로 사용자 계정은 `C:\Program Files(x86)` 또는 `C:\Program Files`과 같은 곳에 대한 **쓰기 액세스 권한이 없을** 가능성이 높습니다. :::

그런 다음 설치가 필요 없이 `QOwnNotes` 폴더에서 `QOwnNotes.exe`를 바로 실행할 수 있습니다.

### 휴대용 모드

`QOwnNotesPortable.bat`을 사용하여 모든 항목 (노트 포함)이 `QOwnNotes` 폴더에만 저장되는 **휴대용 모드**에서 QOwnNotes를 실행합니다.

::: 팁
컴퓨터에 대한 관리 권한이 없는 경우에는 휴대용 모드가 필요하지 않습니다. QOwnNotes 를 설치할 필요가 없습니다!
:::

## Windows XP

Qt는 버전 5.8에서 Windows XP에 대한 지원을 중단했지만, QOwnNotes도 이제 Qt 5.7로 구축되어 Windows XP 사용자가 계속 사용할 수 있습니다.

직접 [AppVeyor](https://ci.appveyor.com/project/pbek/qownnotes/build/artifacts)에서 ZIP 파일을 다운로드하여 원하는 폴더에 압축을 풀어야 합니다.

그런 다음 해당 폴더에서 `QOwnNotes.exe`를 직접 실행할 수 있으며, 설치할 필요가 없습니다.

::: 팁 정보
자동 업데이트 메커니즘은 Windows XP용 AppVeyor 빌드와 함께 작동하지 않습니다!
새 릴리스를 직접 다운로드해야 합니다.
:::

## Chocolatey

There is a community-maintained package of QOwnNotes at [Chocolatey](https://chocolatey.org/packages/qownnotes/).

다음을 사용하여 설치할 수 있습니다:

```shell
choco install qownnotes
```

## Scoop

There is a [community-maintained package of QOwnNotes](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) at [Scoop](https://scoop.sh/). If you add the Extras bucket, you can use it to install QOwnNotes in portable mode.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```

## WinGet

There is a [community-maintained package of QOwnNotes](https://github.com/microsoft/winget-pkgs/tree/master/manifests/p/pbek/QOwnNotes) for [WinGet](https://github.com/microsoft/winget-cli).

You can install it with:

```shell
winget install qownnotes
```

## MSYS2

There is a [community-maintained package of QOwnNotes](https://packages.msys2.org/base/mingw-w64-qownnotes) for [MSYS2](hhttps://www.msys2.org/).

You can install it with:

```shell
pacman -S mingw-w64-qownnotes
```
