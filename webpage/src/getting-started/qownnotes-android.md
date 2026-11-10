# QOwnNotes Android

<div style="display: flex; gap: 16px; margin-bottom: 16px;">
  <img src="/img/qownnotes-android-note-list.webp" alt="QOwnNotes Android note list" style="flex: 1; min-width: 0;" />
  <img src="/img/qownnotes-android-note-view.webp" alt="QOwnNotes Android note view" style="flex: 1; min-width: 0;" />
  <img src="/img/qownnotes-android-note-edit.webp" alt="QOwnNotes Android note edit" style="flex: 1; min-width: 0;" /></div>

[QOwnNotes for Android](https://github.com/qownnotes/qownnotes-android) is an
**offline-capable Markdown notes app** for Android that synchronizes with your
Nextcloud server through the **Nextcloud Notes API** and the
**QOwnNotesAPI** app.

It talks directly to **Nextcloud Notes** and **QOwnNotesAPI** on your Nextcloud
server, giving you access to note synchronization, server-side note versions, and
the trash bin from your phone.

You can also use it **without Nextcloud**: open a [local note folder](#local-folder-setup)
on your phone, for example one that Syncthing shares with QOwnNotes on your desktop.

## Features

- **Offline-first**: Room database as the local source of truth; edits are
  cached immediately and synchronized when the server is reachable
- **Nextcloud Notes API 1.2+** with incremental pulls, ETags, and
  conflict-safe updates
- **Single Sign-On** import from the Nextcloud Files Android app
- **Local note folders** without Nextcloud, laid out like a QOwnNotes desktop
  note folder, with conflict detection and a restorable trash
- **Note version history** and **remote trash browsing** via the optional
  QOwnNotesAPI Nextcloud app
- **Favorites** with offline toggling and favorites-first ordering
- **Markdown rendering** for CommonMark, GitHub Flavored Markdown, checkbox
  lists with three states (open, done, partial), wiki links, tables, fenced
  code blocks, and remote images
- **Markdown editor** with formatting actions, undo/redo, cursor preservation,
  and local draft persistence
- **Share-to-create** notes from text shared by other Android apps
- **Light and dark themes** on Android 9+

## Installation

[![Get it on F-Droid](https://f-droid.org/badge/get-it-on.png)](https://f-droid.org/en/packages/org.qownnotes.mobile/) [![Get it on Obtainium](https://raw.githubusercontent.com/ImranR98/Obtainium/main/assets/graphics/badge_obtainium.png)](https://apps.obtainium.imranr.dev/redirect?r=obtainium://app/%7B%22id%22%3A%22org.qownnotes.mobile%22%2C%22url%22%3A%22https%3A%2F%2Fgithub.com%2Fqownnotes%2Fqownnotes-android%22%2C%22author%22%3A%22qownnotes%22%2C%22name%22%3A%22QOwnNotes%22%7D)

Install QOwnNotes for Android from
[F-Droid](https://f-droid.org/en/packages/org.qownnotes.mobile/), or visit the
[latest release page](https://github.com/qownnotes/qownnotes-android/releases/latest)
and download the APK.

Development builds are published alongside stable releases and can be installed
side-by-side (different application ID).

## Nextcloud account setup

1. Install the **Nextcloud Files** Android app and sign in to your server.
2. Open QOwnNotes for Android and choose **Add Nextcloud account**.
3. The app requires the **Nextcloud Notes** server app with Notes API 1.2 or
   newer.
4. If you also install the **QOwnNotesAPI** Nextcloud app, you gain access to
   note version history and the server-side trash bin.

::: tip
Credentials stay in Nextcloud's SSO integration and are not copied into the
QOwnNotes database.
:::

## Local folder setup

<div style="display: flex; flex-wrap: wrap; gap: 24px; align-items: center; margin: 16px 0;">
  <img src="/img/qownnotes-android-local-folder-setup.webp" alt="QOwnNotes for Android setup screen offering a Nextcloud account or a local folder" style="flex: 0 1 260px; min-width: 200px; max-width: 100%; margin: 0 auto; border-radius: 16px; box-shadow: 0 8px 24px rgba(0, 0, 0, 0.18);" />
  <div style="flex: 1 1 280px;">

1. Choose **Use a local folder** on the first screen, or **Add local folder** in
   the account menu.
2. Select a folder in Android's folder picker. QOwnNotes only gets access to
   that folder and needs no storage permission.
3. Keep the folder in sync with your computer using
   [Syncthing](https://syncthing.net/) or any other file synchronizer, and
   point QOwnNotes on the desktop at the same folder.

  </div>
</div>

The folder is used like a QOwnNotes desktop note folder:

- `.md` and `.txt` files are notes, subfolders are note subfolders, and new
  notes are created as `.md` files
- Images and attachments are read from and added to `media` and `attachments`
- Tags work when the folder contains the `notes.sqlite` of QOwnNotes desktop

Edits are written to the files without a network connection. Changes made by
other programs appear when you refresh or reopen the app. If a file changed
while a local edit was pending, the note becomes a conflict instead of being
overwritten.

Deleted notes move to the app's own trash, where you can restore them.
Note versions, sharing, and Nextcloud Deck need a Nextcloud server and are not
available for local folders.

::: tip
Removing a local folder from the app never deletes the note files in the folder.
:::

## Key differences from the Nextcloud Notes mobile app

| Feature                                     | QOwnNotes Android | Nextcloud Notes mobile |
| ------------------------------------------- | ----------------- | ---------------------- |
| Offline editing with conflict detection     | Yes               | Limited                |
| Note version history via QOwnNotesAPI       | Yes               | No                     |
| Remote trash browsing and restore           | Yes               | No                     |
| Favorites with offline toggle               | Yes               | No                     |
| Local note folders without Nextcloud        | Yes               | No                     |
| Three-state checkbox lists                  | Yes               | No                     |
| Wiki-link and legacy `note://` link support | Yes               | No                     |

## More information

- [GitHub repository](https://github.com/qownnotes/qownnotes-android)
- [Changelog](https://github.com/qownnotes/qownnotes-android/blob/main/CHANGELOG.md)
