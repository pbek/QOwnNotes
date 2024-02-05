{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = with pkgs; with qt6; [
      gnumake
      crowdin-cli
      cmakeWithGui
      qmake
      qttools
      wrapQtAppsHook
      pkg-config
      libclang  # for clang-format
      xmlstarlet # for download-icons.sh and download_translations.sh
      lychee  # for link checking

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
      qt5compat
      botan2
    ];

    shellHook = ''
        echo "qmake:  $(qmake  --version)"
      '';
}

