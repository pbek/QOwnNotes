# Contrôle de version avec git

Vous pouvez configurer QOwnNotes pour automatiquement valider (« commit ») les changements de votre dossier de notes localement dans un dépôt git dans votre dossier de notes.

This can be done in the _Git versioning_ settings in the _General settings_.

![git-note-folder-settings](/img/git/settings.png)

You need to have git installed on your system and the git executable needs to be in your PATH or
you can set the path to the git executable in the _Git versioning_ settings.

Il faut aussi activer le support de git pour chaque dossier de notes pour lequel vous souhaitez utiliser git !

![git-note-folder-settings](/img/git/note-folder-settings.png)

::: tip
Don't forget to set your git username and email address in the terminal:

```bash
git config --global user.email "votre-email@domaine.com"
git config --global user.name "Votre nom"
```

:::
