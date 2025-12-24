{ pkgs, ... }:

{
  # languages.javascript.package = pkgs.nodejs_20;

  # https://devenv.sh/packages/
  packages = with pkgs; [
    zellij
  ];

  env = {
    # We need to set PLAYWRIGHT_BROWSERS_PATH for Playwright to find it's ffmpeg binary
    PLAYWRIGHT_BROWSERS_PATH = "${pkgs.playwright.browsers}";
  };

  # Disable the hint that are printed when the dotenv module is not enabled, but .env is present
  # https://devenv.sh/reference/options/#dotenvdisablehint
  dotenv.disableHint = true;

  # https://devenv.sh/scripts/
  scripts = {
    get_playwright_test_version = {
      exec = ''
        jq -r '.dependencies["@playwright/test"] // .devDependencies["@playwright/test"]' package.json
      '';
      packages = [ pkgs.jq ];
      description = "Get the Playwright Test npm version from package.json";
    };
    get_playwright_core_version = {
      exec = ''
        jq -r '.dependencies["playwright-core"] // .devDependencies["playwright-core"]' package.json
      '';
      packages = [ pkgs.jq ];
      description = "Get the Playwright Core npm version from package.json";
    };
  };

  enterShell = ''
    echo "🛠️ QOwnNotes Webpage Dev Shell"
    playwrightTestNpmVersion="$(get_playwright_test_version)"
    playwrightCoreNpmVersion="$(get_playwright_core_version)"
    echo "❄️ Playwright nix version: ${pkgs.playwright.version}"
    echo "📦 Playwright Test npm version: $playwrightTestNpmVersion"
    echo "📦 Playwright Core npm version: $playwrightCoreNpmVersion"
    echo "📂 Playwright browsers path: $PLAYWRIGHT_BROWSERS_PATH"
    echo "📦 Chromium binary: $CHROMIUM_BIN"
    echo "📦 Firefox binary: $FIREFOX_BIN"

    if [ "${pkgs.playwright.version}" != "$playwrightTestNpmVersion" ]; then
        echo "❌ Playwright Test versions in nix (in devenv.yaml) and npm (in package.json) are not the same! Please adapt the configuration."
    else
        echo "✅ Playwright Test versions in nix and npm are the same"
    fi

    if [ "${pkgs.playwright.version}" != "$playwrightCoreNpmVersion" ]; then
        echo "❌ Playwright Core versions in nix (in devenv.yaml) and npm (in package.json) are not the same! Please adapt the configuration."
    else
        echo "✅ Playwright Core versions in nix and npm are the same"
    fi
  '';

  git-hooks.hooks = {
    eslint.enable = false;
  };
}
