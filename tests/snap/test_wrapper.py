"""Test all Qt6 launch wrappers without building or installing a snap.

Run with: python3 -m unittest discover -s tests/snap -v
"""

import json
import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[2]
WRAPPERS = (
    ROOT / "snap/local/wrapper.sh",
    ROOT / "build-systems/github/snap.qt6/snap/local/wrapper.sh",
    ROOT / "build-systems/snap/snapcraft/snap/local/wrapper.sh",
)
RECIPES = (
    ROOT / "snap/snapcraft.yaml",
    ROOT / "build-systems/github/snap.qt6/snapcraft.yaml",
    ROOT / "build-systems/snap/snapcraft/snapcraft.yaml",
)


class TestSnapRecipes(unittest.TestCase):
    def test_all_recipes_bundle_svg_plugins(self):
        for recipe in RECIPES:
            with self.subTest(recipe=recipe):
                text = recipe.read_text()
                self.assertIn("      - qt6-svg-plugins", text)
                for plugin in ("iconengines/libqsvgicon.so", "imageformats/libqsvg.so"):
                    self.assertIn(
                        '      test -f "${CRAFT_PRIME}/usr/lib/${arch_triplet}'
                        '/qt6/plugins/' + plugin + '"',
                        text,
                    )

    def test_all_recipes_include_runtime_fixes(self):
        for recipe in RECIPES:
            with self.subTest(recipe=recipe):
                text = recipe.read_text()
                for required in (
                    "base: core26",
                    "command: bin/desktop-launch $SNAP/wrapper.sh",
                    "QT_QPA_PLATFORMTHEME: xdgdesktopportal",
                    "      - wayland",
                    "      qmake6 CONFIG+=release CONFIG+=snapcraft QOwnNotes.pro",
                    "      - qt6-xdgdesktopportal-platformtheme",
                    "      - xkb-data",
                    "libqxdgdesktopportal.so",
                    '      glib-compile-schemas --strict "${schema_dir}"',
                    "    after: [desktop-qt6, qt6-gtk-platform]",
                ):
                    self.assertIn(required, text)
                self.assertNotIn("DISABLE_WAYLAND", text)
                self.assertNotIn("QT_SELECT=qt5", text)

    def test_edge_packages_match_release(self):
        # Ignore comments, but compare every part's build and runtime packages.
        def packages(recipe):
            blocks = re.findall(
                r"^    (?:build|stage)-packages:\n((?:      .*\n)+)",
                recipe.read_text(),
                re.MULTILINE,
            )
            return [
                [
                    line.split("#", 1)[0].strip()
                    for line in block.splitlines()
                    if line.strip().startswith("- ")
                ]
                for block in blocks
            ]

        edge_packages = packages(RECIPES[0])
        self.assertTrue(edge_packages)
        self.assertEqual(edge_packages, packages(RECIPES[2]))


class TestSnapWrapper(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="qownnotes-snap-")
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.home = self.root / "real home"
        self.home.mkdir()
        self.snap = self.root / "snap"
        binary = self.snap / "usr/bin/QOwnNotes"
        binary.parent.mkdir(parents=True)
        binary.write_text(
            "#!/usr/bin/env python3\n"
            "import json, os, sys\n"
            "print(json.dumps({'env': dict(os.environ), 'args': sys.argv[1:]}))\n"
        )
        binary.chmod(0o755)
        self.environment = {
            "PATH": os.environ["PATH"],
            "HOME": str(self.root / "private/123"),
            "SNAP": str(self.snap),
            "SNAP_REAL_HOME": str(self.home),
            "SNAP_USER_DATA": str(self.root / "private/123"),
            "SNAP_USER_COMMON": str(self.root / "private/common"),
        }

    def launch(self, wrapper, overrides=None):
        environment = self.environment | (overrides or {})
        result = subprocess.run(
            ["sh", str(wrapper), "--test", "argument with spaces"],
            env=environment,
            capture_output=True,
            text=True,
            check=True,
        )
        return json.loads(result.stdout)

    def test_real_home_with_snap_local_storage(self):
        for wrapper in WRAPPERS:
            with self.subTest(wrapper=wrapper):
                result = self.launch(wrapper)
                environment = result["env"]
                self.assertEqual(environment["HOME"], str(self.home))
                self.assertEqual(
                    environment["XDG_CONFIG_HOME"],
                    self.environment["SNAP_USER_DATA"] + "/.config",
                )
                self.assertEqual(
                    environment["XDG_DATA_HOME"],
                    self.environment["SNAP_USER_DATA"] + "/.local/share",
                )
                self.assertEqual(
                    environment["XDG_CACHE_HOME"],
                    self.environment["SNAP_USER_COMMON"] + "/.cache",
                )
                self.assertEqual(environment["QT_QPA_PLATFORM"], "xcb")
                self.assertEqual(
                    result["args"], ["--snap", "--test", "argument with spaces"]
                )

    def test_preserve_desktop_helper_paths_and_platform(self):
        overrides = {
            "XDG_CONFIG_HOME": str(self.root / "custom config"),
            "XDG_DATA_HOME": str(self.root / "custom data"),
            "XDG_CACHE_HOME": str(self.root / "custom cache"),
            "QT_QPA_PLATFORM": "offscreen",
        }
        for wrapper in WRAPPERS:
            with self.subTest(wrapper=wrapper):
                environment = self.launch(wrapper, overrides)["env"]
                for key, value in overrides.items():
                    self.assertEqual(environment[key], value)
                self.assertEqual(environment["HOME"], str(self.home))

    def test_missing_real_home_keeps_private_home(self):
        for wrapper in WRAPPERS:
            for home in ("", str(self.root / "missing")):
                with self.subTest(wrapper=wrapper, home=home):
                    environment = self.launch(wrapper, {"SNAP_REAL_HOME": home})["env"]
                    self.assertEqual(environment["HOME"], self.environment["HOME"])

    def test_wrappers_stay_in_sync(self):
        for wrapper in WRAPPERS[1:]:
            with self.subTest(wrapper=wrapper):
                self.assertEqual(WRAPPERS[0].read_text(), wrapper.read_text())


if __name__ == "__main__":
    unittest.main()
