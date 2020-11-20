# Als Flatpak installieren

Auf [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes) befindet sich ein Flatpak `org.qownnotes.QOwnNotes` von Drittanbietern.

Sie können es installieren, indem Sie Folgendes eingeben:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Afterwards you should be able to run QOwnNotes with the command `flatpak run org.qownnotes.QOwnNotes`.
