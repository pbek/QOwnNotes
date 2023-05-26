nix-build:
	nix-build -E '((import <nixpkgs> {}).qt6Packages.callPackage (import ./default.nix) { })'
