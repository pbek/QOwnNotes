# QOwnNotes 스크립팅

QOwnNotes 스크립트는 대부분 [Qt QML 파일](https://doc.qt.io/qt-5/qtqml-index.html)의 **JavaScript** 입니다.

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
     * 스크립트 엔진이 초기화될 때 실행됩니다
     */
    function init() {
        script.log("Hello world!");
    }
}
```

원하는 위치에 QML 파일을 배치하고 **스크립트 설정** (`스크립트 추가`/`로컬 스크립트 추가` 버튼 있음)에 추가하여 **QOnNotes에 추가**할 수 있습니다.

::: 팁 [빠른 시작](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples)을 위한 예제 스크립트를 살펴보십시오. :::

**스크립트 설정**에서 [**스크립트 저장소**](https://github.com/qownnotes/scripts)에서 직접 스크립트를 설치할 수도 있습니다. [beurt.github.io/QOwnNoteScriptsList](https://beurt.github.io/QOwnNoteScriptsList/)에서 스크립트를 검색할 수 있는 커뮤니티 호스팅 웹 응용 프로그램이 있습니다.

**스크립트 저장소 **의 스크립트에 대한 문제, 질문 또는 기능 요청은 [QOwnNotes 스크립트 저장소 문제 페이지](https://github.com/qownnotes/scripts/issues)에서 문제를 여십시오.

::: 팁 **스크립트 저장소**에 대한 스크립트를 제안하려면 [QOwnNotes 스크립트 저장소](https://github.com/qownnotes/scripts)의 지침을 따르십시오. :::

QOwnNotes의 특정 기능에 대한 액세스가 필요하거나 질문이나 아이디어가 있는 경우 [QOwnNotes 문제 페이지](https://github.com/pbek/QOwnNotes/issues)에서 문제를 여십시오.

::: tip For logging, you can use the `script.log()` command to log to the _Log panel_, which you can turn on in the _Window / Panels_ menu. :::
