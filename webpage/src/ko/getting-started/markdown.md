# 마크다운

이 마크다운 속임수는 QOwnNotes의 마크다운 구문에 대한 빠른 참조 및 설명을 제공하기 위한 것입니다.

## 제목

제목을 사용하여 텍스트를 구성합니다.

```markdown
# H1

## H2

### H3

#### H4

##### H5

###### H6
```

::: 팁 **네비게이션 패널**은 제목의 구조를 보여줍니다. :::

또는 H1과 H2의 경우 밑줄이 있는 스타일:

```markdown
# Alt-H1

## Alt-H2
```

::: 팁 기본적으로 QOwnNotes는 **머리글 1** (h1)에서 **노트의 파일 이름**을 작성합니다. :::

## 강조

```markdown
Emphasis, aka italics, with _asterisks_.

**애스터리스크**로 굵게 강조합니다.
```

[단축키](./shortcuts.md) <kbd>Ctrl + B</kbd>를 사용하여 텍스트를 굵게 표시하고 <kbd>Ctrl + I</kbd>을 사용하여 기울임꼴로 표시할 수 있습니다. :::

## 밑줄

There also is an optional setting to enable underline rendering in the _Preview settings_.

```markdown
_밑줄_
```

:::  팁 [단축키](./shortcuts.md) <kbd>Ctrl + U</kbd>를 사용하여 텍스트에 밑줄을 표시할 수 있습니다. :::

## 취소선

```markdown
~~취소선~~
```

::: 팁 [단축키](./shortcuts.md) <kbd>Alt + Shift + S</kbd>을 사용하여 텍스트를 삭제할 수 있습니다. :::

## 목록

목록을 만드는 방법은 여러 가지가 있습니다.

```markdown
1. 첫 번째 정렬된 목록 항목
2. 다른 항목

- 정렬되지 않은 하위 목록입니다.

1. Actual numbers don't matter, just that it's a number
1. Ordered sub-list (only works in the editor, not the preview)
1. 그리고 다른 아이템입니다.

- Unordered list can use asterisks

* Or minuses

- Or pluses
```

::: 팁 목록 끝에 있는 <kbd>Enter<0>을 누르면 새 목록 항목이 생성됩니다. :::</p>

<h2 spaces-before="0">
  링크
</h2>

<p spaces-before="0">
  링크를 쉽게 사용할 수 있는 두 가지 방법은 웹 페이지나 기타 노트를 가리킵니다. 이러한 링크는 여러 가지 방법으로 볼 수 있습니다.
</p>

<h3 spaces-before="0">
  외부 링크
</h3>

<pre><code class="markdown">[저는 인라인 스타일 링크입니다](https://www.google.com)

[저는 제목이 있는 인라인 스타일 링크입니다](https://www.google.com "Google's Homepage")

[참조 스타일 링크 정의에 숫자를 사용할 수 있습니다][1]

각 괄호 안의 일반 URL과 URL은 미리보기에서 자동으로 링크로 바뀝니다.
http://www.example.com 또는 &lt;http://www.example.com&gt;

[1]: https://www.qownnotes.org
</code></pre>

<h3 spaces-before="0">
  내부 링크
</h3>

<p spaces-before="0">
  다음과 같은 다른 노트나 첨부 파일에 연결할 수 있습니다:
</p>

<pre><code class="markdown">[Journal.md 노트에 링크합니다] (Journal.md)
&lt;Journal.md&gt; 비슷하게 작동합니다.
</code></pre>

<p spaces-before="0">
  ::: 팁 <a href="./shortcuts.md">단축키</a> <kbd>Ctrl + L</kbd>을 사용하여 웹 페이지 또는 <strong x-id="1">기타 노트에 대한 링크를 만들 수 있습니다</strong>.
</p>

<p spaces-before="0">
  <kbd>Ctrl + Shift + X</kbd>를 사용하면 노트에 <strong x-id="1">첨부 파일을 삽입</strong>하는 데 도움이 되는 대화 상자가 나타납니다.
</p>

<p spaces-before="0">
  노트의 링크에서 커서를 편집하여 링크를 따라가는 동안 <kbd>Ctrl + Space</kbd>를 누를 수 있습니다. :::
</p>

<p spaces-before="0">
  ::: 경고 정보 제목이 있는 내부 링크는 지원되지 않습니다:
</p>

<pre><code class="markdown">[Journal.md 노트에 링크합니다](Journal.md "제목 지원되지 않음")
</code></pre>

<p spaces-before="0">
  :::
</p>

<h3 spaces-before="0">
  북마크
</h3>

<p spaces-before="0">
  <a href="./browser-extension.md">QOwnNotes Web Companion 브라우저 확장</a>에서 사용되는 북마크는 목록의 연결을 사용합니다.
</p>

<pre><code class="markdown">- [웹페이지 이름](https://www.example.com) #tag1 #tag2 일부 설명 및 태그
</code></pre>

<h2 spaces-before="0">
  이미지
</h2>

<p spaces-before="0">
  이미지를 QOwnNotes에 포함할 수 있습니다. 미리보기에 표시됩니다.
</p>

<pre><code class="markdown">![alt text](media/my-image.jpg)
</code></pre>

<p spaces-before="0">
  ::: 팁 <a href="./shortcuts.md">단축키</a> <kbd>Ctrl + Shift + I</kbd> 를 사용하여 노트에 이미지를 삽입할 수 있습니다. 이미지가 클립보드에 있을 수도 있으며 대화상자가 이미지를 감지하고 미리보기를 표시합니다.
</p>

<p spaces-before="0">
  <kbd>Ctrl + Shift + V</kbd>을 사용하여 클립보드의 이미지를 노트에 직접 붙여넣을 수도 있습니다. :::
</p>

<p spaces-before="0">
  ::: 경고 정보 제목이 있는 이미지 링크는 지원되지 않습니다:
</p>

<pre><code class="markdown">![alt text](media/my-image.jpg "제목 지원되지 않음")
</code></pre>

<p spaces-before="0">
  :::
</p>

<h2 spaces-before="0">
  인라인 코드 및 코드 블록
</h2>

<pre><code class="markdown">인라인 '코드' 주위에 '백틱'이 있습니다.
</code></pre>

<p spaces-before="0">
  ::: 팁 선택한 인라인 텍스트 또는 텍스트 바로 안쪽에서 <a href="./shortcuts.md">단축키</a> <kbd>Ctrl + Shift + C</kbd>를 사용하여 인라인 코드 블록을 만들 수 있습니다. :::
</p>

<p spaces-before="0">
  코드 블록은 세 개의 백 틱이 있는 선으로 울타리를 두르거나 네 개의 공백으로 들여씁니다.
</p>

<h3 spaces-before="0">
  4-공간 울타리
</h3>

<p spaces-before="0">
  코드 앞에 공백 4개를 추가하여 코드 블록으로 표시합니다.
</p>

<pre><code class="markdown">    s = "공백 들여쓰기 코드"
    print s
</code></pre>

<h3 spaces-before="0">
  백틱 울타리
</h3>

<p spaces-before="0">
  세 개의 백틱을 사용하여 코드 블록을 만들 수도 있습니다.
</p>

<p spaces-before="0">
  ````마크다운
</p>

<pre><code>코드는 여기에 있습니다
코드는 여기에 있습니다
</code></pre>

<pre><code>
::: 팁
 [shortcut](./shortcuts.md) &lt;kbd>Ctrl + Shift + C&lt;/kbd>를 선택한
 여러 텍스트 행 또는 빈 행에서 사용하여 코드 블록을 만들 수 있습니다.
:::

### 코드 강조 표시가 있는 백틱 울타리

QOwnNotes에는 코드 블록을 사용하여 구문을 강조 표시하는 기능도 있습니다.

````markdown
```bash
# I am a comment
cd Notes
</code></pre>

<pre><code>
현재 지원되는 언어 (및 코드 블록 식별자)는 다음과 같습니다:

- BASh scripting, `bash`
- C, `c`
- C++, `cpp`
- C++, `cxx`
- C++, `c++`
- C#, `c#`
- CMake, `cmake`
- C#, `csharp`
- CSS, `css`
- Go, `go`
- HTML, `html`
- INI, `ini`
- Java, `java`
- JavaScript, `javascript`
- JavaScript, `js`
- JSON, `json`
- Makefile, `make`
- PHP, `php`
- Python, `py`
- Python, `python`
- QML, `qml`
- Rust, `rust`
- Shell scripting, `sh`
- SQL, `sql`
- TypeScript, `ts`
- TypeScript, `typescript`
- V, `v`
- Vex, `vex`
- XML, `xml`
- YAML, `yml`
- YAML, `yaml`

## 테이블

```마크다운
콜론은 열을 정렬하는 데 사용할 수 있습니다.

| Tables        |      Are      |  Cool |
| ------------- | :-----------: | ----: |
| col 3 is      | right-aligned | $1600 |
| col 2 is      |   centered    |   $12 |
| zebra stripes |   are neat    |    $1 |

There must be at least 3 dashes separating each header cell.

인라인 마크다운을 사용할 수도 있습니다.

| Markdown | Less      | Pretty     |
| -------- | --------- | ---------- |
| _Still_  | `renders` | **nicely** |
| 1        | 2         | 3          |
</code></pre>

<p spaces-before="0">
  ::: 팁 <kbd>Alt + Shift + T</kbd>을 눌러 표 작성에 도움이 되는 대화상자를 활성화합니다. 이 대화 상자에서 CSV 파일을 가져올 수도 있습니다.
</p>

<p spaces-before="0">
  마크다운 테이블 내부의 <kbd>Ctrl + Space</kbd>를 사용하여 자동으로 형식을 지정합니다. :::
</p>

<h2 spaces-before="0">
  블럭 따옴표
</h2>

<pre><code class="markdown">&gt; 이메일에서 블록 따옴표는 회신 텍스트를 에뮬레이트하는 데 매우 유용합니다.
&gt; 이 줄은 같은 인용구의 일부입니다.

따옴표로 묶습니다.

&gt; 줄 바꿈이 끝날 때도 여전히 적절하게 따옴표로 묶일 수 있는 매우 긴 줄입니다. 모든 사람이 실제로 감싸기에 충분한 길이인지 확인하기 위해 계속 작성해 보겠습니다. Oh, you can _put_ **Markdown** into a blockquote.
</code></pre>

<p spaces-before="0">
  ::: tip You can tell QOwnNotes to fully highlight blockquotes or just the blockquote character in the <em x-id="4">Editor settings</em>
</p>

<p spaces-before="0">
  <a href="./shortcuts.md">단축키</a> <kbd> Ctrl + Shift + B</kbd>을 사용하여 텍스트를 블록 따옴표로 표시할 수 있습니다. :::
</p>

<h2 spaces-before="0">
  수평 규칙
</h2>

<p spaces-before="0">
  수평 규칙을 얻는 세 가지 방법이 있습니다: 하이픈, 별표 또는 밑줄을 사용할 수 있습니다.
</p>

<pre><code class="markdown">3개 이상...

Hyphens

---

Asterisks

---

Underscores

---
</code></pre>

<h2 spaces-before="0">
  줄 바꿈
</h2>

<ul>
  <li>
    문단을 하나 이상의 행으로 분할하여 편집할 수 있지만, 문단은 계속해서 중단 없이 단일 문단으로 렌더링됩니다.
  </li>
  <li>
    두 개의 공백으로 선을 종료하여 문단 내부에서 줄 바꿈을 수행할 수 있습니다.
  </li>
  <li>
    빈 줄로 구분하여 별도의 단락을 만들 수 있습니다.
  </li>
</ul>

<p spaces-before="0">
  ::: 팁 <kbd>⇧ Shift</kbd> + <kbd>Return</kbd>으로 공백 2개와 줄 바꿈을 입력할 수 있습니다. :::
</p>

<pre><code class="markdown">여기부터 시작할 수 있는 줄이 있습니다.

This line is separated from the one above by two newlines, so it will be a _separate paragraph_.

This line is also begins a separate paragraph, but...  
This line is only separated by two trailing spaces and a single newline, so it's a separate line in the _same paragraph_.
</code></pre>

<p spaces-before="0">
  ::: 팁
후행 공백은 편집기에서 기본적으로 강조 표시됩니다.
:::
</p>

<h2 spaces-before="0">
  주석
</h2>

<p spaces-before="0">
  설명은 미리보기에 표시되지 않습니다.
</p>

<pre><code class="markdown">[comment]: # "This comment will not appear in the preview"

&lt;!-- HTML comments are also hidden --&gt;
</code></pre>

<p spaces-before="0">
  ::: 팁
노트의 선행 html 주석 블록은 자동 노트 파일 이름 생성에서도 무시됩니다.
:::
</p>

<h2 spaces-before="0">
  확인란 목록
</h2>

<p spaces-before="0">
  확인란 목록을 사용하여 간단한 작업관리 목록을 작성할 수 있습니다.
</p>

<pre><code class="markdown">- [x] 완료
- [ ] 할 일
</code></pre>

<p spaces-before="0">
  ::: 팁
미리보기에서 확인란을 선택하거나 선택 취소할 수 있습니다.
:::
</p>

<h2 spaces-before="0">
  전문
</h2>

<p spaces-before="0">
  QOwnNotes에서 전문 (예: YAML)을 사용하여 추가 메타 정보를 추가할 수 있습니다. <strong x-id="1">미리보기에 표시되지 않으며</strong> <strong x-id="1">자동 노트 파일 이름 생성을 방해하지 않습니다</strong>.
</p>

<pre><code class="markdown">---
제목: 일부 이름
description: 일부 설명
---

# 노트 헤드라인이 여기에서 시작됩니다

일부 텍스트
</code></pre>

<p spaces-before="0">
  이 예제 노트의 파일 이름은 <code>Note headline starts here.md</code>입니다.
</p>
