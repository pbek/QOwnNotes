# نصب در جنتو لینوکس

مخزن QOwnNotes را با فرمان زیر اضافه کنید ( [eselect-repository](https://wiki.gentoo.org/wiki/Eselect/Repository) را ملاحظه کنید).

```bash
sudo eselect repository enable qownnotes-overlay
```

سپس طبق معمول می توانید QOwnNotes را نصب کنید.

```bash
sudo emerge --sync qownnotes-overlay && sudo emerge -av qownnotes
```

[QOwnNotes با گیت هاب همپوشانی می کند](https://github.com/qownnotes/gentoo-overlay/)
