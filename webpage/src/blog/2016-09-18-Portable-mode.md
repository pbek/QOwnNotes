---
title: Portable mode
image: /img/blog/Portable-mode_reference.png
date: 2016-09-18T10:22:41+00:00
order: 20160918102241
---

# Portable mode

<v-subheader class="blog">Date: 2016-09-18</v-subheader>

There now is a **portable mode** for **QOwnNotes**, for example to carry around QOwnNotes on an **USB stick**.

In portable mode

- the internal sqlite **database** and the **settings** will be stored inside a **_Data_ folder** at the binary's location
- the settings will be stored in an **ini file**
- the **note folders** will be automatically stored relative to the **_Data_ folder** so that the correct note folders will be loaded regardless where your QOwnNotes installation is currently located

It will be activated if you run QOwnNotes with the **parameter _--portable_**.

On **Windows** there is a **_QOwnNotesPortable.bat_** in your release path to start QOwnNotes in **portable mode**.

Note: QOwnNotesPortable.exe was dropped because of anti virus false positives.

![Portable mode for USB sticks](/img/blog/Portable-mode_reference.png "Portable mode for USB sticks") **Portable mode** for USB sticks
