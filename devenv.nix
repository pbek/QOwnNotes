{
  pkgs,
  config,
  ...
}:

{
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
      qmake
      qttools
      wrapQtAppsHook

      # Packages for "just src-build"
      qtbase
      qtdeclarative
      qtsvg
      qtwebsockets

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

      # Packages for treefmt
      treefmt
      libclang
      nodePackages.prettier
      shfmt
      nixfmt-rfc-style
      statix
      taplo
      cmake-format
    ];

  # https://devenv.sh/git-hooks/
  git-hooks.hooks.treefmt.enable = true;

  # This script updates the symlinks to the qmake binaries in the nix store
  # The symlinks are to be used in Qt Creator and CLion
  scripts.update-qmake-symlinks.exec = ''
    BIN_DIR="''${DEVENV_ROOT}/bin"
    echo "🔗 Updating qmake symlinks in ''${BIN_DIR}..."

    mkdir -p "''${BIN_DIR}"
    ln -sf "${pkgs.kdePackages.qtbase}/bin/qmake" "''${BIN_DIR}/qmake6"
    ln -sf "${pkgs.libsForQt5.qt5.qtbase.dev}/bin/qmake" "''${BIN_DIR}/qmake5"
  '';

  enterShell = ''
    update-qmake-symlinks
  '';

  # See full reference at https://devenv.sh/reference/options/
}
