# Installa come Flatpak

C'è un Flatpak `org.qownnotes.QOwnNotes` manutenuto dalla comunità su [Flathub](https://flathub.org/apps/details/org.qownnotes.QOwnNotes).

Puoi installarlo digitando:

```bash
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak install flathub org.qownnotes.QOwnNotes
```

Successivamente dovresti essere in grado di eseguire QOwnNotes con il comando `flatpak run org.qownnotes.QOwnNotes`.

::: tip Info
Si prega di riferire le problematiche con il pacchetto di Flatpak su [QOwnNotes Flatpak Issues](https://github.com/flathub/org.qownnotes.QOwnNotes/issues).
:::
