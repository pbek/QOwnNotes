{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = with pkgs; with qt6; [
      crowdin-cli
      cmakeWithGui
      qmake
      qttools
      wrapQtAppsHook
      pkg-config
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

