"""Tests for screenshot profile creation (no desktop or Qt required)."""

import configparser
from contextlib import closing
import http.client
import json
import os
from pathlib import Path
import sqlite3
import tempfile
import unittest
from urllib.error import HTTPError
from urllib.request import Request, urlopen
import xml.etree.ElementTree as ET
from unittest.mock import patch

import screenshots
from screenshot_server import CALENDAR, demo_server


def luminance(color):
    channels = [int(color[i:i + 2], 16) / 255 for i in (1, 3, 5)]
    linear = [c / 12.92 if c <= 0.04045 else ((c + 0.055) / 1.055) ** 2.4 for c in channels]
    return 0.2126 * linear[0] + 0.7152 * linear[1] + 0.0722 * linear[2]


def window_state(base, session):
    settings = configparser.ConfigParser(interpolation=None)
    settings.read(base / f"config/PBE/QOwnNotes-{session}.conf")
    return settings, screenshots.decode_state(settings["layout-screenshot"]["windowState"])


def dock_geometry(state, name):
    offset = state.index(screenshots.dock_name(name)) + len(screenshots.dock_name(name)) + 1
    return (int.from_bytes(state[offset:offset + 4], "big"),
             int.from_bytes(state[offset + 4:offset + 8], "big"))


def prepare_scenario(base, scenario):
    url = "http://127.0.0.1:12345" if scenario in screenshots.CLOUD_SCENES else None
    return screenshots.prepare_profile(base, scenario, url)


class ScreenshotTests(unittest.TestCase):
    def setUp(self):
        # Use Plasma's default font, not the developer's desktop, unless a test sets one.
        desktop = tempfile.TemporaryDirectory()
        self.addCleanup(desktop.cleanup)
        environment = patch.dict(os.environ, {"XDG_CONFIG_HOME": desktop.name})
        environment.start()
        self.addCleanup(environment.stop)

    def test_font_scale_follows_the_plasma_font(self):
        with tempfile.TemporaryDirectory() as directory:
            config = Path(directory)
            self.assertEqual(screenshots.font_scale(config), 1.0)
            for font, scale in (("Noto Sans,10,-1,5", 1.0), ("Noto Sans,14,-1,5", 1.4),
                                ("Noto Sans,9,-1,5", 1.0), ("Inter,12.5,-1,5", 1.25)):
                (config / "kdeglobals").write_text(f"[General]\nfont={font}\n")
                self.assertAlmostEqual(screenshots.font_scale(config), scale)

    def test_unattended_desktop_needs_no_active_window_before_capture(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            binary = base / "QOwnNotes"
            binary.touch()
            arguments = ["screenshots.py", "--binary", str(binary), "--presets", "minimal",
                         "--output", str(base / "images"), "--settle", "0"]
            error = screenshots.subprocess.CalledProcessError(1, ["kdotool", "getactivewindow"])
            with patch("sys.argv", arguments), \
                    patch.dict(os.environ, {"KDE_SESSION_VERSION": "6"}), \
                    patch.object(screenshots.shutil, "which", return_value="/fake/tool"), \
                    patch.object(screenshots, "systemd_environment", return_value={}), \
                    patch.object(screenshots, "binary_version", return_value=screenshots.source_version()), \
                    patch.object(screenshots, "run", side_effect=error), \
                    patch.object(screenshots, "capture") as capture:
                screenshots.main()
                capture.assert_called_once()

    def test_read_only_demo_fixtures_get_writable_private_copies(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source"
            source.mkdir()
            (source / "Ideas.md").write_text("# Ideas\n")
            (source / "Ideas.md").chmod(0o444)
            source.chmod(0o555)
            try:
                target = root / "demo"
                screenshots.copy_demo_notes(source, target)
                (target / "Projects").mkdir()
                (target / "Ideas.md").write_text("# More ideas\n")
                self.assertEqual((source / "Ideas.md").read_text(), "# Ideas\n")
                self.assertEqual((source / "Ideas.md").stat().st_mode & 0o777, 0o444)
            finally:
                source.chmod(0o755)
    def test_window_state_round_trips_through_qt_escapes(self):
        for preset in screenshots.PRESETS:
            with self.subTest(preset=preset):
                value = screenshots.layout_settings(preset)["windowState"]
                data = screenshots.decode_state(value)
                self.assertEqual(data[:4], b"\0\0\0\xff")
                self.assertEqual(screenshots.decode_state(screenshots.encode_state(data)),
                                 data)

    def test_plasma_session_is_imported_only_where_missing(self):
        session = {"KDE_SESSION_VERSION": "6", "WAYLAND_DISPLAY": "wayland-1",
                   "XDG_SESSION_TYPE": "wayland", "DISPLAY": ":0", "PATH": "/session"}
        env = {"XDG_SESSION_TYPE": "tty", "DISPLAY": ":1", "PATH": "/mine"}
        imported = screenshots.import_desktop_session(env, session)
        self.assertEqual(env["KDE_SESSION_VERSION"], "6")
        self.assertEqual(env["WAYLAND_DISPLAY"], "wayland-1")
        self.assertEqual(env["XDG_SESSION_TYPE"], "wayland")
        self.assertEqual(env["DISPLAY"], ":1")
        self.assertEqual(env["PATH"], "/mine")
        self.assertNotIn("DISPLAY", imported)
        # Nothing is imported without a running Plasma 6 session.
        env = {}
        self.assertEqual(screenshots.import_desktop_session(
            env, {"KDE_SESSION_VERSION": "5", "DISPLAY": ":0"}), [])
        self.assertEqual(env, {})

    def test_preview_only_note_tags_panel_grows_with_the_desktop_font(self):
        original = screenshots.decode_state(
            screenshots.layout_settings("preview-only")["windowState"])
        order = ("taggingDockWidget", "noteSearchDockWidget", "noteFolderDockWidget",
                 "noteListDockWidget", "noteNavigationDockWidget", "noteTagDockWidget")
        for font, height, width in (("", 60, 400), ("font=Noto Sans,10,-1,5", 60, 400),
                                    ("font=Noto Sans,14,-1,5", 84, 560)):
            with self.subTest(font=font), tempfile.TemporaryDirectory() as directory:
                root = Path(directory)
                (root / "desktop").mkdir()
                (root / "desktop/kdeglobals").write_text(f"[General]\n{font}\n")
                with patch.dict(os.environ, {"XDG_CONFIG_HOME": str(root / "desktop")}):
                    _, session = screenshots.prepare_profile(root / "demo", "preview-only")
                settings, state = window_state(root / "demo", session)
                self.assertTrue(settings.getboolean("General", "dockWasInitializedOnce"))
                tags = dock_geometry(state, "noteTagDockWidget")
                self.assertEqual(tags[1], height)
                self.assertEqual(sum(tags), sum(dock_geometry(original, "noteTagDockWidget")))
                for above, below in zip(order, order[1:]):
                    position, size = dock_geometry(state, above)
                    self.assertEqual(dock_geometry(state, below)[0], position + size + 1)
                self.assertIn(b"\0\0\0\0" + width.to_bytes(4, "big"), state)

    def test_demo_profile_is_offline_and_versions_are_read(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            _, session = screenshots.prepare_profile(base, "minimal")
            settings, _ = window_state(base, session)
            self.assertEqual(settings["networking"]["proxyType"], "3")
            self.assertEqual(settings["networking"]["proxyHostName"], "127.0.0.1")
            self.assertRegex(screenshots.source_version(), r"^\d+\.\d+\.\d+$")
            fake = base / "QOwnNotes"
            fake.write_text("#!/bin/sh\necho 'QOwnNotes 1.2.3'\n")
            fake.chmod(0o755)
            self.assertEqual(screenshots.binary_version(fake), "1.2.3")

    def test_vertical_preview_gets_more_height(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            _, session = screenshots.prepare_profile(base, "full-vertical")
            _, state = window_state(base, session)
            original = screenshots.decode_state(
                screenshots.layout_settings("full-vertical")["windowState"])
            preview = dock_geometry(state, "notePreviewDockWidget")
            edit = dock_geometry(state, "noteEditDockWidget")
            old_edit = dock_geometry(original, "noteEditDockWidget")
            self.assertEqual(preview[1], screenshots.VERTICAL_PREVIEW_HEIGHT)
            self.assertGreater(preview[1], dock_geometry(original, "notePreviewDockWidget")[1])
            self.assertEqual(edit[0], preview[0] + preview[1] + 1)
            self.assertEqual(sum(edit), sum(old_edit))
            self.assertGreater(edit[1], 300)

    def test_every_scenario_has_colored_tags_and_valid_assignments(self):
        with tempfile.TemporaryDirectory() as directory:
            for scenario in screenshots.SCENARIOS:
                with self.subTest(scenario=scenario):
                    base = Path(directory) / scenario
                    prepare_scenario(base, scenario)
                    notes = base / "Demo notes"
                    with closing(sqlite3.connect(notes / "notes.sqlite")) as db:
                        colors = db.execute("SELECT color, dark_color FROM tag").fetchall()
                        self.assertEqual(len(set(colors)), len(screenshots.TAGS))
                        for light, dark in colors:
                            # Pastel rows behind dark text, deep rows behind light text.
                            self.assertGreater(luminance(light), 0.6)
                            self.assertLess(luminance(dark), 0.12)
                        self.assertEqual(db.execute(
                            "SELECT COUNT(*) FROM tag WHERE parent_id = 2").fetchone(), (1,))
                        assignments = db.execute(
                            "SELECT note_file_name, note_sub_folder_path FROM noteTagLink"
                        ).fetchall()
                        self.assertGreaterEqual(len(assignments), 4)
                        for name, folder in assignments:
                            self.assertTrue((notes / folder / f"{name}.md").is_file())
                        self.assertEqual(db.execute("PRAGMA integrity_check").fetchone(),
                                         ("ok",))

    def test_subfolders_and_dark_mode_profiles(self):
        with tempfile.TemporaryDirectory() as directory:
            for scenario in ("subfolders", "darkmode"):
                with self.subTest(scenario=scenario):
                    base = Path(directory) / scenario
                    _, session = screenshots.prepare_profile(base, scenario)
                    notes = base / "Demo notes"
                    self.assertTrue((notes / "Projects/Small adventures/Project field notes.md"
                                     ).is_file())
                    self.assertEqual(len(list(notes.rglob("*.md"))), 4)
                    settings = configparser.ConfigParser(interpolation=None)
                    settings.read(base / f"config/PBE/QOwnNotes-{session}.conf")
                    self.assertTrue(settings.getboolean("General", "showNoteSubFolders"))
                    self.assertTrue(settings.getboolean("General", "dockWasInitializedOnce"))
                    for source in ("system", "linux"):
                        self.assertEqual(settings["MessageBoxOverride"][f"{source}-light-mode"],
                                         "65536")
                    if scenario == "darkmode":
                        self.assertTrue(settings.getboolean("General", "darkMode"))
                        self.assertTrue(settings.getboolean("General", "darkModeColors"))
                        self.assertTrue(settings.getboolean("General", "internalIconTheme"))
                        self.assertFalse(settings.getboolean("General", "systemIconTheme"))
                        self.assertIn("cdbf28fc", settings["Editor"]["CurrentSchemaKey"])

    def test_note_relations_has_links_and_a_wide_graph(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            _, session = screenshots.prepare_profile(base, "note-relations")
            text = (base / "Demo notes/Connected ideas.md").read_text()
            for name in ("Projects", "Reading", "Weekends"):
                self.assertIn(f"]({name}.md)", text)
                self.assertTrue((base / f"Demo notes/{name}.md").is_file())
            _, state = window_state(base, session)
            self.assertIn(b"\0\0\0\x01" + screenshots.GRAPH_WIDTH.to_bytes(4, "big") +
                          b"\0\0\0\0\xfc", state)
            self.assertIn(screenshots.dock_name("noteGraphicsViewDockWidget"), state)
            self.assertEqual(state.count(screenshots.dock_name("notePreviewDockWidget")), 1)
            self.assertNotIn(screenshots.dock_name("logDockWidget"), state)

    def test_overview_coverage_tracks_the_carousel(self):
        coverage = screenshots.overview_coverage()
        self.assertTrue(set(screenshots.OVERVIEW.values()).issubset(coverage))
        self.assertEqual(len(screenshots.OVERVIEW.values()),
                         len(set(screenshots.OVERVIEW.values())))
        self.assertEqual(coverage["screenshot.png"], "automated")
        self.assertEqual(coverage["screenshot-darkmode.png"], "automated")
        self.assertEqual(coverage["screenshot-versioning.png"], "automated")
        self.assertTrue(all(status == "automated" for status in coverage.values()))
        self.assertEqual(len(coverage), 23)
        self.assertTrue(set(screenshots.OVERVIEW).issubset(screenshots.SCENARIOS))

    def test_every_carousel_image_is_published_in_the_repository(self):
        for name in screenshots.overview_coverage():
            with self.subTest(filename=name):
                image = screenshots.ROOT / "screenshots" / name
                self.assertTrue(image.is_file(), image)
                with image.open("rb") as file:
                    self.assertEqual(file.read(len(screenshots.PNG_SIGNATURE)),
                                     screenshots.PNG_SIGNATURE)

    def test_every_scenario_selects_an_existing_note(self):
        with tempfile.TemporaryDirectory() as directory:
            for scenario in screenshots.SCENARIOS:
                with self.subTest(scenario=scenario):
                    base = Path(directory) / scenario
                    prepare_scenario(base, scenario)
                    note = base / "Demo notes" / f"{screenshots.selected_note(scenario)}.md"
                    self.assertTrue(note.is_file())
                    # The newest note is selected on startup.
                    newest = max((base / "Demo notes").rglob("*.md"),
                                 key=lambda path: path.stat().st_mtime_ns)
                    self.assertEqual(newest, note)

    def app_database(self, base, session):
        database = base / f"data/PBE/QOwnNotes-{session}/QOwnNotes.sqlite"
        self.assertTrue(database.is_file())
        return closing(sqlite3.connect(database))

    def test_search_scenes_select_a_word_with_a_helper_script(self):
        with tempfile.TemporaryDirectory() as directory:
            for scenario, (word, action) in screenshots.SEARCHES.items():
                with self.subTest(scenario=scenario):
                    base = Path(directory) / scenario
                    _, session = screenshots.prepare_profile(base, scenario)
                    with self.app_database(base, session) as db:
                        scripts = db.execute("SELECT script_path, enabled FROM script").fetchall()
                    self.assertEqual(len(scripts), 1)
                    path, enabled = scripts[0]
                    self.assertTrue(enabled)
                    self.assertTrue(Path(path).is_relative_to(base))
                    script = Path(path).read_text()
                    self.assertIn(f'"{word}"', script)
                    self.assertIn(f'triggerMenuAction("{action}")', script)
                    note = base / "Demo notes" / f"{screenshots.selected_note(scenario)}.md"
                    self.assertIn(word, note.read_text())

    def test_scripting_scene_shows_a_demo_script_outside_the_users_home(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            _, session = screenshots.prepare_profile(base, "scripting")
            with self.app_database(base, session) as db:
                (name, path), = db.execute("SELECT name, script_path FROM script").fetchall()
            self.assertEqual(name, "Daily journal")
            self.assertTrue(Path(path).is_relative_to(base))
            self.assertIn("settingsVariables", Path(path).read_text())
            self.assertEqual(screenshots.STARTUP_ACTIONS["scripting"], "actionScript_settings")

    def test_note_folders_scene_has_several_existing_folders(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            _, session = screenshots.prepare_profile(base, "note-folders")
            with self.app_database(base, session) as db:
                folders = db.execute(
                    "SELECT id, name, local_path FROM noteFolder ORDER BY priority").fetchall()
            self.assertEqual(len(folders), 1 + len(screenshots.EXTRA_NOTE_FOLDERS))
            self.assertEqual(folders[0][2], str(base / "Demo notes"))
            for _, _, path in folders:
                self.assertTrue(any(Path(path).glob("*.md")))
            settings = configparser.ConfigParser(interpolation=None)
            settings.read(base / f"config/PBE/QOwnNotes-{session}.conf")
            self.assertEqual(settings["General"]["currentNoteFolderId"], str(folders[0][0]))
            self.assertEqual(settings["General"]["notesPath"], folders[0][2])

    def test_other_workspace_scene_has_two_workspaces(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            _, session = screenshots.prepare_profile(base, "other-workspace")
            settings = configparser.ConfigParser(interpolation=None)
            settings.read(base / f"config/PBE/QOwnNotes-{session}.conf")
            self.assertEqual(settings["General"]["currentLayout"], "reading")
            self.assertEqual(settings["General"]["layouts"], "reading, writing")
            self.assertEqual(settings["layout-reading"]["name"], "Reading")
            self.assertEqual(settings["layout-writing"]["name"], "Writing")
            self.assertEqual(settings["layout-reading"]["centralWidget"], "note-preview")
            self.assertNotEqual(settings["layout-reading"]["windowState"],
                                settings["layout-writing"]["windowState"])

    def test_russian_scene_uses_russian_ui_notes_and_tags(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            _, session = screenshots.prepare_profile(base, "russian")
            settings = configparser.ConfigParser(interpolation=None)
            settings.read(base / f"config/PBE/QOwnNotes-{session}.conf")
            self.assertEqual(settings["General"]["interfaceLanguage"], "ru")
            self.assertFalse(settings.getboolean("General", "checkSpelling"))
            notes = sorted(path.name for path in (base / "Demo notes").glob("*.md"))
            self.assertEqual(len(notes), 4)
            self.assertTrue(all(any("а" <= char <= "я" for char in name.lower())
                                for name in notes))
            with closing(sqlite3.connect(base / "Demo notes/notes.sqlite")) as db:
                names = {name for name, in db.execute("SELECT name FROM tag")}
            self.assertEqual(names, set(screenshots.RUSSIAN_TAG_NAMES.values()))

    def test_diff_scene_changes_the_selected_note_in_place(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            _, session = screenshots.prepare_profile(base, "diff")
            settings = configparser.ConfigParser(interpolation=None)
            settings.read(base / f"config/PBE/QOwnNotes-{session}.conf")
            self.assertTrue(settings.getboolean("General", "notifyAllExternalModifications"))
            note = base / "Demo notes" / f"{screenshots.selected_note('diff')}.md"
            inode = note.stat().st_ino
            screenshots.apply_diff_changes(note)
            text = note.read_text()
            for old, new in screenshots.DIFF_CHANGES:
                self.assertIn(new, text)
            # The app's file watcher keeps watching the same file.
            self.assertEqual(note.stat().st_ino, inode)
            with self.assertRaises(RuntimeError):
                screenshots.apply_diff_changes(note)

    def test_scenes_without_fixtures_get_no_app_database(self):
        with tempfile.TemporaryDirectory() as directory:
            base = Path(directory)
            _, session = screenshots.prepare_profile(base, "full")
            self.assertFalse((base / f"data/PBE/QOwnNotes-{session}").exists())

    def test_publish_replaces_reviewed_overview_images_everywhere(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            capture = root / "build-screenshots/20260101-000000-000000"
            capture.mkdir(parents=True)
            (capture / "overview-coverage.json").write_text("{}\n")
            image = screenshots.PNG_SIGNATURE + b"new"
            (capture / "screenshot.png").write_bytes(image)
            (capture / "unrelated.png").write_bytes(image)
            destinations = (root / "website", root / "repository")
            for destination in destinations:
                destination.mkdir()
                (destination / "screenshot.png").write_bytes(b"old")
                (destination / "screenshot-darkmode.png").write_bytes(b"old")
            self.assertEqual(screenshots.latest_overview_capture(root / "build-screenshots"),
                             capture)
            screenshots.publish(capture, destinations)
            for destination in destinations:
                self.assertEqual((destination / "screenshot.png").read_bytes(), image)
                # Images removed from the capture during review are kept unchanged.
                self.assertEqual((destination / "screenshot-darkmode.png").read_bytes(), b"old")
                self.assertFalse((destination / "unrelated.png").exists())

    def test_publish_rejects_invalid_images_without_changes(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "capture").mkdir()
            (root / "capture/screenshot.png").write_bytes(screenshots.PNG_SIGNATURE)
            (root / "capture/screenshot-minimal.png").write_bytes(b"broken")
            (root / "website").mkdir()
            (root / "website/screenshot.png").write_bytes(b"old")
            with self.assertRaises(RuntimeError):
                screenshots.publish(root / "capture", (root / "website",))
            self.assertEqual((root / "website/screenshot.png").read_bytes(), b"old")

    def test_publish_creates_the_ignored_website_directory(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            capture = root / "capture"
            capture.mkdir()
            (capture / "screenshot.png").write_bytes(screenshots.PNG_SIGNATURE + b"demo")
            target = root / "website/public/screenshots"
            screenshots.publish(capture, (target,))
            self.assertTrue((target / "screenshot.png").is_file())

    def test_cloud_profiles_use_only_the_disposable_loopback_api(self):
        with tempfile.TemporaryDirectory() as directory:
            for scenario in screenshots.CLOUD_SCENES:
                with self.subTest(scenario=scenario):
                    base = Path(directory) / scenario
                    env, session = prepare_scenario(base, scenario)
                    settings, _ = window_state(base, session)
                    self.assertEqual(settings["networking"]["proxyPort"], "12345")
                    self.assertTrue(settings.getboolean("ownCloud", "supportEnabled"))
                    overrides = configparser.ConfigParser(interpolation=None)
                    overrides.read(base / f"config/PBE/QOwnNotes-{session}.override.conf")
                    self.assertTrue(overrides.getboolean("ownCloud", "supportEnabled"))
                    self.assertEqual(overrides["ownCloud"]["todoCalendarEnabledList"], "Everyday")
                    self.assertEqual(env["QOWNNOTES_SCREENSHOT_ROOT"], str(base))
                    with self.app_database(base, session) as db:
                        self.assertEqual(db.execute(
                            "SELECT server_url, username, password FROM cloudConnection"
                        ).fetchone(), ("http://127.0.0.1:12345", "demo", ""))
            with self.assertRaises(RuntimeError):
                screenshots.prepare_profile(Path(directory) / "external", "trash",
                                            "https://example.com")

    def test_demo_api_serves_real_protocol_fixtures_and_never_forwards(self):
        with demo_server("# Ideas\n\nYour own space.\n") as server:
            url = f"http://127.0.0.1:{server.server_port}"
            for endpoint, key in (("note/versions", "versions"), ("note/trashed", "notes")):
                with urlopen(url + "/index.php/apps/qownnotesapi/api/v1/" + endpoint) as response:
                    self.assertEqual(len(json.load(response)[key]), 3)
            with urlopen(url + "/index.php/apps/deck/api/v1.1/boards/1/stacks?details=true") as response:
                cards = json.load(response)[0]["cards"]
                self.assertEqual(len(cards), 3)
                self.assertTrue(cards[0]["labels"])
            with urlopen(Request(url + CALENDAR, method="REPORT")) as response:
                xml = ET.fromstring(response.read())
                self.assertEqual(len(xml.findall("{DAV:}response")), 3)
            with urlopen(url + CALENDAR + "1.ics") as response:
                self.assertIn(b"BEGIN:VTODO\r\n", response.read())
            # A proxy request for another origin is rejected before routing.
            connection = http.client.HTTPConnection("127.0.0.1", server.server_port)
            try:
                connection.request("GET", "http://example.com/index.php/apps/deck/api/v1.1/boards")
                response = connection.getresponse()
                self.assertEqual(response.status, 403)
                response.read()
                connection.request("CONNECT", "example.com:443")
                response = connection.getresponse()
                self.assertEqual(response.status, 403)
                response.read()
            finally:
                connection.close()
            with self.assertRaises(HTTPError) as error:
                urlopen(Request(url + "/note/restore_trashed", method="POST"))
            error.exception.close()

    def test_portable_copies_the_payload_and_never_the_real_settings(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            binary = root / "real/QOwnNotes"
            binary.parent.mkdir()
            binary.write_bytes(
                b"wrapper\0--prefix 'QT_PLUGIN_PATH' ':' '/nix/store/demo-qtsvg/lib/qt-6/plugins'\n"
                b"--prefix 'HOME' ':' '/nix/store/not-a-home'\n")
            (binary.parent / ".QOwnNotes-wrapped").write_bytes(b"\x7fELFdemo")
            base = root / "demo"
            env, session = screenshots.prepare_profile(base, "portable")
            env["APPIMAGE"] = str(binary)
            target = screenshots.prepare_portable(base, binary, env, session)
            self.assertEqual(target.read_bytes(), b"\x7fELFdemo")
            self.assertTrue((target.parent / "Data/PBE/QOwnNotes-Demo.ini").is_file())
            self.assertTrue((target.parent / "Data/Demo notes/Portable notes.md").is_file())
            self.assertIn("notesPath=Demo notes\n",
                          (target.parent / "Data/PBE/QOwnNotes-Demo.ini").read_text())
            self.assertNotIn("APPIMAGE", env)
            self.assertTrue(env["QT_PLUGIN_PATH"].startswith("/nix/store/demo-qtsvg/"))
            self.assertNotEqual(env.get("HOME"), "/nix/store/not-a-home")
            self.assertFalse((binary.parent / "Data").exists())

    def test_portable_rejects_appimages_and_shell_launchers(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            for index, header in enumerate((b"#!/bin/sh\n", b"\x7fELF\0\0\0\0AI\x02\0")):
                binary = root / f"launcher-{index}"
                binary.write_bytes(header)
                base = root / f"demo-{index}"
                env, session = screenshots.prepare_profile(base, "portable")
                with self.assertRaises(RuntimeError):
                    screenshots.prepare_portable(base, binary, env, session)
                self.assertFalse((root / "Data").exists())

    def test_only_preview_layouts_get_a_larger_preview(self):
        with tempfile.TemporaryDirectory() as directory:
            for preset in screenshots.PRESETS:
                with self.subTest(preset=preset):
                    base = Path(directory) / preset
                    _, session = screenshots.prepare_profile(base, preset)
                    settings, state = window_state(base, session)
                    original = screenshots.decode_state(
                        screenshots.layout_settings(preset)["windowState"])
                    # First-run dock limits would cap the note tags panel at 40 pixels.
                    self.assertTrue(settings.getboolean("General", "dockWasInitializedOnce"))
                    if preset == "full":
                        self.assertIn(b"\0\0\0\x01" +
                                      screenshots.PREVIEW_WIDTH.to_bytes(4, "big") +
                                      b"\0\0\0\0\xfc", state)
                    elif preset in ("full-vertical", "preview-only"):
                        self.assertNotEqual(state, original)
                    else:
                        self.assertEqual(state, original)

    def test_all_presets_have_required_settings(self):
        for preset in screenshots.PRESETS:
            with self.subTest(preset=preset):
                values = screenshots.layout_settings(preset)
                self.assertIn("@ByteArray(", values["windowState"])
                self.assertIn(values["centralWidget"],
                              ("note-edit", "note-preview", "none"))
                self.assertIn("noteSubFolderDockWidgetVisible", values)

    def test_profile_is_isolated_but_retains_desktop_appearance(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            desktop = root / "desktop"
            desktop.mkdir()
            (desktop / "kdeglobals").write_text("[General]\nColorScheme=Breeze\n")
            (desktop / "PBE").mkdir()
            private_settings = desktop / "PBE/QOwnNotes.conf"
            private_settings.write_text("private settings must not be copied")
            base = root / "demo"
            base.mkdir()
            with patch.dict(os.environ, {"XDG_CONFIG_HOME": str(desktop),
                                         "XDG_DATA_HOME": str(root / "user-data")}):
                env, session = screenshots.prepare_profile(base, "full")
            self.assertEqual((base / "config/kdeglobals").read_text(),
                             (desktop / "kdeglobals").read_text())
            self.assertFalse((base / "config/PBE/QOwnNotes.conf").exists())
            self.assertEqual(private_settings.read_text(),
                             "private settings must not be copied")
            self.assertEqual(env["XDG_CONFIG_HOME"], str(base / "config"))
            self.assertEqual(env["XDG_DATA_HOME"], str(base / "data"))
            self.assertEqual(env["XDG_CACHE_HOME"], str(base / "cache"))
            self.assertEqual(env["QOWNNOTES_SCREENSHOT_ROOT"], str(base))
            self.assertTrue((base / "screenshot-profile").is_file())
            self.assertTrue(env["XDG_DATA_DIRS"].startswith(str(root / "user-data")))
            settings = (base / f"config/PBE/QOwnNotes-{session}.conf").read_text()
            self.assertIn("disableTracking=true", settings)
            self.assertIn("enableSocketServer=false", settings)
            self.assertIn("notificationShown=true", settings)
            self.assertEqual(len(list((base / "Demo notes").glob("*.md"))), 4)
            original = screenshots.ROOT / "scripts/screenshot-notes/Reading journal.md"
            copy = base / "Demo notes/Reading journal.md"
            self.assertEqual(original.read_text(), copy.read_text())
            self.assertNotEqual(original.stat().st_ino, copy.stat().st_ino)


if __name__ == "__main__":
    unittest.main()
