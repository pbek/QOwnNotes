# Telepítés Flatpak néven

Van egy közösség által fenntartott Flatpak a `org.qownnotes.QOwnNotes`on és a [ Flathub ](https://flathub.org/apps/details/org.qownnotes.QOwnNotes)on.

Telepítéséhez írja be:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Ezután képesnek kell lennie a QOwnNotes futtatására a `flatpak run org.qownnotes.QOwnNotes` paranccsal.
