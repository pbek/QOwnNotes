# التثبيت على Gentoo Linux

أضف مستودع QOwnNotes بالأمر التالي (انظر [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository)).

```bash
sudo eselect repository enable qownnotes-overlay
```

عندئذٍ يمكنك تثبيت QOwnNotes بالطريقة المعتادة.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[QOwnNotes overlay على GitHub](https://github.com/qownnotes/gentoo-overlay/)
