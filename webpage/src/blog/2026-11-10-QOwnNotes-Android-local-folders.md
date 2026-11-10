---
title: QOwnNotes for Android now works with local note folders
description: Use QOwnNotes for Android without Nextcloud by opening a note folder on your device, for example one that Syncthing shares with QOwnNotes on your desktop.
image: /img/qownnotes-android-local-folder-setup.webp
date: 2026-11-10
order: 20261110000000
---

# QOwnNotes for Android now works with local note folders

<BlogDate v-bind:fm="$frontmatter" />

Starting with version **26.11.0**, [QOwnNotes for Android](/getting-started/qownnotes-android.md)
no longer needs a Nextcloud server. Point the app at a folder on your phone,
and its Markdown files become your notes. This was one of the most requested
features ([#47](https://github.com/qownnotes/qownnotes-android/issues/47)).

<div style="display: flex; flex-wrap: wrap; gap: 24px; align-items: center; margin: 24px 0;">
  <img src="/img/qownnotes-android-local-folder-setup.webp" alt="QOwnNotes for Android setup screen offering a Nextcloud account or a local folder" style="flex: 0 1 300px; min-width: 220px; max-width: 100%; margin: 0 auto; border-radius: 16px; box-shadow: 0 8px 24px rgba(0, 0, 0, 0.18);" />
  <div style="flex: 1 1 280px;">

**Choose your way on the first screen.** Add a Nextcloud account as before, or
tap **Use a local folder** and pick a folder in Android's folder picker.

The app only gets access to that one folder. It needs no storage permission
and no network connection.

  </div>
</div>

## Your desktop note folder, on your phone

The folder is read just like a QOwnNotes desktop note folder:

- Markdown and text files are notes, and subfolders are note subfolders
- Images and attachments in `media` and `attachments` are shown and added
- Tags from the folder's `notes.sqlite` work like on the desktop

Keep the folder in sync with your computer using
[Syncthing](https://syncthing.net/) or any other file synchronizer.

## Safe with changes from elsewhere

Your edits are saved instantly and written to the files without a network
connection. Changes made by other programs appear when you refresh. If a file
changed while you were editing it, QOwnNotes keeps both versions and lets you
choose, instead of overwriting anything.

Deleted notes go to the app's own trash, where you can restore them.

See the [local folder setup](/getting-started/qownnotes-android.md#local-folder-setup)
for all the details.

## Nextcloud stays first-class

Nothing changes for Nextcloud users. You can even use a Nextcloud account and
local folders side by side and switch between them from the account menu.

Get QOwnNotes for Android from
[F-Droid](https://f-droid.org/en/packages/org.qownnotes.mobile/) or the
[GitHub releases](https://github.com/qownnotes/qownnotes-android/releases/latest).
