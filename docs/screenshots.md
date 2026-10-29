# Desktop screenshots

Capture QOwnNotes using your current KDE Plasma desktop theme, fonts, icons,
and window decorations:

```bash
just screenshots
```

Requires Python 3, Spectacle, kdotool, and a running Plasma session. Build the
application first with `just nix-build`; the command refuses to capture a binary
whose version differs from `src/version.h` (override with `--any-version`). No virtual desktop or artificial theme
is used. Keep the desktop unlocked and avoid typing or switching focus during capture.
The demo windows briefly take focus; the previously active window is restored
afterwards.

When started from a terminal without Plasma's session variables (for example a
terminal multiplexer, IDE, or SSH session on the same machine), the command reads
the missing display variables such as `KDE_SESSION_VERSION` and `WAYLAND_DISPLAY`
from the systemd user session that Plasma exports them to. Variables you have
already set are not changed.

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
`subfolders`, `darkmode`, `note-relations`, `distraction-free`, `search-all-notes`,
`search-current-note`, `other-workspace`, `russian`, `diff`, `scripting`, `note-folders`.
By default, `minimal`, `full`, `preview-only`, `subfolders`, and `darkmode` are
captured. The layouts come from the application's built-in layout presets, with
a larger preview so the rendered Markdown is readable: 560 logical pixels wide
in the `full`, `subfolders`, and `darkmode` screenshots and 400 pixels high above
the editor in `full-vertical`. The note relations scene replaces the preview with
a 640-pixel graph and linked demo notes. `preview-only` gets a 400-pixel-wide
left column with 100 pixels of height for the note tag buttons; both grow with
your Plasma font size (for example 520×140 with a 14 pt font), since larger fonts
make the buttons larger. The app's first-run limit that caps the note tags panel
at 40 pixels is skipped in every scene. `1col` is captured as a narrow window
(at most 640 pixels wide). Edit `scripts/screenshot-notes/` to curate the other
scenes. Preview sizes and tag colors are defined at the top of
`scripts/screenshots.py`.

All demo profiles have five colored tags with real note assignments, including
the `Workspace` sub-tag shown below the expanded `Projects` tag. Light mode uses
soft pastel rows behind dark text; dark mode uses separate, muted deep tones
behind light text. The subfolder and dark mode scenes
organize the notes under `Personal`, `Projects/Small adventures`, and
`Resources/Reading`, with the folder tree expanded. Dark mode uses the app's
built-in dark palette, editor scheme, and light icons for readability; it does
not switch your desktop's color scheme. Window decorations remain those of your
current Plasma desktop.

## Refreshing the website overview

`just screenshots --overview` captures the **fifteen currently automated scenes**
from the [overview carousel](https://www.qownnotes.org/getting-started/overview.html),
using their existing website filenames in a new output directory:

| Scene                                        | Preset                | Website filename                        |
| -------------------------------------------- | --------------------- | --------------------------------------- |
| Colored tags, subfolders, editor and preview | `subfolders`          | `screenshot.png`                        |
| Minimal layout                               | `minimal`             | `screenshot-minimal.png`                |
| Preview above the editor, colored tags       | `full-vertical`       | `screenshot-vertical.png`               |
| Linked notes and relations graph             | `note-relations`      | `screenshot-note-relations.png`         |
| Narrow one-column layout                     | `1col`                | `screenshot-1col.png`                   |
| Dark mode, colored tags and subfolders       | `darkmode`            | `screenshot-darkmode.png`               |
| Distraction-free editing                     | `distraction-free`    | `screenshot-distraction-free-mode.png`  |
| Current desktop icon theme                   | `full`                | `screenshot-freedesktop-theme.png`      |
| Second workspace, selected in the toolbar    | `other-workspace`     | `screenshot-other-workspace.png`        |
| Script settings of a demo script             | `scripting`           | `screenshot-qml.png`                    |
| Russian user interface, notes and tags       | `russian`             | `screenshot-russian.png`                |
| Search across all notes                      | `search-all-notes`    | `screenshot-search-in-all-notes.png`    |
| Search in the current note                   | `search-current-note` | `screenshot-search-in-current-note.png` |
| Several note folders in the settings         | `note-folders`        | `screenshot-settings-note-folders.png`  |
| Diff dialog after an external note change    | `diff`                | `screenshot-diff.png`                   |

How the newer scenes are set up, all inside the temporary profile:

- `other-workspace` has two workspaces, `Reading` (the preview-only layout, active)
  and `Writing`, so the workspace selector in the toolbar shows a non-default
  workspace.
- `search-all-notes` and `search-current-note` install a small helper script that
  selects a word in the current note and triggers _Find note_ (`place`) or _Find
  in note_ (`idea`), which search for the selected text.
- `scripting` installs a copy of `scripts/screenshot-scripts/daily-journal.qml` and
  opens the script settings. `note-folders` adds the `Work` and `Journal` note
  folders and opens the note folder settings. Both capture the settings dialog at
  its own size; KWin draws no shadow around it, so its margin is transparent.
- `russian` uses the Russian interface translation with the notes from
  `scripts/screenshot-notes-ru/`, Russian tag names, and spell checking turned off.
- `diff` changes the copy of the selected note on disk while the app is active,
  with notifications for all external changes enabled, and captures the dialog.

The app's disk database (`QOwnNotes.sqlite`) is only created for scenes with demo
scripts or note folders; the app adds all other tables on startup.

The capture command does not modify website assets or remove any carousel entries.
`overview-coverage.json` lists every screenshot referenced by the carousel and
whether it is automated or still needs a dedicated fixture.

### Replacing the carousel screenshots

1. Capture: `just screenshots --overview`
2. Review the images in the new `build-screenshots/<timestamp>/` directory and
   delete any you don't want to publish. Deleted scenes keep their current
   website image.
3. Publish:

   ```bash
   just screenshots-publish                                    # latest overview capture
   just screenshots-publish build-screenshots/<timestamp>      # a specific capture
   ```

Publishing copies only the known overview filenames from the table above into
both `webpage/src/.vuepress/public/screenshots/` (website carousel) and
`screenshots/` (README and AppStream metadata), which are kept identical. All
images are checked to be PNG files before anything is replaced, and unchanged
images are skipped. Review the result with `git diff --stat` and commit it.

The remaining seven carousel scenes are **not yet automated**:

- Portable mode: the current image is a Windows file listing; it needs a temporary
  portable installation, not the real binary's adjacent settings directory.
- Encryption (locked and unlocked): needs an encrypted demo note and password
  dialog automation.
- PDF export/print: needs print-preview dialog automation without invoking a real
  printer or listing the user's printers.
- CalDAV tasks, Nextcloud trash, and Nextcloud versioning: need disposable demo
  servers and seeded remote data. Never use personal server credentials for these.

## Isolation and testing

Each capture uses temporary settings, data, cache, copies of the demo notes and
scripts, and a generated SQLite database containing only demo tags and their note
assignments, plus, where a scene needs them, demo scripts and note folders.
Only desktop appearance settings are copied from your configuration. Personal
QOwnNotes settings, credentials, and notes are not copied or modified. Tracking
and automatic update dialogs are disabled in the demo profile, and the demo app
uses an unreachable local proxy, so it stays offline and never shows an update notice. Temporary profiles
are removed when the command exits.

If the app does not match Plasma's theme, its Qt version may differ from the
desktop's and prevent the Plasma platform theme plugin from loading. The
`nix-build-system` recipe uses the running system's nixpkgs for that case.

Profile generation can be tested without a desktop:

```bash
python3 -m unittest discover -s scripts -p 'test_screenshots.py'
```
