# Installieren unter Gentoo Linux

Fügen Sie das QOwnNotes-Repository mit dem folgenden Befehl hinzu (siehe [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Dann können Sie QOwnNotes wie gewohnt installieren.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[QOwnNotes-Overlay auf GitHub](https://github.com/qownnotes/gentoo-overlay/)
