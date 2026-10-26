# Desktop screenshots

Capture QOwnNotes using your current KDE Plasma desktop theme, fonts, icons,
and window decorations:

```bash
just screenshots
```

Requires Python 3, Spectacle, kdotool, and a running Plasma session. Build the
application first with `just nix-build`. No virtual desktop or artificial theme
is used. Keep the desktop unlocked and avoid switching focus during capture.
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
```

Available presets: `minimal`, `full`, `preview-only`, `full-vertical`, `1col`.
By default, the first three are captured. The layouts come from the application's
built-in layout presets. Edit `scripts/screenshot-notes/` to curate the demo.

Each capture uses temporary settings, data, cache, and copies of the demo notes.
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
