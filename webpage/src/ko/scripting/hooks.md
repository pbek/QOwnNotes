# Hooks

## NoteStored에서

### 메서드 호출 및 매개 변수

```js
/**
 * 이 기능은 노트가 디스크에 저장될 때 호출됩니다
 * 저장된 노트는 수정할 수 없습니다. 수작업으로 수정할 가능성이 
 * 높기 때문에 혼란스러울 수 있습니다.
 *
 * @param {NoteApi} note - the note object of the stored note
 */
function onNoteStored(note);
```

[on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml) 예제를 살펴보는 것이 좋습니다.

## noteOpenedHook

### 메서드 호출 및 매개 변수

```js
/**
 * 이 함수는 노트가 열린 후에 호출됩니다
 *
 * @param {NoteApi} note - the note object that was opened
 */
function noteOpenedHook(note);
```

[on-note-opened.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/on-note-opened.qml) 예제를 살펴보는 것이 좋습니다.

## noteDoubleClickedHook

### 메서드 호출 및 매개 변수

```js
/**
 * 함수는 노트를 더블클릭한 후 호출됩니다
 *
 * @param {NoteApi} note - the note object that was clicked
 */
function noteDoubleClickedHook(note);
```

[external-note-open.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-open.qml) 예제를 살펴보는 것이 좋습니다.

## insertMediaHook

이 기능은 현재 노트에 미디어 파일을 삽입할 때 호출됩니다.

이 함수가 여러 스크립트에 정의된 경우 비어 있지 않은 문자열을 반환하는 첫 번째 스크립트가 승리합니다.

### 메서드 호출 및 매개 변수

```js
/**
 * @param fileName string the file path of the source media file before it was copied to the media folder
 * @param markdownText string the markdown text of the media file, e.g. ![my-image](media/my-image-4101461585.jpg)
 * @return string the new markdown text of the media file
 */
function insertMediaHook(fileName, markdownText);
```

[example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) 예제를 살펴보는 것이 좋습니다.

## insertAttachmentHook

이 기능은 첨부 파일이 현재 노트에 삽입될 때 호출됩니다.

이 함수가 여러 스크립트에 정의된 경우 비어 있지 않은 문자열을 반환하는 첫 번째 스크립트가 승리합니다.

### 메서드 호출 및 매개 변수

```js
/**
 * @param fileName string the file path of the source attachment file before it was copied to the attachment folder
 * @param markdownText string the markdown text of the attachment file, e.g. [my-file.txt](attachments/my-file-4245650967.txt)
 * @return string the new markdown text of the attachment file
 */
function insertAttachmentHook(fileName, markdownText);
```

[example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) 예제를 살펴보는 것이 좋습니다.

## insertingFromMimeDataHook

### 메서드 호출 및 매개 변수

```js
/**
 * 이 함수는 `Ctrl + Shift + V`를 사용해 노트에 HTML 또는 미디어 파일을 붙여넣을 때 호출됩니다
 *
 * @param text text of the QMimeData object
 * @param html html of the QMimeData object
 * @returns the string that should be inserted instead of the text from the QMimeData object
 */
function insertingFromMimeDataHook(text, html);
```

[example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml), [insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml) 또는 [note-text-from-5pm-mail.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-text-from-5pm-mail.qml) 예제를 살펴보는 것이 좋습니다.

## handleNoteTextFileNameHook

### 메서드 호출 및 매개 변수

```js
/**
 * 이 함수는 노트가 디스크에 저장될 때 다음과 같은 경우에 호출됩니다
 * 설정에서 "노트 파일 이름을 제목과 다르게 허용"이 활성화되어
 * 있습니다
 *
 * 노트 파일의 이름을 수정할 수 있습니다
 * 중복된 이름은 직접 관리해야 한다는 점을 명심하세요!
 *
 * 노트의 파일 이름을 수정하지 않아야 하는 경우
 * 빈 문자열 반환
 *
 * @param {NoteApi} note - the note object of the stored note
 * @return {string} the file name of the note
 */
function handleNoteTextFileNameHook(note);
```

[example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) 또는 [use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml) 예제를 살펴보는 것이 좋습니다.

## handleNoteNameHook

### 메서드 호출 및 매개 변수

```js
/**
 * 이 기능은 노트에 대한 노트 이름이 결정될 때 호출됩니다
 *
 * 표시된 노트의 이름을 수정할 수 있습니다
 *
 * 노트 이름을 수정하지 않아야 하는 경우 빈 문자열 반환
 *
 * @param {NoteApi} note - the note object of the stored note
 * @return {string} the name of the note
 */
function handleNoteNameHook(note);
```

노트 이름은 주로 목록에 노트를 표시할 때 사용됩니다.

[example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) 예제를 살펴보는 것이 좋습니다.

파일 이름을 노트 이름으로 사용하도록 설정이 활성화되어 있는 경우, 이 훅을 사용하는 것이 좋지 않을 수 있습니다.

## handleNewNoteHeadlineHook

### 메서드 호출 및 매개 변수

```js
/**
 * 노트를 만들기 전에 이 함수를 호출합니다
 *
 * 노트를 만들기 전에 노트의 헤드라인을 수정할 수 있습니다
 * 고유한 노트 이름에 주의해야 합니다. 그렇지 않으면
 * 새 노트가 생성되지 않고 노트 목록에서만 찾을 수 있습니다
 *
 * 노트 템플릿을 만드는 데 이 기능을 사용할 수 있습니다
 *
 * @param headline text that would be used to create the headline
 * @return {string} the headline of the note
 */
function handleNewNoteHeadlineHook(headline);
```

[custom-new-note-headline.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-new-note-headline.qml) 예제를 살펴보는 것이 좋습니다.

## preNoteToMarkdownHtmlHook

### 메서드 호출 및 매개 변수

```js
/**
 * 이 함수는 노트의 마크다운 HTML이 생성되기 전에 호출됩니다
 *
 * 마크다운으로 전달되는 내용을 HTML 변환기로 수정할 수 있습니다
 *
 * 예를 들어 이 함수는 여러 스크립트에서 미리보기를 위해 코드 (LaTeX 수학 또는 인어)를
 * 그래픽 표현으로 렌더링하는 데 사용할 수 있습니다
 *
 * 이 과정에서 노트는 변경되지 않습니다
 *
 * @param {NoteApi} note - the note object
 * @param {string} markdown - the markdown that is about to being converted to html
 * @param {bool} forExport - true if the html is used for an export, false for the preview
 * @return {string} the modified markdown or an empty string if nothing should be modified
 */
function preNoteToMarkdownHtmlHook(note, markdown, forExport);
```

[preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml) 예제를 살펴보는 것이 좋습니다.

## noteToMarkdownHtmlHook

### 메서드 호출 및 매개 변수

```js
/**
 * 이 함수는 노트의 마크다운 html이 생성될 때 호출됩니다
 *
 * 이 html을 수정할 수 있습니다
 * 예를 들어 노트 미리 보기에서 이전에 호출됩니다
 *
 * 이 함수는 미리보기의 html을 수정하기 위해 여러 스크립트에서 사용될 수 있습니다
 *
 * @param {NoteApi} note - the note object
 * @param {string} html - the html that is about to being rendered
 * @param {bool} forExport - true if the html is used for an export, false for the preview
 * @return {string} the modified html or an empty string if nothing should be modified
 */
function noteToMarkdownHtmlHook(note, html, forExport);
```

[example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/example.qml) 또는 [preview-styling.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/preview-styling.qml) 예제를 살펴보는 것이 좋습니다.

지원되는 모든 css 스타일 목록은 [지원되는 HTML 하위 집합 문서](http://doc.qt.io/qt-5/richtext-html-subset.html)를 참조하세요.

## encryptionHook

### 메서드 호출 및 매개 변수

```js
/**
 * 이 함수는 텍스트를 암호화하거나 해독해야 할 때 호출됩니다
 *
 * @param text string the text to encrypt or decrypt
 * @param password string the password
 * @param decrypt bool if false encryption is demanded, if true decryption is demanded
 * @return the encrypted decrypted text
 */
function encryptionHook(text, password, decrypt);
```

[encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml), [encryption-pgp.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-pgp.qml) 또는 [encryption-rot13.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-rot13.qml)의 예를 살펴볼 수 있습니다.

## noteTaggingHook

예를 들어 노트에 `@tag1`, `@tag2`, `@tag3`와 같은 특수 텍스트를 사용하여 고유한 노트 태그 지정 메커니즘을 구현할 수 있습니다.

### 메서드 호출 및 매개 변수

```js
/**
 * 노트에 대한 노트 태그 지정 처리
 *
 * 이 기능은 노트에 태그가 추가, 제거 또는 이름 변경되거나 노트의 태그가
 * 나열되어야 할 때 호출됩니다
 *
 * @param note
 * @param action can be "add", "remove", "rename" or "list"
 * @param tagName tag name to be added, removed or renamed
 * @param newTagName tag name to be renamed to if action = "rename"
 * @return note text string or string-list of tag names (if action = "list")
 */
function noteTaggingHook(note, action, tagName, newTagName);
```

- as soon as a script is activated that implements the new function `noteTaggingHook` note tagging will be handled by that function
- 다음 기능은 QOwnNotes 사용자 인터페이스를 통해 작동해야 합니다
  - initially importing tags like `@tag` from your notes and overwriting your current tag assignment
    - you will not lose your tags tree, just the former assignment to notes
    - 여전히 태그를 다른 태그로 이동할 수 있습니다
    - if more than one tag has the same name in your tag tree the first hit will be assigned
  - 노트에 태그를 추가하면 노트 텍스트에 태그가 추가됩니다
  - 노트에서 태그를 제거하면 노트 텍스트에서 태그가 제거됩니다
  - removing of tags in the tag list will remove those tags from your notes
  - 태그 목록에서 태그 이름을 변경하면 노트에 있는 태그 이름이 변경됩니다
  - bulk tagging of notes in the note list will add those tags to your notes
  - 노트 목록에서 태그를 대량으로 제거하면 해당 태그가 노트에서 제거됩니다
  - 태그가 이동하면 응용 프로그램은 모든 노트에서 선택한 모든 태그와 그 하위에 대해 일련의 `추가` 및 `제거` 작업을 트리거합니다

[note-tagging.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging.qml) 예제를 보고 자체 태그 메커니즘을 구현할 수 있습니다.

::: 경고 노트 폴더나 태그 트리가 새로고침될 때마다 모든 노트에 대해 실행되므로 `목록` 작업이 정말 빠르다는 것을 확인하세요!

[mainwindow.cpp](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.cpp)에서 `reloadTagTree`을 찾을 때마다 그렇습니다. :::

## noteTaggingByObjectHook

[noteTaggingHook](#notetagginghook)과 유사하게 자신만의 노트 태그 지정 메커니즘을 구현할 수 있지만, 태그 트리 루트의 이름에 태그를 지정할 필요는 없습니다. 이렇게 하면 태그 목록만 사용하는 대신 전체 태그 트리를 사용할 수 있습니다.

`noteTaggingByObjectHook`을 사용하면 태그 이름 대신 `TagApi` 개체를 매개 변수로 얻을 수 있습니다. 따라서 목록 작업의 경우 태그 Id `목록`을 제공해야 합니다.

또한 목록 작업에 대해 이미 존재하는 태그 Id `목록`을 제공하려면 누락된 태그를 직접 만들어야 합니다.

### 메서드 호출 및 매개 변수

```js
/**
 * 노트에 대한 노트 태그 지정 처리
 *
 * 이 기능은 노트에 태그가 추가, 제거 또는 이름 변경되거나 노트의 태그가
 * 나열되어야 할 때 호출됩니다
 *
 * @param note
 * @param action can be "add", "remove", "rename" or "list"
 * @param tag to be added, removed or renamed
 * @param newTagName tag name to be renamed to if action = "rename"
 * @return note text string or string-list of tag ids (if action = "list")
 */
function noteTaggingByObjectHook(note, action, tag, newTagName);
```

고유한 태그 지정 메커니즘을 구현하기 위해 [note-tagging-by-object.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/note-tagging-by-object.qml) 예제를 살펴볼 수 있습니다.

## autocompletionHook

자동 완성이 호출될 때 자동 완성 목록에 추가할 문자열 목록을 반환할 수 있습니다 (예를들면 <kbd>Ctrl + 스페이스 누르기</kbd>).

### 메서드 호출 및 매개 변수

```js
/**
 * 자동 완성을 호출합니다모든 스크립트 구성 요소에 대한 후크 기능
 * 이 함수는 노트에서 자동 완성이 호출될 때 호출됩니다
 *
 * @return QStringList of text for the autocomplete list
 */
function callAutocompletionHook();
```

[autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml) 예제를 살펴볼 수 있습니다.

## websocketRawDataHook

이 후크는 웹 브라우저의 상황에 맞는 메뉴를 통해 QOwnNotes Web Companion 브라우저 확장에서 데이터를 전송할 때 호출됩니다.

### 메서드 호출 및 매개 변수

```js
/**
 * @param requestType은 "page" 또는 "selection"일 수 있습니다
 * @param page요청이 이루어진 웹페이지의 Url
 * @param page요청이 이루어진 웹페이지의 페이지 제목
 * @paramrawData 전송된 데이터, request용 html "page" 또는 request용 평문Type "selection"
 * @param screenshotDataUrl 요청이 이루어진 웹 페이지의 스크린샷의 데이터 Url
 * @후크에 의해 데이터가 처리된 경우 true를 반환합니다
 */
function callHandleWebsocketRawDataHook(requestType, pageUrl, pageTitle, rawData, screenshotDataUrl);
```

[websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-new-note.qml) 및 [websocket-raw-data-selection-in-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-raw-data-selection-in-note.qml)의 예를 살펴볼 수 있습니다.

## onDetachedProcessCallback

[startDetachedProcess](methods-and-objects.html#starting-an-external-program-in-the-background)의 스크립트 스레드 실행이 완료되면 이 후크가 호출됩니다.

### 메서드 호출 및 매개 변수

```js
/**
 * 이 함수는 스크립트 스레드 실행이 완료되면 호출됩니다.
 힌트: thread[1]==0은 특정 식별자에 대해 시작된 프로세스의 대부분이 수행되었는지 여부를 확인하는 데 도움이 됩니다.
 *
 * @param {QString} callbackIdentifier - the provided id when calling startDetachedProcess()
 * @param {QString} resultSet - the result of the process
 * @param {QVariantList} cmd - the entire command array [0-executablePath, 1-parameters, 2-exitCode]
 * @param {QVariantList} thread - the thread information array [0-passed callbackParameter, 1-remaining threads for this identifier]
 */
function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread);
```

[callback-example.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml) 예제를 살펴볼 수 있습니다.

## windowStateChangedHook

### 메서드 호출 및 매개 변수

```js
/**
 * 이 함수는 WindowStateChange 이벤트가 트리거된 후 호출됩니다
 *
 * @param {QString} windowState - the new window state, parameter value can be "minimized", "maximized", "fullscreen", "active" or "nostate"
 */
function windowStateChangedHook(windowState);
```

[window-state-changed.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/window-state-changed.qml) 예제를 살펴볼 수 있습니다.

## workspaceSwitchedHook

이 후크는 작업 공간이 전환될 때 호출됩니다.

### 메서드 호출 및 매개 변수

```js
/**
 * 이 기능은 작업 공간이 전환될 때 호출됩니다
 *
 * @param oldUuid old uuid of workspace
 * @param newUuid new uuid of workspace
 */
function workspaceSwitchedHook(oldUuid, newUuid);
```

[websocket-raw-data-new-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/workspaces.qml) 예제를 살펴볼 수 있습니다.

## openAiBackendsHook

이 후크는 OpenAI 서비스 구성이 다시 로드될 때 호출됩니다. 예를들면, 이 작업은 스크립트 엔진이 다시 로드될 때도 수행됩니다.

사용자 지정 OpenAI 백엔드에 대한 구성을 제공하는 데 사용할 수 있습니다 ChatGPT API 호환 LLM, 예를 들어 OpenAI, Claude 또는 Ollama와 같은 시스템입니다.

### 메소드 호출 및 매개변수

```js
/**
* 이 기능은 OpenAI 서비스 구성이 다시 로드될 때 호출됩니다.
* 새로운 OpenAI 백엔드에 대한 구성 매개변수가 있는 객체 목록을 반환합니다.
 */
function openAiBackendsHook() {
  return [
    {
      id: "my-custom-ai",
      name: "My Custom AI",
      baseUrl: "http://localhost:5000",
      apiKey: "kDFJkjk3asdm",
      models: ["gpt-3.5-turbo", "gpt-4.0-turbo"],
    },
    {
      id: "my-custom-ai2",
      name: "My Custom AI 2",
      baseUrl: "http://localhost:5001",
      apiKey: "lOikf7eNdb9",
      models: ["gpt-3.5-turbo2", "gpt-4.0-turbo2"],
    },
  ];
}
```

::: 팁 [스크립트 설정 변수 등록하기](./methods-and-objects.md#registering-script-settings-variables)를 사용할 수 있습니다 OpenAI 백엔드 설정을 스크립트 설정에 저장합니다. :::

예제를 살펴보실 수 있습니다 [ custom-openai-backends.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-openai-backends.qml).
