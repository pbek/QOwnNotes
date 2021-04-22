# Installa come Flatpak

C'è una comunità mantenuta Flatpak `org.qownnotes.QOwnNotes` su [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

Puoi installarlo digitando:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Successivamente dovresti essere in grado di eseguire QOwnNotes con il comando `flatpak run org.qownnotes.QOwnNotes`.
