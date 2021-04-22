# Installa su Gentoo Linux

Aggiungi il repository QOwnNotes con il seguente comando (vedi [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Quindi puoi installare QOwnNotes nel solito modo.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[QOwnNotes overlay su GitHub](https://github.com/qownnotes/gentoo-overlay/)
