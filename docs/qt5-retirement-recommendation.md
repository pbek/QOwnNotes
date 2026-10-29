# Qt 5 Retirement Recommendation

- Status: Accepted
- Reviewed: 2026-10-29
- Tracking issue: [#3752](https://github.com/pbek/QOwnNotes/issues/3752)

## Recommendation

Stop publishing Qt 5 through the Bitwarden-driven release process. Retain the Qt
5 artifacts built directly by the GitHub release workflow for now, together with
Qt 5 source compatibility and CI coverage.

Do not create a long-lived Qt 5 maintenance branch unless a maintainer commits
to supporting it. Keep the final external Qt 5 packages available for users of
operating systems that cannot run the Qt 6 builds.

Ending external Qt 5 package publication and removing Qt 5 source compatibility
are separate steps. Source compatibility and conditional code can be removed
after the remaining GitHub artifacts are retired.

## Rationale

Maintaining two Qt generations increases the cost of:

- CI and release workflows
- Distribution package definitions
- Build-system configuration
- Platform-specific testing
- Compatibility branches in application code

## Current Qt 6 Readiness

### Snap

There are working Qt 6 Snapcraft packaging paths for GitHub releases and
Launchpad. They now use `core26`, CMake with `QON_QT6_BUILD=ON`, and distribution
Qt 6 packages. The packaged Qt version is determined by those packages, not the
Qt installation named by the GitHub workflow.

On 2026-10-29, the published AMD64 `latest/stable` snap was verified as version
26.10.14, revision 13914, with base `core26`. Its `usr/bin/QOwnNotes` executable
links to Qt 6 libraries, including `libQt6Core.so.6` and `libQt6Widgets.so.6`,
with no Qt 5 dependencies.

A manual startup smoke test used the downloaded Store artifact, its bundled
libraries, and the `core26` runtime on an isolated Xvfb display. The executable
reported version 26.10.14, opened the "Welcome to QOwnNotes" window, and remained
running for 15 seconds without crashing. Settings and data were isolated from
the user's existing notes. This host had no snapd, so the test did not exercise
installation, confinement, the desktop launcher, or Wayland.

For this retirement decision, the maintainer accepts the published AMD64 Qt 6
snap as working on the basis of this test. Full snapd install-and-launch
automation remains desirable but is not a blocker for retiring the Qt 5 snap.
This acceptance does not constitute runtime validation of other architectures.

The Store lists Patrizio Bekerle (`pbek`) as publisher and shows current stable
and edge publications. Treat the existing Store publication as the active
replacement path. Launchpad appears to provide the multi-architecture builds;
its channel and promotion settings are managed outside this repository. The
previously inspected direct GitHub upload failed because its credentials had
expired and was allowed to fail. Repairing or removing that redundant path is
release-process cleanup, not a blocker while Store publication remains active.

References:

- [Snap Store listing](https://snapcraft.io/qownnotes)
- [Publisher dashboard](https://dashboard.snapcraft.io/snaps/qownnotes)
- [Store artifact metadata](https://api.snapcraft.io/v2/snaps/info/qownnotes)
- [`build-systems/github/snap.qt6/snapcraft.yaml`](../build-systems/github/snap.qt6/snapcraft.yaml)
- [`build-systems/snap/snapcraft/snapcraft.yaml`](../build-systems/snap/snapcraft/snapcraft.yaml)
- [`.github/workflows/build-release.yml`](../.github/workflows/build-release.yml)
- [`.github/workflows/build-continuous-release.yml`](../.github/workflows/build-continuous-release.yml)

### Open Build Service

The active Qt 6 OBS package is `home:pbek:QOwnNotes/desktop-cmake3`. It contains
CMake-based Qt 6 descriptions for Debian/Ubuntu, RPM-based distributions, and
Arch Linux. It successfully produces packages for most of its configured
repositories.

It is not a verified replacement for every former Qt 5 OBS target:

- `xUbuntu_23.04` and `xUbuntu_23.10` remain in the Qt 6 allowlist even though
  they do not expose current Qt 6 packages.
- `Fedora_Rawhide` has lagged behind the other Fedora Qt 6 repositories.
- `Debian_Unstable` is documented as a Qt 6 target but is absent from the Qt 6
  allowlist, so repository synchronization disables it.
- Repository synchronization is manual and is not part of normal release
  deployment.
- OBS project repositories, architectures, and scheduler results are configured
  server-side and cannot be fully audited from this repository.

Continue reconciling the allowlist with live OBS results after retiring the
legacy `desktop` package from the release process.

References:

- [`build-systems/obs/cmake-qt6/README.md`](../build-systems/obs/cmake-qt6/README.md)
- [`build-systems/obs/cmake-qt6/build-for-obs.sh`](../build-systems/obs/cmake-qt6/build-for-obs.sh)
- [`build-systems/obs/cmake-qt6/sync-obs-repositories.sh`](../build-systems/obs/cmake-qt6/sync-obs-repositories.sh)
- [`build-systems/releaser.kdl`](../build-systems/releaser.kdl)

## Remaining Qt 5 Channels

Retiring Qt 5 affects several release channels:

| Channel                          | Impact of retirement                                                                                                            |
| -------------------------------- | ------------------------------------------------------------------------------------------------------------------------------- |
| Ubuntu legacy PPA                | Publication is retired; users must move to a Qt 6 package channel, another distribution format, or remain on the final package. |
| OBS `desktop`                    | Publication is retired; older distribution targets may stop receiving releases.                                                 |
| Qt 5 Snap                        | Retained in the GitHub release workflow for now.                                                                                |
| Legacy macOS DMG                 | Retained in the GitHub release workflow for now.                                                                                |
| AppVeyor legacy Windows artifact | Retired; the final 26.10.0 build is archived as `QOwnNotes-Qt5.7-legacy.zip` in the GitHub v26.10.0 release.                    |

The normal Qt 6 Windows, macOS, AppImage, AUR, Gentoo, Nix, Snap, Launchpad, and
OBS paths should be assessed as replacements rather than assuming that the
presence of a Qt 6 definition proves equivalent platform coverage.

## Follow-up Plan

0. **Retire the AppVeyor Qt 5.7 Windows build** (done)
   - The build targeted Windows XP to Windows 8.1, which cannot run the Qt 6
     Windows release, so no Qt 6 replacement needs to be verified first.
   - It was the oldest Qt version still built, so removing it is a prerequisite
     for raising the minimum supported Qt 5 version to 5.15.

1. **Verify Qt 6 replacements**
   - AMD64 Qt 6 Store artifact and GUI startup verified; Snap replacement
     accepted for retirement on 2026-10-29 (done).
   - Add full snapd install-and-launch automation and repair or remove the
     failing direct Snap Store upload as non-blocking follow-ups.
   - Reconcile the OBS Qt 6 allowlist with live build results and documentation.
   - Confirm the minimum supported macOS and Windows versions for the Qt 6
     artifacts.

2. **Retire the remaining GitHub Qt 5 artifacts**
   - Next, announce the final Qt 5 snap and remove the legacy Snap release jobs;
     the AMD64 Qt 6 replacement is accepted.
   - Remove the legacy macOS release jobs after the Qt 6 macOS artifacts meet
     the release requirements.
   - Remove Qt 5-only Nix and test configurations when they no longer protect a
     supported release path.
   - Update installation documentation to direct users to Qt 6 packages or the
     archived final Qt 5 release.

3. **Remove Qt 5 source compatibility**
   - Make Qt 6 the default and only supported build configuration.
   - Select the minimum Qt 6 version from the oldest retained deployment target.
     The Snap definitions now use `core26`; the former Qt 6.4.2 assumption is
     obsolete. Verify packaged Qt versions across retained targets before
     selecting the minimum.
   - Remove Qt 5 compatibility branches and dependencies incrementally after Qt
     5 CI has been retired.

## Exit Criteria

The remaining GitHub Qt 5 publication can stop when all of the following are
true:

- The Qt 6 Snap replacement is accepted (met for AMD64 on 2026-10-29; see the
  documented startup test and its limitations above).
- A reliable Snap Store publication owner and path are established (accepted
  based on current Store publication under `pbek`; direct-upload cleanup remains
  a follow-up).
- Supported Windows and macOS versions have working Qt 6 artifacts.
- The affected platforms and final Qt 5 artifacts have been announced.

Qt 5 source support can be removed after publication has stopped and the project
has completed at least one Qt 6-only release cycle without a critical migration
regression.
