# Instalar como Flatpak

Hay un Flatpak `org.qownnotes.QOwnNotes` mantenido por la comunidad en [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

Puede instalarlo escribiendo:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Después, debería poder ejecutar QOwnNotes con el comando `flatpak run org.qownnotes.QOwnNotes`.
