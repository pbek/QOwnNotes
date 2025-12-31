# Git 버전 관리

QOwnNotes를 설정하여 노트 폴더의 git 저장소에 로컬로 노트 폴더의 변경 사항을 자동으로 커밋할 수 있습니다.

이는 _일반 설정_의 _Git 버전 관리_ 설정에서 수행할 수 있습니다.

![git-note-folder-settings](/img/git/settings.png)

시스템에 git이 설치되어 있어야 하고, git 실행 파일이 PATH에 있어야 합니다
_Git 버전 관리_ 설정에서 git 실행 파일의 경로를 설정할 수 있습니다.

또한 git 버전을 사용하려는 각 노트 폴더에 대해 git 지원을 켜야 합니다!

![git-note-folder-settings](/img/git/note-folder-settings.png)

::: tip
Don't forget to set your git username and email address in the terminal:

```bash
git config --global user.email "사용자 이메일@domain.com"
git config --global user.name "사용자 이름"
```

:::
