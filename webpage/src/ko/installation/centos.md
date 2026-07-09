# CentOS Linux에 설치

커뮤니티에서 만든 **RHEL 8/9, AlmaLinux 8/9, RockyLinux 8/9, CentOS 8/9 및 Oracle 8/9용 패키지**는 _Jörg Woll_의 다음 링크에서 확인할 수 있습니다. [el7용 x86_64용 QOwnNotes 패키지](http://wilhelm949.spdns.org:10443/w3bservice/7/x86_64/w3bservice/Packages/repoview/qownnotes.html)는 CentOS 7 EOL에서 지원 중단되었습니다, [el7용 ARM용 QOwnNotes 패키지](http://wilhelm949.spdns.org:10443/w3bservice/7/armhfp/w3bservice/Packages/repoview/qownnotes.html)는 CentOS 7 EOL을 지원 되었습니다, [el8용 x86_64용 QOwnNotes 패키지](http://wilhelm949.spdns.org:10443/w3bservice/8/x86_64/w3bservice/Packages/repoview/qownnotes.html)입니다. [el9용 x86_64용 QOwnNotes 패키지입니다](http://wilhelm949.spdns.org:10443/w3bservice/9/x86_64/w3bservice/Packages/repoview/qownnotes.html).

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
