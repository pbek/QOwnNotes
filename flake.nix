{
  description = "QOwnNotes dev flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    nixpkgs-qt69.url = "github:nixos/nixpkgs/6f095be4bda67105d2428224f4fb4899234ce683";
    nixpkgs-qt5153.url = "github:nixos/nixpkgs/72bbea9db7d727ed044e60b5f5febc60a3c5c955";
    #    systems.url = "github:nix-systems/default";
  };

  outputs =
    {
      self,
      nixpkgs,
      nixpkgs-qt69,
      nixpkgs-qt5153,
    }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
      pkgs-qt69 = nixpkgs-qt69.legacyPackages.${system};
      pkgs-qt5153 = nixpkgs-qt5153.legacyPackages.${system};
    in
    {
      packages.${system} = {
        qownnotes-qt6 = pkgs.qt6Packages.callPackage (import ./default.nix) { };
        qownnotes-qt69 = pkgs-qt69.qt6Packages.callPackage (import ./default.nix) { };
        qownnotes-qt5 = pkgs.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix) { };
        qownnotes-qt5153 =
          pkgs-qt5153.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix)
            { };
        default = pkgs.qt6Packages.callPackage (import ./default.nix) { };
      };

      devShell.x86_64-linux =
        with import nixpkgs { system = "x86_64-linux"; };
        mkShell {
          nativeBuildInputs =
            with nixpkgs;
            with qt6;
            [
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

          buildInputs =
            with nixpkgs;
            with qt6;
            [
              qtbase
              qtwebsockets
              qtdeclarative
              qtsvg
              botan3
              libgit2
            ];

          shellHook = ''
            echo "qmake:  $(qmake --version)"
          '';
        };
    };
}
