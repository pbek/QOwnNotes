# Gentoo Linux에 설치

다음 명령을 사용하여 QOwnNotes 저장소를 추가합니다 ([eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository) 참조).

```bash
sudo eselect repository enable qownnotes-overlay
```

그런 다음 일반적인 방법으로 QOwnNotes를 설치할 수 있습니다.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[GitHub의 QOwnNotes 오버레이](https://github.com/qownnotes/gentoo-overlay/)
