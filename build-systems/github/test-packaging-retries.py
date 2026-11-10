#!/usr/bin/env python3
"""Test CI packaging retry limits without macOS or access to the Snap Store."""

from pathlib import Path
import subprocess
import tempfile
import textwrap
import unittest

ROOT = Path(__file__).resolve().parents[2]
MOCK_COMMAND = '''
simulated_tool() {
  calls=0
  if [ -f calls ]; then calls=$(cat calls); fi
  calls=$((calls + 1))
  echo "$calls" > calls
  if [ "$calls" -le "$FAILURES" ]; then return 1; fi
  return 0
}
sleep() { :; }
'''


class PackagingRetryTests(unittest.TestCase):
    def check_attempts(self, shell, script):
        for failures, attempts, status in ((0, 1, 0), (2, 3, 0), (3, 3, 1)):
            with self.subTest(failures=failures), tempfile.TemporaryDirectory() as directory:
                result = subprocess.run(
                    [shell, '-c', f'FAILURES={failures}\n' + MOCK_COMMAND + script],
                    cwd=directory, capture_output=True, text=True)
                self.assertEqual(result.returncode, status, result.stdout + result.stderr)
                self.assertEqual((Path(directory) / 'calls').read_text().strip(), str(attempts))

    def test_dmg_creation(self):
        source = (ROOT / 'build-systems/github/macos/create-dmg-qt6.sh').read_text()
        start = source.index('attempt=1\n')
        end = source.index('\ndone', start) + len('\ndone')
        self.check_attempts('sh', 'APP=QOwnNotes\nTEMPDIR=QOwnNotes\n'
                            'hdiutil() { simulated_tool "$@"; }\n' + source[start:end])

    def test_snap_installation(self):
        source = (ROOT / '.github/actions/build-snap/action.yml').read_text()
        start = source.index('        retry_snap() {')
        end = source.index('\n        }', start) + len('\n        }')
        function = textwrap.dedent(source[start:end])
        self.check_attempts('bash', 'sudo() { simulated_tool "$@"; }\n' + function +
                            '\nretry_snap install --classic snapcraft')


if __name__ == '__main__':
    unittest.main()
