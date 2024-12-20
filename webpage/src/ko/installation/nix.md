# Nix를 통해 설치

QOwnNotes를 [NixOS](https://nixos.org/), 기타 **Linux** 배포판, **macOS** 및 **Windows**에 [Nix 패키지 관리자](https://nixos.org/download/)과 함께 설치할 수 있습니다. 패키지 이름은 [qownnotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes)입니다.

이 패키지는 Qt6로 제작되었으며 fish과 bash를 위한 쉘 통합 기능을 제공합니다.

자세한 내용은 [nix의 QOwnNotes](https://search.nixos.org/packages?channel=unstable&show=qownnotes)를 참조하십시오.

그런 다음 `qownnotes` 또는 `QOwnNotes` 명령으로 QOwnNotes를 실행할 수 있습니다.

::: 팁
QOwnNotes 명령줄 스니펫 관리자 [qc](https://github.com/qownnotes/qc)는 [Nix 스토어](https://search.nixos.org/packages?channel=unstable&show=qc)를 통해 설치할 수도 있습니다

`nix-shell -p qc --run "qc exec"` 을 사용하여 사용해 볼 수 있습니다.
:::

::: 팁 정보
**macOS**에서 패키지는 **x86 및 Apple Silicon**에서 기본적으로 작동합니다!
:::
