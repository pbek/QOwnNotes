# Instalar en CentOS Linux

Hay paquetes creados por la comunidad para **RHEL 8/9, AlmaLinux 8/9, RockyLinux 8/9, CentOS 8/9 y Oracle 8/9** por _Jörg Woll_ en [Paquetes de QOwnNotes para x86_64 para EL7](http://wilhelm949.spdns.org:10443/w3bservice/7/x86_64/w3bservice/Packages/repoview/qownnotes.html), con CentOS 7 EOL obsoleto, [Paquetes de QOwnNotes para ARM para EL7](http://wilhelm949.spdns.org:10443/w3bservice/7/armhfp/w3bservice/Packages/repoview/qownnotes.html), con CentOS 7 EOL obsoleto, [Paquetes de QOwnNotes para x86_64 para EL8](http://wilhelm949.spdns.org:10443/w3bservice/8/x86_64/w3bservice/Packages/repoview/qownnotes.html). [Paquetes de QOwnNotes para x86_64 para EL9](http://wilhelm949.spdns.org:10443/w3bservice/9/x86_64/w3bservice/Packages/repoview/qownnotes.html).

::: tip
Si QOwnNotes registra el mensaje `Could not write secret to keychain`, instale los paquetes de Secret Service que falten y reinicie su sesión de escritorio.

Para GNOME y otros ordenadores de escritorio basados en Servicio Secreto:

```bash
sudo dnf install gnome-keyring libsecret seahorse
```

Para KDE Plasma:

```bash
sudo dnf install kwalletmanager kf6-kwallet
```

QOwnNotes recurrirá al cifrado heredado si el llavero del escritorio no está disponible.
:::
