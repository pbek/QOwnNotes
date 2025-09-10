{
  pkgs,
  inputs,
  ...
}:

let
  unstablePkgs = inputs.nixpkgs-unstable.legacyPackages.${pkgs.system};

  # We need the unstable Qt packages for the latest versions of the tooling (so there is no minor version mismatch)
  unstableQtPkgs = with unstablePkgs.kdePackages; [
    qmake
    qttools
    wrapQtAppsHook
    # Packages for "just src-build"
    qtbase
    qtdeclarative
    qtsvg
    qtwebsockets
  ];
in
{
  # https://devenv.sh/reference/options/#cachixpull
  cachix.pull = [
    "devenv"
    "qownnotes-devenv"
  ];

  # https://devenv.sh/supported-languages/cplusplus/
  languages.cplusplus.enable = true;

  # https://devenv.sh/packages/
  packages =
    with pkgs;
    with qt6;
    with qt6Packages;
    [
      just
      crowdin-cli
      cmake

      # Packages for "just src-build"
      botan3

      pkg-config
      libclang # for clang-format
      procs # fetches process info
      gawk # for parsing procs output
      fzf # for interactive process selection

      # for ./build-systems/github/build-github-src.sh
      coreutils
      gh

      # for shell scripts
      findutils
      gnused
      ripgrep
      librsvg
      imagemagick
      libicns
    ]
    ++ unstableQtPkgs;

  # https://devenv.sh/git-hooks/
  git-hooks.hooks = {
    # https://devenv.sh/reference/options/#git-hookshookstreefmt
    # https://github.com/numtide/treefmt
    # https://github.com/numtide/treefmt-nix
    treefmt = {
      enable = true;
      settings.formatters = with pkgs; [
        libclang
        nodePackages.prettier
        shfmt
        nixfmt-rfc-style
        statix
        taplo
        cmake-format
      ];
    };

    # https://devenv.sh/reference/options/#git-hookshooksdeadnix
    # https://github.com/astro/deadnix
    deadnix = {
      enable = true;
      settings = {
        edit = true; # Allow to edit the file if it is not formatted
      };
    };
  };

  enterShell = ''
    update-qmake-symlinks
  '';

  scripts = {
    # This script updates the symlinks to the qmake binaries in the nix store
    # The symlinks are to be used in Qt Creator and CLion
    update-qmake-symlinks.exec = ''
      BIN_DIR="''${DEVENV_ROOT}/bin"
      echo "🔗 Updating qmake symlinks in ''${BIN_DIR}..."

      mkdir -p "''${BIN_DIR}"
      ln -sf "${unstablePkgs.kdePackages.qtbase}/bin/qmake" "''${BIN_DIR}/qmake6"
      ln -sf "${pkgs.libsForQt5.qt5.qtbase.dev}/bin/qmake" "''${BIN_DIR}/qmake5"
    '';
    "app:build:qt6" = {
      description = "Builds the app with Qt 6";
      exec = ''
        nix-build -E 'let pkgs = import ${inputs.nixpkgs-unstable} {}; in pkgs.qt6Packages.callPackage ./default.nix { }' -- $1
      '';
    };
    "app:build:qt69" = {
      description = "Builds the app with Qt 6.9";
      exec = ''
        nix-build -E 'let pkgs = import ${inputs.nixpkgs-qt69} {}; in pkgs.qt6Packages.callPackage ./default.nix { }' -- $1
      '';
    };
    "app:build:qt5-qmake" = {
      description = "Builds the app with Qt 5 using qmake";
      exec = ''
        nix-build -E 'let pkgs = import ${inputs.nixpkgs-unstable} {}; in pkgs.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix) { }' -- $1
      '';
    };
    "app:build:qt5-cmake" = {
      description = "Builds the app with Qt 5 using CMake";
      exec = ''
        nix-build -E 'let pkgs = import ${inputs.nixpkgs-unstable} {}; in pkgs.libsForQt5.callPackage (import ./build-systems/nix/default-cmake-qt5.nix) { }' -- $1
      '';
    };
    "app:build:qt5153-qmake" = {
      description = "Builds the app with Qt 5.15.3 using qmake";
      exec = ''
        nix-build -E 'let pkgs = import ${inputs.nixpkgs-qt5153} {}; in pkgs.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix) { }' -- $1
      '';
    };
  };

  tasks = {
    "format:all" = {
      description = "Formats all source files";
      exec = "pre-commit run --all-files";
    };
    #    "app:build:qt6" = {
    #      description = "Builds the app with Qt 6";
    #      exec = ''
    #        nix-build -E 'let pkgs = import ${inputs.nixpkgs-unstable} {}; in pkgs.qt6Packages.callPackage ./default.nix { }'
    #      '';
    #    };
    #    "app:build:qt69" = {
    #      description = "Builds the app with Qt 6.9";
    #      exec = ''
    #        nix-build -E 'let pkgs = import ${inputs.nixpkgs-qt69} {}; in pkgs.qt6Packages.callPackage ./default.nix { }'
    #      '';
    #    };
    #    "app:build:qt5-qmake" = {
    #      description = "Builds the app with Qt 5 using qmake";
    #      exec = ''
    #        nix-build -E 'let pkgs = import ${inputs.nixpkgs-unstable} {}; in pkgs.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix) { }'
    #      '';
    #    };
    #    "app:build:qt5-cmake" = {
    #      description = "Builds the app with Qt 5 using CMake";
    #      exec = ''
    #        nix-build -E 'let pkgs = import ${inputs.nixpkgs-unstable} {}; in pkgs.libsForQt5.callPackage (import ./build-systems/nix/default-cmake-qt5.nix) { }'
    #      '';
    #    };
    #    "app:build:qt5153-qmake" = {
    #      description = "Builds the app with Qt 5.15.3 using qmake";
    #      exec = ''
    #        nix-build -E 'let pkgs = import ${inputs.nixpkgs-qt5153} {}; in pkgs.libsForQt5.callPackage (import ./build-systems/nix/default-qt5.nix) { }'
    #      '';
    #    };
  };

  # See full reference at https://devenv.sh/reference/options/
}
