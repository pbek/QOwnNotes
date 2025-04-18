# 명령줄 인터페이스 매개 변수

명령줄 인터페이스 (CLI)에서 다음 매개 변수를 사용하여 응용 프로그램의 동작을 제어할 수 있습니다:

| 매개 변수                        | 설명                                                                    |
| ---------------------------- | --------------------------------------------------------------------- |
| `--help`                     | 도움말 화면을 표시합니다                                                         |
| `--version`                  | 응용 프로그램의 버전 번호를 인쇄합니다                                                 |
| `--portable`                 | 응용 프로그램을 휴대용 모드로 실행합니다                                                |
| `--clear-settings`           | 설정을 지우고 응용 프로그램을 실행합니다                                                |
| `--dump-settings`            | GitHub Markdown에서 응용 프로그램 및 환경에 대한 설정 덤프 및 기타 정보를 인쇄하고 응용 프로그램을 종료합니다 |
| `--session <name>`     | 설정 및 내부 파일에 대해 다른 컨텍스트에서 응용 프로그램을 실행합니다                               |
| `--allow-multiple-instances` | 설정에서 허용되지 않는 경우에도 QOnNotes의 여러 인스턴스를 시작할 수 있습니다                       |
| `--action <name>`      | 응용 프로그램이 시작된 후 메뉴 작업을 트리거합니다 (아래 참조)                                  |
| `--completion <shell>` | 셸 완료 코드를 생성합니다. `fish`, `bash`를 지원합니다.                                |

::: 팁 QOnNotes 설치 시 문제가 발생할 경우 `--session` 매개 변수를 사용하여 현재 설정을 잃지 않고 새로운 설정으로 응용프로그램을 시작할 수 있습니다.

```bash
QOwnNotes --session test
```

:::

명령행 인터페이스에서 응용 프로그램을 다른 운영 체제에서 다르게 실행할 수 있습니다:

| 운영 체제   | 명령                                                     |
| ------- | ------------------------------------------------------ |
| Linux   | `QOwnNotes` (또는 스냅으로 설치된 경우 `qownnotes`)               |
| macOS   | `/Applications/QOwnNotes.app/Contents/MacOS/QOwnNotes` |
| Windows | `QOwnNotes.exe`                                        |

::: 팁 설정 덤프를 클립보드에 직접 복사하여 [GitHub 문제](https://github.com/pbek/QOwnNotes/issues)에 게시하려면 Linux의 경우 `xclip` 또는 `xsel`, macOS의 경우 `pbcopy`, Windows의 경우 `clip`을 사용할 수 있습니다:

```bash
#Linux에서 xclip을 사용하여 설정 덤프를 클립보드에 복사
QOwnNotes --dump-settings | xclip -selection clipboard

# Linux에서 xsel을 사용하여 설정 덤프를 클립보드에 복사
QOwnNotes --dump-settings | xsel --clipboard

# macOS에서 pbcopy를 사용하여 설정 덤프를 클립보드에 복사
QOwnNotes --dump-settings | pbcopy

# Windows에서 clip으로 설정 덤프를 클립보드에 복사
QOwnNotes --dump-settings | clip
```

:::

## 시작 후 메뉴 작업 트리거

파라미터 `--action <name>` 을 사용하여 응용 프로그램이 시작된 후 메뉴 동작을 트리거할 수 있습니다.

시작 후 작업관리 목록 대화상자를 여는 예:

```bash
QOwnNotes --action actionShow_Todo_List
```

::: 팁 메뉴 동작의 객체 이름은 [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui)에서 얻을 수 있습니다. 영어 메뉴 제목만 검색해 보세요. 이러한 텍스트는 시간이 지남에 따라 변경될 수 있습니다. :::

[스크립트 작업](../scripting/methods-and-objects.md#registering-a-custom-action)을 트리거하려면 `customaction_` 뒤에 사용자 지정 작업의 식별자가 표시됩니다. 사용자 지정 작업의 식별자는 스크립트에서 `script.registerCustomAction` 호출의 첫 번째 매개 변수입니다.

예를 들어 사용자 지정 작업 `myAction`을 실행하려면 다음과 같이 QOwnNotes를 시작합니다:

```bash
QOwnNotes --action customAction_myAction
```

::: 팁
수행 매개변수로 QOwnNotes를 두 번 실행하고 하나의 응용프로그램 인스턴스만 허용한 경우, 메뉴 수행은 첫 번째 인스턴스에서 트리거됩니다.
:::
