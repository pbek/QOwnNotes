# Arch Linux에 설치

## Arch 사용자 리포지토리 (AUR)

또는 AUR에 QOwnNotes용 공식 패키지가 있으며, 이를 `qownotes`이라고 합니다.

여기서 찾을 수 있습니다: [AUR에서 QownNotes](https://aur.archlinux.org/packages/qownnotes)

패키지 데이터베이스를 동기화하고 `yay`으로 패키지를 설치합니다:

```bash
yay -S qownnotes
```

::: 팁 빌드 시간을 단축하고 싶다면 [CCACHE 및 AUR](https://www.reddit.com/r/archlinux/comments/6vez44/a_small_tip_if_you_compile_from_aur/)을 읽는 것이 좋습니다. :::

## pacman

`sudo nano /etc/pacman.conf`를 사용하여`/etc/pacman.conf`에 다음 행을 추가합니다:

```ini
[home_pbek_QOwnNotes_Arch_Extra]
SigLevel = Optional TrustAll
Server = http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/$arch
```

다음 셸 명령을 실행하여 리포지토리를 신뢰합니다:

```bash
wget http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra/x86_64/home_pbek_QOwnNotes_Arch_Extra.key -O - | sudo pacman-key --add -
sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB
```

If the command `sudo pacman-key --lsign-key F2205FB121DF142B31450865A3BA514562A835DB` fails with a message like: `ERROR: FFC43FC94539B8B0 could not be locally signed.`, you could first find out the actual _keyid_ of the downloaded key, i.e. with the command (and output):

```bash
gpg /path/to/downloaded/home_pbek_QOwnNotes_Arch_Extra.key
gpg: 경고: 명령이 제공되지 않았습니다.  무슨 말인지 알아맞히려고...
pub   rsa2048 2019-07-31 [SC] [expires: 2021-10-10]
      F2205FB121DF142B31450865A3BA514562A835DB
uid           home:pbek OBS Project <home:pbek@build.opensuse.org>
```

이제 패키지 데이터베이스를 동기화하고 `pacman`을 사용하여 패키지를 설치할 수 있습니다.

```bash
sudo pacman -Syy qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/Arch_Extra)

::: 팁
물론 Manjaro와 같은 다른 Arch Linux 기반 배포판에서도 이 저장소를 사용할 수 있습니다.
:::
