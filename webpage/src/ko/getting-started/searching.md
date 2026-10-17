# 노트 검색

노트를 검색하거나 노트 내부의 텍스트를 검색하려면 바로 가기 <kbd>Ctrl</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd> (또는 macOS에서 <kbd>⌘ Cmd</kbd> + <kbd>⇧ Shift</kbd> + <kbd>F</kbd>)를 사용하여 _노트 검색 패널_로 이동할 수 있습니다.

노트를 검색하는 방법은 여러 가지가 있습니다:

- `note book`은 파일 이름 또는 노트 텍스트에서 `note`과 `book` 을 모두 포함하는 노트를 검색합니다
- `"노트북"`은 파일 이름 또는 노트 텍스트에서 텍스트 노트와 정확히 일치하는 `노트북`을 검색합니다
- `n:북` 또는 `이름:북`은 노트 이름 또는 파일 이름에 `북` 이 있는 노트를 검색합니다
- `n:"note book"` or `name:"note book"` will search for notes with exactly `note book` just in the note name or file name
- `w:note` 또는 `word:note`는 `note`라는 단어 전체를 검색하며, `notebook`과 같은 부분 일치는 제외합니다
- 검색 연산자는 조합하여 사용할 수 있습니다. 예를 들어 `n:w:note`는 노트 이름이나 파일 이름에서 `note`라는 단어 전체를 검색합니다

![검색](/img/searching.png)

::: tip

검색은 최소 두 개의 문자를 입력한 후에 시작됩니다.
:::

## Ignoring accents

If you turn on _Ignore accents in search_ in the _Panels settings_, searches without accented letters will also find text with accented letters. For example, `avion` will also find `Avión`, and `strasse` will also find `Straße`.

- Searches that contain accented letters still only find exact matches, so `schön` will not find `schon`
- Only Latin letters are affected, characters of other scripts are always matched exactly
- The setting is used for the note search, the highlighting of the found text in the note editor and preview, and for the filters of the tag, note subfolder and navigation panels
