nix-build:
	nix-build -E '((import <nixpkgs> {}).qt6Packages.callPackage (import ./default.nix) { })'

nix-build-force:
	nix-build -E '((import <nixpkgs> {}).qt6Packages.callPackage (import ./default.nix) { })' --check
