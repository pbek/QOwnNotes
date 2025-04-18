# 노트 가져오기

## Evernote

There is an Evernote import dialog you can reach in the `Note / Import` menu to import _Enex_ files.

이미지, 첨부 파일, 태그, 기본 형식 및 메타데이터도 가져옵니다.

## Joplin

`노트 /가져오기` 메뉴에서 도달할 수 있는 Joplin 가져오기 대화상자가 있습니다.

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
