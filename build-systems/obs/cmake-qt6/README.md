# OBS Build with CMake and Qt6

[OBS package](https://build.opensuse.org/package/show/home:pbek:QOwnNotes/desktop-cmake3)

The package contains build descriptions for Debian/Ubuntu, RPM-based
distributions, and Arch Linux. Only OBS repositories whose distribution
provides Qt6, libgit2 development files, and CMake 3.16 or newer can build this
package. All three package formats enable the built-in Git version viewer with
the distribution's libgit2; runtime dependencies are tracked by the packaging tools.

CDBS doesn't support qmake after Debian 12 anymore, see [#3267](https://github.com/pbek/QOwnNotes/issues/3267).

To enable the repositories that provide the required Qt6 packages, authenticate
`osc` and run:

```bash
./sync-obs-repositories.sh
```

The allowlist is maintained in `sync-obs-repositories.sh`. It updates the build,
publish, debuginfo, and use-for-build sections because the package disables all
repositories by default.
