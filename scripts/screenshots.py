#!/usr/bin/env python3
"""Capture isolated QOwnNotes demo windows on the current KDE Plasma desktop."""

import argparse
from datetime import datetime
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time


ROOT = Path(__file__).resolve().parent.parent
PRESETS = ("minimal", "full", "preview-only", "full-vertical", "1col")


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
    config = base / "config"
    (config / "PBE").mkdir(parents=True)
    notes = base / "Demo notes"
    shutil.copytree(ROOT / "scripts/screenshot-notes", notes)
    # The most recently modified note is selected on a fresh profile.
    os.utime(notes / "A little room for ideas.md", None)

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
    settings = (
        "[General]\n"
        f"notesPath={notes}\n"
        "demoNotesCreated=true\n"
        "disableAutomaticUpdateDialog=true\n"
        "ShowSystemTray=false\n"
        "showStatusBarNotePath=false\n"
        "enableSocketServer=false\n"
        "systemIconTheme=true\n"
        "currentLayout=screenshot\n"
        "layouts=screenshot\n"
        f"centralWidget={layout['centralWidget']}\n"
        f"noteEditIsCentralWidget={layout['noteEditIsCentralWidget']}\n"
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
    with (base / "app.log").open("w+") as log:
        process = subprocess.Popen(
            [str(args.binary), "--session", session, "--allow-multiple-instances"],
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
            if ("QOwnNotes" not in title or
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
    parser.add_argument("--presets", nargs="+", choices=PRESETS,
                        default=["minimal", "full", "preview-only"])
    parser.add_argument("--width", type=int, default=1440)
    parser.add_argument("--height", type=int, default=960)
    parser.add_argument("--settle", type=float, default=4,
                        help="Seconds to wait for startup and note indexing (default: 4)")
    args = parser.parse_args()
    if args.width < 640 or args.height < 480 or args.settle < 0:
        parser.error("Use dimensions of at least 640x480 and a nonnegative settle time.")
    for command in ("kdotool", "spectacle"):
        if not shutil.which(command):
            parser.error(f"{command} is required; run this from your KDE Plasma desktop.")
    args.binary = args.binary.absolute()
    if not args.binary.is_file():
        parser.error("Application not found; run `just nix-build` or supply --binary.")
    args.output = args.output.absolute()
    targets = [args.output / f"qownnotes-{preset}.png" for preset in args.presets]
    if any(path.exists() for path in targets):
        parser.error("Output images already exist; choose a new --output directory.")
    args.output.mkdir(parents=True, exist_ok=True)
    active = run("kdotool", "getactivewindow")
    print("Capturing demo windows. Please avoid switching focus until finished.", flush=True)
    try:
        for preset, output in zip(args.presets, targets):
            temporary_root = "/tmp/opencode" if Path("/tmp/opencode").is_dir() else None
            with tempfile.TemporaryDirectory(prefix="qownnotes-screenshot-",
                                             dir=temporary_root) as directory:
                capture(args, preset, output, Path(directory))
    finally:
        if active:
            subprocess.run(["kdotool", "windowactivate", active], timeout=30, check=False)


if __name__ == "__main__":
    try:
        main()
    except (RuntimeError, OSError, subprocess.SubprocessError) as error:
        sys.exit(f"Screenshot failed: {error}")
