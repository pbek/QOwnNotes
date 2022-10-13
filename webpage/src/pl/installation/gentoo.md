# Zainstaluj na Gentoo Linux

Dodaj repozytorium QOwnNotes za pomocą następującego polecenia (zobacz [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

Następnie możesz zainstalować QOwnNotes w zwykły sposób.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[Nakładka QOwnNotes na GitHub](https://github.com/qownnotes/gentoo-overlay/)
