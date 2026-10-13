"""Test both Qt6 launch wrappers without building or installing a snap.

Run with: python3 -m unittest discover -s tests/snap -v
"""

import json
import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[2]
WRAPPERS = (
    ROOT / "build-systems/github/snap.qt6/snap/local/wrapper.sh",
    ROOT / "build-systems/snap/snapcraft/snap/local/wrapper.sh",
)


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
        self.assertEqual(WRAPPERS[0].read_text(), WRAPPERS[1].read_text())


if __name__ == "__main__":
    unittest.main()
