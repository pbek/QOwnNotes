# Release builds and libgit2

The maintained CMake/Qt6 recipes enable libgit2 so **Show note git versions**
provides the built-in version list, diff, text preview, and restore action.

| Release method                                            | libgit2 source                                                 |
| --------------------------------------------------------- | -------------------------------------------------------------- |
| GitHub AppImage                                           | Ubuntu `libgit2-dev`; linuxdeploy bundles runtime dependencies |
| GitHub Windows Qt6 ZIP                                    | Pinned static library built with the application               |
| GitHub macOS Qt6 DMG                                      | Pinned static library, universal x86_64/arm64 build            |
| GitHub Qt6 Snap and Launchpad Snap                        | Pinned static library built with the application               |
| Launchpad Qt6 Debian packages (`../ubuntu-launchpad/qt6`) | Distribution `libgit2-dev`                                     |
| OBS CMake/Qt6 Debian, RPM, and Arch packages              | Distribution libgit2                                           |
| Slackware                                                 | Distribution/SBo libgit2, listed in `REQUIRES`                 |
| AUR                                                       | Distribution libgit2 (already enabled)                         |
| Nix Qt6 (`../default.nix`)                                | Nixpkgs libgit2 (already enabled)                              |

Distribution packages use `-DBUILD_WITH_LIBGIT2=ON`. Self-contained releases use
`-DBUILD_WITH_BUNDLED_LIBGIT2=ON`, which requires CMake 3.21 or newer. The bundled
version and archive SHA-256 are pinned in `../src/cmake/BundledLibGit2.cmake`.
It uses bundled compression/regex/hash implementations and disables SSH/HTTPS
transports: QOwnNotes uses libgit2 for local operations, not remote synchronization.

The Qt5 macOS release jobs remain supported. The qmake-based Gentoo recipe,
legacy Qt5 Flatpak manifest, and root `../snap/snapcraft.yaml` are not migrated by
the libgit2 change. The root Snap recipe now specifies Qt6, but its legacy
Qt5-named GitHub release jobs have been retired. Maintained GitHub Snap releases
use `github/snap.qt6`; the final genuine Qt5 AMD64 snap is archived in the
[26.10.6 release](https://github.com/pbek/QOwnNotes/releases/tag/v26.10.6).

Source-archive uploaders (GitHub, SourceForge, and TuxFamily) and release-post
scripts do not build application binaries and need no libgit2 linking changes.
