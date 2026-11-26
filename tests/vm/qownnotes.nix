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

      with subtest("Watch external note changes without watching Git internals"):
          machine.send_key("ctrl-q")
          machine.wait_until_fails("pgrep -u alice -f QOwnNotes")
          git_paths = [
              "/home/alice/Notes/.git/objects",
              "/home/alice/Notes/.git/objects/0f",
              "/home/alice/Notes/.git/objects/0f/test-object",
              "/home/alice/Notes/.git/refs",
              "/home/alice/Notes/.git/refs/heads",
              "/home/alice/Notes/.git/refs/heads/main",
          ]
          machine.succeed("mkdir -p /home/alice/Notes/.git/objects/0f /home/alice/Notes/.git/refs/heads")
          machine.succeed("touch /home/alice/Notes/.git/objects/0f/test-object /home/alice/Notes/.git/refs/heads/main")
          machine.succeed("chown -R alice:users /home/alice/Notes/.git")

          def watch_check(path):
              inode = int(machine.succeed(f"stat -c %i {shlex.quote(path)}").strip())
              return (
                  f"grep -h -q 'inotify .* ino:{inode:x} ' "
                  f"/proc/{watcher_pid}/fdinfo/*"
              )

          ${aliceDo "qownnotes"}
          machine.wait_for_open_port(22222)
          # Nix's Qt wrapper changes the process name; match the executable command line.
          watcher_pid = machine.succeed("pgrep -u alice -f QOwnNotes").strip()
          assert watcher_pid.isdigit(), watcher_pid
          machine.wait_until_succeeds(watch_check("/home/alice/Notes"), timeout=timedelta(seconds=60))
          for path in git_paths:
              machine.fail(watch_check(path))

          # A new note must still trigger re-indexing and get its own file watch.
          external_note = "/home/alice/Notes/externally-created.md"
          machine.succeed(f"echo External-note-watcher-test > {external_note}")
          machine.wait_until_succeeds(watch_check(external_note), timeout=timedelta(seconds=60))
          for path in git_paths:
              machine.fail(watch_check(path))

      with subtest("Switch layouts repeatedly with keyboard shortcuts"):
          machine.send_key("ctrl-q")
          machine.wait_until_fails("pgrep -u alice -f QOwnNotes")

          settings_path = "/home/alice/.config/PBE/QOwnNotes.conf"
          def wait_for_layout(uuid, central_widget):
              # Wait for the queued restore and QSettings writes, not a fixed GUI delay.
              general_settings = f"sed -n '/^\\[General\\]$/,/^\\[/p' {settings_path}"
              machine.wait_until_succeeds(
                  f"{general_settings} | grep -qx 'currentLayout={uuid}' && "
                  f"{general_settings} | grep -qx 'centralWidget={central_widget}'"
              )

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
              for key, uuid in (("alt-shift-f2", "shortcut-viewer"),
                                ("alt-shift-f3", "shortcut-panels"),
                                ("alt-shift-f1", "shortcut-editor")):
                  machine.send_key(key)
                  machine.sleep(timedelta(milliseconds=100))
                  wait_for_layout(uuid, layouts[uuid])
              machine.succeed("ss -ltn | grep -q ':22222 '")

          machine.send_key("alt-shift-f2")
          wait_for_layout("shortcut-viewer", "note-preview")
          machine.succeed("su - alice -c 'xdotool search --onlyvisible --class QOwnNotes windowactivate --sync key --clearmodifiers ctrl+q'")
          machine.wait_until_fails("pgrep -u alice -f QOwnNotes")
          settings.read_string(machine.succeed(f"cat {settings_path}"))
          assert settings["General"]["currentLayout"] == "shortcut-viewer", dict(settings["General"])
          assert settings["General"]["centralWidget"] == "note-preview", dict(settings["General"])
          for uuid, central_widget in layouts.items():
              assert settings[f"layout-{uuid}"]["centralWidget"] == central_widget, {
                  layout: settings[f"layout-{layout}"].get("centralWidget") for layout in layouts
              }

      with subtest("Switch layouts with global shortcuts after startup rebuilds the layout menu"):
          # Global shortcuts are initialized before the layout actions are replaced at startup.
          # Remove the local assignments so only QHotkey can activate these shortcuts.
          if "GlobalShortcuts" not in settings:
              settings.add_section("GlobalShortcuts")
          for index, uuid in enumerate(layouts, 1):
              action_key = f"MainWindow-restoreLayout-{uuid}"
              settings["Shortcuts"].pop(action_key, None)
              settings["GlobalShortcuts"][action_key] = f"Alt+Shift+F{index}"
          output = io.StringIO()
          settings.write(output, space_around_delimiters=False)
          machine.succeed(f"printf %s {shlex.quote(output.getvalue())} > {settings_path}")

          ${aliceDo "qownnotes"}
          machine.wait_for_open_port(22222)
          machine.sleep(2)
          # Exercise a burst of activations without waiting between requests.
          for key in ("alt-shift-f1", "alt-shift-f3", "alt-shift-f2"):
              machine.send_key(key)
          machine.sleep(1)
          for _ in range(20):
              for key, uuid in (("alt-shift-f1", "shortcut-editor"),
                                ("alt-shift-f3", "shortcut-panels"),
                                ("alt-shift-f2", "shortcut-viewer")):
                  machine.send_key(key)
                  machine.sleep(timedelta(milliseconds=100))
                  wait_for_layout(uuid, layouts[uuid])
              machine.succeed("ss -ltn | grep -q ':22222 '")

          machine.send_key("alt-shift-f1")
          wait_for_layout("shortcut-editor", "note-edit")
          machine.succeed("su - alice -c 'xdotool search --onlyvisible --class QOwnNotes windowactivate --sync key --clearmodifiers ctrl+q'")
          machine.wait_until_fails("pgrep -u alice -f QOwnNotes", timeout=timedelta(seconds=30))
          settings.read_string(machine.succeed(f"cat {settings_path}"))
          for uuid, central_widget in layouts.items():
              assert settings[f"layout-{uuid}"]["centralWidget"] == central_widget, {
                  layout: settings[f"layout-{layout}"].get("centralWidget") for layout in layouts
              }
          assert settings["General"]["currentLayout"] == "shortcut-editor", dict(settings["General"])
          assert settings["General"]["centralWidget"] == "note-edit", dict(settings["General"])

      with subtest("Reload consecutive atomic replacements without overwriting them on autosave"):
          profile = "/home/alice/atomic-replacement"
          note = f"{profile}/notes/Replacement.md"
          settings_text = f"""[General]
      notesPath={profile}/notes
      guiFirstRunInit=true
      acceptAllExternalModifications=true
      enableNoteChecksumChecks=true
      noteSaveIntervalTime=5
      [appMetrics]
      notificationShown=true
      disableTracking=true
      [security]
      keychainMigrationNoticeAcknowledged=true
      """
          machine.succeed(f"mkdir -p {profile}/config/PBE {profile}/notes")
          machine.succeed(f"printf %s {shlex.quote(settings_text)} > {profile}/config/PBE/QOwnNotes.conf")
          machine.succeed(f"printf '%s\\n' '# Original note content' > {note}")
          machine.succeed(f"chown -R alice:users {profile}")
          ${aliceDo "XDG_CONFIG_HOME=/home/alice/atomic-replacement/config XDG_DATA_HOME=/home/alice/atomic-replacement/data qownnotes"}
          machine.wait_for_text("Original note content")

          for text in ("First replacement from disk", "Second replacement from disk"):
              temp_note = f"{profile}/notes/.Replacement.md.~download"
              machine.succeed(f"printf '%s\\n' {shlex.quote('# ' + text)} > {temp_note}")
              machine.succeed(f"mv -f {temp_note} {note}")
              machine.wait_for_text(text)
              # The editor must show this version and the next autosave must not undo it.
              machine.sleep(6)
              assert machine.succeed(f"cat {note}").strip() == f"# {text}"
    '';
}
