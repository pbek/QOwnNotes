# Installeer op Gentoo Linux

Voeg de QOwnNotes-repository toe met het volgende commando (zie [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Vervolgens kunt u QOwnNotes op de gebruikelijke manier installeren.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[QOwnNotes-overlay op GitHub](https://github.com/qownnotes/gentoo-overlay/)
