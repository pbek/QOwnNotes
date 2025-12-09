{
  pkgs,
  inputs,
  ...
}:

let
  unstablePkgs = inputs.nixpkgs-unstable.legacyPackages.${pkgs.stdenv.hostPlatform.system};

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
      prettier # for formatting QSS files

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
  git-hooks = {
    # prek 0.2.4 is crashing for this project, so we force using prek from nixpkgs-unstable for prek 0.2.18
    # package = lib.mkForce unstablePkgs.prek;

    hooks = {
      cmake-format.enable = true;
      taplo.enable = true;
      clang-format = {
        enable = true;
        files = "\\.(cpp|h)$";
        excludes = [
          "src/libraries/diff_match_patch/" # Library was broken by clang-format
        ];
      };
      prettier-qss = {
        enable = true;
        name = "prettier-qss";
        description = "Format QSS files with prettier";
        files = "\\.(qss)$";
        entry = "${pkgs.prettier}/bin/prettier --write --parser css";
        language = "system";
        pass_filenames = true;
      };
    };
    excludes = [
      "webpage/src/(ar|de|es|fa|fr|hu|it|ko|nl|pl)/.*"
    ];
  };

  enterShell = ''
    echo "🛠️ QOwnNotes Dev Shell"
    echo "📦 Qt6 version: $(qmake6 -query QT_VERSION)"
    echo "📦 Qt5 version: $(${pkgs.libsForQt5.qt5.qtbase.dev}/bin/qmake -query QT_VERSION)"
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
  };

  tasks = {
    "format:all" = {
      description = "Formats all source files";
      exec = "pre-commit run --all-files";
    };
  };

  # See full reference at https://devenv.sh/reference/options/
}
