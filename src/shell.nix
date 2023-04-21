{ pkgs ? import <nixpkgs> {} }:
  pkgs.mkShell {
    # nativeBuildInputs is usually what you want -- tools you need to run
    nativeBuildInputs = [
      pkgs.buildPackages.qt5.qmake
      pkgs.buildPackages.qt5.qtx11extras
      pkgs.buildPackages.qt5.qtwebsockets
      pkgs.buildPackages.qt5.qtdeclarative
      pkgs.buildPackages.qt5.qtsvg
    ];
}

