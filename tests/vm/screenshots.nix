{
  lib,
  pkgs,
  qownnotes,
  captureRun ? "",
  ...
}:

let
  # Keep the fixture independent of the full repository and host configuration.
  fixtures = pkgs.runCommand "qownnotes-screenshot-fixtures" { } ''
    mkdir -p "$out/scripts" "$out/src/configurations" "$out/webpage/src/.vuepress/components"
    cp ${../../scripts/screenshots.py} "$out/scripts/screenshots.py"
    cp ${../../scripts/screenshot_server.py} "$out/scripts/screenshot_server.py"
    cp -r ${../../scripts/screenshot-notes} "$out/scripts/screenshot-notes"
    cp -r ${../../scripts/screenshot-notes-ru} "$out/scripts/screenshot-notes-ru"
    cp -r ${../../scripts/screenshot-scripts} "$out/scripts/screenshot-scripts"
    cp ${../../src/configurations/layouts.ini} "$out/src/configurations/layouts.ini"
    cp ${../../src/version.h} "$out/src/version.h"
    cp ${../../webpage/src/.vuepress/components/ScreenshotCarousel.vue} \
      "$out/webpage/src/.vuepress/components/ScreenshotCarousel.vue"
  '';
  kdeglobals = pkgs.writeText "screenshot-kdeglobals" ''
    [General]
    ColorScheme=BreezeLight
    font=Noto Sans,10,-1,5,50,0,0,0,0,0
    [Icons]
    Theme=breeze
    [KDE]
    widgetStyle=Breeze
  '';
  screenlocker = pkgs.writeText "screenshot-kscreenlockerrc" ''
    [Daemon]
    Autolock=false
    LockOnResume=false
  '';
  baloo = pkgs.writeText "screenshot-baloofilerc" ''
    [Basic Settings]
    Indexing-Enabled=false
  '';
in
{
  name = "qownnotes-screenshots";
  meta.maintainers = [ lib.maintainers.pbek ];

  nodes.machine = {
    users.users.alice = {
      isNormalUser = true;
      uid = 1000;
      password = "";
    };
    services.desktopManager.plasma6 = {
      enable = true;
      enableQt5Integration = false;
    };
    services.displayManager = {
      sddm = {
        enable = true;
        wayland.enable = true;
      };
      autoLogin = {
        enable = true;
        user = "alice";
      };
      defaultSession = "plasma";
    };
    # Skip optional desktop applications, not the theme, compositor or capture tools.
    environment = {
      plasma6.excludePackages = with pkgs.kdePackages; [
        ark
        elisa
        dolphin
        dolphin-plugins
        gwenview
        kate
        okular
        krdp
        plasma-browser-integration
        plasma-workspace-wallpapers
      ];
      systemPackages = [
        qownnotes
        pkgs.python3
        pkgs.kdotool
        pkgs.kdePackages.spectacle
      ];
      sessionVariables = {
        # No host GPU passthrough or accelerated display server is needed.
        KWIN_COMPOSE = "O2";
        # Virtio's software scanout works reliably with legacy rather than atomic modesetting.
        KWIN_DRM_NO_AMS = "1";
        GALLIUM_DRIVER = "llvmpipe";
        LIBGL_ALWAYS_SOFTWARE = "1";
      };
    };
    systemd.tmpfiles.rules = [
      "d /home/alice/.config 0755 alice users -"
      "C /home/alice/.config/kdeglobals 0644 alice users - ${kdeglobals}"
      "C /home/alice/.config/kscreenlockerrc 0644 alice users - ${screenlocker}"
      "C /home/alice/.config/baloofilerc 0644 alice users - ${baloo}"
    ];
    virtualisation = {
      memorySize = 4096;
      cores = 4;
      restrictNetwork = true;
      qemu.options = [ "-vga none -device virtio-gpu-pci" ];
    };
    boot.kernelParams = [ "video=Virtual-1:1920x1200@60" ];
  };

  testScript = ''
    import json
    import shlex
    import struct
    from datetime import timedelta
    from pathlib import Path

    # A nonce changes only the driver, not the VM closure or application build.
    capture_run = ${builtins.toJSON captureRun}
    desktop_env = "env XDG_RUNTIME_DIR=/run/user/1000 DBUS_SESSION_BUS_ADDRESS=unix:path=/run/user/1000/bus"
    def alice(command):
        return "su - alice -c " + shlex.quote(desktop_env + " " + command)

    start_all()
    machine.wait_for_unit("display-manager.service")
    machine.wait_for_file("/run/user/1000/bus")
    machine.wait_until_succeeds("pgrep -u alice kwin_wayland", timeout=timedelta(seconds=180))
    machine.wait_until_succeeds("pgrep -u alice plasmashell", timeout=timedelta(seconds=180))
    machine.wait_until_succeeds(alice("systemctl --user show-environment | grep -q '^WAYLAND_DISPLAY='"))
    session = dict(line.split("=", 1) for line in
                   machine.succeed(alice("systemctl --user show-environment")).splitlines()
                   if "=" in line)
    desktop_env += " WAYLAND_DISPLAY=" + shlex.quote(session["WAYLAND_DISPLAY"]) + " QT_QPA_PLATFORM=wayland"
    machine.wait_until_succeeds(alice("kdotool search --class plasmashell"))

    # The virtio display provides this mode without host graphics or display scaling.
    machine.succeed(alice("kscreen-doctor output.1.mode.1920x1200 output.1.scale.1"))
    machine.succeed(alice("kscreen-doctor -o"))

    command = "${pkgs.python3}/bin/python3 ${fixtures}/scripts/screenshots.py --binary ${qownnotes}/bin/QOwnNotes --overview --output /home/alice/screenshots --width 1440 --height 960 --settle 6"
    try:
        machine.succeed(alice(command) + " > /home/alice/capture.log 2>&1", timeout=timedelta(seconds=900))
    except Exception:
        machine.screenshot("capture-failure")
        # Retain completed scenes in the kept build directory when a later capture fails.
        machine.copy_from_machine("/home/alice/screenshots")
        print(machine.succeed("journalctl -b _UID=1000 --no-pager | tail -n 100"))
        raise
    finally:
        print(machine.succeed("cat /home/alice/capture.log"))
        machine.copy_from_machine("/home/alice/capture.log")

    machine.succeed("test -s /home/alice/screenshots/overview-coverage.json")
    machine.copy_from_machine("/home/alice/screenshots")
    output = Path(driver.out_dir) / "screenshots"
    images = list(output.glob("*.png"))
    coverage = json.loads((output / "overview-coverage.json").read_text())
    expected = {name for name, status in coverage.items() if status == "automated"}
    assert {image.name for image in images} == expected, "Overview images do not match coverage"
    for image in images:
        data = image.read_bytes()
        assert data.startswith(b"\x89PNG\r\n\x1a\n"), image
        width, height = struct.unpack(">II", data[16:24])
        assert width >= 320 and height >= 240, f"Unexpectedly small screenshot: {image}"
    (output / "vm-info.json").write_text(json.dumps({
        "desktop": "Plasma 6 Wayland", "theme": "Breeze Light", "font": "Noto Sans 10",
        "screen": "1920x1200", "scale": 1, "capture_run": capture_run,
        "application": "${qownnotes.version}", "memory_mib": 4096, "cpus": 4
    }, indent=2) + "\n")
  '';
}
