# Microsoft Windows™에 설치

Download the latest **Windows archive** from [QOwnNotes Releases on GitHub](https://github.com/pbek/QOwnNotes/releases) and unzip it to anywhere you like. There is no installation needed!

::: warning If you want to use the **automatic updater** please make sure you unzip it to a place where your user account has write access to. By default your user account most likely **doesn't have write access** to places like `C:\Program Files (x86)` or `C:\Program Files`. :::

You can then directly run `QOwnNotes.exe` from your `QOwnNotes` folder, no installation is required.

### 휴대용 모드

Use `QOwnNotesPortable.bat` to run QOwnNotes in **portable mode** where everything (including your notes) will only be stored in your `QOwnNotes` folder.

::: tip
You don't need the portable mode if you just don't have Administration permissions
to your computer. QOwnNotes doesn't need to be installed!
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

[ Chocolatey](https://chocolatey.org/packages/qownnotes/)에는 커뮤니티에서 관리하는 QOwnotes 패키지가 있습니다.

다음을 사용하여 설치할 수 있습니다:

```shell
choco install qownnotes
```

## Scoop

[Scop](https://scoop.sh/)에 커뮤니티에서 관리하는 [QOwnotes ](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) 패키지가 있습니다. Extra 버킷을 추가하면 휴대용 모드에서 QOwnNotes를 설치하는 데 사용할 수 있습니다.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```
