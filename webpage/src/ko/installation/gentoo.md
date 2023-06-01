# Install on Gentoo Linux

Add the QOwnNotes repository with the following command (see [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Then you can install QOwnNotes the usual way.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[QOwnNotes overlay on GitHub](https://github.com/qownnotes/gentoo-overlay/)
