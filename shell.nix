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

      # for ./build-systems/github/build-github-src.sh
      coreutils
      gh
    ];

    buildInputs = with pkgs; with qt6; [
      qtbase
      qtwebsockets
      qtdeclarative
      qtsvg
      qt5compat
      botan2
    ];
}

