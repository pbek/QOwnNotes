# Install on Gentoo Linux

Agregue el repositorio QOwnNotes con el siguiente comando (consulte [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Luego puede instalar QOwnNotes de la forma habitual.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[Superposición de QOwnNotes en GitHub](https://github.com/qownnotes/gentoo-overlay/)
