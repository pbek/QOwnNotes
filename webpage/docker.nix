{
  pkgs ? import <nixpkgs> { },
}:

let
  nginx = pkgs.nginx;

  npmPackage = pkgs.buildNpmPackage {
    pname = "qowonnotes-webpage-npm";
    version = "1.0.0";
    src = ./.;

    npmDepsHash = "sha256-2dE7SCHZ3VOYV8jW19hukYMqU0dQQ1Rxcv75T4TVacc="; # You'll need to replace this

    buildInputs = [ pkgs.bash ];

    buildPhase = ''
      cp ../screenshots src/.vuepress/public -R || true
      cp ../CHANGELOG.md src/changelog.md || true
      export HOME=$PWD
      bash ./scripts/run-build.sh
    '';

    installPhase = ''
      mkdir -p $out/usr/share/nginx/html
      cp -r ./src/.vuepress/dist $out/usr/share/nginx/html
      mkdir $out/etc
      cp nginx.conf $out/etc/nginx.conf
      sed -i 's/map_hash_bucket_size/# map_hash_bucket_size/' $out/etc/nginx.conf
      sed -i 's|/usr/share/nginx/html|$out/usr/share/nginx/html|g' $out/etc/nginx.conf
      mkdir $out/usr/share/nginx/html/.well-known
      cp org.flathub.VerifiedApps.txt $out/usr/share/nginx/html/.well-known/org.flathub.VerifiedApps.txt
      mkdir -p $out/var/log/nginx
      mkdir -p $out/var/cache/nginx
    '';
  };

in
pkgs.dockerTools.buildLayeredImage {
  name = "qowonnotes-webpage";
  tag = "latest";

  contents = [
    nginx
    npmPackage
  ];

  config = {
    Cmd = [
      "${nginx}/bin/nginx"
      "-c"
      "${npmPackage}/etc/nginx.conf"
      "-g"
      "daemon off;"
    ];
    ExposedPorts = {
      "80/tcp" = { };
    };
  };

  #   extraCommands = ''
  #     mkdir -p etc/nginx var/cache/nginx var/log/nginx
  #     cat > etc/nginx/nginx.conf <<EOF
  #     user nobody nobody;
  #     error_log /dev/stdout info;
  #     pid /dev/null;
  #     events {
  #       worker_connections 1024;
  #     }
  #     http {
  #       access_log /dev/stdout;
  #       server {
  #         listen 80;
  #         root /usr/share/nginx/html;  # Adjust this path if your build output is in a different directory
  #         index index.html;
  #         location / {
  #           try_files \$uri \$uri/ /index.html;
  #         }
  #       }
  #     }
  #     EOF
  #   '';
}
