{ lib, pkgs, ... }:

let
  qownnotesLocal = pkgs.qt6Packages.callPackage ../../default.nix { };
in
{
  name = "qownnotes";
  meta.maintainers = [ lib.maintainers.pbek ];

  nodes.machine =
    { ... }:

    {
      imports = [
        ./common/user-account.nix
        ./common/x11.nix
      ];

      test-support.displayManager.auto.user = "alice";
      # Let QOwnNotes receive the layout shortcuts instead of IceWM's tiling actions.
      environment.etc."icewm/preferences".text = ''
        KeySysTileVertical=""
        KeySysTileHorizontal=""
      '';
      environment.systemPackages = [
        qownnotesLocal
        pkgs.xdotool
      ];
    };

  enableOCR = true;
  interactive.sshBackdoor.enable = true; # provides ssh-config & vsock access (needs host vsock support)

  # https://nixos.org/manual/nixos/stable/#ssec-machine-objects
  testScript =
    _:
    let
      aliceDo = cmd: ''machine.succeed("su - alice -c '${cmd}' >&2 &");'';
    in
    ''
      import configparser
      import io
      import shlex
      from datetime import timedelta

      class QtSettingsParser(configparser.ConfigParser):
          def optionxform(self, optionstr: str) -> str:
              return optionstr

      with subtest("Ensure X starts"):
          start_all()
          machine.wait_for_x()

      with subtest("Check QOwnNotes version on CLI"):
          ${aliceDo "qownnotes --version"}

          machine.wait_for_console_text("QOwnNotes ${qownnotesLocal.version}")

      with subtest("Ensure QOwnNotes starts"):
          # start QOwnNotes window
          ${aliceDo "qownnotes"}

          machine.wait_for_text("Welcome to QOwnNotes")
          machine.screenshot("QOwnNotes-Welcome")

      with subtest("Finish first-run wizard"):
          # The wizard should show up now
          machine.wait_for_text("Note folder")
          machine.send_key("ret")
          machine.wait_for_console_text("Note path '/home/alice/Notes' was now created.")
          machine.wait_for_text("Layout preset")
          machine.send_key("ret")
          machine.wait_for_text("Nextcloud")
          machine.send_key("ret")

          # OCR can't detect "App metric" anymore, so we will wait for another text
          machine.wait_for_text("Open network settings")
          machine.send_key("ret")

          # Doesn't work for non-root
          #machine.wait_for_window("QOwnNotes - ${pkgs.qownnotes.version}")

          # OCR doesn't seem to be able any more to handle the main window
          #machine.wait_for_text("QOwnNotes - ${pkgs.qownnotes.version}")

          # The main window should now show up
          machine.wait_for_open_port(22222)
          machine.wait_for_console_text("QOwnNotes server listening on port 22222")

          machine.screenshot("QOwnNotes-DemoNote")

      with subtest("Create a new note"):
          machine.send_key("ctrl-n")
          machine.sleep(1)
          machine.send_chars("This is a NixOS test!\n")
          machine.send_key("ctrl-s")
          machine.wait_until_succeeds("grep -Rqi 'This is a NixOS test!' /home/alice/Notes")

          # OCR doesn't seem to be able any more to handle the main window
          #machine.wait_for_text("This is a NixOS test!")

          # Doesn't work for non-root
          #machine.wait_for_window("- QOwnNotes - ${pkgs.qownnotes.version}")

          machine.screenshot("QOwnNotes-NewNote")

      with subtest("Switch layouts repeatedly with keyboard shortcuts"):
          machine.send_key("ctrl-q")
          machine.wait_until_fails("pgrep -u alice -f QOwnNotes")

          settings_path = "/home/alice/.config/PBE/QOwnNotes.conf"
          settings = QtSettingsParser(interpolation=None)
          settings.read_string(machine.succeed(f"cat {settings_path}"))
          layouts = {
              "shortcut-editor": "note-edit",
              "shortcut-viewer": "note-preview",
              "shortcut-panels": "none",
          }
          settings["General"]["layouts"] = ", ".join(layouts)
          settings["General"]["currentLayout"] = "shortcut-editor"
          if "Shortcuts" not in settings:
              settings.add_section("Shortcuts")
          for index, (uuid, central_widget) in enumerate(layouts.items(), 1):
              settings[f"layout-{uuid}"] = {"name": uuid, "centralWidget": central_widget}
              settings["Shortcuts"][f"MainWindow-restoreLayout-{uuid}"] = f"Alt+Shift+F{index}"
          output = io.StringIO()
          settings.write(output, space_around_delimiters=False)
          machine.succeed(f"printf %s {shlex.quote(output.getvalue())} > {settings_path}")

          ${aliceDo "qownnotes"}
          machine.wait_for_open_port(22222)
          machine.sleep(2)
          for _ in range(20):
              for key in ("alt-shift-f2", "alt-shift-f3", "alt-shift-f1"):
                  machine.send_key(key)
                  machine.sleep(timedelta(milliseconds=100))
              machine.succeed("ss -ltn | grep -q ':22222 '")

          machine.send_key("alt-shift-f2")
          machine.sleep(1)
          machine.send_key("ctrl-q")
          machine.wait_until_fails("pgrep -u alice -f QOwnNotes")
          settings.read_string(machine.succeed(f"cat {settings_path}"))
          assert settings["General"]["currentLayout"] == "shortcut-viewer", dict(settings["General"])
          assert settings["General"]["centralWidget"] == "note-preview", dict(settings["General"])
    '';
}
