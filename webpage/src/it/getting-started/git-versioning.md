# Controllo versioni con Git

Puoi impostare automaticamente QOwnNotes per fare il commit delle modifiche dalla tua cartella locale delle note alla cartella delle note nella tua repository di git.

Questo può essere fatto nelle impostazioni di _Controllo versioni con Git_ sotto _Impostazioni generali_.

![impostazioni-cartella-note-git](/img/git/settings.png)

Devo avere git installato nel tuo sistema e avere l'eseguibile git impostato nella tua variabile di sistema 'PATH' o puoi impostare quest'ultima nelle impostazioni di _Controllo versioni con Git_.

Devi anche attivare il supporto a git per ogni cartella delle note con cui vuoi venga effettuato il controllo versioni con Git!

![impostazioni-cartella-note-git](/img/git/note-folder-settings.png)

::: tip
Don't forget to set your git username and email address in the terminal:

```bash
git config --global user.email "la-tua-email@dominio.com"
git config --global user.name "Il tuo nome"
```

:::
