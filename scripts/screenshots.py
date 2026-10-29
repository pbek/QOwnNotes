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
    "search-all-notes": "full",
    "search-current-note": "full",
    "other-workspace": "preview-only",
    "russian": "full",
    "diff": "full",
    "scripting": "full",
    "note-folders": "full",
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
    "other-workspace": "screenshot-other-workspace.png",
    "scripting": "screenshot-qml.png",
    "russian": "screenshot-russian.png",
    "search-all-notes": "screenshot-search-in-all-notes.png",
    "search-current-note": "screenshot-search-in-current-note.png",
    "note-folders": "screenshot-settings-note-folders.png",
    "diff": "screenshot-diff.png",
}
STARTUP_ACTIONS = {
    "distraction-free": "actionToggle_distraction_free_mode",
    "scripting": "actionScript_settings",
    "note-folders": "actionSet_Cloud_Folder",
}
# Scenes that capture a dialog instead of the main window: the dialog's title and its
# size in logical pixels at a 10 pt font, or None to keep the dialog's own size.
DIALOGS = {
    "scripting": ("Settings", None),
    "note-folders": ("Settings", None),
    "diff": ("modified externally", (960, 640)),
}
# A helper script selects the word in the current note, then triggers the search action,
# which searches for the selected text.
SEARCHES = {
    "search-all-notes": ("place", "action_Find_note"),
    "search-current-note": ("idea", "action_Find_text_in_note"),
}
# The note that is selected on startup and expected in the main window title.
SELECTED_NOTES = {
    "note-relations": "Connected ideas",
    "russian": "Немного места для идей",
}
DEFAULT_SELECTED_NOTE = "A little room for ideas"
RUSSIAN_TAG_NAMES = {1: "Идеи", 2: "Проекты", 3: "Личное", 4: "Чтение", 5: "Рабочее место"}
# Extra note folders shown in the note folder settings (name, directory, first note).
EXTRA_NOTE_FOLDERS = (
    ("Work", "Work notes", "Meeting notes"),
    ("Journal", "Journal", "Morning pages"),
)
# The external change shown in the diff dialog.
DIFF_CHANGES = (
    ("Your own space.", "Your own rules."),
    ("- [ ] Turn a good idea into a first draft",
     "- [x] Turn a good idea into a first draft"),
    ("Everything starts with a note.",
     "Everything starts with a note, and grows with the next one."),
)
# The website carousel and the README/AppStream metadata use identical copies.
PUBLISH_DIRECTORIES = (
    ROOT / "webpage/src/.vuepress/public/screenshots",
    ROOT / "screenshots",
)
PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"
# Desktop session variables needed to talk to KWin and to show Plasma-themed windows.
SESSION_VARIABLES = (
    "KDE_SESSION_VERSION", "KDE_FULL_SESSION", "KDE_SESSION_UID",
    "XDG_CURRENT_DESKTOP", "XDG_SESSION_DESKTOP", "XDG_SESSION_TYPE",
    "WAYLAND_DISPLAY", "DISPLAY", "XAUTHORITY", "QT_PLUGIN_PATH", "XCURSOR_PATH",
)
# Tag colors are row backgrounds behind regular text: soft pastels for light
# mode and muted, deep tones for dark mode keep the text readable.
TAGS = {
    # id: (name, parent id, light color, dark color)
    1: ("Ideas", 0, "#cfe4fa", "#24476b"),
    2: ("Projects", 0, "#e2d7f6", "#46386b"),
    3: ("Personal", 0, "#d3f0dd", "#2b5a40"),
    4: ("Reading", 0, "#fde2c6", "#6b4a26"),
    5: ("Workspace", 2, "#f8d8e4", "#683349"),
}
# Sizes below are logical pixels at Plasma's default 10 pt font. They all grow with
# the desktop font size (see font_scale()), so larger fonts give larger screenshots
# with the same proportions instead of cramped panels.
WINDOW_WIDTH = 1440
WINDOW_HEIGHT = 960
# The preview pane, so rendered Markdown has room to breathe.
PREVIEW_WIDTH = 560
GRAPH_WIDTH = 640
VERTICAL_PREVIEW_HEIGHT = 400
# The preview-only layout stacks all panels, including the note tag buttons, on the left.
PREVIEW_ONLY_SIDEBAR_WIDTH = 400
# One row of tag buttons, like the built-in preset (59).
NOTE_TAGS_PANEL_HEIGHT = 60
# The one-column layout is meant to be shown as a narrow window.
WINDOW_WIDTHS = {"1col": 640}


def overview_coverage():
    carousel = ROOT / "webpage/src/.vuepress/components/ScreenshotCarousel.vue"
    filenames = re.findall(r'src="/screenshots/([^"/]+\.png)"', carousel.read_text())
    return {filename: ("automated" if filename in OVERVIEW.values() else
                       "needs dedicated fixture") for filename in filenames}


def latest_overview_capture(base=ROOT / "build-screenshots"):
    captures = sorted(path.parent for path in base.glob("*/overview-coverage.json"))
    if not captures:
        raise RuntimeError("No overview capture found; run `just screenshots --overview` first.")
    return captures[-1]


def publish(source, destinations=PUBLISH_DIRECTORIES):
    """Copy reviewed overview images over the website and repository screenshots."""
    source = source.absolute()
    if not source.is_dir():
        raise RuntimeError(f"Capture directory not found: {source}")
    images = [source / name for name in OVERVIEW.values() if (source / name).is_file()]
    if not images:
        raise RuntimeError(f"No overview images found in {source}.")
    # Validate everything first so a bad file never leaves a partial update behind.
    for image in images:
        with image.open("rb") as file:
            if file.read(len(PNG_SIGNATURE)) != PNG_SIGNATURE:
                raise RuntimeError(f"Not a PNG image: {image}")
    for destination in destinations:
        if not destination.is_dir():
            raise RuntimeError(f"Screenshot directory not found: {destination}")
    print(f"Publishing from {source}", flush=True)
    for image in images:
        for destination in destinations:
            target = destination / image.name
            if target.is_file() and target.read_bytes() == image.read_bytes():
                continue
            shutil.copyfile(image, target)
            print(f"Replaced {target}")
    skipped = sorted(set(OVERVIEW.values()) - {image.name for image in images})
    if skipped:
        print("Kept existing: " + ", ".join(skipped))
    return images


def selected_note(scenario):
    return SELECTED_NOTES.get(scenario, DEFAULT_SELECTED_NOTE)


def seed_demo_tags(notes, tag_names=None):
    """Create a private version-12 note database; the app migrates it on startup."""
    tag_names = tag_names or {}
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
            "INSERT INTO tag (id, name, parent_id, color, dark_color) VALUES (?, ?, ?, ?, ?)",
            [(tag_id, tag_names.get(tag_id, name), parent, light, dark)
             for tag_id, (name, parent, light, dark) in TAGS.items()])
        assignments = {
            "A little room for ideas.md": (1, 2),
            "Project field notes.md": (5,),
            "Reading journal.md": (4,),
            "Weekend possibilities.md": (3,),
            "Connected ideas.md": (1, 2),
            "Projects.md": (2,),
            "Reading.md": (4,),
            "Weekends.md": (3,),
            "Немного места для идей.md": (1, 2),
            "Заметки о проекте.md": (5,),
            "Дневник чтения.md": (4,),
            "Планы на выходные.md": (3,),
        }
        for note in notes.rglob("*.md"):
            for tag_id in assignments.get(note.name, (2,)):
                folder = note.parent.relative_to(notes).as_posix()
                db.execute(
                    "INSERT INTO noteTagLink (tag_id, note_file_name, "
                    "note_sub_folder_path) VALUES (?, ?, ?)",
                    (tag_id, note.stem, "" if folder == "." else folder))
        db.commit()


def search_script(word, action):
    """A QML script that selects a word in the current note and triggers a search action."""
    return (
        "import QtQml 2.0\n"
        "import QOwnNotesTypes 1.0\n\n"
        "Script {\n"
        "    property QtObject timer: Timer {\n"
        "        interval: 1500\n"
        "        running: true\n"
        "        onTriggered: {\n"
        f"            var word = {json.dumps(word)};\n"
        "            var start = script.currentNote().noteText.indexOf(word);\n"
        "            script.noteTextEditSetSelection(start, start + word.length);\n"
        f"            script.triggerMenuAction({json.dumps(action)});\n"
        "        }\n"
        "    }\n"
        "}\n")


def seed_app_database(data, session, scripts=(), note_folders=()):
    """Create the app's disk database with demo scripts and note folders.

    Only the tables that are seeded are created, with their current columns. The app
    creates everything else on startup; its migrations use CREATE TABLE IF NOT EXISTS,
    and adding the already existing columns fails harmlessly.
    """
    for app in ("QOwnNotes", "QOwnNotesDebug"):
        directory = data / "PBE" / f"{app}-{session}"
        directory.mkdir(parents=True, exist_ok=True)
        with closing(sqlite3.connect(directory / "QOwnNotes.sqlite")) as db:
            db.executescript("""
                CREATE TABLE script (
                    id INTEGER PRIMARY KEY, name VARCHAR(255), script_path TEXT,
                    enabled BOOLEAN DEFAULT 1, priority INTEGER DEFAULT 0,
                    identifier VARCHAR(255), info_json TEXT, settings_variables_json TEXT);
                CREATE TABLE noteFolder (
                    id INTEGER PRIMARY KEY, name VARCHAR(255), local_path VARCHAR(255),
                    remote_path VARCHAR(255), priority INTEGER DEFAULT 0,
                    active_tag_id INTEGER, show_subfolders BOOLEAN DEFAULT 0,
                    active_note_sub_folder_data TEXT, use_git BOOLEAN DEFAULT 0,
                    cloud_connection_id INTEGER DEFAULT 1);
            """)
            db.executemany("INSERT INTO script (name, script_path, priority) VALUES (?, ?, ?)",
                           [(name, str(path), priority)
                            for priority, (name, path) in enumerate(scripts)])
            db.executemany(
                "INSERT INTO noteFolder (id, name, local_path, remote_path, priority) "
                "VALUES (?, ?, ?, ?, ?)",
                [(index + 1, name, str(path), remote, index)
                 for index, (name, path, remote) in enumerate(note_folders)])
            db.commit()


def apply_diff_changes(note):
    text = note.read_text()
    for old, new in DIFF_CHANGES:
        if text.count(old) != 1:
            raise RuntimeError(f"The diff demo note no longer contains: {old}")
        text = text.replace(old, new)
    # Overwrite the file with a single write instead of truncating it first, so the
    # app's file watcher never sees an empty note. Replacing the file would lose the watch.
    with note.open("r+", encoding="utf-8") as file:
        file.write(text)
        file.truncate()


def source_version():
    match = re.search(r'#define VERSION "([^"]+)"', (ROOT / "src/version.h").read_text())
    return match.group(1) if match else None


def binary_version(binary):
    try:
        output = subprocess.run([str(binary), "--version"], capture_output=True,
                                text=True, timeout=30).stdout
    except (OSError, subprocess.SubprocessError):
        return None
    match = re.search(r"QOwnNotes (\S+)", output)
    return match.group(1) if match else None


def systemd_environment():
    try:
        output = subprocess.run(["systemctl", "--user", "show-environment"],
                                capture_output=True, text=True, timeout=10).stdout
    except (OSError, subprocess.SubprocessError):
        return {}
    return dict(line.split("=", 1) for line in output.splitlines() if "=" in line)


def import_desktop_session(env, session):
    """Fill in Plasma's display variables when started outside a desktop terminal.

    Terminals inside multiplexers, IDEs, or SSH sessions can lack the variables kdotool,
    Spectacle and Qt need, while Plasma still exports them to the systemd user manager.
    """
    if session.get("KDE_SESSION_VERSION") != "6":
        return []
    imported = []
    for key in SESSION_VARIABLES:
        value = session.get(key)
        missing = not env.get(key) or (key == "XDG_SESSION_TYPE" and
                                       env[key] not in ("wayland", "x11"))
        if value and missing:
            env[key] = value
            imported.append(key)
    return imported


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


def decode_state(value):
    """Decode a QSettings @ByteArray(...) value, including its INI escapes."""
    value = value.strip('"')
    if not value.startswith("@ByteArray(") or not value.endswith(")"):
        raise RuntimeError("Unexpected window state format.")
    body = value[len("@ByteArray("):-1]
    escapes = {"a": 7, "b": 8, "t": 9, "n": 10, "v": 11, "f": 12, "r": 13,
               "\\": 92, '"': 34, "'": 39, "?": 63}
    data = bytearray()
    index = 0
    while index < len(body):
        char = body[index]
        if char != "\\":
            data.append(ord(char))
            index += 1
            continue
        escape = body[index + 1]
        # Like Qt, hex and octal escapes consume as many digits as follow.
        if escape == "x":
            digits = re.match(r"[0-9a-fA-F]+", body[index + 2:]).group()
            data.append(int(digits, 16))
            index += 2 + len(digits)
        elif escape in "01234567":
            digits = re.match(r"[0-7]+", body[index + 1:]).group()
            data.append(int(digits, 8))
            index += 1 + len(digits)
        else:
            data.append(escapes[escape])
            index += 2
    return data


def encode_state(data):
    # Escaping every byte keeps the value unambiguous for Qt's INI parser.
    return '"@ByteArray(' + "".join(f"\\x{byte:02x}" for byte in data) + ')"'


def dock_name(name):
    encoded = name.encode("utf-16-be")
    return len(encoded).to_bytes(4, "big") + encoded


def find_once(data, needle, description):
    if data.count(needle) != 1:
        raise RuntimeError(f"The layout's {description} has changed.")
    return data.index(needle)


def set_dock_geometry(data, name, position, size):
    # A dock item is stored as its name, a flags byte, then position and size.
    offset = find_once(data, dock_name(name), name) + len(dock_name(name)) + 1
    data[offset:offset + 8] = position.to_bytes(4, "big") + size.to_bytes(4, "big")


def dock_geometry(data, name):
    offset = find_once(data, dock_name(name), name) + len(dock_name(name)) + 1
    return (int.from_bytes(data[offset:offset + 4], "big"),
            int.from_bytes(data[offset + 4:offset + 8], "big"))


def resize_docks(data, order, sizes, donor):
    """Resize docks stacked in one dock area, keeping its total extent.

    The donor dock absorbs the difference; positions are recomputed in order with
    Qt's one-pixel separators between the docks.
    """
    current = {name: dock_geometry(data, name) for name in order}
    for name, (position, size) in current.items():
        if size <= 0:
            raise RuntimeError(f"The layout's {name} has no stored size.")
    new = {name: sizes.get(name, size) for name, (_, size) in current.items()}
    new[donor] -= sum(new.values()) - sum(size for _, size in current.values())
    if new[donor] < 100:
        raise RuntimeError(f"Not enough room left for the layout's {donor}.")
    position = current[order[0]][0]
    for name in order:
        set_dock_geometry(data, name, position, new[name])
        position += new[name] + 1


def set_dock_area_width(data, area, width):
    # A dock area starts with its index, its size and a sequence marker (0xfc).
    matches = [match.start() for match in re.finditer(
        re.escape(area.to_bytes(4, "big")) + rb".{8}\xfc", data, re.DOTALL)]
    if len(matches) != 1:
        raise RuntimeError("The layout's dock areas have changed.")
    offset = matches[0] + 4
    data[offset:offset + 4] = width.to_bytes(4, "big")


def rename_dock(data, old, new):
    find_once(data, dock_name(old), old)
    return data.replace(dock_name(old), dock_name(new))


def font_scale(desktop_config):
    """How much larger the desktop font is than Plasma's default 10 pt font.

    Panels that must fit a row of widgets are sized with this factor, because larger
    fonts make buttons taller and wider in logical pixels. Display scaling is not
    included; it does not change logical sizes.
    """
    try:
        text = (desktop_config / "kdeglobals").read_text(errors="replace")
    except OSError:
        return 1.0
    match = re.search(r"^font=[^,\n]*,(\d+(?:\.\d+)?)", text, re.MULTILINE)
    return max(1.0, float(match.group(1)) / 10) if match else 1.0


def screenshot_window_state(scenario, preset, value, scale=1.0):
    data = decode_state(value)
    if preset == "full":
        # Widen the right-hand dock area (index 1) holding the preview, or the graph.
        # Change only the screenshot profile, not the application's built-in preset.
        set_dock_area_width(data, 1, round(scale * (
            GRAPH_WIDTH if scenario == "note-relations" else PREVIEW_WIDTH)))
    if scenario == "note-relations":
        # Reuse the generously sized preview dock for the graph, hiding the preview.
        data = rename_dock(data, "notePreviewDockWidget", "noteGraphicsViewDockWidget")
        data = rename_dock(data, "logDockWidget", "notePreviewDockWidget")
    if preset == "full-vertical":
        # Give the preview above the editor more height; the editor keeps the rest.
        resize_docks(data, ("notePreviewDockWidget", "noteEditDockWidget"),
                     {"notePreviewDockWidget": round(VERTICAL_PREVIEW_HEIGHT * scale)},
                     "noteEditDockWidget")
        # Without a central widget, both dock areas share the window width; store
        # widths that add up to the window so the extra width is not all given to
        # the panels on the left.
        sidebar = round(PREVIEW_ONLY_SIDEBAR_WIDTH * scale)
        set_dock_area_width(data, 0, sidebar)
        set_dock_area_width(data, 1, round(WINDOW_WIDTH * scale) - sidebar - 10)
    if preset == "preview-only":
        # Widen the left column and give the note tag buttons below it room for a
        # full row, taking the height from the short demo note list.
        set_dock_area_width(data, 0, round(PREVIEW_ONLY_SIDEBAR_WIDTH * scale))
        resize_docks(data, ("taggingDockWidget", "noteSearchDockWidget",
                            "noteFolderDockWidget", "noteListDockWidget",
                            "noteNavigationDockWidget", "noteTagDockWidget"),
                     {"noteTagDockWidget": round(NOTE_TAGS_PANEL_HEIGHT * scale)},
                     "noteListDockWidget")
    return encode_state(data)


def prepare_profile(base, preset):
    scenario = preset
    preset = SCENARIOS[scenario]
    config = base / "config"
    (config / "PBE").mkdir(parents=True)
    notes = base / "Demo notes"
    shutil.copytree(ROOT / ("scripts/screenshot-notes-ru" if scenario == "russian" else
                            "scripts/screenshot-notes"), notes)
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
    seed_demo_tags(notes, RUSSIAN_TAG_NAMES if scenario == "russian" else None)
    # The most recently modified note is selected on a fresh profile.
    os.utime(notes / f"{selected_note(scenario)}.md", None)

    session = "Demo"
    scripts = []
    if scenario in SEARCHES:
        script = base / "scripts/search.qml"
        script.parent.mkdir()
        script.write_text(search_script(*SEARCHES[scenario]))
        scripts.append(("Search demo", script))
    if scenario == "scripting":
        # The settings show the script path, so it must not point into the user's home.
        script = base / "scripts/daily-journal.qml"
        script.parent.mkdir()
        shutil.copyfile(ROOT / "scripts/screenshot-scripts/daily-journal.qml", script)
        scripts.append(("Daily journal", script))
    note_folders = []
    if scenario == "note-folders":
        note_folders.append(("Everyday notes", notes, "Notes"))
        for name, directory, note in EXTRA_NOTE_FOLDERS:
            (base / directory).mkdir()
            (base / directory / f"{note}.md").write_text(f"# {note}\n")
            note_folders.append((name, base / directory, f"Notes/{name}"))
    if scripts or note_folders:
        seed_app_database(base / "data", session, scripts, note_folders)

    # Copy only desktop appearance settings, never the user's application profile.
    desktop_config = Path(os.environ.get("XDG_CONFIG_HOME", Path.home() / ".config"))
    for name in ("kdeglobals", "breezerc", "oxygenrc", "kdeglobals.d",
                 "qt5ct", "qt6ct", "Kvantum"):
        source = desktop_config / name
        if source.is_dir():
            shutil.copytree(source, config / name)
        elif source.is_file():
            shutil.copy2(source, config / name)

    # Workspaces as (id, name, scenario, preset); the first one is the current workspace.
    workspaces = [("screenshot", "Screenshot", scenario, preset)]
    if scenario == "other-workspace":
        workspaces = [("reading", "Reading", scenario, preset),
                      ("writing", "Writing", "full", "full")]
    layouts = {}
    for workspace, _, workspace_scenario, workspace_preset in workspaces:
        values = layout_settings(workspace_preset)
        values["windowState"] = screenshot_window_state(
            workspace_scenario, workspace_preset, values["windowState"],
            font_scale(desktop_config))
        layouts[workspace] = values
    layout = layouts[workspaces[0][0]]
    settings = (
        "[General]\n"
        f"notesPath={notes}\n"
        "demoNotesCreated=true\n"
        "disableAutomaticUpdateDialog=true\n"
        "ShowSystemTray=false\n"
        "showStatusBarNotePath=false\n"
        "enableSocketServer=false\n"
        f"systemIconTheme={'false' if scenario == 'darkmode' else 'true'}\n"
        f"currentLayout={workspaces[0][0]}\n"
        f"layouts={', '.join(sorted(layouts))}\n"
        f"centralWidget={layout['centralWidget']}\n"
        f"noteEditIsCentralWidget={layout['noteEditIsCentralWidget']}\n"
        # Skip the first-run dock limits, which cap the note tags panel at 40 logical
        # pixels (too short for its buttons with desktop scaling) while the layout loads.
        "dockWasInitializedOnce=true\n"
    )
    if scenario in ("subfolders", "darkmode"):
        settings += (
            "showNoteSubFolders=true\n"
            "noteSubfoldersPanelDisplayAsFullTree=true\n"
            "noteSubfoldersPanelShowNotesRecursively=true\n"
        )
    if scenario == "russian":
        # An English spell checking dictionary would underline every Russian word.
        settings += "interfaceLanguage=ru\ncheckSpelling=false\n"
    if scenario == "diff":
        # Ask about every external change, even when the note was not edited in the app.
        settings += "notifyAllExternalModifications=true\n"
    if note_folders:
        settings += "currentNoteFolderId=1\n"
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
        # Show the Workspace tag below its expanded Projects parent tag.
        "tagTreeWidgetExpandState-1=2\n"
        # Keep the demo offline through an unreachable proxy (QNetworkProxy::HttpProxy),
        # so no update notice or other network state can appear in the screenshots.
        "\n[networking]\n"
        "proxyType=3\n"
        "proxyHostName=127.0.0.1\n"
        "proxyPort=9\n"
        "\n[appMetrics]\n"
        "notificationShown=true\n"
        "disableTracking=true\n"
        "disableAppHeartbeat=true\n"
    )
    for workspace, name, _, _ in workspaces:
        settings += f"\n[layout-{workspace}]\nname={name}\n"
        for key in ("centralWidget", "noteEditIsCentralWidget", "windowState",
                    "noteSubFolderDockWidgetVisible"):
            settings += f"{key}={layouts[workspace][key]}\n"

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


def find_window(process, title, timeout):
    """Return the first window of the process whose title matches, or an empty string."""
    deadline = time.monotonic() + timeout
    while time.monotonic() < deadline and process.poll() is None:
        # PID-only searches also match empty title patterns in kdotool 0.3.
        windows = run("kdotool", "search", "--all", "--pid", str(process.pid),
                      "--title", re.escape(title)).splitlines()
        if windows:
            return windows[0]
        time.sleep(0.25)
    return ""


def activate(window):
    run("kdotool", "windowactivate", window)
    time.sleep(1)
    if run("kdotool", "getactivewindow") != window:
        raise RuntimeError("Could not activate the demo window; capture cancelled.")


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
            # The main window title contains the selected note; dialogs don't.
            window = find_window(process, selected_note(preset), 45)
            if not window:
                raise RuntimeError("The demo window did not appear.")
            time.sleep(args.settle)
            title = run("kdotool", "getwindowname", window)
            if (selected_note(preset) not in title or
                    run("kdotool", "getwindowpid", window) != str(process.pid)):
                raise RuntimeError(f"Unexpected demo window: {title}")
            dialog_title, dialog_size = DIALOGS.get(preset, (None, None))
            # A dialog opened on startup is modal, so the main window can't be activated.
            if not (dialog_title and preset in STARTUP_ACTIONS):
                run("kdotool", "windowstate", "--remove", "MAXIMIZED", window)
                width = args.width
                if preset in WINDOW_WIDTHS:
                    width = min(width, round(WINDOW_WIDTHS[preset] * args.scale))
                run("kdotool", "windowsize", window, str(width), str(args.height))
                activate(window)
            if preset == "diff":
                # Change the note behind the app's back while it is active, so it asks
                # about the change in a dialog instead of a desktop notification.
                apply_diff_changes(base / "Demo notes" / f"{selected_note(preset)}.md")
            if dialog_title:
                window = find_window(process, dialog_title, 20)
                if not window:
                    raise RuntimeError(f"The {dialog_title!r} dialog did not appear.")
                time.sleep(1)
                if dialog_size:
                    run("kdotool", "windowsize", window,
                        *(str(round(size * args.scale)) for size in dialog_size))
                    # Let KWin redraw the decoration shadow for the new size.
                    time.sleep(2)
                activate(window)
                title = run("kdotool", "getwindowname", window)
            print(f"Capturing: {title}", flush=True)
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
    selection.add_argument("--publish", type=Path, nargs="?", metavar="CAPTURE_DIR",
                           const=False,
                           help="Replace website and repository screenshots with reviewed "
                                "images from an overview capture (default: the latest one)")
    parser.add_argument("--any-version", action="store_true",
                        help="Allow a binary whose version differs from src/version.h")
    parser.add_argument("--width", type=int,
                        help=f"Window width (default: {WINDOW_WIDTH}, scaled with the "
                             "desktop font size)")
    parser.add_argument("--height", type=int,
                        help=f"Window height (default: {WINDOW_HEIGHT}, scaled with the "
                             "desktop font size)")
    parser.add_argument("--settle", type=float, default=4,
                        help="Seconds to wait for startup and note indexing (default: 4)")
    args = parser.parse_args()
    if args.publish is not None:
        publish(args.publish or latest_overview_capture())
        return
    args.presets = (list(OVERVIEW) if args.overview else args.presets or
                    ["minimal", "full", "preview-only", "subfolders", "darkmode"])
    args.scale = font_scale(Path(os.environ.get("XDG_CONFIG_HOME",
                                                Path.home() / ".config")))
    args.width = args.width or round(WINDOW_WIDTH * args.scale)
    args.height = args.height or round(WINDOW_HEIGHT * args.scale)
    if args.width < 640 or args.height < 480 or args.settle < 0:
        parser.error("Use dimensions of at least 640x480 and a nonnegative settle time.")
    for command in ("kdotool", "spectacle"):
        if not shutil.which(command):
            parser.error(f"{command} is required; run this from your KDE Plasma desktop.")
    imported = import_desktop_session(os.environ, systemd_environment())
    if imported:
        print("Using the Plasma session environment: " + ", ".join(imported), flush=True)
    if os.environ.get("KDE_SESSION_VERSION") != "6":
        parser.error("No running Plasma 6 session found; run this from your KDE Plasma desktop.")
    args.binary = args.binary.absolute()
    if not args.binary.is_file():
        parser.error("Application not found; run `just nix-build` or supply --binary.")
    expected, actual = source_version(), binary_version(args.binary)
    if actual != expected and not args.any_version:
        parser.error(f"{args.binary} is version {actual or 'unknown'}, but the source is "
                     f"{expected}; rebuild with `just nix-build` or pass --any-version.")
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
    print(f"Window size: {args.width}x{args.height} logical pixels "
          f"(desktop font scale {args.scale:g})", flush=True)
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
