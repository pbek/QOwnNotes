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

## Features

- **Offline-first**: Room database as the local source of truth; edits are
  cached immediately and synchronized when the server is reachable
- **Nextcloud Notes API 1.2+** with incremental pulls, ETags, and
  conflict-safe updates
- **Single Sign-On** import from the Nextcloud Files Android app
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

Visit the [latest release page](https://github.com/qownnotes/qownnotes-android/releases/latest)
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

## Key differences from the Nextcloud Notes mobile app

| Feature                                     | QOwnNotes Android | Nextcloud Notes mobile |
| ------------------------------------------- | ----------------- | ---------------------- |
| Offline editing with conflict detection     | Yes               | Limited                |
| Note version history via QOwnNotesAPI       | Yes               | No                     |
| Remote trash browsing and restore           | Yes               | No                     |
| Favorites with offline toggle               | Yes               | No                     |
| Three-state checkbox lists                  | Yes               | No                     |
| Wiki-link and legacy `note://` link support | Yes               | No                     |

## More information

- [GitHub repository](https://github.com/qownnotes/qownnotes-android)
- [Changelog](https://github.com/qownnotes/qownnotes-android/blob/main/CHANGELOG.md)
