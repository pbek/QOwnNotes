{ pkgs, ... }:

{
  # languages.javascript.package = pkgs.nodejs_20;

  # https://devenv.sh/packages/
  packages = with pkgs; [
    zellij
  ];

  enterShell = ''
    echo "🛠️ QOwnNotes Webpage Dev Shell"
  '';

  git-hooks.hooks = {
    eslint.enable = false;
  };
}
