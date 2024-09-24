{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = with pkgs; with qt6; [
      just
      crowdin-cli
      cmake
      qmake
      qttools
      wrapQtAppsHook
      pkg-config
      libclang  # for clang-format
      xmlstarlet # for download-icons.sh and download_translations.sh
      lychee  # for link checking
      lurk  # strace replacement
      procs # fetches process info
      gawk  # for parsing procs output
      fzf # for interactive process selection

      # for ./build-systems/github/build-github-src.sh
      coreutils
      gh

      # for shell scripts
      findutils
      gnused
      ripgrep
    ];

    buildInputs = with pkgs; with qt6; [
      qtbase
      qtwebsockets
      qtdeclarative
      qtsvg
      botan2
    ];

    shellHook = ''
        echo "qmake:  $(qmake  --version)"
      '';
}

