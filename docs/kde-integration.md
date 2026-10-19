# Optional KDE integration

- Tracking issue: [#3796](https://github.com/pbek/QOwnNotes/issues/3796)
- Initial release: 26.10.9

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

## Implemented: crash reporting and Plasma shortcuts (phases 1–2)

- GUI startup initializes `KF6::Crash` after metadata registration and after CLI
  commands or secondary instances have exited. DrKonqi support depends on the
  installed reporter and system core-dump configuration. There is no automatic
  restart or emergency-save callback, and no report is submitted automatically.
- The KDE build uses `KF6::GlobalAccel` when `org.kde.kglobalaccel` is already
  available on the session bus. It does not launch the service on other desktops.
- All actions offered in QOwnNotes' shortcut settings are registered in Plasma's
  shortcut settings (System Settings → Keyboard → Shortcuts → QOwnNotes), including
  actions without a global shortcut, so they can be assigned in either place.
  Plasma shows each action with its menu path (for example `Note → New note`),
  because action names alone are often ambiguous. Plasma itself appends the `:`
  after each name.
  Saved Plasma bindings take precedence when an action is registered. Changes made
  in Plasma are reflected in QOwnNotes settings. Explicit edits in QOwnNotes during
  the current run update Plasma instead. Unassigned actions don't add settings entries.
- QOwnNotes shows the first binding when Plasma assigns multiple alternatives;
  unchanged reconfiguration retains the additional bindings. Editing that first
  binding in QOwnNotes replaces the alternatives with the newly selected binding.
- Proxy actions keep stable identities and resolve the current menu action on
  activation, so replaced or disabled actions are handled safely. Shutdown and
  actions that disappear (for example while scripts reload) only deactivate the
  registration and keep the saved Plasma binding. Clearing a shortcut in QOwnNotes
  clears it in Plasma.
- Service arrival/loss switches between KDE and the existing XDG portal/QHotkey
  fallbacks. Portal sessions are closed before registering KDE bindings to prevent
  duplicate activation. Conflicting bindings are not stolen from other applications.
- An isolated mock D-Bus service tests restoration, application/Plasma edits,
  alternative bindings, activation, conflicts, removal, and service lifecycle.

The editor and widget layout remain unchanged. Plasma's Qt platform theme already
provides much of the appearance and native-dialog integration to ordinary Qt
applications. Notifications, sharing, and KRunner integration are not implemented.

For DrKonqi, install the reporter separately. KCrash searches executable directories
including those in `LIBEXEC_PATH`; on NixOS an installed reporter can be made
discoverable with `LIBEXEC_PATH=/run/current-system/sw/libexec`. Useful stack traces
also require debug symbols and a supported debugger/core-dump setup. Setting
`KDE_DEBUG=1` disables KCrash's normal reporter enablement for debugging.

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
ECM 6 or later, KCoreAddons/KCrash/KGlobalAccel 6 or later, and the usual application
dependencies:

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

Run isolated KDE shortcut regression tests with:

```sh
nix build '.?submodules=1#checks.x86_64-linux.qownnotes-kde-shortcuts'
# Or build only the standalone test project:
cmake -S tests/kde -B build-tests-kde
cmake --build build-tests-kde --parallel
ctest --test-dir build-tests-kde --output-on-failure
```

The normal test project can include this target with
`-DQON_KDE_INTEGRATION_TESTS=ON`. The tests use a private D-Bus session and never
alter the user's desktop shortcuts. Real Plasma/Wayland key delivery and crash
reporter presentation still require desktop integration testing; tests do not
deliberately crash an application.

Main risks are duplicate shortcut backends, conflicting settings authorities,
mixed Qt packages/plugins, new framework minimum versions, and maintaining both
code paths. Optional dependencies must remain truly optional when the flag is off.
