{
  lib,
  stdenv,
  fetchurl,
  cmake,
  qttools,
  qtbase,
  qtdeclarative,
  qtsvg,
  qtwayland,
  qtwebsockets,
  qtx11extras,
  makeWrapper,
  wrapQtAppsHook,
  botan3,
  libgit2,
  pkg-config,
  xvfb-run,
  installShellFiles,
}:

let
  pname = "qownnotes";
  appname = "QOwnNotes";
  #  version = builtins.head (builtins.match "#define VERSION \"([0-9.]+)\"" (builtins.readFile ./src/version.h));
  version = "local-build";
in
stdenv.mkDerivation {
  inherit pname appname version;

  src = builtins.path {
    path = ../../src;
    name = "qownnotes";
  };

  nativeBuildInputs =
    [
      cmake
      qttools
      wrapQtAppsHook
      pkg-config
      installShellFiles
    ]
    ++ lib.optionals stdenv.isLinux [ xvfb-run ]
    ++ lib.optionals stdenv.isDarwin [ makeWrapper ];

  buildInputs = [
    qtbase
    qtdeclarative
    qtsvg
    qtwebsockets
    qtx11extras
    botan3
    libgit2
  ] ++ lib.optionals stdenv.isLinux [ qtwayland ];

  cmakeFlags = [
    #    "-DQON_QT6_BUILD=ON"
    "-DBUILD_WITH_SYSTEM_BOTAN=ON"
    "-DBUILD_WITH_LIBGIT2=ON"
  ];

  postInstall =
    ''
      #    installShellCompletion --cmd ${appname} \
      #      --bash <(xvfb-run $out/bin/${appname} --completion bash) \
      #      --fish <(xvfb-run $out/bin/${appname} --completion fish)
      #    installShellCompletion --cmd ${pname} \
      #      --bash <(xvfb-run $out/bin/${appname} --completion bash) \
      #      --fish <(xvfb-run $out/bin/${appname} --completion fish)
    ''
    # Create a lowercase symlink for Linux
    + lib.optionalString stdenv.isLinux ''
      ln -s $out/bin/${appname} $out/bin/${pname}
    ''
    # Remame application for macOS as lowercase binary
    + lib.optionalString stdenv.isDarwin ''
      find $out
      # Prevent "same file" error
      mv $out/bin/${appname} $out/bin/${pname}.bin
      mv $out/bin/${pname}.bin $out/bin/${pname}
    '';

  meta = with lib; {
    description = "Plain-text file notepad and todo-list manager with Markdown support and Nextcloud/ownCloud integration";
    homepage = "https://www.qownnotes.org/";
    changelog = "https://www.qownnotes.org/changelog.html";
    downloadPage = "https://github.com/pbek/QOwnNotes/releases/tag/v${version}";
    license = licenses.gpl2Only;
    maintainers = with maintainers; [
      pbek
      totoroot
    ];
    platforms = platforms.unix;
  };
}
