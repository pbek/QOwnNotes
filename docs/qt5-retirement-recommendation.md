# Qt 5 Retirement Recommendation

- Status: Accepted
- Reviewed: 2026-09-24
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
Launchpad. They use `core24`, `qmake6`, and Qt packages from Ubuntu 24.04 Noble.
The application inside the snap is therefore built with the Noble Qt 6.4.2
package family, not the Qt 6.10.2 installation named by the GitHub workflow.

The workflows currently verify that a snap is produced, but they do not install
and launch the finished artifact. Add a runtime smoke test before removing the
Qt 5 snap.

The direct GitHub-to-Snap-Store publication path also needs attention. The
latest inspected upload failed because the stored Snap Store credentials had
expired, and the step was allowed to fail. Launchpad appears to provide the
current multi-architecture store builds, but its channel and promotion settings
are managed outside this repository.

References:

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
| AppVeyor legacy Windows artifact | Users of older Windows versions may need to remain on the final compatible release.                                             |

The normal Qt 6 Windows, macOS, AppImage, AUR, Gentoo, Nix, Snap, Launchpad, and
OBS paths should be assessed as replacements rather than assuming that the
presence of a Qt 6 definition proves equivalent platform coverage.

## Follow-up Plan

1. **Verify Qt 6 replacements**
   - Install and launch the Qt 6 snap as part of release validation.
   - Repair or remove the failing direct Snap Store publication path.
   - Reconcile the OBS Qt 6 allowlist with live build results and documentation.
   - Confirm the minimum supported macOS and Windows versions for the Qt 6
     artifacts.

2. **Retire the remaining GitHub Qt 5 artifacts**
   - Remove the legacy Snap and macOS release jobs after their Qt 6 replacements
     meet the release requirements.
   - Remove Qt 5-only Nix and test configurations when they no longer protect a
     supported release path.
   - Update installation documentation to direct users to Qt 6 packages or the
     archived final Qt 5 release.

3. **Remove Qt 5 source compatibility**
   - Make Qt 6 the default and only supported build configuration.
   - Select the minimum Qt 6 version from the oldest retained deployment target.
     The current Qt 6 snap uses Qt 6.4.2, so requiring a newer Qt version must be
     evaluated separately.
   - Remove Qt 5 compatibility branches and dependencies incrementally after Qt
     5 CI has been retired.

## Exit Criteria

The remaining GitHub Qt 5 publication can stop when all of the following are
true:

- The Qt 6 snap installs and launches in automated or documented release
  validation.
- A reliable Snap Store publication owner and path are established.
- Supported Windows and macOS versions have working Qt 6 artifacts.
- The affected platforms and final Qt 5 artifacts have been announced.

Qt 5 source support can be removed after publication has stopped and the project
has completed at least one Qt 6-only release cycle without a critical migration
regression.
