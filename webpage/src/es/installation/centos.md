# Instalar en CentOS Linux

Hay paquetes creados por la comunidad para **RHEL 8/9, AlmaLinux 8/9, RockyLinux 8/9, CentOS 8/9 y Oracle 8/9** por _Jörg Woll_ en [Paquetes de QOwnNotes para x86_64 para EL7](http://wilhelm949.spdns.org:10443/w3bservice/7/x86_64/w3bservice/Packages/repoview/qownnotes.html), con CentOS 7 EOL obsoleto, [Paquetes de QOwnNotes para ARM para EL7](http://wilhelm949.spdns.org:10443/w3bservice/7/armhfp/w3bservice/Packages/repoview/qownnotes.html), con CentOS 7 EOL obsoleto, [Paquetes de QOwnNotes para x86_64 para EL8](http://wilhelm949.spdns.org:10443/w3bservice/8/x86_64/w3bservice/Packages/repoview/qownnotes.html). [Paquetes de QOwnNotes para x86_64 para EL9](http://wilhelm949.spdns.org:10443/w3bservice/9/x86_64/w3bservice/Packages/repoview/qownnotes.html).

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages and restart your desktop session.

For GNOME and other Secret Service based desktops:

```bash
sudo dnf install gnome-keyring libsecret seahorse
```

For KDE Plasma:

```bash
sudo dnf install kwalletmanager kf6-kwallet
```

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
