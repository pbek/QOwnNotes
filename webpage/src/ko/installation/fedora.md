# Fedora Linux에 설치

**Fedora 28 이상**에 대한 QOwnNotes저장소가 있습니다.

## Config-manager dnf 플러그인이 있는 시스템

다음 셸 명령을 루트로 실행하여 저장소를 추가합니다.

```bash
dnf config-manager --add-repo http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/

dnf makecache
dnf install qownnotes
```

::: 팁 repo 키에서 다운로드하기 전에 repo 키를 수락해야 할 수도 있습니다.

문제가 있는 경우 다음을 사용하여 직접 키를 가져옵니다:

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40/repodata/repomd.xml.key
```
Please note that the portion "Fedora_40" in the above code should reflect the version of Fedora you are using (i.e. "Fedora_39", "Fedora_38" etc.) :::

## 레거시 설치 방법

Fedora 버전이 `config-manager` 플러그인을 지원하지 않는 경우 이 방법을 사용하여 이러한 명령을 루트로 실행합니다.

다음 셸 명령을 루트로 실행하여 저장소를 신뢰합니다.

```bash
rpm --import http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40/repodata/repomd.xml.key
```
Again: note that the portion "Fedora_40" in the above code should reflect the version of Fedora you are using (i.e. "Fedora_39", "Fedora_38" etc.)

Then run the following shell commands as root to add the repository and install QOwnNotes from there.

```bash
cat > /etc/yum.repos.d/QOwnNotes.repo << EOL
[qownnotes]
name=OBS repo for QOwnNotes (Fedora \$releasever - \$basearch)
type=rpm-md
baseurl=http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/
gpgcheck=1
gpgkey=http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_\$releasever/repodata/repomd.xml.key
enabled=1
EOL

dnf clean expire-cache
dnf install qownnotes
```

[Direct Download](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Fedora_40) (this example link is for Fedora 40)
