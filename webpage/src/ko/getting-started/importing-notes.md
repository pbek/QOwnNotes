# 노트 가져오기

## Evernote

`노트 / 가져오기` 메뉴에서 Evernote 가져오기 대화 상자를 찾을 수 있습니다.

## Joplin

` 노트 / 가져오기 ` 메뉴에서 Joplin 가져오기 대화상자를 찾을 수 있습니다.

## Tomboy

[Trombone](https://github.com/samba/trombone)이라는 스크립트를 사용하여 Tomboy 노트를 가져올 수 있습니다.

스크립트가 작동하도록 python2를 설치할 수 있습니다. 그러면 Tomboy 노트가 Evernote `.enex` 파일로 변환되어 QOnNotes로 가져올 수 있습니다.

이 작업을 원하는 사람은 먼저 Python2가 설치되어 있는지 확인하고 `python-is-python2`도 설치할 수 있습니다 (나중에 제거할 수 있습니다):

```bash
sudo apt install python2 python-is-python2
```

GitHub에서 trombone 파일을 다운로드하여 압축을 풀고 해당 폴더로 이동합니다:

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

특정 노트에 대한 유니코드 오류가 발생하면 각 노트를 제거하고 `저장...`이라는 메시지가 나타날 때까지 변환을 다시 실행하십시오. `EXPORT.enex`라는 파일이 생성되어 QOwnNotes로 가져올 수 있습니다.

Tomboy에는 이러한 기능이 없으므로 QOnNotes로 가져오는 동안 작성/수정 날짜를 제외한 모든 속성의 선택을 취소할 수 있습니다.
