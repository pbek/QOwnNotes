# QOwnNotes에서 제공하는 메서드 및 개체

## 백그라운드에서 외부 프로그램 시작

### 메서드 호출 및 매개 변수

```cpp
/**
 * 분리된 프로세스를 시작하기 위한 QML 래퍼
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param callbackIdentifier an identifier to be used in the onDetachedProcessCallback() function (optional)
 * @param callbackParameter an additional parameter for loops or the like (optional)
 * @param processData data written to the process if the callback is used (optional)
 * @param workingDirectory the working directory to execute the process in (optional, only works without callback)
 * @return true on success, false otherwise
 */
bool startDetachedProcess(QString executablePath, QStringList parameters,
                            QString callbackIdentifier, QVariant callbackParameter,
                            QByteArray processData, QString workingDirectory);
```

### 예제

간단한 예:

```js
script.startDetachedProcess("/path/to/my/program", ["my parameter"]);
```

많은 프로세스 실행:

```js
for (var i = 0; i < 100; i++) {
  var dur = Math.floor(Math.random() * 10) + 1;
  script.startDetachedProcess("sleep", [`${dur}s`], "my-callback", i);
}

function onDetachedProcessCallback(callbackIdentifier, resultSet, cmd, thread) {
  if (callbackIdentifier == "my-callback") {
    script.log(`#${thread[1]} i[${thread[0]}] t${cmd[1]}`);
  }
}
```

[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml), [callback.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/callback.qml) 또는 [execute-command-after-note-update.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/execute-command-after-note-update.qml) 예제를 살펴볼 수 있습니다.

또한 [onDetachedProcessCallback](hooks.html#ondetachedprocesscallback) 후크에 대해서도 살펴볼 수 있습니다.

## 외부 프로그램 시작 및 출력 대기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 동기 프로세스를 시작하기 위한 QML 래퍼
 *
 * @param executablePath the path of the executable
 * @param parameters a list of parameter strings
 * @param data the data that will be written to the process (optional)
 * @param workingDirectory the working directory to execute the process in (optional)
 * @return the text that was returned by the process
QByteArray startSynchronousProcess(QString executablePath, QStringList parameters, QByteArray data, QString workingDirectory);
```

### 예제

```js
var result = script.startSynchronousProcess(
  "/path/to/my/program",
  ["my parameter"],
  "data",
  "/path/to/execute/in",
);
```

[encryption-keybase.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/encryption-keybase.qml)의 예를 살펴볼 수 있습니다.

## 현재 노트 폴더의 경로를 가져오기

### 메서드 호출 및 매개 변수

```cpp
/**
 * QML wrapper to get the current note folder path
 *
 * @return the path of the current note folder
 */
QString currentNoteFolderPath();
```

### 예제

```js
var path = script.currentNoteFolderPath();
```

[absolute-media-links.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/absolute-media-links.qml)의 예를 살펴볼 수 있습니다.

## 현재 노트 가져오기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 현재 노트를 가져오는 QML 래퍼
 *
 * @returns {NoteApi} the current note object
 */
NoteApi currentNote();
```

### 예제

```js
var note = script.currentNote();
```

[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml) 예제를 살펴볼 수 있습니다.

## 로그 위젯에 로깅

### 메서드 호출 및 매개 변수

```cpp
/**
 * 로그 위젯에 로깅하기 위한 QML 래퍼
 *
 * @param text
 */
void log(QString text);
```

### 예제

```js
script.log("my text");
```

## 문자열에 Url 다운로드

### 메서드 호출 및 매개 변수

```cpp
/**
 * QML 래퍼를 사용하여 URL을 다운로드하고 텍스트로 반환하는 방법
 *
 * @param url
 * @return {QString} the content of the downloaded url
 */
QString downloadUrlToString(QUrl url);
```

### 예제

```js
var html = script.downloadUrlToString("https://www.qownnotes.org");
```

[insert-headline-with-link-from-github-url.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/insert-headline-with-link-from-github-url.qml)의 예를 살펴볼 수 있습니다.

## 미디어 폴더로 Url 다운로드

### 메서드 호출 및 매개 변수

```cpp
/**
 * QML 래퍼를 사용해 미디어 폴더로 URL을 다운로드하고 현재 노트와 관련된
 * 미디어 URL 또는 미디어의 마크다운 이미지 텍스트를 반환
 *
 * @param {QString} url
 * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
 * @return {QString} the media markdown or url
 */
QString downloadUrlToMedia(QUrl url, bool returnUrlOnly);
```

### 예제

```js
var markdown = script.downloadUrlToMedia(
  "http://latex.codecogs.com/gif.latex?\frac{1}{1+sin(x)}",
);
```

[paste-latex-image.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/paste-latex-image.qml) 예제를 살펴볼 수 있습니다.

## 미디어 폴더에 미디어 파일 삽입

### 메서드 호출 및 매개 변수

```cpp
/**
 * QML wrapper to insert a media file into the media folder and returning
 * the media url or the markdown image text of the media  relative to the current note
 *
 * @param {QString} mediaFilePath
 * @param {bool} returnUrlOnly if true only the media url will be returned (default false)
 * @return {QString} the media markdown or url
 */
QString ScriptingService::insertMediaFile(QString mediaFilePath,
                                        bool returnUrlOnly);
```

### 예제

```js
var markdown = script.insertMediaFile("/path/to/your/image.png");
```

[scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml)의 예제를 살펴볼 수 있습니다.

## 첨부파일 폴더에 첨부파일 삽입

### 메서드 호출 및 매개 변수

```cpp
 * QML wrapper to insert an attachment file into the `attachments` folder and
 * returning the attachment url or the markdown text of the attachment
 * relative to the current note
 *
 * @param {QString} attachmentFilePath
 * @param {QString} fileName to use in the markdown
 * @param {bool} returnUrlOnly if true only the attachment url will be returned
 * (default false)
 * @return {QString} the attachment markdown or url
 */
QString ScriptingService::insertAttachmentFile(const QString &attachmentFilePath,
                                               const QString &fileName,
                                               bool returnUrlOnly);
```

### 예제

```js
var markdown = script.insertAttachmentFile("/path/to/your/file.png");
```

## 노트 미리보기 재생성

노트 미리 보기를 새로 고칩니다.

### 메서드 호출 및 매개 변수

```cpp
/**
 * Regenerates the note preview
 */
QString ScriptingService::regenerateNotePreview();
```

### 예제

```js
script.regenerateNotePreview();
```

[scribble.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scribble.qml)의 예제를 살펴볼 수 있습니다.

## 사용자 지정 작업 등록

### 메서드 호출 및 매개 변수

```cpp
/**
 * 사용자 지정 작업을 등록
 *
 * @param identifier the identifier of the action
 * @param menuText the text shown in the menu
 * @param buttonText the text shown in the button
 *                   (no button will be viewed if empty)
 * @param icon the icon file path or the name of a freedesktop theme icon
 *             you will find a list of icons here:
 *             https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html
 * @param useInNoteEditContextMenu if true use the action in the note edit
 *                                 context menu (default: false)
 * @param hideButtonInToolbar if true the button will not be shown in the
 *                            custom action toolbar (default: false)
 * @param useInNoteListContextMenu if true use the action in the note list
 *                                 context menu (default: false)
 */
void ScriptingService::registerCustomAction(QString identifier,
                                            QString menuText,
                                            QString buttonText,
                                            QString icon,
                                            bool useInNoteEditContextMenu,
                                            bool hideButtonInToolbar,
                                            bool useInNoteListContextMenu);
```

::: 팁 _ 단축키 설정_에서 사용자 지정 작업에 로컬 및 글로벌 바로 가기를 할당할 수도 있습니다. :::

::: 경고 [무료 데스크톱 테마 아이콘](https://specifications.freedesktop.org/icon-naming-spec/icon-naming-spec-latest.html)은 대부분 Linux에서만 사용할 수 있습니다. 따라서 macOS나 Windows에서 아이콘을 사용하려면 스크립트와 함께 아이콘을 제공해야 합니다. 스크립트 경로를 통해 아이콘에 대한 올바른 경로를 설정하려면 [scriptDirPath property](methods-and-objects.md#reading-the-path-to-the-directory-of-your-script)을 사용합니다. :::

### 예제

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    /**
* 사용자 지정 작업을 초기화합니다
     */
    function init() {
        // 버튼 없이 사용자 지정 작업 추가
        script.registerCustomAction("mycustomaction1", "Menu text");

        // 버튼으로 사용자 지정 작업 추가
        script.registerCustomAction("mycustomaction2", "Menu text", "Button text");

        // 버튼과 무료 데스크톱 테마 아이콘으로 사용자 지정 작업 추가
        script.registerCustomAction("mycustomaction3", "Menu text", "Button text", "task-new");

        // 파일에서 버튼과 아이콘으로 사용자 지정 작업 추가
        script.registerCustomAction("mycustomaction4", "Menu text", "Button text", "/usr/share/icons/breeze/actions/24/view-calendar-tasks.svg");
    }

    /**
     * 이 기능은 메뉴 또는 버튼을 통해 사용자 지정 작업이
     * 트리거될 때 호출됩니다
     *
     * @param identifier string the identifier defined in registerCustomAction
     */
    function customActionInvoked(identifier) {
        switch (identifier) {
            case "mycustomaction1":
                script.log("Action 1");
            break;
            case "mycustomaction2":
                script.log("Action 2");
            break;
            case "mycustomaction3":
                script.log("Action 3");
            break;
            case "mycustomaction4":
                script.log("Action 4");
            break;
        }
    }
}
```

더 많은 예제는 [custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml)을 참조하세요.

::: 팁 매개변수 `--action customAction_<identifier>`을 사용하여 애플리케이션이 시작된 후 사용자 지정 동작을 트리거할 수도 있습니다. 자세한 내용은 [ 시작 후 트리거 메뉴 동작](../getting-started/cli-parameters.md#trigger-menu-actions-after-startup)을 참조하십시오. :::

## 레이블 등록

### 메서드 호출 및 매개 변수

```cpp
/**
 * 쓸 레이블을 등록
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label (optional)
 */
void ScriptingService::registerLabel(QString identifier, QString text);
```

### 예제

```js
script.registerLabel(
  "html-label",
  "<strong>Strong</strong> HTML text<br />with three lines<br />and a <a href='https://www.qownnotes.org'>link to a website</a>.",
);

script.registerLabel(
  "long-label",
  "another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text, another very long text that will wrap",
);

script.registerLabel("counter-label");
```

The labels will be visible in the _Scripting panel_, which you need to enable in the _Window / Panels_ menu.

레이블에는 일반 텍스트 또는 html을 모두 사용할 수 있습니다. 텍스트를 선택할 수 있고 링크를 클릭할 수 있습니다.

그런 다음 스크립트 [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml) 예제를 살펴볼 수 있습니다.

## 등록된 레이블의 텍스트 설정

### 메서드 호출 및 매개 변수

```cpp
/**
 * 등록된 레이블의 텍스트를 설정
 *
 * @param identifier the identifier of the label
 * @param text the text shown in the label
 */
void ScriptingService::setLabelText(QString identifier, QString text);
```

### 예제

```js
script.setLabelText("counter-label", "counter text");
```

레이블에는 일반 텍스트 또는 html을 모두 사용할 수 있습니다. 텍스트를 선택할 수 있고 링크를 클릭할 수 있습니다.

그런 다음 스크립트 [scripting-label-demo.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/scripting-label-demo.qml) 예제를 살펴볼 수 있습니다.

::: 팁 스크립팅 레이블은 또한 ** 외부 링크 **과 노트, 작업, 데크 카드 등에 대한 ** 내부 링크 **을 지원합니다. [URL 처리](https://github.com/pbek/QOwnNotes/blob/964acf69b6382f8ee8252c640c5048f8f4644622/src/utils/urlhandler.cpp#L16-L75)을 보면 무엇이 지원되는지 파악할 수 있습니다. :::

## 새 노트 만들기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 새 노트 만들기
 *
 * @param text the note text
 */
void ScriptingService::createNote(QString text);
```

### 예제

```js
script.createNote("My note headline\n===\n\nMy text");
```

[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml) 예제를 살펴볼 수 있습니다.

::: 팁 노트 제목에 따라 노트 파일 이름이 결정된다는 것을 해제한 경우, 다음과 같이 노트 파일 이름을 직접 변경해야 합니다:

```js
var note = script.currentNote();
note.renameNoteFile("your-filename");
```

:::

## 클립보드 액세스

### 메서드 호출 및 매개 변수

```cpp
/**
 * 클립보드의 내용을 텍스트 또는 html로 반환
 *
 * @param asHtml returns the clipboard content as html instead of text
 */
QString ScriptingService::clipboard(bool asHtml);
```

### 예제

```js
var clipboardText = script.clipboard();
var clipboardHtml = script.clipboard(true);
```

[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml) 예제를 살펴볼 수도 있습니다.

## 노트 텍스트 편집에 텍스트 쓰기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집의 현재 커서 위치에 텍스트 쓰기
 *
 * @param text
 */
void ScriptingService::noteTextEditWrite(QString text);
```

### 예제

```js
// 노트에 텍스트 쓰기 텍스트 편집
script.noteTextEditWrite("My custom text");
```

[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml) 예제에서 사용자 지정 작업 `transformTextRot13`을 살펴볼 수 있습니다.

이 기능을 `noteTextEditSelectAll`과 함께 사용해 현재 노트의 전체 텍스트를 덮어쓸 수 있습니다.

## 노트 텍스트 편집에서 선택한 텍스트 읽기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집에서 선택한 텍스트를 읽기
 *
 * @return
 */
QString ScriptingService::noteTextEditSelectedText();
```

### 예제

```js
// 노트 텍스트 편집에서 선택한 텍스트 읽기
var text = script.noteTextEditSelectedText();
```

[custom-actions.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-actions.qml) 예제에서 사용자 지정 작업 `transformTextRot13`을 살펴볼 수 있습니다.

## 노트 텍스트 편집에서 모든 텍스트 선택

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집에서 모든 텍스트를 선택
 */
void ScriptingService::noteTextEditSelectAll();
```

### 예제

```js
script.noteTextEditSelectAll();
```

이 기능을 `noteTextEditWrite`와 함께 사용해 현재 노트의 전체 텍스트를 덮어쓸 수 있습니다.

## 노트 텍스트 편집에서 현재 줄 선택

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집에서 현재 줄 선택
 */
void ScriptingService::noteTextEditSelectCurrentLine();
```

### 예제

```js
script.noteTextEditSelectCurrentLine();
```

## 노트 텍스트 편집에서 현재 단어 선택

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집에서 현재 단어 선택
 */
void ScriptingService::noteTextEditSelectCurrentWord();
```

### 예제

```js
script.noteTextEditSelectCurrentWord();
```

## 노트 텍스트 편집에서 현재 선택한 텍스트 설정

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집에서 현재 선택한 텍스트 설정
 *
 * @param start
 * @param end
 */
void ScriptingService::noteTextEditSetSelection(int start, int end);
```

### 예제

```js
// expands the current selection by one character
script.noteTextEditSetSelection(
  script.noteTextEditSelectionStart() - 1,
  script.noteTextEditSelectionEnd() + 1,
);
```

## 노트 텍스트 편집에서 현재 선택 영역의 시작 위치를 가져오기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집에서 현재 선택 영역의 시작 위치를 반환
 */
int ScriptingService::noteTextEditSelectionStart();
```

### 예제

```js
script.log(script.noteTextEditSelectionStart());
```

## 노트 텍스트 편집에서 현재 선택 영역의 끝 위치를 가져오기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집에서 현재 선택 영역의 끝 위치를 반환
 */
int ScriptingService::noteTextEditSelectionEnd();
```

### 예제

```js
script.log(script.noteTextEditSelectionEnd());
```

## 노트 텍스트 편집의 텍스트 커서를 특정 위치로 설정

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집의 텍스트 커서를 특정 위치로 설정
 * 0은 노트의 시작
 * 특수한 경우: -1이 노트의 끝
 *
 * @param position
 */
void ScriptingService::noteTextEditSetCursorPosition(int position);
```

### 예제

```js
// 노트의 11번째 문자로 이동
script.noteTextEditSetCursorPosition(10);

// 노트 끝으로 이동
script.noteTextEditSetCursorPosition(-1);
```

## 노트 텍스트 편집에서 텍스트 커서의 현재 위치 가져오기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집에서 텍스트 커서의 현재 위치를 반환
 * 0은 노트의 시작
 */
int ScriptingService::noteTextEditCursorPosition();
```

### 예제

```js
script.log(script.noteTextEditCursorPosition());
```

## 노트 텍스트 편집에서 현재 단어 읽기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집에서 현재 단어를 읽기
 *
 * @param withPreviousCharacters also get more characters at the beginning
 *                               to get characters like "@" that are not
 *                               word-characters
 * @return
 */
QString ScriptingService::noteTextEditCurrentWord(bool withPreviousCharacters);
```

### 예제

```js
// 노트 텍스트 편집에서 현재 단어 읽기
var text = script.noteTextEditCurrentWord();
```

[autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml) 예제를 살펴보는 것이 좋습니다.

## 노트 텍스트 편집에서 현재 블록 읽기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트 편집에서 현재 블록 읽기
 *
 * @return
 */
QString ScriptingService::noteTextEditCurrentBlock();
```

### 예제

```js
// 노트 텍스트 편집에서 현재 블록 읽기
var text = script.noteTextEditCurrentBlock();
```

다음 예시를 살펴볼 수 있습니다 [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## 현재 선택된 AI 모델에서 완료 프롬프트 사용

AI 완료 프롬프트는 ChatGPT, Claude, Ollama와 같은 시스템과 같이 현재 선택된 AI 모델에서 완료 프롬프트를 사용할 수 있게 해주는 기능입니다.

AI 도구모음 또는 기본 메뉴에서 AI 시스템을 활성화해야 작동할 수 있습니다.

### 메서드 호출 및 매개 변수

```cpp
/**
 * AI Complete를 사용하기 위한 QML 래퍼
 *
 * @param prompt
 * @return {QString} the result of the completer
 */
QString ScriptingService::aiComplete(const QString& prompt);
```

### 예제

```js
// 현재 선택한 AI 모델에 요청하여 프롬프트를 완료합니다
var text = script.aiComplete("Tell me how do you feel today?");
```

다음 예시를 살펴볼 수 있습니다 [ai-autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/ai-autocompletion.qml).

## 플랫폼이 Linux, OS X 또는 Windows인지 확인

### 메서드 호출 및 매개 변수

```cpp
bool ScriptingService::platformIsLinux();
bool ScriptingService::platformIsOSX();
bool ScriptingService::platformIsWindows();
```

### 예제

```js
if (script.platformIsLinux()) {
  // Will be executed only if under Linux
}
```

## 현재 노트에 태그 지정

### 메서드 호출 및 매개 변수

```cpp
/**
 * tagName이라는 태그로 현재 노트에 태그를 지정
 *
 * @param tagName
 */
void ScriptingService::tagCurrentNote(QString tagName);
```

### 예제

```js
// 현재 노트에 "즐겨찾기" 태그 추가
script.tagCurrentNote("favorite");
```

[favorite-note.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/favorite-note.qml) 예제에서 사용자 지정 작업 `favoriteNote`를 살펴볼 수 있습니다.

## 이름을 기준으로 태그 만들기 또는 가져오기 브레드크럼 목록

### 메서드 호출 및 매개 변수

```cpp
/**
 * 태그 이름의 "breadcrumb 목록"을 기준으로 태그를 가져오거나 만들기
 * 요소 nameList[0]은 트리에서 가장 높은 위치에 있을 것입니다 (parentId: 0).
 *
 * @param nameList
 * @param createMissing {bool} if true (default) all missing tags will be created
 * @return TagApi object of deepest tag of the name breadcrumb list
 */
TagApi *ScriptingService::getTagByNameBreadcrumbList(
    const QStringList &nameList, bool createMissing);
```

### 예제

```js
// 세번째 수준까지 모든 태그를 생성하고 태그 트리에서 다음과 같이 표시되는
// "level3" 태그에 대한 태그 개체를 반환합니다:
// level1 > level2 > level3
var tag = script.getTagByNameBreadcrumbList(["level1", "level2", "level3"]);
```

## 이름으로 태그 검색

### 메서드 호출 및 매개 변수

```cpp
/**
 * 이름 필드에서 하위 문자열 검색을 수행하여 모든 태그를 가져오기
 *
 * @param name {QString} name to search for
 * @return {QStringList} list of tag names
 */
QStringList ScriptingService::searchTagsByName(QString name);
```

### 예제

```js
// game이라는 단어가 포함된 모든 태그를 검색
var tags = script.searchTagsByName("game");
```

[autocompletion.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/autocompletion.qml) 예제를 살펴보는 것이 좋습니다.

## 노트 텍스트로 노트 검색

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 텍스트에 특정 텍스트가 있는 모든 노트의 노트 ID 목록을 반환
 *
 * 유감스럽게도 QML에서 QList<NoteApi*>를 사용하는 쉬운 방법이 없으므로
 * 노트 ID만 전송할 수 있습니다
 *
 * @return {QList<int>} list of note ids
 */
QList<int> ScriptingService::fetchNoteIdsByNoteTextPart(QString text);
```

### 예제

```js
var noteIds = script.fetchNoteIdsByNoteTextPart("mytext");

noteIds.forEach(function (noteId) {
  var note = script.fetchNoteById(noteId);

  // do something with the note
});
```

[unique-note-id.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/unique-note-id.qml) 예시를 살펴보는 것도 좋습니다.

## 사용자 지정 스타일시트 추가

### 메서드 호출 및 매개 변수

```cpp
/**
 * 응용프로그램에 사용자 지정 스타일시트 추가
 *
 * @param stylesheet
 */
void ScriptingService::addStyleSheet(QString stylesheet);
```

### 예제

```js
// 노트 목록의 텍스트를 더 크게 만들기
script.addStyleSheet("QTreeWidget#noteTreeWidget {font-size: 30px;}");
```

[custom-stylesheet.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/custom-stylesheet.qml) 예제를 살펴보는 것이 좋습니다.

`*.ui` 파일에서 위젯 이름을 가져올 수 있습니다. 예를 들어 기본 창이 [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui)이 됩니다.

Qt 문서 (예: [QMainWindow](https://doc.qt.io/qt-5/qmainwindow.html))를 통해 위젯이 서로 어떻게 관련되어 있는지 확인할 수 있습니다 (페이지에서 `상속` 검색).

거의 모든 것의 기본 위젯은 [QWidget](https://doc.qt.io/qt-5/qwidget.html)입니다. 예를 들어 `QWidget` `QWidget {background-color: black; color: white;}`로 QWidget을 스타일링하면 모든 배경색이 검정색이고 전경색이 흰색이 됩니다.

::: 팁 [style.qss](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle/style.qss)의 [qdarkstyle](https://github.com/pbek/QOwnNotes/blob/main/src/libraries/qdarkstyle)도 변경할 수 있는 스타일에 대한 좋은 참고 자료가 될 수 있습니다. :::

사용 가능한 스타일에 대한 참조는 [스타일 시트 참조](http://doc.qt.io/qt-5/stylesheet-reference.html)를 참조하세요.

HTML 미리 보기에 스타일을 삽입해 노트가 미리 보는 방식을 변경하고 싶으시다면 [notetomarkdownhtmlhook](hooks.html#notetomarkdownhtmlhook)을 참고하세요.

::: 팁 대화 상자가 실제로 어떻게 생겼는지, 대화 상자의 이름이 무엇인지 확인하려면 [Qt Creator](https://www.qt.io/product/development-tools)를 다운로드하고 `*.ui` 파일을 열면 됩니다. :::

## 스크립팅 엔진 다시 불러오기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 스크립팅 엔진을 다시 로드
 */
void ScriptingService::reloadScriptingEngine();
```

### 예제

```js
// 스크립팅 엔진을 다시 로드
script.reloadScriptingEngine();
```

## 파일 이름으로 노트 가져오기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 파일 이름을 기준으로 노트를 가져오기
 *
 * @param fileName string the file name of the note (mandatory)
 * @param noteSubFolderId integer id of the note subfolder
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteByFileName(QString fileName,
                                                int noteSubFolderId);
```

### 예제

```js
// 파일 이름으로 노트 가져오기
script.fetchNoteByFileName("my note.md");
```

## ID로 노트 가져오기

### 메서드 호출 및 매개 변수

```cpp
/**
 * Id로 노트를 가져오기
 *
 * @param id int the id of the note
 * @return NoteApi*
 */
NoteApi* ScriptingService::fetchNoteById(int id);
```

### 예제

```js
// ID로 노트 가져오기
script.fetchNoteById(243);
```

[export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml) 예시를 살펴보는 것이 좋습니다.

## 파일 이름으로 노트가 있는지 확인하기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 파일 이름으로 노트 파일이 있는지 확인
 *
 * @param fileName string the file name of the note (mandatory)
 * @param ignoreNoteId integer id of a note to ignore in the check
 * @param noteSubFolderId integer id of the note subfolder
 * @return bool
 */
bool ScriptingService::noteExistsByFileName(QString fileName,
                                            int ignoreNoteId,
                                            int noteSubFolderId);
```

### 예제

```js
// 노트가 있는지 확인하지만 "note"의 ID는 무시합니다
script.noteExistsByFileName("my note.md", note.id);
```

[use-tag-names-in-filename.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/use-tag-names-in-filename.qml)의 예를 살펴볼 수 있습니다.

## 클립보드로 텍스트 복사

### 메서드 호출 및 매개 변수

```cpp
/**
 * 텍스트를 일반 텍스트 또는 HTML MIME 데이터로 클립보드에 복사
 *
 * @param text string text to put into the clipboard
 * @param asHtml bool if true the text will be set as html mime data
 */
void ScriptingService::setClipboardText(QString text, bool asHtml);
```

### 예제

```js
// 텍스트를 클립보드에 복사
script.setClipboardText("text to copy");
```

[selected-markdown-to-bbcode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/selected-markdown-to-bbcode.qml) 예를 살펴볼 수 있습니다.

## 노트로 이동

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 목록에 노트가 표시되는 경우 현재 노트를
 *
 * @paramnoteApi 노트로 이동할 수 있도록 설정합니다
 * @@paramasTabbool true이면 노트가 새 탭에서 열립니다 (아직 열리지 않은 경우)
 */
void ScriptingService::setCurrentNote(NoteApi *note, bool asTab = false);
```

### 예제

```js
// 노트로 이동
script.setCurrentNote(note);

// 새 탭에서 열린 노트 (아직 열리지 않은 경우)
script.setCurrentNote(note, true);
```

[journal-entry.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/journal-entry.qml) 예제를 살펴볼 수 있습니다.

## 노트 하위 폴더로 이동

### 메서드 호출 및 매개 변수

```cpp
/**
 * 노트 하위 폴더로 이동
 *
 * @param noteSubFolderPath {QString} path of the subfolder, relative to the note folder
 * @param separator {QString} separator between parts of the path, default "/"
 * @return true if jump was successful
 */
bool ScriptingService::jumpToNoteSubFolder(const QString &noteSubFolderPath,
                                            QString separator);
```

### 예제

```js
// 노트 하위 폴더 "a sub folder"로 이동
script.jumpToNoteSubFolder("a sub folder");

// "a sub folder"의 "sub" 노트 하위 폴더로 이동
script.jumpToNoteSubFolder("a sub folder/sub");
```

::: 팁 [`mainWindow.createNewNoteSubFolder`](classes.html#example-2)를 호출하여 현재 하위 폴더에 새 노트 하위 폴더를 만들 수 있습니다. :::

## 정보 메시지 상자 표시

### 메서드 호출 및 매개 변수

```cpp
/**
 * 정보 메시지 상자 표시
 *
 * @param text
 * @param title (optional)
 */
void ScriptingService::informationMessageBox(QString text, QString title);
```

### 예제

```js
// 정보 메시지 상자 표시
script.informationMessageBox("The text I want to show", "Some optional title");
```

## 질문 메시지 상자 표시

### 메서드 호출 및 매개 변수

```cpp
/**
 * 질문 메시지 상자 표시
 *
 * 버튼에 대한 자세한 내용은 다음을 참조하십시오:
 * https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
 *
 * @param text
 * @param title (optional)
 * @param buttons buttons that should be shown (optional)
 * @param defaultButton default button that will be selected (optional)
 * @return id of pressed button
 */
int ScriptingService::questionMessageBox(
        QString text, QString title, int buttons, int defaultButton);
```

### 예제

```js
// show a question message box with an apply and a help button
// see: https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum
var result = script.questionMessageBox(
  "The text I want to show",
  "Some optional title",
  0x01000000 | 0x02000000,
  0x02000000,
);
script.log(result);
```

버튼에 대한 자세한 내용은 [표준 버튼](https://doc.qt.io/qt-5/qmessagebox.html#StandardButton-enum)를 참조하십시오.

[input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml) 예제도 살펴볼 수 있습니다.

## 열려 있는 파일 대화 상자 표시

### 메서드 호출 및 매개 변수

```cpp
/**
 * 열려 있는 파일 대화 상자 표시
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getOpenFileName(QString caption, QString dir,
                                            QString filter);
```

### 예제

```js
// show an open file dialog
var fileName = script.getOpenFileName(
  "Please select an image",
  "/home/user/images",
  "Images (*.png *.xpm *.jpg)",
);
```

## 파일 저장 대화 상자 표시

### 메서드 호출 및 매개 변수

```cpp
/**
 * 파일 저장 대화 상자 표시
 *
 * @param caption (optional)
 * @param dir (optional)
 * @param filter (optional)
 * @return QString
 */
QString ScriptingService::getSaveFileName(QString caption, QString dir,
                                            QString filter);
```

### 예제

```js
// show a save file dialog
var fileName = script.getSaveFileName(
  "Please select HTML file to save",
  "output.html",
  "HTML (*.html)",
);
```

[export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml) 예제를 살펴볼 수 있습니다.

## 스크립트 설정 변수를 등록

스크립트에서 설정 변수를 속성으로 정의하고 `settingsVariables`라는 속성에 등록해야 합니다.

그런 다음 스크립트 설정에서 이러한 속성을 설정할 수 있습니다.

### 예제

```js
// 나중에 액세스하려면 등록된 변수를 정의해야 합니다
property string myString;
property string myStringSecret;
property bool myBoolean;
property string myText;
property int myInt;
property string myFile;
property string myDirectory;
property string mySelection;

// 사용자가 스크립트 설정에서 설정할 수 있도록 설정 변수를 등록합니다
//
// 안타깝게도 Qt에는 QVariantHash가 없으며,
// QVariantMap (임의의 순서가 없는) 또는 QVariantList(
// 적어도 임의로 주문할 수 있는)만 사용할 수 있습니다
property variant settingsVariables: [
    {
        "identifier": "myString",
        "name": "I am a line edit",
        "description": "Please enter a valid string:",
        "type": "string",
        "default": "My default value",
    },
    {
        "identifier": "myStringSecret",
        "name": "I am a password field",
        "description": "Please enter a valid string:",
        "type": "string-secret",
    },
    {
        "identifier": "myBoolean",
        "name": "I am a checkbox",
        "description": "Some description",
        "text": "Check this checkbox",
        "type": "boolean",
        "default": true,
    },
    {
        "identifier": "myText",
        "name": "I am textbox",
        "description": "Please enter your text:",
        "type": "text",
        "default": "This can be a really long text\nwith multiple lines.",
    },
    {
        "identifier": "myInt",
        "name": "I am a number selector",
        "description": "Please enter a number:",
        "type": "integer",
        "default": 42,
    },
    {
        "identifier": "myFile",
        "name": "I am a file selector",
        "description": "Please select the file:",
        "type": "file",
        "default": "pandoc",
    },
    {
        "identifier": "myDirectory",
        "name": "I am a directory selector",
        "description": "Please select the directory:",
        "type": "directory",
        "default": "/home",
    },
    {
        "identifier": "mySelection",
        "name": "I am an item selector",
        "description": "Please select an item:",
        "type": "selection",
        "default": "option2",
        "items": {"option1": "Text for option 1", "option2": "Text for option 2", "option3": "Text for option 3"},
    }
];
property string myString;
property string myStringSecret;
property bool myBoolean;
property string myText;
property int myInt;
property string myFile;
property string myDirectory;
property string mySelection;

// 사용자가 스크립트 설정에서 설정할 수 있도록 설정 변수를 등록합니다
//
// 안타깝게도 Qt에는 QVariantHash가 없으며, 
// QVariantMap(임의의 순서가 없는) 또는 QVariantList(
// 임의로 주문할 수 있는)만 사용할 수 있습니다
property variant settingsVariables: [
    {
        "identifier": "myString",
        "name": "I am a line edit",
        "description": "Please enter a valid string:",
        "type": "string",
        "default": "My default value",
    },
    {
        "identifier": "myStringSecret",
        "name": "I am a password field",
        "description": "Please enter a valid string:",
        "type": "string-secret",
    },
    {
        "identifier": "myBoolean",
        "name": "I am a checkbox",
        "description": "Some description",
        "text": "Check this checkbox",
        "type": "boolean",
        "default": true,
    },
    {
        "identifier": "myText",
        "name": "I am textbox",
        "description": "Please enter your text:",
        "type": "text",
        "default": "This can be a really long text\nwith multiple lines.",
    },
    {
        "identifier": "myInt",
        "name": "I am a number selector",
        "description": "Please enter a number:",
        "type": "integer",
        "default": 42,
    },
    {
        "identifier": "myFile",
        "name": "I am a file selector",
        "description": "Please select the file:",
        "type": "file",
        "default": "pandoc",
    },
    {
        "identifier": "myDirectory",
        "name": "I am a directory selector",
        "description": "Please select the directory:",
        "type": "directory",
        "default": "/home",
    },
    {
        "identifier": "mySelection",
        "name": "I am an item selector",
        "description": "Please select an item:",
        "type": "selection",
        "default": "option2",
        "items": {"option1": "Text for option 1", "option2": "Text for option 2", "option3": "Text for option 3"},
    }
];
```

In addition, you can override the `settingsVariables` with a special function `registerSettingsVariables()` like this:

### 예제

```js
/**
 * 설정 변수를 다시 등록
 *
 * 운영 체제에 따라 기본값을 설정하는 것과 같이 코드를 사용하여 변수를
 * 재정의하려면 이 방법을 사용하십시오.
 */
function registerSettingsVariables() {
  if (script.platformIsWindows()) {
    // override the myFile default value
    settingsVariables[3].default = "pandoc.exe";
  }
}
```

[variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/variables.qml) 예제를 살펴볼 수도 있습니다.

## 지속 변수 저장 및 로드

### 메서드 호출 및 매개 변수

```cpp
/**
 * Stores a persistent variable
 * These variables are accessible globally over all scripts
 * Please use a meaningful prefix in your key like "PersistentVariablesTest/myVar"
 *
 * @param key {QString}
 * @param value {QVariant}
 */
void ScriptingService::setPersistentVariable(const QString &key,
                                             const QVariant &value);

/**
 * Loads a persistent variable
 * These variables are accessible globally over all scripts
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
 * @return
 */
QVariant ScriptingService::getPersistentVariable(const QString &key,
                                                 const QVariant &defaultValue);
```

### 예제

```js
// store persistent variable
script.setPersistentVariable("PersistentVariablesTest/myVar", result);

// load and log persistent variable
script.log(
  script.getPersistentVariable(
    "PersistentVariablesTest/myVar",
    "nothing here yet",
  ),
);
```

모든 스크립트에서 변수에 액세스할 수 있으므로 `PersistentVariablesTest/myVar`와 같이 키에 의미 있는 접두사를 사용해야 합니다.

[persistent-variables.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/persistent-variables.qml) 예제를 살펴볼 수도 있습니다.

## 응용 프로그램 설정 변수 로드

### 메서드 호출 및 매개 변수

```cpp
/**
 * Loads an application settings variable
 *
 * @param key {QString}
 * @param defaultValue {QVariant} return value if the setting doesn't exist (optional)
 * @return
 */
QVariant ScriptingService::getApplicationSettingsVariable(const QString &key,
                                                          const QVariant &defaultValue);
```

### 예제

```js
// 응용 프로그램 설정 변수 로드 및 로깅
script.log(script.getApplicationSettingsVariable("gitExecutablePath"));
```

설정은 실제로 비어 있을 수 있으므로 직접 관리해야 한다는 점을 명심하세요. `defaultValue`은 설정이 전혀 존재하지 않는 경우에만 사용됩니다.

## 캐시 디렉터리 만들기

시스템의 기본 캐시 위치에 파일을 캐시할 수 있습니다.

### 메서드 호출 및 매개 변수

```cpp
/**
 * 스크립트의 캐시 디렉터리 반환
 *
 * @param {QString} subDir the subfolder to create and use
 * @return {QString} the cache dir path
 */
QString ScriptingService::cacheDir(const QString &subDir) const;
```

### 예제

```js
// my-script-id에 대한 캐시 디렉터리 만들기
var cacheDirForScript = script.cacheDir("my-script-id");
```

## 캐시 디렉터리 지우기

clearCacheDir()에 이름을 전달하여 스크립트의 캐시 디렉터리를 지울 수 있습니다.

### 메서드 호출 및 매개 변수

```cpp
/**
 * 스크립트의 캐시 디렉터리 지우기
 *
 * @param {QString} subDir the subfolder to clear
 * @return {bool} true on success
 */
bool ScriptingService::clearCacheDir(const QString &subDir) const;
```

### 예제

```js
// clear cache directory of my-script-id
script.clearCacheDir("my-script-id");
```

## 스크립트 디렉토리의 경로를 읽기

예를 들어 다른 파일을 로드하기 위해 스크립트가 있는 디렉터리의 경로를 가져와야 하는 경우 스크립트가 있는 디렉터리의 문자열 `property string scriptDirPath;` 속성을 등록해야 합니다. 이 속성은 스크립트의 디렉터리 경로로 설정됩니다.

### 예제

```js
import QtQml 2.0
import QOwnNotesTypes 1.0

Script {
    // 스크립트의 디렉터리 경로가 여기에 설정됩니다
    property string scriptDirPath;

    function init() {
        script.log(scriptDirPath);
    }
}
```

## 경로 구분 기호를 기본 경로 구분 기호로 변환

### 메서드 호출 및 매개 변수

```cpp
/**
 * '/' 구분 기호가 기본 운영 체제에 적합한 구분 기호로
 * 변환된 경로를 반환합니다.
 *
 * On Windows, toNativeDirSeparators("c:/winnt/system32") returns
 * "c:\winnt\system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::toNativeDirSeparators(QString path);
```

### 예제

```js
// Windows에서 "c:\winnt\system32"를 반환
script.log(script.toNativeDirSeparators("c:/winnt/system32"));
```

## 기본 경로 구분 기호에서 경로 구분 기호 변환

### 메서드 호출 및 매개 변수

```cpp
/**
 * '/'을(를) 파일 구분 기호로 사용하여 경로를 반환합니다.
 * On Windows, for instance, fromNativeDirSeparators("c:\\winnt\\system32")
 * returns "c:/winnt/system32".
 *
 * @param path
 * @return
 */
QString ScriptingService::fromNativeDirSeparators(QString path);
```

### 예제

```js
// Windows에서 "c:/winnt/system32"를 반환
script.log(script.fromNativeDirSeparators("c:\\winnt\\system32"));
```

## 기본 디렉토리 구분자를 가져오기

### 메서드 호출 및 매개 변수

```cpp
/**
 * Windows에서 네이티브 디렉터리 구분 기호 "/" 또는 "\"를 반환
 *
 * @return
 */
QString ScriptingService::dirSeparator();
```

### 예제

```js
// Windows에서 "\"를 반환
script.log(script.dirSeparator());
```

## 선택한 모든 노트의 경로 목록 가져오기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 선택한 모든 노트의 경로 목록 반환
 *
 * @return {QStringList} list of selected note paths
 */
QStringList ScriptingService::selectedNotesPaths();
```

### 예제

```js
// 선택한 모든 노트의 경로 목록 반환
script.log(script.selectedNotesPaths());
```

[external-note-diff.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/external-note-diff.qml) 예시를 살펴보는 것이 좋습니다.

## 선택한 모든 노트의 ID 목록 가져오기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 선택한 모든 노트의 ID 목록 반환
 *
 * @return {QList<int>} list of selected note ids
 */
QList<int> ScriptingService::selectedNotesIds();
```

### 예제

```js
// 선택한 모든 노트의 Id 목록 반환
script.log(script.selectedNotesIds());
```

[export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml) 예시를 살펴보는 것이 좋습니다.

## 메뉴 작업 트리거

### 메서드 호출 및 매개 변수

```cpp
/**
 * 메뉴 작업을 트리거
 *
 * @param objectName {QString} object name of the action to trigger
 * @param checked {QString} only trigger the action if checked-state is
 *                          different than this parameter (optional, can be 0 or 1)
 */
void ScriptingService::triggerMenuAction(QString objectName, QString checked);
```

### 예제

```js
// 읽기 전용 모드 전환
script.triggerMenuAction("actionAllow_note_editing");

// disable the read-only mode
script.triggerMenuAction("actionAllow_note_editing", 1);
```

[disable-readonly-mode.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/disable-readonly-mode.qml) 예제를 살펴보는 것이 좋습니다.

::: 팁 메뉴 동작의 개체 이름은 [mainwindow.ui](https://github.com/pbek/QOwnNotes/blob/main/src/mainwindow.ui)에서 가져올 수 있습니다. 영문 메뉴 제목을 검색하면 됩니다. 이 텍스트는 시간이 지남에 따라 변경될 수 있습니다. :::

## 선택 상자를 사용하여 입력 대화 상자 열기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 선택 상자가 있는 입력 대화 상자 열기
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param items {QStringList} list of items to select
 * @param current {int} index of the item that should be selected (default: 0)
 * @param editable {bool} if true the text in the dialog can be edited (default: false)
 * @return {QString} text of the selected item
 */
QString ScriptingService::inputDialogGetItem(
        const QString &title, const QString &label, const QStringList &items,
        int current, bool editable);
```

`취소`를 클릭하거나 `Escape`을 누르면 빈 문자열이 반환됩니다.

### 예제

```js
var result = script.inputDialogGetItem("combo box", "Please select an item", [
  "Item 1",
  "Item 2",
  "Item 3",
]);
script.log(result);
```

[input-dialogs.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/input-dialogs.qml) 예제를 살펴볼 수 있습니다.

## 줄 편집을 사용하여 입력 대화 상자 열기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 줄 편집이 있는 입력 대화 상자를 열기
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param text {QString} text in the dialog (optional)
 * @return
 */
QString ScriptingService::inputDialogGetText(
        const QString &title, const QString &label, const QString &text);
```

`취소`를 클릭하거나 `Escape`을 누르면 빈 문자열이 반환됩니다.

### 예제

```js
var result = script.inputDialogGetText(
  "line edit",
  "Please enter a name",
  "current text",
);
script.log(result);
```

## 여러 줄의 텍스트 편집으로 입력 대화상자 열기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 여러 줄의 텍스트 편집으로 입력 대화 상자 열기
 *
 * @param title {QString} title of the dialog
 * @param label {QString} label text of the dialog
 * @param text {QString} text in the dialog (optional)
 * @return
 */
QString ScriptingService::inputDialogGetMultiLineText(
        const QString &title, const QString &label, const QString &text);
```

`취소`를 클릭하거나 `Escape`을 누르면 빈 문자열이 반환됩니다.

### 예제

```js
var result = script.inputDialogGetMultiLineText(
  "multi-line edit",
  "Please enter a text",
  "current text",
);
script.log(result);
```

## 두 텍스트의 차이점을 표시하는 대화 상자 열기

### 메서드 호출 및 매개 변수

```cpp
/**
* 대화 상자를 열어 두 텍스트의 차이점을 표시하고 사용자가 결과를 편집할 수 있도록 합니다
*
* @param title {QString} title of the dialog
* @param label {QString} label text of the dialog
* @param text1 {QString} first text
* @param text2 {QString} second text
* @return
  */
  QString ScriptingService::textDiffDialog(const QString &title, const QString &label,
                                           const QString &text1, const QString &text2);
```

`text2`는 대화 상자에서 편집할 수 있는 텍스트입니다. `취소`을 클릭하거나 `Esc`를 누른 경우 빈 문자열이 반환됩니다.

### 예제

```js
const text = script.noteTextEditSelectedText();
const aiPrompt = "Translate the text to English";
const aiResult = script.aiComplete(aiPrompt + ":\n\n" + text);

var result = script.textDiffDialog(
  "AI Text Tool",
  "Resulting text",
  text,
  aiResult,
);
script.log(result);
```

## 파일이 있는지 확인하기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 파일이 있는지 확인
 * @param filePath
 * @return
 */
bool ScriptingService::fileExists(QString &filePath);
```

### 예제

```js
var result = script.fileExists(filePath);
script.log(result);
```

## 파일에서 텍스트 읽기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 파일에서 텍스트 읽기
 *
 * @param filePath {QString} path of the file to load
 * @param codec {QString} file encoding (default: UTF-8)
 * @return the file data or null if the file does not exist
 */
QString ScriptingService::readFromFile(const QString &filePath, const QString &codec)
```

### 예제

```js
if (script.fileExists(filePath)) {
  var data = script.readFromFile(filePath);
  script.log(data);
}
```

## 파일에 텍스트 쓰기

### 메서드 호출 및 매개 변수

```cpp
/**
 * 파일에 텍스트 쓰기
 *
 * @param filePath {QString}
 * @param data {QString}
 * @param createParentDirs {bool} optional (default: false)
 * @return
 */
bool ScriptingService::writeToFile(const QString &filePath, const QString &data, bool createParentDirs);
```

### 예제

```js
var result = script.writeToFile(filePath, html);
script.log(result);
```

[export-notes-as-one-html.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/export-notes-as-one-html.qml) 예제를 살펴볼 수 있습니다.

## 웹 소켓 작업

`WebSocketServer`를 사용하여 QOwnNotes를 원격으로 제어할 수 있습니다.

[websocket-server.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-server.qml) 예제를 참조하십시오. [Websockettest](https://www.websocket.org/echo.html?location=ws://127.0.0.1:35345)에서 소켓 서버에 연결하여 테스트할 수 있습니다.

`WebSocket`으로 소켓을 들을 수도 있습니다. [websocket-client.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/websocket-client.qml) 예제를 참조하십시오.

이를 사용하려면 Qt의 QML `websocket` 라이브러리가 설치되어 있어야 합니다. 예를 들어 Ubuntu Linux에서 `qml-module-qtwebsockets`을 설치할 수 있습니다.

## 편집기에 대한 강조 표시 규칙 추가

정규식을 정의하고 강조 표시 상태에 할당하여 강조 표시 규칙을 편집기에 직접 주입할 수 있습니다.

### 메소드 호출 및 매개변수

```cpp
/**
 * 편집기의 구문 강조 표시에 강조 표시 규칙을 추가
 *
 * @param pattern {QString} the regular expression pattern to highlight
 * @param shouldContain {QString} a string that must be contained in the highlighted text for the pattern to be parsed
 * @param state {int} the state of the syntax highlighter to use
 * @param capturingGroup {int} the capturing group for the pattern to use for highlighting (default: 0)
 * @param maskedGroup {int} the capturing group for the pattern to use for masking (default: 0)
 */
void ScriptingService::addHighlightingRule(const QString &pattern,
                                           const QString &shouldContain,
                                           int state,
                                           int capturingGroup,
                                           int maskedGroup);
```

### 강조 표시 상태

| 이름        | 번호 |
| --------- | -- |
| 상태 없음     | -1 |
| 링크        | 0  |
| 이미지       | 3  |
| 코드 블록     | 4  |
| 코드 블록 주석  | 5  |
| 기울임꼴      | 7  |
| 굵게        | 8  |
| 목록        | 9  |
| 주석        | 11 |
| H1        | 12 |
| H2        | 13 |
| H3        | 14 |
| H4        | 15 |
| H5        | 16 |
| H6        | 17 |
| 블럭 따옴표    | 18 |
| 수평 눈금자    | 21 |
| 표         | 22 |
| 들여쓰기코드블록  | 23 |
| 마스크된 구문   | 24 |
| 현재 줄 배경색  | 25 |
| 끊어진 링크    | 26 |
| 프론트매터블록   | 27 |
| 후행 공간     | 28 |
| 확인란 선택 취소 | 29 |
| 확인란 선택됨   | 30 |
| 세인트 밑줄    | 31 |

### 예제

```js
// 블록: 일부 텍스트와 같은 텍스트 줄을 블록 따옴표 (상태 18)로 강조 표시
script.addHighlightingRule("^BLOCK: (.+)", "BLOCK:", 18);

// 한 줄에서 32자 이후의 모든 문자를 마스크 아웃(상태 24)합니다
// 은 패턴의 첫 번째 괄호로 묶인 부분의 표현식이 강조 표시됨을 의미합니다
// 은 마스킹을 수행하지 않아야 함을 의미합니다
script.addHighlightingRule("^.{32}(.+)", "", 24, 1, -1);
```

또한 [highlighting.qml](https://github.com/pbek/QOwnNotes/blob/main/docs/scripting/examples/highlighting.qml)의 예를 볼 수 있습니다.
