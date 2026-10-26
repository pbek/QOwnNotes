# Desktop screenshots

Capture QOwnNotes using your current KDE Plasma desktop theme, fonts, icons,
and window decorations:

```bash
just screenshots
```

Requires Python 3, Spectacle, kdotool, and a running Plasma session. Build the
application first with `just nix-build`. No virtual desktop or artificial theme
is used. Keep the desktop unlocked and avoid typing or switching focus during capture.
The demo windows briefly take focus; the previously active window is restored
afterwards.

Images are saved under `build-screenshots/<timestamp>/`, with window decorations
and shadows but without the mouse pointer. Existing images are not overwritten.
The requested window size is in logical desktop pixels; Plasma can constrain it
to the available screen area, and display scaling affects PNG dimensions.

```bash
just screenshots --presets minimal --width 1440 --height 960
just screenshots --presets full preview-only --output /tmp/opencode/demo-shots
just screenshots --binary /path/to/QOwnNotes --settle 8
just screenshots --presets subfolders darkmode
just screenshots --overview
```

Available presets: `minimal`, `full`, `preview-only`, `full-vertical`, `1col`,
`subfolders`, `darkmode`, `note-relations`, `distraction-free`.
By default, `minimal`, `full`, `preview-only`, `subfolders`, and `darkmode` are
captured. The layouts come from the application's built-in layout presets, with
a wider preview pane in the `full`, `subfolders`, and `darkmode` screenshots.
The note relations scene replaces the preview with a wider graph and linked demo
notes. Edit `scripts/screenshot-notes/` to curate the other scenes.

All demo profiles have four colored tags with real note assignments, including
separate colors for light and dark mode. The subfolder and dark mode scenes
organize the notes under `Personal`, `Projects/Small adventures`, and
`Resources/Reading`, with the folder tree expanded. Dark mode uses the app's
built-in dark palette, editor scheme, and light icons for readability; it does
not switch your desktop's color scheme. Window decorations remain those of your
current Plasma desktop.

## Refreshing the website overview

`just screenshots --overview` captures the **eight currently automated scenes**
from the [overview carousel](https://www.qownnotes.org/getting-started/overview.html),
using their existing website filenames in a new output directory:

| Scene                                        | Website filename                       |
| -------------------------------------------- | -------------------------------------- |
| Colored tags, subfolders, editor and preview | `screenshot.png`                       |
| Minimal layout                               | `screenshot-minimal.png`               |
| Vertical layout                              | `screenshot-vertical.png`              |
| Linked notes and relations graph             | `screenshot-note-relations.png`        |
| One-column layout                            | `screenshot-1col.png`                  |
| Dark mode, colored tags and subfolders       | `screenshot-darkmode.png`              |
| Distraction-free editing                     | `screenshot-distraction-free-mode.png` |
| Current desktop icon theme                   | `screenshot-freedesktop-theme.png`     |

Review the images before copying them into `webpage/src/.vuepress/public/screenshots/`.
The command does not modify website assets or remove any carousel entries.
`overview-coverage.json` lists every screenshot referenced by the carousel and
whether it is automated or still needs a dedicated fixture.

The remaining fourteen carousel scenes are **not yet automated**:

- Portable mode: needs a temporary portable installation, not the real binary's
  adjacent settings directory.
- Encryption (locked and unlocked): needs an encrypted demo note and password
  dialog automation.
- External-change diff: needs controlled edits and conflict dialog automation.
- PDF export/print: needs print-preview dialog automation without invoking a real
  printer.
- Other workspace: needs multiple named demo layouts and a visible layout switch.
- Scripting: needs an installed demo script and its settings UI.
- Russian: needs a Russian-language profile and matching demo notes.
- Search across notes and search in the current note: need populated search UI.
- Note-folder settings: needs multiple demo note folders and the settings page.
- CalDAV tasks, Nextcloud trash, and Nextcloud versioning: need disposable demo
  servers and seeded remote data. Never use personal server credentials for these.

## Isolation and testing

Each capture uses temporary settings, data, cache, copies of the demo notes, and a
generated SQLite database containing only demo tags and their note assignments.
Only desktop appearance settings are copied from your configuration. Personal
QOwnNotes settings, credentials, and notes are not copied or modified. Tracking
and automatic update dialogs are disabled in the demo profile. Temporary profiles
are removed when the command exits.

If the app does not match Plasma's theme, its Qt version may differ from the
desktop's and prevent the Plasma platform theme plugin from loading. The
`nix-build-system` recipe uses the running system's nixpkgs for that case.

Profile generation can be tested without a desktop:

```bash
python3 -m unittest discover -s scripts -p 'test_screenshots.py'
```
