# Optional KDE integration

- Tracking issue: [#3796](https://github.com/pbek/QOwnNotes/issues/3796)
- Initial release: 26.10.9 (phase 0 only)

## Goal and scope

Keep one cross-platform Qt Widgets application, with optional KDE Frameworks 6
integration selected by `QON_KDE_INTEGRATION`. The CMake option defaults to `OFF`.
It requires `QON_QT6_BUILD=ON` and currently supports Linux only. On other platforms,
CMake warns and disables the integration. Missing required KDE dependencies fail
configuration rather than silently producing a plain Qt build.

Qt5, qmake, Windows, and macOS builds retain their existing implementation. No
editor, docking UI, settings format, credential storage, or translation migration
is planned. KDE integration does not require running Plasma, and desktop-specific
features must check service availability before use.

## Implemented: phase 0

- CMake option, Extra CMake Modules (ECM), and `KF6::CoreAddons` dependency.
- Registration of application metadata with `KAboutData`, preserving the existing
  application name, organization domain, detailed version, and desktop file name.
  This preserves settings paths, session/debug identities, and single-instance
  behavior. Bug reports continue to point to GitHub rather than KDE Bugzilla.
- KDE build indication in the About dialog and generated debug information.
- A Linux-only `qownnotes-qt6-kde` flake package and a KDE-capable development shell.
- A Linux CI Debug build with `DEV_MODE=ON` and headless CLI smoke checks; existing
  plain Qt6 and Qt5 jobs remain.
  The developer check uses Hunspell without optional Aspell because the bundled
  Aspell backend has an existing GCC `maybe-uninitialized` warning. Normal release
  packages still include Aspell.

**This first phase does not change the editor or add shortcuts, notifications,
crash reporting, or other desktop features.** Linking KDE libraries alone does not
change widget styling. Plasma's Qt platform theme already provides much of the
system appearance and native-dialog integration to ordinary Qt applications.

## Building

The default development recipe selects the KDE package on Linux and the plain Qt6
package on macOS:

```sh
just build
# Equivalent:
just nix-build
```

`nix-build-force`, `nix-build-trace`, and `nix-build-system` use the same selection.
`nix-build-system` uses the system nixpkgs input to align Qt with desktop plugins.
The flake's `default` package remains plain Qt6.

Explicit package selection:

```sh
nix build '.?submodules=1#qownnotes-qt6-kde'
just nix-build-qt6
just nix-build-qt5
```

Manual CMake development in the flake shell:

```sh
nix develop '.?submodules=1'
just src-build-kde
```

This uses a separate `build-QOwnNotes-kde` directory. Outside Nix, install Qt6,
ECM 6 or later, KCoreAddons 6 or later, and the usual application dependencies:

```sh
cmake -S src -B build-QOwnNotes-kde \
  -DQON_QT6_BUILD=ON -DQON_KDE_INTEGRATION=ON \
  -DDEV_MODE=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-QOwnNotes-kde --parallel
```

The actual minimum Qt version is also constrained by the installed KF6 release.
Nix obtains KDE and Qt libraries from the same package scope to avoid mixed-Qt
dependencies.

## Remaining roadmap (not implemented)

Each phase should be independently reviewable, preserve the plain Qt build, and
add only the dependencies it uses. Keep substantial KDE-specific logic in small
services rather than scattering conditionals through `mainwindow.cpp`.

### 1. Crash reporting

- Add `KF6::Crash` and initialize it after application metadata registration for
  GUI starts, not headless CLI commands or secondary instances.
- Use DrKonqi when installed; document debug-symbol and reporter requirements.
- Test with and without the reporter. Do not deliberately crash a user's running
  application during validation.

### 2. Plasma global shortcuts

- Add `KF6::GlobalAccel`, selecting it only when the Plasma service is available.
- Retain QHotkey on X11/Windows/macOS and the existing XDG GlobalShortcuts portal
  on Wayland as fallbacks. Wayland shortcut support already exists today.
- Use stable action identifiers, resolve replaced menu actions on activation, and
  clean up registrations on reconfiguration. Never register both backends.
- Define how Plasma-side changes synchronize with the existing `GlobalShortcuts`
  settings before implementation. Do not overwrite user changes on every start.
- Verify conflicts, disabled shortcuts, session identities, service restarts, and
  behavior on a non-Plasma desktop.

### 3. Desktop notifications

- Add `KF6::Notifications`, an installed notification event configuration, and a
  notification service with a Qt system-tray fallback.
- Evaluate events for external edits/conflicts, connection errors, and updates;
  do not replace blocking errors or important status messages with notifications.
- Add and document a scripting notification API, with consistent fallback behavior.
- Configure event visibility and avoid duplicate or excessive notifications.
- Test notification actions, missing services/tray, and sandbox permissions.

### 4. Plasma Activities library

- Optionally use PlasmaActivities (`KActivities::Consumer`) behind the existing
  `PlasmaActivityService` API instead of its direct D-Bus implementation.
- Preserve settings keys, layout/folder associations, and behavior in plain Qt
  builds. Handle unavailable services and late startup without blocking the UI.
- This is an implementation refinement: Activities integration already exists.

### 5. Color schemes and window activation

- Evaluate `KF6::ColorScheme` for an in-app system color scheme selector.
- Define precedence against QDarkStyle and custom editor colors; retain existing
  preferences and verify live scheme changes and contrast.
- Evaluate `KF6::WindowSystem` and activation-token propagation for shortcut and
  secondary-instance activation. Wayland requires appropriate tokens; merely
  calling an activation function does not bypass compositor focus policy.
- Keep `SingleApplication`; replacing it with KDBusService is not planned.

### 6. Sharing

- Evaluate Purpose for sharing selected text, notes, or attachments to installed
  targets such as KDE Connect or email.
- Hide unavailable targets and keep existing Nextcloud/ownCloud sharing intact.
- Avoid disclosing encrypted/private note content without explicit user action.

### 7. KRunner search

- Implement a separate, optional D-Bus runner registration, with stable identifiers
  and note-opening actions. Evaluate whether this can be provided by plain Qt
  D-Bus without requiring the KDE build flag.
- Reuse existing search, cap result sizes, and keep matching asynchronous.
- Define behavior for multiple sessions, absent application instances, and locked
  or encrypted notes. Do not expose confidential note text to desktop search.

## Packaging and validation

| Channel                       | Policy                                                              |
| ----------------------------- | ------------------------------------------------------------------- |
| Local Nix flake               | KDE variant on Linux; plain variants remain available               |
| Flatpak                       | Evaluate a current KDE runtime and portal/D-Bus permissions         |
| AUR, Gentoo, upstream nixpkgs | Evaluate optional features or separate variants                     |
| OBS                           | Audit live KF6 availability per distribution and architecture first |
| AppImage, Snap                | Keep plain builds initially; audit bundling before opting in        |
| Windows, macOS, Qt5, qmake    | Keep plain builds; optional Craft evaluation later                  |

Do not enable KDE dependencies across package channels automatically. OBS scheduler
results and downstream packaging cannot be established from local manifests alone.
Remaining Qt5 support and older Qt6 deployment targets must not be raised by this
optional feature (see `qt5-retirement-recommendation.md`).

Acceptance and regression checks:

- Build KDE and plain Qt6 with developer warnings; preserve Qt5/qmake CI coverage.
- Reject a KDE-enabled Qt5 configuration with a clear error.
- Verify Linux flake attributes and Darwin's absence of a KDE package.
- Smoke-test headless help/completions and GUI launch on Plasma and non-Plasma.
- Verify unchanged settings paths, session names, desktop IDs, and instance handling.
- Test About/debug build markers in both configurations.
- Add feature-specific tests and sandbox/desktop checks as later phases land.

Main risks are duplicate shortcut backends, conflicting settings authorities,
mixed Qt packages/plugins, new framework minimum versions, and maintaining both
code paths. Optional dependencies must remain truly optional when the flag is off.
