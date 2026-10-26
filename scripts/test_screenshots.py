"""Tests for screenshot profile creation (no desktop or Qt required)."""

import configparser
from contextlib import closing
import os
from pathlib import Path
import sqlite3
import tempfile
import unittest
from unittest.mock import patch

import screenshots


class ScreenshotTests(unittest.TestCase):
    def test_every_scenario_has_colored_tags_and_valid_assignments(self):
        with tempfile.TemporaryDirectory() as directory:
            for scenario in screenshots.SCENARIOS:
                with self.subTest(scenario=scenario):
                    base = Path(directory) / scenario
                    screenshots.prepare_profile(base, scenario)
                    notes = base / "Demo notes"
                    with closing(sqlite3.connect(notes / "notes.sqlite")) as db:
                        colors = db.execute("SELECT color, dark_color FROM tag").fetchall()
                        self.assertEqual(len(set(colors)), 4)
                        self.assertTrue(all(light != dark for light, dark in colors))
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
            settings = configparser.ConfigParser(interpolation=None)
            settings.read(base / f"config/PBE/QOwnNotes-{session}.conf")
            self.assertIn(r"\0\0\0\x1\0\0\x2\x80\0\0\0\0\xfc",
                          settings["layout-screenshot"]["windowState"])

    def test_overview_coverage_tracks_the_carousel(self):
        coverage = screenshots.overview_coverage()
        self.assertTrue(set(screenshots.OVERVIEW.values()).issubset(coverage))
        self.assertEqual(len(screenshots.OVERVIEW.values()),
                         len(set(screenshots.OVERVIEW.values())))
        self.assertEqual(coverage["screenshot.png"], "automated")
        self.assertEqual(coverage["screenshot-darkmode.png"], "automated")
        self.assertEqual(coverage["screenshot-versioning.png"], "needs dedicated fixture")

    def test_only_full_screenshot_gets_a_wider_preview(self):
        with tempfile.TemporaryDirectory() as directory:
            for preset in screenshots.PRESETS:
                with self.subTest(preset=preset):
                    base = Path(directory) / preset
                    _, session = screenshots.prepare_profile(base, preset)
                    settings = configparser.ConfigParser(interpolation=None)
                    settings.read(base / f"config/PBE/QOwnNotes-{session}.conf")
                    state = settings["layout-screenshot"]["windowState"]
                    if preset == "full":
                        self.assertIn(
                            r"\0\0\0\x1\0\0\x1\xe0\0\0\0\0\xfc", state)
                        self.assertTrue(settings.getboolean(
                            "General", "dockWasInitializedOnce"))
                        self.assertNotEqual(
                            state, screenshots.layout_settings(preset)["windowState"])
                    else:
                        self.assertEqual(
                            state, screenshots.layout_settings(preset)["windowState"])
                        self.assertNotIn("dockWasInitializedOnce", settings["General"])

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
