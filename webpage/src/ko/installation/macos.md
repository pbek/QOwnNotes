# macOS™에 설치

[GitHub의 QOwnNotes 릴리스](https://github.com/pbek/QOwnNotes/releases)에서 최신 **macOS 디스크 이미지**를 다운로드한 후 **QOwnNotes** 앱을 **응용프로그램** 폴더로 끕니다.

처음으로 응용 프로그램을 마우스 오른쪽 버튼으로 클릭하고 **열기**을 클릭하여 QOwnNotes를 열어야 합니다.

GitHub 릴리스는 현재 x86 플랫폼용으로만 제작되었습니다.

::: 경고 `개발자를 확인할 수 없기 때문에 QOwnNotes.app을 열 수 없습니다`라는 오류 메시지가 표시되면 MacOS **보안 설정**을 방문하여 `열기`를 클릭해야 합니다.

응용 프로그램을 격리에서 해제하려면 터미널에서 다음 명령을 실행할 수도 있습니다:

```bash
xattr -r -d com.apple.quarantine /Applications/QOwnNotes.app
```

:::

::: 팁 응용 프로그램은 자체 업데이트를 위해 ** 내부 업데이트 프로그램**을 사용합니다. 응용 프로그램 오른쪽 하단에서 최신 버전을 확인할 수 있습니다. :::

## Homebrew

[Homebrew](https://formulae.brew.sh/cask/qownnotes)에는 커뮤니티에서 관리하는 QOwnNotes 통이 있습니다.

다음과 함께 설치할 수 있습니다:

```bash
brew install qownnotes
```

## Nix

또한 **x86 및 Apple Silicon**에서 macOS 아래에 [Nix 패키지 관리자](https://wiki.nixos.org/wiki/Nix_package_manager)을 사용하여 QOwnNotes를 설치할 수도 있습니다 ([Nix를 통해 설치](./nix.md) 참조).

::: 팁
NixOS 불안정 채널을 사용하여 최신 버전의 QOwnNotes를 얻는 것이 가장 좋습니다!
:::
