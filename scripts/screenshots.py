#!/usr/bin/env python3
"""Capture isolated QOwnNotes demo windows on the current KDE Plasma desktop."""

import argparse
from contextlib import closing
from datetime import datetime
import json
import os
from pathlib import Path
import re
import shutil
import sqlite3
import subprocess
import sys
import tempfile
import time


ROOT = Path(__file__).resolve().parent.parent
PRESETS = ("minimal", "full", "preview-only", "full-vertical", "1col")
SCENARIOS = {
    **{preset: preset for preset in PRESETS},
    "subfolders": "full",
    "darkmode": "full",
    "note-relations": "full",
    "distraction-free": "minimal",
}
# Website filenames are explicit so captures can be reviewed before publishing.
OVERVIEW = {
    "subfolders": "screenshot.png",
    "minimal": "screenshot-minimal.png",
    "full-vertical": "screenshot-vertical.png",
    "note-relations": "screenshot-note-relations.png",
    "1col": "screenshot-1col.png",
    "darkmode": "screenshot-darkmode.png",
    "distraction-free": "screenshot-distraction-free-mode.png",
    "full": "screenshot-freedesktop-theme.png",
}
STARTUP_ACTIONS = {
    "distraction-free": "actionToggle_distraction_free_mode",
}


def overview_coverage():
    carousel = ROOT / "webpage/src/.vuepress/components/ScreenshotCarousel.vue"
    filenames = re.findall(r'src="/screenshots/([^"/]+\.png)"', carousel.read_text())
    return {filename: ("automated" if filename in OVERVIEW.values() else
                       "needs dedicated fixture") for filename in filenames}


def seed_demo_tags(notes):
    """Create a private version-12 note database; the app migrates it on startup."""
    with closing(sqlite3.connect(notes / "notes.sqlite")) as db:
        db.executescript("""
            CREATE TABLE appData (name VARCHAR(255) PRIMARY KEY, value VARCHAR(255));
            INSERT INTO appData VALUES ('database_version', '12');
            CREATE TABLE tag (
                id INTEGER PRIMARY KEY, name VARCHAR(255) COLLATE NOCASE,
                priority INTEGER DEFAULT 0, created DATETIME DEFAULT current_timestamp,
                parent_id INTEGER DEFAULT 0, color VARCHAR(20), dark_color VARCHAR(20),
                updated DATETIME DEFAULT current_timestamp);
            CREATE UNIQUE INDEX idxUniqueTag ON tag (name, parent_id);
            CREATE INDEX idxTagParent ON tag (parent_id);
            CREATE TABLE noteTagLink (
                id INTEGER PRIMARY KEY, tag_id INTEGER,
                note_file_name VARCHAR(255) DEFAULT '',
                note_sub_folder_path TEXT DEFAULT '',
                created DATETIME DEFAULT current_timestamp);
            CREATE UNIQUE INDEX idxUniqueTagNoteLink
                ON noteTagLink (tag_id, note_file_name, note_sub_folder_path);
        """)
        db.executemany(
            "INSERT INTO tag (id, name, color, dark_color) VALUES (?, ?, ?, ?)",
            [(1, "Ideas", "#2980b9", "#24546c"),
             (2, "Projects", "#8e44ad", "#633c78"),
             (3, "Personal", "#218c59", "#245b43"),
             (4, "Reading", "#c56b16", "#775021")])
        assignments = {
            "A little room for ideas.md": (1, 2),
            "Project field notes.md": (2,),
            "Reading journal.md": (4,),
            "Weekend possibilities.md": (3,),
            "Connected ideas.md": (1, 2),
            "Projects.md": (2,),
            "Reading.md": (4,),
            "Weekends.md": (3,),
        }
        for note in notes.rglob("*.md"):
            for tag_id in assignments.get(note.name, (2,)):
                folder = note.parent.relative_to(notes).as_posix()
                db.execute(
                    "INSERT INTO noteTagLink (tag_id, note_file_name, "
                    "note_sub_folder_path) VALUES (?, ?, ?)",
                    (tag_id, note.stem, "" if folder == "." else folder))
        db.commit()


def run(*args):
    return subprocess.check_output(args, text=True, timeout=30).strip()


def layout_settings(preset):
    # Keep Qt's serialized @ByteArray intact rather than interpreting INI escapes.
    section = False
    values = {}
    for line in (ROOT / "src/configurations/layouts.ini").read_text().splitlines():
        if line.startswith("["):
            section = line == f"[LayoutPreset-{preset}]"
        elif section and "=" in line:
            key, value = line.split("=", 1)
            values[key] = value
    return values


def prepare_profile(base, preset):
    scenario = preset
    preset = SCENARIOS[scenario]
    config = base / "config"
    (config / "PBE").mkdir(parents=True)
    notes = base / "Demo notes"
    shutil.copytree(ROOT / "scripts/screenshot-notes", notes)
    if scenario == "note-relations":
        for old, new in (("A little room for ideas", "Connected ideas"),
                         ("Project field notes", "Projects"),
                         ("Reading journal", "Reading"),
                         ("Weekend possibilities", "Weekends")):
            (notes / f"{old}.md").rename(notes / f"{new}.md")
        (notes / "Connected ideas.md").write_text(
            "# Connected ideas\n\n"
            "Good ideas grow when you connect them.\n\n"
            "## Follow a thread\n\n"
            "- [Projects](Projects.md)\n"
            "- [Reading](Reading.md)\n"
            "- [Weekends](Weekends.md)\n\n"
            "## From inspiration to action\n\n"
            "Collect a thought, explore a connection, make something new.\n")
    if scenario in ("subfolders", "darkmode"):
        for filename, folder in (
                ("Project field notes.md", "Projects/Small adventures"),
                ("Reading journal.md", "Resources/Reading"),
                ("Weekend possibilities.md", "Personal")):
            target = notes / folder
            target.mkdir(parents=True, exist_ok=True)
            (notes / filename).rename(target / filename)
    seed_demo_tags(notes)
    # The most recently modified note is selected on a fresh profile.
    selected = "Connected ideas" if scenario == "note-relations" else "A little room for ideas"
    os.utime(notes / f"{selected}.md", None)

    # Copy only desktop appearance settings, never the user's application profile.
    desktop_config = Path(os.environ.get("XDG_CONFIG_HOME", Path.home() / ".config"))
    for name in ("kdeglobals", "breezerc", "oxygenrc", "kdeglobals.d",
                 "qt5ct", "qt6ct", "Kvantum"):
        source = desktop_config / name
        if source.is_dir():
            shutil.copytree(source, config / name)
        elif source.is_file():
            shutil.copy2(source, config / name)

    layout = layout_settings(preset)
    if preset == "full":
        # Give the right-hand preview dock 480 pixels instead of its minimum width.
        # Change only the screenshot profile, not the application's built-in preset.
        right_dock = r"\0\0\0\x1\0\0\0\0\0\0\0\0\xfc"
        wider_right_dock = (r"\0\0\0\x1\0\0\x2\x80\0\0\0\0\xfc"
                            if scenario == "note-relations" else
                            r"\0\0\0\x1\0\0\x1\xe0\0\0\0\0\xfc")
        if layout["windowState"].count(right_dock) != 1:
            raise RuntimeError("The full layout's preview dock state has changed.")
        layout["windowState"] = layout["windowState"].replace(
            right_dock, wider_right_dock)
    if scenario == "note-relations":
        # Reuse the generously sized preview dock for the graph, hiding the preview.
        preview = (r'\0\0\0*\0n\0o\0t\0\x65\0P\0r\0\x65\0v\0i\0\x65\0w'
                   r'\0\x44\0o\0\x63\0k\0W\0i\0\x64\0g\0\x65\0t')
        graph = (r'\0\0\0\x34\0n\0o\0t\0\x65\0G\0r\0\x61\0p\0h\0i\0\x63\0s'
                 r'\0V\0i\0\x65\0w\0\x44\0o\0\x63\0k\0W\0i\0\x64\0g\0\x65\0t')
        log = r'\0\0\0\x1a\0l\0o\0g\0\x44\0o\0\x63\0k\0W\0i\0\x64\0g\0\x65\0t'
        state = layout["windowState"]
        if state.count(preview) != 1 or state.count(log) != 1:
            raise RuntimeError("The full layout's dock names have changed.")
        layout["windowState"] = state.replace(preview, graph).replace(log, preview)
    settings = (
        "[General]\n"
        f"notesPath={notes}\n"
        "demoNotesCreated=true\n"
        "disableAutomaticUpdateDialog=true\n"
        "ShowSystemTray=false\n"
        "showStatusBarNotePath=false\n"
        "enableSocketServer=false\n"
        f"systemIconTheme={'false' if scenario == 'darkmode' else 'true'}\n"
        "currentLayout=screenshot\n"
        "layouts=screenshot\n"
        f"centralWidget={layout['centralWidget']}\n"
        f"noteEditIsCentralWidget={layout['noteEditIsCentralWidget']}\n"
    )
    if preset == "full":
        # Do not let first-run dock size limits override the screenshot layout.
        settings += "dockWasInitializedOnce=true\n"
    if scenario in ("subfolders", "darkmode"):
        settings += (
            "showNoteSubFolders=true\n"
            "noteSubfoldersPanelDisplayAsFullTree=true\n"
            "noteSubfoldersPanelShowNotesRecursively=true\n"
        )
    if scenario == "darkmode":
        settings += (
            "darkMode=true\n"
            "darkModeColors=true\n"
            "darkModeIconTheme=true\n"
            "internalIconTheme=true\n"
            "\n[Editor]\n"
            "CurrentSchemaKey=EditorColorSchema-cdbf28fc-1ddc-4d13-bb21-6a4043316a2f\n"
        )
    settings += (
        "\n[MessageBoxOverride]\n"
        "system-dark-mode=65536\n"
        "system-light-mode=65536\n"
        "linux-dark-mode=65536\n"
        "linux-light-mode=65536\n"
        "\n[MainWindow]\n"
        "noteSubFolderTreeWidgetExpandState-1=Projects, Resources\n"
        "\n[appMetrics]\n"
        "notificationShown=true\n"
        "disableTracking=true\n"
        "disableAppHeartbeat=true\n"
        "\n[layout-screenshot]\n"
        "name=Screenshot\n"
    )
    for key in ("centralWidget", "noteEditIsCentralWidget", "windowState",
                "noteSubFolderDockWidgetVisible"):
        settings += f"{key}={layout[key]}\n"

    session = "Demo"
    for app in ("QOwnNotes", "QOwnNotesDebug"):
        (config / "PBE" / f"{app}-{session}.conf").write_text(settings)
    env = dict(os.environ)
    for key, directory in (("XDG_CONFIG_HOME", config),
                           ("XDG_DATA_HOME", base / "data"),
                           ("XDG_CACHE_HOME", base / "cache")):
        directory.mkdir(exist_ok=True)
        env[key] = str(directory)
    # User-installed icons, fonts and Qt plugins still come from the real desktop.
    original_data = os.environ.get("XDG_DATA_HOME", str(Path.home() / ".local/share"))
    env["XDG_DATA_DIRS"] = original_data + ":" + os.environ.get(
        "XDG_DATA_DIRS", "/usr/local/share:/usr/share")
    return env, session


def capture(args, preset, output, base):
    env, session = prepare_profile(base, preset)
    command = [str(args.binary), "--session", session, "--allow-multiple-instances"]
    if preset in STARTUP_ACTIONS:
        command += ["--action", STARTUP_ACTIONS[preset]]
    with (base / "app.log").open("w+") as log:
        process = subprocess.Popen(
            command,
            env=env, stdout=log, stderr=log,
        )
        try:
            window = ""
            deadline = time.monotonic() + 45
            while time.monotonic() < deadline and process.poll() is None:
                # PID-only searches also match empty title patterns in kdotool 0.3.
                windows = run("kdotool", "search", "--all", "--pid", str(process.pid),
                              "--title", "QOwnNotes").splitlines()
                if windows:
                    window = windows[0]
                    break
                time.sleep(0.25)
            if not window:
                raise RuntimeError("The demo window did not appear.")
            time.sleep(args.settle)
            title = run("kdotool", "getwindowname", window)
            expected_note = ("Connected ideas" if preset == "note-relations" else
                             "A little room for ideas")
            if (expected_note not in title or
                    run("kdotool", "getwindowpid", window) != str(process.pid)):
                raise RuntimeError(f"Unexpected demo window: {title}")
            print(f"Capturing: {title}", flush=True)
            run("kdotool", "windowstate", "--remove", "MAXIMIZED", window)
            run("kdotool", "windowsize", window, str(args.width), str(args.height))
            run("kdotool", "windowactivate", window)
            time.sleep(1)
            if run("kdotool", "getactivewindow") != window:
                raise RuntimeError("Could not activate the demo window; capture cancelled.")
            # Capture to a temporary file and verify focus before publishing it.
            image = base / "capture.png"
            subprocess.run(
                ["spectacle", "--background", "--nonotify", "--activewindow",
                 "--output", str(image)], check=True, timeout=45,
            )
            deadline = time.monotonic() + 15
            while not image.is_file() and time.monotonic() < deadline:
                time.sleep(0.1)
            if run("kdotool", "getactivewindow") != window:
                raise RuntimeError("Focus changed during capture; image discarded for privacy.")
            if not image.is_file() or image.stat().st_size == 0:
                raise RuntimeError("Spectacle did not create an image.")
            shutil.copyfile(image, output)
            print(output)
        except Exception:
            log.flush()
            log.seek(0)
            print(log.read()[-6000:], file=sys.stderr)
            raise
        finally:
            if process.poll() is None:
                process.terminate()
                try:
                    process.wait(timeout=10)
                except subprocess.TimeoutExpired:
                    process.kill()
                    process.wait()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary", type=Path, default=ROOT / "result/bin/QOwnNotes")
    parser.add_argument("--output", type=Path, default=ROOT / "build-screenshots" /
                        datetime.now().strftime("%Y%m%d-%H%M%S-%f"))
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument("--presets", nargs="+", choices=SCENARIOS,
                           default=None)
    selection.add_argument("--overview", action="store_true",
                           help="Capture supported overview carousel scenes with website filenames")
    parser.add_argument("--width", type=int, default=1440)
    parser.add_argument("--height", type=int, default=960)
    parser.add_argument("--settle", type=float, default=4,
                        help="Seconds to wait for startup and note indexing (default: 4)")
    args = parser.parse_args()
    args.presets = (list(OVERVIEW) if args.overview else args.presets or
                    ["minimal", "full", "preview-only", "subfolders", "darkmode"])
    if args.width < 640 or args.height < 480 or args.settle < 0:
        parser.error("Use dimensions of at least 640x480 and a nonnegative settle time.")
    for command in ("kdotool", "spectacle"):
        if not shutil.which(command):
            parser.error(f"{command} is required; run this from your KDE Plasma desktop.")
    args.binary = args.binary.absolute()
    if not args.binary.is_file():
        parser.error("Application not found; run `just nix-build` or supply --binary.")
    args.output = args.output.absolute()
    targets = [args.output / (OVERVIEW[preset] if args.overview else
                             f"qownnotes-{preset}.png") for preset in args.presets]
    if any(path.exists() for path in targets):
        parser.error("Output images already exist; choose a new --output directory.")
    args.output.mkdir(parents=True, exist_ok=True)
    if args.overview:
        coverage = overview_coverage()
        pending = sum(status != "automated" for status in coverage.values())
        print(f"Overview: capturing {len(OVERVIEW)} scenes; {pending} still need "
              "dedicated fixtures (see docs/screenshots.md).", flush=True)
    active = run("kdotool", "getactivewindow")
    print("Capturing demo windows. Please avoid switching focus until finished.", flush=True)
    try:
        for preset, output in zip(args.presets, targets):
            temporary_root = "/tmp/opencode" if Path("/tmp/opencode").is_dir() else None
            with tempfile.TemporaryDirectory(prefix="qownnotes-screenshot-",
                                             dir=temporary_root) as directory:
                capture(args, preset, output, Path(directory))
        if args.overview:
            (args.output / "overview-coverage.json").write_text(
                json.dumps(coverage, indent=2) + "\n")
    finally:
        if active:
            subprocess.run(["kdotool", "windowactivate", active], timeout=30, check=False)


if __name__ == "__main__":
    try:
        main()
    except (RuntimeError, OSError, subprocess.SubprocessError) as error:
        sys.exit(f"Screenshot failed: {error}")
