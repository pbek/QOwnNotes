# QOwnNotes Android

<div style="display: flex; gap: 16px; margin-bottom: 16px;">
  <img src="/img/qownnotes-android-note-list.webp" alt="QOwnNotes Android note list" style="flex: 1; min-width: 0;" />
  <img src="/img/qownnotes-android-note-view.webp" alt="QOwnNotes Android note view" style="flex: 1; min-width: 0;" />
  <img src="/img/qownnotes-android-note-edit.webp" alt="QOwnNotes Android note edit" style="flex: 1; min-width: 0;" /></div>

[QOwnNotes pour Android](https://github.com/qownnotes/qownnotes-android) est une **application pouvant fonctionner hors-ligne de notes Markdown** pour Android qui se synchronise avec votre serveur Nextcloud à travers l’**API Nextcloud Notes** et l’application **QOwnNotesAPI**.

Elle interagit directement avec **Nextcloud Notes** et **QOwnNotesAPI** sur votre serveur Nextcloud,
vous donnant accès à la synchronisation des notes, à l’historique distant de versions de notes et à la corbeille distante, le tout depuis votre téléphone.

## Fonctionnalités

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

## Configuration de compte Nextcloud

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

| Fonctionnalité                              | QOwnNotes Android | Nextcloud Notes mobile |
| ------------------------------------------- | ----------------- | ---------------------- |
| Offline editing with conflict detection     | Oui               | Limité                 |
| Note version history via QOwnNotesAPI       | Oui               | Non                    |
| Remote trash browsing and restore           | Oui               | Non                    |
| Favorites with offline toggle               | Oui               | Non                    |
| Three-state checkbox lists                  | Oui               | Non                    |
| Wiki-link and legacy `note://` link support | Oui               | Non                    |

## Plus d’informations

- [Dépôt GitHub](https://github.com/qownnotes/qownnotes-android)
- [Journal de modifications](https://github.com/qownnotes/qownnotes-android/blob/main/CHANGELOG.md)
