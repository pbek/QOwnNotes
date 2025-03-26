# Controllo versioni con Git

Puoi impostare automaticamente QOwnNotes per fare il commit delle modifiche dalla tua cartella locale delle note alla cartella delle note nella tua repository di git.

This can be done in the _Git versioning_ settings in the _General settings_.

![impostazioni-cartella-note-git](/img/git/settings.png)

You need to have git installed on your system and the git executable needs to be in your PATH or
you can set the path to the git executable in the _Git versioning_ settings.

Devi anche attivare il supporto a git per ogni cartella delle note con cui vuoi venga effettuato il controllo versioni con Git!

![impostazioni-cartella-note-git](/img/git/note-folder-settings.png)

::: tip
Don't forget to set your git username and email address in the terminal:

```bash
git config --global user.email "la-tua-email@dominio.com"
git config --global user.name "Il tuo nome"
```

:::
