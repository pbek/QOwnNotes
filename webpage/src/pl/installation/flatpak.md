# Zainstaluj jako Flatpak

Istnieje społeczność Flatpak `org.qownnotes.QOwnNotes` na [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

Możesz go zainstalować, wpisując:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Następnie powinieneś być w stanie uruchomić QOwnNotes za pomocą polecenia `flatpak run org.qownnotes.QOwnNotes`.
