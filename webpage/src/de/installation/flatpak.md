# Als Flatpak installieren

Es gibt ein von der Gemeinschaft gepflegtes Flatpak `org.qownnotes.QOwnNotes` auf [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

Sie können es installieren, indem Sie Folgendes eingeben:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Afterwards you should be able to run QOwnNotes with the command `flatpak run org.qownnotes.QOwnNotes`.
