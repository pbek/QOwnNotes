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
    mkdir -p {{ sourceBuildDir }}; cd {{ sourceBuildDir }} && qmake "CONFIG+=debug USE_SYSTEM_BOTAN=1" ../src/QOwnNotes.pro && make -j$(nproc)

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
    lychee './**/*.md'

# Apply a git patch to the project
[group('patches')]
git-apply-qownnotes-patch:
    git apply {{ transferDir }}/qownnotes.patch

# Apply a git patch to qmarkdowntextedit
[group('patches')]
git-apply-qmarkdowntextedit-patch:
    cd ./src/libraries/qmarkdowntextedit && git apply {{ transferDir }}/qmarkdowntextedit.patch

# Create a git patch for the project and some libraries
[group('patches')]
git-create-patch:
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

# Fig the settings.ui file after QtCreator has destroyed it
[group('linter')]
fix-settings-ui-file:
    ./scripts/fix-settings-ui-file.sh

# Attach to the QOwnNotes process with lurk
[group('debug')]
process-trace:
    sudo lurk --attach `procs QOwnNotes | fzf --height 40% --layout reverse | awk '{print $1}'`

# Format all justfiles
[group('linter')]
just-format:
    #!/usr/bin/env bash
    # Find all files named "justfile" recursively and run just --fmt --unstable on them
    find . -type f -name "justfile" -print0 | while IFS= read -r -d '' file; do
        echo "Formatting $file"
        just --fmt --unstable -f "$file"
    done
