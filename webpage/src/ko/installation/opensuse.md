# OpenSUSE Linux에 설치

<installation-opensuse/>

<!-- <Content :page-key="getPageKey($site.pages, '/installation/ubuntu.md')" /> -->

## 모든 openSUSE에서

[OBS패키지 설치](https://github.com/openSUSE/opi) 도구를 사용하여 QOnNotes를 설치할 수 있습니다.

다음 셸 명령을 루트로 실행하여 `opi`을 설치합니다:

```bash
zypper install opi
```

그런 다음 QOwnNotes를 설치합니다:

```bash
opi qownnotes
```

::: 경고 이 도구는 전체 OBS 서비스를 쿼리하므로 `qownnotes-lang`이 아닌 `qownnotes`를 선택해야 합니다.

또한 선택한 repo가 타사 홈이 아닌 공식 `home:pbek:QOwnNotes`인지 확인합니다. :::

::: 팁
업데이트를 가져오려면 설치 후 리포지토리를 유지하는 옵션을 선택해야 합니다.
:::

## openSUSE Leap 16.0

다음 셸 명령을 실행하여 저장소를 추가하고 여기서 QOwnNotes를 설치합니다.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/16.0/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/16.0)

## openSUSE Leap 15.6

다음 셸 명령을 루트로 실행하여 저장소를 추가하고 여기서QOwnNotes를 설치합니다.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.6)

## openSUSE Leap 15.5

다음 셸 명령을 루트로 실행하여 저장소를 추가하고 여기서QOwnNotes를 설치합니다.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.5)

## openSUSE Leap 15.4

다음 셸 명령을 루트로 실행하여 저장소를 추가하고 여기서QOwnNotes를 설치합니다.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/15.4)

## openSUSE Leap 15.3

다음 셸 명령을 루트로 실행하여 저장소를 추가하고 여기서QOwnNotes를 설치합니다.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.3)

## openSUSE Leap 15.2

다음 셸 명령을 루트로 실행하여 저장소를 추가하고 여기서QOwnNotes를 설치합니다.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Leap_15.2)

## openSUSE Tumbleweed

다음 셸 명령을 루트로 실행하여 저장소를 추가하고 여기서QOwnNotes를 설치합니다.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/openSUSE_Tumbleweed)

## SLE 15

루트로 다음 셸 명령을 실행하여 저장소를 추가하고 해당 위치에서 QOwnNotes를 설치합니다.

```bash
zypper addrepo -f http://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15/home:pbek:QOwnNotes.repo
zypper refresh
zypper install qownnotes
```

[직접 다운로드](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/SLE_15)
