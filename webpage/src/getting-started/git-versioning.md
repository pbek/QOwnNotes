# Git versioning

You can set up QOwnNotes to automatically commit changes in your note folder locally to git.

This can be done in the *Git versioning* settings in the *General settings*. 

![git-note-folder-settings](/img/git/settings.png)

You need to have git installed on your system and the git executable needs to be in your PATH or
you can set the path to the git executable in the *Git versioning* settings.

You also need to turn on git support for each note folder you want to use git versioning with!

![git-note-folder-settings](/img/git/note-folder-settings.png)

::: tip
Don't forget to set your git username and email address in the terminal:

```bash
git config --global user.email "your-email@domain.com" git config --global user.name "Your name"
```
:::
