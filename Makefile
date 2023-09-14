.PHONY: translations-build translations-download translations-upload translations-update-files nix-build nix-build-force

translations-build:
	lrelease src/QOwnNotes.pro

translations-download:
	./scripts/download_translations.sh

translations-upload:
	crowdin upload

translations-update-files:
	./scripts/update-translations.sh

nix-build:
	nix-build -E '((import <nixpkgs> {}).qt6Packages.callPackage (import ./default.nix) { })'

nix-build-force:
	nix-build -E '((import <nixpkgs> {}).qt6Packages.callPackage (import ./default.nix) { })' --check

src-build:
	mkdir -p build-QOwnNotes; cd build-QOwnNotes && qmake "CONFIG+=debug USE_SYSTEM_BOTAN=1" ../src/QOwnNotes.pro && make

src-clean:
	rm -rf build-QOwnNotes

src-run:
	./build-QOwnNotes/QOwnNotes

src-build-run: src-build src-run
