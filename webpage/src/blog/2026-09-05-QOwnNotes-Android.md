---
title: QOwnNotes for Android - Markdown notes on your phone
description: QOwnNotes for Android is an offline-capable Markdown notes app that synchronizes with your Nextcloud server through the Nextcloud Notes API and the QOwnNotesAPI app.
image: /img/qownnotes-android-note-list.webp
date: 2026-09-05
order: 20260905000000
---

# QOwnNotes for Android - Markdown notes on your phone

<BlogDate v-bind:fm="$frontmatter" />

We are excited to announce **QOwnNotes for Android** — a native, offline-capable
Markdown notes app that brings your QOwnNotes experience directly to your Android
device.

After years of community requests, we finally have an official Android companion
that syncs seamlessly with your Nextcloud server through the **Nextcloud Notes
API** and the **QOwnNotesAPI** app.

<div style="display: flex; gap: 16px; margin-bottom: 16px;">
  <img src="/img/qownnotes-android-note-list.webp" alt="QOwnNotes Android note list" style="flex: 1; min-width: 0;" />
  <img src="/img/qownnotes-android-note-view.webp" alt="QOwnNotes Android note view" style="flex: 1; min-width: 0;" />
  <img src="/img/qownnotes-android-note-edit.webp" alt="QOwnNotes Android note edit" style="flex: 1; min-width: 0;" />
</div>

## What is QOwnNotes for Android?

QOwnNotes for Android is an **offline-first Markdown notes application** built
specifically for Android. It talks directly to **Nextcloud Notes** and
**QOwnNotesAPI** on your server, giving you full access to note synchronization,
server-side note versions, and the trash bin — all from your phone.

## Key Features

### Offline-First Architecture

Your notes live in a local Room database, so you can read, write, and edit
without an internet connection. Changes are cached immediately and synchronized
automatically when the server becomes reachable.

### Nextcloud Notes API 1.2+

Full support for the latest Notes API with incremental pulls, ETags, and
conflict-safe updates. Your notes stay consistent across all your devices.

### Single Sign-On (SSO)

Import your account directly from the **Nextcloud Files** Android app — no need
to re-enter credentials. Your login stays secure within Nextcloud's SSO
integration and is never copied into the QOwnNotes database.

### Note Version History & Trash

Install the optional **QOwnNotesAPI** Nextcloud app on your server and gain
access to:

- **Note version history** — browse and restore previous versions of your notes
- **Remote trash bin** — recover accidentally deleted notes directly from your phone

### Favorites with Offline Toggle

Mark notes as favorites and toggle them offline. Favorites appear first in your
note list for quick access.

### Powerful Markdown Rendering

QOwnNotes for Android renders:

- CommonMark and GitHub Flavored Markdown
- Checkbox lists with three states (open, done, partial)
- Wiki links and legacy `note://` links
- Tables
- Fenced code blocks
- Remote images

### Full-Featured Markdown Editor

The built-in editor includes:

- Formatting actions for quick styling
- Undo/redo with cursor preservation
- Local draft persistence — never lose your work
- Tabbing support

### Share-to-Create

Share text from any Android app (browser, messenger, email) directly into
QOwnNotes to create a new note instantly.

### Light and Dark Themes

Seamless theme support on Android 9+ to match your system preferences.

## How QOwnNotes Android Compares

| Feature                                     | QOwnNotes Android | Nextcloud Notes mobile |
| ------------------------------------------- | ----------------- | ---------------------- |
| Offline editing with conflict detection     | Yes               | Limited                |
| Note version history via QOwnNotesAPI       | Yes               | No                     |
| Remote trash browsing and restore           | Yes               | No                     |
| Favorites with offline toggle               | Yes               | No                     |
| Three-state checkbox lists                  | Yes               | No                     |
| Wiki-link and legacy `note://` link support | Yes               | No                     |

## Getting Started

### Installation

Download the latest APK from the
[GitHub releases page](https://github.com/qownnotes/qownnotes-android/releases/latest).
Development builds are also available and can be installed side-by-side with the
stable version.

### Nextcloud Account Setup

1. Install the **Nextcloud Files** Android app and sign in to your server
2. Open QOwnNotes for Android and choose **Add Nextcloud account**
3. The app requires the **Nextcloud Notes** server app with Notes API 1.2 or newer
4. For version history and trash, install the **QOwnNotesAPI** Nextcloud app on your server

That's it — your notes sync automatically and you can start editing right away.

## Why QOwnNotes Android?

For years, QOwnNotes users on Android had to rely on workarounds — third-party
Markdown editors like Markor, manual file syncing, or the limited Nextcloud
Notes app. QOwnNotes for Android changes that.

You get a **dedicated, purpose-built** companion app that understands your notes
the same way the desktop app does. Offline editing, conflict detection, version
history, trash recovery — features that matter when your notes are important.

Your notes remain plain Markdown files. No proprietary formats. No lock-in. Just
your thoughts, synced through your own Nextcloud server.

## Links

- [GitHub Repository](https://github.com/qownnotes/qownnotes-android)
- [Download Latest Release](https://github.com/qownnotes/qownnotes-android/releases/latest)
- [Changelog](https://github.com/qownnotes/qownnotes-android/blob/main/CHANGELOG.md)
- [QOwnNotes Desktop App](https://www.qownnotes.org/)
- [QOwnNotesAPI Nextcloud App](https://apps.nextcloud.com/apps/qownnotesapi)

---

_QOwnNotes for Android is open source and available under the GPL-2.0 license.
Contributions, bug reports, and translations are welcome on
[GitHub](https://github.com/qownnotes/qownnotes-android)._
