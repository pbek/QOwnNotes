# Telepítés a Gentoo Linux rendszerre

Adja hozzá a QOwnNotes adattárat a következő paranccsal (lásd: [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Ezután telepítheti a QOwnNotes programot a szokásos módon.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[QOwnNotes overlay a GitHubon](https://github.com/qownnotes/gentoo-overlay/)
