# Installer en tant que Flatpak

There is a community maintained Flatpak `org.qownnotes.QOwnNotes` on [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

Vous pouvez l'installer en tapant:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Ensuite, vous devriez pouvoir exécuter QOwnNotes avec la commande `flatpak run org.qownnotes.QOwnNotes`.
