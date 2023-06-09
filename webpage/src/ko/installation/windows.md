# Microsoft Windows™에 설치

[GitHub의 QOwnNotes 릴리스](https://github.com/pbek/QOwnNotes/releases)에서 최신 **Windows 압축파일** 다운로드하여 원하는 곳으로 압축을 풉니다. 설치할 필요가 없습니다!

::: 경고 **자동 업데이트 프로그램**을 사용하려면 사용자 계정에 쓰기 권한이 있는 위치로 압축을 풉니다. 기본적으로 사용자 계정에는 `C:\Program Files (x86)` 또는`C:\Program Files`와 같은 위치에 대한 **쓰기 액세스 권한이 없습니다**. :::

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

[ Chocolatey](https://chocolatey.org/packages/qownnotes/)에는 커뮤니티에서 관리하는 QOwnotes 패키지가 있습니다.

다음을 사용하여 설치할 수 있습니다:

```shell
choco install qownnotes
```

## Scoop

[Scop](https://scoop.sh/)에 커뮤니티에서 관리하는 [QOwnotes ](https://github.com/ScoopInstaller/Extras/blob/master/bucket/qownnotes.json) 패키지가 있습니다. 엑스트라 버킷을 추가하면 휴대용 모드에서 QOwnNotes를 설치하는 데 사용할 수 있습니다.

```shell
scoop bucket add extras
scoop update
scoop install qownnotes
```
