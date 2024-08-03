{
  description = "QOwnNotes dev flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    nixpkgs-qt515.url = "github:nixos/nixpkgs/72bbea9db7d727ed044e60b5f5febc60a3c5c955";
    systems.url = "github:nix-systems/default";
  };

  outputs = {
    self,
    nixpkgs,
    nixpkgs-qt515,
#    systems,
  }: let
    systems = [ "x86_64-linux" ];
    eachSystem = nixpkgs.lib.genAttrs (import systems);
  in {
    packages = eachSystem (system: {
      qownnotes-qt5 = nixpkgs.${system}.legacyPackages.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix) { };
      qownnotes-qt515 = nixpkgs-qt515.${system}.legacyPackages.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix) { };
    });

    devShell.x86_64-linux =
    with import nixpkgs {
      system = "x86_64-linux";
    };
    mkShell {
      nativeBuildInputs = with nixpkgs; with qt6; [
        gnumake
        crowdin-cli
        cmakeWithGui
        qmake
        qttools
        wrapQtAppsHook
        pkg-config

        # for ./build-systems/github/build-github-src.sh
        coreutils
        gh
      ];

      buildInputs = with nixpkgs; with qt6; [
        qtbase
        qtwebsockets
        qtdeclarative
        qtsvg
        qt5compat
        botan2
      ];

      shellHook = ''
        echo "qmake:  $(qmake --version)"
      '';
    };
  };
}
