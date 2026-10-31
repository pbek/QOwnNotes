"""Standalone release safeguard tests: python3 -m unittest discover -s tests -p test_source_release_safeguard.py."""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest


SCRIPT = Path(__file__).resolve().parents[1] / "build-systems/github/check-source-release.sh"


class SourceReleaseSafeguardTest(unittest.TestCase):
    def check_release(self, version="26.11.0", draft="false", gh_status="0",
                      tag_commit="abc", source_commit="abc", fetch_status="0"):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "src").mkdir()
            (root / "src/version.h").write_text(f'#define VERSION "{version}"\n')
            (root / "bin").mkdir()
            mocks = {
                "gh": '#!/usr/bin/env bash\nprintf "%s\\n" "$DRAFT"\nexit "$GH_STATUS"\n',
                "git": '''#!/usr/bin/env bash
case "$1" in
  fetch) exit "$FETCH_STATUS" ;;
  rev-parse)
    if [[ $2 == HEAD ]]; then
      echo "$SOURCE_COMMIT"
    else
      echo "$TAG_COMMIT"
    fi ;;
  *) exit 99 ;;
esac
''',
            }
            for name, content in mocks.items():
                executable = root / "bin" / name
                executable.write_text(content)
                executable.chmod(0o755)
            env = dict(os.environ, PATH=f'{root / "bin"}:{os.environ["PATH"]}',
                       DRAFT=draft, GH_STATUS=gh_status, TAG_COMMIT=tag_commit,
                       SOURCE_COMMIT=source_commit, FETCH_STATUS=fetch_status)
            return subprocess.run(["bash", str(SCRIPT), "26.11.0"], cwd=root,
                                  env=env, capture_output=True, text=True)

    def test_matching_published_release(self):
        result = self.check_release()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("safeguard passed", result.stdout)

    def test_previous_remote_version(self):
        result = self.check_release(version="26.10.15")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("does not match", result.stderr)

    def test_missing_release_or_api_failure(self):
        result = self.check_release(gh_status="1")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("cannot verify", result.stderr)

    def test_draft_or_unexpected_api_response(self):
        for draft in ("true", "", "null"):
            with self.subTest(draft=draft):
                result = self.check_release(draft=draft)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("not a published release", result.stderr)

    def test_tag_points_to_different_commit(self):
        result = self.check_release(tag_commit="old")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("different commit", result.stderr)

    def test_tag_fetch_failure(self):
        self.assertNotEqual(self.check_release(fetch_status="1").returncode, 0)


if __name__ == "__main__":
    unittest.main()
