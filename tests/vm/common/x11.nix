{ lib, ... }:

{
  imports = [
    ./auto.nix
  ];

  services = {
    xserver = {
      enable = true;
      # Use IceWM as the window manager.
      windowManager.icewm.enable = true;
    };
    # Don't use a desktop manager.
    displayManager.defaultSession = lib.mkDefault "none+icewm";
  };

  # Automatically log in.
  test-support.displayManager.auto.enable = true;

  # Help with OCR
  environment.etc."icewm/theme".text = ''
    Theme="gtk2/default.theme"
  '';
}
