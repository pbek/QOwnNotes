# Install on CentOS Linux

There are community-created packages for **RHEL 8/9, AlmaLinux 8/9, RockyLinux 8/9, CentOS 8/9 and Oracle 8/9** by _Jörg Woll_ at
[QOwnNotes packages for x86_64 for el7](http://wilhelm949.spdns.org:10443/w3bservice/7/x86_64/w3bservice/Packages/repoview/qownnotes.html) deprecated CentOS 7 EOL,
[QOwnNotes packages for ARM for el7](http://wilhelm949.spdns.org:10443/w3bservice/7/armhfp/w3bservice/Packages/repoview/qownnotes.html) deprecated CentOS 7 EOL,
[QOwnNotes packages for x86_64 for el8](http://wilhelm949.spdns.org:10443/w3bservice/8/x86_64/w3bservice/Packages/repoview/qownnotes.html).
[QOwnNotes packages for x86_64 for el9](http://wilhelm949.spdns.org:10443/w3bservice/9/x86_64/w3bservice/Packages/repoview/qownnotes.html).

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
