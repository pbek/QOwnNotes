# 코드 기여

QOwnNotes에 코드를 제공해 주셔서 감사합니다.

## 풀 요청

### 먼저 문제 열기

풀 요청을 열기 전에 문제를 열어 주세요.

이는 코드를 검토하기 전에 문제, 제안된 해결책, 범위 및 가능한 부작용에 대해 논의하는 데 도움이 됩니다.

유일한 예외는 오타, 아주 작은 문서 수정, 또는 사전 논의가 필요 없는 또 다른 작은 변경과 같은 명백히 사소한 것입니다.

작업에 이미 존재하는 문제가 있는 경우 풀 요청에 이를 참조하세요.

### 분기 및 풀 요청

`main` 지점에 대한 풀 요청을 열어주세요.

작업을 시작하기 전에 변경 사항에 대한 새 분기를 생성하고 해당 분기에서 풀 요청을 엽니다.

포크를 최신 상태로 유지하는 데 문제가 발생할 수 있으며 실수로 관련 없는 변경 사항을 풀 요청에 포함시킬 수 있으므로 자신의 `main` 분기에서 풀 요청을 열지 마세요.

### 번역 기여

번역 파일을 직접 변경하여 번역 기여를 하지 마세요.
번역 파일은 자동으로 생성되므로 번역 작업은 [번역 페이지](translation.md)를 거쳐야 합니다.

## 메시지 커밋

이 저장소에서 사용되는 기존의 커밋 메시지 스타일을 따르세요.

최근 예시:

- `#1765 misc: optionally anonymize personal information in debug output`
- `#1789 misc: add remembered file manager open prompts`
- `#3568 mainwindow: allow dragging text file to import as note`
- `ci: fix step name`
- `release: bump version`

일반적으로 커밋 메시지는 다음과 같아야 합니다:

- 간결하고 설명적으로 작성하세요
- 콜론 뒤에 범위를 지정하세요. 예: `misc:`, `mainwindow:`, `tests:`, 또는 `ci:`
- 콜론 뒤에 명령형 설명을 추가하세요. 예: `fix`, `add`, `allow`, `update`, 또는 `refactor`
- 문제 번호가 있는 경우 시작 부분에 포함하세요. 예: `#3568 mainwindow: allow dragging text file to import as note`

문제 번호가 있는 경우 시작 부분에 포함하세요. 예: `#3568 mainwindow: allow dragging text file to import as note`
