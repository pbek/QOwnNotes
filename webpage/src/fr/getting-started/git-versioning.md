# Contrôle de version avec git

Vous pouvez configurer QOwnNotes pour automatiquement valider (« commit ») les changements de votre dossier de notes localement dans un dépôt git dans votre dossier de notes.

La configuration se fait dans _Contrôle de version git_, dans _Général_, dans les paramètres.

![git-note-folder-settings](/img/git/settings.png)

Il faut que git soit installé sur votre système, et le chemin vers l’exécutable git doit soit être dans la variable d’environnement PATH, soit peut être spécifié dans les paramètres, dans la section _Contrôle de version git_.

Il faut aussi activer le support de git pour chaque dossier de notes pour lequel vous souhaitez utiliser git !

![git-note-folder-settings](/img/git/note-folder-settings.png)

::: tip
Don't forget to set your git username and email address in the terminal:

```bash
git config --global user.email "votre-email@domaine.com"
git config --global user.name "Votre nom"
```

:::
