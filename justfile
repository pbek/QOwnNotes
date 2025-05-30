# Use `just <recipe>` to run a recipe
# https://just.systems/man/en/

# By default, run the `--list` command
default:
    @just --list

# Variables

transferDir := `if [ -d "$HOME/NextcloudPrivate/Transfer" ]; then echo "$HOME/NextcloudPrivate/Transfer"; else echo "$HOME/Nextcloud/Transfer"; fi`
sourceBuildDir := "build-QOwnNotes"
sourceBuildTestDir := "build-tests-QOwnNotes"

# Aliases

alias fix-linting := clang-format
alias linter-fix := clang-format
alias trace-process := process-trace
alias test := src-test
alias download-translations := translations-download
alias fmt := format

# Build the translations
[group('translations')]
translations-build:
    lrelease src/QOwnNotes.pro

# Download the translations from Crowdin
[group('translations')]
translations-download:
    ./scripts/download_translations.sh

# Upload the translations to Crowdin
[group('translations')]
translations-upload:
    crowdin upload

# Update the translations files
[group('translations')]
translations-update-files:
    ./scripts/update-translations.sh

# Build the application for nix
[group('nix')]
nix-build:
    nix-build -E '((import <nixpkgs> {}).qt6Packages.callPackage (import ./default.nix) { })'

# Build the application for nix for aaarch64 (throws errors while building Qt6)
[group('nix')]
nix-build-aarch64:
    nix-build -E '((import <nixpkgs> {}).pkgsCross.aarch64-multiplatform.qt6Packages.callPackage (import ./default.nix) { })'

# Build the application with cmake and Qt5 for nix
[group('nix')]
nix-build-cmake-qt5:
    nix-build -E '((import <nixpkgs> {}).libsForQt5.callPackage (import ./build-systems/nix/default-cmake-qt5.nix) { })'

# Build the application with Qt5 for nix
[group('nix')]
nix-build-qt5:
    nix build '.?submodules=1#qownnotes-qt5'

# Build the application with Qt 5.15.3 for nix
[group('nix')]
nix-build-qt5153:
    nix build '.?submodules=1#qownnotes-qt5153'

# Force a rebuild of the application with Qt5 for nix
[group('nix')]
nix-build-qt5-force:
    nix build '.?submodules=1#qownnotes-qt5' --rebuild

# Force a rebuild of the application with Qt 5.15.3 for nix
[group('nix')]
nix-build-qt5153-force:
    nix build '.?submodules=1#qownnotes-qt5153' --rebuild

# Build the application with Qt 6.9 for nix
[group('nix')]
nix-build-qt69 args='':
    nix build '.?submodules=1#qownnotes-qt69' {{ args }}

# Build the application with Qt6 for nix with a trace
[group('nix')]
nix-build-trace:
    nix-build -E '((import <nixpkgs> {}).qt6Packages.callPackage (import ./default.nix) { })' --show-trace

# Force a rebuild of the application with Qt6 for nix
[group('nix')]
nix-build-force:
    nix-build -E '((import <nixpkgs> {}).qt6Packages.callPackage (import ./default.nix) { })' --check

# Run the built application for nix
[group('nix')]
nix-run:
    ./result/bin/QOwnNotes --session test &

# Build the application direclty from the source
[group('src-build')]
src-build:
    qmake --version
    mkdir -p {{ sourceBuildDir }}; cd {{ sourceBuildDir }} && qmake "CONFIG+=debug USE_SYSTEM_BOTAN=1" ../src/QOwnNotes.pro CONFIG+=DEV_MODE && make -j$(nproc)

# Build the application direclty from the source
[group('src-build')]
src-build-internal-botan:
    qmake --version
    mkdir -p {{ sourceBuildDir }}; cd {{ sourceBuildDir }} && qmake "CONFIG+=debug" ../src/QOwnNotes.pro CONFIG+=DEV_MODE && make -j$(nproc)

# Build the application direclty from the source
[group('src-build')]
src-test:
    mkdir -p {{ sourceBuildTestDir }}; cd {{ sourceBuildTestDir }} && qmake CONFIG+=debug CONFIG+=DEV_MODE DEFINES+=INTEGRATION_TESTS ../tests/QOwnNotesTests.pro && make -j$(nproc)
    ./bin/tests/tests -platform minimal
    rm -rf {{ sourceBuildTestDir }}

# Clean the build directory
[group('src-build')]
src-clean:
    rm -rf {{ sourceBuildDir }}

# Run the built application
[group('src-build')]
src-run:
    ./{{ sourceBuildDir }}/QOwnNotes

# Build and run the application
[group('src-build')]
src-build-run: src-build src-run

# Do a clang format on the project
[group('linter')]
clang-format:
    ./scripts/clang-format-project.sh

# Check links in the markdown files
[group('linter')]
link-check:
    nix-shell -p lychee --run "lychee './**/*.md'"

# Apply a git patch to the project
[group('patches')]
git-apply-qownnotes-patch:
    git apply {{ transferDir }}/qownnotes.patch

# Apply a git patch to qmarkdowntextedit
[group('patches')]
git-apply-qmarkdowntextedit-patch:
    cd ./src/libraries/qmarkdowntextedit && git apply {{ transferDir }}/qmarkdowntextedit.patch

# Apply a git patch to piwiktracker
[group('patches')]
git-apply-piwiktracker-patch:
    cd ./src/libraries/piwiktracker && git apply {{ transferDir }}/piwiktracker.patch

# Apply a git patch to qttoolbareditor
[group('patches')]
git-apply-qttoolbareditor-patch:
    cd ./src/libraries/qttoolbareditor && git apply {{ transferDir }}/qttoolbareditor.patch

# Create git patches for the project and some libraries
[group('patches')]
git-create-patches:
    @echo "transferDir: {{ transferDir }}"
    git diff --no-ext-diff --staged --binary > {{ transferDir }}/qownnotes.patch
    cd src/libraries/qmarkdowntextedit && git diff --no-ext-diff --staged --binary > {{ transferDir }}/qmarkdowntextedit.patch
    cd src/libraries/piwiktracker && git diff --no-ext-diff --staged  --binary > {{ transferDir }}/piwiktracker.patch
    cd src/libraries/qttoolbareditor && git diff --no-ext-diff --staged  --binary > {{ transferDir }}/qttoolbareditor.patch
    ls -l1t {{ transferDir }} | head -5

# Open the Crowdin webpage
[group('translations')]
open-crowdin-webpage:
    xdg-open https://crowdin.com/project/qownnotes/activity-stream

# Fix the settings.ui file after QtCreator has destroyed it
[group('linter')]
fix-settings-ui-file:
    ./scripts/fix-settings-ui-file.sh

# Attach to the QOwnNotes process with lurk
[group('debug')]
process-trace:
    nix-shell -p lurk --run "sudo lurk --attach `procs QOwnNotes | fzf --height 40% --layout reverse | awk '{print $1}'`"

# Generate the icons for the whole project based on icons/icon.svg and icons/icon-dark.svg
[group('icons')]
generate-icons:
    cd icons &&./generate-icons.sh

# Generate the SNAP_TOKEN_GH token for the snap GitHub release action (see https://github.com/canonical/action-publish)
[group('snap')]
snap-generate-token:
    nix-shell -p snapcraft --run "snapcraft export-login --snaps=qownnotes --acls package_access,package_push,package_update,package_release -"

# Add git commit hashes to the .git-blame-ignore-revs file
[group('linter')]
add-git-blame-ignore-revs:
    git log --pretty=format:"%H" --grep="^lint" >> .git-blame-ignore-revs
    sort .git-blame-ignore-revs | uniq > .git-blame-ignore-revs.tmp
    mv .git-blame-ignore-revs.tmp .git-blame-ignore-revs

# Format all files
[group('linter')]
format args='':
    treefmt {{ args }}

# Run a GitHub workflow
[group('linter')]
run-github-workflow args='format-check':
    nix-shell -p act --run "act -j {{ args }}"

# Build botan3 amalgamation
[group('maintenance')]
build-botan3-amalgamation args='':
    nix-build -E 'with import <nixpkgs> {}; callPackage ./scripts/botan3-amalgamation.nix {}' {{ args }}
