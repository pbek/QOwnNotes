---
title: Please test Qt6 version of QOwnNotes from a new PPA for Ubuntu Linux
description: If you want to help and test the Qt6 version of QOwnNotes on Ubuntu Linux there is a new PPA on Launchpad for that.
image: /assets/img/qt6.png
date: 2023-02-03
order: 20230203000000
---

# Please test the Qt6 version of QOwnNotes from a new PPA for Ubuntu Linux

<BlogDate v-bind:fm="$frontmatter" />

If you want to help and test the **Qt6 version of QOwnNotes** on Ubuntu Linux there is a new PPA on Launchpad for that.

```bash
# Remove Qt5 PPA in case you had it installed
sudo add-apt-repository --remove ppa:pbek/qownnotes

# Add Qt6 PPA
sudo add-apt-repository ppa:pbek/qownnotes-qt6

# Install QOwnNotes
sudo apt-get update
sudo apt-get install qownnotes
```

Also see: [QOwnNotes Qt6 Preview](https://www.qownnotes.org/installation/ubuntu.html#qownnotes-qt6-preview) in the installation guide.

If you find any issues please report them on [QOwnNotes Issues on GitHub](https://github.com/pbek/QOwnNotes/issues)!

![qt6](./media/qt6.png)

The image is property of [Qt](https://www.qt.io).
