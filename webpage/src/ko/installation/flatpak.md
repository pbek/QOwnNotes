# Flatpak으로 설치

[Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes)에 유지 관리되는 커뮤니티인 Flatpak `org.qownnotes.QOwnNotes`가 있습니다.

다음을 입력하여 설치할 수 있습니다:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

그런 다음 `flatpak run org.qownnotes.QOwnNotes` 명령으로 QOwnNotes를 실행할 수 있어야 합니다.

::: 팁 정보 [QOwnNotes Flatpak Issues](https://github.com/flathub/org.qownnotes.QOwnNotes/issues)에서 Flatpak 패키지 관련 이슈를 열어주세요. :::
