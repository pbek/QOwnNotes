let
  nixpkgs = builtins.fetchTarball "channel:nixos-21.05";
  pkgs = import nixpkgs { config = { }; overlays = [ ]; };
in
with pkgs;
mkShell {
  nativeBuildInputs = [
    nodejs-12_x
    yarn
  ];

#  shellHook = ''
#    # try to work aroud build issues
#    unset TMPDIR
#
#    export NIX_PATH=nixpkgs=${toString nixpkgs}
#  '';
}