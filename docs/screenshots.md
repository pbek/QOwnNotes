# Desktop screenshots

Capture QOwnNotes using your current KDE Plasma desktop theme, fonts, icons,
and window decorations:

```bash
just screenshots
```

Requires Python 3, Spectacle, kdotool, and a running Plasma 6 session. Build the
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
`search-current-note`, `other-workspace`, `russian`, `diff`, `scripting`, `note-folders`,
`portable`, `encrypted`, `encrypted-unlocked`, `export-print`, `todo`, `trash`,
`versioning`, `nextcloud-deck`.
By default, `minimal`, `full`, `preview-only`, `subfolders`, and `darkmode` are
captured. The layouts come from the application's built-in layout presets, with
a larger preview so the rendered Markdown is readable: 560 logical pixels wide
in the `full`, `subfolders`, and `darkmode` screenshots and 400 pixels high above
the editor in `full-vertical`. The note relations scene replaces the preview with
a 640-pixel graph and linked demo notes. `preview-only` gets a 400-pixel-wide
left column with 60 pixels of height for the note tag buttons; both grow with
your Plasma font size (for example 560×84 with a 14 pt font), since larger fonts
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

`just screenshots --overview` captures **all twenty-two scenes**
from the [overview carousel](https://www.qownnotes.org/getting-started/overview.html),
plus a new **Nextcloud Deck dialog** scene, using website filenames in a new output
directory. The Deck image can be reviewed before adding it to the carousel; capture
and publishing do not change the carousel component.

| Scene                                        | Preset                | Website filename                          |
| -------------------------------------------- | --------------------- | ----------------------------------------- |
| Colored tags, subfolders, editor and preview | `subfolders`          | `screenshot.png`                          |
| Minimal layout                               | `minimal`             | `screenshot-minimal.png`                  |
| Preview above the editor, colored tags       | `full-vertical`       | `screenshot-vertical.png`                 |
| Linked notes and relations graph             | `note-relations`      | `screenshot-note-relations.png`           |
| Narrow one-column layout                     | `1col`                | `screenshot-1col.png`                     |
| Dark mode, colored tags and subfolders       | `darkmode`            | `screenshot-darkmode.png`                 |
| Distraction-free editing                     | `distraction-free`    | `screenshot-distraction-free-mode.png`    |
| Current desktop icon theme                   | `full`                | `screenshot-freedesktop-theme.png`        |
| Second workspace, selected in the toolbar    | `other-workspace`     | `screenshot-other-workspace.png`          |
| Script settings of a demo script             | `scripting`           | `screenshot-qml.png`                      |
| Russian user interface, notes and tags       | `russian`             | `screenshot-russian.png`                  |
| Search across all notes                      | `search-all-notes`    | `screenshot-search-in-all-notes.png`      |
| Search in the current note                   | `search-current-note` | `screenshot-search-in-current-note.png`   |
| Several note folders in the settings         | `note-folders`        | `screenshot-settings-note-folders.png`    |
| Diff dialog after an external note change    | `diff`                | `screenshot-diff.png`                     |
| Temporary portable installation              | `portable`            | `screenshot-portable-mode.png`            |
| Encrypted note, locked                       | `encrypted`           | `screenshot-encrypted-note.png`           |
| Editing an encrypted note                    | `encrypted-unlocked`  | `screenshot-encrypted-note-decrypted.png` |
| PDF export preview                           | `export-print`        | `screenshot-export-print.png`             |
| CalDAV tasks                                 | `todo`                | `screenshot-todo.png`                     |
| Nextcloud trash                              | `trash`               | `screenshot-trash.png`                    |
| Nextcloud note versions                      | `versioning`          | `screenshot-versioning.png`               |
| Nextcloud Deck cards with tags and Markdown  | `nextcloud-deck`      | `screenshot-nextcloud-deck.png`           |

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
scripts, note folders or cloud connections; the app adds all other tables on startup.

The capture command does not modify website assets or remove any carousel entries.
`overview-coverage.json` lists every screenshot referenced by the carousel plus
the additional Deck scene and whether it is automated.

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

### Portable, encryption, PDF and cloud fixtures

```bash
just screenshots --presets portable encrypted encrypted-unlocked export-print
just screenshots --presets todo trash versioning nextcloud-deck
```

- `portable` copies a native Linux ELF executable into a temporary installation,
  seeds its adjacent `Data/PBE/` settings and `Data/Demo notes/`, and runs it with `--portable`. For a Nix
  wrapper, its native `.<binary-name>-wrapped` payload is copied instead, preserving
  the wrapper's Qt plugin, QML import and desktop resource paths so icons still render. AppImages
  and shell launchers are not supported for this scene. The real binary's adjacent
  settings directory is never used. The scene shows the app with a portable-mode
  demo note rather than reproducing the old Windows Explorer image.
- `encrypted` and `encrypted-unlocked` invoke the real encryption action, fill
  its password dialog with a disposable demo password, and show either ciphertext
  or the decrypted editor. No personal password or encrypted note is used.
- `export-print` renders the demo note in Qt's print-preview dialog using only the
  PDF paint engine and a temporary output path. It does not open a native printer
  selection dialog or send anything to a printer.
- `todo`, `trash`, `versioning`, and `nextcloud-deck` start a read-only Python HTTP
  fixture server bound to `127.0.0.1` on a dynamically assigned port. It serves
  CalDAV VTODO data and Nextcloud API responses with three tasks, trashed notes,
  versions or Deck cards. Deck cards include colored tags and Markdown descriptions.
  The dialogs load these responses through the application's normal services;
  the first item is selected to showcase its description or preview.
- Cloud fixtures use a dummy `demo` account with an empty password, so no system
  credentials are needed. In every marked screenshot profile the app keeps generated
  service tokens in temporary settings instead of writing to the desktop keychain.
  Cloud settings overrides preserve the demo calendar selections through legacy
  database migrations. Their HTTP proxy points at the same local server,
  which rejects external origins and HTTPS tunnels and never forwards requests.
  Server writes are unsupported. The server is stopped when the scene exits.

These scenes require a build containing `src/services/screenshotscenario.cpp`.
The helper checks that both the settings and notes belong to the marked temporary
profile before automating anything. Capture waits for the helper's readiness file
instead of photographing a loading overlay; an older binary fails with a rebuild
message even if its release version is identical.

## Isolation and testing

Each capture uses temporary settings, data, cache, copies of the demo notes and
scripts, and a generated SQLite database containing only demo tags and their note
assignments, plus, where a scene needs them, demo scripts, note folders and cloud connections.
Only desktop appearance settings are copied from your configuration. Personal
QOwnNotes settings, credentials, and notes are not copied or modified. Tracking
and automatic update dialogs are disabled in the demo profile, and the demo app
uses an unreachable local proxy (or the non-forwarding demo proxy for cloud scenes),
so it cannot reach external servers. Temporary profiles
are removed when the command exits.

If the app does not match Plasma's theme, its Qt version may differ from the
desktop's and prevent the Plasma platform theme plugin from loading. The
`nix-build-system` recipe uses the running system's nixpkgs for that case.

Profile generation can be tested without a desktop:

```bash
python3 -m unittest discover -s scripts -p 'test_screenshots.py'
```
