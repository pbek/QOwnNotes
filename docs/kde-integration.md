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
applications. Sharing and KRunner integration are not implemented.

## Implemented: desktop notifications (phase 3)

- `NotificationService` is part of all builds. KDE builds send events with
  `KF6::Notifications` when a notification server (or, in Flatpak/Snap, the desktop
  portal) is registered or D-Bus activatable. Without one, and in plain builds,
  script notifications use the system tray and then the status bar.
- Events are defined in `src/kde/qownnotes.notifyrc`. The file is embedded as a Qt
  resource, so it also works for uninstalled builds, and is installed to
  `share/knotifications6` so the events appear in Plasma's notification settings
  (System Settings → Notifications → Applications → QOwnNotes). Events disabled
  there are not shown and don't fall back to the tray or status bar.
- Events:
  - `scriptNotification`: the new `script.showNotification(text, title)` scripting
    API. Clicking the notification shows the main window.
  - `externalNoteConflict` (KDE builds only): the current note was changed outside of
    QOwnNotes while QOwnNotes is in the background, and the diff dialog is waiting.
    The dialog is still shown; the notification doesn't contain the note name or text.
- Notification text is plain text (escaped), and identical notifications within
  10 seconds are only shown once. Clicking passes the notification's XDG activation
  token to Qt, so Wayland compositors can raise the window.
- Evaluated but not added: update notifications (the update dialog and the
  update-available button already cover this, and distribution packages usually
  disable the update checker) and connection errors (they result from user actions
  with their own dialogs; repeated background errors would be excessive).
- An isolated mock notification server tests delivery, escaping, actions, activation
  tokens, disabled events, duplicate suppression, and the fallback without a service.

## Implemented: PlasmaActivities library (phase 4)

- KDE builds use the PlasmaActivities library (`KActivities::Consumer`) behind the
  existing `PlasmaActivityService` API. It caches activities asynchronously, so the
  activity list and current activity don't need blocking D-Bus calls once loaded.
  The library is part of Plasma rather than KDE Frameworks.
- The library is guarded in `PlasmaActivitiesLibrary`:
  - It is only initialized after `kactivitymanagerd` asynchronously reported
    version 6.2.0 or later. The library aborts the application (`qFatal`) on older
    services, and it treats `6.2` as older than `6.2.0`; the check mirrors this.
  - It never starts the activity manager: initialization waits until the service
    is registered, and the library's autostart is disabled before it is created.
  - Until the library has loaded the activities (and on older services, or if the
    service stops), the existing direct D-Bus implementation is used. Plain builds
    always use it. Signals from both are not forwarded twice.
- Settings keys, note folder/layout links, and the startup/switching behavior are
  unchanged.
- An isolated mock activity manager tests version gating (including an unsupported
  5.27 service, which must not abort the tests), no autostart, activity
  names/renames/additions, current-activity changes, and service loss/restart.
- Known limitation: if a running service is replaced by an older one while QOwnNotes
  is running and already uses the library, the library itself would abort.

## Implemented: color schemes and window activation (phase 5)

### Color schemes

- KDE builds offer a color scheme selector in the interface settings (Settings →
  Interface → Interface style). It lists the installed KDE color schemes (`*.colors`
  files in `share/color-schemes`; a user scheme overrides a system scheme with the same
  file name) and is hidden if there are none. The default, "System default", leaves the
  application palette untouched.
- `KF6::ColorScheme` creates the palette (`KColorScheme::createApplicationPalette()`).
  `KColorSchemeManager` is not used, because it stores the selection in its own
  configuration file and automatically applies Breeze or Breeze Dark if the Plasma
  platform theme isn't used, which would change the appearance on other desktops.
- The scheme ID is stored in the new `kdeColorScheme` setting; existing settings are
  unchanged. A selected scheme that is no longer installed stays selected (shown as
  "not installed"), and the system palette is used meanwhile.
- Precedence:
  - The QDarkStyle dark mode theme of the color modes takes precedence, because its
    stylesheet overrides most palette colors. While it is turned on, the scheme isn't
    applied and the selector is disabled with a hint.
  - The editor keeps its colors from the editor color schema (which color modes can
    switch), and the accent color still applies, so contrast between the interface and
    the editor depends on choosing a matching schema, as with the system palette before.
- Like `KColorSchemeManager`, the `KDE_COLOR_SCHEME_PATH` application property is set.
  The Plasma platform theme then keeps the selected palette when the system color scheme
  changes and uses it for the window decoration. With "System default", the platform
  theme still applies system scheme changes live.
- Selecting a scheme applies it immediately; canceling the settings dialog restores the
  previous one. After palette changes, including system ones, palette-derived styling
  (e.g. the note tag frame background) is refreshed.
- Tests cover scheme discovery, overrides and sorting, applying and switching schemes,
  restoring the system palette and a platform-provided scheme property, and the dark mode
  theme precedence.

### Window activation

Wayland compositors only activate a window with an XDG activation token of a recent user
interaction; calling an activation function alone doesn't bypass focus stealing
prevention. Qt 6 already uses a token from the `XDG_ACTIVATION_TOKEN` environment
variable when a window is activated (`KWindowSystem::setCurrentXdgActivationToken()`
does the same), so token propagation is part of all builds (`WindowActivationService`):

- Another instance (single-instance mode) captures `XDG_ACTIVATION_TOKEN` and the X11
  `DESKTOP_STARTUP_ID` of its launch before the application object is created, because
  Qt removes the latter, and sends them with the startup action to the primary instance.
  The primary instance activates its window with them.
  - The message consists of `key:value` lines; values can't contain line breaks. A launch
    with only an action still sends the previous `startupAction:<name>` message.
  - Second instances of older versions send no message without an action, so the window
    is activated without a token if no message follows within 500 ms. An older primary
    instance ignores a new message that contains a token, including its action.
- Global shortcuts of the XDG desktop portal pass the `activation_token` of newer portal
  versions. Desktop notification clicks already pass theirs (phase 3).
- Unused tokens are removed after the activation, so they aren't used by a later
  unrelated activation or inherited by started processes. Tokens are only set on Wayland.
- KDE builds use `KF6::WindowSystem` on X11: the startup ID of another instance is passed
  to the window manager with `KStartupInfo::setNewStartupId()`, like KDBusService does,
  which also completes the launch feedback.
- Not possible: Plasma's KGlobalAccel doesn't provide activation tokens on Wayland, so
  global shortcuts of the KDE backend that show an already mapped window depend on KWin's
  focus stealing prevention (it may only demand attention). On X11, KGlobalAccel updates
  the user timestamp itself.
- `SingleApplication` is kept; replacing it with KDBusService is not planned.
- Tests cover the message format (including older versions and injected lines), capturing
  the environment, removing unused tokens, and restoring minimized windows. Activation
  itself can only be checked on a real desktop.

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
ECM 6 or later, KColorScheme/KCoreAddons/KCrash/KGlobalAccel/KNotifications/KWindowSystem 6 or later,
PlasmaActivities 6 or later, and the usual application dependencies:

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

Run isolated KDE shortcut, notification, activities, color scheme, and window activation regression tests with:

```sh
nix build '.?submodules=1#checks.x86_64-linux.qownnotes-kde-tests'
# Or build only the standalone test project:
cmake -S tests/kde -B build-tests-kde
cmake --build build-tests-kde --parallel
ctest --test-dir build-tests-kde --output-on-failure
```

The normal test project can include these targets with
`-DQON_KDE_INTEGRATION_TESTS=ON`. The tests use a private D-Bus session and
temporary configuration directories, so they never alter the user's desktop
shortcuts, notification settings, or activities. Real Plasma/Wayland key delivery, notification
presentation and window activation, sandboxed (portal) notifications, and crash
reporter presentation still require desktop integration testing; tests do not
deliberately crash an application.

Main risks are duplicate shortcut backends, conflicting settings authorities,
mixed Qt packages/plugins, new framework minimum versions, and maintaining both
code paths. Optional dependencies must remain truly optional when the flag is off.
