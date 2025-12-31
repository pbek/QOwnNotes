---
title: Evernote import for huge files
description: You should now be able to import huge ENEX files from Evernote into QOwnNotes!
image: /assets/evernote.jpg
date: 2023-01-29
order: 20230129000000
---

# Evernote import for huge files

<BlogDate v-bind:fm="$frontmatter" />

You should now be able to import huge files from **Evernote** into QOwnNotes!

The Evernote importer was completely rewritten with stream support (`QXMLStreamReader`)
to now support reading **huge ENEX Evernote** files (for [#2711](https://github.com/pbek/QOwnNotes/issues/2711)).

It also works under Qt 6 now to be more prepared for the future.
A lot of effort is still going into **porting QOwnNotes** and the build and release process to **Qt 6**.

If you can please try to import large ENEX files and **report how it went** on [#2711](https://github.com/pbek/QOwnNotes/issues/2711).

![Evernote](./media/evernote.jpg)
