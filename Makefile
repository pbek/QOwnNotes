.PHONY: translations-download translations-upload translations-update-files nix-build nix-build-force

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
