# Bundled libgit2 regression test

This standalone test loads Qt6 before configuring bundled libgit2, matching the
application's configuration order. With Qt6 and strict C feature probes on Ubuntu
26.04, libgit2 previously failed to detect nanosecond `struct stat` fields and
could not compile. Loading libgit2 without Qt6 first did not reproduce the issue.

Configure and run with Qt6 Core development files, CMake 3.21 or newer, and C/C++
compilers installed:

```sh
cmake -S tests/bundled-libgit2 -B /tmp/opencode/qownnotes-bundled-libgit2-test
cmake --build /tmp/opencode/qownnotes-bundled-libgit2-test --parallel 4
ctest --test-dir /tmp/opencode/qownnotes-bundled-libgit2-test --output-on-failure
```

Use a fresh build directory when testing feature detection; CMake caches the
results. The test compiles the bundled library, initializes a local repository,
and verifies that a stored blob can be read back. It does not build QOwnNotes or
validate the complete Snap package.

The test can also be included in the main test build with
`-DQON_BUNDLED_LIBGIT2_TESTS=ON`; it is opt-in because configuring it downloads
the pinned dependency.
