{
  description = "QOwnNotes dev flake";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = { self, nixpkgs }: {

    devShell.x86_64-linux =
    with import nixpkgs {
      system = "x86_64-linux";
    };
    mkShell {
      nativeBuildInputs = with nixpkgs; with qt6; [
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

      buildInputs = with nixpkgs; with qt6; [
        qtbase
        qtwebsockets
        qtdeclarative
        qtsvg
        qt5compat
        botan2
      ];

      shellHook = ''
        echo "qmake:  $(qmake --version)"
      '';
    };
  };
}
