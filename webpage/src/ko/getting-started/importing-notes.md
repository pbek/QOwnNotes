# 노트 가져오기

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu to import _Enex_ files.

이미지, 첨부 파일, 태그, 기본 형식 및 메타데이터도 가져옵니다.

## Joplin

`노트 /가져오기` 메뉴에서 도달할 수 있는 Joplin 가져오기 대화상자가 있습니다.

## Obsidian

1. 커뮤니티 플러그인 ["Better Markdown Links"](https://github.com/mnaoumov/obsidian-better-markdown-links)을 설치하여 모든 _위키링크 및 Obsidian flavoured 마크다운 링크 (OFM)_을 변환하고 Obsidian이 _표준 마크다운 링크_로 작동하도록 합니다.

2. QOwnNotes에서 동일한 파일 (동일한 볼트)을 엽니다. QOwnNotes 스크립트 [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others)를 설치하고 스크립트 패널에서 "ObsidianImport" 가져오기 버튼을 눌러 모든 파일을 변환하세요.

QOwnNotes에서 Obsidian으로 내보내기는 필요하지 않습니다.

::: 경고 경고 _주의(!)_: 플러그인 `"Better Markdown Links"`는 Obsidian을 열거나 폴더, 파일 또는 제목의 이름이 변경될 때 링크를 자동으로 조정합니다. Obsidian에서 파일을 변경할 때마다 QOwnNotes에서 "Obsidian 가져오기" 버튼을 누르세요. 그렇지 않으면 일부 링크가 QOwnNotes에서 인식되지 않을 수 있습니다! :::

## VS Codium / Github / Markor / Typora / Pandoc 및 기타 (Github flavoured markdown GFM)

Githubflavoured markdown (GFM)은 하이픈 스타일 링크 제목(공백을 하이픈으로 변환)을 사용합니다. A reference to a heading such as `## Heading with spaces` for example will look like `[notelink](β_note.md#heading-with-spaces)`.

QOwnNotes와 Obsidian (Obsidian flavoured markdown OFM)은 _urlencoded (percentencoded) heading fragments_  (공백을 %20으로 변환하고 대문자를 유지)을 사용하므로 동일한 링크가 다르게 표시됩니다. `[notelink](%CE%B2_note.md#Heading%20with%20spaCes)`.

GFM과 QOwnNotes/Obsidian (OFM) 파일 형식 간 변환은 QOwnNotes에서 해당 파일을 열기만 하면 됩니다. QOwnNotes 스크립트 [link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others](https://github.com/qownnotes/scripts/tree/main/link-importer-and-converter-for-obsidian-github-pandoc-markor-and-others)를 설치하고 스크립트 패널에서 "GFMImport" 가져오기 버튼을 눌러 모든 파일을 변환하세요.

QOwnNotes에서 GFM으로 내보내기는 단일 페이지 (pandoc을 사용한 연속 변환에 유용) 또는 모든 파일에 대해 작동합니다.

::: 경고 경고 _주의_: 특정 경우에 제목 링크의 정보가 손실될 수 있습니다! `β_note.md`라는 노트가 있고, 그 안에 `## 장점`과 같이 동일한 제목이 _같은 (!) 노트_ 안에 여러 개 있다고 가정해 봅시다. GFM은 `[notelink](%CE%B2_note.md#advantages), [notelink](%CE%B2_note.md#advantages-1), [notelink](%CE%B2_note.md#advantages-2), ...`와 같이 각 제목을 참조할 수 있도록 합니다. _QOwnNotes와 Obsidian 모두 이를 허용하지 않습니다!_ QOwnNotes와 Obsidian은 같은 문서 내에서 각 항목을 참조하려면 고유한 제목 이름이 필요합니다! 이 기능은 전혀 지원되지 않으므로 변환 후 해당 링크는 모두 특정 이름 `[notelink](%CE%B2_note.md#advantages)`을 가진 첫 번째 제목을 가리키게 됩니다 (특히 긴 문서에서 그렇습니다)! 임시 해결책으로 제목에 텍스트를 추가하고 먼저 제목 링크를 업데이트하세요 (링크가 고유하도록 설정). :::

## 다양한 형식

명령줄 도구 [jimmy](https://github.com/marph91/jimmy)는 다양한 형식 (예: [Google Keep](https://marph91.github.io/jimmy/formats/google_keep/), [Synology Note Station](https://marph91.github.io/jimmy/formats/synology_note_station/) 및 [more](https://marph91.github.io/jimmy/))을 마크다운으로 변환할 수 있습니다. 가능한 한 많은 메타데이터를 보존하려면 [추가 조정](https://marph91.github.io/jimmy/import_instructions/#qownnotes)을 적용하는 것이 좋습니다.

전환에 문제가 있는 경우 [GitHub](https://github.com/marph91/jimmy/issues)에서 티켓을 자유롭게 개설하세요.

## Google Keep

> Google 테이크아웃을 사용하여 Keep 작업 다운로드
> 
> 압축파일 추출
> 
> 추출된 Keep 폴더 옆의 디렉터리에 keep_to_markdown.py를 다운로드
> 
>     python keep_to_markdown.py Keep out/
>     
> 
> JSON 형식의 노트가 들어 있는 폴더로 Keep을 대체합니다. 아웃 폴더가 없으면 아웃 폴더가 생성됩니다.

<https://gitlab.com/-/snippets/2002921>에서

## Tomboy

Tomboy 노트는 [Trombone](https://github.com/samba/trombone)이라는 스크립트를 사용하여 가져올 수 있습니다.

python2를 설치하여 스크립트를 작동시킬 수도 있습니다. 그러면 Tomboy 노트가 Evernote `.enex` 파일로 변환되고, 그러면 QOwnNotes로 가져올 수 있습니다.

이 작업을 원하는 사용자는 먼저 Python2가 설치되어 있는지 확인하고 `python-is-python2`도 설치할 수 있습니다 (나중에 제거할 수 있습니다). 이 옵션이 있다면 chroot 또는 Docker 컨테이너에서 보다 쉽고 안전하게 사용할 수 있습니다 (여기에 나중에 문서화됨).

```bash
sudo apt install python2 python-is-python2
```

GitHub에서 trombone 파일을 다운로드한 후 압축을 풀고 폴더로 이동합니다:

```bash
cd ~/Downloads/trombone-master
```

그런 다음 tomboy 노트가 있는 폴더에 cd를 넣습니다:

```bash
 cd ~/.local/share/tomboy/
```

그런 다음 실행:

```bash
find ./ -type f -name '*.note' -print0 | xargs -0 trombone > EXPORT.enex
```

특정 노트에 대한 유니코드 오류가 발생하면 각 노트를 제거하고 `저장...`이라는 메시지가 나타날 때까지 변환을 다시 실행하십시오. 그러면 `EXPORT.enex`라는 파일을 QOwnNotes로 가져올 수 있습니다.

QOwnNotes로 가져오는 동안 생성/수정 날짜를 제외한 모든 속성의 선택을 취소할 수 있습니다, Tomboy에는 해당 기능이 없기 때문입니다.
