# AppImage로 설치

[QOwnNotes 릴리스 페이지](https://github.com/pbek/QOwnNotes/releases)에서 최신 AppImage를 다운로드할 수 있습니다. 해당 페이지의 이름은 `QOwnNotes-x86_64.AppImage`와 같아야 합니다.

::: tip If you have [jq](https://stedolan.github.io/jq/) installed, you can also download the latest AppImage directly:

```bash
# query the latest Linux release from the QOwnNotes API, parse the JSON for the URL and download it
curl -L https://api.qownnotes.org/latest_releases/linux | jq .url | xargs curl -Lo QOwnNotes-x86_64.AppImage
```

:::

그런 다음 파일에 대한 실행 권한을 변경할 수 있습니다:

```bash
chmod a+x QOwnNotes-*.AppImage
```

::: 팁 AppImages를 실행하려면  `libfuse2`이 필요합니다! 예를 들어, Ubuntu 아래에서 <`sudo apt install libfuse2`로 라이브러리를 설치할 수 있습니다. :::

그런 다음 AppImage를 실행하여 QOwnNotes를 실행할 수 있습니다.

::: 경고 **자동 업데이트** 프로그램을 사용하려면 앱 이미지를 홈 디렉터리와 같이 사용자 계정이 쓰기 액세스할 수 있는 위치에 놓으십시오. :::

::: 팁 glibc 버전이 너무 오래되어 앱이미지를 실행하는 데 문제가 있는 경우 [OBS에서 빌드한 AppImage](https://download.opensuse.org/repositories/home:/pbek:/QOwnNotes/AppImage/QOwnNotes-latest-x86_64.AppImage)를 사용해 볼 수 있으며, 이 앱이미지는 glibc 2.16으로 빌드해야 합니다. :::
