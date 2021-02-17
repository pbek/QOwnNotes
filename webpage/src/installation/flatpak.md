# Install as Flatpak

There is a community maintained Flatpak `org.qownnotes.QOwnNotes` on [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

You can install it by typing:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Afterwards you should be able to run QOwnNotes with the command `flatpak run org.qownnotes.QOwnNotes`.
