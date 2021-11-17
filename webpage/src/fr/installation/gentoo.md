# Installer sur Gentoo Linux

Ajoutez le dépôt QOwnNotes avec la commande suivante (voir [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Vous pouvez ensuite installer QOwnNotes de la manière habituelle.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[Overlay de QOwnNotes sur GitHub](https://github.com/qownnotes/gentoo-overlay/)
