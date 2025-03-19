{
  pkgs ? import <nixpkgs> { },
}:
pkgs.mkShell {
  # nativeBuildInputs is usually what you want -- tools you need to run
  nativeBuildInputs =
    with pkgs;
    with qt6;
    [
      just
      crowdin-cli
      cmake
      qmake
      qttools
      wrapQtAppsHook
      pkg-config
      libclang # for clang-format
      treefmt # formatting tool
      nodePackages.prettier # for webpage formatting
      shfmt # for shell formatting
      nixfmt-rfc-style # for nix formatting
      xmlstarlet # for download-icons.sh and download_translations.sh
      lychee # for link checking
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
    ++ (
      if pkgs.stdenv.isLinux then
        [
          pkgs.lurk # strace replacement
        ]
      else
        [ ]
    );

  buildInputs =
    with pkgs;
    with qt6;
    [
      qtbase
      qtwebsockets
      qtdeclarative
      qtsvg
      botan2
    ];

  shellHook = ''
    ./scripts/nix-update-qmake-symlinks.sh
  '';
}
