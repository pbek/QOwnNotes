# التثبيت كحزمة Flatpak

يتعهد المجتمع حزمة Flatpak تسمى `org.qownnotes.QOwnNotes` على [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

يمكنك تثبيتها بكتابة:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

بعدئذٍ يمكنك تشغيل QOwnNotes بالأمر `flatpak run org.qownnotes.QOwnNotes`.
