# AppImage로 설치

[QOwnNotes 릴리스 페이지](https://github.com/pbek/QOwnNotes/releases)에서 최신 AppImage를 다운로드할 수 있습니다. 해당 페이지의 이름은 `QOwnNotes-x86_64.AppImage`와 같아야 합니다.

::: tip
[jq](https://stedolan.github.io/jq/)가 설치되어 있다면 최신 AppImage를 직접 다운로드할 수도 있습니다:

```bash
# query the latest Linux release from the QOwnNotes API, parse the JSON for the URL and download it
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```

:::

그런 다음 파일에 대한 실행 권한을 변경할 수 있습니다:

```bash
chmod a+x QOwnNotes-*.AppImage
```

::: tip
AppImages를 실행하려면  `libfuse2`이 필요합니다! 예를 들어, Ubuntu 아래에서 <`sudo apt install libfuse2`로 라이브러리를 설치할 수 있습니다.
:::

그런 다음 AppImage를 실행하여 QOwnNotes를 실행할 수 있습니다.

::: warning
**자동 업데이트** 프로그램을 사용하려면 앱 이미지를 홈 디렉터리와 같이 사용자 계정이 쓰기 액세스할 수 있는 위치에 놓으십시오.
:::

::: tip
glibc 버전이 너무 오래되어 앱이미지를 실행하는 데 문제가 있는 경우 [OBS에서 빌드한 AppImage](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage)를 사용해 볼 수 있으며, 이 앱이미지는 glibc 2.16으로 빌드해야 합니다.
:::

## 포터블 모드

`--portable` 인수를 전달하여 AppImage를 **포터블 모드**에서 실행할 수 있습니다:

```bash
./QOwnNotes-x86_64.AppImage --portable
```

포터블 모드에서는 모든 응용 프로그램 데이터가 홈 디렉터리가 아닌 AppImage 파일 옆의 `Data` 폴더에 저장됩니다. 여기에는 다음이 포함됩니다:

- 설정 (`Data/QOwnNotes.ini`)
- 노트 데이터베이스 (`Data/QOwnNotes.sqlite`)
- 당신의 노트 (`Data/Notes/`)
- 스크립트, 철자 검사 사전 및 기타 응용 프로그램 데이터

이렇게 하면 USB 드라이브나 기타 휴대용 저장 장치에 모든 노트와 함께 QOwnNotes를 쉽게 휴대할 수 있습니다.

::: tip
AppImage 파일이 포함된 디렉터리가 쓰기 가능한지 확인하여, 그 옆에 `Data` 폴더를 만들 수 있도록 합니다.
:::

::: tip
If QOwnNotes logs `Could not write secret to keychain`, install the missing Secret Service packages on your Linux distribution and restart your desktop session.

For GNOME and other Secret Service based desktops, install `gnome-keyring`, `libsecret` and `seahorse`.

For KDE Plasma, install KWallet support such as `kwalletmanager`.

QOwnNotes will fall back to legacy encryption if the desktop keychain is unavailable.
:::
