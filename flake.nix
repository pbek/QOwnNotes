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

      # GitHub flake archives omit submodules. Re-fetch the same revision with Git
      # only when needed, so local builds retain working-tree changes.
      submodulePaths = builtins.filter (path: path != null) (
        map (
          line:
          let
            match = builtins.match "[[:space:]]*path = (.*)" line;
          in
          if match == null then null else builtins.head match
        ) (nixpkgs.lib.splitString "\n" (builtins.readFile ./.gitmodules))
      );
      hasSubmodules = builtins.all (
        path: builtins.pathExists (./. + "/${path}") && builtins.readDir (./. + "/${path}") != { }
      ) submodulePaths;
      source =
        if hasSubmodules then
          ./.
        else if self ? rev then
          builtins.fetchGit {
            url = "https://github.com/pbek/QOwnNotes.git";
            inherit (self) rev;
            submodules = true;
            shallow = true;
          }
        else
          throw "QOwnNotes requires Git submodules. Use nix build '.?submodules=1' from an initialized checkout.";
      packageSource = builtins.path {
        path = source + "/src";
        name = "qownnotes";
      };
    in
    {
      packages = forAllSystems (
        system:
        let
          pkgs = pkgsFor system;
          pkgs-qt69 = pkgsQt69For system;
          pkgs-qt5153 = pkgsQt5153For system;

          basePackages =
            pkgs.lib.mapAttrs
              (
                _: package:
                package.overrideAttrs (_: {
                  src = packageSource;
                })
              )
              (
                {
                  qownnotes-qt6 = pkgs.callPackage (import ./default.nix) { };
                  qownnotes-qt6-qlitehtml = pkgs.callPackage (import ./default.nix) { useQlitehtml = true; };
                  qownnotes-qt69 = pkgs-qt69.callPackage (import ./default.nix) { };
                  qownnotes-qt5 = pkgs.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix) { };
                  qownnotes-cmake-qt5 =
                    pkgs.libsForQt5.callPackage (import ./build-systems/nix/default-cmake-qt5.nix)
                      { };
                  qownnotes-qt5153 =
                    pkgs-qt5153.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix)
                      { };
                  default = pkgs.qt6Packages.callPackage (import ./default.nix) { };
                }
                // pkgs.lib.optionalAttrs pkgs.stdenv.hostPlatform.isLinux {
                  qownnotes-qt6-kde = pkgs.callPackage (import ./default.nix) {
                    useKdeIntegration = true;
                    qt6Packages = pkgs.kdePackages;
                  };
                }
              );

          # Incremental Nix builds for development (see "just nix-build-incremental").
          # The "-checkpoint" package stores the whole build directory of a full build,
          # the "-incremental" package restores it and only recompiles changed sources.
          # The checkpoint store path is passed impurely via QON_NIX_CHECKPOINT.
          checkpointPath = builtins.getEnv "QON_NIX_CHECKPOINT";
          checkpointPackages =
            pkgs.lib.concatMapAttrs
              (
                name: package:
                let
                  # The checkpoint tooling diffs and restores the build directory as a whole,
                  # so the sources and the CMake build output need to be in the same directory
                  inSourcePackage = package.overrideAttrs (_: {
                    dontUseCmakeBuildDir = true;
                  });
                in
                {
                  "${name}-checkpoint" = pkgs.checkpointBuildTools.prepareCheckpointBuild inSourcePackage;
                }
                // pkgs.lib.optionalAttrs (checkpointPath != "") {
                  "${name}-incremental" = pkgs.checkpointBuildTools.mkCheckpointBuild inSourcePackage (
                    builtins.storePath checkpointPath
                  );
                }
              )
              (
                pkgs.lib.getAttrs (builtins.filter (name: basePackages ? ${name}) [
                  "qownnotes-qt6"
                  "qownnotes-qt6-kde"
                ]) basePackages
              );
        in
        basePackages // checkpointPackages
      );

      devShells = forAllSystems (
        system:
        let
          pkgs = import nixpkgs { inherit system; };
        in
        {
          default = pkgs.mkShell {
            nativeBuildInputs =
              with pkgs.qt6;
              [
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
              ]
              ++ pkgs.lib.optionals pkgs.stdenv.hostPlatform.isLinux [
                pkgs.kdePackages.extra-cmake-modules
              ];

            buildInputs =
              with pkgs.qt6;
              [
                qtbase
                qtwebsockets
                qtdeclarative
                qtsvg
                pkgs.botan3
                pkgs.libgit2
                pkgs.libsecret
              ]
              ++ pkgs.lib.optionals pkgs.stdenv.hostPlatform.isLinux [
                pkgs.kdePackages.kcolorscheme
                pkgs.kdePackages.kcoreaddons
                pkgs.kdePackages.kcrash
                pkgs.kdePackages.kglobalaccel
                pkgs.kdePackages.knotifications
                pkgs.kdePackages.plasma-activities
                pkgs.kdePackages.kwindowsystem
                pkgs.kdePackages.purpose
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
        qownnotes-kde = self.packages.x86_64-linux.qownnotes-qt6-kde.overrideAttrs (old: {
          cmakeFlags = old.cmakeFlags ++ [
            "-DDEV_MODE=ON"
            "-DCMAKE_BUILD_TYPE=Debug"
            # The bundled Aspell backend has an existing GCC maybe-uninitialized warning.
            # Keep Aspell in release packages; validate developer warnings with Hunspell here.
            "-DBUILD_WITH_ASPELL=OFF"
          ];
          doInstallCheck = true;
          installCheckPhase = ''
            runHook preInstallCheck
            export HOME="$TMPDIR/kde-smoke-home"
            mkdir -p "$HOME"
            # CLI startup must not require a graphical session after KAboutData registration.
            env -u DISPLAY -u WAYLAND_DISPLAY "$out/bin/QOwnNotes" --help > help.txt
            grep -q 'QOwnNotes' help.txt
            env -u DISPLAY -u WAYLAND_DISPLAY "$out/bin/QOwnNotes" --completion bash > completion.bash
            test -s completion.bash
            runHook postInstallCheck
          '';
        });
        qownnotes-kde-tests =
          let
            pkgs = pkgsFor "x86_64-linux";
          in
          pkgs.stdenv.mkDerivation {
            pname = "qownnotes-kde-integration-tests";
            version = "1";
            src = self;
            nativeBuildInputs = [
              pkgs.cmake
              pkgs.dbus
              pkgs.kdePackages.wrapQtAppsHook
            ];
            buildInputs = [
              pkgs.kdePackages.qtbase
              pkgs.kdePackages.kcolorscheme
              pkgs.kdePackages.kglobalaccel
              pkgs.kdePackages.knotifications
              pkgs.kdePackages.plasma-activities
              pkgs.kdePackages.kwindowsystem
              pkgs.kdePackages.purpose
            ];
            dontWrapQtApps = true;
            preConfigure = ''
              cmakeDir="$PWD/tests/kde"
            '';
            doCheck = true;
            checkPhase = ''
              runHook preCheck
              dbus-run-session --config-file=${pkgs.dbus}/share/dbus-1/session.conf -- \
                env QON_KDE_TEST_BUS=1 QT_QPA_PLATFORM=minimal ./kde-integration-tests
              dbus-run-session --config-file=${pkgs.dbus}/share/dbus-1/session.conf -- \
                env QON_KDE_TEST_BUS=1 QT_QPA_PLATFORM=minimal ./kde-notification-tests
              dbus-run-session --config-file=${pkgs.dbus}/share/dbus-1/session.conf -- \
                env QON_KDE_TEST_BUS=1 QT_QPA_PLATFORM=minimal ./kde-activities-tests
              dbus-run-session --config-file=${pkgs.dbus}/share/dbus-1/session.conf -- \
                env QON_KDE_TEST_BUS=1 ./kde-krunner-tests
              env QT_QPA_PLATFORM=minimal ./kde-colorscheme-tests
              env QT_QPA_PLATFORM=minimal ./kde-windowactivation-tests
              # The Purpose plugins provide the targets, e.g. email and the clipboard
              env QT_QPA_PLATFORM=minimal QON_EXPECT_SHARE_TARGETS=1 \
                QT_PLUGIN_PATH=${pkgs.kdePackages.purpose}/${pkgs.kdePackages.qtbase.qtPluginPrefix} \
                ./kde-share-tests
              runHook postCheck
            '';
            installPhase = ''
              mkdir -p "$out"
            '';
          };
      };
    };
}
