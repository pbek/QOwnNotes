{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = [
      pkgs.crowdin-cli
      pkgs.buildPackages.cmakeWithGui
      pkgs.buildPackages.qt6.qtbase
      pkgs.buildPackages.qt6.qmake
      pkgs.buildPackages.qt6.qttools
      pkgs.buildPackages.qt6.wrapQtAppsHook
      pkgs.buildPackages.qt6.qtwebsockets
      pkgs.buildPackages.qt6.qtdeclarative
      pkgs.buildPackages.qt6.qtsvg
    ];
}

