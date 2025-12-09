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
      # deadnix: skip
      self, # to prevent: error: function 'outputs' called with unexpected argument 'self'
      nixpkgs,
      nixpkgs-qt69,
      nixpkgs-qt5153,
    }:
    let
      # Define supported systems
      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      # Helper function to generate attributes for each system
      forAllSystems = nixpkgs.lib.genAttrs systems;

      # Helper to get package sets for a given system
      pkgsFor = system: nixpkgs.legacyPackages.${system};
      pkgsQt69For = system: nixpkgs-qt69.legacyPackages.${system};
      pkgsQt5153For = system: nixpkgs-qt5153.legacyPackages.${system};
    in
    {
      packages = forAllSystems (
        system:
        let
          pkgs = pkgsFor system;
          pkgs-qt69 = pkgsQt69For system;
          pkgs-qt5153 = pkgsQt5153For system;
        in
        {
          qownnotes-qt6 = pkgs.callPackage (import ./default.nix) { };
          qownnotes-qt69 = pkgs-qt69.callPackage (import ./default.nix) { };
          qownnotes-qt5 = pkgs.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix) { };
          qownnotes-qt5153 =
            pkgs-qt5153.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix)
              { };
          default = pkgs.qt6Packages.callPackage (import ./default.nix) { };
        }
      );

      devShells = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.mkShell {
            nativeBuildInputs = with pkgs.qt6; [
              pkgs.gnumake
              pkgs.crowdin-cli
              pkgs.cmakeWithGui
              qmake
              qttools
              wrapQtAppsHook
              pkgs.pkg-config

              # for ./build-systems/github/build-github-src.sh
              pkgs.coreutils
              pkgs.gh
            ];

            buildInputs = with pkgs.qt6; [
              qtbase
              qtwebsockets
              qtdeclarative
              qtsvg
              pkgs.botan3
              pkgs.libgit2
            ];

            shellHook = ''
              echo "qmake:  $(qmake --version)"
            '';
          };
        }
      );

      # Legacy devShell attribute for backwards compatibility
      devShell = forAllSystems (system: self.devShells.${system}.default);

      checks.x86_64-linux = {
        qownnotes = (pkgsFor "x86_64-linux").testers.runNixOSTest ./tests/vm/qownnotes.nix;
      };
    };
}
