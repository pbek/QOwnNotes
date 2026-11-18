"""Standalone release safeguard tests: python3 -m unittest discover -s tests -p test_source_release_safeguard.py."""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest


SCRIPT = Path(__file__).resolve().parents[1] / "build-systems/github/check-source-release.sh"


class SourceReleaseSafeguardTest(unittest.TestCase):
    def check_release(self, version="26.11.0", draft="false", gh_status="0",
                      tag_commit="abc", source_commit="abc", fetch_status="0",
                      publish_after=None, clock_step="300"):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            (root / "src").mkdir()
            (root / "src/version.h").write_text(f'#define VERSION "{version}"\n')
            (root / "bin").mkdir()
            mocks = {
                "gh": '''#!/usr/bin/env bash
Calls=$(cat gh-calls 2>/dev/null || echo 0)
echo "$((Calls + 1))" >gh-calls
if [[ -n $PUBLISH_AFTER ]] && (( Calls >= PUBLISH_AFTER )); then
  echo false
  exit 0
fi
printf "%s\\n" "$DRAFT"
exit "$GH_STATUS"
''',
                "date": '''#!/usr/bin/env bash
Now=$(cat clock 2>/dev/null || echo 0)
echo "$Now"
echo "$((Now + CLOCK_STEP))" >clock
''',
                "sleep": '#!/usr/bin/env bash\necho "$1" >>sleep-calls\n',
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
                        SOURCE_COMMIT=source_commit, FETCH_STATUS=fetch_status,
                        PUBLISH_AFTER="" if publish_after is None else str(publish_after),
                        CLOCK_STEP=clock_step)
            result = subprocess.run(["bash", str(SCRIPT), "26.11.0"], cwd=root,
                                    env=env, capture_output=True, text=True, timeout=10)
            result.gh_calls = int((root / "gh-calls").read_text()) if (root / "gh-calls").exists() else 0
            result.sleeps = (root / "sleep-calls").read_text().splitlines() if (root / "sleep-calls").exists() else []
            return result

    def test_matching_published_release(self):
        result = self.check_release()
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("safeguard passed", result.stdout)
        self.assertEqual(result.gh_calls, 1)
        self.assertEqual(result.sleeps, [])

    def test_previous_remote_version(self):
        result = self.check_release(version="26.10.15")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("does not match", result.stderr)
        self.assertEqual(result.gh_calls, 0)
        self.assertEqual(result.sleeps, [])

    def test_missing_release_or_api_failure(self):
        result = self.check_release(gh_status="1")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("cannot verify", result.stderr)
        self.assertIn("after 5 minutes", result.stderr)

    def test_draft_or_unexpected_api_response(self):
        for draft in ("true", "", "null"):
            with self.subTest(draft=draft):
                result = self.check_release(draft=draft)
                self.assertNotEqual(result.returncode, 0)
                self.assertIn("not a published release", result.stderr)
                self.assertIn("after 5 minutes", result.stderr)

    def test_release_published_after_retry(self):
        for draft, gh_status in (("true", "0"), ("", "1"), ("null", "0")):
            with self.subTest(draft=draft, gh_status=gh_status):
                result = self.check_release(draft=draft, gh_status=gh_status,
                                            publish_after=2, clock_step="5")
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(result.gh_calls, 3)
                self.assertEqual(result.sleeps, ["5", "5"])
                self.assertIn("safeguard passed", result.stdout)

    def test_retry_timeout(self):
        result = self.check_release(gh_status="1", clock_step="100")
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual(result.gh_calls, 3)
        self.assertEqual(result.sleeps, ["5", "5"])
        self.assertIn("after 5 minutes", result.stderr)

    def test_tag_points_to_different_commit(self):
        result = self.check_release(tag_commit="old")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("different commit", result.stderr)
        self.assertEqual(result.sleeps, [])

    def test_commit_mismatch_after_release_published(self):
        result = self.check_release(draft="true", publish_after=1,
                                    clock_step="5", tag_commit="old")
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("different commit", result.stderr)

    def test_tag_fetch_failure(self):
        self.assertNotEqual(self.check_release(fetch_status="1").returncode, 0)


if __name__ == "__main__":
    unittest.main()
