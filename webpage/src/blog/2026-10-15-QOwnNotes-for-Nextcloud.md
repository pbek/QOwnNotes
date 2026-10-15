---
title: QOwnNotes for Nextcloud - Your notes in the Nextcloud web interface
description: QOwnNotes for Nextcloud 26.10.1 is now available in the Nextcloud App Store. View and edit your QOwnNotes notes on the web, with note subfolders and tags, and sync them with QOwnNotes Desktop and Android.
image: /img/qownnotes-nextcloud-editor.webp
date: 2026-10-15
order: 20261015000000
---

# QOwnNotes for Nextcloud - Your notes in the Nextcloud web interface

<BlogDate v-bind:fm="$frontmatter" />

We are happy to announce the first release of **QOwnNotes for Nextcloud**,
version **26.10.1**, in the
[Nextcloud App Store](https://apps.nextcloud.com/apps/qownnotes)!

QOwnNotes for Nextcloud lets you view and edit the notes of your QOwnNotes note
folder right in the Nextcloud web interface, **including note subfolders and
tags**. At the same time, it provides the server APIs for
[QOwnNotes Desktop](https://www.qownnotes.org) and
[QOwnNotes for Android](/blog/2026-09-05-QOwnNotes-Android.md), so a single
Nextcloud app now covers everything QOwnNotes needs on your server.

![QOwnNotes for Nextcloud with Markdown editor, note subfolders and tags](/img/qownnotes-nextcloud-editor.webp)

## What is QOwnNotes for Nextcloud?

Your notes stay what they have always been: **plain Markdown files** in your
Nextcloud, organized in note subfolders just like in QOwnNotes Desktop. The new
app gives you a web interface for these files and understands them the same way
the desktop app does.

Tags are read from and written to the `notes.sqlite` database of your note
folder, so your **tags are the same in QOwnNotes Desktop, on Android and on the
web**.

## Key Features

### Web Interface for Your Note Folder

- Note list with search, favorites and multi-selection (Ctrl/Shift+click)
- Note subfolder tree with folder operations and drag and drop
- Tag tree with hierarchy, colors for light and dark themes and filters
  (all/any tags, child tags, untagged notes)
- Autosave with conflict handling, previous versions with differences and
  restore, and deleted notes that can be restored together with their tags

### Markdown Editor with Live Preview

- Highlighting of headings, inline formatting, links, task checkboxes and
  Markdown markers
- Syntax highlighting of fenced code blocks in the editor and the preview
- Search and replace with **Ctrl+F**, including case, whole words and regular
  expressions
- Toggle task checkboxes with **Ctrl+Space**
- Follow links to other notes with **Ctrl+click**, like in QOwnNotes Desktop,
  including jumping to headings (`[TODO](TODO.md#todo)` or `<Journal.md>`)
- Optional preview beside the editor that updates while you type and scrolls
  together with the editor

![Markdown preview with checkable tasks](/img/qownnotes-nextcloud-preview.webp)

### Nextcloud Integrations

- **Recent notes** dashboard widget
- Unified search for notes, also by tags (`#tag`)
- Link previews and smart picker for notes
- **Open in QOwnNotes** in the Files app

### APIs for QOwnNotes Desktop and Android

- A [Notes API 1.4](https://github.com/qownnotes/qownnotes-nextcloud/blob/main/docs/api/notes-api.md)
  compatible note API for QOwnNotes Android
- The [QOwnNotes API](https://github.com/qownnotes/qownnotes-nextcloud/blob/main/docs/api/qownnotes-api.md)
  for note versions and the trash, plus new endpoints for note subfolders and
  tags

The app replaces the **QOwnNotesAPI** app and, for QOwnNotes Android, the
**Nextcloud Notes** app. It doesn't get in the way of the Nextcloud Notes app
and its clients, so all three apps can be installed at the same time.

### API-only Mode

If you only want to provide the sync APIs, administrators can disable the web
interface in the admin settings or with:

```bash
occ config:app:set qownnotes ui_enabled --value=no
```

Users who only sync with QOwnNotes Desktop or Android can also hide QOwnNotes
from their app navigation in the settings of the web interface.

## Getting Started

### Requirements

- Nextcloud 32 to 35
- PHP 8.2 to 8.5
- The PHP extension `pdo_sqlite` for tags

### Installation

Install **QOwnNotes** from the Nextcloud App Store (category "Office") in the
apps management of your Nextcloud, or with:

```bash
occ app:install qownnotes
```

### Switching from QOwnNotesAPI or Nextcloud Notes

Your notes don't need to be migrated, because all apps work on the same files
in your Nextcloud. Keep the **QOwnNotesAPI** and **Notes** apps installed until
all your QOwnNotes Desktop and Android clients support the new app. The clients
detect the new app automatically by the capabilities of your server.

Please read the
[migration guide](https://github.com/qownnotes/qownnotes-nextcloud/blob/main/docs/migration.md)
for all the details.

## Links

- [QOwnNotes Nextcloud App documentation](/getting-started/qownnotes-nextcloud.md)
- [QOwnNotes in the Nextcloud App Store](https://apps.nextcloud.com/apps/qownnotes)
- [GitHub Repository](https://github.com/qownnotes/qownnotes-nextcloud)
- [Changelog](https://github.com/qownnotes/qownnotes-nextcloud/blob/main/CHANGELOG.md)
- [Migration guide](https://github.com/qownnotes/qownnotes-nextcloud/blob/main/docs/migration.md)
- [API documentation](https://github.com/qownnotes/qownnotes-nextcloud/blob/main/docs/api/README.md)
- [Issues](https://github.com/qownnotes/qownnotes-nextcloud/issues)

---

_QOwnNotes for Nextcloud is open source and available under the AGPL-3.0
license. Contributions, bug reports and translations are welcome on
[GitHub](https://github.com/qownnotes/qownnotes-nextcloud)._
