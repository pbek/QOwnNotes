---
title: QOwnNotes TUI - Your notes in the terminal
description: QOwnNotes TUI is a new keyboard-first terminal browser and editor for your QOwnNotes Markdown note folders.
image: /img/qownnotes-tui.webp
date: 2026-08-15
order: 20260815000000
---

# QOwnNotes TUI - Your notes in the terminal

<BlogDate v-bind:fm="$frontmatter" />

There is a new companion project for everyone who loves to work in the terminal:
[**QOwnNotes TUI**](https://github.com/qownnotes/qownnotes-tui), a keyboard-first
**terminal browser and editor** for local, QOwnNotes-compatible Markdown note folders.

![QOwnNotes TUI](/img/qownnotes-tui.webp "Browse and edit your notes in the terminal")

QOwnNotes TUI directly **uses the note folders configured in QOwnNotes** (opened
read-only) and follows its note sorting settings, so you can start working with
your **existing notes** right away.

Some highlights:

- **Create, edit, search and delete** notes from the terminal, with syntax highlighting
- **Automatic saving** and detection of conflicting external changes
- Note **filenames automatically follow the first meaningful content line**
- **Configurable themes**, mouse controls and shell completions for Bash, Fish and Zsh
- Packaged with **Nix**, including a **Home Manager module**

With Nix you can try it out right away:

```bash
nix run github:qownnotes/qownnotes-tui
```

For more information visit the [QOwnNotes TUI](../getting-started/qownnotes-tui.md) page
or the [qownnotes-tui repository on GitHub](https://github.com/qownnotes/qownnotes-tui).
