"""Tests for screenshot profile creation (no desktop or Qt required)."""

import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

import screenshots


class ScreenshotTests(unittest.TestCase):
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
